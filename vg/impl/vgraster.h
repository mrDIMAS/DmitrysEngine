/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

/*=======================================================================================*/
void de_bitmap_create(de_bitmap_t * bitmap, int width, int height)
{
	bitmap->width = width;
	bitmap->height = height;
	bitmap->pixels = (unsigned char*)de_calloc(width, height);
}

/*=======================================================================================*/
void de_bitmap_set_pixel(de_bitmap_t* bitmap, int x, int y, unsigned char pixel)
{
	if (x < 0 || x >= bitmap->width)
	{
		return;
	}
	if (y < 0 || y >= bitmap->height)
	{
		return;
	}

	bitmap->pixels[y * bitmap->width + x] = pixel;
}

/*=======================================================================================*/
point_array_t de_vg_eval_quad_bezier(const de_point_t * p0, const de_point_t * p1, const de_point_t * p2, int steps)
{
	point_array_t points;
	float t;

	float step = 1.0f / steps;

	DE_ARRAY_INIT(points);

	for (t = 0.0; t <= 1.0f; t += step)
	{
		float inv_t = 1 - t;
		float k0 = inv_t * inv_t;
		float k1 = 2 * t * inv_t;
		float k2 = t * t;
		de_point_t pt;

		pt.x = k0 * p0->x + k1 * p1->x + k2 * p2->x;
		pt.y = k0 * p0->y + k1 * p1->y + k2 * p2->y;

		DE_ARRAY_APPEND(points, pt);
	}

	return points;
}

/*=======================================================================================*/
bool de_line_line_intersection(de_line_t* a, de_line_t* b, de_point_t *out)
{
	float s1x = a->end.x - a->begin.x;
	float s1y = a->end.y - a->begin.y;
	float s2x = b->end.x - b->begin.x;
	float s2y = b->end.y - b->begin.y;
	float s = (-s1y * (a->begin.x - b->begin.x) + s1x * (a->begin.y - b->begin.y)) / (-s2x * s1y + s1x * s2y);
	float t = (s2x * (a->begin.y - b->begin.y) - s2y * (a->begin.x - b->begin.x)) / (-s2x * s1y + s1x * s2y);
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		out->x = a->begin.x + (t * s1x);
		out->y = a->begin.y + (t * s1y);
		return true;
	}
	return false;
}

/*=======================================================================================*/
static int de_compare_floats(const void* a_ptr, const void* b_ptr)
{
	const float* a = (const float*)a_ptr;
	const float* b = (const float*)b_ptr;
	if (*a < *b) return -1;
	if (*a == *b) return 0;
	return 1;
}

/*=======================================================================================*/
line_array_t de_vg_polys_to_scanlines(de_polygon_t* polys, size_t poly_count, float width, float height, float scale)
{
	size_t i;
	size_t j;
	line_array_t scanlines;
	point_array_t intersections;
	float real_width;
	float real_height;
	float y;
	const float bias = 0.0001f;

	real_width = scale * width;
	real_height = scale * height;

	DE_ARRAY_INIT(scanlines);
	DE_ARRAY_INIT(intersections);

	for (y = bias; y <= real_height; ++y)
	{
		de_line_t scanline;
		DE_ARRAY_CLEAR(intersections);

		scanline.begin.x = -1;
		scanline.begin.y = y;

		scanline.end.x = real_width + 2;
		scanline.end.y = y;

		/* Find all intersection points for current y */
		for (i = 0; i < poly_count; ++i)
		{
			de_polygon_t * poly = polys + i;

			for (j = 0; j < poly->points.size; j += 2)
			{
				de_line_t edge;
				de_point_t intersection;

				edge.begin = poly->points.data[j];
				edge.end = poly->points.data[j + 1];

				edge.begin.x *= scale;
				edge.begin.y *= scale;

				edge.end.x *= scale;
				edge.end.y *= scale;

				if (de_line_line_intersection(&scanline, &edge, &intersection))
				{
					DE_ARRAY_APPEND(intersections, intersection);
				}
			}
		}

		DE_ARRAY_QSORT(intersections, de_compare_floats);

		if (intersections.size % 2 != 0)
		{
			for (i = 0; i < intersections.size; ++i)
			{
				printf("%f; %f\n", intersections.data[i].x, intersections.data[i].y);
			}

			de_log("Glyph rasterization failed!");
		}

		/* Convert intersection points into scanlines */
		for (i = 0; i < intersections.size; i += 2)
		{
			de_line_t line;

			line.begin.x = intersections.data[i].x;
			line.begin.y = y;

			line.end.x = intersections.data[i + 1].x;
			line.end.y = y;

			DE_ARRAY_APPEND(scanlines, line);
		}
	}

	DE_ARRAY_FREE(intersections);

	return scanlines;
}

/*=======================================================================================*/
de_bitmap_t de_bitmap_downscale4_box_filter(de_bitmap_t *src)
{
	int dest_y;
	int dest_x;
	int dy;
	int dx;
	de_bitmap_t bitmap;
	de_bitmap_create(&bitmap, (int)((float)src->width / 4 + 0.5f), (int)((float)src->height / 4 + 0.5f));

	for (dest_y = 0; dest_y < bitmap.height; ++dest_y)
	{
		int src_y = 4 * dest_y;

		for (dest_x = 0; dest_x < bitmap.width; ++dest_x)
		{
			int src_x = 4 * dest_x;

			unsigned int sum = 0;
			for (dy = 0; dy < 4; ++dy)
			{
				for (dx = 0; dx < 4; ++dx)
				{
					int ix = src_x + dx;
					int iy = src_y + dy;

					if (ix < src->width && iy < src->height)
					{
						unsigned int index = iy * src->width + ix;

						sum += src->pixels[index];
					}
				}
			}

			sum /= 16;

			if (sum > 255)
			{
				sum = 255;
			}

			de_bitmap_set_pixel(&bitmap, dest_x, dest_y, (unsigned char)sum);
		}
	}

	return bitmap;
}

/*=======================================================================================*/
de_bitmap_t de_vg_raster_scanlines(de_bitmap_t* bitmap, line_array_t lines)
{
	int row, col;
	int border = 4;
	int half_border = border / 2;
	float x;
	size_t i;
	de_bitmap_t downscaled;
	de_bitmap_t out_bitmap;

	/* Rasterize lines */
	for (i = 0; i < lines.size; ++i)
	{
		de_line_t* scanline = lines.data + i;
		for (x = scanline->begin.x; x <= scanline->end.x; ++x)
		{
			int px = (int)(x + 0.5f);
			de_bitmap_set_pixel(bitmap, px, (int)scanline->begin.y, 255);
		}
	}

	/* Use box filter and downscale raster image by four times */
	downscaled = de_bitmap_downscale4_box_filter(bitmap);

	de_bitmap_create(&out_bitmap, downscaled.width + border, downscaled.height + border);

	/* add border to glyph to remove seams due to bilinear filtration on GPU */
	for (row = half_border; row < out_bitmap.height; ++row)
	{
		for (col = half_border; col < out_bitmap.width; ++col)
		{
			int r = row - half_border;
			int c = col - half_border;
			if (r < downscaled.height && c < downscaled.width)
			{
				out_bitmap.pixels[row * out_bitmap.width + col] = downscaled.pixels[r * downscaled.width + c];
			}
		}
	}

	de_free(downscaled.pixels);

	return out_bitmap;
}