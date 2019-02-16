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

de_sound_buffer_t* de_sound_buffer_create(de_sound_context_t* ctx, uint32_t flags) {
	de_sound_buffer_t* buf = DE_NEW(de_sound_buffer_t);
	buf->ctx = ctx;
	buf->flags = flags;
	return buf;
}

void de_sound_buffer_load_file(de_sound_buffer_t* buf, const char* file) {
	buf->decoder = de_sound_decoder_init(file);
	buf->channel_count = buf->decoder->channel_count;
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		/* 1 second streaming buffer */
		buf->sample_per_channel = DE_SOUND_DEVICE_SAMPLE_RATE;
	} else {
		/* Full-length buffer */
		buf->sample_per_channel = buf->decoder->sample_per_channel;
	}
	buf->data = de_calloc(buf->sample_per_channel * buf->channel_count, sizeof(float));
	de_sound_decoder_get_next_block(buf->decoder, buf->data, buf->sample_per_channel);
	/* decoder is needed only for streaming */
	if (!(buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM)) {
		de_sound_decoder_free(buf->decoder);
	}
}

void de_sound_buffer_free(de_sound_buffer_t* buf) {
	de_free(buf);
}