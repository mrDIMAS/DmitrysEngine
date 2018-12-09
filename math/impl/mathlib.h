/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

float de_mat3_at(const de_mat3_t* m, unsigned int row, unsigned int column)
{
	return m->f[row + 3 * column];
}

float de_mat4_at(const de_mat4_t * m, unsigned int row, unsigned int column)
{
	return m->f[row + 4 * column];
}

void de_mat4_identity(de_mat4_t * out)
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
	out->f[12] = 0.0f;
	out->f[13] = 0.0f;
	out->f[14] = 0.0f;
	out->f[15] = 1.0f;
}

void de_mat4_transpose(de_mat4_t * out, const de_mat4_t * m)
{
	int i, j;
	for (j = 0; j < 4; ++j)
	{
		for (i = 0; i < 4; ++i)
		{
			out->f[(j * 4) + i] = m->f[(i * 4) + j];
		}
	}
}

void de_mat4_mul(de_mat4_t * out, const de_mat4_t * a, const de_mat4_t * b)
{
	de_mat4_t temp;

	temp.f[0] = a->f[0] * b->f[0] + a->f[4] * b->f[1] + a->f[8] * b->f[2] + a->f[12] * b->f[3];
	temp.f[1] = a->f[1] * b->f[0] + a->f[5] * b->f[1] + a->f[9] * b->f[2] + a->f[13] * b->f[3];
	temp.f[2] = a->f[2] * b->f[0] + a->f[6] * b->f[1] + a->f[10] * b->f[2] + a->f[14] * b->f[3];
	temp.f[3] = a->f[3] * b->f[0] + a->f[7] * b->f[1] + a->f[11] * b->f[2] + a->f[15] * b->f[3];

	temp.f[4] = a->f[0] * b->f[4] + a->f[4] * b->f[5] + a->f[8] * b->f[6] + a->f[12] * b->f[7];
	temp.f[5] = a->f[1] * b->f[4] + a->f[5] * b->f[5] + a->f[9] * b->f[6] + a->f[13] * b->f[7];
	temp.f[6] = a->f[2] * b->f[4] + a->f[6] * b->f[5] + a->f[10] * b->f[6] + a->f[14] * b->f[7];
	temp.f[7] = a->f[3] * b->f[4] + a->f[7] * b->f[5] + a->f[11] * b->f[6] + a->f[15] * b->f[7];

	temp.f[8] = a->f[0] * b->f[8] + a->f[4] * b->f[9] + a->f[8] * b->f[10] + a->f[12] * b->f[11];
	temp.f[9] = a->f[1] * b->f[8] + a->f[5] * b->f[9] + a->f[9] * b->f[10] + a->f[13] * b->f[11];
	temp.f[10] = a->f[2] * b->f[8] + a->f[6] * b->f[9] + a->f[10] * b->f[10] + a->f[14] * b->f[11];
	temp.f[11] = a->f[3] * b->f[8] + a->f[7] * b->f[9] + a->f[11] * b->f[10] + a->f[15] * b->f[11];

	temp.f[12] = a->f[0] * b->f[12] + a->f[4] * b->f[13] + a->f[8] * b->f[14] + a->f[12] * b->f[15];
	temp.f[13] = a->f[1] * b->f[12] + a->f[5] * b->f[13] + a->f[9] * b->f[14] + a->f[13] * b->f[15];
	temp.f[14] = a->f[2] * b->f[12] + a->f[6] * b->f[13] + a->f[10] * b->f[14] + a->f[14] * b->f[15];
	temp.f[15] = a->f[3] * b->f[12] + a->f[7] * b->f[13] + a->f[11] * b->f[14] + a->f[15] * b->f[15];

	*out = temp;
}

void de_mat4_scale(de_mat4_t * out, const de_vec3_t * v)
{
	out->f[0] = v->x;
	out->f[1] = 0.0f;
	out->f[2] = 0.0f;
	out->f[3] = 0.0f;

	out->f[4] = 0.0f;
	out->f[5] = v->y;
	out->f[6] = 0.0f;
	out->f[7] = 0.0f;

	out->f[8] = 0.0f;
	out->f[9] = 0.0f;
	out->f[10] = v->z;
	out->f[11] = 0.0f;

	out->f[12] = 0.0f;
	out->f[13] = 0.0f;
	out->f[14] = 0.0f;
	out->f[15] = 1.0f;
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

	de_vec3_normalize(&zaxis, de_vec3_sub(&zaxis, at, eye));
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
	if (fabs(det) > 0.00001f)
	{
		det = 1.0f / det;
	}
	for (i = 0; i < 16; i++)
	{
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

static void de_quat_set_at(de_quat_t * quat, float f, unsigned int index)
{
	*(((float*)quat) + index) = f;
}

de_quat_t * de_mat4_to_quat(const de_mat4_t * m, de_quat_t * quat)
{
	de_mat3_t basis;
	float trace;
	float recip;
	float s;
	de_mat4_get_basis(m, &basis);

	trace = basis.f[0] + basis.f[4] + basis.f[8];
	if (trace > 0.0f)
	{
		s = (float)sqrt(trace + 1.0f);
		quat->w = s*0.5f;
		recip = 0.5f / s;
		quat->x = (de_mat3_at(&basis, 2, 1) - de_mat3_at(&basis, 1, 2))*recip;
		quat->y = (de_mat3_at(&basis, 0, 2) - de_mat3_at(&basis, 2, 0))*recip;
		quat->z = (de_mat3_at(&basis, 1, 0) - de_mat3_at(&basis, 0, 1))*recip;
	}
	else
	{
		unsigned int i = 0;
		unsigned int j;
		unsigned int k;
		if (de_mat3_at(&basis, 1, 1) > de_mat3_at(&basis, 0, 0))
		{
			i = 1;
		}
		if (de_mat3_at(&basis, 2, 2) > de_mat3_at(&basis, i, i))
		{
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



de_vec3_t* de_vec3_set(de_vec3_t* out, float x, float y, float z)
{
	out->x = x;
	out->y = y;
	out->z = z;
	return out;
}

de_vec3_t* de_vec3_zero(de_vec3_t* out)
{
	out->x = 0.0f;
	out->y = 0.0f;
	out->z = 0.0f;
	return out;
}

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
	float len = de_vec3_len(a);
	if (len > 0.00001f)
	{
		float k = 1.0f / len;
		out->x = a->x * k;
		out->y = a->y * k;
		out->z = a->z * k;
	}
	return out;
}

de_vec3_t* de_vec3_normalize_ex(de_vec3_t* out, const de_vec3_t* a, float* length)
{
	*length = de_vec3_len(a);
	if (*length > 0.00001f)
	{
		float k = 1.0f / *length;
		out->x = a->x * k;
		out->y = a->y * k;
		out->z = a->z * k;
	}
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
	return (float)acos(de_vec3_dot(a, b) / (de_vec3_len(a) * de_vec3_len(b)));
}

de_vec3_t* de_vec3_project_plane(de_vec3_t* out, const de_vec3_t* point, const de_vec3_t* normal)
{
	de_vec3_t n = { 0 };
	if (de_vec3_sqr_len(normal) < 0.00001f)
	{
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
	if (a->x < vMin->x)
	{
		vMin->x = a->x;
	}
	if (a->y < vMin->y)
	{
		vMin->y = a->y;
	}
	if (a->z < vMin->z)
	{
		vMin->z = a->z;
	}

	if (a->x > vMax->x)
	{
		vMax->x = a->x;
	}
	if (a->y > vMax->y)
	{
		vMax->y = a->y;
	}
	if (a->z > vMax->z)
	{
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
	if (len > 0.00001f)
	{
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
	if (a->x < vMin->x)
	{
		vMin->x = a->x;
	}
	if (a->y < vMin->y)
	{
		vMin->y = a->y;
	}

	if (a->x > vMax->x)
	{
		vMax->x = a->x;
	}
	if (a->y > vMax->y)
	{
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

int de_ray_aabb_intersection(const de_ray_t* ray, const de_aabb_t* aabb, float* out_tmin, float* out_tmax)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (ray->dir.x >= 0)
	{
		tmin = (aabb->min.x - ray->origin.x) / ray->dir.x;
		tmax = (aabb->max.x - ray->origin.x) / ray->dir.x;
	}
	else
	{
		tmin = (aabb->max.x - ray->origin.x) / ray->dir.x;
		tmax = (aabb->min.x - ray->origin.x) / ray->dir.x;
	}
	if (ray->dir.y >= 0)
	{
		tymin = (aabb->min.y - ray->origin.y) / ray->dir.y;
		tymax = (aabb->max.y - ray->origin.y) / ray->dir.y;
	}
	else
	{
		tymin = (aabb->max.y - ray->origin.y) / ray->dir.y;
		tymax = (aabb->min.y - ray->origin.y) / ray->dir.y;
	}
	if ((tmin > tymax) || (tymin > tmax))
	{
		return 0;
	}
	if (tymin > tmin)
	{
		tmin = tymin;
	}
	if (tymax < tmax)
	{
		tmax = tymax;
	}
	if (ray->dir.z >= 0)
	{
		tzmin = (aabb->min.z - ray->origin.z) / ray->dir.z;
		tzmax = (aabb->max.z - ray->origin.z) / ray->dir.z;
	}
	else
	{
		tzmin = (aabb->max.z - ray->origin.z) / ray->dir.z;
		tzmax = (aabb->min.z - ray->origin.z) / ray->dir.z;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
	{
		return 0;
	}
	if (tzmin > tmin)
	{
		tmin = tzmin;
	}
	if (tzmax < tmax)
	{
		tmax = tzmax;
	}
	if (out_tmin)
	{
		*out_tmin = tmin;
	}
	if (out_tmax)
	{
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
	if (out_intersection_point)
	{
		de_vec3_t h;
		de_vec3_scale(&h, &ray->dir, t);
		de_vec3_add(out_intersection_point, &ray->origin, &h);
	}
	return 1;
}

int de_ray_triangle_intersection(const de_ray_t* ray, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c, de_vec3_t* out_intersection_point)
{
	float baDotba, caDotba, caDotca, invDenom, dot02, dot12, u, v;
	de_vec3_t vp, point, ca, ba;
	de_plane_t plane;

	de_vec3_sub(&ba, b, a);
	de_vec3_sub(&ca, c, a);

	/* build plane */
	de_vec3_normalize(&plane.n, de_vec3_cross(&plane.n, &ba, &ca));
	plane.d = -de_vec3_dot(a, &plane.n);

	if (de_ray_plane_intersection(ray, &plane, &point))
	{
		if (out_intersection_point)
		{
			*out_intersection_point = point;
		}

		de_vec3_sub(&vp, &point, a);

		/* check, if intersection point lies in triangle using barycentric
		* coordinates */
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
	if (discriminant < 0.0f)
	{
		return 0;
	}
	discrRoot = (float)sqrt(discriminant);
	/* find roots of quadratic equation */
	r1 = (-b + discrRoot) / 2.0f;
	r2 = (-b - discrRoot) / 2.0f;
	/* write intersection points out if needed */
	if (out_int_point_a)
	{
		de_vec3_add(out_int_point_a, &ray->origin, de_vec3_scale(&rv, &ray->dir, r1));
	}
	if (out_int_point_b)
	{
		de_vec3_add(out_int_point_b, &ray->origin, de_vec3_scale(&rv, &ray->dir, r2));
	}
	return 1;
}

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
	if (fabs(theta) > 0.00001f)
	{
		float d = 1.0f / (float)sin(theta);
		float s0 = (float)sin((1.0f - t) * theta);
		float s1 = (float)sin(t * theta);
		if (de_quat_dot(a, b) < 0)
		{
			q.x = (a->x * s0 - b->x * s1) * d;
			q.y = (a->y * s0 - b->y * s1) * d;
			q.z = (a->z * s0 - b->z * s1) * d;
			q.w = (a->w * s0 - b->w * s1) * d;
		}
		else
		{
			q.x = (a->x * s0 + b->x * s1) * d;
			q.y = (a->y * s0 + b->y * s1) * d;
			q.z = (a->z * s0 + b->z * s1) * d;
			q.w = (a->w * s0 + b->w * s1) * d;
		}
		*out = q;
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
	switch (order)
	{
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
	if (s_squared < 0.0001f)
	{
		de_vec3_set(out_axis, 1.0f, 0.0f, 0.0f);
	}
	else
	{
		float s = 1.0f / (float)sqrt(s_squared);
		de_vec3_set(out_axis, q->x * s, q->y * s, q->z * s);
	}
	return out_axis;
}

float de_quat_get_angle(const de_quat_t* q)
{
	return 2.0f * (float)acos(q->w);
}

de_plane_t* de_plane_setCoef(de_plane_t* p, float a, float b, float c, float d)
{
	p->n.x = a;
	p->n.y = b;
	p->n.z = c;
	de_vec3_normalize(&p->n, &p->n);
	p->d = d;
	return p;
}

de_plane_t* de_plane_set(de_plane_t* p, const de_vec3_t* n, float d)
{
	de_vec3_normalize(&p->n, n);
	p->d = d;
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
	float d = 1.0f / de_vec3_len(&p->n);
	de_vec3_scale(&p->n, &p->n, d);
	p->d *= d;
	return p;
}

de_frustum_t* de_frustum_from_matrix(de_frustum_t* f, const de_mat4_t* m)
{
	de_plane_setCoef(&f->planes[0], m->f[3] + m->f[0], m->f[7] + m->f[4], m->f[11] + m->f[8], m->f[15] + m->f[12]); /* left plane */
	de_plane_setCoef(&f->planes[1], m->f[3] - m->f[0], m->f[7] - m->f[4], m->f[11] - m->f[8], m->f[15] - m->f[12]); /* right plane */
	de_plane_setCoef(&f->planes[2], m->f[3] - m->f[1], m->f[7] - m->f[5], m->f[11] - m->f[9], m->f[15] - m->f[13]); /* top plane */
	de_plane_setCoef(&f->planes[3], m->f[3] + m->f[1], m->f[7] + m->f[5], m->f[11] + m->f[9], m->f[15] + m->f[13]); /* bottom plane */
	de_plane_setCoef(&f->planes[4], m->f[3] - m->f[2], m->f[7] - m->f[6], m->f[11] - m->f[10], m->f[15] - m->f[14]);                                            /* far plane */
	de_plane_setCoef(&f->planes[5], m->f[2], m->f[6], m->f[10], m->f[14]); /* near plane */
	return f;
}

int de_frustum_box_intersection(const de_frustum_t* f, const de_aabb_t* aabb, const de_vec3_t* aabb_offset)
{
	int i, k;
	de_vec3_t aabb_points[8];
	for (i = 0; i < 8; ++i)
	{
		aabb_points[i] = aabb->corners[i];
		if (aabb_offset)
		{
			de_vec3_add(&aabb_points[i], &aabb_points[i], aabb_offset);
		}
	}
	for (i = 0; i < 6; ++i)
	{
		int back_points = 0;
		for (k = 0; k < 8; ++k)
		{
			if (de_plane_dot(&f->planes[i], &aabb_points[k]) <= 0)
			{
				if (++back_points >= 8)
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

int de_frustum_box_intersection_transform(const de_frustum_t* f, const de_aabb_t* aabb, const de_mat4_t* obj_matrix)
{
	int i, k;
	de_vec3_t aabb_points[8];
	for (i = 0; i < 8; ++i)
	{
		aabb_points[i] = aabb->corners[i];
		if (obj_matrix)
		{
			de_vec3_transform(&aabb_points[i], &aabb_points[i], obj_matrix);
		}
	}
	for (i = 0; i < 6; ++i)
	{
		int back_points = 0;
		for (k = 0; k < 8; ++k)
		{
			if (de_plane_dot(&f->planes[i], &aabb_points[k]) <= 0)
			{
				if (++back_points >= 8)
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

int de_frustum_contains_point(const de_frustum_t* f, const de_vec3_t* p)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (de_plane_dot(&f->planes[i], p) <= 0.0f)
		{
			return 0;
		}
	}
	return 1;
}

de_aabb_t* de_aabb_set(de_aabb_t* aabb, const de_vec3_t* min, const de_vec3_t* max)
{
	aabb->min = *min;
	aabb->max = *max;
	return de_aabb_recompute_corners(aabb);
}

de_aabb_t* de_aabb_recompute_corners(de_aabb_t* aabb)
{
	de_vec3_set(&aabb->corners[0], aabb->min.x, aabb->min.y, aabb->min.z);
	de_vec3_set(&aabb->corners[1], aabb->min.x, aabb->min.y, aabb->max.z);
	de_vec3_set(&aabb->corners[2], aabb->max.x, aabb->min.y, aabb->max.z);
	de_vec3_set(&aabb->corners[3], aabb->max.x, aabb->min.y, aabb->min.z);
	de_vec3_set(&aabb->corners[4], aabb->min.x, aabb->max.y, aabb->min.z);
	de_vec3_set(&aabb->corners[5], aabb->min.x, aabb->max.y, aabb->max.z);
	de_vec3_set(&aabb->corners[6], aabb->max.x, aabb->max.y, aabb->max.z);
	de_vec3_set(&aabb->corners[7], aabb->max.x, aabb->max.y, aabb->min.z);
	return aabb;
}

int de_aabb_sphere_intersection(const de_aabb_t* aabb, const de_vec3_t* aabb_offset, const de_vec3_t* position, float radius)
{
	float r2 = radius * radius;
	float dmin = 0;
	de_vec3_t min = aabb->min, max = aabb->max;

	if (aabb_offset)
	{
		de_vec3_add(&min, &min, aabb_offset);
		de_vec3_add(&max, &max, aabb_offset);
	}

	if (position->x < min.x)
	{
		dmin += sdUtilSqr(position->x - min.x);
	}
	else if (position->x > max.x)
	{
		dmin += sdUtilSqr(position->x - max.x);
	}

	if (position->y < min.y)
	{
		dmin += sdUtilSqr(position->y - min.y);
	}
	else if (position->y > max.y)
	{
		dmin += sdUtilSqr(position->y - max.y);
	}

	if (position->z < min.z)
	{
		dmin += sdUtilSqr(position->z - min.z);
	}
	else if (position->z > max.z)
	{
		dmin += sdUtilSqr(position->z - max.z);
	}

	return dmin <= r2 ||
		((position->x >= min.x) && (position->x <= max.x) && (position->y >= min.y) &&
		(position->y <= max.y) && (position->z >= min.z) && (position->z <= max.z));
}

int de_aabb_contains_point(const de_aabb_t* aabb, const de_vec3_t* point)
{
	return point->x >= aabb->min.x && point->x <= aabb->max.x && point->y >= aabb->min.y &&
		point->y <= aabb->max.y && point->z >= aabb->min.z && point->z <= aabb->max.z;
}

int de_aabb_aabb_intersection(const de_aabb_t* aabb, const de_aabb_t* other)
{
	de_vec3_t aabb_size, other_size;
	de_vec3_t aabb_center, other_center;
	de_aabb_get_size(aabb, &aabb_size);
	de_aabb_get_size(other, &other_size);
	de_aabb_get_center(aabb, &aabb_center);
	de_aabb_get_center(other, &other_center);
	/* do aabb intesection test */
	if (fabs(aabb_center.x - other_center.x) > (aabb_size.x + other_size.x))
	{
		return 0;
	}
	if (fabs(aabb_center.y - other_center.y) > (aabb_size.y + other_size.y))
	{
		return 0;
	}
	if (fabs(aabb_center.z - other_center.z) > (aabb_size.z + other_size.z))
	{
		return 0;
	}
	return 1;
}

int de_aabb_triangle_intersection(const de_aabb_t* aabb, const de_vec3_t* a, const de_vec3_t* b, const de_vec3_t* c)
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

de_vec3_t* de_aabb_get_size(const de_aabb_t* aabb, de_vec3_t* size)
{
	return de_vec3_scale(size, de_vec3_sub(size, &aabb->max, &aabb->min), 0.5f);
}

de_vec3_t* de_aabb_get_center(const de_aabb_t* aabb, de_vec3_t* center)
{
	return de_vec3_middle(center, &aabb->min, &aabb->max);
}

de_aabb_t* de_aabb_invalidate(de_aabb_t* aabb)
{
	de_vec3_set(&aabb->max, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	de_vec3_set(&aabb->min, FLT_MAX, FLT_MAX, FLT_MAX);
	return de_aabb_recompute_corners(aabb);
}

float sdUtilSqr(float a)
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
	if (a < min)
	{
		return min;
	}
	if (a > max)
	{
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
	while (v >>= 1)
	{
		power <<= 1;
	}
	power <<= 1;
	return power;
}