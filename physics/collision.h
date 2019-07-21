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
	de_vec3_t a;
	de_vec3_t b;
	de_vec3_t c;
	de_vec3_t normal;
	uint32_t material_hash;
};

typedef enum de_ray_cast_flags_t {
	DE_RAY_CAST_FLAGS_IGNORE_BODY = DE_BIT(0),
	DE_RAY_CAST_FLAGS_IGNORE_STATIC_GEOMETRY = DE_BIT(1),
	DE_RAY_CAST_FLAGS_SORT_RESULTS = DE_BIT(2), /**< Results will be sorted from closest to farthest (closest will be first in array) */
	DE_RAY_CAST_FLAGS_IGNORE_BODY_IN_RAY = DE_BIT(3), /**< Bodies that contain ray origin will be ignored. Useful option if you need to cast a ray from some body, i.e. player. */
} de_ray_cast_flags_t;

typedef struct de_ray_cast_result_t {
	de_vec3_t position; /**< Global position of intersection point. */
	de_vec3_t normal; /**< Normal at intersection point. Can be unnormalized! */
	de_static_triangle_t* triangle /**< Pointer to triangle of static geometry that was hit. */;
	de_static_geometry_t* static_geometry; /**< Pointer to static geometry that was hit. More precise results can be obtained from @ref triangle */
	de_body_t* body; /**< Pointer to body that was hit. */
	float sqr_distance; /**< Square distance from intersection point to origin of ray. */
} de_ray_cast_result_t;

/**
 * @brief Array of intersection result
 */
typedef DE_ARRAY_DECLARE(de_ray_cast_result_t, de_ray_cast_result_array_t);

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
	de_octree_t* octree;
	DE_ARRAY_DECLARE(de_static_triangle_t, triangles); /**< Array of de_static_triangle_t. All geometry stored here */
};

/**
* @brief Fills static geometry using faces from mesh
* @param geom pointer to geometry
* @param mesh pointer to mesh
*/
void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t* transform);

/**
 * @brief Adds new triangle to static geometry. \c material_hash can be used to get additional info on collision detection
 * It is useful to get information about properties of surface.
 */
bool de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, uint32_t material_hash);

/**
 * @brief Performs ray cast. Always return result for closest hit. Flags can be used to choose
 * types of entities that should participate in ray cast. Returns true if there was any hit.
 */
bool de_ray_cast_closest(de_scene_t* scene, const de_ray_t* ray, de_ray_cast_flags_t flags, de_ray_cast_result_t* result);

/**
 * @brief Performs ray cast and fills array with intersection result for every picked entity.
 * Flags can be used to choose types of entities that should participate in ray cast. 
 * Returns true if there was any hit.
 */
bool de_ray_cast(de_scene_t* scene, const de_ray_t* ray, de_ray_cast_flags_t flags, de_ray_cast_result_array_t* result_array);

/**
* @brief Calculates physics for one frame
*/
void de_physics_step(de_core_t* core, double dt);

bool de_static_triangle_contains_point(const de_static_triangle_t* triangle, const de_vec3_t* point);