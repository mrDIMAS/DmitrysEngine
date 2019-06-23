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

de_vec3_t* de_vec3_add(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
	return out;
}

de_vec3_t* de_vec3_sub(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
	return out;
}

de_vec3_t* de_vec3_div(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	out->x = a->x / b->x;
	out->y = a->y / b->y;
	out->z = a->z / b->z;
	return out;
}

de_vec3_t* de_vec3_mul(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	out->x = a->x * b->x;
	out->y = a->y * b->y;
	out->z = a->z * b->z;
	return out;
}

de_vec3_t* de_vec3_scale(de_vec3_t* out, const de_vec3_t* a, float s)
{
	out->x = a->x * s;
	out->y = a->y * s;
	out->z = a->z * s;
	return out;
}

de_vec3_t* de_vec3_negate(de_vec3_t* out, const de_vec3_t* a)
{
	out->x = -a->x;
	out->y = -a->y;
	out->z = -a->z;
	return out;
}

float de_vec3_dot(const de_vec3_t* a, const de_vec3_t* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

de_vec3_t* de_vec3_cross(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	de_vec3_t v;
	v.x = a->y * b->z - a->z * b->y;
	v.y = a->z * b->x - a->x * b->z;
	v.z = a->x * b->y - a->y * b->x;
	out->x = v.x;
	out->y = v.y;
	out->z = v.z;
	return out;
}

float de_vec3_len(const de_vec3_t* a)
{
	return (float)sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
}

float de_vec3_sqr_len(const de_vec3_t* a)
{
	return a->x * a->x + a->y * a->y + a->z * a->z;
}

de_vec3_t* de_vec3_normalize(de_vec3_t* out, const de_vec3_t* a)
{
	float k;
	float len = de_vec3_len(a);
#if DE_MATH_CHECKS
	DE_CHECK_DENOMINATOR(len);
#endif
	k = 1.0f / len;
	out->x = a->x * k;
	out->y = a->y * k;
	out->z = a->z * k;
	return out;
}

de_vec3_t* de_vec3_normalize_ex(de_vec3_t* out, const de_vec3_t* a, float* length)
{
	float k;
	*length = de_vec3_len(a);
#if DE_MATH_CHECKS
	DE_CHECK_DENOMINATOR(*length);
#endif
	k = 1.0f / *length;
	out->x = a->x * k;
	out->y = a->y * k;
	out->z = a->z * k;
	return out;
}

de_vec3_t* de_vec3_lerp(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b, float t)
{
	out->x = a->x + (b->x - a->x) * t;
	out->y = a->y + (b->y - a->y) * t;
	out->z = a->z + (b->z - a->z) * t;
	return out;
}

float de_vec3_distance(const de_vec3_t* a, const de_vec3_t* b)
{
	de_vec3_t d;
	return de_vec3_len(de_vec3_sub(&d, a, b));
}

float de_vec3_sqr_distance(const de_vec3_t* a, const de_vec3_t* b)
{
	de_vec3_t d;
	return de_vec3_sqr_len(de_vec3_sub(&d, a, b));
}

float de_vec3_angle(const de_vec3_t* a, const de_vec3_t* b)
{
	float denominator = de_vec3_len(a) * de_vec3_len(b);
#if DE_MATH_CHECKS
	DE_CHECK_DENOMINATOR(denominator);
#endif
	return (float)acos(de_vec3_dot(a, b) / denominator);
}

de_vec3_t* de_vec3_project_plane(de_vec3_t* out, const de_vec3_t* point, const de_vec3_t* normal)
{
	de_vec3_t n = { 0 };
	if (de_vec3_sqr_len(normal) < 0.00001f) {
		return out; /* degenerated normal vector */
	}
	de_vec3_normalize(&n, normal);
	de_vec3_scale(&n, &n, -de_vec3_dot(&n, point));
	return de_vec3_add(out, point, &n);
}

de_vec3_t* de_vec3_reflect(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* normal)
{
	de_vec3_t v;
	de_vec3_scale(&v, normal, 2.0f * de_vec3_dot(a, normal));
	return de_vec3_sub(out, a, &v);
}

de_vec3_t* de_vec3_transform(de_vec3_t* out, const de_vec3_t* a, const de_mat4_t* mat)
{
	de_vec3_t v;
	v.x = a->x * mat->f[0] + a->y * mat->f[4] + a->z * mat->f[8] + mat->f[12];
	v.y = a->x * mat->f[1] + a->y * mat->f[5] + a->z * mat->f[9] + mat->f[13];
	v.z = a->x * mat->f[2] + a->y * mat->f[6] + a->z * mat->f[10] + mat->f[14];
	out->x = v.x;
	out->y = v.y;
	out->z = v.z;
	return out;
}

de_vec3_t* de_vec3_transform_normal(de_vec3_t* out, const de_vec3_t* a, const de_mat4_t* mat)
{
	de_vec3_t v;
	v.x = a->x * mat->f[0] + a->y * mat->f[4] + a->z * mat->f[8];
	v.y = a->x * mat->f[1] + a->y * mat->f[5] + a->z * mat->f[9];
	v.z = a->x * mat->f[2] + a->y * mat->f[6] + a->z * mat->f[10];
	out->x = v.x;
	out->y = v.y;
	out->z = v.z;
	return out;
}

de_vec3_t* de_vec3_middle(de_vec3_t* out, const de_vec3_t* a, const de_vec3_t* b)
{
	de_vec3_t v;
	de_vec3_add(&v, a, b);
	de_vec3_scale(out, &v, 0.5f);
	return out;
}

void de_vec3_min_max(const de_vec3_t* a, de_vec3_t* vMin, de_vec3_t* vMax)
{
	if (a->x < vMin->x) {
		vMin->x = a->x;
	}
	if (a->y < vMin->y) {
		vMin->y = a->y;
	}
	if (a->z < vMin->z) {
		vMin->z = a->z;
	}

	if (a->x > vMax->x) {
		vMax->x = a->x;
	}
	if (a->y > vMax->y) {
		vMax->y = a->y;
	}
	if (a->z > vMax->z) {
		vMax->z = a->z;
	}
}

int de_vec3_approx_equals(const de_vec3_t* a, const de_vec3_t* b)
{
	return fabs(a->x - b->x) < 0.0001f && fabs(a->y - b->y) < 0.0001f &&
		fabs(a->z - b->z) < 0.0001f;
}

int de_vec3_equals(const de_vec3_t* a, const de_vec3_t* b)
{
	return a->x == b->x && a->y == b->y && a->z == b->z;
}