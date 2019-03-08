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

static void de_model_deinit(de_resource_t* res) {
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_MODEL);
	de_model_t* mdl = &res->s.model;
	de_scene_free(mdl->scene);
}

bool de_model_load(de_model_t* mdl, const de_path_t* path) {
	de_resource_t* res = de_resource_from_model(mdl);
	de_core_t* core = res->core;
	mdl->scene = de_scene_create(core);
	DE_LINKED_LIST_REMOVE(core->scenes, mdl->scene);
	mdl->root = de_fbx_load_to_scene(mdl->scene, de_path_cstr(path));
	return mdl->root != NULL;
}

static void de_model_set_node_model(de_node_t* node, de_resource_t* res) {
	node->model_resource = res;
	de_resource_add_ref(node->model_resource);
	for (size_t i = 0; i < node->children.size; ++i) {
		de_model_set_node_model(node->children.data[i], res);
	}
}

bool de_model_visit(de_object_visitor_t* visitor, de_model_t* mdl) {
	bool result = true;
	if (visitor->is_reading) {
		de_resource_t* res = de_resource_from_model(mdl);
		result &= de_model_load(mdl, &res->source);
	}
	return result;
}

de_node_t* de_model_instantiate(de_model_t* mdl, de_scene_t* dest_scene) {
	de_resource_t* res = de_resource_from_model(mdl);
	de_node_t* copy = de_node_copy(dest_scene, mdl->root);
	de_model_set_node_model(copy, res);
	return copy;
}