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

const char* de_resource_type_to_cstr(de_resource_type_t type) {
	switch (type) {
		case DE_RESOURCE_TYPE_MODEL: return "model";
		case DE_RESOURCE_TYPE_TEXTURE: return "texture";
		case DE_RESOURCE_TYPE_SOUND_BUFFER: return "sound buffer";
	}
	return "<unknown>";
}

/**
* @brief Internal. Use specializations for concrete type.
*/
de_resource_t* de_resource_create(de_core_t* core, de_resource_type_t type) {
	de_resource_t* res = DE_NEW(de_resource_t);
	res->core = core;
	res->type = type;
	res->ref_count = 1;
	DE_ARRAY_APPEND(core->resources, res);
	switch (res->type) {
		case DE_RESOURCE_TYPE_MODEL: break;
		case DE_RESOURCE_TYPE_TEXTURE: break;
		case DE_RESOURCE_TYPE_SOUND_BUFFER: break;
		default: de_fatal_error("unhandled resource type"); break;
	}
	return res;
}

/**
* @brief Increases reference counter of resource. You must call de_resource_release when you do not
* need resource anymore.
*/
void de_resource_add_ref(de_resource_t* res) {
	++res->ref_count;
}

/**
* @brief Frees resource
*/
void de_resource_release(de_resource_t* res) {
	DE_ASSERT(res->ref_count >= 0);
	--res->ref_count;
	if (res->ref_count == 0) {
		switch (res->type) {
			case DE_RESOURCE_TYPE_MODEL: break;
			case DE_RESOURCE_TYPE_TEXTURE: break;
			case DE_RESOURCE_TYPE_SOUND_BUFFER: break;
			default: de_fatal_error("unhandled resource type"); break;
		}
		de_free(res);
	}
}

bool de_resource_visit(de_object_visitor_t* visitor, de_resource_t* res) {
	bool result = true;
	if (visitor->is_reading) {
		res->core = visitor->core;
	}
	result &= de_object_visitor_visit_uint32(visitor, "Type", (uint32_t*)&res->type);
	result &= de_object_visitor_visit_path(visitor, "Source", &res->source);	
	switch (res->type) {
		case DE_RESOURCE_TYPE_MODEL: result &= de_model_visit(visitor, &res->s.model); break;
		case DE_RESOURCE_TYPE_TEXTURE: break;
		case DE_RESOURCE_TYPE_SOUND_BUFFER: break;
		default: de_fatal_error("unhandled resource type"); break;
	}
	return result;
}

de_model_t* de_resource_to_model(de_resource_t* res) {
	return &res->s.model;
}

de_resource_t* de_resource_from_model(de_model_t* mdl) {
	return (de_resource_t*)((char*)mdl - offsetof(de_resource_t, s.model));
}