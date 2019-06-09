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
	DE_NODE_TYPE_CAMERA,
	DE_NODE_TYPE_PARTICLE_SYSTEM
} de_node_type_t;

/**
 * @brief Node dispatch table (vtable). Works together in combination with tagged unions.
 */
typedef struct de_node_dispatch_table_t {
	void(*init)(de_node_t* n);
	void(*free)(de_node_t* n);
	bool(*visit)(de_object_visitor_t* visitor, de_node_t* n);
	void(*resolve)(de_node_t* n);
	void(*copy)(de_node_t* src, de_node_t* dst);
} de_node_dispatch_table_t;

typedef enum de_transform_flags_t {
	DE_TRANSFORM_FLAGS_LOCAL_TRANSFORM_NEED_UPDATE = DE_BIT(0),
	DE_TRANSFORM_FLAGS_ROTATION_PIVOT_NEED_UPDATE = DE_BIT(1),
	DE_TRANSFORM_FLAGS_SCALE_PIVOT_NEED_UPDATE = DE_BIT(2),
	DE_TRANSFORM_FLAGS_POST_ROTATION_NEED_UPDATE = DE_BIT(3)
} de_transform_flags_t;

typedef enum de_node_flags_t {
	DE_NODE_FLAGS_IS_BONE = DE_BIT(0), /**< Indicates that a node is a bone node. Read-only. */
	DE_NODE_FLAGS_DISABLE_LIGHT = DE_BIT(1) /**< Indicates that a node should not be lit up. Read-write. */
} de_node_flags_t;

/**
 * @class de_node_t
 * @brief Common scene node. Tagged union.
 *
 * Actual behaviour of scene node defined by its actual type.
 *
 * Important note: please consider using special functions for nodes, instead of
 * directly accessing fields of structure. Suddenly internals could be changed
 * and your game will not be compiling!
 */
struct de_node_t {
	de_node_type_t type;
	de_str8_t name;
	de_scene_t* scene;
	de_mat4_t local_matrix; /**< Matrix of local transform of the node. Read-only. */
	de_mat4_t global_matrix; /**< Matrix of global transform of the node. Read-only. */
	de_mat4_t inv_bind_pose_matrix; /**< Matrix for meshes with skeletal animation */
	de_vec3_t position; /**< Position of the node relative to parent node (if exists) */
	de_vec3_t scale; /**< Scale of the node relative to parent node (if exists) */
	de_quat_t rotation; /**< Rotation of the node relative to parent node (if exists) */
	de_quat_t pre_rotation;
	de_quat_t post_rotation;
	de_vec3_t rotation_offset;
	de_vec3_t rotation_pivot;
	de_vec3_t scaling_offset;
	de_vec3_t scaling_pivot;
	/* Cached matrices - its too inefficient to compute three inverse matrices every frame for each node. */
	de_mat4_t m_rotation_pivot;
	de_mat4_t m_rotation_pivot_inv;
	de_mat4_t m_post_rotation;
	de_mat4_t m_scale_pivot;
	de_mat4_t m_scale_pivot_inv;
	de_aabb_t bounding_box; /**< Local bounding box of a node. */
	de_transform_flags_t transform_flags;
	de_node_t* parent; /**< Pointer to parent node */
	DE_ARRAY_DECLARE(de_node_t*, children); /**< Array of pointers to child nodes */
	bool local_visibility; /**< Local visibility. Actual visibility defined by hierarchy. So if parent node is invisible, then child node will be too */
	bool global_visibility;
	de_node_t* original; /**< Pointer to original node in resource from which this node copied. */
	void* user_data; /**< Pointer to any data you want the node to hold. Non-serializable. */
	de_node_flags_t flags;
	de_body_t* body;
	float depth_hack; /**< Depth hack value for node, will be used to hack projection matrix on render. Used to make object render on-top of other. */
	de_resource_t* model_resource; /**< Pointer to model from which this node was instantiated.  */
	de_node_dispatch_table_t* dispatch_table;
	DE_LINKED_LIST_ITEM(struct de_node_t);
	/* Specialization. Avoid accessing these directly, use de_node_to_xxx instead. */
	union {
		de_mesh_t mesh;
		de_light_t light;
		de_camera_t camera;
		de_particle_system_t particle_system;
	} s;
};

/**
* @brief Creates new scene node. You should call de_scene_add_node to interact with node.
* @return Pointer to new scene node
*/
de_node_t* de_node_create(de_scene_t* scene, de_node_type_t type);

/**
 * @brief Copies a node with every children to destination scene.
 */
de_node_t* de_node_copy(de_scene_t* dest_scene, de_node_t* node_handle);

/**
* @brief Frees scene node.
* @param node node reference
*
* All child nodes will be also freed. Automatically detaches self from scene.
*/
void de_node_free(de_node_t* handle);

/**
* @brief Attaches node to parent scene node
* @param node reference to node that will be attached to @parent
* @param parent reference to parent node
*/
void de_node_attach(de_node_t* node_handle, de_node_t* parent_handle);

/**
 * @brief Detaches node from current parent.
 */
void de_node_detach(de_node_t* node_handle);

/**
 * @brief Calculates local transform of a node.
 */
void de_node_calculate_local_transform(de_node_t* node);

/**
 * @brief Calculates local and global transforms. Global transform takes into account node hierarchy.
 * Normally owning scene will calculate transforms of each node recursively, by calling
 * @ref de_node_calculate_transforms_descending on root nodes but you can calculate transform of a node
 * directly. This function recursively calculates transform of *each* node in hierarchy, so
 * with this chain: Node1 -> Node2 -> Node3 -> Node4, global transforms of each node will be calculated
 * starting from Node1.
 *
 * @param node pointer to node
 */
de_mat4_t* de_node_calculate_transforms_ascending(de_node_t* node);

/**
 * @brief Calculates local and global transform. Also calculates transforms of child nodes recursively,
 * descending by tree. Unlike @ref de_node_calculate_transforms_ascending, this functions going *down* on tree, so
 * if called with root node a param then transforms of each node in tree will be calculated.
 */
void de_node_calculate_transforms_descending(de_node_t* node);

/**
* @brief Writes out look vector from node's global transform
* @param node pointer to node
* @param look pointer to look vector
*/
void de_node_get_look_vector(const de_node_t* node, de_vec3_t* look);

/**
* @brief Writes out up vector from node's global transform
* @param node pointer to node
* @param up pointer to up vector
*/
void de_node_get_up_vector(const de_node_t* node, de_vec3_t* up);

/**
* @brief Writes out side vector from node's global transform
* @param node pointer to node
* @param side pointer to side vector
*/
void de_node_get_side_vector(const de_node_t* node, de_vec3_t* side);

/**
* @brief Writes out node global position
* @param node pointer to node
* @param pos pointer to position
*/
void de_node_get_global_position(const de_node_t* node, de_vec3_t* pos);

/**
 * @brief Internal. Resolves type-specific dependencies after copying or deserialization.
 */
void de_node_resolve(de_node_t* node);

/**
* @brief Sets local position of node. Sets "need_update" flag
* @param node pointer to node
* @param pos new position of node
*/
void de_node_set_local_position(de_node_t* node, const de_vec3_t* pos);

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
void de_node_set_body(de_node_t* node, de_body_t* body);

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
 * @brief Restores node pointer by given mesh pointer.
 */
de_node_t* de_node_from_mesh(de_mesh_t* mesh);

/**
 * @brief Tries to get light component out of the node, will throw an error if node is not a light!
 */
de_light_t* de_node_to_light(de_node_t* node);

/**
 * @brief Restores node pointer by given light pointer.
 */
de_node_t* de_node_from_light(de_light_t* light);

/**
 * @brief Tries to get camera component out of the node, will throw an error if node is not a camera!
 */
de_camera_t* de_node_to_camera(de_node_t* node);

/**
 * @brief Restores node pointer by given camera pointer.
 */
de_node_t* de_node_from_camera(de_camera_t* camera);

/**
 * @brief Tries to get particle system component out of the node, will throw an error if node is not a particle system!
 */
de_node_t* de_node_from_particle_system(de_particle_system_t* ps);

/**
 * @brief Restores node pointer by given particle system pointer.
 */
de_particle_system_t* de_node_to_particle_system(de_node_t* node);

/**
 * @brief Serializes scene node.
 */
bool de_node_visit(de_object_visitor_t* visitor, de_node_t* node);

/**
 * @brief Sets new name for a node.
 */
void de_node_set_name(de_node_t* node, const char* name);

/**
 * @brief Tries to find a copy of specified node in whole hierarchy of other node defined by root.
 */
de_node_t* de_node_find_copy_of(de_node_t* root, de_node_t* node);

/**
 * @brief Sets value which will be used to modify projection matrix when rendering a node.
 * Very useful for object which should be rendered on-top of other, like weapons in first person shooters.
 * Typical value is 0.1.
 */
void de_node_set_depth_hack(de_node_t* node, float value);

/**
 * @brief Returns current depth hack values used by node.
 */
float de_node_get_depth_hack(de_node_t* node);

void de_node_set_local_visibility(de_node_t* node, bool visibility);

bool de_node_get_local_visibility(de_node_t* node);

bool de_node_get_global_visibility(de_node_t* node);

void de_node_calculate_visibility_descending(de_node_t* node);

void de_node_set_pre_rotation(de_node_t* node, const de_quat_t* q);

void de_node_get_pre_rotation(de_node_t* node, de_quat_t* q);

void de_node_set_post_rotation(de_node_t* node, const de_quat_t* q);

void de_node_get_post_rotation(de_node_t* node, de_quat_t* q);

void de_node_set_rotation_offset(de_node_t* node, const de_vec3_t* v);

void de_node_get_rotation_offset(de_node_t* node, de_vec3_t* v);

void de_node_set_rotation_pivot(de_node_t* node, const de_vec3_t* v);

void de_node_get_rotation_pivot(de_node_t* node, de_vec3_t* v);

void de_node_set_scaling_offset(de_node_t* node, const de_vec3_t* v);

void de_node_get_scaling_offset(de_node_t* node, de_vec3_t* v);

void de_node_set_scaling_pivot(de_node_t* node, const de_vec3_t* v);

/**
 * @brief Returns scaling pivot of a node.
 */
void de_node_get_scaling_pivot(de_node_t* node, de_vec3_t* v);

/**
 * @brief Resets flag indicating that current local and global transform are valid, thus
 * forcing update of transforms on next frame (or current if called before @ref de_scene_update).
 */
void de_node_invalidate_transforms(de_node_t* node);

/**
 * @brief Returns pointer to scene specified node belongs to.
 */
de_scene_t* de_node_get_scene(de_node_t* node);

void de_node_get_bounding_box(de_node_t* node, de_aabb_t* bounding_box);