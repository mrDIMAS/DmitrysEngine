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

static void de_gui_scroll_bar_apply_descriptor(de_gui_node_t* node, const de_gui_node_descriptor_t* desc)
{
	const de_gui_scroll_bar_descriptor_t* sbdesc = &desc->s.scroll_bar;
	if (sbdesc->max != sbdesc->min) {
		de_gui_scroll_bar_set_max_value(node, sbdesc->max);
		de_gui_scroll_bar_set_min_value(node, sbdesc->min);
	}
	de_gui_scroll_bar_set_value(node, sbdesc->value);
	de_gui_scroll_bar_set_value_changed(node, sbdesc->value_changed);
	de_gui_scroll_bar_set_orientation(node, sbdesc->orientation);
	de_gui_scroll_bar_set_step(node, sbdesc->step);
}

static void de_gui_scroll_bar_update_indicator(de_gui_node_t* node)
{
	de_gui_scroll_bar_t* sb;
	float percent;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	sb = &node->s.scroll_bar;
	percent = (sb->value - sb->min) / (sb->max - sb->min);
	switch (sb->orientation) {
		case DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL:
		{
			sb->indicator->desired_local_position.x = percent * de_maxf(0.0f, sb->indicator->parent->actual_size.x - sb->indicator->actual_size.x);
			sb->indicator->desired_local_position.y = 0;

			sb->indicator->desired_size.x = 20;
			sb->indicator->desired_size.y = sb->indicator->parent->actual_size.y;
			break;
		}
		case DE_GUI_SCROLL_BAR_ORIENTATION_VERTICAL:
		{
			sb->indicator->desired_local_position.x = 0;
			sb->indicator->desired_local_position.y = percent * de_maxf(0.0f, sb->indicator->parent->actual_size.y - sb->indicator->actual_size.y);

			sb->indicator->desired_size.x = sb->indicator->parent->actual_size.x;
			sb->indicator->desired_size.y = 20;
			break;
		}
	}
}

static void de_gui_scroll_bar_on_up_click(de_gui_node_t* node, void* user_data)
{
	de_gui_node_t* scroll_bar_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_t* sb = &scroll_bar_node->s.scroll_bar;
	DE_UNUSED(user_data);
	de_gui_scroll_bar_set_value(scroll_bar_node, sb->value - sb->step);
	de_gui_scroll_bar_update_indicator(scroll_bar_node);
}

static void de_gui_scroll_bar_on_down_click(de_gui_node_t* node, void* user_data)
{
	de_gui_node_t* scroll_bar_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_t* sb = &scroll_bar_node->s.scroll_bar;
	DE_UNUSED(user_data);
	de_gui_scroll_bar_set_value(scroll_bar_node, sb->value + sb->step);
	de_gui_scroll_bar_update_indicator(scroll_bar_node);
}

static void de_gui_scroll_bar_indicator_mouse_down(de_gui_node_t* node, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* scroll_bar_node;
	de_gui_scroll_bar_t* sb;
	de_gui_node_capture_mouse(node);
	scroll_bar_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_SCROLL_BAR);
	sb = &scroll_bar_node->s.scroll_bar;
	sb->is_dragging = true;
	de_vec2_sub(&sb->offset, &sb->indicator->screen_position, &args->s.mouse_down.pos);
	args->handled = true;
}

static void de_gui_scroll_bar_indicator_mouse_up(de_gui_node_t* node, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* scroll_bar_node;
	de_gui_scroll_bar_t* sb;
	de_gui_node_release_mouse_capture(node);
	scroll_bar_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_SCROLL_BAR);
	sb = &scroll_bar_node->s.scroll_bar;
	sb->is_dragging = false;
	args->handled = true;
}

static void de_gui_scroll_bar_indicator_mouse_move(de_gui_node_t* node, de_gui_routed_event_args_t* args)
{
	de_gui_node_t* scroll_bar_node = de_gui_node_find_parent_of_type(node, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_t* sb = &scroll_bar_node->s.scroll_bar;
	if (sb->is_dragging) {
		de_vec2_t* mouse_pos = &args->s.mouse_move.pos;
		float percent = 0.0;
		switch (sb->orientation) {
			case DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL:
			{
				float span = sb->canvas->actual_size.x - sb->indicator->actual_size.x;
				float offset = mouse_pos->x - sb->canvas->screen_position.x + sb->offset.x;
				if (span > 0) {
					percent = de_clamp(offset / span, 0.0, 1.0);
				}
				break;
			}
			case DE_GUI_SCROLL_BAR_ORIENTATION_VERTICAL:
			{
				float span = sb->canvas->actual_size.y - sb->indicator->actual_size.y;
				float offset = mouse_pos->y - sb->canvas->screen_position.y + sb->offset.y;
				if (span > 0) {
					percent = de_clamp(offset / span, 0.0, 1.0);
				}
				break;
			}
		}
		de_gui_scroll_bar_set_value(scroll_bar_node, percent * (sb->max - sb->min));
		de_gui_scroll_bar_update_indicator(scroll_bar_node);
		args->handled = true;
	}
}

static void de_gui_scroll_bar_init(de_gui_node_t* n)
{
	de_gui_scroll_bar_t* sb = &n->s.scroll_bar;

	sb->min = 0;
	sb->max = 100;
	sb->value = 0;
	sb->step = 1;

	sb->border = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_set_color_rgba(sb->border, 120, 120, 120, 255);
	de_gui_border_set_stroke_color_rgba(sb->border, 80, 80, 80, 255);
	de_gui_node_attach(sb->border, n);

	sb->grid = de_gui_node_create(n->gui, DE_GUI_NODE_GRID);
	de_gui_node_attach(sb->grid, sb->border);
	de_gui_grid_enable_draw_borders(sb->grid, false);

	sb->canvas = de_gui_node_create(n->gui, DE_GUI_NODE_CANVAS);
	de_gui_node_attach(sb->canvas, sb->grid);

	sb->indicator = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_attach(sb->indicator, sb->canvas);
	de_gui_node_set_desired_size(sb->indicator, 30, 30);
	de_gui_node_set_color_rgba(sb->indicator, 100, 100, 100, 255);
	de_gui_border_set_stroke_color_rgba(sb->indicator, 80, 80, 80, 255);
	de_gui_node_set_mouse_down(sb->indicator, de_gui_scroll_bar_indicator_mouse_down);
	de_gui_node_set_mouse_up(sb->indicator, de_gui_scroll_bar_indicator_mouse_up);
	de_gui_node_set_mouse_move(sb->indicator, de_gui_scroll_bar_indicator_mouse_move);

	sb->up_button = de_gui_node_create(n->gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(sb->up_button), "<");
	de_gui_node_attach(sb->up_button, sb->grid);
	de_gui_button_set_click(sb->up_button, de_gui_scroll_bar_on_up_click, NULL);

	sb->down_button = de_gui_node_create(n->gui, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(de_gui_button_get_text(sb->down_button), ">");
	de_gui_node_attach(sb->down_button, sb->grid);
	de_gui_button_set_click(sb->down_button, de_gui_scroll_bar_on_down_click, NULL);

	de_gui_scroll_bar_set_orientation(n, DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL);
	de_gui_scroll_bar_update_indicator(n);
}

de_gui_dispatch_table_t* de_gui_scroll_bar_get_dispatch_table(void)
{
	static de_gui_dispatch_table_t dispatch_table = {
		.init = de_gui_scroll_bar_init,
		.apply_descriptor = de_gui_scroll_bar_apply_descriptor
	};
	return &dispatch_table;
}

void de_gui_scroll_bar_set_orientation(de_gui_node_t* node, de_gui_scroll_bar_orientation_t orientation)
{
	de_gui_scroll_bar_t* sb;

	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);

	sb = &node->s.scroll_bar;
	sb->orientation = orientation;

	de_gui_grid_clear_columns(sb->grid);
	de_gui_grid_clear_rows(sb->grid);

	switch (orientation) {
		case DE_GUI_SCROLL_BAR_ORIENTATION_VERTICAL:
		{
			de_gui_grid_add_column(sb->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
			de_gui_grid_add_row(sb->grid, 30, DE_GUI_SIZE_MODE_STRICT);
			de_gui_grid_add_row(sb->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
			de_gui_grid_add_row(sb->grid, 30, DE_GUI_SIZE_MODE_STRICT);

			de_gui_node_set_row(sb->up_button, 0);
			de_gui_node_set_row(sb->canvas, 1);
			de_gui_node_set_row(sb->down_button, 2);

			de_gui_text_set_text_utf8(de_gui_button_get_text(sb->up_button), "^");
			de_gui_text_set_text_utf8(de_gui_button_get_text(sb->down_button), "v");

			de_gui_border_set_thickness(de_gui_node_find_direct_child_of_type(sb->up_button, DE_GUI_NODE_BORDER), 0, 0, 0, 1);
			de_gui_border_set_thickness(de_gui_node_find_direct_child_of_type(sb->down_button, DE_GUI_NODE_BORDER), 0, 1, 0, 0);
			de_gui_border_set_thickness(sb->indicator, 0, 1, 0, 1);
			break;
		}
		case DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL:
		{
			de_gui_grid_add_row(sb->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
			de_gui_grid_add_column(sb->grid, 30, DE_GUI_SIZE_MODE_STRICT);
			de_gui_grid_add_column(sb->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
			de_gui_grid_add_column(sb->grid, 30, DE_GUI_SIZE_MODE_STRICT);

			de_gui_node_set_column(sb->up_button, 0);
			de_gui_node_set_column(sb->canvas, 1);
			de_gui_node_set_column(sb->down_button, 2);

			de_gui_border_set_thickness(de_gui_node_find_direct_child_of_type(sb->up_button, DE_GUI_NODE_BORDER), 0, 0, 1, 0);
			de_gui_border_set_thickness(de_gui_node_find_direct_child_of_type(sb->down_button, DE_GUI_NODE_BORDER), 1, 0, 0, 0);
			de_gui_border_set_thickness(sb->indicator, 1, 0, 1, 0);
			break;
		}
		default:
		{
			de_fatal_error("invalid scroll bar orientation!");
			break;
		}
	}
}

void de_gui_scroll_bar_set_min_value(de_gui_node_t* node, float min)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	node->s.scroll_bar.min = min;
	de_gui_scroll_bar_update_indicator(node);
}

void de_gui_scroll_bar_set_max_value(de_gui_node_t* node, float max)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	node->s.scroll_bar.max = max;
	de_gui_scroll_bar_update_indicator(node);
}

void de_gui_scroll_bar_set_value_changed(de_gui_node_t* node, de_scroll_bar_value_changed_event_t evt)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	node->s.scroll_bar.value_changed = evt;
}

void de_gui_scroll_bar_set_step(de_gui_node_t* node, float value)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	node->s.scroll_bar.step = value;
}

void de_gui_scroll_bar_set_value(de_gui_node_t* node, float value)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SCROLL_BAR);
	de_gui_scroll_bar_t* sb = &node->s.scroll_bar;
	float old_value = sb->value;
	if (value < sb->min) {
		value = sb->min;
	} else if (value > sb->max) {
		value = sb->max;
	}
	sb->value = value;
	if (old_value != value && sb->value_changed) {
		sb->value_changed(node, old_value, value);
	}
}