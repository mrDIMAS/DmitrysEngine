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
* @brief Removes all physical bodies.
*/
void de_body_clear_pool(void);

/**
 * @brief Convers handle into a pointer. Returns NULL when handle is invalid.
 */
struct de_body* de_body_get_ptr(de_body_h handle);

/**
* @brief Frees all resources associated with body
* @param body pointer to body
*/
void de_body_free(de_body_h body);

/**
* @brief Creates new body with radius 1.0
*/
de_body_h de_body_create(de_scene_t* s);

/**
* @brief Changes actual position of a body by velocity vector. After this routine,
* body will be moving with passed velocity.
* @param body
* @param velocity
*/
void de_body_move(struct de_body* body, const de_vec3_t* velocity);

/**
* @brief Creates full copy of physical body.
*/
de_body_h de_body_copy(de_scene_t* dest_scene, struct de_body* body);

/**
* @brief Sets actual velocity of a body.
* @param body
* @param velocity
*/
void de_body_set_velocity(struct de_body* body, const de_vec3_t* velocity);

/**
* @brief Sets X parts of velocity independently
*/
void de_body_set_x_velocity(struct de_body* body, float x_velocity);

/**
* @brief Sets Y parts of velocity independently
*/
void de_body_set_y_velocity(struct de_body* body, float y_velocity);

/**
* @brief Sets Z parts of velocity independently
*/
void de_body_set_z_velocity(struct de_body* body, float z_velocity);

/**
* @brief Write out current velocity of the body.
*/
void de_body_get_velocity(struct de_body* body, de_vec3_t* velocity);

/**
* @brief Sets gravity vector for a body.
*/
void de_body_set_gravity(struct de_body* body, const de_vec3_t* gravity);

/**
* @brief Sets actual position of a body, velocity will be set to zero.
*/
void de_body_set_position(struct de_body* body, const de_vec3_t* pos);

/**
 * @brief Returns position of a body.
 */
void de_body_get_position(const struct de_body* body, de_vec3_t* pos);

/**
* @brief Sets actual body radius.
*/
void de_body_set_radius(struct de_body* body, float radius);

/**
* @brief Returns actual body radius.
*/
float de_body_get_radius(struct de_body* body);

/**
 * @brief Returns total amount of physical contacts.
 */
size_t de_body_get_contact_count(struct de_body* body);

/**
 * @brief Returns pointer to physical contact.
 */
de_contact_t* de_body_get_contact(struct de_body* body, size_t i);