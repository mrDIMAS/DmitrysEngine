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

void de_sound_device_send_data(de_sound_device_t* dev) {
	int err;
	if ((err = snd_pcm_writei(dev->playbackDevice, dev->out_buffer, dev->frameCount)) < 0) {
		if (err == -EPIPE) {
			de_log("ALSA Error: Buffer underrun!");
		}
		if ((err = snd_pcm_prepare(dev->playbackDevice)) < 0) {
			de_log("ALSA Error unable to snd_pcm_prepare: %s", snd_strerror(err));
		} else {
			if ((err = snd_pcm_writei(dev->playbackDevice, dev->out_buffer, dev->frameCount)) < 0) {
				de_log("ALSA Error unable to snd_pcm_writei: %s", snd_strerror(err));
			}
		}
	}
}

bool de_sound_device_setup(de_sound_device_t* dev) {
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
    snd_pcm_access_t access;
    unsigned int exactRate;
    long unsigned int exactSize;
	int err;

	dev->frameCount = dev->buffer_len / 4; /* 16-bit stereo is 4 bytes, so frame count is bufferHalfSize / 4 */

	if ((err = snd_pcm_open(&dev->playbackDevice, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		de_log("ALSA Error unable to snd_pcm_open: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_malloc: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_any(dev->playbackDevice, hw_params)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_any: %s", snd_strerror(err));
	}

	access = SND_PCM_ACCESS_RW_INTERLEAVED;
	if ((err = snd_pcm_hw_params_set_access(dev->playbackDevice, hw_params, access)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_access: %s", snd_strerror(err));
	}

	if ((err = snd_pcm_hw_params_set_format(dev->playbackDevice, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_format: %s", snd_strerror(err));
	}

	exactRate = DE_SOUND_DEVICE_SAMPLE_RATE;
	if ((err = snd_pcm_hw_params_set_rate_near(dev->playbackDevice, hw_params, &exactRate, 0)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_rate_near: %s", snd_strerror(err));
	}
	if (exactRate != DE_SOUND_DEVICE_SAMPLE_RATE) {
		de_log("Warning! %d playback rate is not supported, using %d instead.", DE_SOUND_DEVICE_SAMPLE_RATE, exactRate);
	}

	if ((err = snd_pcm_hw_params_set_channels(dev->playbackDevice, hw_params, 2)) < 0) {
		de_log("ALSA Error unable to snd_pcm_hw_params_set_channels: %s", snd_strerror(err));
	}

	/* dev->frameCount * 2 because user defines size of buffer's half, but for removing 'clicks' we need to use full sized buffer */
	exactSize = dev->frameCount * 2;
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

	return true;
}

void de_sound_device_shutdown(de_sound_device_t* dev) {
	snd_pcm_close(dev->playbackDevice);
}