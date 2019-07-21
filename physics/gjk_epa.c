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

/* "Implementing GJK", Casey Muratori: 
 * https://www.youtube.com/watch?v=Qupqu1xe7Io
 *
 * "GJK + Expanding Polytope Algorithm - Implementation and Visualization"
 * https://www.youtube.com/watch?v=6rgiPrzqt9w
 *
 * Some ideas about contact point generation were taken from here
 * https://www.gamedev.net/forums/topic/598678-contact-points-with-epa/
 */

#define DE_GJK_MAX_ITERATIONS 64	

#define DE_EPA_TOLERANCE 0.0001
#define DE_EPA_MAX_ITERATIONS 64	
#define DE_EPA_MAX_LOOSE_EDGES 32
#define DE_EPA_MAX_FACES 64

typedef struct de_polytope_triangle_t {
	de_minkowski_vertex_t vertices[3];
	de_vec3_t normal;
} de_polytope_triangle_t;

typedef struct de_polytope_edge_t {
	de_minkowski_vertex_t begin;
	de_minkowski_vertex_t end;
} de_polytope_edge_t;

static void de_gjk_support(de_minkowski_vertex_t* vertex, const de_convex_shape_t* shape1, 
	const de_vec3_t* shape1_position, const de_convex_shape_t* shape2, const de_vec3_t* shape2_position,
	const de_vec3_t* dir)
{
	vertex->shape_a_world_space = de_convex_shape_get_farthest_point(shape1, shape1_position, dir);

	de_vec3_t neg_dir;
	de_vec3_negate(&neg_dir, dir);

	de_vec3_t b = de_convex_shape_get_farthest_point(shape2, shape2_position, &neg_dir);

	de_vec3_sub(&vertex->minkowski_dif, &vertex->shape_a_world_space, &b);
}

static void de_gjk_update_triangle_simplex(de_simplex_t* simplex, de_vec3_t * outDirection)
{
	de_vec3_t ca;
	de_vec3_sub(&ca, &simplex->c.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t ba;
	de_vec3_sub(&ba, &simplex->b.minkowski_dif, &simplex->a.minkowski_dif);

	/* Direction to origin */
	de_vec3_t ao;
	de_vec3_negate(&ao, &simplex->a.minkowski_dif);

	simplex->rank = 2;

	de_vec3_t triangle_normal;
	de_vec3_cross(&triangle_normal, &ba, &ca);

	de_vec3_t side;
	de_vec3_cross(&side, &ba, &triangle_normal);
	if (de_vec3_same_direction(&side, &ao)) {
		/* Closest to edge AB */
		simplex->c = simplex->a;

		de_vec3_t ba_x_ao;
		de_vec3_cross(&ba_x_ao, &ba, &ao);
		de_vec3_cross(outDirection, &ba_x_ao, &ba);
		return;
	}

	de_vec3_cross(&side, &triangle_normal, &ca);
	if (de_vec3_same_direction(&side, &ao)) {
		/* Closest to edge AC */
		simplex->b = simplex->a;

		de_vec3_t ca_x_ao;
		de_vec3_cross(&ca_x_ao, &ca, &ao);
		de_vec3_cross(outDirection, &ca_x_ao, &ca);
		return;
	}

	simplex->rank = 3;
	if (de_vec3_same_direction(&triangle_normal, &ao)) {
		/* Above triangle */
		simplex->d = simplex->c;
		simplex->c = simplex->b;
		simplex->b = simplex->a;
		*outDirection = triangle_normal;
		return;
	}

	/* Below triangle */
	simplex->d = simplex->b;
	simplex->b = simplex->a;
	de_vec3_negate(outDirection, &triangle_normal);
}

static bool de_gjk_update_tetrahedron_simplex(de_simplex_t* simplex, de_vec3_t * outDirection)
{
	/* Point a is tip of pyramid, BCD is the base (counterclockwise winding order) */

	/* Direction to origin */
	de_vec3_t ao;
	de_vec3_negate(&ao, &simplex->a.minkowski_dif);

	/* Plane-test origin with 3 faces. This is very inaccurate approach and
	 * it would be better to add additional checks for each face of tetrahdron
	 * to select search direction more precisely. In this case we assume that
	 * we always will produce triangles as final simplex. */
	simplex->rank = 3;

	de_vec3_t ba;
	de_vec3_sub(&ba, &simplex->b.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t ca;
	de_vec3_sub(&ca, &simplex->c.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t abc_normal;
	de_vec3_cross(&abc_normal, &ba, &ca);
	if (de_vec3_same_direction(&abc_normal, &ao)) {
		/* In front of ABC */
		simplex->d = simplex->c;
		simplex->c = simplex->b;
		simplex->b = simplex->a;
		*outDirection = abc_normal;
		return false;
	}

	de_vec3_t da;
	de_vec3_sub(&da, &simplex->d.minkowski_dif, &simplex->a.minkowski_dif);
	de_vec3_t acd_normal;
	de_vec3_cross(&acd_normal, &ca, &da);
	if (de_vec3_same_direction(&acd_normal, &ao)) {
		/* In front of ACD */
		simplex->b = simplex->a;
		*outDirection = acd_normal;
		return false;
	}

	de_vec3_t adb_normal;
	de_vec3_cross(&adb_normal, &da, &ba);
	if (de_vec3_same_direction(&adb_normal, &ao)) {
		/* In front of ADB */
		simplex->c = simplex->d;
		simplex->d = simplex->b;
		simplex->b = simplex->a;
		*outDirection = adb_normal;
		return false;
	}

	/* Otherwise origin is inside tetrahedron */
	return true;
}

bool de_gjk_is_intersects(de_convex_shape_t* shape1, const de_vec3_t* shape1_position,
	de_convex_shape_t* shape2, const de_vec3_t* shape2_position, de_simplex_t* out_simplex)
{
	/* This is good enough heuristic to choose initial search direction */
	de_vec3_t search_dir;
	de_vec3_sub(&search_dir, shape1_position, shape2_position);

	if (de_vec3_equals(&search_dir, &(de_vec3_t) { 0, 0, 0})) {
		search_dir.x = 1;
	}

	/* Get initial point for simplex */
	de_simplex_t simplex;
	de_gjk_support(&simplex.c, shape1, shape1_position, shape2, shape2_position, &search_dir);
	de_vec3_negate(&search_dir, &simplex.c.minkowski_dif); //search in direction of origin

	/* Get second point for a line segment simplex */
	de_gjk_support(&simplex.b, shape1, shape1_position, shape2, shape2_position, &search_dir);

	if (!de_vec3_same_direction(&simplex.b.minkowski_dif, &search_dir)) {
		return false;
	}

	de_vec3_t cb;
	de_vec3_sub(&cb, &simplex.c.minkowski_dif, &simplex.b.minkowski_dif);

	de_vec3_t minus_b;
	de_vec3_negate(&minus_b, &simplex.b.minkowski_dif);

	de_vec3_t cb_x_minus_b;
	de_vec3_cross(&cb_x_minus_b, &cb, &minus_b);

	/* Search perpendicular to line segment towards origin */
	de_vec3_cross(&search_dir, &cb_x_minus_b, &cb); 
    /* Origin is on this line segment - fix search direction. */
	if (de_vec3_equals(&search_dir, &(de_vec3_t) { 0, 0, 0 })) {
        /* perpendicular with x-axis */
		de_vec3_cross(&search_dir, &cb, &(de_vec3_t) { 1, 0, 0 }); 
		if (de_vec3_equals(&search_dir, &(de_vec3_t) { 0, 0, 0 })) {
			/* perpendicular with z-axis */
			de_vec3_cross(&search_dir, &cb, &(de_vec3_t) { 0, 0, -1 }); 
		}
	}

	simplex.rank = 2;
	for (int iterations = 0; iterations < DE_GJK_MAX_ITERATIONS; iterations++) {
		de_gjk_support(&simplex.a, shape1, shape1_position, shape2, shape2_position, &search_dir);

		if (!de_vec3_same_direction(&simplex.a.minkowski_dif, &search_dir)) {
			return false;
		}

		simplex.rank++;
		if (simplex.rank == 3) {
			de_gjk_update_triangle_simplex(&simplex, &search_dir);
		} else if (de_gjk_update_tetrahedron_simplex(&simplex, &search_dir)) {
			if (out_simplex) {
				*out_simplex = simplex;
			}
			return true;
		}
	}

	/* No convergence - no intersection */
	return false;
}

static bool de_polytope_edge_eq_ccw(const de_polytope_edge_t* a, const de_polytope_edge_t* b)
{
	return de_vec3_equals(&a->end.minkowski_dif, &b->begin.minkowski_dif)
		&& de_vec3_equals(&a->begin.minkowski_dif, &b->end.minkowski_dif);
}

static de_vec3_t de_epa_compute_contact_point(de_polytope_triangle_t* closest_triangle)
{
	/* Project origin onto triangle's plane */
	de_vec3_t proj;
	de_vec3_scale(&proj, &closest_triangle->normal, de_vec3_dot(&closest_triangle->vertices[0].minkowski_dif, &closest_triangle->normal));

	/* Find barycentric coordinates of the projection in Minkowski difference space */
	float u, v, w;
	de_get_barycentric_coords(&proj,
		&closest_triangle->vertices[0].minkowski_dif,
		&closest_triangle->vertices[1].minkowski_dif,
		&closest_triangle->vertices[2].minkowski_dif,
		&u, &v, &w);
	
	/* Use barycentric coordinates to get projection in world space */
	de_vec3_t a;
	de_vec3_scale(&a, &closest_triangle->vertices[0].shape_a_world_space, u);

	de_vec3_t b;
	de_vec3_scale(&b, &closest_triangle->vertices[1].shape_a_world_space, v);
	
	de_vec3_t c;
	de_vec3_scale(&c, &closest_triangle->vertices[2].shape_a_world_space, w);

	/* Sum all vectors to get world space contact point */
	de_vec3_t contact_point;
	de_vec3_add(&contact_point, &a, &b);
	de_vec3_add(&contact_point, &contact_point, &c);

	return contact_point;
}

bool de_epa_get_penetration_info(de_simplex_t* simplex, de_convex_shape_t* shape1, const de_vec3_t* shape1_position,
	de_convex_shape_t* shape2, const de_vec3_t* shape2_position, de_vec3_t* penetration_vector, de_vec3_t* contact_point)
{
	de_polytope_triangle_t triangles[DE_EPA_MAX_FACES];

	/* Reconstruct polytope from tetrahedron simplex points. */

	/* ABC */
	de_vec3_t ba;
	de_vec3_sub(&ba, &simplex->b.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t ca;
	de_vec3_sub(&ca, &simplex->c.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t abcNormal;
	de_vec3_cross(&abcNormal, &ba, &ca);
	if (de_vec3_try_normalize(&abcNormal, &abcNormal)) {
		triangles[0] = (de_polytope_triangle_t) { .vertices = { simplex->a, simplex->b, simplex->c }, .normal = abcNormal };
	} else {
		return false;
	}

	/* ACD */
	de_vec3_t da;
	de_vec3_sub(&da, &simplex->d.minkowski_dif, &simplex->a.minkowski_dif);

	de_vec3_t acdNormal;
	de_vec3_cross(&acdNormal, &ca, &da);
	if (de_vec3_try_normalize(&acdNormal, &acdNormal)) {
		triangles[1] = (de_polytope_triangle_t) { .vertices = { simplex->a, simplex->c, simplex->d }, .normal = acdNormal };
	} else {		
		return false;
	}

	/* ADB */
	de_vec3_t adbNormal;	
	de_vec3_cross(&adbNormal, &da, &ba);

	if (de_vec3_try_normalize(&adbNormal, &adbNormal)) {
		triangles[2] = (de_polytope_triangle_t) { .vertices = { simplex->a, simplex->d, simplex->b }, .normal = adbNormal };
	} else {
		return false;
	}

	/* BDC */
	de_vec3_t db;
	de_vec3_sub(&db, &simplex->d.minkowski_dif, &simplex->b.minkowski_dif);

	de_vec3_t cb;
	de_vec3_sub(&cb, &simplex->c.minkowski_dif, &simplex->b.minkowski_dif);

	de_vec3_t bdcNormal;
	de_vec3_cross(&bdcNormal, &db, &cb);
	if (de_vec3_try_normalize(&bdcNormal, &bdcNormal)) {
		triangles[3] = (de_polytope_triangle_t) { .vertices = { simplex->b, simplex->d, simplex->c }, .normal = bdcNormal };
	} else {
		return false;
	}

	int triangle_count = 4;
	int closest_triangle_index;

	for (int iteration = 0; iteration < DE_EPA_MAX_ITERATIONS; iteration++) {
		/* Find triangle that is closest to origin */
		float min_dist = de_vec3_dot(&triangles[0].vertices[0].minkowski_dif, &triangles[0].normal);
		closest_triangle_index = 0;
		for (int i = 1; i < triangle_count; i++) {
			const float dist = de_vec3_dot(&triangles[i].vertices[0].minkowski_dif, &triangles[i].normal);
			if (dist < min_dist) {
				min_dist = dist;
				closest_triangle_index = i;
			}
		}

		/* Search normal to triangle that's closest to origin */
		de_polytope_triangle_t* closest_triangle = triangles + closest_triangle_index;
		const de_vec3_t search_dir = closest_triangle->normal;
		de_minkowski_vertex_t new_point;
		de_gjk_support(&new_point, shape1, shape1_position, shape2, shape2_position, &search_dir);

		const float distanceToOrigin = de_vec3_dot(&new_point.minkowski_dif, &search_dir);
		if (distanceToOrigin - min_dist < DE_EPA_TOLERANCE) {
			*contact_point = de_epa_compute_contact_point(closest_triangle);
			de_vec3_scale(penetration_vector, &closest_triangle->normal, distanceToOrigin);
			return true;
		}

		/* Loose edges after we remove triangle must give us list of edges we have
		 * to stitch with new point to keep polytope convex. */
		int loose_edge_count = 0;
		de_polytope_edge_t loose_edges[DE_EPA_MAX_LOOSE_EDGES];
		
		/* Find all triangles that are facing new point and remove them */
		for (int i = 0; i < triangle_count; i++) {
			de_polytope_triangle_t* triangle = triangles + i;

			/* If triangle i faces new point, remove it. Also search for adjacent edges of it
			 * and remove them too to maintain loose edge list in correct state (see below). */
			de_vec3_t to_new_point;
			de_vec3_sub(&to_new_point, &new_point.minkowski_dif, &triangle->vertices[0].minkowski_dif);

			if (de_vec3_dot(&triangle->normal, &to_new_point) > 0) {
				for (int j = 0; j < 3; j++) {
					const de_polytope_edge_t current_edge = { 
						.begin = triangle->vertices[j],
						.end = triangle->vertices[(j + 1) % 3]
					};

					bool already_in_list = false;
					/* Check if current edge is already in list */
					for (int k = 0; k < loose_edge_count; k++) { 
						de_polytope_edge_t* loose_edge = loose_edges + k;					 
						if (de_polytope_edge_eq_ccw(loose_edge, &current_edge)) {
							/* If we found that current edge is same as other loose edge
							 * but in reverse order, then we need to replace the loose
							 * edge by the last loose edge. Lets see at this drawing and 
							 * follow it step-by-step. This is pyramid with tip below A
							 * and bottom BCDE divided into 4 triangles by point A. All
							 * triangles given in CCW order.
							 *       
							 *       B
							 *      /|\
							 *     / | \
							 *    /  |  \
							 *   /   |   \
							 *  /    |    \
							 * E-----A-----C
							 *  \    |    /
							 *   \   |   /
							 *    \  |  /
							 *     \ | /
							 *      \|/
							 *       D
							 *       
							 * We found that triangles we want to remove are ACB (1), ADC (2),
							 * AED (3), and ABE (4). Lets start removing them from triangle ACB.
							 * Also we have to keep list of loose edges for futher linking with
							 * new point.
							 * 
							 * 1. AC, CB, BA - just edges of ACB triangle in CCW order.
							 * 
							 * 2. AC, CB, BA, AD, DC, (CA) - we see that CA already in list
							 *                               but in reverse order. Do not add it
							 *                               but move DC onto AC position.                        
							 *    DC, CB, BA, AD - loose edge list for 2nd triangle
							 *    
							 * 3. DC, CB, BA, AD, AE, ED, (DA) - again we already have DA in list as AD
							 *                                   move ED to AD position.
							 *    DC, CB, BA, ED, AE
							 *
							 * 4. DC, CB, BA, ED, AE, (AB) - same AB already here as BA, move AE to BA.
							 *    continue adding rest of edges
							 *    DC, CB, AE, ED, BE, (EA) - EA already here as AE, move BE to AE 
							 *    
							 *    DC, CB, BE, ED - final list of loose edges which gives us
							 *    
							 *       B
							 *      / \
							 *     /   \
							 *    /     \
							 *   /       \
							 *  /         \
							 * E           C
							 *  \         /
							 *   \       /
							 *    \     /
							 *     \   /
							 *      \ /
							 *       D
							 *       
							 * Viola! We now have contour which we have to patch using new point.   
							 **/														
							loose_edge_count--;
							*loose_edge = loose_edges[loose_edge_count]; 	
														
							already_in_list = true;
							break;
						}
					}

					if (!already_in_list) { 
						/* Add current edge to list */
						if (loose_edge_count >= DE_EPA_MAX_LOOSE_EDGES) {
							break;
						}
						loose_edges[loose_edge_count++] = current_edge;						
					}
				}

				/* Replace current triangle with last in list and discard last, so we will continue
				 * processing last triangle and then next to removed. This will effectively reduce
				 * amount of triangles in polytope. */			
				*triangle = *(triangles + (--triangle_count));
				--i;
			}
		}

		/* Reconstruct polytope with new point added */
		for (int i = 0; i < loose_edge_count; i++) {			
			if (triangle_count >= DE_EPA_MAX_FACES) {
				break;
			}
			const de_polytope_edge_t* loose_edge = loose_edges + i;
			de_polytope_triangle_t* new_triangle = triangles + triangle_count;
			new_triangle->vertices[0] = loose_edge->begin;
			new_triangle->vertices[1] = loose_edge->end;
			new_triangle->vertices[2] = new_point;

			de_vec3_t edge_vector;
			de_vec3_sub(&edge_vector, &loose_edge->begin.minkowski_dif, &loose_edge->end.minkowski_dif);

			de_vec3_t begin_to_point;
			de_vec3_sub(&begin_to_point, &loose_edge->begin.minkowski_dif, &new_point.minkowski_dif);

			de_vec3_cross(&new_triangle->normal, &edge_vector, &begin_to_point);
			de_vec3_normalize(&new_triangle->normal, &new_triangle->normal);
			
			/* Check for wrong normal to maintain CCW winding */
			const float bias = 2 * FLT_EPSILON;
			if (de_vec3_dot(&new_triangle->vertices[0].minkowski_dif, &new_triangle->normal) + bias < 0) {
				/* Swap vertices to make CCW winding and flip normal. */
				de_minkowski_vertex_t temp = new_triangle->vertices[0];
				new_triangle->vertices[0] = new_triangle->vertices[1];
				new_triangle->vertices[1] = temp;				
				de_vec3_negate(&new_triangle->normal, &new_triangle->normal);
			}
			triangle_count++;
		}
	} 

	/* Return most recent closest point - this is still valid result but less accurate than
	 * if we would have total convergence. */
	de_polytope_triangle_t* closest_triangle = triangles + closest_triangle_index;
	de_vec3_scale(penetration_vector, &closest_triangle->normal,
		de_vec3_dot(&closest_triangle->vertices[0].minkowski_dif, &closest_triangle->normal));
	return true;
}

#undef DE_EPA_TOLERANCE
#undef DE_EPA_MAX_FACES
#undef DE_EPA_MAX_LOOSE_EDGES
#undef DE_EPA_MAX_ITERATIONS
