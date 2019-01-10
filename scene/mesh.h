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

/**
* @brief Mesh.
*
* Described as set of surfaces
*/
struct de_mesh_t
{
	de_node_t* parent_node;
	DE_ARRAY_DECLARE(de_surface_t*, surfaces); /**< Array of pointer to surfaces */
};

/**
* @brief Specializes node as empty mesh (without any surface)
*/
void de_mesh_init(de_node_t* node);

/**
* @brief Frees mesh resources
* @param mesh
*/
void de_mesh_deinit(de_mesh_t* mesh);

/**
* @brief Creates new cube mesh
*/
void de_mesh_make_cube(de_mesh_t* mesh);

/**
 * @brief Calculates normals of each surface. Normals are per-face and not smooth.
 */
void de_mesh_calculate_normals(de_mesh_t * mesh);

/**
* @brief Adds surface to mesh
* @param mesh
* @param surf
*
* Uploads surface to gpu if needed
*/
void de_mesh_add_surface(de_mesh_t* mesh, de_surface_t* surf);

de_bool_t de_mesh_is_skinned(de_mesh_t* mesh);