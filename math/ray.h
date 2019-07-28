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
 * Equation: O + D * t , where O - origin, D - direction, t - real parameter.
 */
typedef struct de_ray_t {
	de_vec3_t origin; /**< Origin of the ray */
	de_vec3_t dir; /**< Direction of the ray, length of this vector defines "length" of the ray */
} de_ray_t;

/**
 * @brief Parameters of ray for intersection with convex shape. Defines t parameter in O + D * t 
 * ray equation. t_min is closest parameter (when ray just hit a shape), t_max is farthest 
 * (when ray goes out of shape) like this
 *              +--------------+
 *              |              |
 *              |     -->      |
 *   O     t_min|      D       |t_max
 *   +----------*--------------*--------->
 *              |              |
 *              |              |
 *              |              |
 *              +--------------+
 */
typedef struct de_ray_intersection_parameters_t {
	float t_min;
	float t_max;
} de_ray_intersection_parameters_t;

/**
 * @brief Initializes ray using origin and direction. Length of direction vector defines ray length.
 * In other words if ray parameter t will be in range [0; 1] then point lies not  
 */
void de_ray_set(de_ray_t* out, const de_vec3_t* origin, const de_vec3_t* dir);

/**
 * @brief Initializes ray using two points.
 */
void de_ray_by_two_points(de_ray_t* out, const de_vec3_t* start, const de_vec3_t* end);

/**
 * @brief
 */
int de_ray_aabb_intersection(const de_ray_t* ray, const de_vec3_t* min, const de_vec3_t* max, float* out_tmin, float* out_tmax);

/**
 * @brief Checks intersection between ray and plane. Returns intersection point.
 */
bool de_ray_plane_intersection_point(const de_ray_t* ray, const de_plane_t* plane, de_vec3_t* out_intersection_point);

/**
 * @brief Checks intersection between ray and plane. Returns ray parameter.
 */
float de_ray_plane_intersection(const de_ray_t* ray, const de_plane_t* plane);

de_vec3_t de_ray_evaluate(const de_ray_t* ray, float t);

/**
 * @brief Checks intersection between ray and triangle. Returns intersection point.
 */
int de_ray_triangle_intersection(const de_ray_t* ray, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, de_vec3_t* out_intersection_point);

/**
 * @brief Checks intersection between ray and sphere. Returns two intersection points.
 */
bool de_ray_sphere_intersection_point(const de_ray_t* ray, const de_vec3_t* position, float radius, de_vec3_t* out_int_point_a, de_vec3_t* out_int_point_b);

bool de_ray_sphere_intersection(const de_ray_t* ray, const de_vec3_t* position, float radius, de_ray_intersection_parameters_t* parameters);

bool de_ray_capsule_intersection(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float radius, de_vec3_t int_points[2]);

bool de_ray_cylinder_intersection(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float r, de_cylinder_type_t type, de_ray_intersection_parameters_t* parameters);

bool de_ray_cylinder_intersection_point(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float r, de_cylinder_type_t type, de_vec3_t int_points[2]);

void de_ray_intersection_parameters_reset(de_ray_intersection_parameters_t* params);

void de_ray_intersection_parameters_push(de_ray_intersection_parameters_t* params, float t);