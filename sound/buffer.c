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
	de_sound_buffer_t* buf;
	de_sound_context_lock(ctx);
	buf = DE_NEW(de_sound_buffer_t);
	buf->ctx = ctx;
	buf->flags = flags;
	DE_ARRAY_APPEND(ctx->buffers, buf);
	de_sound_context_unlock(ctx);
	return buf;
}

void de_sound_buffer_load_file(de_sound_buffer_t* buf, const char* file) {
	size_t buffer_sample_count, block_sample_count;
	de_sound_context_lock(buf->ctx);
	buf->decoder = de_sound_decoder_init(file);
	buf->channel_count = buf->decoder->channel_count;
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		/* 1 second streaming buffer */
		buf->sample_per_channel = 2 * buf->ctx->dev.out_samples_count;
	} else {
		/* Full-length buffer */
		buf->sample_per_channel = buf->decoder->sample_per_channel;
	}
	block_sample_count = buf->sample_per_channel * buf->channel_count;	
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		buffer_sample_count = 2 * block_sample_count;
	} else {
		buffer_sample_count = block_sample_count;
	}
	buf->data = de_calloc(buffer_sample_count, sizeof(float));
	/* upload data */
	buf->total_sample_per_channel = buf->decoder->sample_per_channel;
	buf->read_ptr = buf->data;
	buf->stream_write_ptr = buf->data + block_sample_count;
	de_sound_decoder_read(buf->decoder, buf->data, buf->sample_per_channel, 0, buf->sample_per_channel);
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		/* upload second block for streaming */
		de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, 0, buf->sample_per_channel);
	}
	/* decoder is needed only for streaming */
	if (!(buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM)) {
		de_sound_decoder_free(buf->decoder);
		buf->decoder = NULL;
	}
	de_sound_context_unlock(buf->ctx);
}

void de_sound_buffer_free(de_sound_buffer_t* buf) {
	de_sound_context_lock(buf->ctx);
	if (buf->decoder) {
		de_sound_decoder_free(buf->decoder);
	}
	DE_ARRAY_REMOVE(buf->ctx->buffers, buf);
	de_free(buf);
	de_sound_context_unlock(buf->ctx);
}

void de_sound_buffer_update(de_sound_buffer_t* buf) {
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK) {
		size_t readed, next_readed;
		readed = de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, 0, buf->sample_per_channel);
		if (readed < buf->sample_per_channel) {
			de_sound_decoder_rewind(buf->decoder);
			next_readed = de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, readed, buf->sample_per_channel - readed);
			DE_ASSERT(next_readed + readed == buf->sample_per_channel);			
		}
		de_sound_buffer_reset_flags(buf, DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK);
	}
}

void de_sound_buffer_swap_pointers(de_sound_buffer_t* buf) {
	float* temp = buf->read_ptr;
	buf->read_ptr = buf->stream_write_ptr;
	buf->stream_write_ptr = temp;
}

void de_sound_buffer_reset_pointers(de_sound_buffer_t* buf) {
	buf->read_ptr = buf->data;
	buf->stream_write_ptr = buf->data + buf->sample_per_channel * buf->channel_count;
}

void de_sound_buffer_set_flags(de_sound_buffer_t* buf, uint32_t flags) {
	buf->flags |= flags;
}

void de_sound_buffer_reset_flags(de_sound_buffer_t* buf, uint32_t flags) {
	buf->flags &= ~flags;
}

void de_sound_buffer_rewind(de_sound_buffer_t* buf) {
	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		size_t block_sample_count;
		de_sound_decoder_rewind(buf->decoder);
		block_sample_count = buf->sample_per_channel * buf->channel_count;
		/* reset pointers */
		buf->read_ptr = buf->data;
		buf->stream_write_ptr = buf->data + block_sample_count;
		/* load blocks */
		de_sound_decoder_read(buf->decoder, buf->data, buf->sample_per_channel, 0, buf->sample_per_channel);
		de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, 0, buf->sample_per_channel);
	}
}