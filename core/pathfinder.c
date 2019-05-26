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

static float de_pathfinder_heuristic(de_graph_vertex_t* a, de_graph_vertex_t* b)
{
	return de_vec3_sqr_distance(&a->position, &b->position);
}

void de_graph_init(de_graph_t* graph)
{
	DE_ARRAY_INIT(graph->vertices);
}

void de_graph_free(de_graph_t* graph)
{
	DE_ARRAY_FREE(graph->vertices);
}

static void de_graph_reconstruct_path(de_graph_vertex_t* from, de_graph_path_t* out_path)
{
	DE_ARRAY_APPEND(*out_path, from);
	while (from->parent) {
		from = from->parent;
		DE_ARRAY_APPEND(*out_path, from);
	}
}

de_graph_path_type_t de_graph_find_path(de_graph_t* graph, de_graph_vertex_t* start, de_graph_vertex_t* goal, de_graph_path_t* out_path)
{
	if (!start || !goal || graph->vertices.size == 0) {
		return DE_GRAPH_PATH_TYPE_EMPTY;
	}

	DE_ARRAY_CLEAR(*out_path);

	/* reset vertices */
	for (size_t i = 0; i < graph->vertices.size; ++i) {
		de_graph_vertex_t* vertex = graph->vertices.data + i;
		vertex->g_score = FLT_MAX;
		vertex->f_score = FLT_MAX;
		vertex->flags = DE_GRAPH_VERTEX_FLAGS_NON_VISITED;
		vertex->parent = NULL;
	}

	int open_set_size = 1;
	start->flags |= DE_GRAPH_VERTEX_FLAGS_IS_OPEN;
	start->g_score = 0;
	start->f_score = de_pathfinder_heuristic(start, goal);

	while (open_set_size > 0) {
		de_graph_vertex_t* current = NULL;
		for (size_t i = 0; i < graph->vertices.size; ++i) {
			de_graph_vertex_t* vertex = graph->vertices.data + i;
			if (vertex->flags & DE_GRAPH_VERTEX_FLAGS_IS_OPEN) {
				if (!current) {
					current = vertex;
				}
				if (vertex->f_score < current->f_score) {
					current = vertex;
				}
			}
		}

		DE_ASSERT(current);

		if (current == goal) {
			de_graph_reconstruct_path(current, out_path);
			return DE_GRAPH_PATH_TYPE_FULL;
		}

		current->flags &= ~DE_GRAPH_VERTEX_FLAGS_IS_OPEN;
		current->flags |= DE_GRAPH_VERTEX_FLAGS_IS_CLOSED;
		--open_set_size;

		for (size_t i = 0; i < current->neighbours.size; ++i) {
			de_graph_vertex_t* neighbour = current->neighbours.data[i];

			DE_ASSERT(neighbour != current);

			if (neighbour->flags & DE_GRAPH_VERTEX_FLAGS_IS_CLOSED) {
				continue;
			}

			neighbour->flags |= DE_GRAPH_VERTEX_FLAGS_IS_OPEN;
			++open_set_size;

			const float gScore = current->g_score + de_pathfinder_heuristic(current, neighbour);

			if (gScore >= neighbour->g_score) {
				continue;
			}

			neighbour->parent = current;
			neighbour->g_score = gScore;
			neighbour->f_score = gScore + de_pathfinder_heuristic(neighbour, goal);
		}
	}

	de_graph_vertex_t* current = graph->vertices.data;
	for (size_t i = 0; i < graph->vertices.size; ++i) {
		de_graph_vertex_t* vertex = graph->vertices.data + i;
		if (vertex->f_score < current->f_score) {
			current = vertex;
		}
	}

	de_graph_reconstruct_path(current, out_path);

	return out_path->size ? DE_GRAPH_PATH_TYPE_PARTIAL : DE_GRAPH_PATH_TYPE_EMPTY;
}

void de_graph_vertex_link_bidirect(de_graph_vertex_t* vertex_a, de_graph_vertex_t* vertex_b)
{
	DE_ASSERT(vertex_a != vertex_b);
	DE_ARRAY_APPEND(vertex_a->neighbours, vertex_b);
	DE_ARRAY_APPEND(vertex_b->neighbours, vertex_a);
}

void de_graph_vertex_link_unidirect(de_graph_vertex_t* vertex_a, de_graph_vertex_t* vertex_b)
{
	DE_ASSERT(vertex_a != vertex_b);
	DE_ARRAY_APPEND(vertex_a->neighbours, vertex_b);
}

void de_graph_vertex_init(de_graph_vertex_t* vertex, const de_vec3_t* position)
{
	vertex->position = *position;
	vertex->g_score = 0;
	vertex->f_score = 0;
	vertex->parent = NULL;
	vertex->flags = DE_GRAPH_VERTEX_FLAGS_NON_VISITED;
	DE_ARRAY_INIT(vertex->neighbours);
}

void de_graph_vertex_isolate(de_graph_t* graph, de_graph_vertex_t* vertex)
{
	DE_ARRAY_CLEAR(vertex->neighbours);
	for (size_t i = 0; i < graph->vertices.size; ++i) {
		de_graph_vertex_t* other_vertex = graph->vertices.data + i;
		for(int k = (int)other_vertex->neighbours.size - 1; k >= 0; --k) {
			if (other_vertex->neighbours.data[k] == vertex) {
				DE_ARRAY_REMOVE_AT(other_vertex->neighbours, (size_t)k);
			}
		}
	}
}

void de_graph_vertex_unlink(de_graph_vertex_t* vertex)
{
	DE_ARRAY_CLEAR(vertex->neighbours);
}

void de_graph_tests()
{
	de_graph_t graph;
	de_graph_init(&graph);
	
	/* sanity test */
	de_graph_path_type_t path_type = de_graph_find_path(NULL, NULL, NULL, NULL);
	DE_ASSERT(path_type == DE_GRAPH_PATH_TYPE_EMPTY);

	/* initialize vertices first */
	int size = 40;
	for (float y = 0; y < size; ++y) {
		for (float x = 0; x < size; ++x) {
			de_graph_vertex_t* vertex = DE_ARRAY_GROW(graph.vertices, 1);
			de_graph_vertex_init(vertex, &(de_vec3_t) {.x = x, .y = y, .z = 0 });
		}
	}

	/* link to form grid */
	for (int y = 0; y < size - 1; ++y) {
		for (int x = 0; x < size - 1; ++x) {
			de_graph_vertex_link_bidirect(&graph.vertices.data[y * size + x], &graph.vertices.data[y * size + x + 1]);
			de_graph_vertex_link_bidirect(&graph.vertices.data[y * size + x], &graph.vertices.data[(y + 1) * size + x]);
		}
	}

	/* do tests from random to random point */
	de_graph_path_t path;
	DE_ARRAY_INIT(path);

	for (int i = 0; i < 1000; ++i) {
		int sx = de_irand(0, size - 1);
		int sy = de_irand(0, size - 1);

		int gx = de_irand(0, size - 1);
		int gy = de_irand(0, size - 1);
		
		de_graph_vertex_t* from = &graph.vertices.data[sy * size + sx];
		de_graph_vertex_t* to = &graph.vertices.data[gy * size + gx];
		path_type = de_graph_find_path(&graph, from, to, &path);
		DE_ASSERT(path_type == DE_GRAPH_PATH_TYPE_FULL);
	}

	/* isolated vertex test */
	de_graph_vertex_isolate(&graph, &DE_ARRAY_FIRST(graph.vertices));
	path_type = de_graph_find_path(&graph, &DE_ARRAY_FIRST(graph.vertices), &DE_ARRAY_LAST(graph.vertices), &path);
	DE_ASSERT(path_type == DE_GRAPH_PATH_TYPE_PARTIAL);

	DE_ARRAY_FREE(path);
	de_graph_free(&graph);
}