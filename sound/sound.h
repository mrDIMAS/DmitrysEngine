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

de_sound_source_t* de_sound_source_create(de_sound_device_t* dev);

void de_sound_source_free(de_sound_source_t* src);

void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf);
