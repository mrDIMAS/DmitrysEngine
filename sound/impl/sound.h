/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/**
 * @brief Sends data to output device.
 * 
 * Note: This function is blocking. 
 */
static void de_sound_device_send_data(de_sound_device_t* dev, short* data) {
#ifdef _WIN32
	void * outputData;
	int size;
	int result = WaitForMultipleObjects(2, dev->points, 0, INFINITE);
	if (result == WAIT_OBJECT_0) {
		IDirectSoundBuffer8_Lock(dev->buffer, DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE, DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE, &outputData, (LPDWORD)&size, 0, 0, 0);
		memcpy(outputData, data, size);
		IDirectSoundBuffer8_Unlock(dev->buffer, outputData, size, 0, 0);
	}
	if (result == (WAIT_OBJECT_0 + 1)) {
		IDirectSoundBuffer8_Lock(dev->buffer, 0, DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE, &outputData, (LPDWORD)&size, 0, 0, 0);
		memcpy(outputData, data, size);
		IDirectSoundBuffer8_Unlock(dev->buffer, outputData, size, 0, 0);
	}
#else
	int err;
	if ((err = snd_pcm_writei(dev->playbackDevice, data, dev->frameCount)) < 0) {
		if (err == -EPIPE) {
			de_log("ALSA Error: Buffer underrun!");
		}
		if ((err = snd_pcm_prepare(dev->playbackDevice)) < 0) {
			de_log("ALSA Error unable to snd_pcm_prepare: %s", snd_strerror(err));
		} else {
			if ((err = snd_pcm_writei(dev->playbackDevice, data, dev->frameCount)) < 0) {
				de_log("ALSA Error unable to snd_pcm_writei: %s", snd_strerror(err));
			}
		}
	}
#endif
}

static int de_sound_device_thread(void* ptr) {
	de_sound_device_t* dev = (de_sound_device_t*)ptr;
	de_log("Sound thread started!");
	while (dev->mixer_status == DE_MIXER_STATUS_ACTIVE) {
		de_mtx_lock(&dev->mtx);
		/* mix */
		de_mtx_unlock(&dev->mtx);

		/* send_data is locking so mutex is already unlocked here */
		de_sound_device_send_data(dev, dev->out_buffer);		
	}
	dev->mixer_status = DE_MIXER_STATUS_STOPPED;
	de_cnd_signal(&dev->cnd);
	de_log("Sound thread stopped!");
	return 0;
}

bool de_sound_device_init(de_core_t* core, de_sound_device_t* dev) {
	de_thrd_t mixer_thread;
#ifdef _WIN32
	/* dsound */
	WAVEFORMATEX bufferFormat = { 0 };
	DSBUFFERDESC bufferDesc = { 0 };
	DSBPOSITIONNOTIFY pPosNotify[2];
	IDirectSoundBuffer * pDSB;
#else
	/* alsa */
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	int err;
#endif

	de_zero(dev, sizeof(*dev));
	dev->core = core;
	dev->out_buffer = de_calloc(DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE, sizeof(*dev->out_buffer));
	dev->mixer_status = DE_MIXER_STATUS_ACTIVE;
	de_mtx_init(&dev->mtx);
	de_cnd_init(&dev->cnd);

	/* platform specific code */
#ifdef _WIN32
	if (FAILED(DirectSoundCreate8(0, &dev->dsound, 0))) {
		de_log("Failed to create DirectSound8 device");
		return false;
	}

	if (FAILED(IDirectSound8_SetCooperativeLevel(dev->dsound, GetForegroundWindow(), DSSCL_PRIORITY))) {
		de_log("Failed to set DirectSound8 coop level");
		IDirectSound8_Release(dev->dsound);
		return false;
	}

	bufferFormat.cbSize = sizeof(WAVEFORMATEX);
	bufferFormat.wFormatTag = WAVE_FORMAT_PCM;
	bufferFormat.nChannels = 2;
	bufferFormat.nSamplesPerSec = SW_OUTPUT_DEVICE_SAMPLE_RATE;
	bufferFormat.wBitsPerSample = 16;
	bufferFormat.nBlockAlign = (bufferFormat.wBitsPerSample / 8) * bufferFormat.nChannels;
	bufferFormat.nAvgBytesPerSec = bufferFormat.nSamplesPerSec * bufferFormat.nBlockAlign;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE * 2;
	bufferDesc.lpwfxFormat = &bufferFormat;

	if (FAILED(IDirectSound8_CreateSoundBuffer(dev->dsound, &bufferDesc, &pDSB, NULL))) {
		de_log("Failed to create DirectSoundBuffer8!");
		IDirectSound8_Release(dev->dsound);
		return false;
	}

	IDirectSoundBuffer_QueryInterface(pDSB, &IID_IDirectSoundBuffer8, (void**)&dev->buffer);
	IDirectSoundBuffer_Release(pDSB);

	IDirectSoundBuffer8_QueryInterface(dev->buffer, &IID_IDirectSoundNotify8, (void**)&dev->notify);

	dev->points[0] = CreateEvent(0, 0, 0, 0);
	dev->points[1] = CreateEvent(0, 0, 0, 0);

	pPosNotify[0].dwOffset = 0;
	pPosNotify[0].hEventNotify = dev->points[0];

	pPosNotify[1].dwOffset = DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE;
	pPosNotify[1].hEventNotify = dev->points[1];

	IDirectSoundNotify_SetNotificationPositions(dev->notify, 2, pPosNotify);
	IDirectSoundBuffer8_Play(dev->buffer, 0, 0, DSBPLAY_LOOPING);
	de_log("DirectSound8 device successfully initialized!");
#else /* !_WIN32 */

	dev->frameCount = bufferHalfSize / 4; /* 16-bit stereo is 4 bytes, so frame count is bufferHalfSize / 4 */

	if ((err = snd_pcm_open(&dev->playbackDevice, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		de_log("ALSA Error unable to snd_pcm_open: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_malloc: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_any(dev->playbackDevice, hw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_any: %s", snd_strerror(err));
	}

	snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED;
	if ((err = snd_pcm_hw_params_set_access(dev->playbackDevice, hw_params, access)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_access: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_set_format(dev->playbackDevice, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_format: %s", snd_strerror(err));
	}

	unsigned int exactRate = SW_OUTPUT_DEVICE_SAMPLE_RATE;
	if ((err = snd_pcm_hw_params_set_rate_near(dev->playbackDevice, hw_params, &exactRate, 0)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_rate_near: %s", snd_strerror(err));
	}
	if (exactRate != SW_OUTPUT_DEVICE_SAMPLE_RATE) {
		de_log("Warning! %d playback rate is not supported, using %d instead.", SW_OUTPUT_DEVICE_SAMPLE_RATE, exactRate);
	}

	if ((err = snd_pcm_hw_params_set_channels(dev->playbackDevice, hw_params, 2)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_channels: %s", snd_strerror(err));
	}

	/* dev->frameCount * 2 because user defines size of buffer's half, but for removing 'clicks' we need to use full sized buffer */
	long unsigned int exactSize = dev->frameCount * 2;
	if (snd_pcm_hw_params_set_buffer_size_near(dev->playbackDevice, hw_params, &exactSize) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_buffer_size_near: %s", snd_strerror(err));
	}
	if (exactSize != dev->frameCount * 2) {
		de_log("Warning! %d buffer size is not supported, using %d instead.", dev->frameCount, exactSize);
		dev->frameCount = exactSize;
	}

	if ((err = snd_pcm_hw_params(dev->playbackDevice, hw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params: %s", snd_strerror(err));
	}

	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_sw_params_malloc: %s", snd_strerror(err));
	}
	if ((err = snd_pcm_sw_params_current(dev->playbackDevice, sw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_sw_params_current: %s", snd_strerror(err));
	}
	if ((err = snd_pcm_sw_params_set_avail_min(dev->playbackDevice, sw_params, dev->frameCount)) < 0) {
		de_log("ALSA Error unable to snd_pcm_sw_params_set_avail_min: %s", snd_strerror(err));
	}
	if ((err = snd_pcm_sw_params_set_start_threshold(dev->playbackDevice, sw_params, 0U)) < 0) {
		de_log("ALSA Error unable to snd_pcm_sw_params_set_start_threshold: %s", snd_strerror(err));
	}
	if ((err = snd_pcm_sw_params(dev->playbackDevice, sw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_sw_params: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_prepare(dev->playbackDevice)) < 0) {
		de_log("ALSA Error unable to snd_pcm_prepare: %s", snd_strerror(err));
	}

	de_log("ALSA device successfully initialized!");
#endif

	de_thrd_create(&mixer_thread, de_sound_device_thread, dev);
	de_thrd_detach(&mixer_thread);

	return true;
}

void de_sound_device_free(de_sound_device_t* dev) {	
	dev->mixer_status = DE_MIXER_STATUS_NEED_STOP;
	while (dev->mixer_status != DE_MIXER_STATUS_STOPPED) {
		de_cnd_wait(&dev->cnd, &dev->mtx);
	}
	de_free(dev->out_buffer);	
#ifdef _WIN32
	IDirectSoundNotify_Release(dev->notify);
	IDirectSoundBuffer8_Release(dev->buffer);
	IDirectSound8_Release(dev->dsound);
#else
	snd_pcm_close(dev->playbackDevice);
#endif
	de_mtx_unlock(&dev->mtx);
	de_mtx_destroy(&dev->mtx);
	de_cnd_destroy(&dev->cnd);
}