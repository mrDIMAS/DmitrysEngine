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
* @class de_static_triangle_t
* @brief Static triangle for static collision geometry
*/
struct de_static_triangle_t {
	de_vec3_t normal; /**< Normal of triangle */
	de_vec3_t a;      /**< Vertex of triangle */
	de_vec3_t b;      /**< Vertex of triangle */
	de_vec3_t c;      /**< Vertex of triangle */
	de_vec3_t ab;     /**< Edge of triangle */
	de_vec3_t bc;     /**< Edge of triangle */
	de_vec3_t ca;     /**< Edge of triangle */
	de_vec3_t ba;     /**< Edge of triangle */
	float caDotca;    /**< Precalculated dot product between edges */
	float caDotba;    /**< Precalculated dot product between edges */
	float baDotba;    /**< Precalculated dot product between edges */
	float invDenom;   /**< Inverse denominator for barycentric point-in-triangle method */
	de_ray_t ab_ray;  /**< Precalculated edge ray */
	de_ray_t bc_ray;  /**< Precalculated edge ray */
	de_ray_t ca_ray;  /**< Precalculated edge ray */
	float distance;   /**< Precalculated distance to the origin (d component of plane equation) */
};

/**
* @class de_static_geometry_t
* @brief Static collision geometry
*
* Actually geometry can be moved or transformed in any way user wants, but it is really expensive
* operation. Geometry called "static", because such transformations should be rare.
*/
struct de_static_geometry_t {
	DE_LINKED_LIST_ITEM(struct de_static_geometry_t);
	de_scene_t* scene;
	DE_ARRAY_DECLARE(de_static_triangle_t, triangles); /**< Array of de_static_triangle_t. All geometry stored here */
};

/**
* @brief Fills static geometry using faces from mesh
* @param geom pointer to geometry
* @param mesh pointer to mesh
*/
void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t transform);

/**
* @brief Adds new triangle to static geometry
* @param geom pointer to static geometry
* @param a vertex of triangle
* @param b vertex of triangle
* @param c vertex of triangle
*/
void de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c);

/**
* @brief Calculates physics for one frame
*/
void de_physics_step(de_core_t* core, double dt);

bool de_static_triangle_contains_point(const de_static_triangle_t* triangle, const de_vec3_t* point);