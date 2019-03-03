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
* @class de_body_s
* @brief Body type for position-based physics.
*
* Each body is a sphere (particle). But can represent capsule too.
*/
struct de_body {
	de_scene_t* scene;
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
* @brief Internal. Returns pointer to pool for physical bodies.
*/
static de_pool_t* de_body_get_pool(void) {
	static de_pool_t pool;
	if (!pool.is_init) {
		de_pool_init(&pool, sizeof(struct de_body), 64, NULL);
	}
	return &pool;
}

void de_body_clear_pool(void) {
	de_pool_clear(de_body_get_pool());
}

struct de_body* de_body_get_ptr(de_body_h handle) {
	return de_pool_get_ptr(de_body_get_pool(), handle.ref);
}

static void de_body_return(de_body_h handle) {
	de_pool_return(de_body_get_pool(), handle.ref);
}

static void de_project_point_on_line(const de_vec3_t* point, const de_ray_t* ray, de_vec3_t* out) {
	float sqr_len;
	de_vec3_t pa, offset;

	sqr_len = de_vec3_sqr_len(&ray->dir);

	if (sqr_len < 0.0001f) {
		de_vec3_zero(out);
	}

	de_vec3_sub(&pa, point, &ray->origin);
	de_vec3_scale(&offset, &ray->dir, de_vec3_dot(&pa, &ray->dir) / sqr_len);
	de_vec3_add(out, &ray->origin, &offset);
}

static bool de_is_point_on_line_segment(const de_vec3_t* point, const de_vec3_t* a, const de_vec3_t* b) {
	/* simply check, if point lies in bounding box (a,b), means that point is on line  */
	de_vec3_t min = *a;
	de_vec3_t max = *b;

	/* swap coordinates if needed */
	float temp;
	if (min.x > max.x) {
		temp = min.x;
		min.x = max.x;
		max.x = temp;
	}
	if (min.y > max.y) {
		temp = min.y;
		min.y = max.y;
		max.y = temp;
	}
	if (min.z > max.z) {
		temp = min.z;
		min.z = max.z;
		max.z = temp;
	}

	if ((point->x > max.x) || (point->y > max.y) || (point->z > max.z)) {
		return false;
	}
	if ((point->x < min.x) || (point->y < min.y) || (point->z < min.z)) {
		return false;
	}
	return true;
}

static bool de_body_sphere_intersection(const de_ray_t* edgeRay, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt) {
	de_vec3_t ray_end_pt;
	if (de_ray_sphere_intersection(edgeRay, sphere_pos, sphere_radius, NULL, NULL)) {
		de_project_point_on_line(sphere_pos, edgeRay, intersection_pt);
		de_vec3_add(&ray_end_pt, &edgeRay->origin, &edgeRay->dir);
		if (de_is_point_on_line_segment(intersection_pt, &edgeRay->origin, &ray_end_pt)) {
			return true;
		}
	}
	return false;
}

static bool de_body_point_intersection(const de_vec3_t* point, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt) {
	if (de_vec3_sqr_distance(point, sphere_pos) < sphere_radius * sphere_radius) {
		*intersection_pt = *point;
		return true;
	}
	return false;
}

static bool de_sphere_triangle_intersection(const de_vec3_t* sphere_pos, float sphere_radius, const de_static_triangle_t* triangle, de_vec3_t* intersection_pt) {
	de_plane_t plane;
	float distance;

	de_plane_set(&plane, &triangle->normal, triangle->distance);

	distance = de_plane_distance(&plane, sphere_pos);
	if (distance <= sphere_radius) {
		de_vec3_t offset;
		de_vec3_scale(&offset, &plane.n, distance);
		de_vec3_sub(intersection_pt, sphere_pos, &offset);
		return de_static_triangle_contains_point(triangle, intersection_pt) ||
			de_body_sphere_intersection(&triangle->ab_ray, sphere_pos, sphere_radius, intersection_pt) ||
			de_body_sphere_intersection(&triangle->bc_ray, sphere_pos, sphere_radius, intersection_pt) ||
			de_body_sphere_intersection(&triangle->ca_ray, sphere_pos, sphere_radius, intersection_pt) ||
			de_body_point_intersection(&triangle->a, sphere_pos, sphere_radius, intersection_pt) ||
			de_body_point_intersection(&triangle->b, sphere_pos, sphere_radius, intersection_pt) ||
			de_body_point_intersection(&triangle->c, sphere_pos, sphere_radius, intersection_pt);
	}
	return false;
}

static void de_body_verlet(struct de_body* body, float dt2) {
	de_vec3_t last_position;
	float friction, k1, k2;
	de_vec3_t velocity;
	float velocity_limit = 0.75f;

	if (body->contact_count > 0) {
		friction = 1 - body->friction;
	} else {
		friction = DE_AIR_FRICTION;
	}

	k1 = 2.0f - friction;
	k2 = 1.0f - friction;

	last_position = body->position;

	/* Verlet integration */
	body->position.x = k1 * body->position.x - k2 * body->last_position.x + body->acceleration.x * dt2;
	body->position.y = k1 * body->position.y - k2 * body->last_position.y + body->acceleration.y * dt2;
	body->position.z = k1 * body->position.z - k2 * body->last_position.z + body->acceleration.z * dt2;

	body->last_position = last_position;

	/* Drop acceleration */
	de_vec3_zero(&body->acceleration);

	/* Velocity limiting */
	de_vec3_sub(&velocity, &body->last_position, &body->position);
	if (de_vec3_sqr_len(&velocity) > velocity_limit * velocity_limit) {
		de_vec3_normalize(&velocity, &velocity);
		de_vec3_scale(&velocity, &velocity, velocity_limit);
		de_vec3_sub(&body->last_position, &body->position, &velocity);
	}
}

static de_contact_t* de_body_add_contact(struct de_body* body) {
	if (body->contact_count < DE_MAX_CONTACTS) {
		return &body->contacts[body->contact_count++];
	}
	return NULL;
}

void de_body_triangle_collision(de_static_triangle_t* triangle, struct de_body* sphere) {
	de_vec3_t intersectionPoint;
	de_vec3_t middle, orientation, offset;
	float length, penetrationDepth;
	de_contact_t* contact;

	if (de_sphere_triangle_intersection(&sphere->position, sphere->radius, triangle, &intersectionPoint)) {
		length = 0.0f;

		/* Calculate penetration depth and push vector */
		de_vec3_sub(&middle, &sphere->position, &intersectionPoint);
		de_vec3_normalize_ex(&orientation, &middle, &length);
		penetrationDepth = sphere->radius - length;

		/* Degenerated case, ignore */
		if (penetrationDepth < 0.0f) {
			return;
		}

		/* Push sphere outside of triangle */
		de_vec3_add(&sphere->position, &sphere->position, de_vec3_scale(&offset, &orientation, penetrationDepth));

		/* Write contact info */
		contact = de_body_add_contact(sphere);

		if (contact) {
			contact->body = (de_body_h) { .ref = de_null_ref };
			contact->normal = orientation;
			contact->position = intersectionPoint;
			contact->triangle = triangle;
		}
	}
}

void de_body_set_gravity(struct de_body* body, const de_vec3_t * gravity) {	
	DE_ASSERT(body);
	body->gravity = *gravity;
}

void de_body_set_position(struct de_body* body, const de_vec3_t * pos) {	
	DE_ASSERT(body);
	body->position = *pos;
	body->last_position = *pos;
}

void de_body_get_position(const struct de_body* body, de_vec3_t* pos) {	
	DE_ASSERT(body);
	*pos = body->position;
}

void de_body_set_radius(struct de_body* body, float radius) {	
	DE_ASSERT(body);
	body->radius = radius;
}

float de_body_get_radius(struct de_body* body) {	
	DE_ASSERT(body);
	return body->radius;
}

size_t de_body_get_contact_count(struct de_body* body) {	
	DE_ASSERT(body);
	return body->contact_count;
}

de_contact_t* de_body_get_contact(struct de_body* body, size_t i) {
	if (i >= DE_MAX_CONTACTS) {
		return NULL;
	}
	
	DE_ASSERT(body);
	return body->contacts + i;
}

void de_body_free(de_body_h body) {
	struct de_body* b = de_body_get_ptr(body);
	DE_ASSERT(b);
	DE_ARRAY_REMOVE(b->scene->bodies, body);
	de_body_return(body);
}

void de_body_move(struct de_body* body, const de_vec3_t* velocity) {	
	DE_ASSERT(body);
	de_vec3_add(&body->position, &body->position, velocity);
}

void de_body_set_velocity(struct de_body* body, const de_vec3_t* velocity) {	
	DE_ASSERT(body);
	body->last_position = body->position;
	de_vec3_sub(&body->last_position, &body->last_position, velocity);
}

void de_body_set_y_velocity(struct de_body* body, float y_velocity) {	
	DE_ASSERT(body);
	body->last_position.y = body->position.y - y_velocity;
}

void de_body_set_x_velocity(struct de_body* body, float x_velocity) {	
	DE_ASSERT(body);
	body->last_position.x = body->position.x - x_velocity;
}

void de_body_set_z_velocity(struct de_body* body, float z_velocity) {	
	DE_ASSERT(body);
	body->last_position.z = body->position.z - z_velocity;
}

void de_body_get_velocity(struct de_body* body, de_vec3_t * velocity) {	
	DE_ASSERT(body);
	de_vec3_sub(velocity, &body->position, &body->last_position);
}

de_body_h de_body_create(de_scene_t* s) {
	DE_ASSERT(s);

	de_body_h handle = { .ref = de_pool_spawn(de_body_get_pool()) };
	struct de_body* body = de_pool_get_ptr(de_body_get_pool(), handle.ref);

	DE_ARRAY_APPEND(s->bodies, handle);

	body->scene = s;
	body->radius = 1.0f;
	body->friction = 0.985f;
	de_vec3_set(&body->scale, 1, 1, 1);
	de_vec3_set(&body->gravity, 0, -9.81f, 0);

	return handle;
}

de_body_h de_body_copy(de_scene_t* dest_scene, struct de_body* body) {
	DE_ASSERT(dest_scene);
	DE_ASSERT(body);
	de_body_h copy_handle = (de_body_h) { .ref = de_pool_spawn(de_body_get_pool()) };
	struct de_body* copy = de_pool_get_ptr(de_body_get_pool(), copy_handle.ref);
	copy->scene = dest_scene;
	copy->gravity = body->gravity;
	copy->position = body->position;
	copy->last_position = body->last_position;
	copy->acceleration = body->acceleration;
	copy->radius = body->radius;
	copy->friction = body->friction;
	copy->scale = body->scale;
	return copy_handle;
}