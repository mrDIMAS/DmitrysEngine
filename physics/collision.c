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

bool de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, uint32_t material_hash)
{
	de_static_triangle_t triangle;

	if (!de_try_get_triangle_normal(&triangle.normal, a, b, c)) {
		de_log("static geometry: degenerated triangle found!");
		return false;
	}

	triangle.a = *a;
	triangle.b = *b;
	triangle.c = *c;
	triangle.material_hash = material_hash;

	DE_ARRAY_APPEND(geom->triangles, triangle);

	return true;
}

void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t* transform)
{
	DE_ASSERT(geom);
	DE_ASSERT(mesh);
	DE_ASSERT(transform);

	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		const de_surface_t* surf = mesh->surfaces.data[i];
		const de_surface_shared_data_t* data = surf->shared_data;
		uint32_t material_hash;
		if (surf->diffuse_map) {
			de_resource_t* resource = de_resource_from_texture(surf->diffuse_map);
			material_hash = de_path_hash(&resource->source);
		} else {
			material_hash = 0;
		}
		for (size_t k = 0; k < data->index_count; k += 3) {
			const int a = data->indices[k];
			const int b = data->indices[k + 1];
			const int c = data->indices[k + 2];

			de_vec3_t pa = data->positions[a];
			de_vec3_transform(&pa, &pa, transform);

			de_vec3_t pb = data->positions[b];
			de_vec3_transform(&pb, &pb, transform);

			de_vec3_t pc = data->positions[c];
			de_vec3_transform(&pc, &pc, transform);

			de_static_geometry_add_triangle(geom, &pa, &pb, &pc, material_hash);
		}
	}

	geom->octree = de_octree_build((char*)geom->triangles.data + offsetof(de_static_triangle_t, a), geom->triangles.size, sizeof(de_static_triangle_t), 64);
}

void de_physics_step(de_core_t* core, double dt)
{
	const float dt2 = (float)(dt * dt);
	for (de_scene_t* scene = core->scenes.head; scene; scene = scene->next) {
		for(de_body_t* body = scene->bodies.head; body; body = body->next) {
			/* Drop contact information */
			body->contact_count = 0;
			/* Apply gravity */
			de_vec3_add(&body->acceleration, &body->acceleration, &body->gravity);
			/* Do Verlet integration */
			de_body_verlet(body, dt2);
			/* Solve body-mesh collisions */
			for (de_static_geometry_t* geom = scene->static_geometries.head; geom; geom = geom->next) {

				/* TODO: */
				const float radius = 3.0f; 

				de_octree_trace_sphere(geom->octree, &body->position, radius);
				for (int i = 0; i < geom->octree->trace_buffer.size; ++i) {
					de_octree_node_t* node = geom->octree->trace_buffer.nodes[i];
					for (int k = 0; k < node->index_count; ++k) {
						de_static_triangle_t* triangle = &geom->triangles.data[node->triangle_indices[k]];
						de_convex_shape_t triangle_shape = {
							.type = DE_CONVEX_SHAPE_TYPE_TRIANGLE,
							.s.triangle = {
							    .vertices = { triangle->a, triangle->b, triangle->c }
						    }
						};
						de_simplex_t simplex = { 0 };
						if (de_gjk_is_intersects(&body->shape, &body->position, &triangle_shape, &(de_vec3_t) { 0, 0, 0}, &simplex)) {
							de_vec3_t penetration_vector;
							de_vec3_t contact_point;
							if (de_epa_get_penetration_info(&simplex, &body->shape, &body->position, &triangle_shape, &(de_vec3_t) { 0, 0, 0}, &penetration_vector, &contact_point)) {
								de_vec3_sub(&body->position, &body->position, &penetration_vector);
								/* Write contact info only if we have contact that really pushes the body */
								if (de_vec3_sqr_len(&penetration_vector)) {								    
									de_contact_t* contact = de_body_add_contact(body);
									if (contact) {
										contact->body = NULL;
										de_vec3_negate(&contact->normal, &penetration_vector);
										de_vec3_normalize(&contact->normal, &contact->normal);
										contact->position = contact_point;
										contact->triangle = triangle;
										contact->geometry = geom;
									}
								}
							}
						}
					}
				}
			}

			/* Solve body-body collisions */
			for(de_body_t* other = scene->bodies.head; other; other = other->next) {
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
	if (result_a->sqr_distance > result_b->sqr_distance) {
		return 1;
	} else if (result_a->sqr_distance < result_b->sqr_distance) {
		return -1;
	}
	return 0;
}

bool de_ray_cast(de_scene_t* scene, const de_ray_t* ray, de_ray_cast_flags_t flags, de_ray_cast_result_array_t* result_array)
{
	DE_ARRAY_CLEAR(*result_array);

	/* Check bodies */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_BODY)) {
		for(de_body_t* body = scene->bodies.head; body; body = body->next) {
			switch (body->shape.type) {
				case DE_CONVEX_SHAPE_TYPE_SPHERE: {
					de_sphere_shape_t* sphere = de_convex_shape_to_sphere(&body->shape);

					if (flags & DE_RAY_CAST_FLAGS_IGNORE_BODY_IN_RAY) {
						if (de_vec3_sqr_distance(&body->position, &ray->origin) <= sphere->radius * sphere->radius) {
							continue;
						}
					}

					de_vec3_t intersection_points[2];
					if (de_ray_sphere_intersection_point(ray, &body->position, sphere->radius, &intersection_points[0], &intersection_points[1])) {
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
					break;
				}
				case DE_CONVEX_SHAPE_TYPE_TRIANGLE: {
					de_triangle_shape_t* triangle = de_convex_shape_to_triangle(&body->shape);

					const de_vec3_t* a = &triangle->vertices[0];
					const de_vec3_t* b = &triangle->vertices[1];
					const de_vec3_t* c = &triangle->vertices[2];

					de_vec3_t intersection_point;
					if(de_ray_triangle_intersection(ray, a, b, c, &intersection_point)) {
						de_ray_cast_result_t* result = DE_ARRAY_GROW(*result_array, 1);
						result->position = intersection_point;
						
						/* TODO: Not sure if this can fail after finding an intersection, need a 
						 * closer look, leave this as is for now. */
						de_vec3_t normal;
						if (de_try_get_triangle_normal(&normal, a, b, c)) {
							result->normal = normal;
						} else {
							de_log("ray cast: degenerated triangle detected! normal will be wrong!");						
							result->normal = (de_vec3_t) { 0, 1, 0 };
						}						

						result->sqr_distance = de_vec3_sqr_distance(&intersection_point, &ray->origin);
						result->body = body;
						result->triangle = NULL;
						result->static_geometry = NULL;
					}
					break;
				}
				case DE_CONVEX_SHAPE_TYPE_CAPSULE: {
					de_capsule_shape_t* capsule = de_convex_shape_to_capsule(&body->shape);

					de_vec3_t pa, pb;
					de_capsule_shape_get_extreme_points(capsule, &pa, &pb);
					
					de_vec3_add(&pa, &pa, &body->position);
					de_vec3_add(&pb, &pb, &body->position);
										
					de_vec3_t intersection_points[2];
					if (de_ray_capsule_intersection(ray, &pa, &pb, capsule->radius, intersection_points)) {
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
					break;
				}
				case DE_CONVEX_SHAPE_TYPE_POINT_CLOUD: {
					/* TODO: Implement this. This requires to build convex hull from point cloud first
					 * i.e. by gift wrapping algorithm or some other more efficient algorithms -
					 * https://dccg.upc.edu/people/vera/wp-content/uploads/2014/11/GA2014-ConvexHulls3D-Roger-Hernando.pdf */
					break;
				}
				default: {
					de_fatal_error("ray cast: forgot to implement type %d", body->shape.type);
				}
			}
		}
	}

	/* Check static geometries */
	if (!(flags & DE_RAY_CAST_FLAGS_IGNORE_STATIC_GEOMETRY)) {
		for(de_static_geometry_t* geom = scene->static_geometries.head; geom; geom = geom->next) {
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

	return result_array->size > 0;
}
