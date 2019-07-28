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

typedef enum de_convex_shape_type_t {
	/* Be very careful when adding new types here, follow an already 
	 * existing type and implement all operations for new type! 
	 * VERY IMPORTANT: Add new types only in the end of the enum, otherwise
	 * you'll break compatibility with save files of previous versions! */
	DE_CONVEX_SHAPE_TYPE_BOX,
	DE_CONVEX_SHAPE_TYPE_SPHERE,
	DE_CONVEX_SHAPE_TYPE_CAPSULE,
	DE_CONVEX_SHAPE_TYPE_TRIANGLE,	
	DE_CONVEX_SHAPE_TYPE_POINT_CLOUD
} de_convex_shape_type_t;

typedef struct de_sphere_shape_t {
	float radius;
} de_sphere_shape_t;

typedef struct de_triangle_shape_t {
	de_vec3_t vertices[3];
} de_triangle_shape_t;

typedef struct de_box_shape_t {
	de_vec3_t half_extents;
} de_box_shape_t;

typedef DE_ARRAY_DECLARE(de_vec3_t, de_point_cloud_point_array_t);

typedef struct de_point_cloud_shape_t {
	de_point_cloud_point_array_t points;
} de_point_cloud_shape_t;

typedef enum de_axis_t {
	DE_AXIS_X = 0,
	DE_AXIS_Y = 1,
	DE_AXIS_Z = 2,
} de_axis_t;

typedef struct de_capsule_shape_t {
	de_axis_t axis;
	float radius;
	float height;
	/* Non-serializable. Calculated in runtime. */
	float dimensions[3];
} de_capsule_shape_t;

/**
 * @brief Convex collision shape.
 * 
 * TODO: Add scaling support!
 */
typedef struct de_convex_shape_t {
	de_convex_shape_type_t type;
	union {
		de_box_shape_t box;
		de_sphere_shape_t sphere;
		de_capsule_shape_t capsule;
		de_triangle_shape_t triangle;
		de_point_cloud_shape_t point_cloud;
	} s;
} de_convex_shape_t;

/**
 * @brief Create capsule convex shape using specified axis (X, Y, or Z), radius, and height.
 * Total height is height + 2 * radius, so the \c height param must be distance between centers
 * of sphere caps of capsule.
 */
de_convex_shape_t de_convex_shape_create_capsule(de_axis_t axis, float radius, float height);

/**
 * @brief Creates sphere convex shape with specified radius. Radius must be greater than zero!
 */
de_convex_shape_t de_convex_shape_create_sphere(float radius);

/**
 * @brief Creates point cloud convex shape. Takes ownership of points array!
 */
de_convex_shape_t de_convex_shape_create_point_cloud(de_point_cloud_point_array_t points);

/**
 * @brief Returns farthest point in given direction. 
 */
de_vec3_t de_convex_shape_get_farthest_point(const de_convex_shape_t* shape, const de_vec3_t* position, const de_vec3_t* dir);

float de_capsule_shape_get_radius(const de_capsule_shape_t* capsule);

void de_capsule_shape_set_radius(de_capsule_shape_t* capsule, float radius);

float de_capsule_shape_get_height(const de_capsule_shape_t* capsule);

void de_capsule_shape_set_height(de_capsule_shape_t* capsule, float height);

void de_sphere_shape_set_radius(de_sphere_shape_t* shape, float radius);

float de_sphere_shape_get_radius(const de_sphere_shape_t* shape);

de_box_shape_t* de_convex_shape_to_box(de_convex_shape_t* convex_shape);

de_capsule_shape_t* de_convex_shape_to_capsule(de_convex_shape_t* convex_shape);

de_triangle_shape_t* de_convex_shape_to_triangle(de_convex_shape_t* convex_shape);

de_sphere_shape_t* de_convex_shape_to_sphere(de_convex_shape_t* convex_shape);

de_point_cloud_shape_t* de_convex_shape_to_point_cloud(de_convex_shape_t* convex_shape);

void de_convex_shape_free(de_convex_shape_t* shape);

bool de_convex_shape_visit(de_object_visitor_t* visitor, de_convex_shape_t* shape);

void de_capsule_shape_get_extreme_points(const de_capsule_shape_t* capsule, de_vec3_t* pa, de_vec3_t* pb);