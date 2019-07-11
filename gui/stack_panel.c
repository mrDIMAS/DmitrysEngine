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

static void de_gui_stack_panel_apply_descriptor(de_gui_node_t* n, const de_gui_node_descriptor_t* desc)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);
	de_gui_stack_panel_t* sp = &n->s.stack_panel;
	const de_gui_stack_panel_descriptor_t* spdesc = &desc->s.stack_panel;
	sp->orientation = spdesc->orientation;
}

static void de_gui_stack_panel_init(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);
	de_gui_stack_panel_t* sp = &n->s.stack_panel;
	sp->orientation = DE_GUI_ORIENTATION_VERTICAL;
}

static de_vec2_t de_gui_stack_panel_measure_override(de_gui_node_t* n, const de_vec2_t* available_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);

	de_gui_stack_panel_t* sp = &n->s.stack_panel;

	de_vec2_t child_constraint = { INFINITY, INFINITY };

	if (sp->orientation == DE_GUI_ORIENTATION_VERTICAL) {
		child_constraint.x = available_size->x;

		if (!isnan(n->width)) {
			child_constraint.x = n->width;
		}

		if (child_constraint.x < n->min_size.x) {
			child_constraint.x = n->min_size.x;
		}
		if (child_constraint.x > n->max_size.x) {
			child_constraint.x = n->max_size.x;
		}
	} else {
		child_constraint.y = available_size->y;

		if (!isnan(n->height)) {
			child_constraint.y = n->height;
		}

		if (child_constraint.y < n->min_size.y) {
			child_constraint.y = n->min_size.y;
		}
		if (child_constraint.y > n->max_size.y) {
			child_constraint.y = n->max_size.y;
		}
	}

	de_vec2_t measured_size = { 0, 0 };

	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_measure(child, &child_constraint);

		if (sp->orientation == DE_GUI_ORIENTATION_VERTICAL) {
			if (child->desired_size.x > measured_size.x) {
				measured_size.x = child->desired_size.x;
			}
			measured_size.y += child->desired_size.y;
		} else {
			measured_size.x += child->desired_size.x;
			if (child->desired_size.y > measured_size.y) {
				measured_size.y = child->desired_size.y;
			}
		}
	}

	return measured_size;
}

static de_vec2_t de_gui_stack_panel_arrange_override(de_gui_node_t* n, const de_vec2_t* final_size)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);

	de_gui_stack_panel_t* sp = &n->s.stack_panel;

	float width = final_size->x;
	float height = final_size->y;

	if (sp->orientation == DE_GUI_ORIENTATION_VERTICAL) {
		height = 0.0;
	} else {
		width = 0.0;
	}

	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];

		if (sp->orientation == DE_GUI_ORIENTATION_VERTICAL) {
			const de_rectf_t child_bounds = {
				.x = 0.0,
				.y = height,
				.w = de_maxf(width, child->desired_size.x),
				.h = child->desired_size.y
			};
			de_gui_node_arrange(child, &child_bounds);
			width = de_maxf(width, child->desired_size.x);
			height += child->desired_size.y;
		} else {
			const de_rectf_t child_bounds = {
				.x = width,
				.y = 0.0f,
				.w = child->desired_size.x,
				.h = de_maxf(height, child->desired_size.y)				
			};
			de_gui_node_arrange(child, &child_bounds);
			width += child->desired_size.x;
			height = de_maxf(height, child->desired_size.y);
		}
	}

	if (sp->orientation == DE_GUI_ORIENTATION_VERTICAL) {
		height = de_maxf(height, final_size->y);
	} else {
		width = de_maxf(width, final_size->x);
	}

	return (de_vec2_t) { width, height };
}

void de_gui_stack_panel_set_orientation(de_gui_node_t* n, de_gui_orientation_t orientation)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);
	de_gui_stack_panel_t* sp = &n->s.stack_panel;
	sp->orientation = orientation;
}

de_gui_orientation_t de_gui_stack_panel_get_orientation(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_STACK_PANEL);
	de_gui_stack_panel_t* sp = &n->s.stack_panel;
	return sp->orientation;
}

de_gui_node_dispatch_table_t* de_gui_stack_panel_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t table = {
		.init = de_gui_stack_panel_init,
		.measure_override = de_gui_stack_panel_measure_override,
		.arrange_override = de_gui_stack_panel_arrange_override,
		.apply_descriptor = de_gui_stack_panel_apply_descriptor
	};
	return &table;
}