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

void de_ray_intersection_parameters_reset(de_ray_intersection_parameters_t* params)
{
	params->t_min = FLT_MAX;
	params->t_max = -FLT_MAX;
}

void de_ray_intersection_parameters_push(de_ray_intersection_parameters_t* params, float t)
{
	if (t > params->t_max) {
		params->t_max = t;
	}
	if (t < params->t_min) {
		params->t_min = t;
	}
}

void de_ray_set(de_ray_t* out, const de_vec3_t* origin, const de_vec3_t* dir)
{
	out->origin = *origin;
	out->dir = *dir;
}

void de_ray_by_two_points(de_ray_t* out, const de_vec3_t* start, const de_vec3_t* end)
{
	out->origin = *start;
	de_vec3_sub(&out->dir, end, start);
}

int de_ray_aabb_intersection(const de_ray_t* ray, const de_vec3_t* min, const de_vec3_t* max, float* out_tmin, float* out_tmax)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (ray->dir.x >= 0) {
		tmin = (min->x - ray->origin.x) / ray->dir.x;
		tmax = (max->x - ray->origin.x) / ray->dir.x;
	} else {
		tmin = (max->x - ray->origin.x) / ray->dir.x;
		tmax = (min->x - ray->origin.x) / ray->dir.x;
	}
	if (ray->dir.y >= 0) {
		tymin = (min->y - ray->origin.y) / ray->dir.y;
		tymax = (max->y - ray->origin.y) / ray->dir.y;
	} else {
		tymin = (max->y - ray->origin.y) / ray->dir.y;
		tymax = (min->y - ray->origin.y) / ray->dir.y;
	}
	if ((tmin > tymax) || (tymin > tmax)) {
		return 0;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}
	if (ray->dir.z >= 0) {
		tzmin = (min->z - ray->origin.z) / ray->dir.z;
		tzmax = (max->z - ray->origin.z) / ray->dir.z;
	} else {
		tzmin = (max->z - ray->origin.z) / ray->dir.z;
		tzmax = (min->z - ray->origin.z) / ray->dir.z;
	}
	if ((tmin > tzmax) || (tzmin > tmax)) {
		return 0;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}
	if (out_tmin) {
		*out_tmin = tmin;
	}
	if (out_tmax) {
		*out_tmax = tmax;
	}
	return ((tmin < 1.0f) && (tmax > 0.0f));
}

float de_ray_plane_intersection(const de_ray_t* ray, const de_plane_t* plane)
{
	/* solve plane equation */
	float u = -(de_vec3_dot(&ray->origin, &plane->n) + plane->d);
	float v = de_vec3_dot(&ray->dir, &plane->n);
	return u / v;
}

de_vec3_t de_ray_evaluate(const de_ray_t* ray, float t)
{
	/* o + d * t */
	de_vec3_t result;
	de_vec3_scale(&result, &ray->dir, t);
	de_vec3_add(&result, &result, &ray->origin);

	return result;
}

bool de_ray_plane_intersection_point(const de_ray_t* ray, const de_plane_t* plane, de_vec3_t* out_intersection_point)
{
	const float t = de_ray_plane_intersection(ray, plane);

	if (t < 0.0f) {
		return false;
	}

	if (out_intersection_point) {
		*out_intersection_point = de_ray_evaluate(ray, t);
	}

	return true;
}

int de_ray_triangle_intersection(const de_ray_t* ray, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, de_vec3_t* out_intersection_point)
{
	de_vec3_t point, ca, ba;
	de_plane_t plane;

	de_vec3_sub(&ba, b, a);
	de_vec3_sub(&ca, c, a);

	/* build plane */
	de_vec3_normalize(&plane.n, de_vec3_cross(&plane.n, &ba, &ca));
	plane.d = -de_vec3_dot(a, &plane.n);

	if (de_ray_plane_intersection_point(ray, &plane, &point)) {
		if (out_intersection_point) {
			*out_intersection_point = point;
		}

		return de_is_point_inside_triangle(&point, a, b, c);
	}

	return 0;
}

bool de_ray_sphere_intersection_point(const de_ray_t* ray, const de_vec3_t* position, float radius, de_vec3_t* out_int_point_a, de_vec3_t* out_int_point_b)
{
	de_ray_intersection_parameters_t parameters;
	de_ray_intersection_parameters_reset(&parameters);

	if (de_ray_sphere_intersection(ray, position, radius, &parameters)) {		
		*out_int_point_a = de_ray_evaluate(ray, parameters.t_min);
		*out_int_point_b = de_ray_evaluate(ray, parameters.t_max);
		return true;
	}

	return false;
}

bool de_ray_sphere_intersection(const de_ray_t* ray, const de_vec3_t* position, float radius, de_ray_intersection_parameters_t* parameters)
{
	de_vec3_t d;
	de_vec3_sub(&d, &ray->origin, position);

	const float a = de_vec3_sqr_len(&ray->dir);
	const float b = 2.0f * de_vec3_dot(&ray->dir, &d);
	const float c = de_vec3_sqr_len(&d) - radius * radius;

	float roots[2];
	if (de_solve_quadratic(a, b, c, roots)) {
		de_ray_intersection_parameters_push(parameters, roots[0]);
		de_ray_intersection_parameters_push(parameters, roots[1]);

		return true;
	}

	/* No real roots. */
	return false;
}

bool de_ray_capsule_intersection(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float radius, de_vec3_t int_points[2])
{
	/* Dumb approach - check intersection with finite cylinder without caps, then check 
	 * two sphere caps. */
	de_ray_intersection_parameters_t parameters;
	de_ray_intersection_parameters_reset(&parameters);

	bool intersection = de_ray_cylinder_intersection(ray, pa, pb, radius, DE_CYLINDER_TYPE_FINITE, &parameters);
	intersection |= de_ray_sphere_intersection(ray, pa, radius, &parameters);
	intersection |= de_ray_sphere_intersection(ray, pb, radius, &parameters);

	int_points[0] = de_ray_evaluate(ray, parameters.t_min);
	int_points[1] = de_ray_evaluate(ray, parameters.t_max);

	return intersection;
}

bool de_ray_cylinder_intersection(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float r, de_cylinder_type_t type, de_ray_intersection_parameters_t* parameters)
{
	/*
	 * https://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
	 *
	 * Infinite cylinder oriented along line pa + va * t:
	 *     sqr_len(q - pa - dot(va, q - pa) * va) - r ^ 2 = 0
	 * where q - point on cylinder, substite q with ray p + v * t:
	 *    sqr_len(p - pa + vt - dot(va, p - pa + vt) * va) - r ^ 2 = 0
	 * reduce to A * t * t + B * t + C = 0 (quadratic equation), where:
	 *    A = sqr_len(v - dot(v, va) * va)
	 *    B = 2 * dot(v - dot(v, va) * va, dp - dot(dp, va) * va)
	 *    C = sqr_len(dp - dot(dp, va) * va) - r ^ 2
	 *    where dp = p - pa
	 * to find intersection points we have to solve quadratic equation
	 * to get root which will be t parameter of ray equation.
	 **/

	de_vec3_t va;
	de_vec3_sub(&va, pb, pa);

	const de_vec3_t* v = &ray->dir;
	const de_vec3_t* p = &ray->origin;

	de_vec3_t vs;
	de_vec3_scale(&vs, &va, de_vec3_dot(v, &va)); /* dot(v, va) * va */

	de_vec3_t vl;
	de_vec3_sub(&vl, v, &vs); /* v - dot(v, va) * va */

	const float A = de_vec3_sqr_len(&vl);

	de_vec3_t dp;
	de_vec3_sub(&dp, p, pa); /* sqr_len(v - dot(v, va) * va) */

	de_vec3_t ds;
	de_vec3_scale(&ds, &va, de_vec3_dot(&dp, &va)); /* dot(dp, va) * va */

	de_vec3_t dpva;
	de_vec3_sub(&dpva, &dp, &ds); /* dp - dot(dp, va) * va */

	const float B = 2.0f * de_vec3_dot(&vl, &dpva); /* 2 * dot(v - dot(v, va) * va, dp - dot(dp, va) * va) */

	const float C = de_vec3_sqr_len(&dpva) - r * r; /* sqr_len(dp - dot(dp, va) * va) - r ^ 2 */

	/* Get roots for cylinder surfaces */
	float cylinder_roots[2] = { 0, 0 };
	const int cylinder_root_count = de_solve_quadratic(A, B, C, cylinder_roots);

	switch (type) {
		case DE_CYLINDER_TYPE_CAPPED: {
			/* In case of cylinder with caps we have to check intersection with caps */
			const de_vec3_t* caps[2] = { pa, pb };

			float cap_roots[2];
			int cap_root_count = 0;

			for (int i = 0; i < 2; ++i) {
				const de_vec3_t* cap_center = caps[i];

				de_vec3_t cap_normal;
				if (i == 0) {
					de_vec3_negate(&cap_normal, &va);
				} else {
					cap_normal = va;
				}

				de_plane_t cap_plane;
				de_plane_set(&cap_plane, cap_center, &cap_normal);

				const float t = de_ray_plane_intersection(ray, &cap_plane);

				if (t > 0) {
					const de_vec3_t intersection = de_ray_evaluate(ray, t);

					if (de_vec3_sqr_distance(cap_center, &intersection) <= r * r) {
						/* Point inside cap bounds */
						cap_roots[cap_root_count++] = t;
					}
				}
			}

			/* Find min and max roots */
			for (int i = 0; i < cylinder_root_count; ++i) {
				de_ray_intersection_parameters_push(parameters, cylinder_roots[i]);
			}

			for (int i = 0; i < cap_root_count; ++i) {
				de_ray_intersection_parameters_push(parameters, cap_roots[i]);
			}

			break;
		}
		case DE_CYLINDER_TYPE_FINITE: {
			/* In case of finite cylinder without caps we have to check that intersection
			 * points on cylinder surface are between two planes of caps. */
			int int_point_count = 0;
			for (int i = 0; i < cylinder_root_count; ++i) {
				const float root = cylinder_roots[i];

				de_vec3_t int_point = de_ray_evaluate(ray, root);

				de_vec3_t da;
				de_vec3_sub(&da, &int_point, pa);

				if (de_vec3_dot(&da, &va) < 0) {
					continue;
				}

				de_vec3_t db;
				de_vec3_sub(&db, pb, &int_point);

				if (de_vec3_dot(&db, &va) < 0) {
					continue;
				}

				de_ray_intersection_parameters_push(parameters, root);
				++int_point_count;
			}

			return int_point_count > 0;
		}
		case DE_CYLINDER_TYPE_INFINITE: {
			/* Infinite cylinder */
			for (int i = 0; i < cylinder_root_count; ++i) {
				de_ray_intersection_parameters_push(parameters, cylinder_roots[i]);
			}

			return cylinder_root_count > 0;
		}
	}

	/* We have no roots, so no intersection. */
	return false;
}

bool de_ray_cylinder_intersection_point(const de_ray_t* ray, const de_vec3_t* pa, const de_vec3_t* pb, float r, de_cylinder_type_t type, de_vec3_t int_points[2])
{
	de_ray_intersection_parameters_t parameters;
	de_ray_intersection_parameters_reset(&parameters);

	if (de_ray_cylinder_intersection(ray, pa, pb, r, type, &parameters) > 0) {
		int_points[0] = de_ray_evaluate(ray, parameters.t_min);
		int_points[1] = de_ray_evaluate(ray, parameters.t_max);
		return true;
	}
	return false;
}