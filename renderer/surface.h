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

typedef struct de_bone_proxy_t
{
	/* Pointer to real bone node (de_node_t) which affects vertex. It is multifunctional pointer.
	 * For example in FBX loader at first step it contains pointer to fbx model (de_fbx_model_t)
	 * after conversion it will contain pointer to bone node (de_node_t) */
	void* node;
	/* Actual weight of a node in total transform */
	float weight;
} de_bone_proxy_t;

typedef struct de_vertex_bone_group_t
{	
	de_bone_proxy_t bones[4];	
	size_t bone_count; /**< Actual count of bones in group */
} de_vertex_bone_group_t;

/**
* @brief Surface
*
* Described as set of vertices and faces, uses single texture.
* Surface lives on GPU (both vertex and index buffer)
*/
struct de_surface_t
{
	de_renderer_t* renderer;
	de_texture_t* texture; /**< Pointer to texture */
	DE_ARRAY_DECLARE(de_vertex_t, vertices);   /**< Array of vertices, will be directly uploaded to GPU */
	DE_ARRAY_DECLARE(int, indices);    /**< Array of faces */
	GLuint vbo;      /**< Vertex buffer object id */
	GLuint vao;      /**< Vertex array object id */
	GLuint ebo;      /**< Element buffer object id */
	de_bool_t need_upload;    /**< Indicates that surface needs to be uploaded to GPU */
	DE_ARRAY_DECLARE(de_vertex_bone_group_t, skinning_data); /**< Additional skinning data */
	DE_ARRAY_DECLARE(de_node_t*, bones); /**< List of bones that affects this surface */
};

/**
* @brief Applies texture to each surface of mesh
* @param mesh pointer to mesh
* @param texture pointer to texture
*/
void de_mesh_set_texture(de_mesh_t* mesh, de_texture_t* texture);

/**
 * @brief Fills in new vertices and indices. Removes previous data
 */
void de_surface_load_data(de_surface_t* surf, de_vertex_t* vertices, size_t vertex_count, int* indices, size_t index_count);

/**
* @brief Applies texture to surface. Increases ref counter of texture
* @param surf pointer to surface
* @param tex pointer to texture
*/
void de_surface_set_texture(de_surface_t* surf, de_texture_t* tex);

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
de_bool_t de_surface_prepare_vertices_for_skinning(de_surface_t* surf);

/**
 * @brief Adds new surface affecting node.
 * @return Returns DE_TRUE if bone was added, DE_FALSE - if vertex already added.
 * 
 * Notes: amortized O(n)
 */
de_bool_t de_surface_add_bone(de_surface_t* surf, de_node_t* bone);

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
void de_surface_get_skinning_matrices(de_surface_t* surf, de_mat4_t* mesh_local_transform, de_mat4_t* out_matrices, size_t max_matrices);

de_bool_t de_surface_is_skinned(de_surface_t* surf);