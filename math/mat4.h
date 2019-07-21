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
 * @brief Square 4x4 matrix.
 */
struct de_mat4_t {
	float f[16];
};

/**
 * @brief Extracts basis as 3x3 matrix.
 */
void de_mat4_get_basis(const de_mat4_t* in, de_mat3_t* out);

/**
 * @brief Returns matrix component at specified row and column.
 */
float de_mat4_at(const de_mat4_t* m, unsigned int row, unsigned int column);

/**
 * Fills main diagonal of 'out' with 1.0, other elements becomes zero
 * \param out pointer to matrix that will be set to identity
 */
void de_mat4_identity(de_mat4_t* out);

/**
 * @brief Every nth row of input matrix 'm' becomes nth column of output matrix 'out'
 * @param out pointer to output matrix
 * @param m pointer to matrix that be transposed
 */
void de_mat4_transpose(de_mat4_t* out, const de_mat4_t* m);

/**
 * @brief Multiplies two matrices together (a * b), and puts result into 'out' matrix
 * @param out is pointer to output matrix
 * @param a pointer to left matrix
 * @param b pointer to right matrix
 */
void de_mat4_mul(de_mat4_t* out, const de_mat4_t* a, const de_mat4_t* b);

/**
 * @brief Builds scaling matrix. Main diagonal elements becomes (v->x, v->y, v->z, 1.0f), other - zero
 * @param out pointer to output matrix
 * @param v pointer to scaling vector
 * @return @out
 */
void de_mat4_scale(de_mat4_t* out, const de_vec3_t* v);

/**
 * @brief Builds perspective matrix
 * @param out pointer to output matrix
 * @param fov_radians field of view, expressed in radians
 * @param aspect is an aspect ratio of frustum base (width / height)
 * @param zNear distance to near clipping plane
 * @param zFar distance to far clipping plane
 */
void de_mat4_perspective(de_mat4_t* out, float fov_radians, float aspect, float zNear, float zFar);

/**
 * @brief Builds matrix for orthographics projection
 * @param out pointer to output matrix
 * @param left X coordinate of left top corner of the "plane" of projection
 * @param right X coordinate of right bottom corner of the "plane" of projection
 * @param bottom Y coordinate of right bottom corner of the "plane" of projection
 * @param top Y coordinate of left top corner of the "plane" of projection
 * @param zNear distance to near clipping plane
 * @param zFar distance to far clipping plane
 * @return @out
 */
void de_mat4_ortho(de_mat4_t* out, float left, float right, float bottom, float top, float zNear, float zFar);

/**
 * @brief Builds translation matrix
 * @param out pointer to output matrix
 * @param v pointer to vector that contains translation
 * @return @out
 */
void de_mat4_translation(de_mat4_t* out, const de_vec3_t* v);

/**
 * @brief Builds "look-at" view matrix
 * @param out pointer to output matrix
 * @param eye observer position
 * @param look look direction of the observer
 * @param up up-vector of the observer
 * @return @out
 */
void de_mat4_look_at(de_mat4_t* out, const de_vec3_t* eye, const de_vec3_t* at, const de_vec3_t* up);

/**
 * @brief Inverses matrix, so A * A_inv = Identity
 * @param out pointer to output matrix
 * @param mat pointer to matrix being inverted
 * @return @out
 */
void de_mat4_inverse(de_mat4_t* out, const de_mat4_t* a);

/**
 * @brief Build rotation matrix from quaternion
 * @param out pointer to output matrix
 * @param q rotation quaternion
 * @return @out
 */
void de_mat4_rotation(de_mat4_t* out, const struct de_quat_t* q);

/**
 * @brief Builds rotation matrix around (1, 0, 0) axis
 * @param out pointer to output matrix
 * @param angle_radians rotation angle in radians
 * @return @out
 */
void de_mat4_rotation_x(de_mat4_t* out, float angle_radians);

/**
 * @brief Builds rotation matrix around (0, 1, 0) axis
 * @param out pointer to output matrix
 * @param angle_radians rotation angle in radians
 * @return @out
 */
void de_mat4_rotation_y(de_mat4_t* out, float angle_radians);

/**
 * @brief Builds rotation matrix around (0, 0, 1) axis
 * @param out pointer to output matrix
 * @param angle_radians rotation angle in radians
 * @return @out
 */
void de_mat4_rotation_z(de_mat4_t* out, float angle_radians);

/**
 * @brief Extracts "up" vector from basis of matrix @m
 * @param m input matrix
 * @param up pointer to vector, holds result
 * @return @up
 */
de_vec3_t* de_mat4_up(const de_mat4_t* m, de_vec3_t* up);

/**
 * @brief Extracts "side" vector from basis of matrix @m
 * @param m input matrix
 * @param side pointer to vector, holds result
 * @return @side
 */
de_vec3_t* de_mat4_side(const de_mat4_t* m, de_vec3_t* side);

/**
 * @brief Extracts "look" vector from basis of matrix @m
 * @param m input matrix
 * @param look pointer to vector, holds result
 * @return @look
 */
de_vec3_t* de_mat4_look(const de_mat4_t* m, de_vec3_t* look);

void de_mat4_get_translation(const de_mat4_t * m, de_vec3_t* translation);

de_quat_t* de_mat4_to_quat(const de_mat4_t* m, de_quat_t* quat);