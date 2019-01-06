/*=======================================================================================*/
static void de_gui_text_deinit(de_gui_node_t* n)
{
	de_gui_text_t* txt;

	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_TEXT);

	txt = &n->s.text;

	DE_ARRAY_FREE(txt->str);
	DE_ARRAY_FREE(txt->lines);
}

/*=======================================================================================*/
static void de_gui_text_break_on_lines(de_gui_node_t* node)
{
	size_t i;
	de_gui_text_t* txt;
	de_gui_text_line_t line = { 0 };
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	txt = &node->s.text;
	if (!txt->font)
	{
		return;
	}
	txt->total_lines_height = 0;
	DE_ARRAY_CLEAR(txt->lines);
	for (i = 0; i < txt->str.size; ++i)
	{
		float new_width;		
		uint32_t code = txt->str.data[i];
		de_glyph_t* glyph = de_font_get_glyph(txt->font, code);
		if (!glyph)
		{
			continue;
		}		
		new_width = line.width + glyph->advance;
		if (new_width > node->actual_size.x || code == '\n')
		{
			de_bool_t control_char = code == '\n' || code == '\r';
			/* commit line */
			DE_ARRAY_APPEND(txt->lines, line);
			/* start new line */
			line.begin = control_char ? i + 1 : i;
			line.end = line.begin;
			line.width = glyph->advance;
			txt->total_lines_height += txt->font->ascender;
		}
		else
		{
			line.width = new_width;
			++line.end;
		}
	}
	/* commit rest of the text */
	if (line.begin != line.end)
	{
		line.end = txt->str.size;
		DE_ARRAY_APPEND(txt->lines, line);
		txt->total_lines_height += txt->font->ascender;
	}
}

/*=======================================================================================*/
static void de_gui_text_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	size_t i;
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_text_t* txt = &n->s.text;
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_TEXT);
	DE_UNUSED(nesting);
	if (txt->font)
	{
		de_vec2_t pos = n->screen_position;
		de_gui_text_break_on_lines(n);
		if (txt->alignment == DE_GUI_TA_CENTER)
		{
			pos.y = scr_pos->y + n->actual_size.y * 0.5f - txt->total_lines_height * 0.5f;
		}
		for (i = 0; i < txt->lines.size; ++i)
		{
			de_gui_text_line_t* line = txt->lines.data + i;
			uint32_t* str = txt->str.data + line->begin;
			size_t len = line->end - line->begin;
			if (txt->alignment == DE_GUI_TA_CENTER)
			{
				pos.x = scr_pos->x + n->actual_size.x * 0.5f - line->width * 0.5f;
			}
			else if (txt->alignment == DE_GUI_TA_LEFT)
			{
				pos.x = scr_pos->x;
			}
			de_gui_draw_list_push_text(dl, txt->font, str, len, &pos, &n->color);
			pos.y += txt->font->ascender;
		}
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, txt->font->texture->id, n);
	}
}

/*=======================================================================================*/
de_gui_node_t* de_gui_text_create(de_gui_t* gui)
{
	de_gui_node_t* n;
	de_gui_text_t* txt;

	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;
		if (!init)
		{
			dispatch_table.deinit = de_gui_text_deinit;
			dispatch_table.render = de_gui_text_render;
			init = DE_TRUE;
		}
	}

	n = de_gui_node_alloc(gui, DE_GUI_NODE_TEXT, &dispatch_table);

	txt = &n->s.text;
	txt->font = gui->default_font;
	DE_ARRAY_INIT(txt->str);

	return n;
}

/*=======================================================================================*/
void de_gui_text_set_font(de_gui_node_t* node, de_font_t* font)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	node->s.text.font = font;
	de_gui_text_break_on_lines(node);
}

/*=======================================================================================*/
void de_gui_text_set_text(de_gui_node_t* node, const char* utf8str)
{
	de_gui_text_t* txt;
	size_t i;
	size_t len;
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	txt = &node->s.text;
	len = de_utf8_to_utf32(utf8str, node->gui->text_buffer, node->gui->text_buffer_size);
	DE_ARRAY_CLEAR(txt->str);
	for (i = 0; i < len; ++i)
	{
		DE_ARRAY_APPEND(txt->str, node->gui->text_buffer[i]);
	}
	de_gui_text_break_on_lines(node);
}

/*=======================================================================================*/
void de_gui_text_set_alignment(de_gui_node_t* node, de_gui_text_alignment_t alignment)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_TEXT);
	node->s.text.alignment = alignment;
}

