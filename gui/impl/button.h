/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

/*=======================================================================================*/
static void de_button_border_mouse_down(de_gui_node_t* border, de_gui_routed_event_args_t* args)
{
	de_gui_button_t* btn;
	DE_ASSERT_NODE_TYPE(border, DE_GUI_NODE_BORDER);
	btn = &border->parent->s.button;
	border->color = btn->pressed_color;
	btn->was_pressed = DE_TRUE;
	args->handled = DE_TRUE;
}

/*=======================================================================================*/
static void de_button_border_mouse_enter(de_gui_node_t* border, de_gui_routed_event_args_t* args)
{
	de_gui_button_t* btn;
	DE_ASSERT_NODE_TYPE(border, DE_GUI_NODE_BORDER);
	btn = &border->parent->s.button;
	border->color = btn->hover_color;
	args->handled = DE_TRUE;
}

/*=======================================================================================*/
static void de_button_border_mouse_leave(de_gui_node_t* border, de_gui_routed_event_args_t* args)
{
	de_gui_button_t* btn;
	DE_ASSERT_NODE_TYPE(border, DE_GUI_NODE_BORDER);
	btn = &border->parent->s.button;
	border->color = btn->normal_color;
	args->handled = DE_TRUE;
}

/*=======================================================================================*/
static void de_button_border_mouse_up(de_gui_node_t* border, de_gui_routed_event_args_t* args)
{
	de_gui_button_t* btn;
	DE_ASSERT_NODE_TYPE(border, DE_GUI_NODE_BORDER);
	btn = &border->parent->s.button;
	if (border->is_mouse_over)
	{
		border->color = btn->hover_color;
	}
	else
	{
		border->color = btn->normal_color;
	}
	if (btn->click && btn->was_pressed)
	{
		btn->click(border);
	}
	args->handled = DE_TRUE;
}

/*=======================================================================================*/
static void de_gui_button_deinit(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_BUTTON);

	DE_UNUSED(n);
}

/*=======================================================================================*/
static de_bool_t de_gui_button_set_property(de_gui_node_t* n, const char* name, const void* value, size_t data_size)
{
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.pressed_color, name, DE_GUI_BUTTON_PRESSED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.hover_color, name, DE_GUI_BUTTON_HOVERED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, s.button.normal_color, name, DE_GUI_BUTTON_NORMAL_COLOR_PROPERTY, value, data_size, n);
	return DE_FALSE;
}

/*=======================================================================================*/
static de_bool_t de_gui_button_get_property(de_gui_node_t* n, const char* name, void* value, size_t data_size)
{
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.pressed_color, name, DE_GUI_BUTTON_PRESSED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.hover_color, name, DE_GUI_BUTTON_HOVERED_COLOR_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_GETTER(de_gui_node_t, s.button.normal_color, name, DE_GUI_BUTTON_NORMAL_COLOR_PROPERTY, value, data_size, n);
	return DE_FALSE;
}

/*=======================================================================================*/
de_gui_node_t* de_gui_button_create(de_gui_t* gui)
{
	de_gui_node_t* n;
	de_gui_button_t* b;
	
	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;
		if (!init)
		{
			dispatch_table.deinit = de_gui_button_deinit;
			dispatch_table.set_property = de_gui_button_set_property;
			dispatch_table.get_property = de_gui_button_get_property;
			init = DE_TRUE;
		}
	}

	n = de_gui_node_alloc(gui, DE_GUI_NODE_BUTTON, &dispatch_table);
	b = &n->s.button;
	de_color_set(&b->normal_color, 120, 120, 120, 255);
	de_color_set(&b->pressed_color, 100, 100, 100, 255);
	de_color_set(&b->hover_color, 140, 140, 140, 255);
	b->border = de_gui_border_create(gui);
	de_gui_node_set_color(b->border, &b->normal_color);
	b->border->mouse_down = de_button_border_mouse_down;
	b->border->mouse_up = de_button_border_mouse_up;
	b->border->mouse_enter = de_button_border_mouse_enter;
	b->border->mouse_leave = de_button_border_mouse_leave;
	b->text = de_gui_text_create(gui);
	de_gui_node_set_color_rgba(b->text, 220, 220, 220, 255);
	de_gui_border_set_stroke_color_rgba(b->border, 80, 80, 80, 255);
	de_gui_node_attach(b->border, n);
	de_gui_node_attach(b->text, n);
	return n;
}

/*=======================================================================================*/
void de_gui_button_set_click(de_gui_node_t* node, void(*on_click)(de_gui_node_t*))
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_BUTTON);

	node->s.button.click = on_click;
}

/*=======================================================================================*/
de_gui_node_t* de_gui_button_get_text(de_gui_node_t* node)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_BUTTON);

	return node->s.button.text;
}

/*=======================================================================================*/
de_gui_node_t* de_gui_button_get_border(de_gui_node_t* node)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_BUTTON);

	return node->s.button.text;
}