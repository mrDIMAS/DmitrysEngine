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

#include "resources/model.h"

struct de_resource_t {
	de_resource_type_t type;
	de_path_t source;
	de_core_t* core;	
	de_resource_dispatch_table_t* dispatch_table;
	int32_t ref_count; /** TODO: Probably should be atomic? */
	uint32_t flags;
	union {
		de_sound_buffer_t sound_buffer;
		de_texture_t texture;
		de_model_t model;
	} s;
};

const char* de_resource_type_to_cstr(de_resource_type_t type);

/**
 * @brief Creates new resource of specified type with specified flags. Automatically
 * registers self in core. path can be null for runtime resources.
 */
de_resource_t* de_resource_create(de_core_t* core, const de_path_t* path, de_resource_type_t type);

void de_resource_set_flags(de_resource_t* res, de_resource_flags_t flags);

/**
 * @brief Increases reference counter of resource. You must call de_resource_release when you do not
 * need resource anymore.
 */
void de_resource_add_ref(de_resource_t* res);

/**
 * @brief Frees resource
 */
int de_resource_release(de_resource_t* res);

bool de_resource_visit(de_object_visitor_t* visitor, de_resource_t* res);

/**
 * @brief Returns pointer to model based on resource pointer.
 */
de_model_t* de_resource_to_model(de_resource_t* res);

/**
 * @brief Returns pointer to resource from pointer to model resource.
 */
de_resource_t* de_resource_from_model(de_model_t* mdl);

/**
 * @brief Returns pointer to model resource based on resource pointer.
 */
de_sound_buffer_t* de_resource_to_sound_buffer(de_resource_t* res);

/**
 * @brief Returns pointer to resource from pointer to sound buffer resource.
 */
de_resource_t* de_resource_from_sound_buffer(de_sound_buffer_t* buf);

/**
* @brief Returns pointer to texture resource based on resource pointer.
*/
de_texture_t* de_resource_to_texture(de_resource_t* res);

/**
* @brief Returns pointer to resource from pointer to texture resource.
*/
de_resource_t* de_resource_from_texture(de_texture_t* buf);


de_resource_dispatch_table_t* de_resource_get_dispatch_table_by_type(de_resource_type_t type);

/**
 * @brief Internal. Performs dispatch call to type-specific loading routine.
 */
bool de_resource_load(de_resource_t* res);

de_resource_type_t de_resource_get_type(de_resource_t* res);