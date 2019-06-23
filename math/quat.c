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

de_quat_t* de_quat_set(de_quat_t* out, float x, float y, float z, float w)
{
	out->x = x;
	out->y = y;
	out->z = z;
	out->w = w;
	return out;
}

de_quat_t* de_quat_from_axis_angle(de_quat_t* out, const de_vec3_t* axis, float angle)
{
	float half_angle = angle * 0.5f;
	float d = de_vec3_len(axis);
	float s = (float)sin(half_angle) / d;
	out->x = axis->x * s;
	out->y = axis->y * s;
	out->z = axis->z * s;
	out->w = (float)cos(half_angle);
	return out;
}

float de_quat_dot(const de_quat_t* a, const de_quat_t* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

float de_quat_len(const de_quat_t* a)
{
	return (float)sqrt(a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w);
}

float de_quat_sqr_len(const de_quat_t* a)
{
	return a->x * a->x + a->y * a->y + a->z * a->z + a->w * a->w;
}

float de_quat_angle(const de_quat_t* a, const de_quat_t* b)
{
	float s = (float)sqrt(de_quat_sqr_len(a) * de_quat_sqr_len(b));
	return (float)acos(de_quat_dot(a, b) / s);
}

de_quat_t* de_quat_slerp(de_quat_t* out, const de_quat_t* a, const de_quat_t* b, float t)
{
	de_quat_t q;
	float theta = de_quat_angle(a, b);
	if (fabs(theta) > 0.00001f) {
		float d = 1.0f / (float)sin(theta);
		float s0 = (float)sin((1.0f - t) * theta);
		float s1 = (float)sin(t * theta);
		if (de_quat_dot(a, b) < 0) {
			q.x = (a->x * s0 - b->x * s1) * d;
			q.y = (a->y * s0 - b->y * s1) * d;
			q.z = (a->z * s0 - b->z * s1) * d;
			q.w = (a->w * s0 - b->w * s1) * d;
		} else {
			q.x = (a->x * s0 + b->x * s1) * d;
			q.y = (a->y * s0 + b->y * s1) * d;
			q.z = (a->z * s0 + b->z * s1) * d;
			q.w = (a->w * s0 + b->w * s1) * d;
		}
		*out = q;
	} else {
		*out = *a;
	}
	return out;
}

void de_quat_from_euler(de_quat_t * out, const de_vec3_t * euler_radians, de_euler_t order)
{
	static de_vec3_t x = { 1, 0, 0 };
	static de_vec3_t y = { 0, 1, 0 };
	static de_vec3_t z = { 0, 0, 1 };
	de_quat_t qx, qy, qz;
	de_quat_from_axis_angle(&qx, &x, euler_radians->x);
	de_quat_from_axis_angle(&qy, &y, euler_radians->y);
	de_quat_from_axis_angle(&qz, &z, euler_radians->z);
	switch (order) {
		case DE_EULER_XYZ:
			de_quat_mul(out, &qz, &qy);
			de_quat_mul(out, out, &qx);
			break;
		case DE_EULER_XZY:
			de_quat_mul(out, &qy, &qz);
			de_quat_mul(out, out, &qx);
			break;
		case DE_EULER_YZX:
			de_quat_mul(out, &qx, &qz);
			de_quat_mul(out, out, &qy);
			break;
		case DE_EULER_YXZ:
			de_quat_mul(out, &qz, &qx);
			de_quat_mul(out, out, &qy);
			break;
		case DE_EULER_ZXY:
			de_quat_mul(out, &qy, &qx);
			de_quat_mul(out, out, &qz);
			break;
		case DE_EULER_ZYX:
			de_quat_mul(out, &qx, &qy);
			de_quat_mul(out, out, &qz);
			break;
	}
}

de_quat_t* de_quat_mul(de_quat_t* out, const de_quat_t* a, const de_quat_t* b)
{
	de_quat_t t;
	t.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
	t.y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
	t.z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
	t.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
	*out = t;
	return out;
}

de_quat_t* de_quat_normalize(de_quat_t* out, de_quat_t* a)
{
	float k = 1.0f / de_quat_len(a);
	out->x *= k;
	out->y *= k;
	out->z *= k;
	out->w *= k;
	return out;
}

de_vec3_t* de_quat_get_axis(const de_quat_t* q, de_vec3_t* out_axis)
{
	float s_squared = 1.0f - q->w * q->w;
	if (s_squared < 0.0001f) {
		*out_axis = (de_vec3_t) { 1.0f, 0.0f, 0.0f };
	} else {
		float s = 1.0f / (float)sqrt(s_squared);
		*out_axis = (de_vec3_t) { q->x * s, q->y * s, q->z * s };
	}
	return out_axis;
}

float de_quat_get_angle(const de_quat_t* q)
{
	return 2.0f * (float)acos(q->w);
}

bool de_quat_equals(const de_quat_t* a, const de_quat_t* b)
{
	return a->x == b->x && a->y == b->y && a->z == b->z && a->w == b->w;
}

void de_quat_set_at(de_quat_t * quat, float f, unsigned int index)
{
	*(((float*)quat) + index) = f;
}