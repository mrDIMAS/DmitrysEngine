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
 * @brief Ray.
 */
typedef struct de_ray_t {
	de_vec3_t origin; /**< Origin of the ray */
	de_vec3_t dir;    /**< Direction of the ray, length of this vector defines "length" of the ray */
} de_ray_t;

/**
* @brief
* @param out
* @param origin
* @param dir
* @return
*/
de_ray_t* de_ray_set(de_ray_t* out, const de_vec3_t* origin, const de_vec3_t* dir);

/**
* @brief
* @param out
* @param start
* @param end
* @return
*/
de_ray_t* de_ray_by_two_points(de_ray_t* out, const de_vec3_t* start, const de_vec3_t* end);

/**
* @brief
*/
int de_ray_aabb_intersection(const de_ray_t* ray, const de_vec3_t* min, const de_vec3_t* max, float* out_tmin, float* out_tmax);

/**
* @brief Checks intersection between ray and plane. Returns intersection point.
*/
int de_ray_plane_intersection(const de_ray_t* ray, const de_plane_t* plane, de_vec3_t* out_intersection_point);

/**
* @brief Checks intersection between ray and triangle. Returns intersection point.
*/
int de_ray_triangle_intersection(const de_ray_t* ray, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, de_vec3_t* out_intersection_point);

/**
* @brief Checks intersection between ray and sphere. Returns two intersection points.
*/
int de_ray_sphere_intersection(const de_ray_t* ray, const de_vec3_t* position, float radius, de_vec3_t* out_int_point_a, de_vec3_t* out_int_point_b);
