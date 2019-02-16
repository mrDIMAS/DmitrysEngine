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

de_sound_source_t* de_sound_source_create(de_sound_context_t* ctx) {
	de_sound_source_t* src = DE_NEW(de_sound_source_t);
	src->ctx = ctx;
	src->pitch = 1.0f;
	src->playback_position = 0.0;
	src->current_sample_rate = 1.0f;
	src->pan = 0;
	DE_LINKED_LIST_APPEND(ctx->sounds, src);
	return src;
}

void de_sound_source_free(de_sound_source_t* src) {
	DE_LINKED_LIST_REMOVE(src->ctx->sounds, src);
}

void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf) {
	src->buffer = buf;
}

void de_sound_source_sample(de_sound_source_t* src, float samples[2]) {	
	size_t i;
	src->playback_position += src->current_sample_rate;
	if (src->buffer) {
		i = (size_t)src->playback_position;
		if (i >= src->buffer->sample_per_channel) {
			i = 0;
			src->playback_position = 0;
		}
		if (src->buffer->channel_count == 2) {
			samples[0] = src->buffer->data[i];
			samples[1] = src->buffer->data[i + src->buffer->sample_per_channel];
		} else if (src->buffer->channel_count == 1) {
			samples[0] = samples[1] = src->buffer->data[i];
		}
	} else {
		/* silence */
		samples[0] = 0.0f;		
		samples[1] = 0.0f;		
	}
}