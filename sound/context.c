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

de_sound_context_t* de_sound_context_create(de_core_t* core)
{
	de_sound_context_t* ctx = DE_NEW(de_sound_context_t);
	de_mtx_init(&ctx->mtx);
	ctx->core = core;
	ctx->master_volume = 1.0f;
	de_listener_init(&ctx->listener);
	de_sound_device_init(ctx, &ctx->dev);
	return ctx;
}

void de_sound_context_free(de_sound_context_t* ctx)
{
	de_sound_context_clear(ctx);
	de_sound_device_free(&ctx->dev);
	for (size_t i = 0; i < ctx->sounds.size; ++i) {
		de_sound_source_t* src = ctx->sounds.data[i];
		de_log("unfreed sound source found -> mem leaks. details:\n\tsource: %s",
			src->buffer ? de_path_cstr(&de_resource_from_sound_buffer(src->buffer)->source) : "not set");
	}
	DE_ARRAY_FREE(ctx->sounds);
	de_mtx_destroy(&ctx->mtx);
	de_free(ctx);
}

void de_sound_context_clear(de_sound_context_t* ctx)
{
	for (int i = (int)ctx->sounds.size - 1; i >= 0; --i) {
		de_sound_source_t* src = ctx->sounds.data[i];
		if (src->play_once) {
			de_sound_source_free(src);
		}
	}
}

void de_sound_context_update(de_sound_context_t* ctx)
{
	de_sound_context_lock(ctx);

	for (size_t i = 0; i < ctx->sounds.size; ++i) {
		de_sound_source_t* src = ctx->sounds.data[i];
		de_sound_source_update(src);
	}

	for (int i = (int)ctx->sounds.size - 1; i >= 0; --i) {
		de_sound_source_t* src = ctx->sounds.data[i];
		if (src->play_once && (src->status & DE_SOUND_SOURCE_STATUS_STOPPED)) {
			de_sound_source_free(src);			
		}
	}

	for (size_t i = 0; i < ctx->core->resources.size; ++i) {
		de_resource_t* res = ctx->core->resources.data[i];
		if (res->type == DE_RESOURCE_TYPE_SOUND_BUFFER) {
			de_sound_buffer_t* buf = de_resource_to_sound_buffer(res);
			de_sound_buffer_update(buf);
		}
	}

	de_sound_context_unlock(ctx);
}

void de_sound_context_lock(de_sound_context_t* ctx)
{
	de_mtx_lock(&ctx->mtx);
}

void de_sound_context_unlock(de_sound_context_t* ctx)
{
	de_mtx_unlock(&ctx->mtx);
}

de_listener_t* de_sound_context_get_listener(de_sound_context_t* ctx)
{
	return &ctx->listener;
}

bool de_sound_context_visit(de_object_visitor_t* visitor, de_sound_context_t* ctx)
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Sources", ctx->sounds, de_sound_source_visit);
	return result;
}

void de_sound_context_set_master_volume(de_sound_context_t* ctx, float vol)
{
	ctx->master_volume = de_clamp(vol, 0.0f, 1.0f);
}