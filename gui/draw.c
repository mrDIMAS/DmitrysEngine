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

static void de_gui_get_line_thickness_vector(const de_vec2_t* a, const de_vec2_t* b, float thickness, de_vec2_t* thickness_vec)
{
	de_vec2_t dir;
	de_vec2_sub(&dir, b, a);
	de_vec2_normalize(&dir, &dir);
	de_vec2_perp(thickness_vec, &dir);
	de_vec2_scale(thickness_vec, thickness_vec, thickness * 0.5f);
}


void de_gui_draw_list_push_line(de_gui_draw_list_t* draw_list, const de_vec2_t* a, const de_vec2_t* b, float thickness, const de_color_t* clr)
{
	de_vec2_t perp;
	int index = de_gui_draw_list_get_next_index_origin(draw_list);
	de_gui_vertex_t* v = de_gui_draw_list_reserve_vertices(draw_list, 4);

	de_gui_get_line_thickness_vector(a, b, thickness, &perp);

	/* vertices */
	de_vec2_sub(&v->pos, a, &perp);
	v->color = *clr;
	de_vec2_set(&v->tex_coord, 0, 0);

	++v;
	de_vec2_sub(&v->pos, b, &perp);
	v->color = *clr;
	de_vec2_set(&v->tex_coord, 1, 0);

	++v;
	de_vec2_add(&v->pos, b, &perp);
	v->color = *clr;
	de_vec2_set(&v->tex_coord, 1, 1);

	++v;
	de_vec2_add(&v->pos, a, &perp);
	v->color = *clr;
	de_vec2_set(&v->tex_coord, 0, 1);

	/* indices */
	de_gui_draw_list_ib_push_triangle(draw_list, index, index + 1, index + 2);
	de_gui_draw_list_ib_push_triangle(draw_list, index, index + 2, index + 3);
}


void de_gui_polyline_init(de_gui_polyline_t* polyline)
{
	DE_ARRAY_INIT(polyline->points);
	polyline->thickness = 1.0f;
}


void de_gui_polyline_draw(de_gui_polyline_t* polyline, de_gui_draw_list_t* draw_list)
{
	size_t i;
	/* draw line bodies first */
	for (i = 0; i < polyline->points.size - 1; ++i) {
		int index = de_gui_draw_list_get_next_index_origin(draw_list);
		de_gui_vertex_t* v = de_gui_draw_list_reserve_vertices(draw_list, 4);
		de_gui_line_point_t* a = polyline->points.data + i;
		de_gui_line_point_t* b = a + 1;
		de_vec2_t perp;
		de_gui_get_line_thickness_vector(&b->pos, &a->pos, polyline->thickness, &perp);

		/* line vertices */
		de_vec2_sub(&v->pos, &a->pos, &perp);
		v->color = a->color;
		de_vec2_set(&v->tex_coord, 0, 0);

		++v;
		de_vec2_sub(&v->pos, &b->pos, &perp);
		v->color = b->color;
		de_vec2_set(&v->tex_coord, 1, 0);

		++v;
		de_vec2_add(&v->pos, &b->pos, &perp);
		v->color = b->color;
		de_vec2_set(&v->tex_coord, 1, 1);

		++v;
		de_vec2_add(&v->pos, &a->pos, &perp);
		v->color = a->color;
		de_vec2_set(&v->tex_coord, 0, 1);

		/* indices */
		de_gui_draw_list_ib_push_triangle(draw_list, index, index + 1, index + 2);
		de_gui_draw_list_ib_push_triangle(draw_list, index, index + 2, index + 3);
	}

	/* patch holes between lines */
	for (i = 1; i < polyline->points.size - 1; ++i) {
		int index = de_gui_draw_list_get_next_index_origin(draw_list);
		de_gui_vertex_t* v = de_gui_draw_list_reserve_vertices(draw_list, 5);
		de_gui_line_point_t* left = polyline->points.data + i - 1;
		de_gui_line_point_t* center = left + 1;
		de_gui_line_point_t* right = center + 1;
		de_vec2_t thickness_vec_left, thickness_vec_right;
		de_vec2_t left_a, left_b, right_a, right_b;
		de_gui_get_line_thickness_vector(&left->pos, &center->pos, polyline->thickness, &thickness_vec_left);
		de_gui_get_line_thickness_vector(&center->pos, &right->pos, polyline->thickness, &thickness_vec_right);
		de_vec2_add(&left_a, &center->pos, &thickness_vec_left);
		de_vec2_sub(&left_b, &center->pos, &thickness_vec_left);
		de_vec2_add(&right_a, &center->pos, &thickness_vec_right);
		de_vec2_sub(&right_b, &center->pos, &thickness_vec_right);

		/* add patch vertices */
		v->pos = center->pos;
		v->color = center->color;

		++v;
		v->pos = left_a;
		v->color = left->color;

		++v;
		v->pos = right_a;
		v->color = right->color;

		++v;
		v->pos = left_b;
		v->color = left->color;

		++v;
		v->pos = right_b;
		v->color = right->color;

		/* add patch indices */
		de_gui_draw_list_ib_push_triangle(draw_list, index + 1, index + 2, index);
		de_gui_draw_list_ib_push_triangle(draw_list, index + 3, index + 4, index);
	}
}


void de_gui_polyline_free(de_gui_polyline_t* polyline)
{
	DE_ARRAY_FREE(polyline->points);
}


void de_gui_polyline_add_point(de_gui_polyline_t* polyline, float x, float y, const de_color_t* clr)
{
	de_gui_line_point_t* pt;
	DE_ARRAY_GROW(polyline->points, 1);
	pt = polyline->points.data + polyline->points.size - 1;
	pt->pos.x = x;
	pt->pos.y = y;
	pt->color = *clr;
}


void de_gui_draw_list_push_rect(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, float thickness, const de_color_t* clr)
{
	float offset = thickness * 0.5f;

	de_vec2_t left_top;
	de_vec2_t right_top;
	de_vec2_t right_bottom;
	de_vec2_t left_bottom;

	de_vec2_t left_top_off;
	de_vec2_t right_top_off;
	de_vec2_t right_bottom_off;
	de_vec2_t left_bottom_off;

	left_top.x = pos->x + offset;
	left_top.y = pos->y + thickness;

	right_top.x = pos->x + size->x - offset;
	right_top.y = pos->y + thickness;

	right_bottom.x = pos->x + size->x - offset;
	right_bottom.y = pos->y + size->y - thickness;

	left_bottom.x = pos->x + offset;
	left_bottom.y = pos->y + size->y - thickness;

	left_top_off.x = pos->x;
	left_top_off.y = pos->y + offset;

	right_top_off.x = pos->x + size->x;
	right_top_off.y = pos->y + offset;

	right_bottom_off.x = pos->x + size->x;
	right_bottom_off.y = pos->y + size->y - offset;

	left_bottom_off.x = pos->x;
	left_bottom_off.y = pos->y + size->y - offset;

	/* horizontal lines */
	de_gui_draw_list_push_line(draw_list, &left_top_off, &right_top_off, thickness, clr);
	de_gui_draw_list_push_line(draw_list, &right_bottom_off, &left_bottom_off, thickness, clr);

	/* vertical lines */
	de_gui_draw_list_push_line(draw_list, &right_top, &right_bottom, thickness, clr);
	de_gui_draw_list_push_line(draw_list, &left_bottom, &left_top, thickness, clr);
}


void de_gui_draw_list_push_rect_vary(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, const de_gui_thickness_t* thickness, const de_color_t* clr)
{
	de_vec2_t left_top;
	de_vec2_t right_top;
	de_vec2_t right_bottom;
	de_vec2_t left_bottom;

	de_vec2_t left_top_off;
	de_vec2_t right_top_off;
	de_vec2_t right_bottom_off;
	de_vec2_t left_bottom_off;

	left_top.x = pos->x + thickness->left * 0.5f;
	left_top.y = pos->y + thickness->top;

	right_top.x = pos->x + size->x - thickness->right * 0.5f;
	right_top.y = pos->y + thickness->top;

	right_bottom.x = pos->x + size->x - thickness->right * 0.5f;
	right_bottom.y = pos->y + size->y - thickness->bottom;

	left_bottom.x = pos->x + thickness->left * 0.5f;
	left_bottom.y = pos->y + size->y - thickness->bottom;

	left_top_off.x = pos->x;
	left_top_off.y = pos->y + thickness->top * 0.5f;

	right_top_off.x = pos->x + size->x;
	right_top_off.y = pos->y + thickness->top * 0.5f;

	right_bottom_off.x = pos->x + size->x;
	right_bottom_off.y = pos->y + size->y - thickness->bottom * 0.5f;

	left_bottom_off.x = pos->x;
	left_bottom_off.y = pos->y + size->y - thickness->bottom * 0.5f;

	/* horizontal lines */
	de_gui_draw_list_push_line(draw_list, &left_top_off, &right_top_off, thickness->top, clr);
	de_gui_draw_list_push_line(draw_list, &right_bottom_off, &left_bottom_off, thickness->bottom, clr);

	/* vertical lines */
	de_gui_draw_list_push_line(draw_list, &right_top, &right_bottom, thickness->right, clr);
	de_gui_draw_list_push_line(draw_list, &left_bottom, &left_top, thickness->left, clr);
}


void de_gui_draw_list_ib_push_triangle(de_gui_draw_list_t* draw_list, int a, int b, int c)
{
	DE_ARRAY_APPEND(draw_list->index_buffer, a);
	DE_ARRAY_APPEND(draw_list->index_buffer, b);
	DE_ARRAY_APPEND(draw_list->index_buffer, c);
	++draw_list->triangles_to_commit;
}


de_gui_vertex_t* de_gui_draw_list_reserve_vertices(de_gui_draw_list_t* draw_list, size_t n)
{
	DE_ARRAY_GROW(draw_list->vertex_buffer, n);
	return draw_list->vertex_buffer.data + draw_list->vertex_buffer.size - n;
}


int de_gui_draw_list_get_next_index_origin(de_gui_draw_list_t* draw_list)
{
	return draw_list->index_buffer.size ? DE_ARRAY_LAST(draw_list->index_buffer) + 1 : 0;
}


void de_gui_draw_list_push_rect_filled(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, const de_color_t* clr, const de_vec2_t* tex_coords)
{
	int index = de_gui_draw_list_get_next_index_origin(draw_list);
	de_gui_vertex_t* v = de_gui_draw_list_reserve_vertices(draw_list, 4);

	/* vertices */
	v->pos.x = pos->x;
	v->pos.y = pos->y;
	v->color = *clr;
	if (tex_coords) {
		v->tex_coord = tex_coords[0];
	} else {
		v->tex_coord.x = 0.0f;
		v->tex_coord.y = 0.0f;
	}

	++v;
	v->pos.x = pos->x + size->x;
	v->pos.y = pos->y;
	v->color = *clr;
	if (tex_coords) {
		v->tex_coord = tex_coords[1];
	} else {
		v->tex_coord.x = 1.0f;
		v->tex_coord.y = 0.0f;
	}

	++v;
	v->pos.x = pos->x + size->x;
	v->pos.y = pos->y + size->y;
	v->color = *clr;
	if (tex_coords) {
		v->tex_coord = tex_coords[2];
	} else {
		v->tex_coord.x = 1.0f;
		v->tex_coord.y = 1.0f;
	}

	++v;
	v->pos.x = pos->x;
	v->pos.y = pos->y + size->y;
	v->color = *clr;
	if (tex_coords) {
		v->tex_coord = tex_coords[3];
	} else {
		v->tex_coord.x = 0.0f;
		v->tex_coord.y = 1.0f;
	}

	/* indices */
	de_gui_draw_list_ib_push_triangle(draw_list, index, index + 1, index + 2);
	de_gui_draw_list_ib_push_triangle(draw_list, index, index + 2, index + 3);
}


void de_gui_draw_list_commit(de_gui_draw_list_t* draw_list, de_gui_draw_command_type_t type, unsigned int texture, de_gui_node_t* n)
{
	if (draw_list->triangles_to_commit) {
		de_gui_draw_command_t* cmd;
		DE_ARRAY_GROW(draw_list->commands, 1);
		cmd = &DE_ARRAY_LAST(draw_list->commands);

		cmd->type = type;
		cmd->texture = texture;
		cmd->nesting = draw_list->current_nesting;
		if (draw_list->index_buffer.size > 0) {
			cmd->index_offset = draw_list->index_buffer.size - draw_list->triangles_to_commit * 3;
		}
		cmd->triangle_count = draw_list->triangles_to_commit;
		draw_list->triangles_to_commit = 0;
		if (n) {
			DE_ARRAY_APPEND(n->geometry, draw_list->commands.size - 1);
		}
	}
}

void de_gui_draw_list_push_text(de_gui_draw_list_t* draw_list, const de_font_t* font, const uint32_t* utf32_text, size_t text_len, const de_vec2_t* pos, const de_color_t* clr)
{
	de_vec2_t cursor = *pos;
	for (size_t i = 0; i < text_len; ++i) {
		const uint32_t code = utf32_text[i];		
		de_glyph_t* glyph = de_font_get_glyph(font, code);
		if (glyph) {
			if (glyph->has_outline) {
				const de_vec2_t chpos = {
					cursor.x + glyph->bitmap_left,
					cursor.y + font->ascender - glyph->bitmap_top - glyph->bitmap_height
				};
				const de_vec2_t char_size = {
					(float)glyph->bitmap_width, 
					(float)glyph->bitmap_height
				};
				de_gui_draw_list_push_rect_filled(draw_list, &chpos, &char_size, clr, glyph->texCoords);
			}
			cursor.x += glyph->advance;
		} else {
			const de_vec2_t invalid_char_pos = {
				cursor.x,
				cursor.y + font->ascender 
			};
			const de_vec2_t invalid_char_size = {
				(float)font->height,
				(float)font->height
			};
			de_gui_draw_list_push_rect(draw_list, &invalid_char_pos, &invalid_char_size, 1, clr);
			cursor.x += invalid_char_size.x;
		}
	}
}

void de_gui_draw_list_commit_clip_geom(de_gui_draw_list_t* draw_list, de_gui_node_t* n)
{
	de_gui_draw_list_commit(draw_list, DE_GUI_DRAW_COMMAND_TYPE_CLIP, 0, n);
	DE_ARRAY_APPEND(draw_list->clip_cmd_stack, draw_list->commands.size - 1);
}

void de_gui_draw_list_revert_clip_geom(de_gui_draw_list_t* draw_list)
{
	int i;
	if (draw_list->clip_cmd_stack.size == 0) {
		de_fatal_error("mismatch push/pop clipping commands!");
	} else {
		/* decrement stack top */
		i = --draw_list->clip_cmd_stack.size;
		if (i >= 0) {
			/* re-commit previous clipping command */
			size_t index = draw_list->clip_cmd_stack.data[i];
			DE_ARRAY_APPEND(draw_list->commands, draw_list->commands.data[index]);
		}
	}
}


void de_gui_draw_list_commit_clip_rect(de_gui_draw_list_t* draw_list, float x, float y, float w, float h, de_gui_node_t* n)
{
	de_vec2_t pos;
	de_vec2_t size;
	de_color_t clr = { 0, 0, 0, 0 };
	de_vec2_t tc[4] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };

	pos.x = x;
	pos.y = y;

	size.x = w;
	size.y = h;

	de_gui_draw_list_push_rect_filled(draw_list, &pos, &size, &clr, tc);
	de_gui_draw_list_commit_clip_geom(draw_list, n);
}


void de_gui_draw_list_set_nesting(de_gui_draw_list_t* draw_list, uint8_t nesting)
{
	draw_list->current_nesting = nesting;
}

void de_gui_draw_list_clear(de_gui_draw_list_t* draw_list)
{
	DE_ARRAY_CLEAR(draw_list->commands);
	DE_ARRAY_CLEAR(draw_list->vertex_buffer);
	DE_ARRAY_CLEAR(draw_list->index_buffer);
	DE_ARRAY_CLEAR(draw_list->clip_cmd_stack);
}