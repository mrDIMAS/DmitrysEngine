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

#define DE_CHECK_DENOMINATOR(n) if ((n) == 0) de_fatal_error("math assertion failed in %s. %s cannot be zero", __func__, DE_STRINGIZE(n));
#define DE_CHECK_NAN(n) if ((n) == 0) de_fatal_error("math assertion failed in %s. %s is nan", __func__, DE_STRINGIZE(n));

typedef struct de_mat4_t de_mat4_t;
typedef struct de_mat3_t de_mat3_t;
typedef struct de_vec3_t de_vec3_t;
typedef struct de_vec2_t de_vec2_t;

/**
 * 4D float vector
 */
typedef struct de_vec4_t {
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
	float z; /**< Z coordinate of the vector */
	float w; /**< W coordinate of the vector */
} de_vec4_t;

typedef struct de_quat_t de_quat_t;
typedef struct de_plane_t de_plane_t;

#include "math/quat.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "math/vec2.h"
#include "math/ray.h"
#include "math/plane.h"
#include "math/aabb.h"
#include "math/frustum.h"

/**
 * @brief Performs @a^2
 * @param a scalar
 * @return @a^2
 */
float de_sqr(float a);

/**
 * @brief Converts radians to degress
 * @param a angle in radians
 * @return angle in degrees
 */
float de_rad_to_deg(float a);

/**
 * @brief Converts degrees to radians
 * @param a angle in degrees
 * @return angle in radians
 */
float de_deg_to_rad(float a);

/**
 * @brief Returns a + (b - a) * t
 * @param a scalar
 * @param b scalar
 * @param t scalar
 * @return interpolated value
 */
float de_lerp(float a, float b, float t);

/**
 * @brief Clamps value in range.
 * @param a initial value
 * @param min minimal value
 * @param max maximal value
 * @return a in [min; max]
 */
float de_clamp(float a, float min, float max);

/**
 * @brief Returns maximum of two numbers
 */
float de_maxf(float a, float b);

/**
* @brief Returns minimum of two numbers
*/
float de_minf(float a, float b);

/**
 * @brief Returns next large pow2 number after @v
 */
unsigned int de_ceil_pow2(unsigned int v);

/**
 * @brief Wraps number in specified bounds.
 * Note: Original source code from HPL Engine by Frictional Games, bug fixed by Dmitry Stepanov
 */
float de_fwrap(float n, float min_limit, float max_limit);

/**
 * @brief Calculates normal of a polygon using Newell's method.
 *
 * Note: To get accurate results, a polygon should be flat.
 */
void de_get_polygon_normal(const de_vec3_t* points, size_t count, de_vec3_t* normal);

/**
 * @brief Checks if a point lies inside of a triangle.
 */
bool de_is_point_inside_triangle(const de_vec3_t* point, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c);

/**
 * @brief Checks if a 2d point lies inside of a 2d triangle.
 */
bool de_is_point_inside_triangle_2D(const de_vec2_t* point, const de_vec2_t* a, const de_vec2_t* b, const de_vec2_t* c);

/**
 * @brief Returns signed area of a 2D triangle.
 */
double de_get_signed_triangle_area(const de_vec2_t* v1, const de_vec2_t* v2, const de_vec2_t* v3);

/**
 * @brief Checks intersection between two lines.
 */
bool de_line_line_intersection(const de_vec3_t* a_begin, const de_vec3_t* a_end, const de_vec3_t* b_begin, const de_vec3_t* b_end, de_vec3_t *out);

/**
 * @brief Maps 3d point onto 2d plane of selected class.
 *
 * Main purpose of this function is to make a quick projection of 3d point onto
 * 2D plane.
 */
void de_vec3_to_vec2_by_plane(de_plane_class_t plane, const de_vec3_t* normal, const de_vec3_t * point, de_vec2_t * mapped);

/**
 * @brief Returns random real number in range [min; max]
 */
float de_frand(float min, float max);

/**
 * @brief Returns random integer number in range [min; max]
 */
int de_irand(int min, int max);