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


de_vec2_t* de_vec2_set(de_vec2_t* out, float x, float y)
{
	out->x = x;
	out->y = y;
	return out;
}

de_vec2_t* de_vec2_zero(de_vec2_t* out)
{
	out->x = 0.0f;
	out->y = 0.0f;
	return out;
}

de_vec2_t* de_vec2_add(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b)
{
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	return out;
}

de_vec2_t* de_vec2_sub(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	return out;
}

de_vec2_t* de_vec2_div(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b)
{
	out->x = a->x / b->x;
	out->y = a->y / b->y;
	return out;
}

de_vec2_t* de_vec2_mul(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b)
{
	out->x = a->x * b->x;
	out->y = a->y * b->y;
	return out;
}

de_vec2_t* de_vec2_scale(de_vec2_t* out, const de_vec2_t* a, float s)
{
	out->x = a->x * s;
	out->y = a->y * s;
	return out;
}

de_vec2_t* de_vec2_negate(de_vec2_t* out, const de_vec2_t* a)
{
	out->x = -a->x;
	out->y = -a->y;
	return out;
}

float de_vec2_dot(const de_vec2_t* a, const de_vec2_t* b)
{
	return a->x * b->x + a->y * b->y;
}

float de_vec2_len(const de_vec2_t* a)
{
	return (float)sqrt(a->x * a->x + a->y * a->y);
}

float de_vec2_sqr_len(const de_vec2_t* a)
{
	return a->x * a->x + a->y * a->y;
}

void de_vec2_perp(de_vec2_t* out, const de_vec2_t* a)
{
	out->x = a->y;
	out->y = -a->x;
}

de_vec2_t* de_vec2_normalize(de_vec2_t* out, const de_vec2_t* a)
{
	float len = de_vec2_len(a);
	if (len > 0.00001f) {
		float k = 1.0f / len;
		out->x = a->x * k;
		out->y = a->y * k;
	}
	return out;
}

de_vec2_t* de_vec2_lerp(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b, float t)
{
	out->x = a->x + (b->x - a->x) * t;
	out->y = a->y + (b->y - a->y) * t;
	return out;
}

float de_vec2_distance(const de_vec2_t* a, const de_vec2_t* b)
{
	de_vec2_t d;
	return de_vec2_len(de_vec2_sub(&d, a, b));
}

float de_vec2_sqr_distance(const de_vec2_t* a, const de_vec2_t* b)
{
	de_vec2_t d;
	return de_vec2_sqr_len(de_vec2_sub(&d, a, b));
}

float de_vec2_angle(const de_vec2_t* a, const de_vec2_t* b)
{
	return (float)acos(de_vec2_dot(a, b) / (de_vec2_len(a) * de_vec2_len(b)));
}

de_vec2_t* de_vec2_middle(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b)
{
	de_vec2_t v;
	de_vec2_add(&v, a, b);
	de_vec2_scale(out, &v, 0.5f);
	return out;
}

void de_vec2_min_max(const de_vec2_t* a, de_vec2_t* vMin, de_vec2_t* vMax)
{
	if (a->x < vMin->x) {
		vMin->x = a->x;
	}
	if (a->y < vMin->y) {
		vMin->y = a->y;
	}

	if (a->x > vMax->x) {
		vMax->x = a->x;
	}
	if (a->y > vMax->y) {
		vMax->y = a->y;
	}
}

int de_vec2_approx_equals(const de_vec2_t* a, const de_vec2_t* b)
{
	return fabs(a->x - b->x) < 0.0001f && fabs(a->y - b->y) < 0.0001f;
}

int de_vec2_equals(const de_vec2_t* a, const de_vec2_t* b)
{
	return a->x == b->x && a->y == b->y;
}