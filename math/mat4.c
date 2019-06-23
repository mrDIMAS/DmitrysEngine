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

void de_mat4_get_basis(const de_mat4_t * in, de_mat3_t * out)
{
	out->f[0] = in->f[0];
	out->f[1] = in->f[1];
	out->f[2] = in->f[2];

	out->f[3] = in->f[4];
	out->f[4] = in->f[5];
	out->f[5] = in->f[6];

	out->f[6] = in->f[8];
	out->f[7] = in->f[9];
	out->f[8] = in->f[10];
}

float de_mat4_at(const de_mat4_t * m, unsigned int row, unsigned int column)
{
	return m->f[row + 4 * column];
}

void de_mat4_identity(de_mat4_t * out)
{
	*out = (de_mat4_t)
	{
		.f[0] = 1.0f, .f[5] = 1.0f, .f[10] = 1.0f, .f[15] = 1.0f
	};
}

void de_mat4_transpose(de_mat4_t * out, const de_mat4_t * m)
{
	de_mat4_t temp;
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 4; ++i) {
			temp.f[(j * 4) + i] = m->f[(i * 4) + j];
		}
	}
	*out = temp;
}

void de_mat4_mul(de_mat4_t * out, const de_mat4_t * a, const de_mat4_t * b)
{
	*out = (de_mat4_t)
	{
		.f[0] = a->f[0] * b->f[0] + a->f[4] * b->f[1] + a->f[8] * b->f[2] + a->f[12] * b->f[3],
			.f[1] = a->f[1] * b->f[0] + a->f[5] * b->f[1] + a->f[9] * b->f[2] + a->f[13] * b->f[3],
			.f[2] = a->f[2] * b->f[0] + a->f[6] * b->f[1] + a->f[10] * b->f[2] + a->f[14] * b->f[3],
			.f[3] = a->f[3] * b->f[0] + a->f[7] * b->f[1] + a->f[11] * b->f[2] + a->f[15] * b->f[3],

			.f[4] = a->f[0] * b->f[4] + a->f[4] * b->f[5] + a->f[8] * b->f[6] + a->f[12] * b->f[7],
			.f[5] = a->f[1] * b->f[4] + a->f[5] * b->f[5] + a->f[9] * b->f[6] + a->f[13] * b->f[7],
			.f[6] = a->f[2] * b->f[4] + a->f[6] * b->f[5] + a->f[10] * b->f[6] + a->f[14] * b->f[7],
			.f[7] = a->f[3] * b->f[4] + a->f[7] * b->f[5] + a->f[11] * b->f[6] + a->f[15] * b->f[7],

			.f[8] = a->f[0] * b->f[8] + a->f[4] * b->f[9] + a->f[8] * b->f[10] + a->f[12] * b->f[11],
			.f[9] = a->f[1] * b->f[8] + a->f[5] * b->f[9] + a->f[9] * b->f[10] + a->f[13] * b->f[11],
			.f[10] = a->f[2] * b->f[8] + a->f[6] * b->f[9] + a->f[10] * b->f[10] + a->f[14] * b->f[11],
			.f[11] = a->f[3] * b->f[8] + a->f[7] * b->f[9] + a->f[11] * b->f[10] + a->f[15] * b->f[11],

			.f[12] = a->f[0] * b->f[12] + a->f[4] * b->f[13] + a->f[8] * b->f[14] + a->f[12] * b->f[15],
			.f[13] = a->f[1] * b->f[12] + a->f[5] * b->f[13] + a->f[9] * b->f[14] + a->f[13] * b->f[15],
			.f[14] = a->f[2] * b->f[12] + a->f[6] * b->f[13] + a->f[10] * b->f[14] + a->f[14] * b->f[15],
			.f[15] = a->f[3] * b->f[12] + a->f[7] * b->f[13] + a->f[11] * b->f[14] + a->f[15] * b->f[15]
	};
}

void de_mat4_scale(de_mat4_t * out, const de_vec3_t * v)
{
	*out = (de_mat4_t)
	{
		.f[0] = v->x, .f[5] = v->y, .f[10] = v->z, .f[15] = 1.0f
	};
}

void de_mat4_perspective(de_mat4_t * out, float fov_radians, float aspect, float zNear, float zFar)
{
	float y_scale = 1.0f / (float)tan(fov_radians * 0.5f);
	float x_scale = y_scale / aspect;

	out->f[0] = x_scale;
	out->f[1] = 0;
	out->f[2] = 0;
	out->f[3] = 0;

	out->f[4] = 0;
	out->f[5] = y_scale;
	out->f[6] = 0;
	out->f[7] = 0;

	out->f[8] = 0;
	out->f[9] = 0;
	out->f[10] = zFar / (zNear - zFar);
	out->f[11] = -1;

	out->f[12] = 0;
	out->f[13] = 0;
	out->f[14] = zNear * zFar / (zNear - zFar);
	out->f[15] = 0;
}

void de_mat4_ortho(de_mat4_t * out, float left, float right, float bottom, float top, float zNear, float zFar)
{
	out->f[0] = 2.0f / (right - left);
	out->f[1] = 0.0f;
	out->f[2] = 0.0f;
	out->f[3] = 0.0f;

	out->f[4] = 0.0f;
	out->f[5] = 2.0f / (top - bottom);
	out->f[6] = 0.0f;
	out->f[7] = 0.0f;

	out->f[8] = 0.0f;
	out->f[9] = 0.0f;
	out->f[10] = 1.0f / (zFar - zNear);
	out->f[11] = 0.0f;

	out->f[12] = (left + right) / (left - right);
	out->f[13] = (top + bottom) / (bottom - top);
	out->f[14] = zNear / (zNear - zFar);
	out->f[15] = 1.0f;
}

void de_mat4_translation(de_mat4_t * out, const de_vec3_t * v)
{
	out->f[0] = 1.0f;
	out->f[1] = 0.0f;
	out->f[2] = 0.0f;
	out->f[3] = 0.0f;

	out->f[4] = 0.0f;
	out->f[5] = 1.0f;
	out->f[6] = 0.0f;
	out->f[7] = 0.0f;

	out->f[8] = 0.0f;
	out->f[9] = 0.0f;
	out->f[10] = 1.0f;
	out->f[11] = 0.0f;

	out->f[12] = v->x;
	out->f[13] = v->y;
	out->f[14] = v->z;
	out->f[15] = 1.0f;
}

void de_mat4_look_at(de_mat4_t * out, const de_vec3_t * eye, const de_vec3_t * at, const de_vec3_t * up)
{
	de_vec3_t zaxis, xaxis, yaxis;

	de_vec3_normalize(&zaxis, de_vec3_sub(&zaxis, eye, at));
	de_vec3_normalize(&xaxis, de_vec3_cross(&xaxis, up, &zaxis));
	de_vec3_normalize(&yaxis, de_vec3_cross(&yaxis, &zaxis, &xaxis));

	out->f[0] = xaxis.x;
	out->f[1] = yaxis.x;
	out->f[2] = zaxis.x;
	out->f[3] = 0.0f;

	out->f[4] = xaxis.y;
	out->f[5] = yaxis.y;
	out->f[6] = zaxis.y;
	out->f[7] = 0.0f;

	out->f[8] = xaxis.z;
	out->f[9] = yaxis.z;
	out->f[10] = zaxis.z;
	out->f[11] = 0.0f;

	out->f[12] = -de_vec3_dot(&xaxis, eye);
	out->f[13] = -de_vec3_dot(&yaxis, eye);
	out->f[14] = -de_vec3_dot(&zaxis, eye);
	out->f[15] = 1.0f;
}

void de_mat4_inverse(de_mat4_t * out, const de_mat4_t * a)
{
	int i;
	float det;
	de_mat4_t temp;
	temp.f[0] = a->f[5] * a->f[10] * a->f[15] - a->f[5] * a->f[14] * a->f[11] - a->f[6] * a->f[9] * a->f[15] + a->f[6] * a->f[13] * a->f[11] + a->f[7] * a->f[9] * a->f[14] - a->f[7] * a->f[13] * a->f[10];
	temp.f[1] = -a->f[1] * a->f[10] * a->f[15] + a->f[1] * a->f[14] * a->f[11] + a->f[2] * a->f[9] * a->f[15] - a->f[2] * a->f[13] * a->f[11] - a->f[3] * a->f[9] * a->f[14] + a->f[3] * a->f[13] * a->f[10];
	temp.f[2] = a->f[1] * a->f[6] * a->f[15] - a->f[1] * a->f[14] * a->f[7] - a->f[2] * a->f[5] * a->f[15] + a->f[2] * a->f[13] * a->f[7] + a->f[3] * a->f[5] * a->f[14] - a->f[3] * a->f[13] * a->f[6];
	temp.f[3] = -a->f[1] * a->f[6] * a->f[11] + a->f[1] * a->f[10] * a->f[7] + a->f[2] * a->f[5] * a->f[11] - a->f[2] * a->f[9] * a->f[7] - a->f[3] * a->f[5] * a->f[10] + a->f[3] * a->f[9] * a->f[6];
	temp.f[4] = -a->f[4] * a->f[10] * a->f[15] + a->f[4] * a->f[14] * a->f[11] + a->f[6] * a->f[8] * a->f[15] - a->f[6] * a->f[12] * a->f[11] - a->f[7] * a->f[8] * a->f[14] + a->f[7] * a->f[12] * a->f[10];
	temp.f[5] = a->f[0] * a->f[10] * a->f[15] - a->f[0] * a->f[14] * a->f[11] - a->f[2] * a->f[8] * a->f[15] + a->f[2] * a->f[12] * a->f[11] + a->f[3] * a->f[8] * a->f[14] - a->f[3] * a->f[12] * a->f[10];
	temp.f[6] = -a->f[0] * a->f[6] * a->f[15] + a->f[0] * a->f[14] * a->f[7] + a->f[2] * a->f[4] * a->f[15] - a->f[2] * a->f[12] * a->f[7] - a->f[3] * a->f[4] * a->f[14] + a->f[3] * a->f[12] * a->f[6];
	temp.f[7] = a->f[0] * a->f[6] * a->f[11] - a->f[0] * a->f[10] * a->f[7] - a->f[2] * a->f[4] * a->f[11] + a->f[2] * a->f[8] * a->f[7] + a->f[3] * a->f[4] * a->f[10] - a->f[3] * a->f[8] * a->f[6];
	temp.f[8] = a->f[4] * a->f[9] * a->f[15] - a->f[4] * a->f[13] * a->f[11] - a->f[5] * a->f[8] * a->f[15] + a->f[5] * a->f[12] * a->f[11] + a->f[7] * a->f[8] * a->f[13] - a->f[7] * a->f[12] * a->f[9];
	temp.f[9] = -a->f[0] * a->f[9] * a->f[15] + a->f[0] * a->f[13] * a->f[11] + a->f[1] * a->f[8] * a->f[15] - a->f[1] * a->f[12] * a->f[11] - a->f[3] * a->f[8] * a->f[13] + a->f[3] * a->f[12] * a->f[9];
	temp.f[10] = a->f[0] * a->f[5] * a->f[15] - a->f[0] * a->f[13] * a->f[7] - a->f[1] * a->f[4] * a->f[15] + a->f[1] * a->f[12] * a->f[7] + a->f[3] * a->f[4] * a->f[13] - a->f[3] * a->f[12] * a->f[5];
	temp.f[11] = -a->f[0] * a->f[5] * a->f[11] + a->f[0] * a->f[9] * a->f[7] + a->f[1] * a->f[4] * a->f[11] - a->f[1] * a->f[8] * a->f[7] - a->f[3] * a->f[4] * a->f[9] + a->f[3] * a->f[8] * a->f[5];
	temp.f[12] = -a->f[4] * a->f[9] * a->f[14] + a->f[4] * a->f[13] * a->f[10] + a->f[5] * a->f[8] * a->f[14] - a->f[5] * a->f[12] * a->f[10] - a->f[6] * a->f[8] * a->f[13] + a->f[6] * a->f[12] * a->f[9];
	temp.f[13] = a->f[0] * a->f[9] * a->f[14] - a->f[0] * a->f[13] * a->f[10] - a->f[1] * a->f[8] * a->f[14] + a->f[1] * a->f[12] * a->f[10] + a->f[2] * a->f[8] * a->f[13] - a->f[2] * a->f[12] * a->f[9];
	temp.f[14] = -a->f[0] * a->f[5] * a->f[14] + a->f[0] * a->f[13] * a->f[6] + a->f[1] * a->f[4] * a->f[14] - a->f[1] * a->f[12] * a->f[6] - a->f[2] * a->f[4] * a->f[13] + a->f[2] * a->f[12] * a->f[5];
	temp.f[15] = a->f[0] * a->f[5] * a->f[10] - a->f[0] * a->f[9] * a->f[6] - a->f[1] * a->f[4] * a->f[10] + a->f[1] * a->f[8] * a->f[6] + a->f[2] * a->f[4] * a->f[9] - a->f[2] * a->f[8] * a->f[5];
	det = a->f[0] * temp.f[0] + a->f[4] * temp.f[1] + a->f[8] * temp.f[2] + a->f[12] * temp.f[3];
	if (fabs(det) > 0.00001f) {
		det = 1.0f / det;
	}
	for (i = 0; i < 16; i++) {
		out->f[i] = temp.f[i] * det;
	}
}

void de_mat4_rotation(de_mat4_t * out, const de_quat_t * q)
{
	float s = 2.0f / de_quat_sqr_len(q);

	float xs = q->x * s;
	float ys = q->y * s;
	float zs = q->z * s;

	float wx = q->w * xs;
	float wy = q->w * ys;
	float wz = q->w * zs;

	float xx = q->x * xs;
	float xy = q->x * ys;
	float xz = q->x * zs;

	float yy = q->y * ys;
	float yz = q->y * zs;
	float zz = q->z * zs;

	out->f[0] = 1.0f - (yy + zz);
	out->f[4] = xy - wz;
	out->f[8] = xz + wy;
	out->f[12] = 0.0f;

	out->f[1] = xy + wz;
	out->f[5] = 1.0f - (xx + zz);
	out->f[9] = yz - wx;
	out->f[13] = 0.0f;

	out->f[2] = xz - wy;
	out->f[6] = yz + wx;
	out->f[10] = 1.0f - (xx + yy);
	out->f[14] = 0.0f;

	out->f[3] = 0.0f;
	out->f[7] = 0.0f;
	out->f[11] = 0.0f;
	out->f[15] = 1.0f;
}

void de_mat4_rotation_x(de_mat4_t * out, float angle_radians)
{
	float c = (float)cos(angle_radians);
	float s = (float)sin(angle_radians);

	out->f[0] = 1.0f;
	out->f[1] = 0.0f;
	out->f[2] = 0.0f;
	out->f[3] = 0.0f;

	out->f[4] = 0.0f;
	out->f[5] = c;
	out->f[6] = s;
	out->f[7] = 0.0f;

	out->f[8] = 0.0f;
	out->f[9] = -s;
	out->f[10] = c;
	out->f[11] = 0.0f;

	out->f[12] = 0.0f;
	out->f[13] = 0.0f;
	out->f[14] = 0.0f;
	out->f[15] = 1.0f;
}

void de_mat4_rotation_y(de_mat4_t * out, float angle_radians)
{
	float c = (float)cos(angle_radians);
	float s = (float)sin(angle_radians);

	out->f[0] = c;
	out->f[1] = 0.0f;
	out->f[2] = -s;
	out->f[3] = 0.0f;

	out->f[4] = 0.0f;
	out->f[5] = 1.0f;
	out->f[6] = 0.0f;
	out->f[7] = 0.0f;

	out->f[8] = s;
	out->f[9] = 0.0f;
	out->f[10] = c;
	out->f[11] = 0.0f;

	out->f[12] = 0.0f;
	out->f[13] = 0.0f;
	out->f[14] = 0.0f;
	out->f[15] = 1.0f;
}

void de_mat4_rotation_z(de_mat4_t * out, float angle_radians)
{
	float c = (float)cos(angle_radians);
	float s = (float)sin(angle_radians);

	out->f[0] = c;
	out->f[1] = s;
	out->f[2] = 0.0f;
	out->f[3] = 0.0f;

	out->f[4] = -s;
	out->f[5] = c;
	out->f[6] = 0.0f;
	out->f[7] = 0.0f;

	out->f[8] = 0.0f;
	out->f[9] = 0.0f;
	out->f[10] = 1.0f;
	out->f[11] = 0.0f;

	out->f[12] = 0.0f;
	out->f[13] = 0.0f;
	out->f[14] = 0.0f;
	out->f[15] = 1.0f;
}

de_vec3_t * de_mat4_up(const de_mat4_t * m, de_vec3_t * up)
{
	up->x = m->f[4];
	up->y = m->f[5];
	up->z = m->f[6];
	return up;
}

de_vec3_t * de_mat4_side(const de_mat4_t * m, de_vec3_t * side)
{
	side->x = m->f[0];
	side->y = m->f[1];
	side->z = m->f[2];
	return side;
}

de_vec3_t * de_mat4_look(const de_mat4_t * m, de_vec3_t * look)
{
	look->x = m->f[8];
	look->y = m->f[9];
	look->z = m->f[10];
	return look;
}