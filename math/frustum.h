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
* Culling frustum
*/
typedef struct de_frustum_t {
	de_plane_t planes[6]; /**< Six planes that "cuts" frustum volume */
} de_frustum_t;

/**
 * @brief Extracts frustum planes from view-projection matrix.
 */
de_frustum_t* de_frustum_from_matrix(de_frustum_t* f, const de_mat4_t* m);

/**
 * @brief Checks intersection between frustum and specified cloud of points.
 */
bool de_frustum_point_cloud_intersection(const de_frustum_t* f, const de_vec3_t* points, size_t count);

/**
 * @brief Checks intersection between frustum and axis-aligned bounding box.
 */
bool de_frustum_box_intersection(const de_frustum_t* f, const de_aabb_t* aabb);

/**
 * @brief Checks intersection between frustum and axis-aligned bounding box with offset.
 */
bool de_frustum_box_intersection_offset(const de_frustum_t* f, const de_aabb_t* aabb, const de_vec3_t* aabb_offset);

/**
 * @brief Checks intersection between frustum and transformed axis-aligned bounding box.
 *
 * AABB will be transformed into OBB (oriented bounding box) and only then intersection check
 * will be performed!
 */
bool de_frustum_box_intersection_transform(const de_frustum_t* f, const de_aabb_t* aabb, const de_mat4_t* world_matrix);

/**
 * @brief Checks if point is inside frustum volume.
 */
bool de_frustum_contains_point(const de_frustum_t* f, const de_vec3_t* p);

/**
 * @brief Checks if sphere intersect frustum.
 */
bool de_frustum_sphere_intersection(const de_frustum_t* f, const de_vec3_t* p, float r);