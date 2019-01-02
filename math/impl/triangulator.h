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

static de_bool_t de_is_ear(
	const de_triangulator_polygon_t* polygon,
	const de_triangulator_vertex_t* vprev,
	const de_triangulator_vertex_t* vear,
	const de_triangulator_vertex_t* vnext)
{
	de_triangulator_vertex_t* v;
	de_vec2_t ab, cb;

	/* step 1: check angle at ear tip */
	de_vec2_sub(&ab, &vear->position, &vprev->position);
	de_vec2_sub(&cb, &vear->position, &vnext->position);

	float angle = de_vec2_angle(&ab, &cb);

	/* not an ear tip, so not an ear triangle */
	if (angle > M_PI)
	{
		return DE_FALSE;
	}

	/* step 2: check winding of a triangle (assume CCW order)*/
	if (de_get_signed_triangle_area(&vprev->position, &vear->position, &vnext->position) >= 0)
	{
		return DE_FALSE;
	}

	/* step 3: check if other points are inside of triangle */
	for (v = polygon->head; v != polygon->tail; v = v->next)
	{
		if (v == vprev || v == vear || v == vnext)
		{
			continue;
		}

		/* some other point lies in triangle, so current triangle is not an ear */
		if (de_is_point_inside_triangle_2D(&v->position, &vprev->position, &vear->position, &vnext->position))
		{
			return DE_FALSE;
		}
	}

	return DE_TRUE;
}

int* de_triangulate(de_vec3_t* polygon, size_t vertex_count, int* out_count)
{
	size_t i;
	de_vec3_t normal;
	int vertices_left, loop_sentinel = 0, iteration_limit;
	de_triangulator_vertex_t* vear, *v;
	de_triangulator_polygon_t poly;
	de_plane_class_t proj_plane_class;
	DE_ARRAY_DECLARE(int, indices);

	if (vertex_count < 3)
	{
		de_log("triangulator: trying to triangulate a line or point???");
		return NULL;
	}

	vertices_left = vertex_count;
	iteration_limit = vertex_count * 2;

	DE_ARRAY_INIT(indices);
	DE_LINKED_LIST_INIT(poly);

	/* calculate polygon normal - it will be used to project polygon onto 2D plane */
	de_get_polygon_normal(polygon, vertex_count, &normal);

	/* choose projection plane */
	proj_plane_class = de_plane_classify(&normal);

	/* prepare vertices: project them to 2d plane and link to circle */
	for (i = 0; i < vertex_count; ++i)
	{
		v = (de_triangulator_vertex_t*)de_malloc(sizeof(de_triangulator_vertex_t));
		v->index = i;
		de_vec3_to_vec2_by_plane(proj_plane_class, &normal, &polygon[i], &v->position);
		DE_LINKED_LIST_APPEND(poly, v);
	}

	/* make sure that we have closed polygon */
	DE_LINKED_LIST_MAKE_CIRCULAR(poly);

	/* now start iterating thru polygon and search for ears */
	for (vear = poly.head; vertices_left >= 3 && loop_sentinel < iteration_limit; ++loop_sentinel)
	{
		de_triangulator_vertex_t* vprev = vear->prev;
		de_triangulator_vertex_t* vnext = vear->next;
		if (de_is_ear(&poly, vprev, vear, vnext))
		{
			DE_ARRAY_APPEND(indices, vprev->index);
			DE_ARRAY_APPEND(indices, vear->index);
			DE_ARRAY_APPEND(indices, vnext->index);
			/* remove ear vertex */
			DE_LINKED_LIST_REMOVE(poly, vear);
			de_free(vear);
			--vertices_left;
			/* previous vertex is a good candidate for ear tip */
			vear = vprev;
		}
		else
		{
			/* try next vertex */
			vear = vear->next;
		}
	}

	/* cleanup rest */
	v = poly.head;
	do
	{
		de_triangulator_vertex_t* next = v->next;
		de_free(v);		
		v = next;
	} while (v != poly.head);

	*out_count = indices.size;

	return indices.data;
}

/**
 * Shallow tests, more complex tests were done in FBX loader
 */
void de_triangulator_tests(void)
{
	size_t i;
	int index_count;
	
	de_vec3_t poly[] = {
	#if 0
		{ 0, 3, 0 },
		{ 2, 0, 0 },
		{ 5, 2, 0 },
		{ 7, 1, 0 },
		{ 9, 3, 0 },
		{ 7, 2, 0 },
		{ 6, 4, 0 },
		{ 4, 2, 0 },
		{ 2, 2, 0 },
		{ 1, 4, 0 },
	#else
		{ 0, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 1, 0 },
		{ 0, 1, 0 },
	#endif
	};
	
	int* indices = de_triangulate(poly, sizeof(poly) / sizeof(poly[0]), &index_count);

	for (i = 0; i < index_count; i += 3)
	{
		printf("%d;%d;%d\n", indices[i], indices[i + 1], indices[i + 2]);
	}

	de_free(indices);
}