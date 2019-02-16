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

typedef enum de_sound_format_t {
	DE_SOUND_FORMAT_MONO,
	DE_SOUND_FORMAT_STEREO
} de_sound_format_t;

typedef enum de_sound_buffer_flags_t {
	DE_SOUND_BUFFER_FLAGS_STREAM = DE_BIT(0),
} de_sound_buffer_flags_t;

struct de_sound_buffer_t {
	de_sound_context_t* ctx;
	de_sound_format_t format;
	/* Data is NON-INTERLEAVED, which for left (L) and right (R) channels means:
	 * LLLLLLLLLLLLLL RRRRRRRRRRRRRR 
	 * So basically data split into chunks for each channel.
	 */
	float* data;
	size_t sample_per_channel;
	size_t channel_count;
	uint32_t flags;
	de_sound_decoder_t* decoder;
};

de_sound_buffer_t* de_sound_buffer_create(de_sound_context_t* ctx, uint32_t flags);

void de_sound_buffer_load_file(de_sound_buffer_t* buf, const char* file);

void de_sound_buffer_free(de_sound_buffer_t* buf);