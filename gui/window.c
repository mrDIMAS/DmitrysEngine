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

static void de_gui_window_header_mouse_down(de_gui_node_t* node, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* window_node;
	de_gui_window_t* wnd;

	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);

	window_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_WINDOW);

	if (!de_gui_window_is_flags_set(window_node, DE_GUI_WINDOW_FLAGS_NO_MOVE)) {
		de_gui_node_capture_mouse(node);
		wnd = &window_node->s.window;
		wnd->is_dragging = true;
		de_vec2_set(&wnd->mouse_click_pos, args->s.mouse_down.pos.x, args->s.mouse_down.pos.y);
		de_vec2_set(&wnd->init_pos, window_node->actual_local_position.x, window_node->actual_local_position.y);
		args->handled = true;
	}
}

static void de_gui_window_header_mouse_up(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* window_node;
	de_gui_window_t* wnd;

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);

	window_node = de_gui_node_find_parent_of_type(n, DE_GUI_NODE_WINDOW);

	if (!de_gui_window_is_flags_set(window_node, DE_GUI_WINDOW_FLAGS_NO_MOVE)) {
		de_gui_node_release_mouse_capture(n);
		wnd = &window_node->s.window;
		wnd->is_dragging = false;
		args->handled = true;
	}
}

static void de_gui_window_header_mouse_move(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* window_node;
	de_gui_window_t* wnd;

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);

	window_node = de_gui_node_find_parent_of_type(n, DE_GUI_NODE_WINDOW);

	if (!de_gui_window_is_flags_set(window_node, DE_GUI_WINDOW_FLAGS_NO_MOVE)) {
		wnd = &window_node->s.window;
		if (wnd->is_dragging) {
			de_vec2_t* mouse_pos = &args->s.mouse_move.pos;

			de_vec2_t delta;
			de_vec2_sub(&delta, mouse_pos, &wnd->mouse_click_pos);

			window_node->desired_local_position.x = wnd->init_pos.x + delta.x;
			window_node->desired_local_position.y = wnd->init_pos.y + delta.y;

			args->handled = true;
		}
	}
}

static void de_gui_window_init(de_gui_node_t* n)
{
	const float header_height = 30.0f;

	de_gui_window_t* wnd = &n->s.window;

	wnd->border = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_set_color_rgba(wnd->border, 120, 120, 120, 255);
	de_gui_node_attach(wnd->border, n);

	wnd->grid = de_gui_node_create(n->gui, DE_GUI_NODE_GRID);
	de_gui_node_attach(wnd->grid, wnd->border);
	de_gui_grid_add_column(wnd->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(wnd->grid, 0, DE_GUI_SIZE_MODE_AUTO);
	de_gui_grid_add_row(wnd->grid, 0, DE_GUI_SIZE_MODE_STRETCH);

	wnd->header = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_set_color_rgba(wnd->header, 120, 120, 120, 255);
	de_gui_node_attach(wnd->header, wnd->grid);
	de_gui_node_set_row(wnd->header, 0);
	de_gui_node_set_horizontal_alignment(wnd->header, DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH);
	de_gui_node_set_size(wnd->header, 0, header_height);
	wnd->header->mouse_down = de_gui_window_header_mouse_down;
	wnd->header->mouse_up = de_gui_window_header_mouse_up;
	wnd->header->mouse_move = de_gui_window_header_mouse_move;

	wnd->header_grid = de_gui_node_create(n->gui, DE_GUI_NODE_GRID);
	de_gui_node_attach(wnd->header_grid, wnd->header);
	de_gui_grid_add_column(wnd->header_grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(wnd->header_grid, header_height, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_column(wnd->header_grid, header_height, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_row(wnd->header_grid, 0, DE_GUI_SIZE_MODE_STRETCH);

	wnd->title = de_gui_node_create(n->gui, DE_GUI_NODE_TEXT);
	de_gui_text_set_text_utf8(wnd->title, "Unnamed window");
	de_gui_node_set_margin_uniform(wnd->title, 5);
	de_gui_node_attach(wnd->title, wnd->header_grid);
	de_gui_node_set_row(wnd->title, 0);
	de_gui_node_set_column(wnd->title, 0);

	wnd->close_button = de_gui_node_create(n->gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(wnd->close_button), "X");
	de_gui_node_attach(wnd->close_button, wnd->header_grid);
	de_gui_node_set_row(wnd->close_button, 0);
	de_gui_node_set_column(wnd->close_button, 2);
	de_gui_node_set_margin_uniform(wnd->close_button, 2);

	wnd->collapse_button = de_gui_node_create(n->gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(wnd->collapse_button), "_");
	de_gui_node_attach(wnd->collapse_button, wnd->header_grid);
	de_gui_node_set_row(wnd->collapse_button, 0);
	de_gui_node_set_column(wnd->collapse_button, 1);
	de_gui_node_set_margin_uniform(wnd->collapse_button, 2);

	wnd->scroll_viewer = de_gui_node_create(n->gui, DE_GUI_NODE_SCROLL_VIEWER);
	de_gui_node_attach(wnd->scroll_viewer, wnd->grid);
	de_gui_node_set_row(wnd->scroll_viewer, 1);
}

de_gui_node_dispatch_table_t* de_gui_window_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_window_init
	};
	return &dispatch_table;
}

de_gui_node_t * de_gui_window_get_title(de_gui_node_t * window)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	return window->s.window.title;
}

void de_gui_window_set_content(de_gui_node_t * window, de_gui_node_t * content)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	de_gui_scroll_viewer_set_content(window->s.window.scroll_viewer, content);
}

de_gui_node_t * de_gui_window_get_content(de_gui_node_t * window)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	return window->s.window.scroll_viewer->s.scroll_viewer.content;
}

void de_gui_window_set_flags(de_gui_node_t* window, uint32_t flags)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	window->s.window.flags |= flags;
}

bool de_gui_window_is_flags_set(de_gui_node_t* window, uint32_t flags)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	return (window->s.window.flags & flags) == flags;
}

void de_gui_window_enable_vertical_scroll(de_gui_node_t* window, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	de_gui_window_t* wnd = &window->s.window;
	de_gui_scroll_viewer_enable_vertical_scroll(wnd->scroll_viewer, value);
}

void de_gui_window_enable_horizontal_scroll(de_gui_node_t* window, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(window, DE_GUI_NODE_WINDOW);
	de_gui_window_t* wnd = &window->s.window;
	de_gui_scroll_viewer_enable_horizontal_scroll(wnd->scroll_viewer, value);
}