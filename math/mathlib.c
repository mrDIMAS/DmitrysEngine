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

#include "math/aabb.c"
#include "math/frustum.c"
#include "math/quat.c"
#include "math/mat4.c"
#include "math/mat3.c"
#include "math/vec3.c"
#include "math/plane.c"
#include "math/vec2.c"
#include "math/ray.c"

de_quat_t * de_mat4_to_quat(const de_mat4_t * m, de_quat_t * quat)
{
	de_mat3_t basis;
	float trace;
	float recip;
	float s;
	de_mat4_get_basis(m, &basis);

	trace = basis.f[0] + basis.f[4] + basis.f[8];
	if (trace > 0.0f) {
		s = (float)sqrt(trace + 1.0f);
		quat->w = s*0.5f;
		recip = 0.5f / s;
		quat->x = (de_mat3_at(&basis, 2, 1) - de_mat3_at(&basis, 1, 2))*recip;
		quat->y = (de_mat3_at(&basis, 0, 2) - de_mat3_at(&basis, 2, 0))*recip;
		quat->z = (de_mat3_at(&basis, 1, 0) - de_mat3_at(&basis, 0, 1))*recip;
	} else {
		unsigned int i = 0;
		unsigned int j;
		unsigned int k;
		if (de_mat3_at(&basis, 1, 1) > de_mat3_at(&basis, 0, 0)) {
			i = 1;
		}
		if (de_mat3_at(&basis, 2, 2) > de_mat3_at(&basis, i, i)) {
			i = 2;
		}
		j = (i + 1) % 3;
		k = (j + 1) % 3;
		s = (float)sqrt(de_mat3_at(&basis, i, i) - de_mat3_at(&basis, j, j) - de_mat3_at(&basis, k, k) + 1.0f);
		de_quat_set_at(quat, 0.5f*s, i);
		recip = 0.5f / s;
		quat->w = (de_mat3_at(&basis, k, j) - de_mat3_at(&basis, j, k))*recip;
		de_quat_set_at(quat, (de_mat3_at(&basis, j, i) + de_mat3_at(&basis, i, j))*recip, j);
		de_quat_set_at(quat, (de_mat3_at(&basis, k, i) + de_mat3_at(&basis, i, k))*recip, k);
	}

	return quat;
}

bool de_is_point_inside_triangle(const de_vec3_t* point, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c)
{
	float baDotba, caDotba, caDotca, invDenom, dot02, dot12, u, v;
	de_vec3_t vp, ca, ba;

	de_vec3_sub(&ba, b, a);
	de_vec3_sub(&ca, c, a);

	de_vec3_sub(&vp, point, a);

	baDotba = de_vec3_dot(&ba, &ba);
	caDotba = de_vec3_dot(&ca, &ba);
	caDotca = de_vec3_dot(&ca, &ca);

	dot02 = de_vec3_dot(&ca, &vp);
	dot12 = de_vec3_dot(&ba, &vp);

	invDenom = 1.0f / (caDotca * baDotba - caDotba * caDotba);

	/* calculate barycentric coordinates */
	u = (baDotba * dot02 - caDotba * dot12) * invDenom;
	v = (caDotca * dot12 - caDotba * dot02) * invDenom;

	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

bool de_is_point_inside_triangle_2D(const de_vec2_t* point, const de_vec2_t* a, const de_vec2_t* b, const de_vec2_t* c)
{
	float baDotba, caDotba, caDotca, invDenom, dot02, dot12, u, v;
	de_vec2_t vp, ca, ba;

	de_vec2_sub(&ba, b, a);
	de_vec2_sub(&ca, c, a);

	de_vec2_sub(&vp, point, a);

	baDotba = de_vec2_dot(&ba, &ba);
	caDotba = de_vec2_dot(&ca, &ba);
	caDotca = de_vec2_dot(&ca, &ca);

	dot02 = de_vec2_dot(&ca, &vp);
	dot12 = de_vec2_dot(&ba, &vp);

	invDenom = 1.0f / (caDotca * baDotba - caDotba * caDotba);

	/* calculate barycentric coordinates */
	u = (baDotba * dot02 - caDotba * dot12) * invDenom;
	v = (caDotca * dot12 - caDotba * dot02) * invDenom;

	return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

float de_sqr(float a)
{
	return a * a;
}

float de_rad_to_deg(float a)
{
	return a * 180.0f / 3.14159f;
}

float de_deg_to_rad(float a)
{
	return a * 3.14159f / 180.0f;
}

float de_lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

float de_clamp(float a, float min, float max)
{
	if (a < min) {
		return min;
	}
	if (a > max) {
		return max;
	}
	return a;
}

float de_maxf(float a, float b)
{
	return a > b ? a : b;
}

float de_minf(float a, float b)
{
	return a < b ? a : b;
}

unsigned int de_ceil_pow2(unsigned int v)
{
	unsigned int power = 1;
	while (v >>= 1) {
		power <<= 1;
	}
	power <<= 1;
	return power;
}

float de_fwrap(float n, float min_limit, float max_limit)
{
	float offset, num_of_max;

	if (n >= min_limit && n <= max_limit) {
		return n;
	}

	if (max_limit == 0.0f && min_limit == 0.0f) {
		return 0.0f;
	}

	max_limit = max_limit - min_limit;

	offset = min_limit;
	min_limit = 0;
	n = n - offset;

	num_of_max = (float)floor(fabs(n / max_limit));

	if (n >= max_limit) {
		n = n - num_of_max * max_limit;
	} else if (n < min_limit) {
		n = ((num_of_max + 1.0f) * max_limit) + n;
	}

	return n + offset;
}

void de_get_polygon_normal(const de_vec3_t* points, size_t count, de_vec3_t* normal)
{
	size_t i, j;
	*normal = (de_vec3_t) { 0 };
	for (i = 0; i < count; ++i) {
		j = (i + 1) % count;
		normal->x += (points[i].y - points[j].y) * (points[i].z + points[j].z);
		normal->y += (points[i].z - points[j].z) * (points[i].x + points[j].x);
		normal->z += (points[i].x - points[j].x) * (points[i].y + points[j].y);
	}
	if (de_vec3_sqr_len(normal) > FLT_EPSILON) {
		de_vec3_normalize(normal, normal);
	} else {
		de_log("unable to get normal of degenerated polygon");
		*normal = (de_vec3_t) { 0, 1, 0 };
	}
}

double de_get_signed_triangle_area(const de_vec2_t* v1, const de_vec2_t* v2, const de_vec2_t* v3)
{
	return 0.5 * (v1->x * ((double)v3->y - v2->y) + v2->x * ((double)v1->y - v3->y) + v3->x * ((double)v2->y - v1->y));
}

bool de_line_line_intersection(const de_vec3_t* a_begin, const de_vec3_t* a_end, const de_vec3_t* b_begin, const de_vec3_t* b_end, de_vec3_t *out)
{
	float s1x = a_end->x - a_begin->x;
	float s1y = a_end->y - a_begin->y;
	float s2x = b_end->x - b_begin->x;
	float s2y = b_end->y - b_begin->y;
	float s = (-s1y * (a_begin->x - b_begin->x) + s1x * (a_begin->y - b_begin->y)) / (-s2x * s1y + s1x * s2y);
	float t = (s2x * (a_begin->y - b_begin->y) - s2y * (a_begin->x - b_begin->x)) / (-s2x * s1y + s1x * s2y);
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		if (out) {
			out->x = a_begin->x + (t * s1x);
			out->y = a_begin->y + (t * s1y);
		}
		return true;
	}

	return false;
}

void de_vec3_to_vec2_by_plane(de_plane_class_t plane, const de_vec3_t* normal, const de_vec3_t * point, de_vec2_t * mapped)
{
	if (plane == DE_PLANE_OXY) {
		if (normal->z < 0) {
			mapped->x = point->y;
			mapped->y = point->x;
		} else {
			mapped->x = point->x;
			mapped->y = point->y;
		}
	} else if (plane == DE_PLANE_OXZ) {
		if (normal->y < 0) {
			mapped->x = point->x;
			mapped->y = point->z;
		} else {
			mapped->x = point->z;
			mapped->y = point->x;
		}
	} else if (plane == DE_PLANE_OYZ) {
		if (normal->x < 0) {
			mapped->x = point->z;
			mapped->y = point->y;
		} else {
			mapped->x = point->y;
			mapped->y = point->z;
		}
	}
}

float de_frand(float min, float max)
{
	return min + rand() / (float)RAND_MAX * (max - min);
}

int de_irand(int min, int max)
{
	return min + rand() * (max - min) / (int)RAND_MAX;
}

de_vec3_t de_point_cloud_get_farthest_point(const de_vec3_t* points, int count, const de_vec3_t* dir)
{
	int n_farthest = 0;
	float max_dot = -FLT_MAX;
	for (int i = 0; i < count; ++i) {
		float dot = de_vec3_dot(dir, &points[i]);
		if (dot > max_dot) {
			n_farthest = i;
			max_dot = dot;
		}
	}
	return points[n_farthest];
}

void de_get_barycentric_coords(const de_vec3_t* p, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, float *u, float *v, float *w)
{
	de_vec3_t v0;
	de_vec3_sub(&v0, b, a);

	de_vec3_t v1;
	de_vec3_sub(&v1, c, a);

	de_vec3_t v2;
	de_vec3_sub(&v2, p, a);

	float d00 = de_vec3_dot(&v0, &v0);
	float d01 = de_vec3_dot(&v0, &v1);
	float d11 = de_vec3_dot(&v1, &v1);
	float d20 = de_vec3_dot(&v2, &v0);
	float d21 = de_vec3_dot(&v2, &v1);
	float denom = d00 * d11 - d01 * d01;
	*v = (d11 * d20 - d01 * d21) / denom;
	*w = (d00 * d21 - d01 * d20) / denom;
	*u = 1.0f - *v - *w;
}