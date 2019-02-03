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

static void de_gui_text_box_deinit(de_gui_node_t* n) {
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);

	DE_UNUSED(n);
}

static bool de_gui_text_box_text_entered(de_gui_node_t* n, const de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	switch (args->type) {
		case DE_GUI_ROUTED_EVENT_TEXT:
			de_str_append(&tb->text, args->s.text.unicode);
			de_gui_text_set_text_utf32(tb->text_block, tb->text.data);
			return true;			
		default:
			break;
	}
	return false;
}

static bool de_gui_text_box_lost_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	DE_UNUSED(args);
}

static bool de_gui_text_box_got_focus(de_gui_node_t* n, de_gui_routed_event_args_t* args) {
	de_gui_text_box_t* tb;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_TEXT_BOX);
	tb = &n->s.text_box;
	DE_UNUSED(args);
}

de_gui_node_t* de_gui_text_box_create(de_gui_t* gui) {
	de_gui_node_t* n;
	de_gui_text_box_t* tb;

	static de_gui_dispatch_table_t dispatch_table;
	{
		static bool init = false;
		if (!init) {
			dispatch_table.deinit = de_gui_text_box_deinit;
			init = true;
		}
	}

	n = de_gui_node_alloc(gui, DE_GUI_NODE_TEXT_BOX, &dispatch_table);
	n->text_entered = de_gui_text_box_text_entered;

	tb = &n->s.text_box;

	tb->border = de_gui_border_create(gui);
	de_gui_node_set_color_rgba(tb->border, 120, 120, 120, 255);
	de_gui_node_attach(tb->border, n);

	tb->scroll_view = de_gui_scroll_viewer_create(gui);
	de_gui_node_attach(tb->scroll_view, tb->border);
	
	tb->text_block = de_gui_text_create(gui);	
	de_gui_node_set_vertical_alignment(tb->text_block, DE_GUI_VERTICAL_ALIGNMENT_TOP);
	de_gui_node_set_horizontal_alignment(tb->text_block, DE_GUI_HORIZONTAL_ALIGNMENT_LEFT);
	de_gui_scroll_viewer_set_content(tb->scroll_view, tb->text_block);

	//tb->cursor = de_gui_border_create(gui);
	

	DE_ARRAY_INIT(tb->text);

	return n;
}
