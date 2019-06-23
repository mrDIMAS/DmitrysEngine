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

de_frustum_t* de_frustum_from_matrix(de_frustum_t* f, const de_mat4_t* m)
{
	de_plane_set_abcd(&f->planes[0], m->f[3] + m->f[0], m->f[7] + m->f[4], m->f[11] + m->f[8], m->f[15] + m->f[12]); /* left plane */
	de_plane_set_abcd(&f->planes[1], m->f[3] - m->f[0], m->f[7] - m->f[4], m->f[11] - m->f[8], m->f[15] - m->f[12]); /* right plane */
	de_plane_set_abcd(&f->planes[2], m->f[3] - m->f[1], m->f[7] - m->f[5], m->f[11] - m->f[9], m->f[15] - m->f[13]); /* top plane */
	de_plane_set_abcd(&f->planes[3], m->f[3] + m->f[1], m->f[7] + m->f[5], m->f[11] + m->f[9], m->f[15] + m->f[13]); /* bottom plane */
	de_plane_set_abcd(&f->planes[4], m->f[3] - m->f[2], m->f[7] - m->f[6], m->f[11] - m->f[10], m->f[15] - m->f[14]); /* far plane */
	de_plane_set_abcd(&f->planes[5], m->f[3] + m->f[2], m->f[7] + m->f[6], m->f[11] + m->f[10], m->f[15] + m->f[14]); /* near plane */
	return f;
}

bool de_frustum_point_cloud_intersection(const de_frustum_t* f, const de_vec3_t* points, size_t count)
{
	for (int i = 0; i < 6; ++i) {
		size_t back_points = 0;
		for (size_t k = 0; k < count; ++k) {
			if (de_plane_dot(&f->planes[i], &points[k]) <= 0) {
				if (++back_points >= count) {
					return false;
				}
			}
		}
	}
	return true;
}

bool de_frustum_box_intersection(const de_frustum_t* f, const de_aabb_t* aabb)
{
	DE_ASSERT(f);
	DE_ASSERT(aabb);

	const de_vec3_t corners[8] = {
		{ aabb->min.x, aabb->min.y, aabb->min.z },
		{ aabb->min.x, aabb->min.y, aabb->max.z },
		{ aabb->max.x, aabb->min.y, aabb->max.z },
		{ aabb->max.x, aabb->min.y, aabb->min.z },
		{ aabb->min.x, aabb->max.y, aabb->min.z },
		{ aabb->min.x, aabb->max.y, aabb->max.z },
		{ aabb->max.x, aabb->max.y, aabb->max.z },
		{ aabb->max.x, aabb->max.y, aabb->min.z },
	};

	return de_frustum_point_cloud_intersection(f, corners, 8);
}

bool de_frustum_box_intersection_offset(const de_frustum_t* f, const de_aabb_t* aabb, const de_vec3_t* aabb_offset)
{
	DE_ASSERT(f);
	DE_ASSERT(aabb);
	DE_ASSERT(aabb_offset);

	const de_vec3_t corners[8] = {
		{ aabb->min.x + aabb_offset->x, aabb->min.y + aabb_offset->y, aabb->min.z + aabb_offset->z },
		{ aabb->min.x + aabb_offset->x, aabb->min.y + aabb_offset->y, aabb->max.z + aabb_offset->z },
		{ aabb->max.x + aabb_offset->x, aabb->min.y + aabb_offset->y, aabb->max.z + aabb_offset->z },
		{ aabb->max.x + aabb_offset->x, aabb->min.y + aabb_offset->y, aabb->min.z + aabb_offset->z },
		{ aabb->min.x + aabb_offset->x, aabb->max.y + aabb_offset->y, aabb->min.z + aabb_offset->z },
		{ aabb->min.x + aabb_offset->x, aabb->max.y + aabb_offset->y, aabb->max.z + aabb_offset->z },
		{ aabb->max.x + aabb_offset->x, aabb->max.y + aabb_offset->y, aabb->max.z + aabb_offset->z },
		{ aabb->max.x + aabb_offset->x, aabb->max.y + aabb_offset->y, aabb->min.z + aabb_offset->z },
	};

	return de_frustum_point_cloud_intersection(f, corners, 8);
}

bool de_frustum_box_intersection_transform(const de_frustum_t* f, const de_aabb_t* aabb, const de_mat4_t* world_matrix)
{
	DE_ASSERT(f);
	DE_ASSERT(aabb);
	DE_ASSERT(world_matrix);

	de_vec3_t corners[8] = {
		{ aabb->min.x, aabb->min.y, aabb->min.z },
		{ aabb->min.x, aabb->min.y, aabb->max.z },
		{ aabb->max.x, aabb->min.y, aabb->max.z },
		{ aabb->max.x, aabb->min.y, aabb->min.z },
		{ aabb->min.x, aabb->max.y, aabb->min.z },
		{ aabb->min.x, aabb->max.y, aabb->max.z },
		{ aabb->max.x, aabb->max.y, aabb->max.z },
		{ aabb->max.x, aabb->max.y, aabb->min.z },
	};

	/* Transform points into world space */
	for (int i = 0; i < 8; ++i) {
		de_vec3_transform(&corners[i], &corners[i], world_matrix);
	}

	return de_frustum_point_cloud_intersection(f, corners, 8);
}

bool de_frustum_contains_point(const de_frustum_t* f, const de_vec3_t* p)
{
	for (int i = 0; i < 6; i++) {
		if (de_plane_dot(&f->planes[i], p) <= 0.0f) {
			return false;
		}
	}
	return true;
}

bool de_frustum_sphere_intersection(const de_frustum_t* f, const de_vec3_t* p, float r)
{
	for (int i = 0; i < 6; ++i) {
		float d = de_plane_dot(&f->planes[i], p);
		if (d < -r) {
			return false;
		}
		if ((float)fabs(d) < r) {
			return true;
		}
	}
	return true;
}