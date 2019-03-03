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

de_scene_t* de_scene_create(de_core_t* core) {
	de_scene_t* s = DE_NEW(de_scene_t);
	s->core = core;
	DE_LINKED_LIST_INIT(s->nodes);
	DE_ARRAY_APPEND(core->scenes, s);	
	return s;
}

void de_scene_free(de_scene_t* s) {
	/* free nodes */
	while (s->nodes.size) {
		de_node_free(s->nodes.data[0]);
	}

	/* free bodies */
	while(s->bodies.size) {
		de_body_free(s->bodies.data[0]);
	}

	/* free geoms */
	while (s->static_geometries.head) {
		de_scene_free_static_geometry(s, s->static_geometries.head);
	}

	/* free animations */
	while (s->animations.head) {
		de_animation_free(s->animations.head);
	}

	if (s->core) {
		DE_ARRAY_REMOVE(s->core->scenes, s);		
	}

	de_free(s);
}

de_static_geometry_t* de_scene_create_static_geometry(de_scene_t* s) {
	de_static_geometry_t* geom;
	assert(s);
	geom = DE_NEW(de_static_geometry_t);
	DE_LINKED_LIST_APPEND(s->static_geometries, geom);
	geom->scene = s;
	return geom;
}

void de_scene_free_static_geometry(de_scene_t* s, de_static_geometry_t* geom) {
	assert(s);
	DE_LINKED_LIST_REMOVE(s->static_geometries, geom);
	DE_ARRAY_FREE(geom->triangles);
	de_free(geom);
}

void de_scene_add_node(de_scene_t* s, de_node_h handle) {
	de_node_t* node = de_node_get_ptr(handle);
	DE_ASSERT(node);
	DE_ARRAY_APPEND(s->nodes, handle);
	node->scene = s;
	if (node->type == DE_NODE_TYPE_CAMERA) {
		s->active_camera = node;
	}
}

void de_scene_remove_node(de_scene_t* s, de_node_h handle) {
	de_node_t* node = de_node_get_ptr(handle);
	DE_ASSERT(node);

	if (node == s->active_camera) {
		s->active_camera = NULL;
	}

	node->scene = NULL;

	DE_ARRAY_REMOVE(s->nodes, handle);
}

de_node_h de_scene_find_node(const de_scene_t* s, const char* name) {	
	for (size_t i = 0; i < s->nodes.size; ++i) {
		de_node_t* node = de_node_get_ptr(s->nodes.data[i]);
		if (de_str8_eq(&node->name, name)) {
			return s->nodes.data[i];
		}
	}
	return (de_node_h) { .ref = de_null_ref };
}

void de_scene_update(de_scene_t* s, double dt) {
	de_animation_t* anim;
	
	/* Animations prepass - reset local transform of associated track nodes for blending */
	DE_LINKED_LIST_FOR_EACH(s->animations, anim) {
		if (de_animation_is_flags_set(anim, DE_ANIMATION_FLAG_ENABLED)) {
			size_t i;

			for (i = 0; i < anim->tracks.size; ++i) {
				de_animation_track_t* track = anim->tracks.data[i];
				de_node_t* node = de_node_get_ptr(track->node);
				if (node) {
					de_vec3_zero(&node->position);
					de_quat_set(&node->rotation, 0, 0, 0, 1);
					de_vec3_set(&node->scale, 1, 1, 1);
				}
			}
		}
	}
	/* Animation pass */
	DE_LINKED_LIST_FOR_EACH(s->animations, anim) {
		de_animation_update(anim, (float)dt);
	}
	/* Calculate transforms of nodes */
	for(size_t i = 0; i < s->nodes.size; ++i) {
		de_node_t* node = de_node_get_ptr(s->nodes.data[i]);
		if (node) {
			de_node_calculate_transforms(node);
		} else {
			de_log("dead ref ");
		}
	}
}

bool de_scene_visit(de_object_visitor_t* visitor, de_scene_t* scene) {
	bool result = true;

	//result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "Nodes", scene->nodes, de_node_t, de_node_visit);
	/*result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "Bodies", scene->bodies, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "StaticGeometries", scene->static_geometries, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "Animations", scene->animations, de_node_visit);*/
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "ActiveCamera", &scene->active_camera, de_node_visit);

	return result;
}