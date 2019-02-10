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

static void de_gui_slide_selector_deinit(de_gui_node_t* node) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
}

static void de_gui_slide_selector_on_prev_click(de_gui_node_t* node, void* user_data) {	
	de_gui_slide_selector_t* ss;
	de_gui_node_t* selector_node;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	selector_node = (de_gui_node_t*)user_data;
	ss = &selector_node->s.slide_selector;
	if (ss->get_item_text) {
		if (ss->selection_index > 0) {
			char buffer[512];
			--ss->selection_index;
			ss->get_item_text(ss->items, ss->selection_index, buffer, sizeof(buffer));
			de_gui_text_set_text_utf8(ss->current_item, buffer);
			if (ss->selection_changed) {
				ss->selection_changed(selector_node, ss->selection_index);
			}
		}
	} else {
		de_gui_text_set_text_utf8(ss->current_item, "");
	}
}

static void de_gui_slide_selector_on_next_click(de_gui_node_t* node, void* user_data) {
	de_gui_slide_selector_t* ss;
	de_gui_node_t* selector_node;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);	
	selector_node = (de_gui_node_t*)user_data;
	ss = &selector_node->s.slide_selector;
	if (ss->get_item_text) {
		if (ss->selection_index < ss->item_count - 1) {
			char buffer[512];
			++ss->selection_index;
			ss->get_item_text(ss->items, ss->selection_index, buffer, sizeof(buffer));
			de_gui_text_set_text_utf8(ss->current_item, buffer);
			if (ss->selection_changed) {
				ss->selection_changed(selector_node, ss->selection_index);
			}
		}
	} else {
		de_gui_text_set_text_utf8(ss->current_item, "");
	}
}

de_gui_node_t* de_gui_slide_selector_create(de_gui_t* gui) {
	de_gui_node_t* node;
	de_gui_node_t* border;
	de_gui_node_t* grid;
	de_gui_node_t* prev;
	de_gui_node_t* next;
	de_gui_slide_selector_t* ss;
	static de_gui_dispatch_table_t table;
	{
		static bool init = false;
		if (!init) {
			table.deinit = de_gui_slide_selector_deinit;
			init = true;
		}
	}

	node = de_gui_node_alloc(gui, DE_GUI_NODE_SLIDE_SELECTOR, &table);
	ss = &node->s.slide_selector;

	border = de_gui_border_create(gui);
	de_gui_node_set_color_rgba(border, 120, 120, 120, 255);
	de_gui_node_attach(border, node);

	grid = de_gui_grid_create(gui);
	de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(grid, 30, DE_GUI_SIZE_MODE_STRICT);
	de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(grid, 30, DE_GUI_SIZE_MODE_STRICT);
	de_gui_node_attach(grid, border);

	next = de_gui_button_create(gui);
	de_gui_button_set_text(next, ">");
	de_gui_node_set_column(next, 2);
	de_gui_node_attach(next, grid);
	de_gui_button_set_click(next, de_gui_slide_selector_on_next_click, node);

	ss->current_item = de_gui_text_create(gui);
	de_gui_node_set_column(ss->current_item, 1);
	de_gui_node_attach(ss->current_item, grid);
	de_gui_text_set_vertical_alignment(ss->current_item, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
	de_gui_text_set_horizontal_alignment(ss->current_item, DE_GUI_HORIZONTAL_ALIGNMENT_CENTER);
	de_gui_text_set_text_utf8(ss->current_item, "some text");

	prev = de_gui_button_create(gui);
	de_gui_button_set_text(prev, "<");
	de_gui_node_set_column(prev, 0);
	de_gui_node_attach(prev, grid);
	de_gui_button_set_click(prev, de_gui_slide_selector_on_prev_click, node);

	return node;
}

void de_gui_slide_selector_set_items(de_gui_node_t* node, void* items, int item_count, de_gui_item_text_getter getter) {
    de_gui_slide_selector_t* ss;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	ss = &node->s.slide_selector;
	ss->items = items;
	ss->item_count = item_count;
	ss->get_item_text = getter;
}

void* de_gui_slide_selector_get_selection(de_gui_node_t* node) {
    de_gui_slide_selector_t* ss;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	ss = &node->s.slide_selector;
	return ss->selection;
}

void de_gui_slide_selector_set_selection_changed(de_gui_node_t* node, de_gui_selection_changed callback) {
    de_gui_slide_selector_t* ss;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	ss = &node->s.slide_selector;
	ss->selection_changed = callback;
}