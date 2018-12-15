/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

typedef enum de_node_type_t
{
	DE_NODE_BASE,
	DE_NODE_LIGHT,
	DE_NODE_MESH,
	DE_NODE_CAMERA
} de_node_type_t;

/**
 * @class de_node_t
 * @brief Common scene node. Typed union.
 *
 * Actual behaviour of scene node defined by set of its components
 */
struct de_node_t
{
	DE_LINKED_LIST_ITEM(struct de_node_t);

	de_scene_t* parent_scene;
	de_node_type_t type;

	/**
	 * Standard properties
	 */
	char* name;
	de_mat4_t local_matrix; /**< Matrix of local transform of the node. Read-only. */
	de_mat4_t global_matrix; /**< Matrix of global transform of the node. Read-only. */

	de_vec3_t position; /**< Position of the node relative to parent node (if exists) */
	de_vec3_t scale; /**< Scale of the node relative to parent node (if exists) */
	de_quat_t rotation; /**< Rotation of the node relative to parent node (if exists) */

	/**
	 * FBX stuff, but can be used if you want special transformations
	 */
	de_quat_t pre_rotation; 
	de_quat_t post_rotation;
	de_vec3_t rotation_offset;
	de_vec3_t rotation_pivot;
	de_vec3_t scaling_offset;
	de_vec3_t scaling_pivot;

	de_bool_t need_update; /**< Indicates that node's transform needs to be updated */
	de_node_t* parent; /**< Pool reference to parent node */
	DE_ARRAY_DECLARE(de_node_t*, children); /**< Array of pool references to child nodes */
	de_bool_t visible; /**< Local visibility. Actual visibility defined by hierarchy. So if parent node is invisible, then child node will be too */
	
	/* Physics */
	de_body_t* body;

	/* Specialization */
	union
	{
		de_mesh_t mesh;
		de_light_t light;
		de_camera_t camera;
	} s;
};

/**
* @brief Creates new scene node. You should call de_scene_add_node to interact with node.
* @return Pointer to new scene node
*/
de_node_t* de_node_create(de_node_type_t type);

/**
* @brief Frees scene node.
* @param node node reference
*
* All child nodes will be also freed. Automatically detaches self from scene.
*/
void de_node_free(de_node_t* node);

/**
* @brief Attaches node to parent scene node
* @param node reference to node that will be attached to @parent
* @param parent reference to parent node
*/
void de_node_attach(de_node_t* node, de_node_t* parent);

/**
* @brief Calculates local and global transforms. Global transform takes into account node hierarchy
* @param node pointer to node
*/
void de_node_calculate_transforms(de_node_t* node);

/**
* @brief Writes out look vector from node's global transform
* @param node pointer to node
* @param look pointer to look vector
*/
void de_node_get_look_vector(de_node_t* node, de_vec3_t* look);

/**
* @brief Writes out up vector from node's global transform
* @param node pointer to node
* @param up pointer to up vector
*/
void de_node_get_up_vector(de_node_t* node, de_vec3_t* up);

/**
* @brief Writes out side vector from node's global transform
* @param node pointer to node
* @param side pointer to side vector
*/
void de_node_get_side_vector(de_node_t* node, de_vec3_t* side);

/**
* @brief Writes out node global position
* @param node pointer to node
* @param pos pointer to position
*/
void de_node_get_global_position(de_node_t* node, de_vec3_t* pos);

/**
* @brief Sets local position of node. Sets "need_update" flag
* @param node pointer to node
* @param pos new position of node
*/
void de_node_set_local_position(de_node_t* node, de_vec3_t* pos);

/**
* @brief Sets local rotation of node. Sets "need_update" flag
* @param node pointer to node
* @param rot new rotation of node
*/
void de_node_set_local_rotation(de_node_t* node, de_quat_t* rot);

/**
* @brief Sets local scale of node. Sets "need_update" flag
* @param node pointer to node
* @param scl new scale of node
*/
void de_node_set_local_scale(de_node_t* node, de_vec3_t* scl);

/**
* @brief Moves node by given offset
* @param node pointer to node
* @param offset offset
*/
void de_node_move(de_node_t* node, de_vec3_t* offset);

/**
* @brief
* @param node
*/
void de_node_set_body(de_node_t* node, de_body_t* body);

/**
* @brief
* @param root
* @param name
* @return
*/
de_node_t* de_node_find(de_node_t* node, const char* name);
