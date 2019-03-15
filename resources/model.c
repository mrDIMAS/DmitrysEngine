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

void de_model_deinit(de_model_t* mdl) {		
	DE_ASSERT(mdl);
	de_scene_free(mdl->scene);
}

static void de_model_set_node_resouce(de_node_t* node, de_resource_t* res) {
	DE_ASSERT(node);
	DE_ASSERT(res);
	node->model_resource = res;
	de_resource_add_ref(node->model_resource);
	for (size_t i = 0; i < node->children.size; ++i) {
		de_model_set_node_resouce(node->children.data[i], res);
	}
}

bool de_model_load(de_model_t* mdl, const de_path_t* path) {
	DE_ASSERT(path);
	DE_ASSERT(mdl);
	de_resource_t* res = de_resource_from_model(mdl);
	de_core_t* core = res->core;
	mdl->scene = de_scene_create(core);
	DE_LINKED_LIST_REMOVE(core->scenes, mdl->scene);
	mdl->root = de_fbx_load_to_scene(mdl->scene, de_path_cstr(path));
	if (mdl->root) {
		de_model_set_node_resouce(mdl->root, res);
		DE_LINKED_LIST_FOR_EACH_T(de_animation_t*, anim, mdl->scene->animations) {
			anim->resource = res;
		}
	} else {
		de_log("failed to load model %s", de_path_cstr(path));
	}
	return mdl->root != NULL;
}

bool de_model_visit(de_object_visitor_t* visitor, de_model_t* mdl) {
	DE_ASSERT(visitor);
	DE_ASSERT(mdl);

	bool result = true;
	if (visitor->is_reading) {
		de_resource_t* res = de_resource_from_model(mdl);
		result &= de_model_load(mdl, &res->source);
	}
	return result;
}

de_node_t* de_model_instantiate(de_model_t* mdl, de_scene_t* dest_scene) {
	DE_ASSERT(mdl);
	DE_ASSERT(dest_scene);

	if (!mdl->root) {
		return NULL;
	}

	/* Instantiate nodes. */
	de_node_t* copy = de_node_copy(dest_scene, mdl->root);

	/* Instantiate animations. */
	DE_LINKED_LIST_FOR_EACH_T(de_animation_t*, ref_anim, mdl->scene->animations) {
		de_animation_t* anim_copy = de_animation_copy(ref_anim, dest_scene);

		/* Remap animation track nodes. */
		for (size_t i = 0; i < ref_anim->tracks.size; ++i) {
			de_animation_track_t* ref_track = ref_anim->tracks.data[i];
			
			/* Find instantiated node that corresponds to node in resource */
			DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, dest_scene->nodes) {
				if (ref_track->node && node->original == ref_track->node) {
					anim_copy->tracks.data[i]->node = node;
				}
			}			 
		}
	}

	return copy;
}