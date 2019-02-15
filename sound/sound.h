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

#define SW_OUTPUT_DEVICE_SAMPLE_RATE 44100
#define DE_OUTPUT_DEVICE_HALF_BUFFER_SIZE (SW_OUTPUT_DEVICE_SAMPLE_RATE / 4) 
#define DE_SOUND_MAX_CHANNELS 2 

typedef enum de_sound_format_t {
	DE_SOUND_FORMAT_MONO,
	DE_SOUND_FORMAT_STEREO
} de_sound_format_t;

struct de_sound_buffer_t {
	de_sound_format_t format;
	float* data;
	size_t size;
};

struct de_sound_source_t {
	de_sound_device_t* device;
	de_sound_buffer_t* buffer;
	double playback_position;
	float pan;
	float pitch;
	float gain;
	/* gain of each channel. currently mono and stereo are supported */
	float channel_gain[DE_SOUND_MAX_CHANNELS];
	/* spatial data */
	de_vec3_t position;
	DE_LINKED_LIST_ITEM(de_sound_source_t);
};

typedef enum de_mixer_status_t {
	DE_MIXER_STATUS_ACTIVE,
	DE_MIXER_STATUS_NEED_STOP,
	DE_MIXER_STATUS_STOPPED
} de_mixer_status_t;

struct de_sound_device_t {
	de_core_t* core;
	de_mtx_t mtx;
	de_cnd_t cnd;
	de_mixer_status_t mixer_status;
	short* out_buffer;
	DE_LINKED_LIST_DECLARE(de_sound_source_t, sounds);
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

bool de_sound_device_init(de_core_t* core, de_sound_device_t* dev);

void de_sound_device_free(de_sound_device_t* dev);

de_sound_source_t* de_sound_source_create(de_sound_device_t* dev) {
	de_sound_source_t* src = DE_NEW(de_sound_source_t);
	src->device = dev;
	src->pitch = 1.0f;
	src->playback_position = 0.0;
	src->pan = 0;
	DE_LINKED_LIST_APPEND(dev->sounds, src);
	return src;
}

void de_sound_source_free(de_sound_source_t* src) {
	DE_LINKED_LIST_REMOVE(src->device->sounds, src);
}

void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf) {
	src->buffer = buf;
}

void de_sound_device_update(de_sound_device_t* dev) {
	de_sound_source_t* src;
	DE_LINKED_LIST_FOR_EACH(dev->sounds, src) {

	}
}