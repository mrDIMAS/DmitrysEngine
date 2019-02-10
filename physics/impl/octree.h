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

typedef struct de_face_aabb_t {
	de_vec3_t min;
	de_vec3_t max;
} de_face_aabb_t;


static void de_octree_count_leafs_recursive(de_octree_node_t* node, int* counter) {
    int i;
	if (node->split) {
		for (i = 0; i < 8; ++i) {
			de_octree_count_leafs_recursive(node->children[i], counter);
		}
	} else {
		(*counter)++;
	}
}


static int de_octree_count_leafs(de_octree_t* octree) {
	int counter = 0;
	de_octree_count_leafs_recursive(octree->root, &counter);
	return counter;
}


static void de_octree_create_trace_buffers(de_octree_t* octree) {
	int leafCount = de_octree_count_leafs(octree);
	octree->traceBuffer.nodes = (de_octree_node_t **)de_calloc(leafCount, sizeof(*octree->traceBuffer.nodes));
	octree->traceBuffer.size = 0;
}


static void de_octree_split_node(de_octree_node_t* node) {
    int i;
	de_vec3_t center;

	de_vec3_middle(&center, &node->min, &node->max);

	for (i = 0; i < 8; i++) {
		node->children[i] = DE_NEW(de_octree_node_t);
	}

	de_vec3_set(&node->children[0]->min, node->min.x, node->min.y, node->min.z);
	de_vec3_set(&node->children[0]->max, center.x, center.y, center.z);

	de_vec3_set(&node->children[1]->min, center.x, node->min.y, node->min.z);
	de_vec3_set(&node->children[1]->max, node->max.x, center.y, center.z);

	de_vec3_set(&node->children[2]->min, node->min.x, node->min.y, center.z);
	de_vec3_set(&node->children[2]->max, center.x, center.y, node->max.z);

	de_vec3_set(&node->children[3]->min, center.x, node->min.y, center.z);
	de_vec3_set(&node->children[3]->max, node->max.x, center.y, node->max.z);

	de_vec3_set(&node->children[4]->min, node->min.x, center.y, node->min.z);
	de_vec3_set(&node->children[4]->max, center.x, node->max.y, center.z);

	de_vec3_set(&node->children[5]->min, center.x, center.y, node->min.z);
	de_vec3_set(&node->children[5]->max, node->max.x, node->max.y, center.z);

	de_vec3_set(&node->children[6]->min, node->min.x, center.y, center.z);
	de_vec3_set(&node->children[6]->max, center.x, node->max.y, node->max.z);

	de_vec3_set(&node->children[7]->min, center.x, center.y, center.z);
	de_vec3_set(&node->children[7]->max, node->max.x, node->max.y, node->max.z);

	node->split = true;
}


static void de_octree_build_recursive_internal(
	de_octree_node_t* node,
	int* indices,
	size_t index_count,
	size_t max_triangles_per_node,
	de_face_aabb_t* faces_aabbs) {
	size_t i, k;

	if (index_count < max_triangles_per_node) {
		size_t sizeBytes = sizeof(int)* index_count;
		node->indexCount = index_count;
		node->indices = (int *)de_malloc(sizeBytes);
		memcpy(node->indices, indices, sizeBytes);
	} else {
		de_octree_split_node(node);

		for (i = 0; i < 8; i++) {
			de_octree_node_t* child = node->children[i];
			DE_ARRAY_DECLARE(int, leaf_indices);

			DE_ARRAY_INIT(leaf_indices);
			for (i = 0, k = 0; i < index_count; ++k) {
				de_vec3_t cA, rA, cB, rB;
				de_face_aabb_t* face_aabb = faces_aabbs + k;

				de_vec3_middle(&cA, &face_aabb->max, &face_aabb->min);

				de_vec3_sub(&rA, &face_aabb->max, &face_aabb->min);
				de_vec3_scale(&rA, &rA, 0.5f);

				de_vec3_middle(&cB, &node->max, &node->min);

				de_vec3_sub(&rB, &node->max, &node->min);
				de_vec3_scale(&rB, &rB, 0.5f);

				if (fabs(cA.x - cB.x) <= (rA.x + rB.x) &&
					fabs(cA.y - cB.y) <= (rA.y + rB.y) &&
					fabs(cA.z - cB.z) <= (rA.z + rB.z)) {
					DE_ARRAY_APPEND(leaf_indices, indices[i++]);
					DE_ARRAY_APPEND(leaf_indices, indices[i++]);
					DE_ARRAY_APPEND(leaf_indices, indices[i++]);
				}
			}

			de_octree_build_recursive_internal(child, leaf_indices.data, leaf_indices.size, max_triangles_per_node, faces_aabbs);

			DE_ARRAY_FREE(leaf_indices);
		}
	}
}


de_octree_t* de_octree_build(const void* vertices, int stride, int* indices, size_t index_count, size_t max_triangles_per_node) {
	size_t i, k;
	de_face_aabb_t* face_aabbs;
	de_octree_t* octree = DE_NEW(de_octree_t);

	const char* data = (char*)vertices;

	/* compute metrics of vertices( min, max ) and build root node*/
	octree->root = DE_NEW(de_octree_node_t);

	de_vec3_set(&octree->root->min, FLT_MAX, FLT_MAX, FLT_MAX);
	de_vec3_set(&octree->root->max, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (i = 0; i < index_count; i++) {
		const de_vec3_t* v = (de_vec3_t*)(data + stride * i);

		de_vec3_min_max(v, &octree->root->min, &octree->root->max);
	}

	/* precompute each face bounds*/
	face_aabbs = (de_face_aabb_t*)de_malloc(index_count / 3 * sizeof(de_face_aabb_t));
	for (i = 0, k = 0; i < index_count; ++k) {
		de_face_aabb_t* aabb = face_aabbs + k;
        de_vec3_t* v0, *v1, *v2;

		de_vec3_set(&aabb->max, -FLT_MAX, -FLT_MAX, -FLT_MAX);
		de_vec3_set(&aabb->min, FLT_MAX, FLT_MAX, FLT_MAX);

		v0 = (de_vec3_t*)(data + stride * indices[i++]);
		v1 = (de_vec3_t*)(data + stride * indices[i++]);
		v2 = (de_vec3_t*)(data + stride * indices[i++]);

		de_vec3_min_max(v0, &aabb->min, &aabb->max);
		de_vec3_min_max(v1, &aabb->min, &aabb->max);
		de_vec3_min_max(v2, &aabb->min, &aabb->max);
	}

	de_octree_build_recursive_internal(octree->root, indices, index_count, max_triangles_per_node, face_aabbs);

	de_octree_create_trace_buffers(octree);

	return octree;
}


static void de_octree_node_free(de_octree_node_t* node) {
    int i;
	if (node->split) {
		for (i = 0; i < 8; ++i) {
			de_octree_node_free(node->children[i]);
		}
	}

	de_free(node->indices);
	de_free(node);
}


void de_octree_free(de_octree_t* octree) {
	de_octree_node_free(octree->root);
	de_free(octree->traceBuffer.nodes);
	de_free(octree);
}


static void de_octree_trace_ray_recursive(de_octree_t* octree, de_octree_node_t* node, const de_ray_t* ray) {
    int i;
	if (de_ray_aabb_intersection(ray, &node->min, &node->max, NULL, NULL)) {
		if (node->split) {
			for (i = 0; i < 8; i++) {
				de_octree_trace_ray_recursive(octree, node->children[i], ray);
			}
		} else {
			octree->traceBuffer.nodes[octree->traceBuffer.size++] = node;
		}
	}
}


void de_octree_trace_ray(de_octree_t* octree, const de_ray_t* ray) {
	octree->traceBuffer.size = 0;
	de_octree_trace_ray_recursive(octree, octree->root, ray);
}


static bool de_octree_node_is_intersect_sphere(const de_octree_node_t* node, const de_vec3_t* position, float radius) {
	float r2 = radius* radius;
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


static void de_octree_trace_sphere_recursive(de_octree_t* octree, de_octree_node_t* node, const de_vec3_t* position, float radius) {
    int i;
	if (de_octree_node_is_intersect_sphere(node, position, radius)) {
		if (node->split) {
			for (i = 0; i < 8; i++) {
				de_octree_trace_sphere_recursive(octree, node->children[i], position, radius);
			}
		} else {
			octree->traceBuffer.nodes[octree->traceBuffer.size++] = node;
		}
	}
}


bool de_octree_node_is_point_inside(de_octree_node_t* node, const de_vec3_t* point) {
	return point->x >= node->min.x && point->x <= node->max.x &&
		point->y >= node->min.y && point->y <= node->max.y &&
		point->z >= node->min.z && point->z <= node->max.z;
}


void de_octree_trace_sphere(de_octree_t* octree, const de_vec3_t* position, float radius) {
	octree->traceBuffer.size = 0;
	de_octree_trace_sphere_recursive(octree, octree->root, position, radius);
}
