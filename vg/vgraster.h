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

typedef struct de_point_t {
	float x;
	float y;
	uint8_t flags;
} de_point_t;

typedef struct de_line2_t {
	de_point_t begin;
	de_point_t end;
} de_line2_t;

typedef DE_ARRAY_DECLARE(de_line2_t, line_array_t);
typedef DE_ARRAY_DECLARE(de_point_t, point_array_t);

typedef struct de_bitmap_t {
	uint8_t* pixels;
	int width;
	int height;
} de_bitmap_t;

typedef struct de_polygon_t {
	DE_ARRAY_DECLARE(de_point_t, points);
} de_polygon_t;

void de_bitmap_create(de_bitmap_t * bitmap, int width, int height);
void de_bitmap_set_pixel(de_bitmap_t* bitmap, int x, int y, unsigned char pixel);
point_array_t de_vg_eval_quad_bezier(const de_point_t * p0, const de_point_t * p1, const de_point_t * p2, int steps);
bool de_vg_line_line_intersection(const de_line2_t* a, const de_line2_t* b, de_point_t *out);
void de_vg_polys_to_scanlines(de_polygon_t* polys, size_t poly_count, float width, float height, float scale, line_array_t* out);
de_bitmap_t de_bitmap_downscale4_box_filter(de_bitmap_t *src);
de_bitmap_t de_vg_raster_scanlines(de_bitmap_t* bitmap, line_array_t lines);