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
 * @brief Axis-aligned bounding box (AABB)
 */
typedef struct de_aabb_t {
	de_vec3_t min; /**< Corner with with set of minimal coordinates, describing the AABB */
	de_vec3_t max; /**< Corner with with set of maximal coordinates, describing the AABB */
} de_aabb_t;

/**
 * @brief Initializes axis-aligned bounding box from specified corner points.
 */
void de_aabb_set(de_aabb_t* aabb, const de_vec3_t* min, const de_vec3_t* max);

/**
 * @brief Checks intersection between specified sphere and aabb. 
 */
bool de_aabb_sphere_intersection(const de_aabb_t* aabb, const de_vec3_t* aabb_offset, const de_vec3_t* position, float radius);

/**
 * @brief Checks if axis-aligned bounding box contains specified point.
 */
bool de_aabb_contains_point(const de_aabb_t* aabb, const de_vec3_t* point);

/**
 * @brief Chechs intersection between two axis-aligned bounding boxes.
 */
bool de_aabb_aabb_intersection(const de_aabb_t* aabb, const de_aabb_t* other);

/**
 * @brief Checks intersection between triangle and axis-aligned bounding box.
 * 
 * This is just rough check - check for intersection between aabb of triangle with specified aabb.
 */
bool de_aabb_triangle_intersection(const de_aabb_t* aabb, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c);

/**
 * @brief Adds point to axis-aligned bounding box.
 */
de_aabb_t* de_aabb_push_point(de_aabb_t* aabb, const de_vec3_t* p);

/**
 * @brief Returns size of axis-aligned bounding box.
 */
void de_aabb_get_size(const de_aabb_t* aabb, de_vec3_t* size);

/**
 * @brief Returns center of axis-aligned bounding box.
 */
void de_aabb_get_center(const de_aabb_t* aabb, de_vec3_t* center);

/**
 * @brief Sets extremal points to invalid values (min -> (huge_value), max -> (-huge_value))  
 */
void de_aabb_invalidate(de_aabb_t* aabb);

/**
 * @brief Merges specified axis-aligned bounding box with other box. Specified aabb becomes large 
 * enough to contain two boxes.
 */
void de_aabb_merge(de_aabb_t* aabb, const de_aabb_t* other);