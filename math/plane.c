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

de_plane_t* de_plane_set_abcd(de_plane_t* p, float a, float b, float c, float d)
{
	p->n.x = a;
	p->n.y = b;
	p->n.z = c;
	p->d = d;
	de_plane_normalize(p);
	return p;
}

de_plane_t* de_plane_set(de_plane_t* p, const de_vec3_t* plane_point, const de_vec3_t* plane_normal)
{
	de_vec3_normalize(&p->n, plane_normal);
	p->d = -de_vec3_dot(plane_point, &p->n);
	return p;
}

float de_plane_distance(const de_plane_t* p, const de_vec3_t* point)
{
	return (float)fabs(de_vec3_dot(&p->n, point) + p->d);
}

float de_plane_dot(const de_plane_t* p, const de_vec3_t* point)
{
	return de_vec3_dot(&p->n, point) + p->d;
}

de_plane_t* de_plane_normalize(de_plane_t* p)
{
	const float denominator = de_vec3_len(&p->n);
#if DE_MATH_CHECKS
	if (denominator == 0.0f) {
		de_fatal_error("Cannot normalize plane with degenerated normal vector!");
	}
#endif
	const float d = 1.0f / denominator;
	de_vec3_scale(&p->n, &p->n, d);
	p->d *= d;
	return p;
}

de_plane_class_t de_plane_classify(const de_vec3_t * triangle_normal)
{
	float longest = 0.0f;

	de_plane_class_t plane = DE_PLANE_OXY;

	if (fabs(triangle_normal->x) > longest) {
		longest = (float)fabs(triangle_normal->x);
		plane = DE_PLANE_OYZ;
	}

	if (fabs(triangle_normal->y) > longest) {
		longest = (float)fabs(triangle_normal->y);
		plane = DE_PLANE_OXZ;
	}

	if (fabs(triangle_normal->z) > longest) {
		plane = DE_PLANE_OXY;
	}

	return plane;
}