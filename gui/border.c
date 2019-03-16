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

static void de_gui_border_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting) {
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_border_t* b = &n->s.border;
	de_vec2_t tex_coords[4] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_BORDER);

	DE_UNUSED(nesting);

	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &n->color, tex_coords);
	de_gui_draw_list_push_rect_vary(dl, scr_pos, &n->actual_size, &b->thickness, &b->stroke_color);

	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
}

static void de_gui_border_layout_children(de_gui_node_t* n) {
	size_t i;
	de_gui_border_t* border = &n->s.border;
	de_vec2_t inner_size;

	inner_size.x = n->actual_size.x - (border->thickness.left + border->thickness.right);
	inner_size.y = n->actual_size.y - (border->thickness.top + border->thickness.bottom);

	for (i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		float desired_x = 0;
		float desired_y = 0;
		float desired_width = child->desired_size.x;
		float desired_height = child->desired_size.y;

		/* apply vertical alignment */
		switch (child->vertical_alignment) {
			case DE_GUI_VERTICAL_ALIGNMENT_TOP:
			{
				desired_y = 0;
				break;
			}
			case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
			{
				desired_y = (inner_size.y - child->actual_size.y) * 0.5f;
				break;
			}
			case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
			{
				desired_y = inner_size.y - child->actual_size.y;
				break;
			}
			case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
			{
				desired_height = inner_size.y;
				break;
			}
		}

		/* apply horizontal alignment */
		switch (child->horizontal_alignment) {
			case DE_GUI_HORIZONTAL_ALIGNMENT_LEFT:
			{
				desired_x = 0;
				break;
			}
			case DE_GUI_HORIZONTAL_ALIGNMENT_CENTER:
			{
				desired_x = (inner_size.x - child->actual_size.x) * 0.5f;
				break;
			}
			case DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT:
			{
				desired_x = inner_size.x - child->actual_size.x;
				break;
			}
			case DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH:
			{
				desired_width = inner_size.x;
				break;
			}
		}

		desired_x += border->thickness.left;
		desired_y += border->thickness.top;

		de_gui_node_set_desired_local_position(child, desired_x, desired_y);
		de_gui_node_set_desired_size(child, desired_width, desired_height);
	}
}

static void de_gui_border_init(de_gui_node_t* n) {
	de_gui_border_t* b = &n->s.border;
	de_gui_border_set_thickness_uniform(n, 1);
	de_color_set(&b->stroke_color, 150, 150, 150, 255);
}

de_gui_dispatch_table_t* de_gui_border_get_dispatch_table() {
	static de_gui_dispatch_table_t dispatch_table = {
		.init = de_gui_border_init,
		.render = de_gui_border_render,
		.layout_children = de_gui_border_layout_children,
	};
	return &dispatch_table;
}

void de_gui_border_set_stroke_color(de_gui_node_t* node, const de_color_t* color) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.stroke_color = *color;
}

void de_gui_border_set_stroke_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.stroke_color.r = r;
	node->s.border.stroke_color.g = g;
	node->s.border.stroke_color.b = b;
	node->s.border.stroke_color.a = a;
}

void de_gui_border_set_thickness_uniform(de_gui_node_t* node, float thickness) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.thickness.left = node->s.border.thickness.top = node->s.border.thickness.right = node->s.border.thickness.bottom = (float)fabs(thickness);
}

void de_gui_border_set_thickness(de_gui_node_t* node, float left, float top, float right, float bottom) {
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_BORDER);
	node->s.border.thickness.left = (float)fabs(left);
	node->s.border.thickness.top = (float)fabs(top);
	node->s.border.thickness.right = (float)fabs(right);
	node->s.border.thickness.bottom = (float)fabs(bottom);
}