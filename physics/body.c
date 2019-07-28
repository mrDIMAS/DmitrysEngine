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

#if 0
static float de_project_point_on_line(const de_vec3_t* point, const de_ray_t* ray, de_vec3_t* out)
{
	const float sqr_len = de_vec3_sqr_len(&ray->dir);

	if (sqr_len < 0.0001f) {
		*out = (de_vec3_t) { 0 };
		return -1.0f;
	}

	de_vec3_t pa;
	de_vec3_sub(&pa, point, &ray->origin);

	const float t = de_vec3_dot(&pa, &ray->dir) / sqr_len;

	de_vec3_t offset;
	de_vec3_scale(&offset, &ray->dir, t);
	de_vec3_add(out, &ray->origin, &offset);

	return t;
}

static bool de_body_sphere_intersection(const de_ray_t* edgeRay, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt)
{
	if (de_ray_sphere_intersection_point(edgeRay, sphere_pos, sphere_radius, NULL, NULL)) {
		const float t = de_project_point_on_line(sphere_pos, edgeRay, intersection_pt);
		return t >= 0.0f && t <= 1.0f;
	}
	return false;
}

static bool de_body_point_intersection(const de_vec3_t* point, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt)
{
	if (de_vec3_sqr_distance(point, sphere_pos) < sphere_radius * sphere_radius) {
		*intersection_pt = *point;
		return true;
	}
	return false;
}

static bool de_sphere_triangle_intersection(const de_vec3_t* sphere_pos, float sphere_radius, const de_static_triangle_t* triangle, de_vec3_t* intersection_pt)
{
	de_plane_t plane;
	de_plane_set(&plane, &triangle->normal, triangle->distance);

	const float distance = de_plane_distance(&plane, sphere_pos);
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
#endif

static void de_body_verlet(de_body_t* body, float dt2)
{
	const float velocity_limit = 0.75f;

	//const float friction = body->contact_count > 0 ? 1 - body->friction : DE_AIR_FRICTION;
	const float friction = 0.003f;

	const float k1 = 2.0f - friction;
	const float k2 = 1.0f - friction;

	const de_vec3_t last_position = body->position;

	/* Verlet integration */
	body->position.x = k1 * body->position.x - k2 * body->last_position.x + body->acceleration.x * dt2;
	body->position.y = k1 * body->position.y - k2 * body->last_position.y + body->acceleration.y * dt2;
	body->position.z = k1 * body->position.z - k2 * body->last_position.z + body->acceleration.z * dt2;

	body->last_position = last_position;

	/* Drop acceleration */
	body->acceleration = (de_vec3_t) { 0 };

	/* Velocity limiting */
	de_vec3_t velocity;
	de_vec3_sub(&velocity, &body->last_position, &body->position);
	if (de_vec3_sqr_len(&velocity) > velocity_limit * velocity_limit) {
		de_vec3_normalize(&velocity, &velocity);
		de_vec3_scale(&velocity, &velocity, velocity_limit);
		de_vec3_sub(&body->last_position, &body->position, &velocity);
	}
}

static de_contact_t* de_body_add_contact(de_body_t* body)
{
	if (body->contact_count < DE_MAX_CONTACTS) {
		return &body->contacts[body->contact_count++];
	}
	return NULL;
}

#if 0
void de_body_triangle_collision(de_static_geometry_t* geom, de_static_triangle_t* triangle, de_body_t* sphere)
{
	de_vec3_t intersectionPoint;
	if (de_sphere_triangle_intersection(&sphere->position, sphere->radius, triangle, &intersectionPoint)) {
		/* Calculate penetration depth and push vector */
		de_vec3_t middle;
		de_vec3_sub(&middle, &sphere->position, &intersectionPoint);

		float length = 0.0f;
		de_vec3_t orientation;
		de_vec3_normalize_ex(&orientation, &middle, &length);
		const float penetrationDepth = sphere->radius - length;
		if (penetrationDepth >= 0.0f) {
			/* Push sphere outside of triangle */
			de_vec3_t offset;
			de_vec3_add(&sphere->position, &sphere->position, de_vec3_scale(&offset, &orientation, penetrationDepth));

			/* Write contact info */
			de_contact_t* contact = de_body_add_contact(sphere);
			if (contact) {
				contact->body = NULL;
				contact->normal = orientation;
				contact->position = intersectionPoint;
				contact->triangle = triangle;
				contact->geometry = geom;
			}
		}
	}
}
#endif

static void de_sphere_sphere_body_collision(de_body_t* body, de_body_t* other)
{
	const de_sphere_shape_t* shape = de_convex_shape_to_sphere(&body->shape);
	const de_sphere_shape_t* other_shape = de_convex_shape_to_sphere(&other->shape);

	de_vec3_t dir;
	de_vec3_sub(&dir, &other->position, &body->position);
	const float distance = de_vec3_len(&dir);
	const float radius_sum = shape->radius + other_shape->radius;
	if (distance <= radius_sum) {
		de_vec3_t push_vec;
		de_vec3_scale(&dir, &dir, 1.0f / distance);
		de_vec3_scale(&push_vec, &dir, (distance - radius_sum) * 0.5f);
		de_vec3_add(&body->position, &body->position, &push_vec);
		de_vec3_sub(&other->position, &other->position, &push_vec);

		/* Write contact info */
		de_vec3_t center;
		de_vec3_middle(&center, &body->position, &other->position);
		de_contact_t* contact = de_body_add_contact(body);
		if (contact) {
			contact->body = other;
			contact->normal = dir;
			contact->position = center;
			contact->triangle = NULL;
			contact->geometry = NULL;
		}
		contact = de_body_add_contact(other);
		if (contact) {
			contact->body = body;
			de_vec3_negate(&contact->normal, &dir);
			contact->position = center;
			contact->triangle = NULL;
			contact->geometry = NULL;
		}
	}
}

void de_body_body_collision(de_body_t* body, de_body_t* other)
{
	if (body->shape.type == DE_CONVEX_SHAPE_TYPE_SPHERE && other->shape.type == DE_CONVEX_SHAPE_TYPE_SPHERE) {
		/* Special, very fast method to resolve collisions between two spheres */
		de_sphere_sphere_body_collision(body, other);
	} else {
		/* Generic collisions between two convex bodies */
		de_convex_shape_t* shape1 = &body->shape;
		de_convex_shape_t* shape2 = &other->shape;

		de_simplex_t simplex;
		if (de_gjk_is_intersects(shape1, &body->position, shape2, &other->position, &simplex)) {
			de_vec3_t penetration_vector;
			de_vec3_t contact_point;
			if (de_epa_get_penetration_info(&simplex, shape1, &body->position, shape2, &other->position, &penetration_vector, &contact_point)) {
				if (de_vec3_sqr_len(&penetration_vector)) {
					de_vec3_t half_push;
					de_vec3_scale(&half_push, &penetration_vector, 0.5f);

					/* Push body by a half vector and write contact info */
					de_vec3_sub(&body->position, &body->position, &half_push);
					de_contact_t* contact = de_body_add_contact(body);
					if (contact) {
						contact->body = other;
						/* Make sure that we'll get correct normal. */
						de_vec3_negate(&contact->normal, &penetration_vector);
						de_vec3_normalize(&contact->normal, &contact->normal);
						contact->position = contact_point;
						contact->triangle = NULL;
						contact->geometry = NULL;
					}

					/* Push other body by a half vector in opposite side and write contact info */
					de_vec3_add(&other->position, &other->position, &half_push);
					contact = de_body_add_contact(other);
					if (contact) {
						contact->body = body;
						de_vec3_normalize(&contact->normal, &penetration_vector);
						contact->position = contact_point;
						contact->triangle = NULL;
						contact->geometry = NULL;
					}
				}
			}
		}
	}
}

void de_body_add_acceleration(de_body_t* body, const de_vec3_t* acceleration)
{
	DE_ASSERT(body);
	de_vec3_add(&body->acceleration, &body->acceleration, acceleration);
}

void de_body_set_gravity(de_body_t* body, const de_vec3_t * gravity)
{
	DE_ASSERT(body);
	body->gravity = *gravity;
}

void de_body_set_position(de_body_t* body, const de_vec3_t * pos)
{
	DE_ASSERT(body);
	body->position = *pos;
	body->last_position = *pos;
}

void de_body_get_position(const de_body_t* body, de_vec3_t* pos)
{
	DE_ASSERT(body);
	*pos = body->position;
}

size_t de_body_get_contact_count(de_body_t* body)
{
	DE_ASSERT(body);
	return body->contact_count;
}

de_contact_t* de_body_get_contact(de_body_t* body, size_t i)
{
	if (i >= DE_MAX_CONTACTS) {
		return NULL;
	}

	DE_ASSERT(body);
	return body->contacts + i;
}

de_convex_shape_t* de_body_get_shape(de_body_t* body)
{
	DE_ASSERT(body);
	return &body->shape;
}

void de_body_free(de_body_t* body)
{
	DE_ASSERT(body);
	de_convex_shape_free(&body->shape);
	DE_LINKED_LIST_REMOVE(body->scene->bodies, body);
	de_free(body);
}

void de_body_move(de_body_t* body, const de_vec3_t* velocity)
{
	DE_ASSERT(body);
	de_vec3_add(&body->position, &body->position, velocity);
}

void de_body_set_velocity(de_body_t* body, const de_vec3_t* velocity)
{
	DE_ASSERT(body);
	body->last_position = body->position;
	de_vec3_sub(&body->last_position, &body->last_position, velocity);
}

void de_body_set_y_velocity(de_body_t* body, float y_velocity)
{
	DE_ASSERT(body);
	body->last_position.y = body->position.y - y_velocity;
}

void de_body_set_x_velocity(de_body_t* body, float x_velocity)
{
	DE_ASSERT(body);
	body->last_position.x = body->position.x - x_velocity;
}

void de_body_set_z_velocity(de_body_t* body, float z_velocity)
{
	DE_ASSERT(body);
	body->last_position.z = body->position.z - z_velocity;
}

void de_body_get_velocity(de_body_t* body, de_vec3_t * velocity)
{
	DE_ASSERT(body);
	de_vec3_sub(velocity, &body->position, &body->last_position);
}

static de_convex_shape_t de_convex_shape_copy(de_convex_shape_t* shape)
{
	if (shape->type == DE_CONVEX_SHAPE_TYPE_POINT_CLOUD) {
		/* Point cloud is special becuase it contains heap-allocated memory which we 
		 * must copy, not share between multiple shapes to prevent double free. */
		de_convex_shape_t copy;		
		DE_ARRAY_COPY(shape->s.point_cloud.points, copy.s.point_cloud.points);
	}
	/* In other cases we can make byte-to-byte copy */
	return *shape;
}

de_body_t* de_body_create(de_scene_t* s, de_convex_shape_t shape)
{
	DE_ASSERT(s);
	de_body_t* body = DE_NEW(de_body_t);
	DE_LINKED_LIST_APPEND(s->bodies, body);
	body->scene = s;
	body->shape = shape;
	body->friction = 0.99f;
	body->gravity = DE_DEFAULT_GRAVITY;
	return body;
}

bool de_body_visit(de_object_visitor_t* visitor, de_body_t* body)
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Scene", &body->scene, de_scene_visit);
	result &= de_object_visitor_visit_vec3(visitor, "Gravity", &body->gravity);
	result &= de_object_visitor_visit_vec3(visitor, "Position", &body->position);
	result &= de_object_visitor_visit_vec3(visitor, "LastPosition", &body->last_position);
	result &= de_object_visitor_visit_vec3(visitor, "Acceleration", &body->acceleration);
	result &= de_convex_shape_visit(visitor, &body->shape);
	result &= de_object_visitor_visit_float(visitor, "Friction", &body->friction);
	return result;
}

de_body_t* de_body_copy(de_scene_t* dest_scene, de_body_t* body)
{
	DE_ASSERT(dest_scene);
	DE_ASSERT(body);
	de_body_t* copy = DE_NEW(de_body_t);
	copy->scene = dest_scene;
	copy->gravity = body->gravity;
	copy->position = body->position;
	copy->last_position = body->last_position;
	copy->acceleration = body->acceleration;
	copy->shape = de_convex_shape_copy(&body->shape);
	copy->friction = body->friction;
	return copy;
}
