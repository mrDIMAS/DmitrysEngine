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

typedef struct de_fbx_buffer_t {
	char* data;
	char* ptr;
	char* end;
	size_t size;
} de_fbx_buffer_t;

void de_fbx_buffer_init(de_fbx_buffer_t* buf, size_t size);

/**
 * @brief Performs very fast continuous memory "allocations" in fixed-size buffer
 *
 * Main purpose of this method is to reduce memory thrashing when loading FBX
 * properties so every property will be placed in the single memory block.
 */
void* de_fbx_buffer_alloc(de_fbx_buffer_t* buf, size_t size);

void de_fbx_buffer_free(de_fbx_buffer_t* buf);

typedef struct de_fbx_node_s {
	/* Name of a node */
	de_str8_t name;

	/* Means that each attribute is binary so no conversion needed on fetch */
	bool is_binary;

	/* Array of attributes */
	DE_ARRAY_DECLARE(char*, attributes);

	/* Pointer to parent node */
	struct de_fbx_node_s* parent;

	/* Array of pointers to children nodes */
	DE_ARRAY_DECLARE(struct de_fbx_node_s*, children);
} de_fbx_node_t;

int de_fbx_get_int(de_fbx_node_t* node, int index);

int64_t de_fbx_get_int64(de_fbx_node_t* node, int index);

float de_fbx_get_float(de_fbx_node_t* node, int index);

double de_fbx_get_double(de_fbx_node_t* node, int index);

/* reads 3 elements from passed index */
void de_fbx_get_vec3(de_fbx_node_t* node, int index, de_vec3_t* out);

/* reads 2 elements from passed index */
void de_fbx_get_vec2(de_fbx_node_t* node, int index, de_vec2_t* out);

char* de_fbx_get_string(de_fbx_node_t* node, int index);

/* Allocates new hierarchy node with passed name, returns pointer to new node. */
de_fbx_node_t* de_fbx_create_node(const char* name);

de_fbx_node_t* de_fbx_node_get_child(de_fbx_node_t* node, const char* name);

de_fbx_node_t* de_fbx_node_find_child(de_fbx_node_t* node, const char* name);

void de_fbx_node_free(de_fbx_node_t* node);