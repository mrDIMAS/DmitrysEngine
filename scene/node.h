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

typedef enum de_node_type_t {
	DE_NODE_TYPE_BASE,
	DE_NODE_TYPE_LIGHT,
	DE_NODE_TYPE_MESH,
	DE_NODE_TYPE_CAMERA
} de_node_type_t;

#if DE_DISABLE_ASSERTS
#  define DE_ASSERT_SCENE_NODE_TYPE(node, expected_type)
#else
#  define DE_ASSERT_SCENE_NODE_TYPE(node, expected_type) \
	if(node->type != expected_type) de_fatal_error("Scene node must be " #expected_type " type!")
#endif

typedef struct de_node_dispatch_table_t {
	void(*deinit)(de_node_t* node);
	void(*copy)(de_node_t* node, de_node_t* dest);
} de_node_dispatch_table_t;

/**
 * @class de_node_t
 * @brief Common scene node. Typed union.
 *
 * Actual behaviour of scene node defined by its actual type.
 *
 * Important note: please consider using special functions for nodes, instead of
 * directly accessing fields of structure. Suddenly internals could be changed
 * and your game will not be compiling!
 */
struct de_node_t {
	/**
	 * Standard properties
	 */
	de_node_type_t type;
	de_str8_t name;
	de_scene_t* scene;
	de_mat4_t local_matrix; /**< Matrix of local transform of the node. Read-only. */
	de_mat4_t global_matrix; /**< Matrix of global transform of the node. Read-only. */
	de_mat4_t inv_bind_pose_matrix; /**< Matrix for meshes with skeletal animaton */
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

	bool need_update; /**< Indicates that node's transform needs to be updated */
	de_node_h parent; /**< Pool reference to parent node */
	DE_ARRAY_DECLARE(de_node_h, children); /**< Array of pool references to child nodes */
	bool visible; /**< Local visibility. Actual visibility defined by hierarchy. So if parent node is invisible, then child node will be too */

	void* user_data; /**< Non-serializable. */
	bool is_bone;

	/* Physics */
	de_body_h body;

	de_resource_t* resource; /**<  */

	/* Specialization. Avoid accessing these directly, use de_node_to_xxx instead. */
	union {
		de_mesh_t mesh;
		de_light_t light;
		de_camera_t camera;
	} s;

	de_node_dispatch_table_t* dispatch_table;

	DE_LINKED_LIST_ITEM(struct de_node_t);
};

void de_node_clear_pool(void);

de_node_t* de_node_get_ptr(de_node_h handle);

/**
* @brief Creates new scene node. You should call de_scene_add_node to interact with node.
* @return Pointer to new scene node
*/
de_node_h de_node_create(de_scene_t* scene);

/**
 * @brief Internal. 
 */
de_node_h de_node_alloc(de_scene_t* scene, de_node_type_t type, de_node_dispatch_table_t* tbl);

de_node_h de_node_copy(de_scene_t* dest_scene, de_node_h node_handle);

/**
* @brief Frees scene node.
* @param node node reference
*
* All child nodes will be also freed. Automatically detaches self from scene.
*/
void de_node_free(de_node_h handle);

/**
* @brief Attaches node to parent scene node
* @param node reference to node that will be attached to @parent
* @param parent reference to parent node
*/
void de_node_attach(de_node_h node_handle, de_node_h parent_handle);

/**
 * @brief Detaches node from current parent.
 */
void de_node_detach(de_node_h node_handle);

/**
* @brief Calculates local and global transforms. Global transform takes into account node hierarchy
* @param node pointer to node
*/
de_mat4_t* de_node_calculate_transforms(de_node_t* node);

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
* @brief Sets local position of node. Sets "need_update" flag
* @param node pointer to node
*/
void de_node_set_local_position_xyz(de_node_t* node, float x, float y, float z);

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
* @brief Sets current physical body for node.
* @param node
*/
void de_node_set_body(de_node_t* node, de_body_h body);

/**
* @brief Performs recursive search on tree to find a specified node.
* @param root Pointer to node from which you want to start searching.
* @param name Name of node you looking for.
* @return
*/
de_node_t* de_node_find(de_node_t* node, const char* name);

/**
 * @brief Tries to get mesh component out of the node, will throw an error if node is not a mesh!
 */
de_mesh_t* de_node_to_mesh(de_node_t* node);

/**
 * @brief Tries to get light component out of the node, will throw an error if node is not a light!
 */
de_light_t* de_node_to_light(de_node_t* node);

/**
 * @brief Tries to get camera component out of the node, will throw an error if node is not a camera!
 */
de_camera_t* de_node_to_camera(de_node_t* node);

/**
 * @brief Serializes scene node.
 */
bool de_node_visit(de_object_visitor_t* visitor, de_node_t* node);

void de_node_set_name(de_node_t* node, const char* name);