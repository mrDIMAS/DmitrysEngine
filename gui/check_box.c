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

static void de_gui_check_box_apply_descriptor(de_gui_node_t* n, const de_gui_node_descriptor_t* desc)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CHECK_BOX);
	de_gui_check_box_set_value(n, desc->s.check_box.checked);
}

static void de_gui_check_box_mouse_down(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	DE_UNUSED(args);
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CHECK_BOX);
	de_gui_check_box_t* cb = &n->s.check_box;
	de_gui_check_box_set_value(n, !cb->checked);
}

static void de_gui_check_box_init(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CHECK_BOX);
	de_gui_check_box_t* cb = &n->s.check_box;
	n->mouse_down = de_gui_check_box_mouse_down;

	cb->border = de_gui_node_create_with_desc(n->gui, DE_GUI_NODE_BORDER, &(de_gui_node_descriptor_t){
		.parent = n, .s.border = (de_gui_border_descriptor_t)
		{
			.thickness = { 2, 2, 2, 2 }, .stroke_color = { 80, 80, 80, 255 }
		},
			.color = (de_color_t) { .r = 120, .g = 120, .b = 120, .a = 255 },
	});

	cb->check_mark = de_gui_node_create_with_desc(n->gui, DE_GUI_NODE_BORDER, &(de_gui_node_descriptor_t){
		.margin = (de_gui_thickness_t) { .left = 4, .right = 4, .top = 4, .bottom = 4 },
			.parent = n,
			.visibility = DE_GUI_NODE_VISIBILITY_COLLAPSED,
			.color = { 200, 200, 200, 255 }
	});
}

struct de_gui_node_dispatch_table_t* de_gui_check_box_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t table = {
		.init = de_gui_check_box_init,
		.apply_descriptor = de_gui_check_box_apply_descriptor,
	};
	return &table;
}

void de_gui_check_box_set_value(de_gui_node_t* n, bool value)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CHECK_BOX);
	de_gui_check_box_t* cb = &n->s.check_box;
	cb->checked = value;
	de_gui_node_set_visibility(cb->check_mark, cb->checked ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
}

bool de_gui_check_box_get_value(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CHECK_BOX);
	de_gui_check_box_t* cb = &n->s.check_box;
	return cb->checked;
}