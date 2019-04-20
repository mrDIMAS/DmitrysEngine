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

void de_sound_device_send_data(de_sound_device_t* dev)
{
	void * outputData;
	int size;
	int result = WaitForMultipleObjects(2, dev->points, 0, INFINITE);
	if (result == WAIT_OBJECT_0) {
		IDirectSoundBuffer8_Lock(dev->buffer, dev->buffer_len_bytes, dev->buffer_len_bytes, &outputData, (LPDWORD)&size, 0, 0, 0);
		memcpy(outputData, dev->out_buffer, size);
		IDirectSoundBuffer8_Unlock(dev->buffer, outputData, size, 0, 0);
	} if (result == (WAIT_OBJECT_0 + 1)) {
		IDirectSoundBuffer8_Lock(dev->buffer, 0, dev->buffer_len_bytes, &outputData, (LPDWORD)&size, 0, 0, 0);
		memcpy(outputData, dev->out_buffer, size);
		IDirectSoundBuffer8_Unlock(dev->buffer, outputData, size, 0, 0);
	}
}

bool de_sound_device_setup(de_sound_device_t* dev)
{
	WAVEFORMATEX bufferFormat = { 0 };
	DSBUFFERDESC bufferDesc = { 0 };
	DSBPOSITIONNOTIFY pPosNotify[2];
	IDirectSoundBuffer * pDSB;

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
	bufferFormat.nSamplesPerSec = dev->sample_rate;
	bufferFormat.wBitsPerSample = 8 * sizeof(int16_t);
	bufferFormat.nBlockAlign = (bufferFormat.wBitsPerSample / 8) * bufferFormat.nChannels;
	bufferFormat.nAvgBytesPerSec = bufferFormat.nSamplesPerSec * bufferFormat.nBlockAlign;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = 2 * dev->buffer_len_bytes; /* two sizes of mixing buffer */
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

	/* notify at begin */
	pPosNotify[0].dwOffset = 0;
	pPosNotify[0].hEventNotify = dev->points[0];

	/* notify on center */
	pPosNotify[1].dwOffset = bufferDesc.dwBufferBytes / 2;
	pPosNotify[1].hEventNotify = dev->points[1];

	IDirectSoundNotify_SetNotificationPositions(dev->notify, 2, pPosNotify);
	IDirectSoundBuffer8_Play(dev->buffer, 0, 0, DSBPLAY_LOOPING);
	de_log("DirectSound8 device successfully initialized!");

	return true;
}

void de_sound_device_shutdown(de_sound_device_t* dev)
{
	IDirectSoundNotify_Release(dev->notify);
	IDirectSoundBuffer8_Release(dev->buffer);
	IDirectSound8_Release(dev->dsound);
}