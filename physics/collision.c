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

bool de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c)
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
	de_vec3_cross(&triangle.normal, &triangle.ba, &ca);
	if (de_vec3_sqr_len(&triangle.normal) > FLT_EPSILON) {
		de_vec3_normalize(&triangle.normal, &triangle.normal);
	} else {
		de_log("static geometry: degenerated triangle found!");
		return false;
	}

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

	return true;
}

void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t transform)
{
	if (!geom || !mesh) {
		return;
	}

	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		de_surface_t* surf;

		surf = mesh->surfaces.data[i];

		de_surface_shared_data_t* data = surf->shared_data;
		for (size_t k = 0; k < data->index_count; k += 3) {
			de_vec3_t pa, pb, pc;
			int a, b, c;

			a = data->indices[k];
			b = data->indices[k + 1];
			c = data->indices[k + 2];

			pa = data->positions[a];
			pb = data->positions[b];
			pc = data->positions[c];

			de_vec3_transform(&pa, &pa, &transform);
			de_vec3_transform(&pb, &pb, &transform);
			de_vec3_transform(&pc, &pc, &transform);

			de_static_geometry_add_triangle(geom, &pa, &pb, &pc);
		}
	}

	geom->octree = de_octree_build((char*)geom->triangles.data + offsetof(de_static_triangle_t, a), geom->triangles.size, sizeof(de_static_triangle_t), 64);
}

bool de_static_triangle_contains_point(const de_static_triangle_t* triangle, const de_vec3_t* point)
{
	de_vec3_t vp;
	de_vec3_sub(&vp, point, &triangle->a);
	float dot02 = de_vec3_dot(&triangle->ca, &vp);
	float dot12 = de_vec3_dot(&triangle->ba, &vp);
	float u = (triangle->baDotba * dot02 - triangle->caDotba * dot12) * triangle->invDenom;
	float v = (triangle->caDotca * dot12 - triangle->caDotba * dot02) * triangle->invDenom;
	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

void de_physics_step(de_core_t* core, double dt)
{
	const float dt2 = (float)(dt * dt);
	DE_LINKED_LIST_FOR_EACH_T(de_scene_t*, scene, core->scenes)
	{
		DE_LINKED_LIST_FOR_EACH_T(de_body_t*, body, scene->bodies)
		{
			/* Drop contact information */
			body->contact_count = 0;
			/* Apply gravity */
			de_vec3_add(&body->acceleration, &body->acceleration, &body->gravity);
			/* Do Verlet integration */
			de_body_verlet(body, dt2);
			/* Solve sphere-mesh collisions */
			DE_LINKED_LIST_FOR_EACH_T(de_static_geometry_t*, geom, scene->static_geometries)
			{
				de_octree_trace_sphere(geom->octree, &body->position, body->radius);
				for (int i = 0; i < geom->octree->trace_buffer.size; ++i) {
					de_octree_node_t* node = geom->octree->trace_buffer.nodes[i];
					for (int k = 0; k < node->index_count; ++k) {
						de_body_triangle_collision(&geom->triangles.data[node->triangle_indices[k]], body);
					}
				}
			}
			/* Solve sphere-sphere collisions */
			DE_LINKED_LIST_FOR_EACH_T(de_body_t*, other, scene->bodies)
			{
				if (other != body) {
					de_body_body_collision(body, other);
				}
			}
		}
	}
}

static int de_ray_cast_result_distance_comparer(const void* a, const void* b)
{
	const de_ray_cast_result_t* result_a = a;
	const de_ray_cast_result_t* result_b = b;
	if(result_a->sqr_distance > result_b->sqr_distance) {
		return 1;
	} else if(result_a->sqr_distance < result_b->sqr_distance) {
		return -1;
	}
	return 0;
}

bool de_ray_cast(de_scene_t* scene, const de_ray_t* ray, de_ray_cast_flags_t flags, de_ray_cast_result_array_t* result_array)
{
	bool hit = false;

	/* check bodies */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_BODY)) {
		DE_LINKED_LIST_FOR_EACH_T(de_body_t*, body, scene->bodies)
		{
			if (flags & DE_RAY_CAST_FLAGS_IGNORE_BODY_IN_RAY) {
				if (de_vec3_sqr_distance(&body->position, &ray->origin) <= body->radius * body->radius) {
					continue;
				}
			}

			de_vec3_t intersection_points[2];
			if (de_ray_sphere_intersection(ray, &body->position, body->radius, &intersection_points[0], &intersection_points[1])) {
				for (size_t i = 0; i < 2; ++i) {
					de_ray_cast_result_t* result = DE_ARRAY_GROW(*result_array, 1);
					result->position = intersection_points[i];
					de_vec3_sub(&result->normal, &result->position, &body->position);
					result->body = body;
					result->triangle = NULL;
					result->static_geometry = NULL;
					result->sqr_distance = de_vec3_sqr_distance(&intersection_points[i], &ray->origin);
				}
			}
		}
	}

	/* check static geometries */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_STATIC_GEOMETRY)) {
		DE_LINKED_LIST_FOR_EACH_T(de_static_geometry_t*, geom, scene->static_geometries)
		{
			de_octree_trace_ray(geom->octree, ray);
			for (int i = 0; i < geom->octree->trace_buffer.size; ++i) {
				de_octree_node_t* node = geom->octree->trace_buffer.nodes[i];
				for (int k = 0; k < node->index_count; ++k) {
					de_vec3_t intersection_point;
					de_static_triangle_t* triangle = &geom->triangles.data[node->triangle_indices[k]];
					if (de_ray_triangle_intersection(ray, &triangle->a, &triangle->b, &triangle->c, &intersection_point)) {
						de_ray_cast_result_t* result = DE_ARRAY_GROW(*result_array, 1);
						result->position = intersection_point;
						result->normal = triangle->normal;
						result->body = NULL;
						result->triangle = triangle;
						result->static_geometry = geom;
						result->sqr_distance = de_vec3_sqr_distance(&intersection_point, &ray->origin);
					}
				}
			}
		}
	}

	if (flags & DE_RAY_CAST_FLAGS_SORT_RESULTS) {
		DE_ARRAY_QSORT(*result_array, de_ray_cast_result_distance_comparer);
	}

	return hit;
}

bool de_ray_cast_closest(de_scene_t* scene, const de_ray_t* ray, de_ray_cast_flags_t flags, de_ray_cast_result_t* result)
{
	bool hit = false;

	result->position = (de_vec3_t) { FLT_MAX, FLT_MAX, FLT_MAX };

	float closest_distance = FLT_MAX;

	/* check bodies */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_BODY)) {
		DE_LINKED_LIST_FOR_EACH_T(de_body_t*, body, scene->bodies)
		{
			if (flags & DE_RAY_CAST_FLAGS_IGNORE_BODY_IN_RAY) {
				if (de_vec3_sqr_distance(&body->position, &ray->origin) <= body->radius * body->radius) {
					continue;
				}
			}

			de_vec3_t intersection_points[2];
			if (de_ray_sphere_intersection(ray, &body->position, body->radius, &intersection_points[0], &intersection_points[1])) {
				hit = true;
				for (size_t i = 0; i < 2; ++i) {
					const float new_distance = de_vec3_sqr_distance(&ray->origin, &intersection_points[i]);
					if (new_distance < closest_distance) {
						result->position = intersection_points[i];
						de_vec3_sub(&result->normal, &result->position, &body->position);
						result->body = body;
						result->triangle = NULL;
						result->static_geometry = NULL;
						result->sqr_distance = new_distance;
						closest_distance = new_distance;
					}
				}
			}
		}
	}

	/* check static geometries */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_STATIC_GEOMETRY)) {
		DE_LINKED_LIST_FOR_EACH_T(de_static_geometry_t*, geom, scene->static_geometries)
		{
			de_octree_trace_ray(geom->octree, ray);
			for (int i = 0; i < geom->octree->trace_buffer.size; ++i) {
				de_octree_node_t* node = geom->octree->trace_buffer.nodes[i];
				for (int k = 0; k < node->index_count; ++k) {
					de_vec3_t intersection_point;
					de_static_triangle_t* triangle = &geom->triangles.data[node->triangle_indices[k]];
					if (de_ray_triangle_intersection(ray, &triangle->a, &triangle->b, &triangle->c, &intersection_point)) {
						hit = true;
						const float new_distance = de_vec3_sqr_distance(&ray->origin, &intersection_point);
						if (new_distance < closest_distance) {
							result->position = intersection_point;
							result->normal = triangle->normal;
							result->body = NULL;
							result->triangle = triangle;
							result->static_geometry = geom;
							result->sqr_distance = new_distance;
							closest_distance = new_distance;
						}
					}
				}
			}
		}
	}

	return hit;
}