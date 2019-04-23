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

static void de_gui_scroll_viewer_ver_scroll_changed(de_gui_node_t* sv_node, float old_value, float new_value)
{
	de_gui_node_t* scroll_viewer_node;
	de_gui_scroll_viewer_t* sv;
	DE_UNUSED(old_value);
	DE_ASSERT_GUI_NODE_TYPE(sv_node, DE_GUI_NODE_SCROLL_BAR);
	scroll_viewer_node = de_gui_node_find_parent_of_type(sv_node, DE_GUI_NODE_SCROLL_VIEWER);
	sv = &scroll_viewer_node->s.scroll_viewer;
	de_gui_scroll_content_presenter_set_v_scroll(sv->scroll_content_presenter, new_value);
}

static void de_gui_scroll_viewer_hor_scroll_changed(de_gui_node_t* sv_node, float old_value, float new_value)
{
	de_gui_node_t* scroll_viewer_node;
	de_gui_scroll_viewer_t* sv;
	DE_UNUSED(old_value);
	DE_ASSERT_GUI_NODE_TYPE(sv_node, DE_GUI_NODE_SCROLL_BAR);
	scroll_viewer_node = de_gui_node_find_parent_of_type(sv_node, DE_GUI_NODE_SCROLL_VIEWER);
	sv = &scroll_viewer_node->s.scroll_viewer;
	de_gui_scroll_content_presenter_set_h_scroll(sv->scroll_content_presenter, new_value);
}

void de_gui_scroll_viewer_update(de_gui_node_t* n)
{
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;
	de_gui_node_visibility_t vis;

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_VIEWER);

	/* hide/show horizontal scroll bar */
	if (sv->content) {
		if (sv->content->desired_size.x <= sv->scroll_content_presenter->desired_size.x) {
			vis = DE_GUI_NODE_VISIBILITY_COLLAPSED;
		} else {
			vis = DE_GUI_NODE_VISIBILITY_VISIBLE;
		}
	} else {
		vis = DE_GUI_NODE_VISIBILITY_COLLAPSED;
	}

	de_gui_node_set_visibility(sv->hor_scroll_bar, vis);

	/* hide/show vertical scroll bar */
	if (sv->content) {
		if (sv->content->desired_size.y <= sv->scroll_content_presenter->desired_size.y) {
			vis = DE_GUI_NODE_VISIBILITY_COLLAPSED;
		} else {
			vis = DE_GUI_NODE_VISIBILITY_VISIBLE;
		}
	} else {
		vis = DE_GUI_NODE_VISIBILITY_COLLAPSED;
	}

	de_gui_node_set_visibility(sv->ver_scroll_bar, vis);

	/* update scroll info */
	if (sv->content) {
		float max = de_maxf(0.0f, sv->content->desired_size.x - sv->scroll_content_presenter->desired_size.x);
		de_gui_scroll_bar_set_max_value(sv->hor_scroll_bar, max);

		max = de_maxf(0.0f, sv->content->desired_size.y - sv->scroll_content_presenter->desired_size.y);
		de_gui_scroll_bar_set_max_value(sv->ver_scroll_bar, max);
	}
}

static void de_gui_scroll_viewer_mouse_wheel(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_VIEWER);	
	de_gui_scroll_bar_set_value(sv->ver_scroll_bar, de_gui_scroll_bar_get_value(sv->ver_scroll_bar) - 10 * args->s.wheel.delta);	
}

static void de_gui_scroll_viewer_init(de_gui_node_t* n)
{
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;
	
	n->mouse_wheel = de_gui_scroll_viewer_mouse_wheel;

	sv->border = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_set_color_rgba(sv->border, 100, 100, 100, 255);
	de_gui_node_attach(sv->border, n);

	sv->grid = de_gui_node_create(n->gui, DE_GUI_NODE_GRID);
	de_gui_node_attach(sv->grid, sv->border);
	de_gui_grid_add_row(sv->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(sv->grid, 0, DE_GUI_SIZE_MODE_AUTO);
	de_gui_grid_add_column(sv->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(sv->grid, 0, DE_GUI_SIZE_MODE_AUTO);
	de_gui_grid_enable_draw_borders(sv->grid, false);

	sv->scroll_content_presenter = de_gui_node_create(n->gui, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);
	de_gui_node_attach(sv->scroll_content_presenter, sv->grid);

	sv->hor_scroll_bar = de_gui_node_create(n->gui, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_set_orientation(sv->hor_scroll_bar, DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL);
	de_gui_node_attach(sv->hor_scroll_bar, sv->grid);
	de_gui_node_set_column(sv->hor_scroll_bar, 0);
	de_gui_node_set_row(sv->hor_scroll_bar, 1);
	de_gui_node_set_size(sv->hor_scroll_bar, 0, 20);
	de_gui_scroll_bar_set_value_changed(sv->hor_scroll_bar, de_gui_scroll_viewer_hor_scroll_changed);

	sv->ver_scroll_bar = de_gui_node_create(n->gui, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_set_orientation(sv->ver_scroll_bar, DE_GUI_SCROLL_BAR_ORIENTATION_VERTICAL);
	de_gui_node_attach(sv->ver_scroll_bar, sv->grid);
	de_gui_node_set_column(sv->ver_scroll_bar, 1);
	de_gui_node_set_row(sv->ver_scroll_bar, 0);
	de_gui_node_set_size(sv->ver_scroll_bar, 20, 0);
	de_gui_scroll_bar_set_value_changed(sv->ver_scroll_bar, de_gui_scroll_viewer_ver_scroll_changed);
}

de_gui_node_dispatch_table_t* de_gui_scroll_viewer_get_dispatch_table(void)
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_scroll_viewer_init,
		.update = de_gui_scroll_viewer_update,
	};
	return &dispatch_table;
}

void de_gui_scroll_viewer_set_content(de_gui_node_t* n, de_gui_node_t* content)
{
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_VIEWER);

	if (sv->content) {
		de_gui_node_detach(sv->content);
	}
	if (content) {
		de_gui_node_attach(content, sv->scroll_content_presenter);
	}

	sv->content = content;
}

void de_gui_scroll_viewer_enable_vertical_scroll(de_gui_node_t* n, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_VIEWER);
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;
	de_gui_scroll_content_presenter_enable_vertical_scroll(sv->scroll_content_presenter, value);
}

void de_gui_scroll_viewer_enable_horizontal_scroll(de_gui_node_t* n, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_SCROLL_VIEWER);
	de_gui_scroll_viewer_t* sv = &n->s.scroll_viewer;
	de_gui_scroll_content_presenter_enable_horizontal_scroll(sv->scroll_content_presenter, value);
}