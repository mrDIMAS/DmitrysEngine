/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

#define DE_MAX_CONTACTS (8)
#define DE_AIR_FRICTION (0.003f)

/**
* @class de_contact_s
* @brief Physical contact
*/
typedef struct de_contact_s
{
	de_body_t* body;                /**< Pointer to body with which contact is appeared */
	de_vec3_t position;             /**< Position of contact */
	de_vec3_t normal;               /**< Normal vector in contact point */
	de_static_triangle_t* triangle; /**< Pointer to triangle of static geometry */
} de_contact_t;

/**
* @class de_body_s
* @brief Body type for position-based physics
*
* Each body is a sphere (particle). But can represent capsule too.
*/
struct de_body_t
{
	DE_LINKED_LIST_ITEM(struct de_body_t);
	de_scene_t* parent_scene;
	de_vec3_t gravity;
	de_vec3_t position;                     /**< Global position of body */
	de_vec3_t last_position;                /**< Global position of previous frame */
	de_vec3_t acceleration;                 /**< Acceleration of a body in m/s^2 */
	float radius;                           /**< Radius of a body */
	float friction;                         /**< Friction coefficient [0; 1]. Zero means no friction */
	de_contact_t contacts[DE_MAX_CONTACTS]; /**< Array of contacts. */
	int contact_count;                      /**< Actual count of physical contacts */
	de_vec3_t scale;                        /**< Scaling coefficients. When != (1, 1, 1) - body is ellipsoid */
};

/**
* @class de_static_triangle_t
* @brief Static triangle for static collision geometry
*/
struct de_static_triangle_t
{
	de_vec3_t normal; /**< Normal of triangle */
	de_vec3_t a;      /**< Vertex of triangle */
	de_vec3_t b;      /**< Vertex of triangle */
	de_vec3_t c;      /**< Vertex of triangle */
	de_vec3_t ab;     /**< Edge of triangle */
	de_vec3_t bc;     /**< Edge of triangle */
	de_vec3_t ca;     /**< Edge of triangle */
	de_vec3_t ba;     /**< Edge of triangle */
	float caDotca;    /**< Precalculated dot product between edges */
	float caDotba;    /**< Precalculated dot product between edges */
	float baDotba;    /**< Precalculated dot product between edges */
	float invDenom;   /**< Inverse denominator for barycentric point-in-triangle method */
	de_ray_t ab_ray;  /**< Precalculated edge ray */
	de_ray_t bc_ray;  /**< Precalculated edge ray */
	de_ray_t ca_ray;  /**< Precalculated edge ray */
	float distance;   /**< Precalculated distance to the origin (d component of plane equation) */
};

/**
* @class de_static_geometry_t
* @brief Static collision geometry
*
* Actually geometry can be moved or transformed in any way user wants, but it is really expensive
* operation. Geometry called "static", because such transformations should be rare.
*/
struct de_static_geometry_t
{
	DE_LINKED_LIST_ITEM(struct de_static_geometry_t);
	de_scene_t* parent_scene;
	DE_ARRAY_DECLARE(de_static_triangle_t, triangles); /**< Array of de_static_triangle_t. All geometry stored here */
};

/**
* @brief
* @param body
* @param velocity
*/
void de_body_move(de_body_t* body, const de_vec3_t* velocity);

/**
* @brief
* @param body
* @param velocity
*/
void de_body_set_velocity(de_body_t* body, const de_vec3_t* velocity);

/**
* @brief
* @param body
* @param x_velocity
*/
void de_body_set_x_velocity(de_body_t* body, float x_velocity);

/**
* @brief
* @param body
* @param y_velocity
*/
void de_body_set_y_velocity(de_body_t* body, float y_velocity);

/**
* @brief
* @param body
* @param z_velocity
*/
void de_body_set_z_velocity(de_body_t* body, float z_velocity);

/**
* @brief
* @param body
* @param velocity
*/
void de_body_get_velocity(de_body_t* body, de_vec3_t* velocity);

/**
* @brief Fills static geometry using faces from mesh
* @param geom pointer to geometry
* @param mesh pointer to mesh
*/
void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t transform);

/**
* @brief Adds new triangle to static geometry
* @param geom pointer to static geometry
* @param a vertex of triangle
* @param b vertex of triangle
* @param c vertex of triangle
*/
void de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c);

/**
* @brief Calculates physics for one frame
*/
void de_physics_step(float dt);


void de_body_set_gravity(de_body_t* body, const de_vec3_t* gravity);


void de_body_set_position(de_body_t* body, const de_vec3_t* pos);

void de_body_set_radius(de_body_t* body, float radius);