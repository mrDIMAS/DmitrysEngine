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

/*=======================================================================================*/
void de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c)
{
	de_vec3_t ca;
	de_static_triangle_t triangle;

	triangle.a = *a;
	triangle.b = *b;
	triangle.c = *c;

	/* Find vectors from triangle vertices */
	de_vec3_sub(&triangle.ba, b, a);
	de_vec3_sub(&ca, c, a);

	/* Normal of triangle is a cross product of above vectors */
	de_vec3_normalize(&triangle.normal, de_vec3_cross(&triangle.normal, &triangle.ba, &ca));

	/* Find triangle edges */
	de_vec3_sub(&triangle.ab, a, b);
	de_vec3_sub(&triangle.bc, b, c);
	de_vec3_sub(&triangle.ca, c, a);
	de_ray_by_two_points(&triangle.ab_ray, a, b);
	de_ray_by_two_points(&triangle.bc_ray, b, c);
	de_ray_by_two_points(&triangle.ca_ray, c, a);

	/* Precalculate dot-products for barycentric "point-in-triangle" method */
	triangle.caDotca = de_vec3_dot(&ca, &ca);
	triangle.caDotba = de_vec3_dot(&ca, &triangle.ba);
	triangle.baDotba = de_vec3_dot(&triangle.ba, &triangle.ba);
	triangle.invDenom = 1.0f / (triangle.caDotca * triangle.baDotba - triangle.caDotba * triangle.caDotba);
	triangle.distance = -de_vec3_dot(a, &triangle.normal);

	/* Add new triangle to array */
	DE_ARRAY_APPEND(geom->triangles, triangle);
}

/*=======================================================================================*/
void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t transform)
{
	size_t i, k;

	if (!geom || !mesh)
	{
		return;
	}

	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_surface_t* surf;

		surf = mesh->surfaces.data[i];

		for (k = 0; k < surf->indices.size; k += 3)
		{
			de_vertex_t va, vb, vc;
			de_vec3_t pa, pb, pc;
			int a, b, c;

			a = surf->indices.data[k];
			b = surf->indices.data[k + 1];
			c = surf->indices.data[k + 2];

			va = surf->vertices.data[a];
			vb = surf->vertices.data[b];
			vc = surf->vertices.data[c];

			pa = va.position;
			pb = vb.position;
			pc = vc.position;

			de_vec3_transform(&pa, &pa, &transform);
			de_vec3_transform(&pb, &pb, &transform);
			de_vec3_transform(&pc, &pc, &transform);

			de_static_geometry_add_triangle(geom, &pa, &pb, &pc);
		}
	}
}

/*=======================================================================================*/
static de_bool_t de_static_triangle_contains_point(const de_static_triangle_t* triangle, const de_vec3_t* point)
{
	de_vec3_t vp;
	float dot02, dot12, u, v;

	de_vec3_sub(&vp, point, &triangle->a);
	dot02 = de_vec3_dot(&triangle->ca, &vp);
	dot12 = de_vec3_dot(&triangle->ba, &vp);
	u = (triangle->baDotba * dot02 - triangle->caDotba * dot12) * triangle->invDenom;
	v = (triangle->caDotca * dot12 - triangle->caDotba * dot02) * triangle->invDenom;
	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

/*=======================================================================================*/
static de_bool_t de_is_point_on_line_segment(const de_vec3_t* point, const de_vec3_t* a, const de_vec3_t* b)
{
	/* simply check, if point lies in bounding box (a,b), means that point is on line  */
	de_vec3_t min = *a;
	de_vec3_t max = *b;

	/* swap coordinates if needed */
	float temp;
	if (min.x > max.x)
	{
		temp = min.x;
		min.x = max.x;
		max.x = temp;
	}
	if (min.y > max.y)
	{
		temp = min.y;
		min.y = max.y;
		max.y = temp;
	}
	if (min.z > max.z)
	{
		temp = min.z;
		min.z = max.z;
		max.z = temp;
	}

	if ((point->x > max.x) || (point->y > max.y) || (point->z > max.z))
	{
		return DE_FALSE;
	}
	if ((point->x < min.x) || (point->y < min.y) || (point->z < min.z))
	{
		return DE_FALSE;
	}
	return DE_TRUE;
}

/*=======================================================================================*/
static void de_project_point_on_line(const de_vec3_t* point, const de_ray_t* ray, de_vec3_t* out)
{
	float sqr_len;
	de_vec3_t pa, offset;

	sqr_len = de_vec3_sqr_len(&ray->dir);

	if (sqr_len < 0.0001f)
	{
		de_vec3_zero(out);
	}

	de_vec3_sub(&pa, point, &ray->origin);
	de_vec3_scale(&offset, &ray->dir, de_vec3_dot(&pa, &ray->dir) / sqr_len);
	de_vec3_add(out, &ray->origin, &offset);
}

/*=======================================================================================*/
static de_bool_t de_body_sphere_intersection(const de_ray_t* edgeRay, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt)
{
	de_vec3_t ray_end_pt;
	if (de_ray_sphere_intersection(edgeRay, sphere_pos, sphere_radius, NULL, NULL))
	{
		de_project_point_on_line(sphere_pos, edgeRay, intersection_pt);
		de_vec3_add(&ray_end_pt, &edgeRay->origin, &edgeRay->dir);
		if (de_is_point_on_line_segment(intersection_pt, &edgeRay->origin, &ray_end_pt))
		{
			return DE_TRUE;
		}
	}
	return DE_FALSE;
}

/*=======================================================================================*/
static de_bool_t de_body_point_intersection(const de_vec3_t* point, const de_vec3_t* sphere_pos, float sphere_radius, de_vec3_t* intersection_pt)
{
	if (de_vec3_sqr_distance(point, sphere_pos) < sphere_radius * sphere_radius)
	{
		*intersection_pt = *point;
		return DE_TRUE;
	}
	return DE_FALSE;
}

/*=======================================================================================*/
static de_bool_t de_sphere_triangle_intersection(const de_vec3_t* sphere_pos, float sphere_radius, const de_static_triangle_t* triangle, de_vec3_t* intersection_pt)
{
	de_plane_t plane;
	float distance;

	de_plane_set(&plane, &triangle->normal, triangle->distance);

	distance = de_plane_distance(&plane, sphere_pos);
	if (distance <= sphere_radius)
	{
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
	return DE_FALSE;
}

/*=======================================================================================*/
static void de_body_verlet(de_body_t* body, float dt2)
{
	de_vec3_t last_position;
	float friction, k1, k2;
	de_vec3_t velocity;
	float velocity_limit = 0.75f;

	if (body->contact_count > 0)
	{
		friction = 1 - body->friction;
	}
	else
	{
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

	if (de_vec3_sqr_len(&velocity) > velocity_limit * velocity_limit)
	{
		de_vec3_normalize(&velocity, &velocity);
		de_vec3_scale(&velocity, &velocity, velocity_limit);

		de_vec3_sub(&body->last_position, &body->position, &velocity);
	}
}

/*=======================================================================================*/
static de_contact_t* de_body_add_contact(de_body_t* body)
{
	if (body->contact_count < DE_MAX_CONTACTS)
	{
		return &body->contacts[body->contact_count++];
	}
	return NULL;
}

/*=======================================================================================*/
void de_body_triangle_collision(de_static_triangle_t* triangle, de_body_t* sphere)
{
	de_vec3_t intersectionPoint;
	de_vec3_t middle, orientation, offset;
	float length, penetrationDepth;
	de_contact_t* contact;

	if (de_sphere_triangle_intersection(&sphere->position, sphere->radius, triangle, &intersectionPoint))
	{
		length = 0.0f;

		/* Calculate penetration depth and push vector */
		de_vec3_sub(&middle, &sphere->position, &intersectionPoint);
		de_vec3_normalize_ex(&orientation, &middle, &length);
		penetrationDepth = sphere->radius - length;

		/* Degenerated case, ignore */
		if (penetrationDepth < 0.0f)
		{
			return;
		}

		/* Push sphere outside of triangle */
		de_vec3_add(&sphere->position, &sphere->position, de_vec3_scale(&offset, &orientation, penetrationDepth));

		/* Write contact info */
		contact = de_body_add_contact(sphere);

		if (contact)
		{
			contact->body = NULL;
			contact->normal = orientation;
			contact->position = intersectionPoint;
			contact->triangle = triangle;
		}
	}
}

/*=======================================================================================*/
void de_physics_step(de_core_t* core, float dt)
{
	float dt2;
	de_scene_t* scene;
	de_body_t* body;
	de_static_geometry_t* geom;

	dt2 = dt * dt;

	DE_LINKED_LIST_FOR_EACH(core->scenes, scene)
	{
		DE_LINKED_LIST_FOR_EACH(scene->bodies, body)
		{
			/* Drop contact information */
			body->contact_count = 0;

			/* Apply gravity */
			de_vec3_add(&body->acceleration, &body->acceleration, &body->gravity);

			/* Do Verlet integration */
			de_body_verlet(body, dt2);

			/* Solve collisions */
			DE_LINKED_LIST_FOR_EACH(scene->static_geometries, geom)
			{
				size_t j;

				for (j = 0; j < geom->triangles.size; ++j)
				{
					de_body_triangle_collision(&geom->triangles.data[j], body);
				}
			}
		}
	}
}

/*=======================================================================================*/
void de_body_set_gravity(de_body_t * body, const de_vec3_t * gravity)
{
	body->gravity = *gravity;
}

/*=======================================================================================*/
void de_body_set_position(de_body_t * body, const de_vec3_t * pos)
{
	body->position = *pos;
	body->last_position = *pos;
}

/*=======================================================================================*/
void de_body_set_radius(de_body_t * body, float radius)
{
	body->radius = radius;
}

/*=======================================================================================*/
float de_body_get_radius(de_body_t* body)
{
	return body->radius;
}

/*=======================================================================================*/
size_t de_body_get_contact_count(de_body_t* body)
{
	return body->contact_count;
}

/*=======================================================================================*/
de_contact_t* de_body_get_contact(de_body_t* body, size_t i)
{
	if (i >= DE_MAX_CONTACTS)
	{
		return NULL;
	}

	return body->contacts + i;
}

/*=======================================================================================*/
void de_body_move(de_body_t * body, const de_vec3_t* velocity)
{
	if (!body || !velocity)
	{
		return;
	}

	de_vec3_add(&body->position, &body->position, velocity);
}

/*=======================================================================================*/
void de_body_set_velocity(de_body_t* body, const de_vec3_t* velocity)
{
	if (!body || !velocity)
	{
		return;
	}

	body->last_position = body->position;

	de_vec3_sub(&body->last_position, &body->last_position, velocity);
}

/*=======================================================================================*/
void de_body_set_y_velocity(de_body_t * body, float y_velocity)
{
	if (!body)
	{
		return;
	}

	body->last_position.y = body->position.y - y_velocity;
}

/*=======================================================================================*/
void de_body_set_x_velocity(de_body_t * body, float x_velocity)
{
	if (!body)
	{
		return;
	}

	body->last_position.x = body->position.x - x_velocity;
}

/*=======================================================================================*/
void de_body_set_z_velocity(de_body_t * body, float z_velocity)
{
	if (!body)
	{
		return;
	}

	body->last_position.z = body->position.z - z_velocity;
}

/*=======================================================================================*/
void de_body_get_velocity(de_body_t * body, de_vec3_t * velocity)
{
	if (!body || velocity)
	{
		return;
	}
	de_vec3_sub(velocity, &body->position, &body->last_position);
}

