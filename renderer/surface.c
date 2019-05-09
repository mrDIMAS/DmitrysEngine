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

de_surface_shared_data_t* de_surface_shared_data_create(size_t vertex_capacity, size_t index_capacity)
{
	de_surface_shared_data_t* data = DE_NEW(de_surface_shared_data_t);

	data->vertex_count = 0;
	data->vertex_capacity = vertex_capacity;
	data->positions = de_malloc(vertex_capacity * sizeof(*data->positions));
	data->normals = de_malloc(vertex_capacity * sizeof(*data->normals));
	data->tex_coords = de_malloc(vertex_capacity * sizeof(*data->tex_coords));
	data->tangents = de_malloc(vertex_capacity * sizeof(*data->tangents));
	data->bone_indices = de_malloc(vertex_capacity * sizeof(*data->bone_indices));
	data->bone_weights = de_malloc(vertex_capacity * sizeof(*data->bone_weights));

	data->index_count = 0;
	data->index_capacity = index_capacity;
	data->indices = de_malloc(index_capacity * sizeof(*data->indices));

	return data;
}

void de_surface_shared_data_grow_vertices(de_surface_shared_data_t* data, size_t vertex_inc)
{
	if (data->vertex_count + vertex_inc >= data->vertex_capacity) {
		data->vertex_capacity = 2 * data->vertex_capacity + 1;
		data->positions = de_realloc(data->positions, sizeof(*data->positions) * data->vertex_capacity);
		data->normals = de_realloc(data->normals, sizeof(*data->normals) * data->vertex_capacity);
		data->tex_coords = de_realloc(data->tex_coords, sizeof(*data->tex_coords) * data->vertex_capacity);
		data->tangents = de_realloc(data->tangents, sizeof(*data->tangents) * data->vertex_capacity);
		data->bone_indices = de_realloc(data->bone_indices, sizeof(*data->bone_indices) * data->vertex_capacity);
		data->bone_weights = de_realloc(data->bone_weights, sizeof(*data->bone_weights) * data->vertex_capacity);
	}
}

void de_surface_shared_data_grow_indices(de_surface_shared_data_t* data, size_t index_inc)
{
	if (data->index_count + index_inc >= data->index_capacity) {
		data->index_capacity = 2 * data->index_capacity + 1;
		data->indices = de_realloc(data->indices, sizeof(*data->indices) * data->index_capacity);
	}
}

void de_surface_data_shrink_to_fit(de_surface_shared_data_t* data)
{
	if (data->vertex_capacity != data->vertex_count) {
		data->vertex_capacity = data->vertex_count;
		data->positions = de_realloc(data->positions, sizeof(*data->positions) * data->vertex_capacity);
		data->normals = de_realloc(data->normals, sizeof(*data->normals) * data->vertex_capacity);
		data->tex_coords = de_realloc(data->tex_coords, sizeof(*data->tex_coords) * data->vertex_capacity);
		data->tangents = de_realloc(data->tangents, sizeof(*data->tangents) * data->vertex_capacity);
		data->bone_indices = de_realloc(data->bone_indices, sizeof(*data->bone_indices) * data->vertex_capacity);
		data->bone_weights = de_realloc(data->bone_weights, sizeof(*data->bone_weights) * data->vertex_capacity);
	}

	if (data->index_capacity != data->index_count) {
		data->index_capacity = data->index_count;
		data->indices = de_realloc(data->indices, sizeof(*data->indices) * data->index_count);
	}
}

void de_surface_shared_data_free(de_surface_shared_data_t* data)
{
	de_free(data->positions);
	de_free(data->normals);
	de_free(data->tex_coords);
	de_free(data->tangents);
	de_free(data->indices);
	de_free(data->bone_indices);
	de_free(data->bone_weights);
	de_free(data);
}

de_surface_t* de_surface_copy(de_surface_t* surf)
{
	de_surface_t* copy = DE_NEW(de_surface_t);
	copy->renderer = surf->renderer;
	if (surf->diffuse_map) {
		de_resource_add_ref(de_resource_from_texture(surf->diffuse_map));
		copy->diffuse_map = surf->diffuse_map;
	}
	if (surf->normal_map) {
		de_resource_add_ref(de_resource_from_texture(surf->normal_map));
		copy->normal_map = surf->normal_map;
	}
	copy->need_upload = true;
	de_surface_set_data(copy, surf->shared_data);
	/* Raw copy, needs further bone nodes remapping */
	DE_ARRAY_COPY(surf->bones, copy->bones);
	return copy;
}

void de_surface_shared_data_release(de_surface_shared_data_t* data)
{
	if (data) {
		--data->ref_count;
		if (data->ref_count <= 0) {
			de_surface_shared_data_free(data);
		}
	}
}

void de_surface_set_data(de_surface_t* surf, de_surface_shared_data_t* data)
{
	de_surface_shared_data_release(surf->shared_data);
	++data->ref_count;
	surf->shared_data = data;
	surf->need_upload = true;
}

void de_surface_upload(de_surface_t* surf)
{
	surf->need_upload = true;
}

void de_surface_set_diffuse_texture(de_surface_t * surf, de_texture_t *tex)
{
	if (!surf || !tex) {
		return;
	}

	if (surf->diffuse_map) {
		de_resource_release(de_resource_from_texture(surf->diffuse_map));
	}

	de_resource_add_ref(de_resource_from_texture(tex));

	surf->diffuse_map = tex;
}

void de_surface_set_normal_texture(de_surface_t * surf, de_texture_t *tex)
{
	if (!surf || !tex) {
		return;
	}

	if (surf->normal_map) {
		de_resource_release(de_resource_from_texture(surf->normal_map));
	}

	de_resource_add_ref(de_resource_from_texture(tex));

	surf->normal_map = tex;
}

void de_surface_set_specular_texture(de_surface_t * surf, de_texture_t* tex)
{
	if (!surf || !tex) {
		return;
	}

	if (surf->specular_map) {
		de_resource_release(de_resource_from_texture(surf->specular_map));
	}

	de_resource_add_ref(de_resource_from_texture(tex));

	surf->specular_map = tex;
}

void de_surface_calculate_normals(de_surface_t * surf)
{
	de_surface_shared_data_t* data = surf->shared_data;
	for (size_t m = 0; m < data->index_count; m += 3) {
		int ia, ib, ic;
		de_vec3_t *a, *b, *c;
		de_vec3_t ab, ac;
		de_vec3_t normal;

		ia = data->indices[m];
		ib = data->indices[m + 1];
		ic = data->indices[m + 2];

		a = &data->positions[ia];
		b = &data->positions[ib];
		c = &data->positions[ic];

		de_vec3_sub(&ab, b, a);
		de_vec3_sub(&ac, c, a);

		de_vec3_cross(&normal, &ab, &ac);
		de_vec3_normalize(&normal, &normal);

		data->normals[ia] = normal;
		data->normals[ib] = normal;
		data->normals[ic] = normal;
	}
}

bool de_surface_prepare_vertices_for_skinning(de_surface_t* surf)
{
	de_surface_shared_data_t* data = surf->shared_data;

	/* ensure that surface can be skinned */
	if (surf->vertex_weights.size != data->vertex_count) {
		return false;
	}

	/* problem: gpu knows only index of matrix for each vertex that it should apply to vertex
	 * to do skinning, but on engine side we want to be able to use scene nodes as bones,
	 * so here we just calculating index of affecting bone of a vertex, so index will point
	 * to matrix that gpu will use.
	 * */

	 /* map bone nodes to bone indices */
	memset(data->bone_weights, 0, data->vertex_count * sizeof(*data->bone_weights));
	for (size_t i = 0; i < data->vertex_count; ++i) {
		de_vertex_weight_group_t* weight_group = surf->vertex_weights.data + i;
		for (size_t k = 0; k < weight_group->weight_count; ++k) {
			de_vertex_weight_t* vertex_weight = weight_group->bones + k;
			data->bone_indices[i].indices[k] = (uint8_t)de_surface_get_bone_index(surf, vertex_weight->node);
			data->bone_weights[i].weights[k] = vertex_weight->weight;
		}
	}

	return true;
}

bool de_surface_add_bone(de_surface_t* surf, de_node_t* bone)
{
	size_t i;

	for (i = 0; i < surf->bones.size; ++i) {
		if (surf->bones.data[i] == bone) {
			return false;
		}
	}

	DE_ARRAY_APPEND(surf->bones, bone);

	return true;
}

int de_surface_get_bone_index(de_surface_t* surf, de_node_t* bone)
{
	size_t i;

	for (i = 0; i < surf->bones.size; ++i) {
		if (surf->bones.data[i] == bone) {
			return i;
		}
	}

	de_log("error: no such bone %s found in surface's bones!", de_str8_cstr(&bone->name));

	return -1;
}

void de_surface_get_skinning_matrices(de_surface_t* surf, de_mat4_t* out_matrices, size_t max_matrices)
{
	size_t i;

	for (i = 0; i < surf->bones.size && i < max_matrices; ++i) {
		de_node_t* bone_node = surf->bones.data[i];
		de_mat4_t* m = out_matrices + i;
		if (bone_node) {
			de_mat4_mul(m, &bone_node->global_matrix, &bone_node->inv_bind_pose_matrix);
		} else {
			de_mat4_identity(m);
		}
	}
}

bool de_surface_is_skinned(de_surface_t* surf)
{
	return surf->bones.size > 0;
}

void de_surface_calculate_tangents(de_surface_t* surf)
{
	de_surface_shared_data_t* data = surf->shared_data;
	de_vec3_t *tan1 = (de_vec3_t *)de_calloc(data->vertex_count * 2, sizeof(de_vec3_t));
	de_vec3_t *tan2 = tan1 + data->vertex_count;

	for (size_t i = 0; i < data->index_count; i += 3) {
		de_vec3_t sdir, tdir;

		int i1 = data->indices[i + 0];
		int i2 = data->indices[i + 1];
		int i3 = data->indices[i + 2];

		const de_vec3_t* v1 = &data->positions[i1];
		const de_vec3_t* v2 = &data->positions[i2];
		const de_vec3_t* v3 = &data->positions[i3];

		const de_vec2_t* w1 = &data->tex_coords[i1];
		const de_vec2_t* w2 = &data->tex_coords[i2];
		const de_vec2_t* w3 = &data->tex_coords[i3];

		float x1 = v2->x - v1->x;
		float x2 = v3->x - v1->x;
		float y1 = v2->y - v1->y;
		float y2 = v3->y - v1->y;
		float z1 = v2->z - v1->z;
		float z2 = v3->z - v1->z;

		float s1 = w2->x - w1->x;
		float s2 = w3->x - w1->x;
		float t1 = w2->y - w1->y;
		float t2 = w3->y - w1->y;

		float r = 1.0F / (s1 * t2 - s2 * t1);

		sdir.x = (t2 * x1 - t1 * x2) * r;
		sdir.y = (t2 * y1 - t1 * y2) * r;
		sdir.z = (t2 * z1 - t1 * z2) * r;

		tdir.x = (s1 * x2 - s2 * x1) * r;
		tdir.y = (s1 * y2 - s2 * y1) * r;
		tdir.z = (s1 * z2 - s2 * z1) * r;

		tan1[i1].x += sdir.x;
		tan1[i1].y += sdir.y;
		tan1[i1].z += sdir.z;

		tan1[i2].x += sdir.x;
		tan1[i2].y += sdir.y;
		tan1[i2].z += sdir.z;

		tan1[i3].x += sdir.x;
		tan1[i3].y += sdir.y;
		tan1[i3].z += sdir.z;

		tan2[i1].x += tdir.x;
		tan2[i1].y += tdir.y;
		tan2[i1].z += tdir.z;

		tan2[i2].x += tdir.x;
		tan2[i2].y += tdir.y;
		tan2[i2].z += tdir.z;

		tan2[i3].x += tdir.x;
		tan2[i3].y += tdir.y;
		tan2[i3].z += tdir.z;
	}

	for (size_t i = 0; i < data->vertex_count; ++i) {
		const de_vec3_t* n = &data->normals[i];
		const de_vec3_t* t = tan1 + i;
		de_vec3_t tangent, temp;

		/* Gram-Schmidt orthogonalize */
		de_vec3_scale(&temp, n, de_vec3_dot(n, t));
		de_vec3_sub(&tangent, t, &temp);
		de_vec3_normalize(&tangent, &tangent);

		/* Calculate handedness */
		de_vec3_cross(&temp, n, t);
		float w = (de_vec3_dot(&temp, &tan2[i]) < 0.0F) ? -1.0F : 1.0F;
		data->tangents[i] = (de_vec4_t) { .x = tangent.x, .y = tangent.y, .z = tangent.z, .w = w };
	}

	de_free(tan1);
}

#if 0
void de_surface_make_cube(de_surface_t* surf)
{
	static de_vertex_t vertices[] = {
		/* front */
		{ { -0.5, -0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5, 0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, 0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, -0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },

		/* back */
		{ { -0.5, -0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5, 0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, 0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, -0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },

		/* left */
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },

		/* right */
		{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },

		/* top */
		{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },

		/* bottom */
		{ { -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0 }, { 0 } }
	};

	static int faces[] = {
		2, 1, 0,
		3, 2, 0,

		4, 5, 6,
		4, 6, 7,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		18, 17, 16,
		19, 18, 16,

		20, 21, 22,
		20, 22, 23
	};

	de_surface_set_data(surf, vertices, DE_ARRAY_SIZE(vertices), faces, DE_ARRAY_SIZE(faces));
	de_surface_calculate_normals(surf);
	de_surface_calculate_tangents(surf);
}
#endif

static void de_surface_add_vertex_pos_tex(de_surface_t* surf, de_vec3_t* pos, de_vec2_t* tex_coord)
{
	int i;
	bool found_identic = false;

	de_surface_shared_data_t* data = surf->shared_data;
	/* reverse search is much faster, because it most likely that we'll find identic
	* vertex nearby current in the list */
	for (i = (int)data->vertex_count - 1; i >= 0; --i) {
		if (de_vec3_equals(&data->positions[i], pos) && de_vec2_equals(&data->tex_coords[i], tex_coord)) {
			found_identic = true;
			break;
		}
	}

	if (!found_identic) {
		i = data->vertex_count;
		size_t k = data->vertex_count;
		de_surface_shared_data_grow_vertices(data, 1);
		data->positions[k] = *pos;
		data->tex_coords[k] = *tex_coord;
		++data->vertex_count;
	}

	de_surface_shared_data_grow_indices(data, 1);
	data->indices[data->index_count++] = i;

	surf->need_upload = true;
}

void de_surface_make_sphere(de_surface_t* surf, int slices, int stacks, float r)
{
	float d_theta = (float)M_PI / slices;
	float d_phi = 2.0f * (float)M_PI / stacks;
	float d_tc_y = 1.0f / stacks;
	float d_tc_x = 1.0f / slices;

	de_surface_shared_data_t* data = de_surface_shared_data_create(4, 6);
	de_surface_set_data(surf, data);

	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int nj = j + 1;
			int ni = i + 1;

			float k0 = r * (float)sin(d_theta * i);
			float k1 = (float)cos(d_phi * j);
			float k2 = (float)sin(d_phi * j);
			float k3 = r * (float)cos(d_theta * i);

			float k4 = r * (float)sin(d_theta * ni);
			float k5 = (float)cos(d_phi * nj);
			float k6 = (float)sin(d_phi * nj);
			float k7 = r * (float)cos(d_theta * ni);

			if (i != (stacks - 1)) {
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k0 * k1, k0 * k2, k3 }, &(de_vec2_t){ d_tc_x * j, d_tc_y * i });
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k4 * k1, k4 * k2, k7 }, &(de_vec2_t){ d_tc_x * j, d_tc_y * ni });
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k4 * k5, k4 * k6, k7 }, &(de_vec2_t){ d_tc_x * nj, d_tc_y * ni });
			}

			if (i != 0) {
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k4 * k5, k4 * k6, k7 }, &(de_vec2_t){ d_tc_x * nj, d_tc_y * ni });
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k0 * k5, k0 * k6, k3 }, &(de_vec2_t){ d_tc_x * nj, d_tc_y * i });
				de_surface_add_vertex_pos_tex(surf, &(de_vec3_t){ k0 * k1, k0 * k2, k3 }, &(de_vec2_t){ d_tc_x * j, d_tc_y * i });
			}
		}
	}

	de_surface_calculate_normals(surf);
	de_surface_calculate_tangents(surf);
}