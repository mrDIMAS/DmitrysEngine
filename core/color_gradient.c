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

de_color_t de_color_gradient_get_color(const de_color_gradient_t* gradient, float location)
{
	/* handle edge cases */
	if (gradient->points.size == 0) {
		/* stub - opaque white */
		return (de_color_t) { 255, 255, 255, 255 };
	} else if (gradient->points.size == 1) {
		/* single point - just return its color */
		return gradient->points.data[0].color;
	} else if (gradient->points.size == 2) {
		/* special case for two points (much faster than generic) */
		const de_color_gradient_point_t* pt_a = gradient->points.data + 0;
		const de_color_gradient_point_t* pt_b = gradient->points.data + 1;
		if (location >= pt_a->location && location <= pt_b->location) {
			/* linear interpolation */
			const float span = pt_b->location - pt_a->location;
			const float t = (location - pt_a->location) / span;
			return de_color_interpolate(&pt_a->color, &pt_b->color, t);
		} else {
			if (location < pt_a->location) {
				return pt_a->color;
			} else {
				return pt_b->color;
			}
		}
	}

	/* generic case */
	/* check for out-of-bounds */
	const de_color_gradient_point_t* first = &DE_ARRAY_FIRST(gradient->points);
	const de_color_gradient_point_t* last = &DE_ARRAY_LAST(gradient->points);
	if (location <= first->location) {
		return first->color;
	} else if (location >= last->location) {
		return last->color;
	} else {
		/* find span first */
		de_color_gradient_point_t* pt_a = NULL, *pt_b = NULL; /* make compiler happy - set default values */
		for (size_t i = 0; i < gradient->points.size; ++i) {
			de_color_gradient_point_t* pt = gradient->points.data + i;
			if (location >= pt->location) {
				pt_a = pt;
			}
		}
		pt_b = pt_a + 1;
		/* linear interpolation */
		const float span = pt_b->location - pt_a->location;
		const float t = (location - pt_a->location) / span;
		return de_color_interpolate(&pt_a->color, &pt_b->color, t);
	}
}

static int de_color_gradient_point_sort(const void* a, const void* b)
{
	const de_color_gradient_point_t* pt_a = a;
	const de_color_gradient_point_t* pt_b = b;
	if (pt_a->location < pt_b->location) {
		return -1;
	} else if (pt_a->location > pt_b->location) {
		return 1;
	}
	return 0;
}

void de_color_gradient_add_point(de_color_gradient_t* gradient, float location, const de_color_t* color)
{
	DE_ASSERT(location >= 0.0f && location <= 1.0f);
	const de_color_gradient_point_t point = { .location = location, .color = *color };
	DE_ARRAY_APPEND(gradient->points, point);
	DE_ARRAY_QSORT(gradient->points, de_color_gradient_point_sort);
}

void de_color_gradient_clear(de_color_gradient_t* gradient)
{
	DE_ASSERT(gradient);
	DE_ARRAY_CLEAR(gradient->points);
}

void de_color_gradient_free(de_color_gradient_t* gradient)
{
	DE_ASSERT(gradient);
	DE_ARRAY_FREE(gradient->points);
}

void de_color_gradient_init(de_color_gradient_t* gradient) {
	DE_ARRAY_INIT(gradient->points);
}

static bool de_color_gradient_point_visit(de_object_visitor_t* visitor, de_color_gradient_point_t* pt) 
{
	bool result = true;
	result &= de_object_visitor_visit_float(visitor, "Location", &pt->location);
	result &= de_object_visitor_visit_color(visitor, "Color", &pt->color);	
	return true;
}

bool de_color_gradient_visit(de_object_visitor_t* visitor, de_color_gradient_t* gradient) 
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_ARRAY(visitor, "Points", gradient->points, de_color_gradient_point_visit);
	return result;
}

void de_color_gradient_tests() {
	de_color_gradient_t gradient;
	de_color_gradient_init(&gradient);
	de_color_t color = de_color_gradient_get_color(&gradient, 0.0f);
	DE_ASSERT(color.r == 255 && color.g == 255 && color.b == 255 && color.a == 255);	
	de_color_gradient_add_point(&gradient, 0.00f, &(de_color_t) { 255, 255, 255, 0 });
	color = de_color_gradient_get_color(&gradient, 0.0f);
	DE_ASSERT(color.r == 255 && color.g == 255 && color.b == 255 && color.a == 0);
	de_color_gradient_add_point(&gradient, 0.95f, &(de_color_t) { 255, 255, 255, 255 });
	de_color_gradient_add_point(&gradient, 0.05f, &(de_color_t) { 255, 255, 255, 255 });	
	de_color_gradient_add_point(&gradient, 1.00f, &(de_color_t) { 255, 255, 255, 0 });
	for(float t = 0; t < 1.0f; t += 0.01f) {
		color = de_color_gradient_get_color(&gradient, t);
		printf("%d;%d;%d;%d\n", color.r, color.g, color.b, color.a);
	}
}