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

typedef struct de_triangulator_vertex_t {
	de_vec2_t position;
	size_t index;
	DE_LINKED_LIST_ITEM(struct de_triangulator_vertex_t);
} de_triangulator_vertex_t;

typedef DE_LINKED_LIST_DECLARE(de_triangulator_vertex_t, de_triangulator_polygon_t);

/**
 * @brief Performs fast fan triangulation for quadrilaterals or ear-clipping triangulation
 * for simple arbitrary polygon. Function can fail if polygon is not simple or  not enough
 * vertices was passed into (4 or more required).
 *
 * @return Returns count of indices that forms triangles. Negative number indicates error.
 */
int de_triangulate(de_vec3_t* polygon, size_t vertex_count, int* out_indices, int buffer_size);

/**
 * @brief Returns approximate count of indices that is enough to store result
 * of triangulation. Count of triangles will be index_count / 3.
 */
int de_triangulate_get_approx_index_count(size_t vertex_count);

/**
 * @brief Tests for triangulator.
 */
void de_triangulator_tests(void);