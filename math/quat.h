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
 * Possible rotation orders
 */
typedef enum de_euler_t {
	DE_EULER_XYZ,
	DE_EULER_XZY,
	DE_EULER_YZX,
	DE_EULER_YXZ,
	DE_EULER_ZXY,
	DE_EULER_ZYX
} de_euler_t;

/**
 * Rotation quaternion
 */
struct de_quat_t {
	float x; /**< X component */
	float y; /**< Y component */
	float z; /**< Z component */
	float w; /**< W component */
};

/**
 * @brief Directly sets quaternion components. 
 */
de_quat_t* de_quat_set(de_quat_t* out, float x, float y, float z, float w);

/**
 * @brief Initializes quaternion using axis and angle (in radians)
 */
de_quat_t* de_quat_from_axis_angle(de_quat_t* out, const de_vec3_t* axis, float angle);

/**
 * @brief
 */
float de_quat_dot(const de_quat_t* a, const de_quat_t* b);

/**
 * @brief Returns length of specified quaternion.
 */
float de_quat_len(const de_quat_t* a);

/**
 * @brief Returns square length of specified quaternion.
 */
float de_quat_sqr_len(const de_quat_t* a);

/**
 * @brief Returns angle between quaternions.
 */
float de_quat_angle(const de_quat_t* a, const de_quat_t* b);

/**
 * @brief Performs spherical interpolation between two quaternions
 */
de_quat_t* de_quat_slerp(de_quat_t* out, const de_quat_t* a, const de_quat_t* b, float t);

/**
 * @brief Initializes new quaternion using Euler angles with given order of rotation (XYZ, YXZ, etc)
 */
void de_quat_from_euler(de_quat_t* out, const de_vec3_t* euler_radians, de_euler_t order);

/**
 * @brief Multiplies two quaternions. Allows to combine rotations.
 */
de_quat_t* de_quat_mul(de_quat_t* out, const de_quat_t* a, const de_quat_t* b);

/**
 * @brief Normalizes quaternion.
 */
de_quat_t* de_quat_normalize(de_quat_t* out, de_quat_t* a);

/**
 * @brief Returns main axis of quaternion.
 */
de_vec3_t* de_quat_get_axis(const de_quat_t* q, de_vec3_t* out_axis);

/**
 * @brief
 */
float de_quat_get_angle(const de_quat_t* q);

/**
 * @brief Checks equality of quaternions by per-component equality check.
 */
bool de_quat_equals(const de_quat_t* a, const de_quat_t* b);

void de_quat_set_at(de_quat_t * quat, float f, unsigned int index);