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

/**
 * AStar Pathfinding.
 */

typedef enum de_graph_vertex_flags_t {
	DE_GRAPH_VERTEX_FLAGS_NON_VISITED = 0, /**< Vertex wasn't processed yet. */
	DE_GRAPH_VERTEX_FLAGS_IS_OPEN = DE_BIT(0), /**< Vertex is in open set. */
	DE_GRAPH_VERTEX_FLAGS_IS_CLOSED = DE_BIT(1) /**< Vertex is in closed set. */
} de_graph_vertex_flags_t;

/**
 * @brief Graph vertex
 */
typedef struct de_graph_vertex_t {
	de_vec3_t position;
	de_graph_vertex_flags_t flags;
	float g_score;
	float f_score;
	/** 
	 * Pointer to "parent" vertex from which we can reach this vertex on shorthest path to target.
	 * In other words this pointer used to store path information.
	 */
	struct de_graph_vertex_t* parent;
	DE_ARRAY_DECLARE(struct de_graph_vertex_t*, neighbours); /**< Array of pointers to neighbour vertices. */
} de_graph_vertex_t;

/**
* @brief Array of pointers to graph vertices that describes path.
*/
typedef DE_ARRAY_DECLARE(de_graph_vertex_t*, de_graph_path_t);

/**
 * @brief Graph is just array of vertices.
 */
typedef struct de_graph_t {
	DE_ARRAY_DECLARE(de_graph_vertex_t, vertices);
} de_graph_t;

typedef enum de_graph_path_type_t {
	DE_GRAPH_PATH_TYPE_FULL,
	DE_GRAPH_PATH_TYPE_PARTIAL,
	DE_GRAPH_PATH_TYPE_EMPTY,
} de_graph_path_type_t;

/**
 * @brief Prepares graph.
 */
void de_graph_init(de_graph_t* graph);

/**
 * @brief Frees graph resources.
 */
void de_graph_free(de_graph_t* graph);

/**
 * @brief Initializes vertex with specified position.
 */
void de_graph_vertex_init(de_graph_vertex_t* vertex, const de_vec3_t* position);

/**
 * @brief Tries to find a path between specified graph vertices. Writes path to out_path as array of
 * pointers to graph vertices. IMPORTANT: NEVER! EVER! store pointers this function returns, you must
 * use them immediately, for example copy information about positions of vertices into some other path
 * array and only then use it for navigation. Even if direct path is not found, pathfinder will build
 * shorthest possible path to last reachable vertex near target vertex - in this case function will
 * return DE_GRAPH_PATH_TYPE_PARTIAL. If full path is found then result will be DE_GRAPH_PATH_TYPE_FULL,
 * and finally if path is not found (i.e. start vertex is isolated one) function will return
 * DE_GRAPH_PATH_TYPE_EMPTY. Out path is always reversed (target vertex will be first in array)!
 */
de_graph_path_type_t de_graph_find_path(de_graph_t* graph, de_graph_vertex_t* start, de_graph_vertex_t* goal, de_graph_path_t* out_path);

/**
 * @brief Links graph vertices to each other making bidirectional link A <-> B.
 */
void de_graph_vertex_link_bidirect(de_graph_vertex_t* vertex_a, de_graph_vertex_t* vertex_b);

/**
 * @brief Links vertex_a with vertex_b making unidirectional link A -> B
 */
void de_graph_vertex_link_unidirect(de_graph_vertex_t* vertex_a, de_graph_vertex_t* vertex_b);

/**
 * @brief Breaks A <-> B link between vertex and its neighbour so vertex become isolated. O(n) complexity.
 */
void de_graph_vertex_isolate(de_graph_t* graph, de_graph_vertex_t* vertex);

/**
 * @brief Breaks A -> B link.
 */
void de_graph_vertex_unlink(de_graph_vertex_t* vertex);

/**
 * @brief Tests. Can be used as code examples.
 */
void de_graph_tests();