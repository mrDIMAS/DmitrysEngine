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

void de_node_free(de_node_t* node) {
	/* Free children first */
	while (node->children.size) {
		de_node_free(node->children.data[0]);
	}
	DE_ARRAY_FREE(node->children);

	/* Free the node */
	de_node_detach(node);

	if (node->scene) {
		de_scene_remove_node(node->scene, node);
	}

	de_str8_free(&node->name);
	switch (node->type) {
		case DE_NODE_TYPE_BASE: break; /* handled already */
		case DE_NODE_TYPE_CAMERA: de_camera_deinit(&node->s.camera); break;
		case DE_NODE_TYPE_LIGHT: de_light_deinit(&node->s.light); break;
		case DE_NODE_TYPE_MESH: de_mesh_deinit(&node->s.mesh); break;
		default: de_fatal_error("unhandled node type!"); break;
	}

	if (node->model_resource) {
		de_resource_release(node->model_resource);
		node->model_resource = NULL;
	}
	de_free(node);
}

de_node_t* de_node_create(de_scene_t* scene, de_node_type_t type) {
	de_node_t* node = DE_NEW(de_node_t);
	node->type = type;
	node->scene = scene;
	de_mat4_identity(&node->global_matrix);
	de_mat4_identity(&node->local_matrix);
	de_vec3_set(&node->scale, 1, 1, 1);
	de_quat_set(&node->rotation, 0, 0, 0, 1);
	de_quat_set(&node->pre_rotation, 0, 0, 0, 1);
	de_quat_set(&node->post_rotation, 0, 0, 0, 1);
	switch (node->type) {
		case DE_NODE_TYPE_BASE: break; /* handled already */
		case DE_NODE_TYPE_CAMERA: de_camera_init(&node->s.camera); break;
		case DE_NODE_TYPE_LIGHT: de_light_init(&node->s.light); break;
		case DE_NODE_TYPE_MESH: de_mesh_init(&node->s.mesh); break;
		default: de_fatal_error("unhandled node type!"); break;
	}
	return node;
}

static de_node_t* de_node_copy_internal(de_scene_t* dest_scene, de_node_t* node) {	
	de_node_t* copy = DE_NEW(de_node_t);
	copy->type = node->type;
	de_str8_copy(&node->name, &copy->name);
	copy->scene = dest_scene;
	copy->inv_bind_pose_matrix = node->inv_bind_pose_matrix;
	copy->position = node->position;
	copy->original = node;
	copy->scale = node->scale;
	copy->scene = node->scene;
	copy->rotation = node->rotation;
	copy->pre_rotation = node->pre_rotation;
	copy->post_rotation = node->post_rotation;
	copy->rotation_offset = node->rotation_offset;
	copy->rotation_pivot = node->rotation_pivot;
	copy->scaling_offset = node->scaling_offset;
	copy->scaling_pivot = node->scaling_pivot;	
	copy->need_update = true;
	copy->parent = NULL;
	copy->visible = node->visible;
	copy->is_bone = node->is_bone;
	copy->model_resource = node->model_resource;
	if (copy->model_resource) {
		de_resource_add_ref(copy->model_resource);
	}
	de_scene_add_node(dest_scene, copy);
	de_body_t* body = node->body;
	if (body) {
		copy->body = de_body_copy(dest_scene, body);
	}
	for (size_t i = 0; i < node->children.size; ++i) {
		de_node_attach(de_node_copy_internal(dest_scene, node->children.data[i]), copy);
	}
	switch (node->type) {
		case DE_NODE_TYPE_BASE: break; /* handled already */ 
		case DE_NODE_TYPE_CAMERA: de_camera_copy(&node->s.camera, &copy->s.camera); break;
		case DE_NODE_TYPE_LIGHT: de_light_copy(&node->s.light, &copy->s.light); break;
		case DE_NODE_TYPE_MESH: de_mesh_copy(&node->s.mesh, &copy->s.mesh); break;
		default: de_fatal_error("unhandled node type!"); break;
	}
	de_node_calculate_transforms(copy);
	return copy;
}

void de_node_resolve(de_node_t* node) {
	switch (node->type) {
		case DE_NODE_TYPE_BASE: 
			break; 
		case DE_NODE_TYPE_CAMERA: 
			break;
		case DE_NODE_TYPE_LIGHT: 
			break;
		case DE_NODE_TYPE_MESH: 
			de_mesh_resolve_bones(&node->s.mesh);
			break;
		default: de_fatal_error("unhandled node type!"); break;
	}

	for (size_t i = 0; i < node->children.size; ++i) {
		de_node_resolve(node->children.data[i]);
	}
}

de_node_t* de_node_copy(de_scene_t* dest_scene, de_node_t* node) {
	de_node_t* root = de_node_copy_internal(dest_scene, node);
	/* Resolve after copy */
	de_node_resolve(root);
	return root;
}

void de_node_attach(de_node_t* node, de_node_t* parent) {
	de_node_detach(node);
	DE_ARRAY_APPEND(parent->children, node);
	node->parent = parent;
}

void de_node_detach(de_node_t* node) {
	if (node->parent) {
		DE_ARRAY_REMOVE(node->parent->children, node);
		node->parent = NULL;
	}
}

de_mat4_t* de_node_calculate_transforms(de_node_t* node) {
	de_mat4_t pre_rotation;
	de_mat4_t post_rotation;
	de_mat4_t rotation;
	de_mat4_t scale;
	de_mat4_t translation;
	de_mat4_t rotation_offset;
	de_mat4_t rotation_pivot;
	de_mat4_t rotation_pivot_inv;
	de_mat4_t scale_offset;
	de_mat4_t scale_pivot;
	de_mat4_t scale_pivot_inv;

	if (!node) {
		return NULL;
	}

	const de_body_t* body = node->body;
	if (body) {
		de_body_get_position(body, &node->position);
	}

	/**
	 * TODO: Why there is so much matrices? Because of FBX support.
	 * FBX uses very complex transformations and there is no way (in my opinion)
	 * to convert these to just three parameters:
	 *  - translation
	 *  - rotation quaternion
	 *  - scale
	 * So to exclude weird transformations behaviour when using FBX, I just decided
	 * to use all these matrices. This is not optimized at all, for example 70% of
	 * these multiplications and matrices creation can be done only once on load.
	 *
	 * TODO: OPTIMIZE THIS ASAP!
	 */

	de_mat4_rotation(&pre_rotation, &node->pre_rotation);

	de_mat4_rotation(&post_rotation, &node->post_rotation);
	de_mat4_inverse(&post_rotation, &post_rotation);

	de_mat4_rotation(&rotation, &node->rotation);
	de_mat4_scale(&scale, &node->scale);
	de_mat4_translation(&translation, &node->position);

	de_mat4_translation(&rotation_offset, &node->rotation_offset);
	de_mat4_translation(&rotation_pivot, &node->rotation_pivot);
	de_mat4_inverse(&rotation_pivot_inv, &rotation_pivot);

	de_mat4_translation(&scale_offset, &node->scaling_offset);
	de_mat4_translation(&scale_pivot, &node->scaling_pivot);
	de_mat4_inverse(&scale_pivot_inv, &scale_pivot);

	de_mat4_mul(&node->local_matrix, &translation, &rotation_offset);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &rotation_pivot);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &pre_rotation);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &rotation);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &post_rotation);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &rotation_pivot_inv);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &scale_offset);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &scale_pivot);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &scale);
	de_mat4_mul(&node->local_matrix, &node->local_matrix, &scale_pivot_inv);

	de_node_t* parent = node->parent;
	if (parent) {
		de_mat4_mul(&node->global_matrix, de_node_calculate_transforms(parent), &node->local_matrix);
	} else {
		node->global_matrix = node->local_matrix;
	}

	return &node->global_matrix;
}

void de_node_get_look_vector(de_node_t* node, de_vec3_t* look) {
	if (!node || !look) {
		return;
	}

	de_mat4_look(&node->global_matrix, look);
}

void de_node_get_up_vector(de_node_t* node, de_vec3_t* up) {
	if (!node || !up) {
		return;
	}

	de_mat4_up(&node->global_matrix, up);
}

void de_node_get_side_vector(de_node_t* node, de_vec3_t* side) {
	if (!node || !side) {
		return;
	}

	de_mat4_side(&node->global_matrix, side);
}

void de_node_get_global_position(de_node_t* node, de_vec3_t* pos) {
	if (!node || !pos) {
		return;
	}

	pos->x = node->global_matrix.f[12];
	pos->y = node->global_matrix.f[13];
	pos->z = node->global_matrix.f[14];
}

void de_node_set_local_position(de_node_t* node, de_vec3_t* pos) {
	if (!node || !pos) {
		return;
	}

	de_body_t* body = node->body;
	if (body) {
		de_body_set_position(body, pos);
	} else {
		node->position = *pos;
	}
}

void de_node_set_local_position_xyz(de_node_t* node, float x, float y, float z) {
	de_vec3_t p;

	de_vec3_set(&p, x, y, z);

	de_node_set_local_position(node, &p);
}

void de_node_set_body(de_node_t* node, de_body_t* body) {
	if (!node) {
		return;
	}

	node->body = body;
}

void de_node_move(de_node_t* node, de_vec3_t* offset) {
	if (!node || !offset) {
		return;
	}

	de_vec3_add(&node->position, &node->position, offset);
}

void de_node_set_local_rotation(de_node_t* node, de_quat_t* rot) {
	if (!node || !rot) {
		return;
	}

	node->rotation = *rot;
}

void de_node_set_local_scale(de_node_t* node, de_vec3_t* scl) {
	if (!node || !scl) {
		return;
	}

	node->scale = *scl;
}

de_node_t* de_node_find(de_node_t* node, const char* name) {
	if (node) {
		if (de_str8_eq(&node->name, name)) {
			return node;
		} else {
			for (size_t i = 0; i < node->children.size; ++i) {
				de_node_t* child = node->children.data[i];
				de_node_t* result = de_node_find(child, name);
				if (result) {
					return result;
				}
			}
		}
	}

	return NULL;
}

de_mesh_t* de_node_to_mesh(de_node_t* node) {
	DE_ASSERT_SCENE_NODE_TYPE(node, DE_NODE_TYPE_MESH);
	return &node->s.mesh;
}

de_node_t* de_node_from_mesh(de_mesh_t* mesh) {
	return (de_node_t*)((char*)mesh - offsetof(de_node_t, s.mesh));
}

de_light_t* de_node_to_light(de_node_t* node) {
	DE_ASSERT_SCENE_NODE_TYPE(node, DE_NODE_TYPE_LIGHT);
	return &node->s.light;
}

de_node_t* de_node_from_light(de_mesh_t* light) {
	return (de_node_t*)((char*)light - offsetof(de_node_t, s.light));
}

de_camera_t* de_node_to_camera(de_node_t* node) {
	DE_ASSERT_SCENE_NODE_TYPE(node, DE_NODE_TYPE_CAMERA);
	return &node->s.camera;
}

de_node_t* de_node_from_camera(de_camera_t* camera) {
	return (de_node_t*)((char*)camera - offsetof(de_node_t, s.camera));
}

bool de_node_visit(de_object_visitor_t* visitor, de_node_t* node) {
	bool result = true;
	result &= de_object_visitor_visit_uint32(visitor, "Type", (uint32_t*)&node->type);
	result &= de_object_visitor_visit_string(visitor, "Name", &node->name);
	result &= de_object_visitor_visit_mat4(visitor, "LocalTransform", &node->local_matrix);
	result &= de_object_visitor_visit_mat4(visitor, "GlobalTransform", &node->global_matrix);
	result &= de_object_visitor_visit_mat4(visitor, "InvBindPoseTransform", &node->inv_bind_pose_matrix);
	result &= de_object_visitor_visit_vec3(visitor, "LocalPosition", &node->position);
	result &= de_object_visitor_visit_vec3(visitor, "LocalScale", &node->scale);
	result &= de_object_visitor_visit_quat(visitor, "LocalRotation", &node->rotation);
	result &= de_object_visitor_visit_quat(visitor, "PreRotation", &node->pre_rotation);
	result &= de_object_visitor_visit_quat(visitor, "PostRotation", &node->post_rotation);
	result &= de_object_visitor_visit_vec3(visitor, "RotationOffset", &node->rotation_offset);
	result &= de_object_visitor_visit_vec3(visitor, "RotationPivot", &node->rotation_pivot);
	result &= de_object_visitor_visit_vec3(visitor, "RotationPivot", &node->rotation_pivot);
	result &= de_object_visitor_visit_vec3(visitor, "ScalingOffset", &node->scaling_offset);
	result &= de_object_visitor_visit_vec3(visitor, "ScalingPivot", &node->scaling_pivot);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "ModelResource", &node->model_resource, de_resource_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Body", &node->body, de_body_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Scene", &node->scene, de_scene_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Parent", &node->parent, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Children", node->children, de_node_visit);
	switch (node->type) {
		case DE_NODE_TYPE_BASE: break;/* handled already */
		case DE_NODE_TYPE_CAMERA: result &= de_camera_visit(visitor, &node->s.camera); break;
		case DE_NODE_TYPE_LIGHT: result &= de_light_visit(visitor, &node->s.light); break;
		case DE_NODE_TYPE_MESH: result &= de_mesh_visit(visitor, &node->s.mesh); break;
		default: de_fatal_error("unhandled node type!"); break;
	}
	if (visitor->is_reading) {
		if (node->model_resource) {
			/* acquire resource */
			de_resource_add_ref(node->model_resource);
		}
	}
	return result;
}

void de_node_set_name(de_node_t* node, const char* name) {
	de_str8_set(&node->name, name);
}

static void de_node_find_copy_of_internal(de_node_t* root, de_node_t* node, de_node_t** out) {
	if (root->original == node) {
		*out = root;		
	} else {
		for (size_t i = 0; i < root->children.size; ++i) {
			de_node_find_copy_of_internal(root->children.data[i], node, out);
		}
	}
}

de_node_t* de_node_find_copy_of(de_node_t* root, de_node_t* node) {
	de_node_t* result = NULL;
	de_node_find_copy_of_internal(root, node, &result);
	return result;
}