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

typedef enum de_resource_type_t {
	DE_RESOURCE_TYPE_MODEL,
	DE_RESOURCE_TYPE_TEXTURE,
	DE_RESOURCE_TYPE_SOUND_BUFFER,
} de_resource_type_t;

#include "resources/model.h"

typedef struct de_resource_dispatch_table_t {
	void(*deinit)(de_resource_t*);
} de_resource_dispatch_table_t;

struct de_resource_t {
	de_core_t* core;
	de_resource_type_t type; /**< Actual resource type. */
	int ref_count;
	de_path_t source; /**< Path for external resource. */
	bool external; /**< true if resource needs to be loaded from external source, false for dynamic resources. */
	union {
		de_texture_t texture;
		de_model_t model;
	} s;
	de_resource_dispatch_table_t* dispatch_table;
};

const char* de_resource_type_to_cstr(de_resource_type_t type);

/**
 * @brief Internal. Use specializations for concrete type.
 */
de_resource_t* de_resource_alloc(de_core_t* core, de_resource_type_t type, de_resource_dispatch_table_t* tbl);

/**
 * @brief Increases reference counter of resource. You must call de_resource_release when you do not
 * need resource anymore.
 */
void de_resource_add_ref(de_resource_t* res);

/**
 * @brief Frees resource
 */
void de_resource_release(de_resource_t* res);