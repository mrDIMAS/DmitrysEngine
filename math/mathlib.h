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
typedef enum de_euler_t
{
	DE_EULER_XYZ,
	DE_EULER_XZY,
	DE_EULER_YZX,
	DE_EULER_YXZ,
	DE_EULER_ZXY,
	DE_EULER_ZYX
} de_euler_t;

typedef struct de_mat4_t
{
	float f[16];
} de_mat4_t;

typedef struct de_mat3_t
{
	float f[9];
} de_mat3_t;

/**
 * 2D float vector
 */
typedef struct de_vec2_s
{
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
} de_vec2_t;

/**
 * 3D float vector
 */
typedef struct de_vec3_s
{
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
	float z; /**< Z coordinate of the vector */
} de_vec3_t;

/**
 * 4D float vector
 */
typedef struct de_vec4_t
{
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
	float z; /**< Z coordinate of the vector */
	float w; /**< W coordinate of the vector */
} de_vec4_t;

/**
 * 3D float ray
 */
typedef struct de_ray_t
{
	de_vec3_t origin; /**< Origin of the ray */
	de_vec3_t dir;    /**< Direction of the ray, length of this vector defines "length" of the ray */
} de_ray_t;

/**
 * Rotation quaternion
 */
typedef struct de_quat_t
{
	float x; /**< X component */
	float y; /**< Y component */
	float z; /**< Z component */
	float w; /**< W component */
} de_quat_t;

/**
 * Arbitrary plane
 */
typedef struct de_plane_t
{
	de_vec3_t n; /**< Normal vector of the plane */
	float d;     /**< Distance to the plane from the origin */
} de_plane_t;

/**
 * Fixed plane class.
 */
typedef enum de_plane_class_t
{
	DE_PLANE_OXY = 1,
	DE_PLANE_OXZ = 2,
	DE_PLANE_OYZ = 3,
} de_plane_class_t;

/**
 * Culling frustum
 */
typedef struct de_frustum_t
{
	de_plane_t planes[6]; /**< Six planes that "cuts" frustum volume */
} de_frustum_t;

/**
 * Axis-aligned bounding box (AABB)
 */
typedef struct de_aabb_t
{
	de_vec3_t min;        /**< Corner with with set of minimal coordinates, describing the AABB */
	de_vec3_t max;        /**< Corner with with set of maximal coordinates, describing the AABB */
	de_vec3_t corners[8]; /**< Eight corners of the AABB */
} de_aabb_t;

void de_mat4_get_basis(const de_mat4_t* in, de_mat3_t* out);

float de_mat3_at(const de_mat3_t* m, unsigned int row, unsigned int column);

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

de_quat_t* de_mat4_to_quat(const de_mat4_t* m, de_quat_t* quat);

/**
 * @brief Initializes vector
 * @param out output vector
 * @param x coordinate
 * @param y coordinate
 * @param z coordinate
 * @return @out
 */
de_vec3_t* de_vec3_set(de_vec3_t* out, float x, float y, float z);

/**
 * @brief Sets vector to zero (degenerated vector)
 * @param out vector
 * @return @out
 */
de_vec3_t* de_vec3_zero(de_vec3_t* out);

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
 * @brief Normalizes vector. If length of vector is less than Epsilon, vector will not be modified
 * @param out output vector
 * @param a input vector
 * @return normalized @a
 */
de_vec3_t* de_vec3_normalize(de_vec3_t* out, const de_vec3_t* a);

/**
 * @brief Normalizes vector. If length of vector is less than Epsilon, vector will not be modified
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

/**
 * @brief Initializes vector
 * @param out output vector
 * @param x coordinate
 * @param y coordinate
 * @return @out
 */
de_vec2_t* de_vec2_set(de_vec2_t* out, float x, float y);

/**
 * @brief Sets all components to zero
 * @param out output vector
 * @return @out
 */
de_vec2_t* de_vec2_zero(de_vec2_t* out);

/**
 * @brief Adds two vectors and puts result into @out
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec2_t* de_vec2_add(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief Subtracts @b from @a, puts result into @out
 * @param out output vector
 * @param a input vector
 * @param b input vector
 * @return @out
 */
de_vec2_t* de_vec2_sub(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param out
 * @param a
 * @param b
 * @return
 */
de_vec2_t* de_vec2_div(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param out
 * @param a
 * @param b
 * @return
 */
de_vec2_t* de_vec2_mul(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param out
 * @param a
 * @param s
 * @return
 */
de_vec2_t* de_vec2_scale(de_vec2_t* out, const de_vec2_t* a, float s);

/**
 * @brief
 * @param out
 * @param a
 * @return
 */
de_vec2_t* de_vec2_negate(de_vec2_t* out, const de_vec2_t* a);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_vec2_dot(const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param a
 * @return
 */
float de_vec2_len(const de_vec2_t* a);

/**
 * @brief
 * @param a
 * @return
 */
float de_vec2_sqr_len(const de_vec2_t* a);

/**
 * @brief
 * @param out
 * @param a
 * @return
 */
de_vec2_t* de_vec2_normalize(de_vec2_t* out, const de_vec2_t* a);

/**
 * @brief
 * @param out
 * @param a
 * @param b
 * @param t
 * @return
 */
de_vec2_t* de_vec2_lerp(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b, float t);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_vec2_distance(const de_vec2_t* a, const de_vec2_t* b);


/**
* @brief
* @param out
* @param b
* @return
*/
void de_vec2_perp(de_vec2_t* out, const de_vec2_t* a);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_vec2_sqr_distance(const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_vec2_angle(const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param out
 * @param a
 * @param b
 * @return
 */
de_vec2_t* de_vec2_middle(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param a
 * @param vMin
 * @param vMax
 */
void de_vec2_min_max(const de_vec2_t* a, de_vec2_t* vMin, de_vec2_t* vMax);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
int de_vec2_approx_equals(const de_vec2_t* a, const de_vec2_t* b);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
int de_vec2_equals(const de_vec2_t* a, const de_vec2_t* b);

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
 * @param ray
 * @param aabb
 * @param out_tmin
 * @param out_tmax
 * @return
 */
int de_ray_aabb_intersection(const de_ray_t* ray, const de_vec3_t* min, const de_vec3_t* max, float* out_tmin, float* out_tmax);

/**
 * @brief
 * @param ray
 * @param plane
 * @param out_intersection_point
 * @return
 */
int de_ray_plane_intersection(const de_ray_t* ray, const de_plane_t* plane, de_vec3_t* out_intersection_point);

/**
 * @brief
 * @param ray
 * @param a
 * @param b
 * @param c
 * @param out_intersection_point
 * @return
 */
int de_ray_triangle_intersection(const de_ray_t* ray, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, de_vec3_t* out_intersection_point);

/**
 * @brief
 * @param ray
 * @param position
 * @param radius
 * @param out_int_point_a
 * @param out_int_point_b
 * @return
 */
int de_ray_sphere_intersection(const de_ray_t* ray, const de_vec3_t* position, float radius, de_vec3_t* out_int_point_a, de_vec3_t* out_int_point_b);

/**
 * @brief
 * @param out
 * @param x
 * @param y
 * @param z
 * @param w
 * @return
 */
de_quat_t* de_quat_set(de_quat_t* out, float x, float y, float z, float w);

/**
 * @brief Initializes quaternion using axis and angle (in radians)
 * @param out output quaternion
 * @param axis axis of rotation
 * @param angle angle of rotation (in radians)
 * @return @out
 */
de_quat_t* de_quat_from_axis_angle(de_quat_t* out, const de_vec3_t* axis, float angle);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_quat_dot(const de_quat_t* a, const de_quat_t* b);

/**
 * @brief
 * @param a
 * @return
 */
float de_quat_len(const de_quat_t* a);

/**
 * @brief
 * @param a
 * @return
 */
float de_quat_sqr_len(const de_quat_t* a);

/**
 * @brief
 * @param a
 * @param b
 * @return
 */
float de_quat_angle(const de_quat_t* a, const de_quat_t* b);

/**
 * @brief Performs spherical interpolation between two quaternions
 * @param out
 * @param a
 * @param b
 * @param t
 * @return
 */
de_quat_t* de_quat_slerp(de_quat_t* out, const de_quat_t* a, const de_quat_t* b, float t);

/**
 * @brief Initializes new quaternion using Euler angles with given order of rotation (XYZ, YXZ, etc)
 * @param out
 * @param a
 * @param b
 * @param t
 * @return
 */
void de_quat_from_euler(de_quat_t* out, const de_vec3_t* euler_radians, de_euler_t order);

/**
 * @brief
 * @param out
 * @param a
 * @param b
 * @return
 */
de_quat_t* de_quat_mul(de_quat_t* out, const de_quat_t* a, const de_quat_t* b);

/**
 * @brief
 * @param out
 * @param a
 * @return
 */
de_quat_t* de_quat_normalize(de_quat_t* out, de_quat_t* a);

/**
 * @brief
 * @param q
 * @param out_axis
 * @return
 */
de_vec3_t* de_quat_get_axis(const de_quat_t* q, de_vec3_t* out_axis);

/**
 * @brief
 * @param q
 * @return
 */
float de_quat_get_angle(const de_quat_t* q);

/**
 * @brief
 * @param p
 * @param a
 * @param b
 * @param c
 * @param d
 * @return
 */
de_plane_t* de_plane_setCoef(de_plane_t* p, float a, float b, float c, float d);

/**
 * @brief
 * @param p
 * @param n
 * @param d
 * @return
 */
de_plane_t* de_plane_set(de_plane_t* p, const de_vec3_t* n, float d);

/**
 * @brief
 * @param p
 * @param point
 * @return
 */
float de_plane_distance(const de_plane_t* p, const de_vec3_t* point);

/**
 * @brief
 * @param p
 * @param point
 * @return
 */
float de_plane_dot(const de_plane_t* p, const de_vec3_t* point);

/**
 * @brief
 * @param p
 * @return
 */
de_plane_t* de_plane_normalize(de_plane_t* p);

/**
 * @brief
 * @param f
 * @param m
 * @return
 */
de_frustum_t* de_frustum_from_matrix(de_frustum_t* f, const de_mat4_t* m);

/**
 * @brief
 * @param f
 * @param aabb
 * @param aabb_offset
 * @return
 */
int de_frustum_box_intersection(const de_frustum_t* f, const de_aabb_t* aabb, const de_vec3_t* aabb_offset);

/**
 * @brief
 * @param f
 * @param aabb
 * @param obj_matrix
 * @return
 */
int de_frustum_box_intersection_transform(const de_frustum_t* f, const de_aabb_t* aabb, const de_mat4_t* obj_matrix);

/**
 * @brief
 * @param f
 * @param p
 * @return
 */
int de_frustum_contains_point(const de_frustum_t* f, const de_vec3_t* p);

/**
 * @brief
 * @param aabb
 * @param min
 * @param max
 * @return
 */
de_aabb_t* de_aabb_set(de_aabb_t* aabb, const de_vec3_t* min, const de_vec3_t* max);

/**
 * @brief
 * @param aabb
 * @return
 */
de_aabb_t* de_aabb_recompute_corners(de_aabb_t* aabb);

/**
 * @brief
 * @param aabb
 * @param aabb_offset
 * @param position
 * @param radius
 * @return
 */
int de_aabb_sphere_intersection(const de_aabb_t* aabb, const de_vec3_t* aabb_offset, const de_vec3_t* position, float radius);

/**
 * @brief
 * @param aabb
 * @param point
 * @return
 */
int de_aabb_contains_point(const de_aabb_t* aabb, const de_vec3_t* point);

/**
 * @brief
 * @param aabb
 * @param other
 * @return
 */
int de_aabb_aabb_intersection(const de_aabb_t* aabb, const de_aabb_t* other);

/**
 * @brief
 * @param aabb
 * @param a
 * @param b
 * @param c
 * @return
 */
int de_aabb_triangle_intersection(const de_aabb_t* aabb, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c);

/**
 * @brief
 * @param aabb
 * @param p
 * @return
 */
de_aabb_t* de_aabb_push_point(de_aabb_t* aabb, const de_vec3_t* p);

/**
 * @brief
 * @param aabb
 * @param size
 * @return
 */
de_vec3_t* de_aabb_get_size(const de_aabb_t* aabb, de_vec3_t* size);

/**
 * @brief
 * @param aabb
 * @param center
 * @return
 */
de_vec3_t* de_aabb_get_center(const de_aabb_t* aabb, de_vec3_t* center);

/**
 * @brief Sets extremal points to invalid values (min -> (huge_value), max -> (-huge_value))
 * @param aabb pointer to aabb
 * @return @aabb
 */
de_aabb_t* de_aabb_invalidate(de_aabb_t* aabb);

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
 * @brief Returns closest class of plane by its normal. 
 */
de_plane_class_t de_plane_classify(const de_vec3_t * triangle_normal);

/**
 * @brief Maps 3d point onto 2d plane of selected class.
 * 
 * Main purpose of this function is to make a quick projection of 3d point onto 
 * 2D plane.
 */
void de_vec3_to_vec2_by_plane(de_plane_class_t plane, const de_vec3_t* normal, const de_vec3_t * point, de_vec2_t * mapped);