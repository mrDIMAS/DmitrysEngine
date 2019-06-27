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

static void de_sound_buffer_init(de_resource_t* res)
{
	de_sound_buffer_t* buf = de_resource_to_sound_buffer(res);
	de_sound_context_t* ctx = res->core->sound_context;
	de_sound_context_lock(ctx);
	buf->ctx = ctx;
	buf->flags = 0;
	de_sound_context_unlock(ctx);
}

void de_sound_buffer_reload(de_sound_buffer_t* buf)
{
	DE_ASSERT(buf);
	de_sound_context_lock(buf->ctx);
	buf->need_reload = true;
	de_sound_context_unlock(buf->ctx);
}

static bool de_sound_buffer_load(de_resource_t* res)
{
	de_sound_buffer_t* buf = de_resource_to_sound_buffer(res);
	if (de_sound_decoder_is_source_valid(&res->source)) {
		de_sound_buffer_reload(buf);
		return true;
	}
	de_log("sound buffer: invalid resource %s", de_path_cstr(&res->source));
	return false;
}

static void de_sound_buffer_clear(de_sound_buffer_t* buf)
{
	if (buf->decoder) {
		de_sound_decoder_free(buf->decoder);
		buf->decoder = NULL;
	}
	de_free(buf->data);
	buf->data = NULL;
}

static void de_sound_buffer_load_internal(de_sound_buffer_t* buf, const de_path_t* source)
{
	DE_ASSERT(buf->decoder == NULL);
	DE_ASSERT(buf->data == NULL);

	buf->decoder = de_sound_decoder_create(de_path_cstr(source));
	buf->channel_count = buf->decoder->channel_count;

	if (buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM) {
		/* 1 second streaming buffer */
		buf->sample_per_channel = 2 * buf->ctx->dev.out_samples_count;
	} else {
		/* Full-length buffer */
		buf->sample_per_channel = buf->decoder->sample_per_channel;
	}

	size_t buffer_sample_count;
	const size_t block_sample_count = buf->sample_per_channel * buf->channel_count;
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
}

static void de_sound_buffer_deinit(de_resource_t* res)
{
	de_sound_buffer_t* buf = de_resource_to_sound_buffer(res);
	de_sound_context_t* ctx = buf->ctx;
	de_sound_context_lock(ctx);
	de_sound_buffer_clear(buf);
	de_sound_context_unlock(ctx);
}

void de_sound_buffer_update(de_sound_buffer_t* buf)
{
	/* TODO: Resource load ideally should be performed in separate thread, 
	 * this eventually will be done, but for now we'll load everything here. */
	if (buf->need_reload) {
		de_resource_t* res = de_resource_from_sound_buffer(buf);
		de_sound_buffer_clear(buf);
		de_sound_buffer_load_internal(buf, &res->source);
		buf->need_reload = false;
	}

	if (buf->flags & DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK) {
		const size_t readed = de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, 0, buf->sample_per_channel);
		if (readed < buf->sample_per_channel) {
			/* Make sure to read rest of block from begin of source file. */
			de_sound_decoder_rewind(buf->decoder);
			const size_t next_readed = de_sound_decoder_read(buf->decoder, buf->stream_write_ptr, buf->sample_per_channel, readed, buf->sample_per_channel - readed);
			DE_ASSERT(next_readed + readed == buf->sample_per_channel);
		}
		de_sound_buffer_reset_flags(buf, DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK);
	}
}

void de_sound_buffer_swap_pointers(de_sound_buffer_t* buf)
{
	float* temp = buf->read_ptr;
	buf->read_ptr = buf->stream_write_ptr;
	buf->stream_write_ptr = temp;
}

void de_sound_buffer_reset_pointers(de_sound_buffer_t* buf)
{
	buf->read_ptr = buf->data;
	buf->stream_write_ptr = buf->data + buf->sample_per_channel * buf->channel_count;
}

void de_sound_buffer_set_flags(de_sound_buffer_t* buf, uint32_t flags)
{
	if ((flags & DE_SOUND_BUFFER_FLAGS_STREAM) && (!(buf->flags & DE_SOUND_BUFFER_FLAGS_STREAM))) {
		de_sound_buffer_reload(buf);
	}

	buf->flags |= flags;
}

void de_sound_buffer_reset_flags(de_sound_buffer_t* buf, uint32_t flags)
{
	buf->flags &= ~flags;
}

void de_sound_buffer_rewind(de_sound_buffer_t* buf)
{
	de_sound_context_t* ctx = buf->ctx;
	de_sound_context_lock(ctx);
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
	de_sound_context_unlock(ctx);
}

static bool de_sound_buffer_visit(de_object_visitor_t* visitor, de_resource_t* res)
{
	de_sound_buffer_t* buf = de_resource_to_sound_buffer(res);
	bool result = true;
	result &= de_object_visitor_visit_uint32(visitor, "Flags", &buf->flags);
	if (visitor->is_reading) {
		buf->ctx = visitor->core->sound_context;
	}
	return result;
}

de_resource_dispatch_table_t* de_sound_buffer_get_dispatch_table()
{
	static de_resource_dispatch_table_t table = {
		.init = de_sound_buffer_init,
		.deinit = de_sound_buffer_deinit,
		.visit = de_sound_buffer_visit,
		.load = de_sound_buffer_load
	};
	return &table;
}