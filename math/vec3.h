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
 * @brief 3D float vector
 */
struct de_vec3_t {
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
	float z; /**< Z coordinate of the vector */
};

/**
 * @brief Adds two vectors ( @a + @b ), stores result into @out
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec3_t* de_vec3_add(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Subtracts two vectors ( @a - @b ), stores result into @out
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec3_t* de_vec3_sub(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Divides @a using @b (per-component divide). All values of @b must be non-zero
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec3_t* de_vec3_div(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Multiplies @a using @b (per-component multiplication)
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec3_t* de_vec3_mul(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Multiplies each component of @a using @s
 * @param out output vector
 * @param a input vector
 * @param s input scalar
 * @return @out
 */
de_vec3_t* de_vec3_scale(de_vec3_t* out, const de_vec3_t* a, float s);

/**
 * @brief Negates vector
 * @param out output vector
 * @param a input vector
 * @return @out
 */
de_vec3_t* de_vec3_negate(de_vec3_t* out, const de_vec3_t* a);

/**
 * @brief Calculates dot-product between two vectors
 * @param a input vector
 * @param b input vector
 * @return @a . @b
 */
float de_vec3_dot(const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Calculates cross-product between two vectors
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @a x @b
 */
de_vec3_t* de_vec3_cross(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Calculates length of vector
 * @param a input vector
 * @return | @a |
 */
float de_vec3_len(const de_vec3_t* a);

/**
 * @brief Calculates square length of vector
 * @param a input vector
 * @return | @a |^2
 */
float de_vec3_sqr_len(const de_vec3_t* a);

/**
 * @brief Normalizes vector. If length of vector is zero, result is undefined.
 * @param out output vector
 * @param a input vector
 * @return normalized @a
 */
de_vec3_t* de_vec3_normalize(de_vec3_t* out, const de_vec3_t* a);

/**
 * @brief Normalizes vector. If length of vector is less than zero, result is undefined.
 * @param out output vector
 * @param a input vector
 * @param length output vector length, can NOT be NULL
 * @return normalized @a
 */
de_vec3_t* de_vec3_normalize_ex(de_vec3_t* out, const de_vec3_t* a, float* length);

/**
 * @brief Performs linear interpolation between two vectors @a and @b using @t as interpolator
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @param t interpolator (should be in [0;1] range)
 * @return @out
 */
de_vec3_t* de_vec3_lerp(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b, float t);

/**
 * @brief Calculates distance between @a and @b
 * @param a input vector
 * @param b input vector
 * @return distance
 */
float de_vec3_distance(const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Calculates square distance between @a and @b
 * @param a input vector
 * @param b input vector
 * @return square distance
 */
float de_vec3_sqr_distance(const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Calculates angle between @a and @b
 * @param a input vector
 * @param b input vector
 * @return angle between vectors in radians
 */
float de_vec3_angle(const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Projects point onto plane using normal vector of the plane
 * @param out projected point
 * @param point point to be projected
 * @param normal normal of the plane
 * @return @out
 */
de_vec3_t* de_vec3_project_plane(de_vec3_t* out, const de_vec3_t* point, const de_vec3_t* normal);

/**
 * @brief Calculates reflection vector of @a using @normal vector of plane
 * @param out output vector represents reflected vector @a
 * @param a vector to be reflected
 * @param normal normal vector of plane
 * @return @out
 */
de_vec3_t* de_vec3_reflect(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* normal);

/**
 * @brief Calculates new position of point @a using matrix @mat (multiplies 4-D vector (a->x, a->y, a->z, 1.0) using matrix @mat)
 * @param out output point
 * @param a input point
 * @param mat transformation matrix
 * @return @out
 */
de_vec3_t* de_vec3_transform(de_vec3_t* out, const de_vec3_t* a, const de_mat4_t* mat);

/**
 * @brief Calculates new direction of vector @a using matrix @mat (multiplies vector using matrix basis)
 * @param out output transformed vector
 * @param a input vector
 * @param mat transformation matrix (only 3x3 basis is used)
 * @return
 */
de_vec3_t* de_vec3_transform_normal(de_vec3_t* out, const de_vec3_t* a, const de_mat4_t* mat);

/**
 * @brief Calculates middle point between two points
 * @param out output point
 * @param a input point
 * @param b input point
 * @return @out
 */
de_vec3_t* de_vec3_middle(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Calculates per-component min-max values. Can be used to calculate bounds of cloud of points
 * @param a
 * @param vMin pointer to vector that will store minimal components of vector @a
 * @param vMax pointer to vector that will store maximal components of vector @a
 */
void de_vec3_min_max(const de_vec3_t* a, de_vec3_t* vMin, de_vec3_t* vMax);

/**
 * @brief Performs rough equality check using some threshold delta value.
 * @param a input vector
 * @param b input vector
 * @return non-zero if vectors are roughly equal
 */
int de_vec3_approx_equals(const de_vec3_t* a, const de_vec3_t* b);

/**
 * @brief Performs strict equality check
 * @param a input vector
 * @param b input vector
 * @return non-zero if vectors are equal
 */
int de_vec3_equals(const de_vec3_t* a, const de_vec3_t* b);