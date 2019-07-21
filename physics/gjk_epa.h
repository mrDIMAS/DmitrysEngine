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

/* Vertex in space of Minkowski sum */
typedef struct de_minkowski_vertex_t {
	/* World space position of vertex of shape A. This position will be used
	* to compute contact point in world space. */
	de_vec3_t shape_a_world_space;
	/* Minkowski difference between point in shape A and point in shape B.
	* This position will be used to do all simplex and polytope operations.
	* https://en.wikipedia.org/wiki/Minkowski_addition */
	de_vec3_t minkowski_dif;
} de_minkowski_vertex_t;

typedef struct de_simplex_t {
	/* Vertices of simplex.
	* Important: a is most recently added point! */
	de_minkowski_vertex_t a, b, c, d;
	/* Rank of simplex
	* 1 - point
	* 2 - line
	* 3 - triangle
	* 4 - tetrahedron */
	int rank;
} de_simplex_t;

bool de_gjk_is_intersects(de_convex_shape_t* shape1, const de_vec3_t* shape1_position,
	de_convex_shape_t* shape2, const de_vec3_t* shape2_position, de_simplex_t* out_simplex);

bool de_epa_get_penetration_info(de_simplex_t* simplex, de_convex_shape_t* shape1, const de_vec3_t* shape1_position,
	de_convex_shape_t* shape2, const de_vec3_t* shape2_position, de_vec3_t* penetration_vector, de_vec3_t* contact_point);