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

de_ray_t* de_ray_set(de_ray_t* out, const de_vec3_t* origin, const de_vec3_t* dir)
{
	out->origin = *origin;
	out->dir = *dir;
	return out;
}

de_ray_t* de_ray_by_two_points(de_ray_t* out, const de_vec3_t* start, const de_vec3_t* end)
{
	out->origin = *start;
	de_vec3_sub(&out->dir, end, start);
	return out;
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

int de_ray_plane_intersection(const de_ray_t* ray, const de_plane_t* plane, de_vec3_t* out_intersection_point)
{
	/* solve plane equation */
	float u = -(de_vec3_dot(&ray->origin, &plane->n) + plane->d);
	float v = de_vec3_dot(&ray->dir, &plane->n);
	float t = u / v;
	/* ray miss */
	if (t < 0.0f)
		return 0;
	/* find intersection point */
	if (out_intersection_point) {
		de_vec3_t h;
		de_vec3_scale(&h, &ray->dir, t);
		de_vec3_add(out_intersection_point, &ray->origin, &h);
	}
	return 1;
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

	if (de_ray_plane_intersection(ray, &plane, &point)) {
		if (out_intersection_point) {
			*out_intersection_point = point;
		}

		return de_is_point_inside_triangle(&point, a, b, c);
	}

	return 0;
}

int de_ray_sphere_intersection(const de_ray_t* ray, const de_vec3_t* position, float radius, de_vec3_t* out_int_point_a, de_vec3_t* out_int_point_b)
{
	de_vec3_t d, rv;
	float a, b, c, discriminant, discrRoot, r1, r2;
	de_vec3_sub(&d, &ray->origin, position);
	a = de_vec3_dot(&ray->dir, &ray->dir);
	b = 2.0f * de_vec3_dot(&ray->dir, &d);
	c = de_vec3_dot(&d, &d) - radius * radius;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0f) {
		return 0;
	}
	discrRoot = (float)sqrt(discriminant);
	/* find roots of quadratic equation */
	r1 = (-b + discrRoot) / 2.0f;
	r2 = (-b - discrRoot) / 2.0f;
	/* write intersection points out if needed */
	if (out_int_point_a) {
		de_vec3_add(out_int_point_a, &ray->origin, de_vec3_scale(&rv, &ray->dir, r1));
	}
	if (out_int_point_b) {
		de_vec3_add(out_int_point_b, &ray->origin, de_vec3_scale(&rv, &ray->dir, r2));
	}
	return 1;
}
