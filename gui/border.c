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

static void de_gui_border_apply_descriptor(de_gui_node_t* n, const de_gui_node_descriptor_t* desc)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);
	de_gui_border_t* b = &n->s.border;
	const de_gui_border_descriptor_t* bdesc = &desc->s.border;
	b->stroke_color = bdesc->stroke_color;
	b->thickness = bdesc->thickness;
}

static void de_gui_border_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);
	DE_UNUSED(nesting);

	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_border_t* b = &n->s.border;
	const de_vec2_t tex_coords[4] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &n->color, tex_coords);
	de_gui_draw_list_push_rect_vary(dl, scr_pos, &n->actual_size, &b->thickness, &b->stroke_color);

	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
}

static de_vec2_t de_gui_border_measure_override(de_gui_node_t* n, const de_vec2_t* available_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);

	de_gui_border_t* b = &n->s.border;

	const float margin_x = b->thickness.left + b->thickness.right;
	const float margin_y = b->thickness.top + b->thickness.bottom;

	const de_vec2_t size_for_child = {
		available_size->x - margin_x,
		available_size->y - margin_y,
	};
	de_vec2_t desired_size = { 0, 0 };
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
	desired_size.x += margin_x;
	desired_size.y += margin_y;
	return desired_size;
}

static de_vec2_t de_gui_border_arrange_override(de_gui_node_t* n, const de_vec2_t* final_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);

	de_gui_border_t* b = &n->s.border;

	const de_rectf_t rect_for_child = {
		b->thickness.left, b->thickness.top,
		final_size->x - (b->thickness.right + b->thickness.left),
		final_size->y - (b->thickness.bottom + b->thickness.top),
	};
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_arrange(child, &rect_for_child);
	}
	return *final_size;
}

static void de_gui_border_init(de_gui_node_t* n)
{
	de_gui_border_t* b = &n->s.border;
	de_gui_border_set_thickness_uniform(n, 1);
	de_color_set(&b->stroke_color, 150, 150, 150, 255);
}

de_gui_node_dispatch_table_t* de_gui_border_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.init = de_gui_border_init,
		.render = de_gui_border_render,
		.measure_override = de_gui_border_measure_override,
		.arrange_override = de_gui_border_arrange_override,
		.apply_descriptor = de_gui_border_apply_descriptor,
	};
	return &dispatch_table;
}

void de_gui_border_set_stroke_color(de_gui_node_t* node, const de_color_t* color)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.stroke_color = *color;
}

void de_gui_border_set_stroke_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.stroke_color.r = r;
	node->s.border.stroke_color.g = g;
	node->s.border.stroke_color.b = b;
	node->s.border.stroke_color.a = a;
}

void de_gui_border_set_thickness_uniform(de_gui_node_t* node, float thickness)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.thickness.left = node->s.border.thickness.top = node->s.border.thickness.right = node->s.border.thickness.bottom = (float)fabs(thickness);
}

void de_gui_border_set_thickness(de_gui_node_t* node, float left, float top, float right, float bottom)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.thickness.left = (float)fabs(left);
	node->s.border.thickness.top = (float)fabs(top);
	node->s.border.thickness.right = (float)fabs(right);
	node->s.border.thickness.bottom = (float)fabs(bottom);
}