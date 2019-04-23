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

/**
 * @brief Make sure that content present will emit transparent rectangle for hit test.  
 */
static void de_gui_scroll_content_presenter_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	const de_vec2_t* scr_pos = &n->screen_position;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_vec2_t tex_coords[4] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	DE_UNUSED(nesting);

	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &(de_color_t) {0, 0, 0, 0 }, tex_coords);
	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
}

static de_vec2_t de_gui_scroll_content_presenter_measure_override(de_gui_node_t* n, const de_vec2_t* available_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;
	de_vec2_t desired_size = { 0, 0 };
	const de_vec2_t size_for_child = {
		scp->can_horizontally_scroll ? INFINITY : available_size->x,
		scp->can_vertically_scroll ? INFINITY : available_size->y
	};
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_measure(child, &size_for_child);
		if (child->desired_size.x > desired_size.x) {
			desired_size.x = child->desired_size.x;
		}
		if (child->desired_size.y > desired_size.y) {
			desired_size.y = child->desired_size.y;
		}
	}
	return desired_size;
}

static de_vec2_t de_gui_scroll_content_presenter_arrange_override(de_gui_node_t* n, const de_vec2_t* final_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;
	const de_rectf_t child_rect = {
		-scp->scroll.x, -scp->scroll.y,
		final_size->x + scp->scroll.x, final_size->y + scp->scroll.y,
	};
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_arrange(child, &child_rect);
	}
	return *final_size;
}

static void de_gui_scroll_content_presenter_init(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;
	scp->scroll.x = 0;
	scp->scroll.y = 0;
}

de_gui_node_dispatch_table_t* de_gui_scroll_content_presenter_get_dispatch_table(void)
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_scroll_content_presenter_init,
		.measure_override = de_gui_scroll_content_presenter_measure_override,
		.arrange_override = de_gui_scroll_content_presenter_arrange_override,
		.render = de_gui_scroll_content_presenter_render
	};
	return &dispatch_table;
}

void de_gui_scroll_content_presenter_set_v_scroll(de_gui_node_t* node, float val)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	node->s.scroll_content_presenter.scroll.y = val;
}

void de_gui_scroll_content_presenter_set_h_scroll(de_gui_node_t* node, float val)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	node->s.scroll_content_presenter.scroll.x = val;
}

void de_gui_scroll_content_presenter_enable_vertical_scroll(de_gui_node_t* n, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;
	scp->can_vertically_scroll = value;
}

void de_gui_scroll_content_presenter_enable_horizontal_scroll(de_gui_node_t* n, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;
	scp->can_horizontally_scroll = value;
}