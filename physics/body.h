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

typedef enum de_convex_shape_type_t {
	DE_CONVEX_SHAPE_TYPE_SPHERE,
	DE_CONVEX_SHAPE_TYPE_TRIANGLE,
	DE_CONVEX_SHAPE_TYPE_POINT_CLOUD
} de_convex_shape_type_t;

typedef struct de_sphere_shape_t {
	float radius;
} de_sphere_shape_t;

typedef struct de_triangle_shape_t {
	de_vec3_t vertices[3];
} de_triangle_shape_t;

typedef struct de_point_cloud_shape_t {
	DE_ARRAY_DECLARE(de_vec3_t, vertices);
} de_point_cloud_shape_t;

typedef struct de_convex_shape_t {
	de_convex_shape_type_t type;
	union {
		de_sphere_shape_t sphere;
		de_triangle_shape_t triangle;
		de_point_cloud_shape_t point_cloud;
	} s;
} de_convex_shape_t;

/**
* @class de_body_s
* @brief Body type for position-based physics.
*
* Each body is a sphere (particle). But can represent capsule too.
*/
struct de_body_t {
	de_convex_shape_t shape;
	de_scene_t* scene;
	de_vec3_t gravity;
	de_vec3_t position;                     /**< Global position of body */
	de_vec3_t last_position;                /**< Global position of previous frame */
	de_vec3_t acceleration;                 /**< Acceleration of a body in m/s^2 */
	float friction;                         /**< Friction coefficient [0; 1]. Zero means no friction */
	de_contact_t contacts[DE_MAX_CONTACTS]; /**< Array of contacts. */
	int contact_count;                      /**< Actual count of physical contacts */
	de_vec3_t scale;                        /**< Scaling coefficients. When != (1, 1, 1) - body is ellipsoid */
	DE_LINKED_LIST_ITEM(de_body_t);
};

/**
* @brief Frees all resources associated with body
* @param body pointer to body
*/
void de_body_free(de_body_t* body);

/**
* @brief Creates new body with radius 1.0
*/
de_body_t* de_body_create(de_scene_t* s);

bool de_body_visit(de_object_visitor_t* visitor, de_body_t* body);

/**
* @brief Changes actual position of a body by velocity vector. After this routine,
* body will be moving with passed velocity.
* @param body
* @param velocity
*/
void de_body_move(de_body_t* body, const de_vec3_t* velocity);

/**
* @brief Creates full copy of physical body.
*/
de_body_t* de_body_copy(de_scene_t* dest_scene, de_body_t* body);

/**
* @brief Sets actual velocity of a body.
* @param body
* @param velocity
*/
void de_body_set_velocity(de_body_t* body, const de_vec3_t* velocity);

/**
* @brief Sets X parts of velocity independently
*/
void de_body_set_x_velocity(de_body_t* body, float x_velocity);

/**
* @brief Sets Y parts of velocity independently
*/
void de_body_set_y_velocity(de_body_t* body, float y_velocity);

/**
* @brief Sets Z parts of velocity independently
*/
void de_body_set_z_velocity(de_body_t* body, float z_velocity);

/**
* @brief Write out current velocity of the body.
*/
void de_body_get_velocity(de_body_t* body, de_vec3_t* velocity);

/**
* @brief Sets gravity vector for a body.
*/
void de_body_set_gravity(de_body_t* body, const de_vec3_t* gravity);

/**
* @brief Sets actual position of a body, velocity will be set to zero.
*/
void de_body_set_position(de_body_t* body, const de_vec3_t* pos);

/**
 * @brief Returns position of a body.
 */
void de_body_get_position(const de_body_t* body, de_vec3_t* pos);

/**
 * @brief Applies specified amount of acceleration to specified body.
 */
void de_body_add_acceleration(de_body_t* body, const de_vec3_t* acceleration);

/**
* @brief Sets actual body radius.
*/
void de_body_set_radius(de_body_t* body, float radius);

/**
* @brief Returns actual body radius.
*/
float de_body_get_radius(de_body_t* body);

/**
 * @brief Returns total amount of physical contacts.
 */
size_t de_body_get_contact_count(de_body_t* body);

/**
 * @brief Returns pointer to physical contact.
 */
de_contact_t* de_body_get_contact(de_body_t* body, size_t i);

de_vec3_t de_convex_shape_get_farthest_point(const de_convex_shape_t* shape, const de_vec3_t* position, const de_vec3_t* dir);