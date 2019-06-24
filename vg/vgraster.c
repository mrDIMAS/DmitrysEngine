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

void de_bitmap_create(de_bitmap_t * bitmap, int width, int height)
{
	bitmap->width = width;
	bitmap->height = height;
	bitmap->pixels = (unsigned char*)de_calloc(width, height);
}

void de_bitmap_set_pixel(de_bitmap_t* bitmap, int x, int y, uint8_t pixel)
{
	if (x < 0 || x >= bitmap->width) {
		return;
	}
	if (y < 0 || y >= bitmap->height) {
		return;
	}

	bitmap->pixels[y * bitmap->width + x] = pixel;
}

float de_bitmap_get_fpixel(const de_bitmap_t* bitmap, int x, int y)
{
	if (x < 0 || x >= bitmap->width) {
		return 0;
	}
	if (y < 0 || y >= bitmap->height) {
		return 0;
	}

	return bitmap->pixels[y * bitmap->width + x] / 255.0f;
}

point_array_t de_vg_eval_quad_bezier(const de_point_t * p0, const de_point_t * p1, const de_point_t * p2, int steps)
{
	point_array_t points;

	const float step = 1.0f / steps;

	DE_ARRAY_INIT(points);

	for (float t = 0.0; t <= 1.0f; t += step) {
		const float inv_t = 1 - t;
		const float k0 = inv_t * inv_t;
		const float k1 = 2 * t * inv_t;
		const float k2 = t * t;
		const de_point_t pt = {
			.x = k0 * p0->x + k1 * p1->x + k2 * p2->x,
			.y = k0 * p0->y + k1 * p1->y + k2 * p2->y
		};
		DE_ARRAY_APPEND(points, pt);
	}

	return points;
}

bool de_vg_line_line_intersection(const de_line2_t* a, const de_line2_t* b, de_point_t *out)
{
	const float s1x = a->end.x - a->begin.x;
	const float s1y = a->end.y - a->begin.y;
	const float s2x = b->end.x - b->begin.x;
	const float s2y = b->end.y - b->begin.y;
	const float s = (-s1y * (a->begin.x - b->begin.x) + s1x * (a->begin.y - b->begin.y)) / (-s2x * s1y + s1x * s2y);
	const float t = (s2x * (a->begin.y - b->begin.y) - s2y * (a->begin.x - b->begin.x)) / (-s2x * s1y + s1x * s2y);
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		out->x = a->begin.x + (t * s1x);
		out->y = a->begin.y + (t * s1y);
		return true;
	}
	return false;
}

static int de_compare_floats(const void* a_ptr, const void* b_ptr)
{
	const float* a = (const float*)a_ptr;
	const float* b = (const float*)b_ptr;
	if (*a < *b) return -1;
	if (*a == *b) return 0;
	return 1;
}

void de_vg_polys_to_scanlines(de_polygon_t* polys, size_t poly_count, float width, float height, float scale, line_array_t* out)
{
	const float bias = 0.0001f;
	const float y_oversample = 5.0f;
	const float y_step = 1.0f / y_oversample;

	const float real_width = scale * width;
	const float real_height = scale * height;

	DE_ARRAY_CLEAR(*out);

	point_array_t intersections;
	DE_ARRAY_INIT(intersections);

	de_line2_t scanline = {
		.begin.x = -1,
		.begin.y = 0,
		.end.x = real_width + 2,
		.end.y = 0,
	};

	for (float y = bias; y < real_height; y += y_step) {
		DE_ARRAY_CLEAR(intersections);

		scanline.begin.y = y;
		scanline.end.y = y;

		/* Find all intersection points for current y */
		for (size_t i = 0; i < poly_count; ++i) {
			de_polygon_t * poly = polys + i;

			for (size_t j = 0; j < poly->points.size; j += 2) {
				de_line2_t edge;

				edge.begin = poly->points.data[j];
				edge.end = poly->points.data[j + 1];

				edge.begin.x *= scale;
				edge.begin.y *= scale;

				edge.end.x *= scale;
				edge.end.y *= scale;

				de_point_t intersection;
				if (de_vg_line_line_intersection(&scanline, &edge, &intersection)) {					
					DE_ARRAY_APPEND(intersections, intersection);
				}
			}
		}

		DE_ARRAY_QSORT(intersections, de_compare_floats);

		if (intersections.size % 2 != 0) {
			for (size_t i = 0; i < intersections.size; ++i) {
				printf("%f; %f\n", intersections.data[i].x, intersections.data[i].y);
			}

			de_log("scanline rasterization failed!");
		}

		/* Convert intersection points into scanlines */
		for (size_t i = 0; i < intersections.size; i += 2) {
			const de_line2_t line = {
				.begin.x = intersections.data[i].x,
				.begin.y = y,
				.end.x = intersections.data[i + 1].x,
				.end.y = y
			};
			DE_ARRAY_APPEND(*out, line);
		}
	}

	DE_ARRAY_FREE(intersections);
}

de_bitmap_t de_bitmap_downscale4_box_filter(de_bitmap_t *src)
{
	const float factor = 4.0;
	const int bitmap_width = (int)ceil(src->width / factor);
	const int bitmap_height = (int)ceil(src->height / factor);

	de_bitmap_t bitmap;
	de_bitmap_create(&bitmap, bitmap_width, bitmap_height);

	for (int dest_y = 0; dest_y < bitmap.height; ++dest_y) {
		const int src_y = 4 * dest_y;
		for (int dest_x = 0; dest_x < bitmap.width; ++dest_x) {
			const int src_x = 4 * dest_x;
			unsigned int sum = 0;
			for (int dy = 0; dy < 4; ++dy) {
				for (int dx = 0; dx < 4; ++dx) {
					const int ix = src_x + dx;
					const int iy = src_y + dy;
					if (ix < src->width && iy < src->height) {
						unsigned int index = iy * src->width + ix;
						sum += src->pixels[index];
					}
				}
			}
			sum /= 16;
			if (sum > 255) {
				sum = 255;
			}
			de_bitmap_set_pixel(&bitmap, dest_x, dest_y, (uint8_t)sum);
		}
	}

	return bitmap;
}

de_bitmap_t de_vg_raster_scanlines(de_bitmap_t* bitmap, line_array_t lines)
{
	/* Antialized scanline rasterization. */
	for (size_t i = 0; i < lines.size; ++i) {
		const de_line2_t* scanline = lines.data + i;
		const int yi = (int)scanline->begin.y;

		/* Calculate new opacity for pixel at the begin of scanline. */
		const float bx = scanline->begin.x;
		const float begin_opacity = 0.5f * (((float)ceil(bx) - bx) + de_bitmap_get_fpixel(bitmap, (int)bx, yi));
		de_bitmap_set_pixel(bitmap, (int)bx, yi, (uint8_t)(255.0f * begin_opacity));

		/* Calculate new opacity for pixel at the end of scanline. */
		const float ex = scanline->end.x;
		const float end_opacity = 0.5f * ((ex - (float)floor(ex)) + de_bitmap_get_fpixel(bitmap, (int)ex, yi));
		de_bitmap_set_pixel(bitmap, (int)ex, yi, (uint8_t)(255.0f * end_opacity));

		/* Modulate rest with opaque color. */
		const int begin = (int)ceil(bx);
		const int end = (int)floor(ex);
		for (int x = begin; x < end; ++x) {
			const float value = 0.5f * (1.0f + de_bitmap_get_fpixel(bitmap, x, yi));
			de_bitmap_set_pixel(bitmap, x, yi, (uint8_t)(255.0f * value));
		}
	}

	const int border = 4;
	de_bitmap_t out_bitmap;
	de_bitmap_create(&out_bitmap, bitmap->width + border, bitmap->height + border);

	/* add border to glyph to remove seams due to bilinear filtration on GPU */
	const int half_border = border / 2;
	for (int row = half_border; row < out_bitmap.height; ++row) {
		for (int col = half_border; col < out_bitmap.width; ++col) {
			int r = row - half_border;
			int c = col - half_border;
			if (r < bitmap->height && c < bitmap->width) {
				out_bitmap.pixels[row * out_bitmap.width + col] = bitmap->pixels[r * bitmap->width + c];
			}
		}
	}

	return out_bitmap;
}