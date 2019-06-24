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

#include "resources/model.c"

const char* de_resource_type_to_cstr(de_resource_type_t type)
{
	switch (type) {
		case DE_RESOURCE_TYPE_MODEL: return "model";
		case DE_RESOURCE_TYPE_TEXTURE: return "texture";
		case DE_RESOURCE_TYPE_SOUND_BUFFER: return "sound buffer";
		default: break;
	}
	return "<unknown>";
}

de_resource_dispatch_table_t* de_resource_get_dispatch_table_by_type(de_resource_type_t type)
{
	switch (type) {
		case DE_RESOURCE_TYPE_MODEL: return de_model_get_dispatch_table();
		case DE_RESOURCE_TYPE_TEXTURE: return de_texture_get_dispatch_table();
		case DE_RESOURCE_TYPE_SOUND_BUFFER: return de_sound_buffer_get_dispatch_table();
		default: return NULL;
	}
}

de_resource_t* de_resource_create(de_core_t* core, const de_path_t* path, de_resource_type_t type)
{
	de_resource_t* res = DE_NEW(de_resource_t);
	res->core = core;
	res->dispatch_table = de_resource_get_dispatch_table_by_type(type);
	DE_ASSERT(res->dispatch_table);
	res->type = type;
	res->ref_count = 0;
	res->flags = 0;
	if (path) {
		de_path_copy(path, &res->source);
	}
	DE_ARRAY_APPEND(core->resources, res);
	if (res->dispatch_table->init) {
		res->dispatch_table->init(res);
	}
	de_log("resource created: %s", de_path_cstr(&res->source));
	return res;
}

void de_resource_set_flags(de_resource_t* res, de_resource_flags_t flags) 
{
	DE_ASSERT(res);
	res->flags = flags;
}

void de_resource_add_ref(de_resource_t* res)
{
	++res->ref_count;
}

int de_resource_release(de_resource_t* res)
{
	/**
	 * If this assert triggers most likely that you have mismatch in de_resource_add_ref/de_resource_release
	 * calls. This can happen for various reasons, most common is that you forgot to call
	 * de_resource_release. If this happens on deserialization then you forgot to serialize an object
	 * that uses this resource, imagine this situation: you have a level and a player, player have weapons,
	 * some weapon uses same texture as some level part uses. Then you doing serialization and serializing
	 * only level and player and forget to serialize weapons, then you will have mismatch in de_resource_add_ref
	 * and de_resource_release calls which will result in this assert. Check everything carefully!
	 */
	DE_ASSERT(res->ref_count >= 0);

	--res->ref_count;
	if (res->ref_count == 0) {
		if (res->dispatch_table->deinit) {
			res->dispatch_table->deinit(res);
		}
		DE_ARRAY_REMOVE(res->core->resources, res);
		de_log("resource released: %s", de_path_cstr(&res->source));
		de_path_free(&res->source);
		de_free(res);
		return 0;
	}
	return res->ref_count;
}

bool de_resource_visit(de_object_visitor_t* visitor, de_resource_t* res)
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_ENUM(visitor, "Type", &res->type);
	if (visitor->is_reading) {
		res->core = visitor->core;
		res->dispatch_table = de_resource_get_dispatch_table_by_type(res->type);
		DE_ASSERT(res->dispatch_table);
	}
	result &= de_object_visitor_visit_path(visitor, "Source", &res->source);
	/* do not serialize ref counter because internally resource can load any other
	 * resources increasing ref counter of resource */
	if (res->dispatch_table->visit) {
		result &= res->dispatch_table->visit(visitor, res);
	}
	return result;
}

de_model_t* de_resource_to_model(de_resource_t* res)
{
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_MODEL);
	return &res->s.model;
}

de_resource_t* de_resource_from_model(de_model_t* mdl)
{
	de_resource_t* res = (de_resource_t*)((char*)mdl - offsetof(de_resource_t, s.model));
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_MODEL);
	return res;
}

de_sound_buffer_t* de_resource_to_sound_buffer(de_resource_t* res)
{
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_SOUND_BUFFER);
	return &res->s.sound_buffer;
}

de_resource_t* de_resource_from_sound_buffer(de_sound_buffer_t* buf)
{
	de_resource_t* res = (de_resource_t*)((char*)buf - offsetof(de_resource_t, s.sound_buffer));
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_SOUND_BUFFER);
	return res;
}

de_texture_t* de_resource_to_texture(de_resource_t* res)
{
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_TEXTURE);
	return &res->s.texture;
}

de_resource_t* de_resource_from_texture(de_texture_t* buf)
{
	de_resource_t* res = (de_resource_t*)((char*)buf - offsetof(de_resource_t, s.texture));
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_TEXTURE);
	return res;
}

bool de_resource_load(de_resource_t* res)
{
	if (res->dispatch_table->load) {
		return res->dispatch_table->load(res);
	}
	return true; // is this valid?
}

de_resource_type_t de_resource_get_type(de_resource_t* res)
{
	return res->type;
}