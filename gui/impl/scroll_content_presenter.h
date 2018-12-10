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
static void de_gui_scroll_content_presenter_deinit(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);

	DE_UNUSED(n);
}

/*=======================================================================================*/
static void de_gui_scroll_content_presenter_perform_layout(de_gui_node_t* n)
{
	size_t i;

	de_gui_scroll_content_presenter_t* scp = &n->s.scroll_content_presenter;

	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);

	for (i = 0; i < n->children.size; ++i)
	{
		de_gui_node_t* child = n->children.data[i];

		/* apply standard alignment */
		de_gui_node_default_layout(child);

		/* explicitly set position */
		de_gui_node_set_desired_local_position(child, -scp->scroll.x, -scp->scroll.y);
	}
}

/*=======================================================================================*/
de_gui_node_t* de_gui_scroll_content_presenter_create(void)
{
	de_gui_node_t* n;
	de_gui_scroll_content_presenter_t* scp;
	
	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;

		if (!init)
		{
			dispatch_table.deinit = de_gui_scroll_content_presenter_deinit;
			dispatch_table.layout_children = de_gui_scroll_content_presenter_perform_layout;

			init = DE_TRUE;
		}
	}
	
	n = de_gui_node_alloc(DE_GUI_NODE_SCROLL_CONTENT_PRESENTER, &dispatch_table);

	scp = &n->s.scroll_content_presenter;
	scp->scroll.x = 0;
	scp->scroll.y = 0;

	return n;
}

/*=======================================================================================*/
void de_gui_scroll_content_presenter_set_v_scroll(de_gui_node_t* node, float val)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);

	node->s.scroll_content_presenter.scroll.y = val;
}

/*=======================================================================================*/
void de_gui_scroll_content_presenter_set_h_scroll(de_gui_node_t* node, float val)
{
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_SCROLL_CONTENT_PRESENTER);

	node->s.scroll_content_presenter.scroll.x = val;
}