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

/* Loader implemented using Apple TrueType Reference Manual:
 * https://developer.apple.com/fonts/TrueType-Reference-Manual */

typedef struct de_ttf_glyph_t {
	int16_t num_contours;
	int16_t x_min;
	int16_t y_min;
	int16_t x_max;
	int16_t y_max;
	char has_outline;
	uint16_t advance;
	int16_t left_side_bearing;
	uint16_t* end_points;
	de_polygon_t * raw_contours;
	de_polygon_t * contours;
} de_ttf_glyph_t;

enum {
	ON_CURVE_POINT = 1,
	X_SHORT_VECTOR = 2,
	Y_SHORT_VECTOR = 4,
	REPEAT_FLAG = 8,
	X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 16,
	Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 32
};

typedef struct de_true_type_t {
	/* memory-mapped font file */
	uint8_t* data;
	uint8_t* cmap_table;
	uint8_t* loca_table;
	uint8_t* maxp_table;
	uint8_t* head_table;
	uint8_t* glyf_table;
	uint8_t* hhea_table;
	uint8_t* hmtx_table;
	uint8_t* kern_table;
	uint16_t num_glyphs;
	de_ttf_glyph_t* glyphs;
} de_true_type_t;

static uint16_t de_ptr_to_u16(const uint8_t *p)
{
	return p[0] * 256 + p[1];
}

static int16_t de_ptr_to_i16(const uint8_t *p)
{
	return p[0] * 256 + p[1];
}

static uint32_t de_ptr_to_u32(const uint8_t *p)
{
	return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
}

#define DE_FOURCC(d,c,b,a) ((uint32_t) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a)))

static void de_ttf_find_tables(de_true_type_t* ttf)
{
	const uint16_t num_tables = de_ptr_to_u16(ttf->data + 4);

	for (int i = 0; i < num_tables; ++i) {
		const uint8_t* subtable = ttf->data + 12 + i * 16;
		const uint32_t tag = de_ptr_to_u32(subtable + 0);
		const uint32_t offset = de_ptr_to_u32(subtable + 8);
		uint8_t* table_location = ttf->data + offset;
		switch (tag) {
			case DE_FOURCC('c', 'm', 'a', 'p'): ttf->cmap_table = table_location; break;
			case DE_FOURCC('l', 'o', 'c', 'a'): ttf->loca_table = table_location; break;
			case DE_FOURCC('m', 'a', 'x', 'p'): ttf->maxp_table = table_location; break;
			case DE_FOURCC('h', 'e', 'a', 'd'): ttf->head_table = table_location; break;
			case DE_FOURCC('g', 'l', 'y', 'f'): ttf->glyf_table = table_location; break;
			case DE_FOURCC('h', 'h', 'e', 'a'): ttf->hhea_table = table_location; break;
			case DE_FOURCC('h', 'm', 't', 'x'): ttf->hmtx_table = table_location; break;
			case DE_FOURCC('k', 'e', 'r', 'n'): ttf->kern_table = table_location; break;
		}
	}

	ttf->num_glyphs = de_ptr_to_u16(ttf->maxp_table + 4);
}

static uint32_t de_ttf_segmented_mapping(const uint8_t* subtable, uint32_t unicode)
{
	const int segment_count = de_ptr_to_u16(subtable + 6) / 2;
	const uint16_t* end_codes = (uint16_t*)(subtable + 14);
	const uint16_t* start_codes = (uint16_t*)(subtable + 16 + 2 * segment_count);
	const uint16_t* id_delta = (uint16_t*)(subtable + 16 + 4 * segment_count);
	const uint16_t* id_range_offset = (uint16_t*)(subtable + 16 + 6 * segment_count);

	int segment;
	for (segment = 0; segment < segment_count; ++segment) {
		if (de_byte_order_swap_u16(end_codes[segment]) >= unicode) {
			break;
		}
	}

	if (segment != segment_count) {
		const uint16_t startCode = de_byte_order_swap_u16(start_codes[segment]);

		if (startCode <= unicode) {
			const int range_offset = de_byte_order_swap_u16(id_range_offset[segment]) / 2;

			if (range_offset == 0) {
				return (unicode + de_byte_order_swap_u16(id_delta[segment])) & 0xFFFF;
			} else {
				return de_byte_order_swap_u16(*(id_range_offset + segment + range_offset + (unicode - startCode)));
			}
		}
	}

	return 0;
}

static uint32_t de_ttf_direct_mapping(const uint8_t* subtable, uint32_t unicode)
{
	if (unicode < 256) {
		const uint8_t* indices = subtable + 6;
		return indices[unicode];
	}

	return 0;
}

static uint32_t de_ttf_dense_mapping(const uint8_t* subtable, uint32_t unicode)
{
	const uint16_t first = de_ptr_to_u16(subtable + 6);
	const uint16_t entry_count = de_ptr_to_u16(subtable + 8);
	const uint16_t* indices = (uint16_t*)(subtable + 10);

	if (unicode > first && unicode < (uint16_t)(first + entry_count)) {
		return de_byte_order_swap_u16(indices[unicode - first]);
	}

	return 0;
}

static uint32_t de_ttf_unicode_to_glyph_index(const de_true_type_t* ttf, uint32_t unicode)
{
	const uint16_t subtable_count = de_ptr_to_u16(ttf->cmap_table + 2);

	for (int i = 0; i < subtable_count; ++i) {
		const uint32_t subtable_offset = de_ptr_to_u32(ttf->cmap_table + 4 + 4 * i + 4);
		const uint8_t* subtable = ttf->cmap_table + subtable_offset;
		const uint16_t format = de_ptr_to_u16(subtable + 0);
		switch (format) {
			case 0: return de_ttf_direct_mapping(subtable, unicode);
			case 4: return de_ttf_segmented_mapping(subtable, unicode);
			case 6: return de_ttf_dense_mapping(subtable, unicode);
				/* todo: add more mappings */
		}
	}

	return 0;
}

static void de_polygon_add_vertex(de_polygon_t* poly, const de_point_t* v)
{
	DE_ARRAY_APPEND(poly->points, *v);
}

uint32_t de_ttf_get_glyph_offset(de_true_type_t* ttf, uint32_t index)
{
	const int16_t index_to_loc_format = de_ptr_to_i16(ttf->head_table + 50);

	if (index_to_loc_format & 1) {
		return de_ptr_to_u32(ttf->loca_table + 4 * index);
	} else {
		return de_ptr_to_u16(ttf->loca_table + 2 * index) * 2;
	}
}

static void de_ttf_prepare_contours(de_ttf_glyph_t * glyph, de_point_t* points)
{
	const int16_t glyph_height = glyph->y_max - glyph->y_min;

	/* Extract contours */
	glyph->raw_contours = de_calloc(glyph->num_contours, sizeof(*glyph->raw_contours));

	size_t prev_end_pt = 0;
	for (int j = 0; j < glyph->num_contours; ++j) {
		const uint16_t end_pt = glyph->end_points[j];
		de_polygon_t* contour = glyph->raw_contours + j;

		/* Extract vertices */
		for (size_t k = prev_end_pt; k <= end_pt; ++k) {
			const de_point_t * pt = points + k;

			const de_point_t off_pt = {
				.x = pt->x - glyph->x_min,
				.y = pt->y - glyph->y_min,
				.flags = pt->flags,
			};

			de_polygon_add_vertex(contour, &off_pt);
		}

		prev_end_pt = end_pt + 1;
	}

	/* Unpack contours */
	for (int j = 0; j < glyph->num_contours; ++j) {
		de_polygon_t* raw_contour = glyph->raw_contours + j;
		de_polygon_t unpacked_contour = { { 0 } };

		const int start_off = !(raw_contour->points.data[0].flags & ON_CURVE_POINT);

		size_t to;
		if (start_off) {
			/* when first point is off-curve we should add middle point between first and last points */
			const de_point_t* first = &DE_ARRAY_FIRST(raw_contour->points);
			const de_point_t* last = &DE_ARRAY_LAST(raw_contour->points);

			const de_point_t middle = {
				.flags = ON_CURVE_POINT,
				.x = (first->x + last->x) / 2,
				.y = glyph_height - (first->y + last->y) / 2
			};

			de_polygon_add_vertex(&unpacked_contour, &middle);

			/* also make sure to iterate not to the end - we already added point */
			to = raw_contour->points.size - 1;
		} else {
			to = raw_contour->points.size;
		}

		for (size_t k = 0; k < to; ++k) {
			const de_point_t * p0 = raw_contour->points.data + k;
			const de_point_t * p1 = (k + 1 < raw_contour->points.size) ? p0 + 1 : raw_contour->points.data;

			const bool p0_off_curve = !(p0->flags & ON_CURVE_POINT);
			const bool p1_off_curve = !(p1->flags & ON_CURVE_POINT);

			const de_point_t flipped = {
				.flags = p0->flags,
				.x = p0->x,
				.y = glyph_height - p0->y
			};
			de_polygon_add_vertex(&unpacked_contour, &flipped);

			if (p0_off_curve && p1_off_curve) {
				const de_point_t middle = {
					.flags = ON_CURVE_POINT,
					.x = (p0->x + p1->x) / 2,
					.y = glyph_height - (p0->y + p1->y) / 2
				};
				de_polygon_add_vertex(&unpacked_contour, &middle);
			}
		}

		de_free(raw_contour->points.data);
		raw_contour->points = unpacked_contour.points;
		raw_contour->points.size = unpacked_contour.points.size;
	}
}

static void de_ttf_get_glyph_horizontal_metrics(de_true_type_t* ttf, int glyph_index, de_ttf_glyph_t* glyph)
{
	const uint16_t numOfLongHorMetrics = de_ptr_to_u16(ttf->hhea_table + 34);
	if (glyph_index < numOfLongHorMetrics) {
		glyph->advance = de_ptr_to_u16(ttf->hmtx_table + 4 * glyph_index);
		glyph->left_side_bearing = de_ptr_to_i16(ttf->hmtx_table + 4 * glyph_index + 2);
	} else {
		glyph->advance = de_ptr_to_u16(ttf->hmtx_table + 4 * (numOfLongHorMetrics - 1));
		glyph->left_side_bearing = de_ptr_to_i16(ttf->hmtx_table + 4 * numOfLongHorMetrics + 2 * (glyph_index - numOfLongHorMetrics));
	}
}

void de_ttf_read_kern(de_true_type_t* ttf)
{
	if(!ttf->kern_table) {
		return;
	}
	const uint16_t version = de_ptr_to_u16(ttf->kern_table + 0);
	const uint16_t num_tables = de_ptr_to_u16(ttf->kern_table + 2);
	for (uint32_t i = 0; i < num_tables; ++i) {
		uint8_t* sub_table = ttf->kern_table + 8;
		const uint32_t length = de_ptr_to_u32(sub_table);
		const uint16_t coverage = de_ptr_to_u16(sub_table + 4);
		const uint16_t tuple_index = de_ptr_to_u16(sub_table + 2);

		/* TODO: Implement. */

		DE_UNUSED(version);
		DE_UNUSED(length);
		DE_UNUSED(coverage);
		DE_UNUSED(tuple_index);		
	}
}

void de_ttf_read_glyphs(de_true_type_t* ttf)
{
	/* Allocate glyphs */
	ttf->glyphs = de_calloc(ttf->num_glyphs, sizeof(*ttf->glyphs));
	for (int i = 0; i < ttf->num_glyphs; ++i) {
		de_ttf_glyph_t * glyph = ttf->glyphs + i;

		const uint32_t offset = de_ttf_get_glyph_offset(ttf, i);
		const uint32_t next_offset = de_ttf_get_glyph_offset(ttf, i + 1);
		const uint8_t* glyph_data = ttf->glyf_table + offset;

		glyph->has_outline = (next_offset - offset) != 0;
		glyph->num_contours = de_ptr_to_i16(glyph_data + 0);
		glyph->x_min = de_ptr_to_i16(glyph_data + 2);
		glyph->y_min = de_ptr_to_i16(glyph_data + 4);
		glyph->x_max = de_ptr_to_i16(glyph_data + 6);
		glyph->y_max = de_ptr_to_i16(glyph_data + 8);

		if (glyph->num_contours < 0) {
			/* TODO: Implement compound glyph support. */
			const uint16_t flags = de_ptr_to_u16(glyph_data + 10);
			const uint16_t glyph_index = de_ptr_to_u16(glyph_data + 12);

			DE_UNUSED(flags);
			DE_UNUSED(glyph_index);

			enum de_compound_glyph_flags {
				ARG_1_AND_2_ARE_WORDS = DE_BIT(0),
				ARGS_ARE_XY_VALUES = DE_BIT(1),
				ROUND_XY_TO_GRID = DE_BIT(2),
				WE_HAVE_A_SCALE = DE_BIT(3),
				/* 4th bit is obsolete */
				MORE_COMPONENTS = DE_BIT(5),
				WE_HAVE_AN_X_AND_Y_SCALE = DE_BIT(6),
				WE_HAVE_A_TWO_BY_TWO = DE_BIT(7),
				WE_HAVE_INSTRUCTIONS = DE_BIT(8),
				USE_MY_METRICS = DE_BIT(9),
				OVERLAP_COMPOUND = DE_BIT(10)
			};
		} else { 
			/* Simple glyph */
			/* Read end contour points */
			int point_count = 0;
			glyph->end_points = de_malloc(glyph->num_contours * sizeof(*glyph->end_points));
			for (int j = 0; j < glyph->num_contours; ++j) {
				glyph->end_points[j] = de_ptr_to_u16(glyph_data + 10 + j * 2);
				if (glyph->end_points[j] > point_count) {
					point_count = glyph->end_points[j];
				}
			}
			++point_count;

			/* Alloc points */
			de_point_t* points = de_calloc(point_count, sizeof(*points));

			/* TODO: Skip instructions for now. Simple interpreter would be nice. */
			const uint16_t instructions = de_ptr_to_u16(glyph_data + 10 + 2 * glyph->num_contours);
			
			/* Read flags for each point */
			const uint8_t* flags = glyph_data + 10 + 2 * glyph->num_contours + 2 + instructions;
			for (int j = 0; j < point_count; ++j) {
				de_point_t* pt = points + j;
				pt->flags = *flags;
				++flags;
				if (pt->flags & REPEAT_FLAG) {
					uint8_t repeat_count = *flags;
					++flags;
					for (int k = 1; k <= repeat_count; ++k) {
						points[j + k].flags = pt->flags;
					}
					j += repeat_count;
				}
			}

			/* Read x-coordinates for each point */
			const uint8_t* coords = flags;
			int x = 0;
			for (int j = 0; j < point_count; ++j) {
				de_point_t * pt = points + j;
				if (pt->flags & 2) {
					const int16_t dx = *coords;
					++coords;
					x += (pt->flags & 16) ? dx : -dx;
				} else {
					if (!(pt->flags & 16)) {
						x += de_ptr_to_i16(coords);
						coords += 2;
					}
				}
				pt->x = (float)x;
			}

			/* Read y-coordinates for each point */
			int y = 0;
			for (int j = 0; j < point_count; ++j) {
				de_point_t * pt = points + j;
				if (pt->flags & 4) {
					const int16_t dy = *coords;
					++coords;
					y += (pt->flags & 32) ? dy : -dy;
				} else {
					if (!(pt->flags & 32)) {
						y += de_ptr_to_i16(coords);
						coords += 2;
					}
				}
				pt->y = (float)y;
			}
			de_ttf_prepare_contours(glyph, points);

			de_free(points);
		}

		de_ttf_get_glyph_horizontal_metrics(ttf, i, glyph);
	}
}

static float de_em_to_pixels(de_true_type_t* ttf, float pixels)
{
	const uint16_t units_per_em = de_ptr_to_u16(ttf->head_table + 18);
	return pixels / units_per_em;
}

static void de_convert_curves_to_line_set(de_ttf_glyph_t * glyph)
{
	glyph->contours = de_calloc(glyph->num_contours, sizeof(*glyph->contours));

	for (int i = 0; i < glyph->num_contours; ++i) {
		de_polygon_t* contour = glyph->contours + i;
		const de_polygon_t* raw_contour = glyph->raw_contours + i;

		/* Extract vertices */
		for (size_t j = 0; j < raw_contour->points.size;) {
			const de_point_t* p0 = raw_contour->points.data + j;
			const de_point_t* p1 = (j + 1 < raw_contour->points.size) ? (p0 + 1) : raw_contour->points.data;
			const de_point_t* p2 = (j + 2 < raw_contour->points.size) ? (p0 + 2) : raw_contour->points.data;

			const bool p0_on = p0->flags & ON_CURVE_POINT;
			const bool p1_on = p1->flags & ON_CURVE_POINT;
			const bool p2_on = p2->flags & ON_CURVE_POINT;

			if (p0_on && (!p1_on) && p2_on) {
				point_array_t points = de_vg_eval_quad_bezier(p0, p1, p2, 6);
				for (size_t k = 0; k < points.size - 1; ++k) {
					de_polygon_add_vertex(contour, points.data + k);
					de_polygon_add_vertex(contour, points.data + k + 1);
				}
				DE_ARRAY_FREE(points);
				j += 2;
			} else if (p0_on && p1_on) {
				de_polygon_add_vertex(contour, p0);
				de_polygon_add_vertex(contour, p1);
				++j;
			} else {
				j += 2;

				de_log("Invalid point sequence! Probably a bug in de_ttf_prepare_contours");
			}
		}
	}
}

static void de_ttf_render_glyph(de_true_type_t* ttf, int glyph_index, de_glyph_t* out_glyph, float scale)
{
	de_ttf_glyph_t* ttf_glyph = ttf->glyphs + glyph_index;

	if (ttf_glyph->num_contours < 0) {
		return;
	}

	const uint16_t height = (uint16_t)(scale * (ttf_glyph->y_max - ttf_glyph->y_min)) + 1;
	const uint16_t width = (uint16_t)(scale * (ttf_glyph->x_max - ttf_glyph->x_min)) + 1;

	de_convert_curves_to_line_set(ttf_glyph);

	line_array_t lines;
	DE_ARRAY_INIT(lines);

	de_vg_polys_to_scanlines(
		ttf_glyph->contours,
		ttf_glyph->num_contours,
		(float)(ttf_glyph->x_max - ttf_glyph->x_min),
		(float)(ttf_glyph->y_max - ttf_glyph->y_min),
		scale, &lines
	);

	de_bitmap_t bitmap;
	de_bitmap_create(&bitmap, width, height);
	de_bitmap_t final_bitmap = de_vg_raster_scanlines(&bitmap, lines);
	de_free(bitmap.pixels);
	DE_ARRAY_FREE(lines);

	const int pixel_count = final_bitmap.width * final_bitmap.height;
	out_glyph->pixels = de_malloc(pixel_count * sizeof(*out_glyph->pixels));
	for (int i = 0; i < pixel_count; ++i) {
		*(out_glyph->pixels + i) = final_bitmap.pixels[i];
	}
	out_glyph->bitmap_width = final_bitmap.width;
	out_glyph->bitmap_height = final_bitmap.height;
	out_glyph->advance = ttf_glyph->advance * scale;
	out_glyph->bitmap_left = ttf_glyph->x_min * scale;
	out_glyph->bitmap_top = ttf_glyph->y_min * scale;
	out_glyph->has_outline = ttf_glyph->has_outline;

	de_free(final_bitmap.pixels);
}

void de_ttf_clear(de_true_type_t* ttf)
{
	for (int i = 0; i < ttf->num_glyphs; ++i) {
		de_ttf_glyph_t* glyph = ttf->glyphs + i;
		de_free(glyph->end_points);
		for (int j = 0; j < glyph->num_contours; ++j) {
			DE_ARRAY_FREE(glyph->raw_contours[j].points);
			DE_ARRAY_FREE(glyph->contours[j].points);
		}
		de_free(glyph->contours);
		de_free(glyph->raw_contours);
	}
	de_free(ttf->glyphs);
}

static int de_font_compute_atlas_size(de_font_t * font)
{
	int totalArea = 0;
	/* compute total area of all glyphs */
	for (int i = 0; i < font->glyph_count; ++i) {
		totalArea += font->glyphs[i].bitmap_width * font->glyphs[i].bitmap_height;
	}

	return (int)(sqrt(totalArea) * 1.1f);
}

static bool de_font_pack(de_renderer_t* r, de_font_t * font)
{
	/* alloc root packer node */
	const int atlas_size = de_font_compute_atlas_size(font);
	de_rectpack_node_t * root = de_rectpack_create_node(0, 0, atlas_size, atlas_size);

	/* prepare atlas */
	de_resource_t* tex_resource = de_resource_create(r->core, NULL, DE_RESOURCE_TYPE_TEXTURE);
	de_resource_set_flags(tex_resource, DE_RESOURCE_FLAG_INTERNAL);
	de_resource_add_ref(tex_resource);
	font->texture = de_resource_to_texture(tex_resource);
	de_texture_alloc_pixels(font->texture, atlas_size, atlas_size, 4);
	for (int i = 0; i < font->texture->width * font->texture->height; ++i) {
		de_rgba8_t* pix = (de_rgba8_t*)(font->texture->pixels) + i;
		pix->r = pix->g = pix->b = 255;
	}

	/* pack each glyph into atlas and copy it's pixels to atlas */
	for (int i = 0; i < font->glyph_count; ++i) {
		de_glyph_t* glyph = font->glyphs + i;
		de_rectpack_node_t* node = de_rectpack_get_free(root, glyph->bitmap_width, glyph->bitmap_height, glyph);

		if (!node) {
			de_log("not enough space in font atlas");
			return false;
		}

		const float w = node->w / (float)atlas_size;
		const float h = node->h / (float)atlas_size;
		const float x = node->x / (float)atlas_size;
		const float y = node->y / (float)atlas_size;

		glyph->texCoords[0].x = x;
		glyph->texCoords[0].y = y;

		glyph->texCoords[1].x = x + w;
		glyph->texCoords[1].y = y;

		glyph->texCoords[2].x = x + w;
		glyph->texCoords[2].y = y + h;

		glyph->texCoords[3].x = x;
		glyph->texCoords[3].y = y + h;

		const int rowEnd = node->y + node->h;
		const int colEnd = node->x + node->w;

		/* copy glyph pixels to atlas pixels */
		for (int row = node->y, srcRow = 0; row < rowEnd; row++, srcRow++) {
			for (int col = node->x, srcCol = 0; col < colEnd; col++, srcCol++) {
				de_rgba8_t* pix = (de_rgba8_t*)(font->texture->pixels) + row * atlas_size + col;
				pix->a = glyph->pixels[srcRow * node->w + srcCol];
			}
		}
	}

	de_rectpack_free(root);

	/* free pixels for each glyph */
	for (int i = 0; i < font->glyph_count; ++i) {
		de_free(font->glyphs[i].pixels);
		font->glyphs[i].pixels = NULL;
	}

	return true;
}

static int de_compare_charmap_entry(const void* a, const void* b)
{
	const de_font_charmap_entry_t* entry_a = (const de_font_charmap_entry_t*)a;
	const de_font_charmap_entry_t* entry_b = (const de_font_charmap_entry_t*)b;
	if (entry_a->unicode < entry_b->unicode) {
		return -1;
	}
	if (entry_a->unicode == entry_b->unicode) {
		return 0;
	}
	return 1;
}

de_font_t* de_font_load_ttf_from_memory(de_core_t* core, void* data, float height, const int* char_set, int char_count)
{
	DE_ASSERT(data);
	DE_ASSERT(char_count);
	DE_ASSERT(char_count > 0);
	DE_ASSERT(char_set);

	de_true_type_t ttf = { 0 };
	ttf.data = (uint8_t*)data;

	de_ttf_find_tables(&ttf);
	de_ttf_read_glyphs(&ttf);
	de_ttf_read_kern(&ttf);
			
	de_font_t* font = DE_NEW(de_font_t);
	font->core = core;
	font->height = height;

	const float scale = de_em_to_pixels(&ttf, height);
	font->ascender = scale * de_ptr_to_i16(ttf.hhea_table + 4);
	font->descender = scale * de_ptr_to_i16(ttf.hhea_table + 6);
	font->line_gap = scale * de_ptr_to_i16(ttf.hhea_table + 8);

	font->glyph_count = ttf.num_glyphs;
	font->glyphs = (de_glyph_t*)de_calloc(ttf.num_glyphs, sizeof(de_glyph_t));

	for (int i = 0; i < font->glyph_count; ++i) {
		de_ttf_render_glyph(&ttf, i, font->glyphs + i, scale);
	}

	/* fill charmap */
	DE_ARRAY_RESERVE(font->charmap, char_count);
	for (int i = 0; i < char_count; ++i) {
		const de_font_charmap_entry_t entry = {
			.unicode = char_set[i],
			.glyph_index = de_ttf_unicode_to_glyph_index(&ttf, entry.unicode)
		};
		DE_ARRAY_APPEND(font->charmap, entry);
	}
	/* sort charmap for fast binary search */
	DE_ARRAY_QSORT(font->charmap, de_compare_charmap_entry);

	de_font_pack(core->renderer, font);
	de_ttf_clear(&ttf);

	DE_LINKED_LIST_APPEND(core->fonts, font);

	return font;
}

de_font_t* de_font_load_ttf(de_core_t* core, const char * filename, float height, const int* char_set, int char_count)
{
	DE_ASSERT(filename);
	DE_ASSERT(char_count);
	DE_ASSERT(char_count > 0);
	DE_ASSERT(char_set);

	size_t size;
	void* data = de_load_file_into_memory(filename, &size);
	if (!data) {
		return NULL;
	}
	de_font_t* font = de_font_load_ttf_from_memory(core, data, height, char_set, char_count);
	de_free(data);
	return font;
}

void de_font_free(de_font_t* font)
{
	DE_LINKED_LIST_REMOVE(font->core->fonts, font);
	DE_ARRAY_FREE(font->charmap);
	de_resource_release(de_resource_from_texture(font->texture));
	de_free(font->glyphs);
	de_free(font);
}

static int de_charmap_search_compare(const void* unicode_ptr, const void* elem)
{
	const de_font_charmap_entry_t* entry = (const de_font_charmap_entry_t*)elem;
	uint32_t unicode = *((uint32_t*)unicode_ptr);
	if (unicode < entry->unicode) {
		return -1;
	}
	if (unicode == entry->unicode) {
		return 0;
	}
	return 1;
}

de_glyph_t* de_font_get_glyph(const de_font_t* font, uint32_t code)
{
	de_font_charmap_entry_t* entry = (de_font_charmap_entry_t*)DE_ARRAY_BSEARCH(font->charmap, &code, de_charmap_search_compare);
	if (entry) {
		return font->glyphs + entry->glyph_index;
	}
	return NULL;
}