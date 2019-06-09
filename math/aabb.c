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

void de_aabb_set(de_aabb_t* aabb, const de_vec3_t* min, const de_vec3_t* max)
{
	aabb->min = *min;
	aabb->max = *max;
}

bool de_aabb_sphere_intersection(const de_aabb_t* aabb, const de_vec3_t* aabb_offset, const de_vec3_t* position, float radius)
{
	float r2 = radius * radius;
	float dmin = 0;
	de_vec3_t min = aabb->min, max = aabb->max;

	if (aabb_offset) {
		de_vec3_add(&min, &min, aabb_offset);
		de_vec3_add(&max, &max, aabb_offset);
	}

	if (position->x < min.x) {
		dmin += de_sqr(position->x - min.x);
	} else if (position->x > max.x) {
		dmin += de_sqr(position->x - max.x);
	}

	if (position->y < min.y) {
		dmin += de_sqr(position->y - min.y);
	} else if (position->y > max.y) {
		dmin += de_sqr(position->y - max.y);
	}

	if (position->z < min.z) {
		dmin += de_sqr(position->z - min.z);
	} else if (position->z > max.z) {
		dmin += de_sqr(position->z - max.z);
	}

	return dmin <= r2 ||
		((position->x >= min.x) && (position->x <= max.x) && (position->y >= min.y) &&
		(position->y <= max.y) && (position->z >= min.z) && (position->z <= max.z));
}

bool de_aabb_contains_point(const de_aabb_t* aabb, const de_vec3_t* point)
{
	return point->x >= aabb->min.x && point->x <= aabb->max.x && point->y >= aabb->min.y &&
		point->y <= aabb->max.y && point->z >= aabb->min.z && point->z <= aabb->max.z;
}

bool de_aabb_aabb_intersection(const de_aabb_t* aabb, const de_aabb_t* other)
{
	de_vec3_t aabb_size, other_size;
	de_vec3_t aabb_center, other_center;
	de_aabb_get_size(aabb, &aabb_size);
	de_aabb_get_size(other, &other_size);
	de_aabb_get_center(aabb, &aabb_center);
	de_aabb_get_center(other, &other_center);
	/* do aabb intesection test */
	if (fabs(aabb_center.x - other_center.x) > (aabb_size.x + other_size.x)) {
		return false;
	}
	if (fabs(aabb_center.y - other_center.y) > (aabb_size.y + other_size.y)) {
		return false;
	}
	if (fabs(aabb_center.z - other_center.z) > (aabb_size.z + other_size.z)) {
		return false;
	}
	return true;
}

bool de_aabb_triangle_intersection(const de_aabb_t* aabb, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c)
{
	de_aabb_t triangle_aabb;
	de_aabb_invalidate(&triangle_aabb);
	de_aabb_push_point(&triangle_aabb, a);
	de_aabb_push_point(&triangle_aabb, b);
	de_aabb_push_point(&triangle_aabb, c);
	return de_aabb_aabb_intersection(aabb, &triangle_aabb);
}

de_aabb_t* de_aabb_push_point(de_aabb_t* aabb, const de_vec3_t* p)
{
	de_vec3_min_max(p, &aabb->min, &aabb->max);
	return aabb;
}

void de_aabb_get_size(const de_aabb_t* aabb, de_vec3_t* size)
{
	de_vec3_scale(size, de_vec3_sub(size, &aabb->max, &aabb->min), 0.5f);
}

void de_aabb_get_center(const de_aabb_t* aabb, de_vec3_t* center)
{
	de_vec3_middle(center, &aabb->min, &aabb->max);
}

void de_aabb_invalidate(de_aabb_t* aabb)
{
	aabb->max = (de_vec3_t) { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	aabb->min = (de_vec3_t) { FLT_MAX, FLT_MAX, FLT_MAX };
}

void de_aabb_merge(de_aabb_t* aabb, const de_aabb_t* other)
{
	de_aabb_push_point(aabb, &other->min);
	de_aabb_push_point(aabb, &other->max);
}