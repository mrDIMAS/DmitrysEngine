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

static void de_button_apply_descriptor(de_gui_node_t* node, const de_gui_node_descriptor_t* desc)
{
	const de_gui_button_descriptor_t* button_desc = &desc->s.button;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	de_gui_button_t* btn = &node->s.button;
	btn->click = button_desc->click;
	de_gui_button_set_text(node, button_desc->text);
}

static void de_button_border_mouse_down(de_gui_node_t* button, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(button, DE_GUI_NODE_BUTTON);
	de_gui_button_t* btn = &button->s.button;
	btn->border->color = btn->pressed_color;
	btn->was_pressed = true;
	args->handled = true;
}

static void de_button_border_mouse_enter(de_gui_node_t* button, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(button, DE_GUI_NODE_BUTTON);
	de_gui_button_t* btn = &button->s.button;
	btn->border->color = btn->hover_color;
	args->handled = true;
}

static void de_button_border_mouse_leave(de_gui_node_t* button, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(button, DE_GUI_NODE_BUTTON);
	de_gui_button_t* btn = &button->s.button;
	btn->border->color = btn->normal_color;
	args->handled = true;
}

static void de_button_border_mouse_up(de_gui_node_t* button, de_gui_routed_event_args_t* args)
{
	DE_ASSERT_GUI_NODE_TYPE(button, DE_GUI_NODE_BUTTON);
	de_gui_button_t* btn = &button->s.button;
	if (button->is_mouse_over) {
		btn->border->color = btn->hover_color;
	} else {
		btn->border->color = btn->normal_color;
	}
	if (btn->click.func && btn->was_pressed) {
		btn->click.func(button, btn->click.arg);
	}
	args->handled = true;
}

static bool de_gui_button_set_property(de_gui_node_t* n, const char* name, const void* value, size_t data_size)
{
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.pressed_color, name, DE_GUI_BUTTON_PRESSED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.hover_color, name, DE_GUI_BUTTON_HOVERED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.normal_color, name, DE_GUI_BUTTON_NORMAL_COLOR_PROPERTY, value, data_size, n);
	return false;
}

static bool de_gui_button_get_property(de_gui_node_t* n, const char* name, void* value, size_t data_size)
{
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.pressed_color, name, DE_GUI_BUTTON_PRESSED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.hover_color, name, DE_GUI_BUTTON_HOVERED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.normal_color, name, DE_GUI_BUTTON_NORMAL_COLOR_PROPERTY, value, data_size, n);
	return false;
}

static void de_gui_button_init(de_gui_node_t* n)
{
	de_gui_button_t* b = &n->s.button;
	n->min_size = (de_vec2_t) { 20, 20 };
	de_color_set(&b->normal_color, 120, 120, 120, 255);
	de_color_set(&b->pressed_color, 100, 100, 100, 255);
	de_color_set(&b->hover_color, 140, 140, 140, 255);
	b->border = de_gui_node_create(n->gui, DE_GUI_NODE_BORDER);
	de_gui_node_set_color(b->border, &b->normal_color);
	n->mouse_down = de_button_border_mouse_down;
	n->mouse_up = de_button_border_mouse_up;
	n->mouse_enter = de_button_border_mouse_enter;
	n->mouse_leave = de_button_border_mouse_leave;
	b->text = de_gui_node_create(n->gui, DE_GUI_NODE_TEXT);
	de_gui_text_set_vertical_alignment(b->text, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
	de_gui_text_set_horizontal_alignment(b->text, DE_GUI_HORIZONTAL_ALIGNMENT_CENTER);
	de_gui_node_set_hit_test_visible(b->text, false);
	de_gui_node_set_color_rgba(b->text, 220, 220, 220, 255);
	de_gui_border_set_stroke_color_rgba(b->border, 80, 80, 80, 255);
	de_gui_node_attach(b->border, n);
	de_gui_node_attach(b->text, n);
}

de_gui_node_dispatch_table_t* de_gui_button_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_button_init,
		.set_property = de_gui_button_set_property,
		.get_property = de_gui_button_get_property,
		.apply_descriptor = de_button_apply_descriptor,
	};
	return &dispatch_table;
}

void de_gui_button_set_click(de_gui_node_t* node, de_gui_callback_func_t click, void* arg)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	node->s.button.click.func = click;
	node->s.button.click.arg = arg;
}

de_gui_node_t* de_gui_button_get_text(de_gui_node_t* node)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	return node->s.button.text;
}

de_gui_node_t* de_gui_button_get_border(de_gui_node_t* node)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	return node->s.button.text;
}

void de_gui_button_set_text(de_gui_node_t* node, const char* txt)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BUTTON);
	de_gui_text_set_text_utf8(node->s.button.text, txt);
}