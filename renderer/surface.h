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

typedef struct de_vertex_weight_t {
	void* fbx_model;
	/* Handle of node affecting a vertex */
	de_node_t* node;
	/* Actual weight of a node in total transform */
	float weight;
} de_vertex_weight_t;

typedef struct de_vertex_weight_group_t {
	de_vertex_weight_t bones[4];
	size_t weight_count; /**< Actual count of bones in group */
} de_vertex_weight_group_t;

typedef struct de_vertex_weight_data_t {
	float weights[4];
} de_vertex_weight_data_t;

typedef struct de_vertex_index_data_t {
	uint8_t indices[4];
} de_vertex_index_data_t;

typedef struct de_surface_shared_data_t {
	size_t vertex_count;
	size_t vertex_capacity;
	de_vec3_t* positions;
	de_vec3_t* normals;
	de_vec2_t* tex_coords;
	de_vec4_t* tangents;
	de_vertex_weight_data_t* bone_weights;
	de_vertex_index_data_t* bone_indices;
	size_t index_count;
	size_t index_capacity;
	int* indices;
	int ref_count; /* Count of surfaces that sharing this data. */
	GLuint vertex_array_object;
	GLuint vertex_buffer;
	GLuint index_buffer;
} de_surface_shared_data_t;

/**
* @brief Surface
*
* Described as set of vertices and faces, uses single texture.
* Surface lives on GPU (both vertex and index buffer)
*/
struct de_surface_t {
	de_renderer_t* renderer;
	de_texture_t* diffuse_map; 
	de_texture_t* normal_map;
	de_texture_t* specular_map;
	bool need_upload; /**< Indicates that surface needs to be uploaded to GPU */
	DE_ARRAY_DECLARE(de_vertex_weight_group_t, vertex_weights); /**< Additional skinning data */
	DE_ARRAY_DECLARE(de_node_t*, bones); /**< List of bones that affects this surface */
	de_surface_shared_data_t* shared_data; /* Common vertices shared between instances. */
};

/**
 * @brief Allocates buffers of specified capacity. Vertex and index count remains zero.
 */
de_surface_shared_data_t* de_surface_shared_data_create(size_t vertex_capacity, size_t index_capacity);

void de_surface_shared_data_grow_vertices(de_surface_shared_data_t* data, size_t vertex_inc);

void de_surface_shared_data_grow_indices(de_surface_shared_data_t* data, size_t index_inc);

/**
 * @brief Reallocates buffers to make them exact size to store given vertex and index count.
 * In other words, reduces memory consumption of surface data.
 *
 * Notes: Its worth to allocate excess data in de_surface_shared_data_create, fill vertices and
 * indices and then shrink memory to fit. It is much faster than increasing buffer size by small
 * portions like engine arrays does.
 */
void de_surface_data_shrink_to_fit(de_surface_shared_data_t* data);

/**
 * @brief Frees all allocated memory.
 */
void de_surface_shared_data_free(de_surface_shared_data_t* data);

de_surface_t* de_surface_copy(de_surface_t* surf);

/**
* @brief Applies texture to each surface of mesh
* @param mesh pointer to mesh
* @param texture pointer to texture
*/
void de_mesh_set_texture(de_mesh_t* mesh, de_texture_t* texture);

/**
 * @brief Fills in new vertices and indices. Removes previous data
 */
void de_surface_set_data(de_surface_t* surf, de_surface_shared_data_t* data);

/**
 * @brief Applies diffuse texture to surface. Increases ref counter of texture
 * @param surf pointer to surface
 * @param tex pointer to texture
 */
void de_surface_set_diffuse_texture(de_surface_t* surf, de_texture_t* tex);

/**
 * @brief Applies normal texture to surface. Increases ref counter of texture
 * @param surf pointer to surface
 * @param tex pointer to texture
 */
void de_surface_set_normal_texture(de_surface_t* surf, de_texture_t* tex);

/**
 * @brief Applies specular texture to surface. Increases ref counter of texture
 * @param surf pointer to surface
 * @param tex pointer to texture
 */
void de_surface_set_specular_texture(de_surface_t * surf, de_texture_t* tex);

/**
* @brief Uploads buffers to GPU
* @param surf pointer to surface
*
* Buffers will be uploaded only if 'need_upload' flag is set
*/
void de_surface_upload(de_surface_t* surf);

/**
 * @brief Calculates normals
 * @param surf pointer to surface
 *
 * Note: Dumb, per-face normals generation.
 */
void de_surface_calculate_normals(de_surface_t * surf);

/**
 * @brief Prepares surface's vertices for skinning.
 *
 * Calculates correct indices of matrices for each vertex in the surface.
 */
bool de_surface_prepare_vertices_for_skinning(de_surface_t* surf);

/**
 * @brief Adds new surface affecting node.
 * @return Returns true if bone was added, false - if vertex already added.
 *
 * Notes: amortized O(n)
 */
bool de_surface_add_bone(de_surface_t* surf, de_node_t* bone);

/**
 * @brief Returns bone index in array of surface's bones. If no bone was found, returns -1.
 *
 * Notes: O(n)
 */
int de_surface_get_bone_index(de_surface_t* surf, de_node_t* bone);

/**
 * @brief Fills matrices for each bone. Matrices array will be filled so each vertex will
 * have correct index of matrix.
 */
void de_surface_get_skinning_matrices(de_surface_t* surf, de_mat4_t* out_matrices, size_t max_matrices);

/**
 * @brief Returns true if surface is skinned.
 */
bool de_surface_is_skinned(de_surface_t* surf);

/**
 * @brief Computes tangents for surface vertices.
 *
 * Based on Lengyel, Eric.
 * "Computing Tangent Space Basis Vectors for an Arbitrary Mesh".
 * Terathon Software, 2001. http://terathon.com/code/tangent.html
 */
void de_surface_calculate_tangents(de_surface_t* surf);

/* TODO */
void de_surface_make_cube(de_surface_t* surf);

/* TODO */
void de_surface_make_sphere(de_surface_t* surf, int slices, int stacks, float r);