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

/**
* @brief Surface
*
* Described as set of vertices and faces, uses single texture.
* Surface lives on GPU (both vertex and index buffer)
*/
struct de_surface_t
{
	de_texture_t* texture; /**< Pointer to texture */
	DE_ARRAY_DECLARE(de_vertex_t, vertices);   /**< Array of vertices */
	DE_ARRAY_DECLARE(int, indices);    /**< Array of faces */
	GLuint vbo;      /**< Vertex buffer object id */
	GLuint vao;      /**< Vertex array object id */
	GLuint ebo;      /**< Element buffer object id */
	de_bool_t need_upload;    /**< Indicates that surface needs to be uploaded to GPU */
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
 */
void de_surface_calculate_normals(de_surface_t * surf);