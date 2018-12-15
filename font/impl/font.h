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

#define DE_TTF_DOWNSCALE_FACTOR 4
#define DE_TTF_TEMP_RENDER_OUPUT 0

typedef struct de_ttf_glyph_t
{
	int16_t num_contours;
	int16_t xMin;
	int16_t yMin;
	int16_t xMax;
	int16_t yMax;
	char has_outline;
	uint16_t advance;
	int16_t left_side_bearing;
	uint16_t* end_points;
	de_polygon_t * raw_contours;
	de_polygon_t * contours;
} de_ttf_glyph_t;

enum
{
	ON_CURVE_POINT = 1,
	X_SHORT_VECTOR = 2,
	Y_SHORT_VECTOR = 4,
	REPEAT_FLAG = 8,
	X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 16,
	Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 32
};

typedef struct de_true_type_t
{
	/* memory-mapped font file */
	uint8_t* data;
	uint8_t* cmap_table;
	uint8_t* loca_table;
	uint8_t* maxp_table;
	uint8_t* head_table;
	uint8_t* glyf_table;
	uint8_t* hhea_table;
	uint8_t* hmtx_table;
	uint16_t num_glyphs;
	de_ttf_glyph_t* glyphs;
} de_true_type_t;


/*=======================================================================================*/
static uint16_t ToU16(uint16_t p)
{
	return de_byte_order_swap_u16(p);
}

/*=======================================================================================*/
static uint16_t de_ptr_to_u16(uint8_t *p)
{
	return p[0] * 256 + p[1];
}

/*=======================================================================================*/
static int16_t de_ptr_to_i16(uint8_t *p)
{
	return p[0] * 256 + p[1];
}

/*=======================================================================================*/
static uint32_t de_ptr_to_u32(uint8_t *p)
{
	return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
}

/*=======================================================================================*/
#define DE_FOURCC(d,c,b,a) ((uint32_t) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a)))

static void de_ttf_find_tables(de_true_type_t* ttf)
{
	int i;
	uint16_t num_tables = de_ptr_to_u16(ttf->data + 4);

	for (i = 0; i < num_tables; ++i)
	{
		uint8_t* subtable = ttf->data + 12 + i * 16;
		uint32_t tag = de_ptr_to_u32(subtable + 0);
		uint32_t offset = de_ptr_to_u32(subtable + 8);
		uint8_t* table_location = ttf->data + offset;
		switch (tag)
		{
		case DE_FOURCC('c', 'm', 'a', 'p'): ttf->cmap_table = table_location; break;
		case DE_FOURCC('l', 'o', 'c', 'a'): ttf->loca_table = table_location; break;
		case DE_FOURCC('m', 'a', 'x', 'p'): ttf->maxp_table = table_location; break;
		case DE_FOURCC('h', 'e', 'a', 'd'): ttf->head_table = table_location; break;
		case DE_FOURCC('g', 'l', 'y', 'f'): ttf->glyf_table = table_location; break;
		case DE_FOURCC('h', 'h', 'e', 'a'): ttf->hhea_table = table_location; break;
		case DE_FOURCC('h', 'm', 't', 'x'): ttf->hmtx_table = table_location; break;
		}
	}

	ttf->num_glyphs = de_ptr_to_u16(ttf->maxp_table + 4);
}

/*=======================================================================================*/
static uint32_t de_ttf_segmented_mapping(uint8_t* subtable, uint32_t unicode)
{
	int segment;
	int segment_count = de_ptr_to_u16(subtable + 6) / 2;
	uint16_t* end_codes = (uint16_t*)(subtable + 14);
	uint16_t* start_codes = (uint16_t*)(subtable + 16 + 2 * segment_count);
	uint16_t* id_delta = (uint16_t*)(subtable + 16 + 4 * segment_count);
	uint16_t* id_range_offset = (uint16_t*)(subtable + 16 + 6 * segment_count);

	for (segment = 0; segment < segment_count; ++segment)
	{
		if (ToU16(end_codes[segment]) >= unicode)
		{
			break;
		}
	}

	if (segment != segment_count)
	{
		uint16_t startCode = ToU16(start_codes[segment]);

		if (startCode <= unicode)
		{
			int range_offset = ToU16(id_range_offset[segment]) / 2;

			if (range_offset == 0)
			{
				return (unicode + ToU16(id_delta[segment])) & 0xFFFF;
			}
			else
			{
				return ToU16(*(id_range_offset + segment + range_offset + (unicode - startCode)));
			}
		}
	}

	return 0;
}

/*=======================================================================================*/
static uint32_t de_ttf_direct_mapping(uint8_t* subtable, uint32_t unicode)
{
	if (unicode < 256)
	{
		uint8_t* indices = subtable + 6;
		return indices[unicode];
	}

	return 0;
}

/*=======================================================================================*/
static uint32_t de_ttf_dense_mapping(uint8_t* subtable, uint32_t unicode)
{
	uint16_t first = de_ptr_to_u16(subtable + 6);
	uint16_t entry_count = de_ptr_to_u16(subtable + 8);
	uint16_t* indices = (uint16_t*)(subtable + 10);

	if (unicode > first && unicode < (uint16_t)(first + entry_count))
	{
		return ToU16(indices[unicode - first]);
	}

	return 0;
}

/*=======================================================================================*/
static uint32_t de_ttf_unicode_to_glyph_index(de_true_type_t* ttf, uint32_t unicode)
{
	int i;

	uint16_t subtable_count = de_ptr_to_u16(ttf->cmap_table + 2);

	for (i = 0; i < subtable_count; ++i)
	{
		uint32_t subtable_offset = de_ptr_to_u32(ttf->cmap_table + 4 + 4 * i + 4);
		uint8_t* subtable = ttf->cmap_table + subtable_offset;
		uint16_t format = de_ptr_to_u16(subtable + 0);
		switch (format)
		{
		case 0: return de_ttf_direct_mapping(subtable, unicode);
		case 4: return de_ttf_segmented_mapping(subtable, unicode);
		case 6: return de_ttf_dense_mapping(subtable, unicode);
		}
	}

	return 0;
}

/*=======================================================================================*/
static void de_polygon_add_vertex(de_polygon_t *poly, de_point_t* v)
{
	DE_ARRAY_APPEND(poly->points, *v);
}

/*=======================================================================================*/
uint32_t de_ttf_get_glyph_offset(de_true_type_t* ttf, uint32_t index)
{
	int16_t index_to_loc_format = de_ptr_to_i16(ttf->head_table + 50);

	if (index_to_loc_format & 1)
	{
		return de_ptr_to_u32(ttf->loca_table + 4 * index);
	}
	else
	{
		return de_ptr_to_u16(ttf->loca_table + 2 * index) * 2;
	}
}

/*=======================================================================================*/
static void de_ttf_prepare_contours(de_ttf_glyph_t * glyph, de_point_t* points)
{
	int j;
	size_t k;
	size_t prev_end_pt = 0;
	int16_t glyph_height = glyph->yMax - glyph->yMin;

	/* Extract contours */
	glyph->raw_contours = de_calloc(glyph->num_contours, sizeof(*glyph->raw_contours));

	for (j = 0; j < glyph->num_contours; ++j)
	{
		uint16_t end_pt = glyph->end_points[j];
		de_polygon_t * contour = glyph->raw_contours + j;

		/* Extract vertices */
		for (k = prev_end_pt; k <= end_pt; ++k)
		{
			de_point_t * pt = points + k;

			de_point_t off_pt;
			off_pt.x = pt->x - glyph->xMin;
			off_pt.y = pt->y - glyph->yMin;
			off_pt.flags = pt->flags;

			de_polygon_add_vertex(contour, &off_pt);
		}

		prev_end_pt = end_pt + 1;
	}

	/* Unpack contours */
	for (j = 0; j < glyph->num_contours; ++j)
	{
		de_polygon_t * raw_contour = glyph->raw_contours + j;
		de_polygon_t unpacked_contour = { 0 };

		int start_off = !(raw_contour->points.data[0].flags & ON_CURVE_POINT);

		size_t to;
		if (start_off)
		{
			/* when first point is off-curve we should add middle point between first and last points */
			de_point_t middle;
			de_point_t* first;
			de_point_t* last;

			first = &DE_ARRAY_FIRST(raw_contour->points);
			last = &DE_ARRAY_LAST(raw_contour->points);

			middle.flags = ON_CURVE_POINT;
			middle.x = (first->x + last->x) / 2;
			middle.y = glyph_height - (first->y + last->y) / 2;
			
			de_polygon_add_vertex(&unpacked_contour, &middle);

			/* also make sure to iterate not to the end - we already added point */
			to = raw_contour->points.size - 1;
		}
		else
		{
			to = raw_contour->points.size;
		}

		for (k = 0; k < to; ++k)
		{
			de_point_t flipped;
			de_point_t * p0 = raw_contour->points.data + k;
			de_point_t * p1;
			bool p0_off_curve;
			bool p1_off_curve;

			if (k + 1 < raw_contour->points.size)
			{
				p1 = p0 + 1;
			}
			else
			{
				/* close polygon */
				p1 = raw_contour->points.data;
			}

			p0_off_curve = !(p0->flags & ON_CURVE_POINT);
			p1_off_curve = !(p1->flags & ON_CURVE_POINT);

			flipped.flags = p0->flags;
			flipped.x = p0->x;
			flipped.y = glyph_height - p0->y;
			de_polygon_add_vertex(&unpacked_contour, &flipped);

			if (p0_off_curve && p1_off_curve)
			{
				de_point_t middle;

				middle.flags = ON_CURVE_POINT;

				middle.x = (p0->x + p1->x) / 2;
				middle.y = glyph_height - (p0->y + p1->y) / 2;

				de_polygon_add_vertex(&unpacked_contour, &middle);
			}
		}

		de_free(raw_contour->points.data);
		raw_contour->points = unpacked_contour.points;
		raw_contour->points.size = unpacked_contour.points.size;
	}
}

/*=======================================================================================*/
static void de_ttf_get_glyph_horizontal_metrics(de_true_type_t* ttf, int glyph_index, de_ttf_glyph_t* glyph)
{
	uint16_t numOfLongHorMetrics;

	assert(ttf);

	numOfLongHorMetrics = de_ptr_to_u16(ttf->hhea_table + 34);

	if (glyph_index < numOfLongHorMetrics)
	{
		glyph->advance = de_ptr_to_u16(ttf->hmtx_table + 4 * glyph_index);
		glyph->left_side_bearing = de_ptr_to_i16(ttf->hmtx_table + 4 * glyph_index + 2);
	}
	else
	{
		glyph->advance = de_ptr_to_u16(ttf->hmtx_table + 4 * (numOfLongHorMetrics - 1));
		glyph->left_side_bearing = de_ptr_to_i16(ttf->hmtx_table + 4 * numOfLongHorMetrics + 2 * (glyph_index - numOfLongHorMetrics));
	}
}

/*=======================================================================================*/
void de_ttf_read_glyphs(de_true_type_t * ttf)
{
	int i, j, k;

	/* Allocate glyphs */
	ttf->glyphs = de_calloc(ttf->num_glyphs, sizeof(*ttf->glyphs));
	for (i = 0; i < ttf->num_glyphs; ++i)
	{
		de_ttf_glyph_t * glyph = ttf->glyphs + i;

		uint32_t offset = de_ttf_get_glyph_offset(ttf, i);
		uint32_t next_offset = de_ttf_get_glyph_offset(ttf, i + 1);
		uint8_t* glyph_data = ttf->glyf_table + offset;

		glyph->has_outline = (next_offset - offset) != 0;
		glyph->num_contours = de_ptr_to_i16(glyph_data + 0);
		glyph->xMin = de_ptr_to_i16(glyph_data + 2);
		glyph->yMin = de_ptr_to_i16(glyph_data + 4);
		glyph->xMax = de_ptr_to_i16(glyph_data + 6);
		glyph->yMax = de_ptr_to_i16(glyph_data + 8);

		if (glyph->num_contours < 0)
		{
			/* Compound glyph */

		}
		else
		{
			/* Simple glyph */
			int pointCount = 0;
			de_point_t* points = 0;
			uint16_t instructions;
			uint8_t* coords;
			int x;
			int y;
			uint8_t* flags;

			/* Read end contour points */
			pointCount = 0;
			glyph->end_points = de_malloc(glyph->num_contours * sizeof(*glyph->end_points));
			for (j = 0; j < glyph->num_contours; ++j)
			{
				glyph->end_points[j] = de_ptr_to_u16(glyph_data + 10 + j * 2);
				if (glyph->end_points[j] > pointCount)
				{
					pointCount = glyph->end_points[j];
				}
			}
			++pointCount;

			/* Alloc points */
			points = de_calloc(pointCount, sizeof(*points));

			instructions = de_ptr_to_u16(glyph_data + 10 + 2 * glyph->num_contours);

			/* Read flags for each point */
			flags = glyph_data + 10 + 2 * glyph->num_contours + 2 + instructions;
			for (j = 0; j < pointCount; ++j)
			{
				de_point_t * pt = points + j;
				pt->flags = *flags;
				++flags;
				if (pt->flags & REPEAT_FLAG)
				{
					uint8_t repeat_count = *flags;
					++flags;
					for (k = 1; k <= repeat_count; ++k)
					{
						points[j + k].flags = pt->flags;
					}
					j += repeat_count;
				}
			}

			/* Read x-coordinates for each point */
			coords = flags;
			x = 0;
			for (j = 0; j < pointCount; ++j)
			{
				de_point_t * pt = points + j;
				if (pt->flags & 2)
				{
					int16_t dx = *coords;
					++coords;
					x += (pt->flags & 16) ? dx : -dx;
				}
				else
				{
					if (!(pt->flags & 16))
					{
						x += de_ptr_to_i16(coords);
						coords += 2;
					}
				}
				pt->x = (float)x;
			}

			/* Read y-coordinates for each point */
			y = 0;
			for (j = 0; j < pointCount; ++j)
			{
				de_point_t * pt = points + j;
				if (pt->flags & 4)
				{
					int16_t dy = *coords;
					++coords;
					y += (pt->flags & 32) ? dy : -dy;
				}
				else
				{
					if (!(pt->flags & 32))
					{
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

/*=======================================================================================*/
static float de_em_to_pixels(de_true_type_t* ttf, float pixels)
{
	uint16_t units_per_em = de_ptr_to_u16(ttf->head_table + 18);
	return pixels / units_per_em;
}

/*=======================================================================================*/
static void de_convert_curves_to_line_set(de_ttf_glyph_t * glyph)
{
	int i;
	size_t j;
	size_t k;
	glyph->contours = de_calloc(glyph->num_contours, sizeof(*glyph->contours));

	for (i = 0; i < glyph->num_contours; ++i)
	{
		de_polygon_t * contour = glyph->contours + i;
		de_polygon_t * rawContour = glyph->raw_contours + i;

		/* Extract vertices */
		for (j = 0; j < rawContour->points.size;)
		{
			de_point_t * p0 = rawContour->points.data + j;
			de_point_t * p1 = (j + 1 < rawContour->points.size) ? (p0 + 1) : rawContour->points.data;
			de_point_t * p2 = (j + 2 < rawContour->points.size) ? (p0 + 2) : rawContour->points.data;

			bool p0_on = p0->flags & ON_CURVE_POINT;
			bool p1_on = p1->flags & ON_CURVE_POINT;
			bool p2_on = p2->flags & ON_CURVE_POINT;

			if (p0_on && (!p1_on) && p2_on)
			{
				point_array_t points = de_vg_eval_quad_bezier(p0, p1, p2, 4);
				for (k = 0; k < points.size - 1; ++k)
				{
					de_polygon_add_vertex(contour, points.data + k);
					de_polygon_add_vertex(contour, points.data + k + 1);
				}
				DE_ARRAY_FREE(points);
				j += 2;
			}
			else if (p0_on && p1_on)
			{
				de_polygon_add_vertex(contour, p0);
				de_polygon_add_vertex(contour, p1);
				++j;
			}
			else
			{
				j += 2;

				de_log("Invalid point sequence! Probably a bug in de_ttf_prepare_contours");
			}
		}
	}
}

/*=======================================================================================*/
static void de_ttf_render_glyph(de_true_type_t* ttf, int glyph_index, de_glyph_t* out_glyph, float scaler)
{
	de_bitmap_t bitmap, final_bitmap;
	line_array_t lines;
	de_ttf_glyph_t * ttf_glyph;
	uint16_t height;
	uint16_t width;
	float back_scaler;
	int i;
	int pixel_count;

	ttf_glyph = ttf->glyphs + glyph_index;

	if (ttf_glyph->num_contours < 0)
	{
		return;
	}

	height = (uint16_t)(scaler * (ttf_glyph->yMax - ttf_glyph->yMin)) + 1;
	width = (uint16_t)(scaler * (ttf_glyph->xMax - ttf_glyph->xMin)) + 1;

	de_convert_curves_to_line_set(ttf_glyph);

	lines = de_vg_polys_to_scanlines(
		ttf_glyph->contours,
		ttf_glyph->num_contours,
		(float)(ttf_glyph->xMax - ttf_glyph->xMin),
		(float)(ttf_glyph->yMax - ttf_glyph->yMin),
		scaler
	);


	de_bitmap_create(&bitmap, width, height);
	final_bitmap = de_vg_raster_scanlines(&bitmap, lines);
	de_free(bitmap.pixels);
	DE_ARRAY_FREE(lines);

	back_scaler = scaler / DE_TTF_DOWNSCALE_FACTOR;
	pixel_count = final_bitmap.width * final_bitmap.height;
	out_glyph->pixels = de_malloc(pixel_count * sizeof(*out_glyph->pixels));
	for (i = 0; i < pixel_count; ++i)
	{
		de_color_t * pixel = out_glyph->pixels + i;
		pixel->a = final_bitmap.pixels[i];
		pixel->r = pixel->g = pixel->b = 0;
	}
	out_glyph->bitmap_width = final_bitmap.width;
	out_glyph->bitmap_height = final_bitmap.height;
	out_glyph->advance = ttf_glyph->advance * back_scaler;
	out_glyph->bitmap_left = ttf_glyph->xMin * back_scaler;
	out_glyph->bitmap_top = ttf_glyph->yMin * back_scaler;
	out_glyph->has_outline = ttf_glyph->has_outline;

	de_free(final_bitmap.pixels);
}

/*=======================================================================================*/
void de_ttf_clear(de_true_type_t* ttf)
{
	int i, j;
	for (i = 0; i < ttf->num_glyphs; ++i)
	{
		de_ttf_glyph_t* glyph = ttf->glyphs + i;
		de_free(glyph->end_points);
		for (j = 0; j < glyph->num_contours; ++j)
		{
			DE_ARRAY_FREE(glyph->raw_contours[j].points);
			DE_ARRAY_FREE(glyph->contours[j].points);
		}
		de_free(glyph->contours);
		de_free(glyph->raw_contours);
	}
	de_free(ttf->glyphs);
}


/*=======================================================================================*/
static int de_font_compute_atlas_size(de_font_t * font)
{
	int totalArea = 0;
	int i = 0;

	/* compute total area of all glyphs */
	for (i = 0; i < font->glyph_count; ++i)
	{
		totalArea += font->glyphs[i].bitmap_width * font->glyphs[i].bitmap_height;
	}

	return (int)(sqrt(totalArea) * 1.1f);
}

/*=======================================================================================*/
static int de_font_pack(de_renderer_t* r, de_font_t * font)
{
	int i = 0;
	int row = 0;
	int col = 0;
	int srcRow = 0;
	int srcCol = 0;
	int atlas_size = de_font_compute_atlas_size(font);
	float w = 0, h = 0;
	float x = 0, y = 0;
	int rowEnd = 0;
	int colEnd = 0;
	de_rectpack_node_t * root;

	/* alloc root packer node */
	root = de_rectpack_create_node(0, 0, atlas_size, atlas_size);

	/* prepare atlas */
	font->texture = de_renderer_create_texture(r, atlas_size, atlas_size, 4);
	for (i = 0; i < font->texture->width * font->texture->height; ++i)
	{
		de_rgba8_t* pix = (de_rgba8_t*)(font->texture->pixels) + i;
		pix->r = pix->g = pix->b = 255;
	}

	/* pack each glyph into atlas and copy it's pixels to atlas */
	for (i = 0; i < font->glyph_count; ++i)
	{
		de_glyph_t * glyph = font->glyphs + i;
		de_rectpack_node_t * node = de_rectpack_get_free(root, glyph->bitmap_width, glyph->bitmap_height, glyph);

		if (!node)
		{
			de_error("not enough space in font atlas");
		}

		w = node->w / (float)atlas_size;
		h = node->h / (float)atlas_size;
		x = node->x / (float)atlas_size;
		y = node->y / (float)atlas_size;

		glyph->texCoords[0].x = x;
		glyph->texCoords[0].y = y;

		glyph->texCoords[1].x = x + w;
		glyph->texCoords[1].y = y;

		glyph->texCoords[2].x = x + w;
		glyph->texCoords[2].y = y + h;

		glyph->texCoords[3].x = x;
		glyph->texCoords[3].y = y + h;

		rowEnd = node->y + node->h;
		colEnd = node->x + node->w;

		/* copy glyph pixels to atlas pixels */
		for (row = node->y, srcRow = 0; row < rowEnd; row++, srcRow++)
		{
			for (col = node->x, srcCol = 0; col < colEnd; col++, srcCol++)
			{
				de_rgba8_t* pix = (de_rgba8_t*)(font->texture->pixels) + row * atlas_size + col;
				pix->a = glyph->pixels[srcRow * node->w + srcCol].a;
			}
		}
	}

	de_rectpack_free(root);

	/* free pixels for each glyph */
	for (i = 0; i < font->glyph_count; ++i)
	{
		de_free(font->glyphs[i].pixels);
		font->glyphs[i].pixels = NULL;
	}

	return 1;
}

/*=======================================================================================*/
static int de_compare_charmap_entry(const void* a, const void* b)
{
	const de_font_charmap_entry_t* entry_a = a;
	const de_font_charmap_entry_t* entry_b = b;
	if (entry_a->unicode < entry_b->unicode)
	{
		return -1;
	}
	if (entry_a->unicode == entry_b->unicode)
	{
		return 0;
	}
	return 1;
}

/*=======================================================================================*/
de_font_t* de_font_load_ttf_from_memory(de_core_t* core, void* data, float height, const int* char_set, int char_count)
{
	int i;
	de_true_type_t ttf;
	float oversample_height;
	float scaler;
	float back_scaler;
	de_font_t * font;

	assert(data);
	assert(char_count);
	assert(char_count > 0);
	assert(char_set);

	ttf.data = data;

	de_ttf_find_tables(&ttf);
	de_ttf_read_glyphs(&ttf);

	oversample_height = height * DE_TTF_DOWNSCALE_FACTOR;
	scaler = de_em_to_pixels(&ttf, oversample_height);
	back_scaler = scaler / DE_TTF_DOWNSCALE_FACTOR;

	font = DE_NEW(de_font_t);
	font->core = core;

	font->ascender = back_scaler * de_ptr_to_i16(ttf.hhea_table + 4);
	font->descender = back_scaler * de_ptr_to_i16(ttf.hhea_table + 6);
	font->line_gap = back_scaler * de_ptr_to_i16(ttf.hhea_table + 8);

	font->glyph_count = ttf.num_glyphs;
	font->glyphs = de_calloc(ttf.num_glyphs, sizeof(de_glyph_t));

	for (i = 0; i < font->glyph_count; ++i)
	{
		de_ttf_render_glyph(&ttf, i, font->glyphs + i, scaler);
	}

	/* fill charmap */
	for (i = 0; i < char_count; ++i)
	{
		de_font_charmap_entry_t entry;
		entry.unicode = char_set[i];
		entry.glyph_index = de_ttf_unicode_to_glyph_index(&ttf, entry.unicode);
		DE_ARRAY_APPEND(font->charmap, entry);
	}
	/* sort charmap for fast binary search */
	DE_ARRAY_QSORT(font->charmap, de_compare_charmap_entry);

	de_font_pack(core->renderer, font);
	de_ttf_clear(&ttf);

	DE_LINKED_LIST_APPEND(core->fonts, font);

	return font;
}

/*=======================================================================================*/
de_font_t* de_font_load_ttf(de_core_t* core, const char * filename, float height, const int* char_set, int char_count)
{
	int size;
	FILE* fontFile;
	void* data;
	de_font_t* font;

	assert(filename);
	assert(char_count);
	assert(char_count > 0);
	assert(char_set);

	fontFile = fopen(filename, "rb");
	if (!fontFile)
	{
		return NULL;
	}

	fseek(fontFile, 0, SEEK_END);
	size = ftell(fontFile);
	data = de_malloc(size);
	fseek(fontFile, 0, SEEK_SET);
	fread(data, 1, size, fontFile);
	fclose(fontFile);

	font = de_font_load_ttf_from_memory(core, data, height, char_set, char_count);

	de_free(data);
	
	return font;
}

/*=======================================================================================*/
void de_font_free(de_font_t* font)
{
	DE_LINKED_LIST_REMOVE(font->core->fonts, font);

	DE_ARRAY_FREE(font->charmap);
	de_texture_release(font->texture);
	de_free(font->glyphs);
	de_free(font);
}

/*=======================================================================================*/
static int de_charmap_search_compare(const void* unicode_ptr, const void* elem)
{
	const de_font_charmap_entry_t* entry = elem;
	uint32_t unicode = *((uint32_t*)unicode_ptr);
	if (unicode < entry->unicode)
	{
		return -1;
	}
	if (unicode == entry->unicode)
	{
		return 0;
	}
	return 1;
}

/*=======================================================================================*/
de_glyph_t* de_font_get_glyph(const de_font_t* font, uint32_t code)
{
	de_font_charmap_entry_t* entry = DE_ARRAY_BSEARCH(font->charmap, &code, de_charmap_search_compare);
	if (entry)
	{
		return font->glyphs + entry->glyph_index;
	}
	return NULL;
}