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

static void de_gui_text_box_deinit(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	de_gui_text_box_t* tb = &n->s.text_box;
	de_str32_free(&tb->str);
	DE_ARRAY_FREE(tb->lines);
}

static void de_gui_text_box_reset_cursor_blink(de_gui_text_box_t* tb)
{
	tb->cursor_visible = true;
	tb->blink_timer = 0;
}

static void de_gui_text_box_lost_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(args);
	de_gui_text_box_t* tb = &n->s.text_box;
	tb->show_cursor = false;
}

static void de_gui_text_box_got_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(args);
	de_gui_text_box_t* tb = &n->s.text_box;
	tb->show_cursor = true;
	de_gui_text_box_reset_cursor_blink(tb);
}

static void de_gui_text_box_move_cursor_y(de_gui_text_box_t* tb, int dir)
{
	if (dir > 0) {
		if (tb->cursor_line < (int)tb->lines.size - 1) {
			++tb->cursor_line;
			de_gui_text_box_reset_cursor_blink(tb);
		}
	} else if (dir < 0) {
		if (tb->cursor_line > 0) {
			--tb->cursor_line;
			de_gui_text_box_reset_cursor_blink(tb);
		}
	}
}

static void de_gui_text_box_move_cursor_x(de_gui_text_box_t* tb, int amount)
{
	de_gui_text_line_t* line = tb->lines.size ? tb->lines.data + tb->cursor_line : NULL;
	if (line) {
		if (amount < 0) {
			int old_cursor_offset = tb->cursor_offset;
			tb->cursor_offset += amount;
			if (tb->cursor_offset <= 0) {
				int prev_index = tb->cursor_line - 1;
				de_gui_text_line_t* prev_line = prev_index >= 0 ? tb->lines.data + prev_index : NULL;
				if (prev_line) {
					tb->cursor_offset = prev_line->end - prev_line->begin - old_cursor_offset - amount;
					--tb->cursor_line;
				} else {
					tb->cursor_offset = 0;
				}
			}
			de_gui_text_box_reset_cursor_blink(tb);
		} else if (amount > 0) {
			int len;
			tb->cursor_offset += amount;
			len = (int)line->end - (int)line->begin;
			if (tb->cursor_offset > len) {
				if (tb->cursor_line + 1 < (int)tb->lines.size) {
					tb->cursor_offset = 1;
					++tb->cursor_line;
				} else {
					tb->cursor_offset = len;
				}
			}
			de_gui_text_box_reset_cursor_blink(tb);
		}
	}
}

static de_gui_text_line_t* de_gui_text_box_get_current_line(de_gui_text_box_t* tb)
{
	return tb->lines.size && tb->cursor_line < (int)tb->lines.size ? tb->lines.data + tb->cursor_line : NULL;
}

static int de_gui_text_box_cursor_to_index(de_gui_text_box_t* tb)
{
/* any out-of-bounds result are valid here since we are inserting characters via oob-protected func */
	de_gui_text_line_t* line = de_gui_text_box_get_current_line(tb);
	return line ? line->begin + tb->cursor_offset : 0;
}

static void de_gui_text_box_key_down(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	int i;
	int index, jump_offset;
	de_gui_text_box_t* tb;
	de_gui_text_line_t* line;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	switch (args->s.key.key) {
		case DE_KEY_LEFT:
			if (args->s.key.control) {
				uint32_t c;
				/* jump over word to left */
				index = de_gui_text_box_cursor_to_index(tb);
				if (index < 0) {
					index = 0;
				} else if (index >= (int)de_str32_length(&tb->str)) {
					index = de_str32_length(&tb->str);
				}
				c = de_str32_at(&tb->str, index > 0 ? index - 1 : 0);
				if (c < 255 && (ispunct(c) || isspace(c))) {
					/* step over 1 symbol */
					jump_offset = 1;
				} else {
					for (i = index, jump_offset = 0; i >= 0; --i, ++jump_offset) {
						c = de_str32_at(&tb->str, i);
						if (c < 255 && (ispunct(c) || isspace(c))) {
							--jump_offset;
							break;
						}
					}
				}
				de_gui_text_box_move_cursor_x(tb, -jump_offset);
			} else {
				de_gui_text_box_move_cursor_x(tb, -1);
			}
			break;
		case DE_KEY_RIGHT:
			if (args->s.key.control) {
				/* jump over word to right */

			} else {
				de_gui_text_box_move_cursor_x(tb, +1);
			}
			break;
		case DE_KEY_UP:
			de_gui_text_box_move_cursor_y(tb, -1);
			break;
		case DE_KEY_DOWN:
			de_gui_text_box_move_cursor_y(tb, +1);
			break;
		case DE_KEY_BACKSPACE:
			index = de_gui_text_box_cursor_to_index(tb) - 1;
			if (index >= 0) {
				de_str32_remove(&tb->str, index, 1);	
				de_gui_text_box_move_cursor_x(tb, -1);				
			}			
			break;
		case DE_KEY_DELETE:
			index = de_gui_text_box_cursor_to_index(tb);
			if (index >= 0) {
				de_str32_remove(&tb->str, index, 1);
			}
			break;
		case DE_KEY_V: /* paste */
			if (args->s.key.control) {

			}
			break;
		case DE_KEY_C: /* copy */
			if (args->s.key.control) {
				char* txt = de_clipboard_get_text();
				if (txt) {
					de_free(txt);
				}
			}
			break;
		case DE_KEY_End:
			line = de_gui_text_box_get_current_line(tb);
			if (line) {
				tb->cursor_offset = line->end - line->begin;
				de_gui_text_box_reset_cursor_blink(tb);
			}
			break;
		case DE_KEY_Home:
			tb->cursor_offset = 0;
			de_gui_text_box_reset_cursor_blink(tb);
			break;
		default:
			break;
	}
}

static void de_gui_text_box_update(de_gui_node_t* node)
{
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT_BOX);
	tb = &node->s.text_box;
	++tb->blink_timer;
	if (tb->blink_timer >= tb->blink_interval) {
		tb->blink_timer = 0;
		/* blink */
		tb->cursor_visible = !tb->cursor_visible;
	}
}

static void de_gui_text_box_mouse_down(de_gui_node_t* node, de_gui_routed_event_args_t* args)
{
	DE_UNUSED(node);
    DE_UNUSED(args);
}

static void de_gui_text_box_break_on_lines(de_gui_node_t* node)
{
	size_t i;
	de_vec2_t pos;
	de_gui_text_line_t line = { 0 };
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT_BOX);
	de_gui_text_box_t* tb = &node->s.text_box;
	if (!tb->font) {
		return;
	}
	tb->total_lines_height = 0;
	DE_ARRAY_CLEAR(tb->lines);
	for (i = 0; i < de_str32_length(&tb->str); ++i) {
		float new_width;
		bool control_char;
		de_glyph_t* glyph;
		uint32_t code = de_str32_at(&tb->str, i);
		control_char = code == '\n' || code == '\r';
		glyph = de_font_get_glyph(tb->font, code);
		if (!glyph) {
			continue;
		}
		new_width = line.width + glyph->advance;
		if (new_width > node->actual_size.x || control_char) {
			/* commit line */
			DE_ARRAY_APPEND(tb->lines, line);
			/* start new line */
			line.begin = control_char ? i + 1 : i;
			line.end = line.begin + 1;
			line.width = glyph->advance;
			tb->total_lines_height += tb->font->ascender;
		} else {
			line.width = new_width;
			++line.end;
		}
	}
	/* commit rest of the text */
	if (line.begin != line.end) {
		line.end = de_str32_length(&tb->str);
		DE_ARRAY_APPEND(tb->lines, line);
		tb->total_lines_height += tb->font->ascender;
	}
	/* calculate offsets */
	pos.x = pos.y = 0;
	for (i = 0; i < tb->lines.size; ++i) {
		de_gui_text_line_t* l = tb->lines.data + i;
		switch (tb->hor_alignment) {
			case DE_GUI_HORIZONTAL_ALIGNMENT_LEFT:
				pos.x = 0;
				break;
			case DE_GUI_HORIZONTAL_ALIGNMENT_CENTER:
				pos.x = node->actual_size.x * 0.5f - l->width * 0.5f;
				break;
			case DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT:
				pos.x = node->actual_size.x - l->width;
				break;
			case DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH:
				/* todo */
				break;
		}
		l->x = pos.x;
	}	
}

static void de_gui_text_box_text_entered(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	uint32_t code;
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	switch (args->type) {
		case DE_GUI_ROUTED_EVENT_TEXT:
			code = args->s.text.unicode;
			if (code < 255 && iscntrl(code)) {
				break;
			}
			int index = de_gui_text_box_cursor_to_index(tb);
			de_str32_insert(&tb->str, index, code);
			de_gui_text_box_break_on_lines(n);
			de_gui_text_box_move_cursor_x(tb, 1);
			return;
		default:
			break;
	}
}

static void de_gui_text_box_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	size_t i;
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_text_box_t* tb = &n->s.text_box;
	de_color_t clr = { 120, 120, 120, 255 };
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	DE_UNUSED(nesting);
	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &clr, NULL);
	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
	if (tb->font) {
		de_vec2_t pos = n->screen_position;
		de_gui_text_box_break_on_lines(n);
		switch (tb->ver_alignment) {
			case DE_GUI_VERTICAL_ALIGNMENT_TOP:
				/* already at top */
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
				pos.y = scr_pos->y + n->actual_size.y - tb->total_lines_height;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
				pos.y = scr_pos->y + n->actual_size.y * 0.5f - tb->total_lines_height * 0.5f;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
				/* todo */
				break;
		}
		for (i = 0; i < tb->lines.size; ++i) {
			de_gui_text_line_t* line = tb->lines.data + i;
			const uint32_t* str = de_str32_get_data(&tb->str) + line->begin;
			size_t len = line->end - line->begin;
			pos.x += line->x;
			de_gui_draw_list_push_text(dl, tb->font, str, len, &pos, &n->color);
			pos.y += tb->font->ascender;
		}
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, tb->font->texture, n);
		/* cursor */
		if (tb->show_cursor && tb->cursor_visible) {
			int k;
			de_vec2_t cpos, csize;
			if (tb->lines.size) {
				de_gui_text_line_t* line = tb->lines.data + tb->cursor_line;
				cpos.x = 0;
				for (i = line->begin, k = 0; i < line->end && k < tb->cursor_offset; ++i, ++k) {
					uint32_t c = de_str32_at(&tb->str, i);
					de_glyph_t* glyph = de_font_get_glyph(tb->font, c);
					if (glyph && (c != '\n')) {
						cpos.x += glyph->advance;
					}
				}
				cpos.x += scr_pos->x + line->x;
			} else {
				cpos.x = scr_pos->x;
			}
			cpos.y = scr_pos->y + tb->cursor_line * tb->font->ascender;
			csize.x = 2;
			csize.y = tb->font->ascender;
			de_gui_draw_list_push_rect_filled(dl, &cpos, &csize, &n->color, NULL);
			de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
		}
	}
}

static void de_gui_text_box_init(de_gui_node_t* n)
{
	n->text_entered = de_gui_text_box_text_entered;
	n->got_focus = de_gui_text_box_got_focus;
	n->lost_focus = de_gui_text_box_lost_focus;
	n->key_down = de_gui_text_box_key_down;
	n->mouse_down = de_gui_text_box_mouse_down;
	de_color_set(&n->color, 255, 255, 255, 255);
	de_gui_text_box_t* tb = &n->s.text_box;
	tb->font = n->gui->default_font;
	tb->blink_interval = 35;
	tb->blink_timer = 0;
	de_str32_init(&tb->str);
}

de_gui_node_dispatch_table_t* de_gui_text_box_get_dispatch_table(void)
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_text_box_init,
		.deinit = de_gui_text_box_deinit,
		.render = de_gui_text_box_render,
		.update = de_gui_text_box_update
	};
	return &dispatch_table;
}