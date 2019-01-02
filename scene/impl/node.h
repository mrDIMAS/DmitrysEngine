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

/*=======================================================================================*/
void de_node_free(de_node_t* node)
{
	size_t i;

	if (node->scene)
	{
		de_scene_remove_node(node->scene, node);
	}

	de_free(node->name);

	switch (node->type)
	{
	case DE_NODE_BASE:
		break;
	case DE_NODE_MESH:
		de_mesh_deinit(&node->s.mesh);
		break;
	case DE_NODE_CAMERA:
		de_camera_deinit(&node->s.camera);
		break;
	case DE_NODE_LIGHT:
		de_light_deinit(&node->s.light);
		break;
	}

	/* Free children */
	for (i = 0; i < node->children.size; ++i)
	{
		de_node_free(node->children.data[i]);
	}
	DE_ARRAY_FREE(node->children);


	de_free(node);
}

/*=======================================================================================*/
de_node_t* de_node_create(de_scene_t* scene, de_node_type_t type)
{
	de_node_t* node = DE_NEW(de_node_t);

	node->type = type;
	node->scene = scene;
	de_mat4_identity(&node->global_matrix);
	de_mat4_identity(&node->local_matrix);
	de_vec3_set(&node->scale, 1, 1, 1);
	de_quat_set(&node->rotation, 0, 0, 0, 1);
	de_quat_set(&node->pre_rotation, 0, 0, 0, 1);
	de_quat_set(&node->post_rotation, 0, 0, 0, 1);

	switch (type)
	{
	case DE_NODE_BASE:
		break;
	case DE_NODE_MESH:
	{
		de_mesh_init(node, &node->s.mesh);
		break;
	}
	case DE_NODE_CAMERA:
	{
		de_camera_init(node, &node->s.camera);
		break;
	}
	case DE_NODE_LIGHT:
	{
		de_light_t* light = &node->s.light;
		de_light_init(light);
		light->parent_node = node;
		break;
	}
	}

	return node;
}

/*=======================================================================================*/
void de_node_attach(de_node_t* node, de_node_t* parent)
{
	if (!node || !parent)
	{
		return;
	}

	DE_ARRAY_APPEND(parent->children, node);
	node->parent = parent;
}

/*=======================================================================================*/
void de_node_calculate_transforms(de_node_t* node)
{
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

	if (!node)
	{
		return;
	}

	if (node->body)
	{
		node->position = node->body->position;
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

	if (node->parent)
	{
		de_mat4_mul(&node->global_matrix, &node->parent->global_matrix, &node->local_matrix);
	}
	else
	{
		node->global_matrix = node->local_matrix;
	}
}

/*=======================================================================================*/
void de_node_get_look_vector(de_node_t* node, de_vec3_t* look)
{
	if (!node || !look)
	{
		return;
	}

	de_mat4_look(&node->global_matrix, look);
}

/*=======================================================================================*/
void de_node_get_up_vector(de_node_t* node, de_vec3_t* up)
{
	if (!node || !up)
	{
		return;
	}

	de_mat4_up(&node->global_matrix, up);
}

/*=======================================================================================*/
void de_node_get_side_vector(de_node_t* node, de_vec3_t* side)
{
	if (!node || !side)
	{
		return;
	}

	de_mat4_side(&node->global_matrix, side);
}

/*=======================================================================================*/
void de_node_get_global_position(de_node_t* node, de_vec3_t* pos)
{
	if (!node || !pos)
	{
		return;
	}

	pos->x = node->global_matrix.f[12];
	pos->y = node->global_matrix.f[13];
	pos->z = node->global_matrix.f[14];
}

/*=======================================================================================*/
void de_node_set_local_position(de_node_t* node, de_vec3_t* pos)
{
	if (!node || !pos)
	{
		return;
	}

	if (node->body)
	{
		de_body_set_position(node->body, pos);
	}
	else
	{
		node->position = *pos;
	}
}

/*=======================================================================================*/
void de_node_move(de_node_t* node, de_vec3_t* offset)
{
	if (!node || !offset)
	{
		return;
	}

	de_vec3_add(&node->position, &node->position, offset);
}

/*=======================================================================================*/
void de_node_set_local_rotation(de_node_t* node, de_quat_t* rot)
{
	if (!node || !rot)
	{
		return;
	}

	node->rotation = *rot;
}

/*=======================================================================================*/
void de_node_set_local_scale(de_node_t* node, de_vec3_t* scl)
{
	if (!node || !scl)
	{
		return;
	}

	node->scale = *scl;
}

/*=======================================================================================*/
void de_node_set_body(de_node_t* node, de_body_t* body)
{
	if (!node)
	{
		return;
	}

	node->body = body;
}

/*=======================================================================================*/
de_node_t* de_node_find(de_node_t* node, const char* name)
{
	de_node_t* result = NULL;
	size_t i;

	if (node)
	{
		if (node->name && strcmp(node->name, name) == 0)
		{
			result = node;
		}
		else
		{
			for (i = 0; i < node->children.size; ++i)
			{
				result = de_node_find(node->children.data[i], name);

				if (result)
				{
					break;
				}
			}
		}
	}

	return result;
}
