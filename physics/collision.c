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

void de_static_geometry_add_triangle(de_static_geometry_t* geom, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c) {
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

void de_static_geometry_fill(de_static_geometry_t* geom, const de_mesh_t* mesh, const de_mat4_t transform) {
	size_t i, k;

	if (!geom || !mesh) {
		return;
	}

	for (i = 0; i < mesh->surfaces.size; ++i) {
		de_surface_t* surf;

		surf = mesh->surfaces.data[i];

		for (k = 0; k < surf->indices.size; k += 3) {
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

bool de_static_triangle_contains_point(const de_static_triangle_t* triangle, const de_vec3_t* point) {
	de_vec3_t vp;
	float dot02, dot12, u, v;

	de_vec3_sub(&vp, point, &triangle->a);
	dot02 = de_vec3_dot(&triangle->ca, &vp);
	dot12 = de_vec3_dot(&triangle->ba, &vp);
	u = (triangle->baDotba * dot02 - triangle->caDotba * dot12) * triangle->invDenom;
	v = (triangle->caDotca * dot12 - triangle->caDotba * dot02) * triangle->invDenom;
	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

void de_physics_step(de_core_t* core, double dt) {		
	const float dt2 = (float)(dt * dt);
	for (size_t i = 0; i < core->scenes.size; ++i) {
		de_scene_t* scene = core->scenes.data[i];
		DE_LINKED_LIST_FOR_EACH_T(de_body_t*, body, scene->bodies) {
			/* Drop contact information */
			body->contact_count = 0;
			/* Apply gravity */
			de_vec3_add(&body->acceleration, &body->acceleration, &body->gravity);
			/* Do Verlet integration */
			de_body_verlet(body, dt2);
			/* Solve collisions */
			de_static_geometry_t* geom;
			DE_LINKED_LIST_FOR_EACH(scene->static_geometries, geom) {				
				for (size_t j = 0; j < geom->triangles.size; ++j) {
					de_body_triangle_collision(&geom->triangles.data[j], body);
				}
			}
		}
	}
}
