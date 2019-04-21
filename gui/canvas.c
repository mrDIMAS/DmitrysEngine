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

static void de_gui_canvas_perform_layout(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_CANVAS);
	DE_UNUSED(n);
}

de_vec2_t de_gui_canvas_measure_override(de_gui_node_t* n, const de_vec2_t* available_size)
{
	DE_UNUSED(available_size);

	const de_vec2_t size_for_child = { INFINITY, INFINITY };
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_measure(child, &size_for_child);
	}

	return (de_vec2_t) { 0, 0 };
}

de_vec2_t de_gui_canvas_arrange_override(de_gui_node_t* n, const de_vec2_t* final_size)
{
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		const de_rectf_t rect = { child->desired_local_position.x, child->desired_local_position.y, child->desired_size.x, child->desired_size.y };
		de_gui_node_arrange(child, &rect);
	}

	return *final_size;
}

static void de_gui_canvas_init(de_gui_node_t* n)
{
	DE_UNUSED(n);
}

de_gui_dispatch_table_t* de_gui_canvas_get_dispatch_table()
{
	static de_gui_dispatch_table_t dispatch_table = {
		.init = de_gui_canvas_init,
		.measure_override = de_gui_canvas_measure_override,
		.arrange_override = de_gui_canvas_arrange_override,
	};
	return &dispatch_table;
}
