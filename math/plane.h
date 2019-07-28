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
* Fixed plane class.
*/
typedef enum de_plane_class_t {
	DE_PLANE_OXY = 1,
	DE_PLANE_OXZ = 2,
	DE_PLANE_OYZ = 3
} de_plane_class_t;

/**
 * @brief Arbitrary plane
 */
struct de_plane_t {
	de_vec3_t n; /**< Normal vector of the plane */
	float d;     /**< Distance to the plane from the origin */
};

/**
 * @brief Creates plane directly from plane equation coefficients.
 */
de_plane_t* de_plane_set_abcd(de_plane_t* p, float a, float b, float c, float d);

/**
 * @brief Creates plane from point and normal in that point.
 */
de_plane_t* de_plane_set(de_plane_t* p, const de_vec3_t* plane_point, const de_vec3_t* plane_normal);

/**
 * @brief Returns unsigned distance from point to plane.
 */
float de_plane_distance(const de_plane_t* p, const de_vec3_t* point);

/**
 * @brief Returns signed distance from point to plane.
 */
float de_plane_dot(const de_plane_t* p, const de_vec3_t* point);

/**
 * @brief Normalizes plane.
 */
de_plane_t* de_plane_normalize(de_plane_t* p);

/**
 * @brief Returns closest class of plane by its normal.
 */
de_plane_class_t de_plane_classify(const de_vec3_t * triangle_normal);