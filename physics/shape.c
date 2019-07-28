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


static float de_capsule_shape_get_radius_internal(const de_capsule_shape_t* capsule)
{
	const int axis = (capsule->axis + 2) % 3;
	return capsule->dimensions[axis];
}

static float de_capsule_shape_get_height_internal(const de_capsule_shape_t* capsule)
{
	return capsule->dimensions[capsule->axis];
}

de_vec3_t de_convex_shape_get_farthest_point(const de_convex_shape_t* shape, const de_vec3_t* position, const de_vec3_t* dir)
{
	de_vec3_t farthest;

	switch (shape->type) {
		case DE_CONVEX_SHAPE_TYPE_BOX: {
			const de_box_shape_t* box = &shape->s.box;

			farthest.x = dir->x >= 0 ? box->half_extents.x : -box->half_extents.x;
			farthest.y = dir->y >= 0 ? box->half_extents.y : -box->half_extents.y;
			farthest.z = dir->z >= 0 ? box->half_extents.z : -box->half_extents.z;

			break;
		}
		case DE_CONVEX_SHAPE_TYPE_SPHERE: {
			const de_sphere_shape_t* sphere = &shape->s.sphere;

			de_vec3_t norm_dir;
			de_vec3_normalize(&norm_dir, dir);
			de_vec3_scale(&farthest, &norm_dir, sphere->radius);

			break;
		}
		case DE_CONVEX_SHAPE_TYPE_TRIANGLE: {
			const de_triangle_shape_t* triangle = &shape->s.triangle;
			
			farthest = de_point_cloud_get_farthest_point(triangle->vertices, 3, dir);

			break;
		}
		case DE_CONVEX_SHAPE_TYPE_POINT_CLOUD: {
			const de_point_cloud_shape_t* point_cloud = &shape->s.point_cloud;

			farthest = de_point_cloud_get_farthest_point(point_cloud->points.data, point_cloud->points.size, dir);

			break;
		}
		case DE_CONVEX_SHAPE_TYPE_CAPSULE: {
			const de_capsule_shape_t* capsule = &shape->s.capsule;

			de_vec3_t norm_dir;
			if (!de_vec3_try_normalize(&norm_dir, dir)) {
				norm_dir = (de_vec3_t) { 1.0f, 0.0f, 0.0f };
			}

			const float radius = de_capsule_shape_get_radius_internal(capsule);
			const float half_height = de_capsule_shape_get_height_internal(capsule);

			float max = -FLT_MAX;
			/* Check upper cap (or other sides in case of other axes) */
			{
				de_vec3_t pos = (de_vec3_t) { 0.0f, 0.0f, 0.0f };
				((float*)&pos)[capsule->axis] = half_height;

				de_vec3_t vertex;
				de_vec3_scale(&vertex, &norm_dir, radius);
				de_vec3_add(&vertex, &vertex, &pos);
				const float dot = de_vec3_dot(&norm_dir, &vertex);
				if (dot > max) {
					max = dot;
					farthest = vertex;
				}
			}

			/* Check lower cap (or other sides in case of other axes) */
			{
				de_vec3_t pos = (de_vec3_t) { 0.0f, 0.0f, 0.0f };
				((float*)&pos)[capsule->axis] = -half_height;

				de_vec3_t vertex;
				de_vec3_scale(&vertex, &norm_dir, radius);
				de_vec3_add(&vertex, &vertex, &pos);
				const float dot = de_vec3_dot(&norm_dir, &vertex);
				if (dot > max) {
					max = dot;
					farthest = vertex;
				}
			}

			break;
		}
		default:
			/* Make compiler happy */
			farthest = (de_vec3_t) { 0, 0, 0 };

			/* and then crash. TODO: Maybe its better to let compiler give a warning? Only problem in Visual Studio
			* for some reason it does not complain about missing case. :/ */
			de_fatal_error("gjk-epa failure: forgot to add method to get farthest point in direction for shape type %d!", shape->type);
			break;
	}

	de_vec3_add(&farthest, &farthest, position);

	return farthest;
}

static void de_capsule_shape_set_dimensions(de_capsule_shape_t* capsule, de_axis_t axis, float radius, float height)
{
	switch (axis) {
		case DE_AXIS_X: {
			capsule->dimensions[0] = 0.5f * height;
			capsule->dimensions[1] = radius;
			capsule->dimensions[2] = radius;
			break;
		}
		case DE_AXIS_Y: {
			capsule->dimensions[0] = radius;
			capsule->dimensions[1] = 0.5f * height;
			capsule->dimensions[2] = radius;
			break;
		}
		case DE_AXIS_Z: {
			capsule->dimensions[0] = radius;
			capsule->dimensions[1] = radius;
			capsule->dimensions[2] = 0.5f * height;
			break;
		}
	}
}

de_convex_shape_t de_convex_shape_create_capsule(de_axis_t axis, float radius, float height)
{
	DE_ASSERT(radius > 0);
	DE_ASSERT(height > 0);

	de_convex_shape_t shape = {
		.type = DE_CONVEX_SHAPE_TYPE_CAPSULE,
	};

	de_capsule_shape_t* capsule = &shape.s.capsule;
	capsule->axis = axis;
	capsule->radius = radius;
	capsule->height = height;
	de_capsule_shape_set_dimensions(capsule, axis, radius, height);
	return shape;
}

de_convex_shape_t de_convex_shape_create_sphere(float radius)
{
	DE_ASSERT(radius > 0);

	const de_convex_shape_t shape = {
		.type = DE_CONVEX_SHAPE_TYPE_SPHERE,
		.s.sphere = (de_sphere_shape_t) { .radius = radius }
	};

	return shape;
}

de_convex_shape_t de_convex_shape_create_point_cloud(de_point_cloud_point_array_t points)
{
	const de_convex_shape_t shape = {
		.type = DE_CONVEX_SHAPE_TYPE_SPHERE,
		.s.point_cloud = (de_point_cloud_shape_t) { .points = points }
	};

	return shape;
}

void de_convex_shape_free(de_convex_shape_t* shape)
{
	if (shape->type == DE_CONVEX_SHAPE_TYPE_POINT_CLOUD) {
		de_point_cloud_shape_t* point_cloud = &shape->s.point_cloud;
		DE_ARRAY_FREE(point_cloud->points);
	}
}

bool de_convex_shape_visit(de_object_visitor_t* visitor, de_convex_shape_t* shape)
{
	bool result = true;

	result &= DE_OBJECT_VISITOR_VISIT_ENUM(visitor, "Type", &shape->type);

	switch (shape->type) {
		case DE_CONVEX_SHAPE_TYPE_BOX: {
			de_box_shape_t* box = &shape->s.box;
			result &= de_object_visitor_visit_vec3(visitor, "HalfExtents", &box->half_extents);
			break;
		}
		case DE_CONVEX_SHAPE_TYPE_SPHERE: {
			de_sphere_shape_t* sphere = &shape->s.sphere;
			result &= de_object_visitor_visit_float(visitor, "Radius", &sphere->radius);
			break;
		}
		case DE_CONVEX_SHAPE_TYPE_CAPSULE: {
			de_capsule_shape_t* capsule = &shape->s.capsule;
			result &= DE_OBJECT_VISITOR_VISIT_ENUM(visitor, "Axis", &capsule->axis);
			result &= de_object_visitor_visit_float(visitor, "Radius", &capsule->radius);
			result &= de_object_visitor_visit_float(visitor, "Height", &capsule->height);

			/* Recalculate dimensions on read. */
			if (visitor->is_reading) {
				de_capsule_shape_set_dimensions(capsule, capsule->axis, capsule->radius, capsule->height);
			}
			break;
		}
		case DE_CONVEX_SHAPE_TYPE_TRIANGLE: {
			de_triangle_shape_t* triangle = &shape->s.triangle;
			result &= de_object_visitor_visit_vec3(visitor, "A", &triangle->vertices[0]);
			result &= de_object_visitor_visit_vec3(visitor, "B", &triangle->vertices[1]);
			result &= de_object_visitor_visit_vec3(visitor, "C", &triangle->vertices[2]);
			break;
		}
		case DE_CONVEX_SHAPE_TYPE_POINT_CLOUD: {
			de_point_cloud_shape_t* point_cloud = &shape->s.point_cloud;
			result &= DE_OBJECT_VISITOR_VISIT_PRIMITIVE_ARRAY(visitor, "Points", point_cloud->points);
			break;
		}
		default:
			/* TODO: Maybe its better to let compiler give a warning? Only problem in Visual Studio
			 * for some reason it does not complain about missing case. :/ */
			de_fatal_error("visit failure: forgot to add visit for shape type %d!", shape->type);
	}

	return result;
}

float de_capsule_shape_get_radius(const de_capsule_shape_t* capsule)
{
	return capsule->radius;
}

float de_capsule_shape_get_height(const de_capsule_shape_t* capsule)
{
	return capsule->height;
}

void de_capsule_shape_set_radius(de_capsule_shape_t* capsule, float radius)
{
	capsule->radius = radius;
	de_capsule_shape_set_dimensions(capsule, capsule->axis, radius, capsule->height);
}

void de_capsule_shape_set_height(de_capsule_shape_t* capsule, float height)
{
	capsule->height = height;
	de_capsule_shape_set_dimensions(capsule, capsule->axis, capsule->radius, height);
}

void de_sphere_shape_set_radius(de_sphere_shape_t* shape, float radius)
{
	DE_ASSERT(shape);
	DE_ASSERT(radius > 0);
	shape->radius = radius;
}

float de_sphere_shape_get_radius(const de_sphere_shape_t* shape)
{
	DE_ASSERT(shape);
	return shape->radius;
}

void de_capsule_shape_get_extreme_points(const de_capsule_shape_t* capsule, de_vec3_t* pa, de_vec3_t* pb) 
{
	const float half_height = capsule->height * 0.5f;
	switch (capsule->axis) {
		case DE_AXIS_X: {
			pa->x = -half_height;
			pa->y = 0.0f;
			pa->z = 0.0f;

			pb->x = half_height;
			pb->y = 0.0f;
			pb->z = 0.0f;
			break;
		}
		case DE_AXIS_Y: {
			pa->x = 0.0f;
			pa->y = -half_height;
			pa->z = 0.0f;

			pb->x = 0.0f;
			pb->y = half_height;
			pb->z = 0.0f;
			break;
		}
		case DE_AXIS_Z: {
			pa->x = 0.0f;
			pa->y = 0.0f;
			pa->z = -half_height;

			pb->x = 0.0f;
			pb->y = 0.0f;
			pb->z = half_height;
			break;
		}
	}
}

de_box_shape_t* de_convex_shape_to_box(de_convex_shape_t* convex_shape)
{
	DE_ASSERT(convex_shape->type == DE_CONVEX_SHAPE_TYPE_BOX);
	return &convex_shape->s.box;
}

de_capsule_shape_t* de_convex_shape_to_capsule(de_convex_shape_t* convex_shape)
{
	DE_ASSERT(convex_shape->type == DE_CONVEX_SHAPE_TYPE_CAPSULE);
	return &convex_shape->s.capsule;
}

de_triangle_shape_t* de_convex_shape_to_triangle(de_convex_shape_t* convex_shape)
{
	DE_ASSERT(convex_shape->type == DE_CONVEX_SHAPE_TYPE_TRIANGLE);
	return &convex_shape->s.triangle;
}

de_sphere_shape_t* de_convex_shape_to_sphere(de_convex_shape_t* convex_shape)
{
	DE_ASSERT(convex_shape->type == DE_CONVEX_SHAPE_TYPE_SPHERE);
	return &convex_shape->s.sphere;
}

de_point_cloud_shape_t* de_convex_shape_to_point_cloud(de_convex_shape_t* convex_shape)
{
	DE_ASSERT(convex_shape->type == DE_CONVEX_SHAPE_TYPE_POINT_CLOUD);
	return &convex_shape->s.point_cloud;
}