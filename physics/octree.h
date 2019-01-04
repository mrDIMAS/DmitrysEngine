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

typedef struct de_octree_node_t
{
	int * indices;
	int indexCount;
	char split;
	de_vec3_t min;	
	de_vec3_t max;	
	struct de_octree_node_t * children[8];
} de_octree_node_t;

typedef struct de_trace_buffer
{
	de_octree_node_t ** nodes;
	int size;
} de_trace_buffer;

typedef struct de_octree_t
{
	de_trace_buffer traceBuffer;
	de_octree_node_t * root;
} de_octree_t;

/**
 * @brief Builds octree using a set of vertices and indices.
 * @param vertices Set of vertices
 * @param stride Offset between positions in vertices. Can be zero, if you have tigthly packed array of positios.
 * @param index_count Count of indices 
 * @param 
 */
de_octree_t * de_octree_build(const void* vertices, int stride, int* indices, size_t index_count, int max_triangles_per_node);

/**
 * @brief 
 */
void de_octree_free(de_octree_t * octree);

/**
 * @brief Fills trace buffer with octree nodes which intersects with sphere.
 */
void de_octree_trace_sphere(de_octree_t * octree, const de_vec3_t* position, float radius);

/**
 * @brief Fills trace buffer with octree nodes which itersects with ray.
 */
void de_octree_trace_ray(de_octree_t * octree, const de_ray_t * ray);
