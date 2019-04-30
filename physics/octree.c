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
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

typedef struct de_octree_triangle_t {
	int index;
	de_vec3_t min;
	de_vec3_t max;
} de_octree_triangle_t;

static void de_octree_count_leafs_recursive(de_octree_node_t* node, int* counter)
{
	if (node->split) {
		for (int i = 0; i < 8; ++i) {
			de_octree_count_leafs_recursive(node->children[i], counter);
		}
	} else {
		(*counter)++;
	}
}

static int de_octree_count_leafs(de_octree_t* octree)
{
	int counter = 0;
	de_octree_count_leafs_recursive(octree->root, &counter);
	return counter;
}

static void de_octree_create_trace_buffers(de_octree_t* octree)
{
	int leafCount = de_octree_count_leafs(octree);
	octree->trace_buffer.nodes = (de_octree_node_t **)de_calloc(leafCount, sizeof(*octree->trace_buffer.nodes));
	octree->trace_buffer.size = 0;
}

static void de_octree_split_node(de_octree_node_t* node)
{
	de_vec3_t center;
	de_vec3_middle(&center, &node->min, &node->max);

	for (int i = 0; i < 8; i++) {
		node->children[i] = DE_NEW(de_octree_node_t);
	}

	node->children[0]->min = (de_vec3_t) { node->min.x, node->min.y, node->min.z };
	node->children[0]->max = (de_vec3_t) { center.x, center.y, center.z };

	node->children[1]->min = (de_vec3_t) { center.x, node->min.y, node->min.z };
	node->children[1]->max = (de_vec3_t) { node->max.x, center.y, center.z };

	node->children[2]->min = (de_vec3_t) { node->min.x, node->min.y, center.z };
	node->children[2]->max = (de_vec3_t) { center.x, center.y, node->max.z };

	node->children[3]->min = (de_vec3_t) { center.x, node->min.y, center.z };
	node->children[3]->max = (de_vec3_t) { node->max.x, center.y, node->max.z };

	node->children[4]->min = (de_vec3_t) { node->min.x, center.y, node->min.z };
	node->children[4]->max = (de_vec3_t) { center.x, node->max.y, center.z };

	node->children[5]->min = (de_vec3_t) { center.x, center.y, node->min.z };
	node->children[5]->max = (de_vec3_t) { node->max.x, node->max.y, center.z };

	node->children[6]->min = (de_vec3_t) { node->min.x, center.y, center.z };
	node->children[6]->max = (de_vec3_t) { center.x, node->max.y, node->max.z };

	node->children[7]->min = (de_vec3_t) { center.x, center.y, center.z };
	node->children[7]->max = (de_vec3_t) { node->max.x, node->max.y, node->max.z };

	node->split = true;
}

static void de_octree_build_recursive_internal(
	de_octree_node_t* node,
	de_octree_triangle_t* triangles,
	size_t triangle_count,
	size_t max_triangles_per_node)
{
	if (triangle_count < max_triangles_per_node) {
		node->index_count = triangle_count;
		if (node->index_count > 0) {
			size_t sizeBytes = sizeof(*node->triangle_indices) * triangle_count;
			node->triangle_indices = de_malloc(sizeBytes);
			for (size_t i = 0; i < triangle_count; ++i) {
				node->triangle_indices[i] = triangles[i].index;
			}
		}
	} else {
		de_octree_split_node(node);

		for (size_t n = 0; n < 8; n++) {
			de_octree_node_t* child = node->children[n];

			DE_ARRAY_DECLARE(de_octree_triangle_t, leaf_triangles);
			DE_ARRAY_INIT(leaf_triangles);

			for (size_t i = 0; i < triangle_count; ++i) {
				de_octree_triangle_t* triangle = triangles + i;

				de_vec3_t center_a;
				de_vec3_middle(&center_a, &triangle->max, &triangle->min);

				de_vec3_t size_a;
				de_vec3_sub(&size_a, &triangle->max, &triangle->min);
				de_vec3_scale(&size_a, &size_a, 0.5f);

				de_vec3_t center_b;
				de_vec3_middle(&center_b, &child->max, &child->min);

				de_vec3_t size_b;
				de_vec3_sub(&size_b, &child->max, &child->min);
				de_vec3_scale(&size_b, &size_b, 0.5f);

				if (fabs(center_a.x - center_b.x) <= (size_a.x + size_b.x) &&
					fabs(center_a.y - center_b.y) <= (size_a.y + size_b.y) &&
					fabs(center_a.z - center_b.z) <= (size_a.z + size_b.z)) {
					DE_ARRAY_APPEND(leaf_triangles, *triangle);
				}
			}

			de_octree_build_recursive_internal(child, leaf_triangles.data, leaf_triangles.size, max_triangles_per_node);

			DE_ARRAY_FREE(leaf_triangles);
		}
	}
}

de_octree_t* de_octree_build(const void* src_triangles, size_t triangle_count, int pos_stride, size_t max_triangles_per_node)
{
	de_octree_t* octree = DE_NEW(de_octree_t);

	/* compute metrics of vertices (min and max) and build root node */
	octree->root = DE_NEW(de_octree_node_t);

	octree->root->min = (de_vec3_t) { FLT_MAX, FLT_MAX, FLT_MAX };
	octree->root->max = (de_vec3_t) { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (size_t i = 0; i < triangle_count; ++i) {
		const de_vec3_t* v0 = (de_vec3_t*)((char*)src_triangles + i * pos_stride);
		const de_vec3_t* v1 = v0 + 1;
		const de_vec3_t* v2 = v1 + 1;

		de_vec3_min_max(v0, &octree->root->min, &octree->root->max);
		de_vec3_min_max(v1, &octree->root->min, &octree->root->max);
		de_vec3_min_max(v2, &octree->root->min, &octree->root->max);
	}
		
	de_octree_triangle_t* triangles = de_malloc(triangle_count * sizeof(*triangles));
	for (size_t i = 0; i < triangle_count; ++i) {
		de_octree_triangle_t* triangle = triangles + i;

		const de_vec3_t* v0 = (de_vec3_t*)((char*)src_triangles + i * pos_stride);
		const de_vec3_t* v1 = v0 + 1;
		const de_vec3_t* v2 = v1 + 1;

		triangle->max = (de_vec3_t) { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		triangle->min = (de_vec3_t) { FLT_MAX, FLT_MAX, FLT_MAX };
		triangle->index = i;

		de_vec3_min_max(v0, &triangle->min, &triangle->max);
		de_vec3_min_max(v1, &triangle->min, &triangle->max);
		de_vec3_min_max(v2, &triangle->min, &triangle->max);
	}

	de_octree_build_recursive_internal(octree->root, triangles, triangle_count, max_triangles_per_node);

	de_octree_create_trace_buffers(octree);

	de_free(triangles);

	return octree;
}

static void de_octree_node_free(de_octree_node_t* node)
{
	if (node->split) {
		for (int i = 0; i < 8; ++i) {
			de_octree_node_free(node->children[i]);
		}
	}

	de_free(node->triangle_indices);
	de_free(node);
}

void de_octree_free(de_octree_t* octree)
{
	de_octree_node_free(octree->root);
	de_free(octree->trace_buffer.nodes);
	de_free(octree);
}

static void de_octree_trace_ray_recursive(de_octree_t* octree, de_octree_node_t* node, const de_ray_t* ray)
{
	if (de_ray_aabb_intersection(ray, &node->min, &node->max, NULL, NULL)) {
		if (node->split) {
			for (int i = 0; i < 8; i++) {
				de_octree_trace_ray_recursive(octree, node->children[i], ray);
			}
		} else {
			octree->trace_buffer.nodes[octree->trace_buffer.size++] = node;
		}
	}
}

void de_octree_trace_ray(de_octree_t* octree, const de_ray_t* ray)
{
	octree->trace_buffer.size = 0;
	de_octree_trace_ray_recursive(octree, octree->root, ray);
}

static bool de_octree_node_is_intersect_sphere(const de_octree_node_t* node, const de_vec3_t* position, float radius)
{
	float r2 = radius * radius;
	float dmin = 0;
	bool sphereInside;

	if (position->x < node->min.x) {
		dmin += de_sqr(position->x - node->min.x);
	} else if (position->x > node->max.x) {
		dmin += de_sqr(position->x - node->max.x);
	}

	if (position->y < node->min.y) {
		dmin += de_sqr(position->y - node->min.y);
	} else if (position->y > node->max.y) {
		dmin += de_sqr(position->y - node->max.y);
	}

	if (position->z < node->min.z) {
		dmin += de_sqr(position->z - node->min.z);
	} else if (position->z > node->max.z) {
		dmin += de_sqr(position->z - node->max.z);
	}

	sphereInside = (position->x >= node->min.x) && (position->x <= node->max.x) &&
		(position->y >= node->min.y) && (position->y <= node->max.y) &&
		(position->z >= node->min.z) && (position->z <= node->max.z);

	return dmin <= r2 || sphereInside;
}

static void de_octree_trace_sphere_recursive(de_octree_t* octree, de_octree_node_t* node, const de_vec3_t* position, float radius)
{
	if (de_octree_node_is_intersect_sphere(node, position, radius)) {
		if (node->split) {
			for (int i = 0; i < 8; i++) {
				de_octree_trace_sphere_recursive(octree, node->children[i], position, radius);
			}
		} else {
			octree->trace_buffer.nodes[octree->trace_buffer.size++] = node;
		}
	}
}

bool de_octree_node_is_point_inside(de_octree_node_t* node, const de_vec3_t* point)
{
	return point->x >= node->min.x && point->x <= node->max.x &&
		point->y >= node->min.y && point->y <= node->max.y &&
		point->z >= node->min.z && point->z <= node->max.z;
}

void de_octree_trace_sphere(de_octree_t* octree, const de_vec3_t* position, float radius)
{
	octree->trace_buffer.size = 0;
	de_octree_trace_sphere_recursive(octree, octree->root, position, radius);
}
