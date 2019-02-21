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

de_sound_source_t* de_sound_source_create(de_sound_context_t* ctx, de_sound_source_type_t type) {
	size_t i;
	de_sound_source_t* src;
	de_sound_context_lock(ctx);
	src = DE_NEW(de_sound_source_t);
	src->type = type;
	src->ctx = ctx;
	src->pitch = 1.0f;
	src->buf_read_pos = 0.0;
	src->current_sample_rate = 1.0f;
	src->pan = 0;
	src->loop = true;
	for (i = 0; i < DE_SOUND_MAX_CHANNELS; ++i) {
		src->channel_gain[i] = 1.0f;
	}
	DE_ARRAY_APPEND(ctx->sounds, src);
	de_sound_context_unlock(ctx);
	return src;
}

void de_sound_source_free(de_sound_source_t* src) {
	de_sound_context_lock(src->ctx);
	DE_ARRAY_REMOVE(src->ctx->sounds, src);
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf) {
	de_sound_context_lock(src->ctx);
	src->buffer = buf;
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_sample(de_sound_source_t* src, float samples[2]) {
	uint64_t i;

	src->buf_read_pos += src->current_sample_rate;
	src->playback_pos += src->current_sample_rate;

	if (src->playback_pos >= src->buffer->total_sample_per_channel) {
		src->playback_pos = 0;
		src->status = src->loop ? DE_SOUND_SOURCE_STATUS_PLAYING : DE_SOUND_SOURCE_STATUS_STOPPED;
	}

	i = (uint64_t)src->buf_read_pos;

	if (i >= src->buffer->sample_per_channel) {
		if (src->buffer->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
			de_sound_buffer_swap_pointers(src->buffer);
			de_sound_buffer_set_flags(src->buffer, DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK);
		}
		src->buf_read_pos = 0;
		i = 0;
	}

	if (src->buffer->channel_count == 2) {
		samples[DE_SOUND_CHANNEL_LEFT] = src->channel_gain[DE_SOUND_CHANNEL_LEFT] * src->buffer->read_ptr[i];
		samples[DE_SOUND_CHANNEL_RIGHT] = src->channel_gain[DE_SOUND_CHANNEL_RIGHT] * src->buffer->read_ptr[i + src->buffer->sample_per_channel];
	} else if (src->buffer->channel_count == 1) {
		float sample = src->buffer->read_ptr[i];
		samples[DE_SOUND_CHANNEL_LEFT] = src->channel_gain[DE_SOUND_CHANNEL_LEFT] * sample;
		samples[DE_SOUND_CHANNEL_RIGHT] = src->channel_gain[DE_SOUND_CHANNEL_RIGHT] * sample;
	}
}

bool de_sound_source_can_produce_samples(de_sound_source_t* src) {
	size_t i;
	bool gain_significant;

	if (!(src->status == DE_SOUND_SOURCE_STATUS_PLAYING) || !src->buffer) {
		return false;
	}
	
	/* Ignore almost inaudible sounds. This will significantly reduce number of sound sources 
	 * participating in mixing for distant or muted sounds. */
	gain_significant = false;		
	for (i = 0; i < DE_SOUND_MAX_CHANNELS; ++i) {
		if (src->channel_gain[i] > 0.0005f) {
			gain_significant = true;
			break;
		}
	}

	return gain_significant;
}

void de_sound_source_play(de_sound_source_t* src) {
	de_sound_context_lock(src->ctx);
	src->status = DE_SOUND_SOURCE_STATUS_PLAYING;
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_stop(de_sound_source_t* src) {
	de_sound_context_lock(src->ctx);
	src->status = DE_SOUND_SOURCE_STATUS_STOPPED;
	/* rewind */
	src->buf_read_pos = 0;
	de_sound_buffer_rewind(src->buffer);
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_pause(de_sound_source_t* src) {
	de_sound_context_lock(src->ctx);
	src->status = DE_SOUND_SOURCE_STATUS_PAUSED;
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_set_type(de_sound_source_t* src, de_sound_source_type_t type) {
	de_sound_context_lock(src->ctx);
	src->type = type;
	de_sound_context_unlock(src->ctx);
}

void de_sound_source_update(de_sound_source_t* src) {
	switch (src->type) {
		case DE_SOUND_SOURCE_TYPE_2D:
			break;
		case DE_SOUND_SOURCE_TYPE_3D:
			break;
		default:
			break;
	}
}