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

typedef enum de_mixer_status_t {
	DE_MIXER_STATUS_ACTIVE,
	DE_MIXER_STATUS_NEED_STOP,
	DE_MIXER_STATUS_STOPPED
} de_mixer_status_t;

struct de_sound_device_t {
	de_sound_context_t* ctx;
	de_mtx_t mtx;
	de_cnd_t cnd;
	de_mixer_status_t mixer_status;
	short* out_buffer;
	size_t buffer_len;
#ifdef _WIN32
	/* dsound */
	IDirectSound8* dsound;
	IDirectSoundBuffer8* buffer;
	IDirectSoundNotify* notify;
	HANDLE points[2];
#else
	/* alsa */
	snd_pcm_t* playbackDevice;
	int frameCount;
#endif
};

/**
 * @brief Initializes sound device.
 */
bool de_sound_device_init(de_sound_context_t* ctx, de_sound_device_t* dev);

/**
 * @brief Destroys sound device.
 */
void de_sound_device_free(de_sound_device_t* dev);