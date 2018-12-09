/*=======================================================================================*/
static void de_gui_canvas_perform_layout(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_CANVAS);

	DE_UNUSED(n);
}

/*=======================================================================================*/
static void de_gui_canvas_deinit(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_CANVAS);

	DE_UNUSED(n);
}

/*=======================================================================================*/
de_gui_node_t* de_gui_canvas_create(void)
{
	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;

		if (!init)
		{
			dispatch_table.deinit = de_gui_canvas_deinit;
			dispatch_table.layout_children = de_gui_canvas_perform_layout;

			init = DE_TRUE;
		}
	}

	de_gui_node_t* n = de_gui_node_alloc(DE_GUI_NODE_CANVAS, &dispatch_table);

	return n;
}