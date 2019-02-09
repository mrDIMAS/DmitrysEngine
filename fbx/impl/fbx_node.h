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


int de_fbx_get_int(de_fbx_node_t* node, int index) {
	int value;

	if (node->is_binary) {
		value = *((int*)node->attributes.data[index]);
	} else {
		value = atoi(node->attributes.data[index]);
	}

	return value;
}


int64_t de_fbx_get_int64(de_fbx_node_t* node, int index) {
	int64_t value;

	if (node->is_binary) {
		value = *((int64_t*)node->attributes.data[index]);
	} else {
		value = atoll(node->attributes.data[index]);
	}

	return value;
}


float de_fbx_get_float(de_fbx_node_t* node, int index) {
	float value;

	if (node->is_binary) {
		value = *((float*)node->attributes.data[index]);
	} else {
		value = (float)atof(node->attributes.data[index]);
	}

	return value;
}

double de_fbx_get_double(de_fbx_node_t* node, int index) {
	double value;

	if (node->is_binary) {
		value = *((double*)node->attributes.data[index]);
	} else {
		value = atof(node->attributes.data[index]);
	}

	return value;
}


void de_fbx_get_vec3(de_fbx_node_t* node, int index, de_vec3_t* out) {
	out->x = (float)de_fbx_get_double(node, index);
	out->y = (float)de_fbx_get_double(node, index + 1);
	out->z = (float)de_fbx_get_double(node, index + 2);
}


void de_fbx_get_vec2(de_fbx_node_t* node, int index, de_vec2_t* out) {
	out->x = (float)de_fbx_get_double(node, index);
	out->y = (float)de_fbx_get_double(node, index + 1);
}


char* de_fbx_get_string(de_fbx_node_t* node, int index) {
	return node->attributes.data[index];
}


de_fbx_node_t* de_fbx_create_node(const char* name) {
	de_fbx_node_t* node = DE_NEW(de_fbx_node_t);
	de_str8_set(&node->name, name);
	return node;
}


de_fbx_node_t* de_fbx_node_find_child(de_fbx_node_t* node, const char* name) {
	size_t i;

	/* Look for the node in children nodes. */
	for (i = 0; i < node->children.size; ++i) {
		de_fbx_node_t* child = node->children.data[i];

		if (de_str8_eq(&child->name, name)) {
			return child;
		}
	}

	if (i == node->children.size) {
		de_log("Unable to find '%s' node", name);
	}

	return NULL;
}


void de_fbx_node_free(de_fbx_node_t* node) {
	size_t i;

	/* Free name string. */
	de_str8_free(&node->name);

	/* Free attributes. */
	DE_ARRAY_FREE(node->attributes);

	/* Free children recursively. */
	for (i = 0; i < node->children.size; ++i) {
		de_fbx_node_free(node->children.data[i]);
	}
	DE_ARRAY_FREE(node->children);

	/* Free node. */
	de_free(node);
}


de_fbx_node_t* de_fbx_node_get_child(de_fbx_node_t* node, const char* name) {
	size_t i;

	/* Look for the node in children nodes. */
	for (i = 0; i < node->children.size; ++i) {
		de_fbx_node_t* child = node->children.data[i];
		if (de_str8_eq(&child->name, name)) {
			return child;
		}
	}

	return NULL;
}