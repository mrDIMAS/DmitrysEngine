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

//
// Resolving resource dependicies on loading of a save.
// 
// Case1: Changes in graphical data (meshes, textures and so on). Solution: find corresponding
// nodes in resource by names of nodes and extract data from them. In case when corresponding 
// node not found in resource - do nothing.
//                                             +
//     Scene graph in save file                |                Resource
//                                             |
//           +--------+                        |              +--------+
//           |        |                        |              |        |
//           |  Node1 |  <----------------------------------+ |  Node1 |
//           |        |                        |              |        |
//           +--------+                        |              +--------+
//              |  |                           |                 |  |
//     +-------------------------------------------------+       |  |
//     |        |  |                           |         |       |  |
// +---v-----<--+  +-->---------+              |    +---------<--+  +-->---------+
// |        |          |        |              |    |        |          |        |
// |  Node5 |          |  Node2 <-----+        |    |  Node5 |    +-----+  Node2 |
// |        |          |        |     |        |    |        |    |     |        |
// +--------+          +--------+     |        |    +--------+    |     +--------+
//                        |  |        +---------------------------+        |  |
//                        |  |                 |                           |  |
//           +---------<--+  +-->---------+    |              +---------<--+  +-->---------+
//           |        |          |        |    |              |        |          |        |
//           |  Node4 |          |  Node3 |    |       +------+  Node4 |          |  Node3 |
//           |        |          |        |    |       |      |        |          |        |
//           +---^----+          +---^----+    |       |      +--------+          +--------+
//               |                   |         |       |                               |
//               +-------------------------------------+                               |
//                                   |         |             Resolve by name           |
//                                   |         |                                       |
//                                   +-------------------------------------------------+
//                                             |
//                                             +
//
// Case 2: Hierarchy changes. Solution: find corresponding parents for each nodes in resource
// and reattach nodes in scene graph accordingly. When parent node was deleted in resource -
// do nothing.
//
//                                            +
//    Scene graph in save file                |   Resource with changed hierarchy (Node4 and Node3)
//                                            |
//          +--------+                        |              +--------+
//          |        |                        |              |        |
//          |  Node1 |  <----------------------------------+ |  Node1 |
//          |        |                        |              |        |
//          +--------+                        |              +--------+
//             |  |                           |                 |  |
//    +-------------------------------------------------+       |  |
//    |        |  |                           |         |       |  |
//+---v-----<--+  +-->---------+              |    +---------<--+  +-->---------+
//|        |          |        |              |    |        |          |        |
//|  Node5 |          |  Node2 <-----+        |    |  Node5 |    +-----+  Node2 |
//|        |          |        |     |        |    |        |    |     |        |
//+--------+          +--------+     |        |    +--------+    |     +--------+
//                       |  |        +---------------------------+
//                       |  |                 |        |
//          +---------<--+  +-->---------+    |        +---->---------+          +--------+
//          |        |          |        |    |              |        |          |        |
//          |  Node4 |          |  Node3 |    |       +------+  Node4 +--------->+  Node3 |
//          |        |          |        |    |       |      |        |          |        |
//          +---^----+          +---^----+    |       |      +--------+          +--------+
//              |                   |         |       |                               |
//              +-------------------------------------+                               |
//                                  |         |             Resolve by name           |
//                                  |         |                                       |
//                                  +-------------------------------------------------+
//                                            |
//                                            +
//    Hierarchy changes will be resolved by node names, so old hierarchy Node2->Node4 will become
//    Node5->Node4 and Node2->Node3 will become Node4->Node3.
//

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

	if (node->dispatch_table->free) {
		node->dispatch_table->free(node);
	}

	if (node->model_resource) {
		de_resource_release(node->model_resource);
		node->model_resource = NULL;
	}
	de_free(node);
}

static de_node_dispatch_table_t* de_node_get_dispatch_table_by_type(de_node_type_t type) {
	static de_node_dispatch_table_t stub_table;
	switch (type) {
		case DE_NODE_TYPE_BASE: return &stub_table;
		case DE_NODE_TYPE_CAMERA: return de_camera_get_dispatch_table();
		case DE_NODE_TYPE_MESH: return de_mesh_get_dispatch_table();
		case DE_NODE_TYPE_LIGHT: return de_light_get_dispatch_table();
        default:
            de_log("unhandled node type!");
            break;
	}
	
	return NULL;
}

de_node_t* de_node_create(de_scene_t* scene, de_node_type_t type) {
	de_node_t* node = DE_NEW(de_node_t);
	node->dispatch_table = de_node_get_dispatch_table_by_type(type);
	node->type = type;
	node->scene = scene;
	de_mat4_identity(&node->global_matrix);
	de_mat4_identity(&node->local_matrix);
	node->scale = (de_vec3_t) { 1, 1, 1 };
	node->rotation = (de_quat_t) { 0, 0, 0, 1 };
	node->pre_rotation = (de_quat_t) { 0, 0, 0, 1 };
	node->post_rotation = (de_quat_t) { 0, 0, 0, 1 };
	if(node->dispatch_table->init) {
		node->dispatch_table->init(node);
	}
	return node;
}

static de_node_t* de_node_copy_internal(de_scene_t* dest_scene, de_node_t* node) {
	de_node_t* copy = DE_NEW(de_node_t);
	copy->dispatch_table = node->dispatch_table;
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
	if(node->dispatch_table->copy) {
		node->dispatch_table->copy(node, copy);
	}
	de_node_calculate_transforms(copy);
	return copy;
}

void de_node_resolve(de_node_t* node) {
	/* resolve hierarchy changes */
	de_node_t* original = node->original;
	if (original && original->parent && node->parent) {
		/* parent has changed if names of parents in both model resource and
		* current node are different */
		if (!de_str8_is_empty(&node->parent->name) && !de_str8_is_empty(&original->parent->name)) {
			if (!de_str8_eq_str8(&node->parent->name, &original->parent->name)) {
				/* search will be performed from root of instantiated node */
				de_node_t* root = de_mesh_get_model_root(node);
				/* find new parent */
				de_node_t* new_parent = de_node_find(root, de_str8_cstr(&original->parent->name));
				/* resolve hierarchy */
				de_node_attach(node, new_parent);
			}
		}
	}

	/* todo: resolve transform changes */

	if(node->dispatch_table->resolve) {
		node->dispatch_table->resolve(node);
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
	if (node->body) {
		de_body_get_position(node->body, &node->position);
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

	de_mat4_t pre_rotation;
	de_mat4_rotation(&pre_rotation, &node->pre_rotation);

	de_mat4_t post_rotation;
	de_mat4_rotation(&post_rotation, &node->post_rotation);
	de_mat4_inverse(&post_rotation, &post_rotation);

	de_mat4_t rotation;
	de_mat4_rotation(&rotation, &node->rotation);

	de_mat4_t scale;
	de_mat4_scale(&scale, &node->scale);

	de_mat4_t translation;
	de_mat4_translation(&translation, &node->position);

	de_mat4_t rotation_offset;
	de_mat4_translation(&rotation_offset, &node->rotation_offset);

	de_mat4_t rotation_pivot;
	de_mat4_translation(&rotation_pivot, &node->rotation_pivot);

	de_mat4_t rotation_pivot_inv;
	de_mat4_inverse(&rotation_pivot_inv, &rotation_pivot);

	de_mat4_t scale_offset;
	de_mat4_translation(&scale_offset, &node->scaling_offset);

	de_mat4_t scale_pivot;
	de_mat4_translation(&scale_pivot, &node->scaling_pivot);

	de_mat4_t scale_pivot_inv;
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

	if (node->parent) {
		de_mat4_mul(&node->global_matrix, de_node_calculate_transforms(node->parent), &node->local_matrix);
	} else {
		node->global_matrix = node->local_matrix;
	}

	return &node->global_matrix;
}

void de_node_get_look_vector(const de_node_t* node, de_vec3_t* look) {
	DE_ASSERT(node);
	de_mat4_look(&node->global_matrix, look);
}

void de_node_get_up_vector(const de_node_t* node, de_vec3_t* up) {
	DE_ASSERT(node);
	DE_ASSERT(up);
	de_mat4_up(&node->global_matrix, up);
}

void de_node_get_side_vector(const de_node_t* node, de_vec3_t* side) {
	DE_ASSERT(node);
	DE_ASSERT(side);
	de_mat4_side(&node->global_matrix, side);
}

void de_node_get_global_position(const de_node_t* node, de_vec3_t* pos) {
	DE_ASSERT(node);
	DE_ASSERT(pos);
	pos->x = node->global_matrix.f[12];
	pos->y = node->global_matrix.f[13];
	pos->z = node->global_matrix.f[14];
}

void de_node_set_local_position(de_node_t* node, de_vec3_t* pos) {
	DE_ASSERT(node);
	DE_ASSERT(pos);
	de_body_t* body = node->body;
	if (body) {
		de_body_set_position(body, pos);
	} else {
		node->position = *pos;
	}
}

void de_node_set_body(de_node_t* node, de_body_t* body) {
	DE_ASSERT(node);
	node->body = body;
}

void de_node_move(de_node_t* node, de_vec3_t* offset) {
	DE_ASSERT(node);
	DE_ASSERT(offset);
	de_vec3_add(&node->position, &node->position, offset);
}

void de_node_set_local_rotation(de_node_t* node, de_quat_t* rot) {
	DE_ASSERT(node);
	DE_ASSERT(rot);
	node->rotation = *rot;
}

void de_node_set_local_scale(de_node_t* node, de_vec3_t* scl) {
	DE_ASSERT(node);
	DE_ASSERT(scl);
	node->scale = *scl;
}

de_node_t* de_node_find(de_node_t* node, const char* name) {
	DE_ASSERT(node);
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
	DE_ASSERT(node);
	DE_ASSERT(node->type == DE_NODE_TYPE_MESH);
	return &node->s.mesh;
}

de_node_t* de_node_from_mesh(de_mesh_t* mesh) {
	DE_ASSERT(mesh);
	return (de_node_t*)((char*)mesh - offsetof(de_node_t, s.mesh));
}

de_light_t* de_node_to_light(de_node_t* node) {
	DE_ASSERT(node);
	DE_ASSERT(node->type == DE_NODE_TYPE_LIGHT);
	return &node->s.light;
}

de_node_t* de_node_from_light(de_mesh_t* light) {
	DE_ASSERT(light);
	return (de_node_t*)((char*)light - offsetof(de_node_t, s.light));
}

de_camera_t* de_node_to_camera(de_node_t* node) {
	DE_ASSERT(node);
	DE_ASSERT(node->type == DE_NODE_TYPE_CAMERA);
	return &node->s.camera;
}

de_node_t* de_node_from_camera(de_camera_t* camera) {
	DE_ASSERT(camera);
	return (de_node_t*)((char*)camera - offsetof(de_node_t, s.camera));
}

bool de_node_visit(de_object_visitor_t* visitor, de_node_t* node) {
	DE_ASSERT(node);
	bool result = true;
	result &= de_object_visitor_visit_uint32(visitor, "Type", (uint32_t*)&node->type);
	if (visitor->is_reading) {
		/* restore dispatch table */
		node->dispatch_table = de_node_get_dispatch_table_by_type(node->type);
	}
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
	if(node->dispatch_table->visit) {
		node->dispatch_table->visit(visitor, node);
	}
	if(visitor->is_reading) {
		if(node->model_resource) {
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