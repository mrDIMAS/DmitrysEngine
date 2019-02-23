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

static void de_gui_text_deinit(de_gui_node_t* n) {
	de_gui_text_t* txt;

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT);

	txt = &n->s.text;

	de_str32_free(&txt->str);
	DE_ARRAY_FREE(txt->lines);
}

static void de_gui_text_break_on_lines(de_gui_node_t* node) {
	size_t i;
	de_vec2_t pos;
	float max_width = 0;
	de_gui_text_t* txt;
	de_gui_text_line_t line = { 0 };
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	txt = &node->s.text;
	if (!txt->font) {
		return;
	}
	txt->total_lines_height = 0;
	DE_ARRAY_CLEAR(txt->lines);
	for (i = 0; i < de_str32_length(&txt->str); ++i) {
		float new_width;
		uint32_t code = de_str32_at(&txt->str, i);
		de_glyph_t* glyph = de_font_get_glyph(txt->font, code);
		if (!glyph) {
			continue;
		}
		new_width = line.width + glyph->advance;
		if (new_width > node->actual_size.x || code == '\n') {
			bool control_char = code == '\n' || code == '\r';
			/* commit line */
			DE_ARRAY_APPEND(txt->lines, line);
			if (line.width > max_width) {
				max_width = line.width;
			}
			/* start new line */
			line.begin = control_char ? i + 1 : i;
			line.end = line.begin;
			line.width = glyph->advance;
			txt->total_lines_height += txt->font->ascender;
		} else {
			line.width = new_width;
			++line.end;
		}
	}
	/* commit rest of the text */
	if (line.begin != line.end) {
		line.end = de_str32_length(&txt->str);
		DE_ARRAY_APPEND(txt->lines, line);
		txt->total_lines_height += txt->font->ascender;
	}
	/* calculate offsets */
	pos.x = pos.y = 0;
	for (i = 0; i < txt->lines.size; ++i) {
		de_gui_text_line_t* l = txt->lines.data + i;
		switch (txt->hor_alignment) {
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

static void de_gui_text_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting) {
	size_t i;
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_text_t* txt = &n->s.text;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT);
	DE_UNUSED(nesting);
	if (txt->font) {
		de_vec2_t pos = n->screen_position;
		de_gui_text_break_on_lines(n);
		switch (txt->ver_alignment) {
			case DE_GUI_VERTICAL_ALIGNMENT_TOP:
				/* already at top */
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
				pos.y = scr_pos->y + n->actual_size.y - txt->total_lines_height;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
				pos.y = scr_pos->y + n->actual_size.y * 0.5f - txt->total_lines_height * 0.5f;
				break;
			case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
				/* todo */
				break;
		}
		for (i = 0; i < txt->lines.size; ++i) {
			de_gui_text_line_t* line = txt->lines.data + i;
			const uint32_t* str = de_str32_get_data(&txt->str) + line->begin;
			size_t len = line->end - line->begin;			
			pos.x += line->x;
			de_gui_draw_list_push_text(dl, txt->font, str, len, &pos, &n->color);
			pos.y += txt->font->ascender;
		}
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, txt->font->texture->id, n);
	}
}

de_gui_node_t* de_gui_text_create(de_gui_t* gui) {
	de_gui_node_t* n;
	de_gui_text_t* txt;

	static de_gui_dispatch_table_t dispatch_table;
	{
		static bool init = false;
		if (!init) {
			dispatch_table.deinit = de_gui_text_deinit;
			dispatch_table.render = de_gui_text_render;
			init = true;
		}
	}

	n = de_gui_node_alloc(gui, DE_GUI_NODE_TEXT, &dispatch_table);

	txt = &n->s.text;
	txt->font = gui->default_font;
	de_str32_init(&txt->str);
	return n;
}

void de_gui_text_set_font(de_gui_node_t* node, de_font_t* font) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	node->s.text.font = font;
	de_gui_text_break_on_lines(node);
}

void de_gui_text_set_text_utf8(de_gui_node_t* node, const char* utf8str) {
	de_gui_text_t* txt;
	size_t i, len, utf8strlen;	
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	txt = &node->s.text;
	utf8strlen = strlen(utf8str);
	len = de_utf8_to_utf32(utf8str, utf8strlen, node->gui->text_buffer, node->gui->text_buffer_size);	
	de_str32_clear(&txt->str);
	for (i = 0; i < len; ++i) {
		de_str32_append(&txt->str, node->gui->text_buffer[i]);
	}
	de_gui_text_break_on_lines(node);
}

void de_gui_text_set_text_utf32(de_gui_node_t* node, const uint32_t* utf32str) {
	de_gui_text_t* txt;
	const uint32_t* ptr;
	size_t i, len = 0;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	txt = &node->s.text;
	ptr = utf32str;
	while (*ptr++) {
		++len;
	}
	de_str32_clear(&txt->str);
	for (i = 0; i < len; ++i) {
		de_str32_append(&txt->str, utf32str[i]);
	}
}

void de_gui_text_set_vertical_alignment(de_gui_node_t* node, de_gui_vertical_alignment_t alignment) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	node->s.text.ver_alignment = alignment;
}

void de_gui_text_set_horizontal_alignment(de_gui_node_t* node, de_gui_horizontal_alignment_t alignment) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	node->s.text.hor_alignment = alignment;
}
