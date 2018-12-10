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
static void de_gui_grid_perform_layout(de_gui_node_t* n)
{
	size_t i;
	de_gui_grid_t* grid = &n->s.grid;

	float preset_width = 0;
	float preset_height = 0;

	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_GRID);

	/* calculate size of strict-sized and auto-sized rows and columns */
	for (i = 0; i < grid->columns.size; ++i)
	{
		de_gui_grid_column_t* col = grid->columns.data + i;
		if (col->size_mode == DE_GUI_SIZE_MODE_STRICT)
		{
			col->actual_width = col->desired_width;
			preset_width += col->actual_width;
		}
		else if (col->size_mode == DE_GUI_SIZE_MODE_AUTO)
		{
			size_t child_index = de_gui_grid_find_child_on_column(n, 0, i);
			if (child_index < n->children.size)
			{
				if (n->children.data[child_index]->visibility == DE_GUI_NODE_VISIBILITY_COLLAPSED)
				{
					col->actual_width = 0;
				}
				else
				{
					while (child_index < n->children.size)
					{
						de_gui_node_t* node_on_col = n->children.data[child_index];
						float desired_width;
						desired_width = node_on_col->actual_size.x;
						if (desired_width > col->actual_width)
						{
							col->actual_width = desired_width;
						}
						child_index = de_gui_grid_find_child_on_column(n, child_index + 1, i);
					}
				}
			}
			else
			{
				col->actual_width = col->desired_width;
			}
			preset_width += col->actual_width;
		}
	}
	for (i = 0; i < grid->rows.size; ++i)
	{
		de_gui_grid_row_t* row = grid->rows.data + i;
		if (row->size_mode == DE_GUI_SIZE_MODE_STRICT)
		{
			row->actual_height = row->desired_height;
			preset_height += row->actual_height;
		}
		else if (row->size_mode == DE_GUI_SIZE_MODE_AUTO)
		{
			size_t child_index = de_gui_grid_find_child_on_row(n, 0, i);
			if (child_index < n->children.size)
			{
				if (n->children.data[child_index]->visibility == DE_GUI_NODE_VISIBILITY_COLLAPSED)
				{
					row->actual_height = 0;
				}
				else
				{
					while (child_index < n->children.size)
					{
						de_gui_node_t* node_on_row = n->children.data[child_index];
						float desired_height;
						desired_height = node_on_row->actual_size.y;
						if (desired_height > row->actual_height)
						{
							row->actual_height = desired_height;
						}
						child_index = de_gui_grid_find_child_on_row(n, child_index + 1, i);
					}
				}
			}
			else
			{
				row->actual_height = row->desired_height;
			}
			preset_height += row->actual_height;
		}
	}

	/* then fit stretch-sized rows and columns */
	{
		size_t stretch_sized_columns = 0;
		float rest_width = n->actual_size.x - preset_width;
		float width_per_col;
		/* count columns first */
		for (i = 0; i < grid->columns.size; ++i)
		{
			if (grid->columns.data[i].size_mode == DE_GUI_SIZE_MODE_STRETCH)
			{
				++stretch_sized_columns;
			}
		}
		if (stretch_sized_columns)
		{
			width_per_col = rest_width / stretch_sized_columns;
			for (i = 0; i < grid->columns.size; ++i)
			{
				de_gui_grid_column_t* col = grid->columns.data + i;
				if (col->size_mode == DE_GUI_SIZE_MODE_STRETCH)
				{
					col->actual_width = width_per_col;
				}
			}
		}
	}
	{
		size_t stretch_sized_rows = 0;
		float height_per_row;
		float rest_height = n->actual_size.y - preset_height;
		/* count rows first */
		for (i = 0; i < grid->rows.size; ++i)
		{
			if (grid->rows.data[i].size_mode == DE_GUI_SIZE_MODE_STRETCH)
			{
				++stretch_sized_rows;
			}
		}
		if (stretch_sized_rows)
		{
			height_per_row = rest_height / stretch_sized_rows;
			for (i = 0; i < grid->rows.size; ++i)
			{
				de_gui_grid_row_t* row = grid->rows.data + i;
				if (row->size_mode == DE_GUI_SIZE_MODE_STRETCH)
				{
					row->actual_height = height_per_row;
				}
			}
		}
	}

	/* finally compute positions of each row and column */
	{
		float y = 0;
		for (i = 0; i < grid->rows.size; ++i)
		{
			de_gui_grid_row_t* row = grid->rows.data + i;
			row->y = y;
			y += row->actual_height;
		}
	}
	{
		float x = 0;
		for (i = 0; i < grid->columns.size; ++i)
		{
			de_gui_grid_column_t* col = grid->columns.data + i;
			col->x = x;
			x += col->actual_width;
		}
	}

	/* now arrange children */
	for (i = 0; i < n->children.size; ++i)
	{
		de_gui_node_t* child = n->children.data[i];

		if (grid->rows.size > 0 && grid->columns.size > 0)
		{
			de_gui_grid_column_t* col;
			de_gui_grid_row_t* row;
			de_vec2_t new_size;

			new_size.x = child->desired_size.x;
			new_size.y = child->desired_size.y;

			/* find row */
			if (child->row < grid->rows.size)
			{
				row = grid->rows.data + child->row;
			}
			else
			{
				row = grid->rows.data + 0;
			}

			/* find column */
			if (child->column < grid->columns.size)
			{
				col = grid->columns.data + child->column;
			}
			else
			{
				col = grid->columns.data + 0;
			}

			de_gui_node_set_desired_local_position(child, col->x, row->y);

			if (col->size_mode == DE_GUI_SIZE_MODE_STRETCH || col->size_mode == DE_GUI_SIZE_MODE_STRICT)
			{
				new_size.x = col->actual_width;
			}
			if (row->size_mode == DE_GUI_SIZE_MODE_STRETCH || row->size_mode == DE_GUI_SIZE_MODE_STRICT)
			{
				new_size.y = row->actual_height;
			}

			if (child->vertical_alignment == DE_GUI_VERTICAL_ALIGNMENT_STRETCH)
			{
				de_gui_node_set_desired_size(child, child->desired_size.x, new_size.y);
			}
			if (child->horizontal_alignment == DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH)
			{
				de_gui_node_set_desired_size(child, new_size.x, child->desired_size.y);
			}
		}
		else
		{
			/* no rows and columns, arrange and stretch to size of grid */
			de_gui_node_set_desired_local_position(child, 0, 0);

			if (child->vertical_alignment == DE_GUI_VERTICAL_ALIGNMENT_STRETCH)
			{
				de_gui_node_set_desired_size(child, child->desired_size.x, n->actual_size.y);
			}
			if (child->horizontal_alignment == DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH)
			{
				de_gui_node_set_desired_size(child, n->actual_size.x, child->desired_size.y);
			}
		}
	}
}

/*=======================================================================================*/
static void de_gui_grid_deinit(de_gui_node_t* n)
{
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_GRID);

	DE_ARRAY_FREE(n->s.grid.rows);
	DE_ARRAY_FREE(n->s.grid.columns);
}

/*=======================================================================================*/
static void de_gui_grid_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	size_t i;
	const de_gui_grid_t* grid = &n->s.grid;
	const de_vec2_t* scr_pos = &n->screen_position;

	DE_UNUSED(nesting);
	DE_ASSERT_NODE_TYPE(n, DE_GUI_NODE_GRID);

	if (grid->draw_borders)
	{
		de_vec2_t left_top;
		de_vec2_t right_top;
		de_vec2_t right_bottom;
		de_vec2_t left_bottom;

		left_top.x = scr_pos->x;
		left_top.y = scr_pos->y;

		right_top.x = scr_pos->x + n->actual_size.x;
		right_top.y = scr_pos->y;

		right_bottom.x = scr_pos->x + n->actual_size.x;
		right_bottom.y = scr_pos->y + n->actual_size.y;

		left_bottom.x = scr_pos->x;
		left_bottom.y = scr_pos->y + n->actual_size.y;

		/* draw border first */
		de_gui_draw_list_push_line(dl, &left_top, &right_top, 1, &n->color);
		de_gui_draw_list_push_line(dl, &right_top, &right_bottom, 1, &n->color);
		de_gui_draw_list_push_line(dl, &right_bottom, &left_bottom, 1, &n->color);
		de_gui_draw_list_push_line(dl, &left_bottom, &left_top, 1, &n->color);

		/* then draw each column and row */
		for (i = 0; i < grid->columns.size; ++i)
		{
			float x = grid->columns.data[i].x;
			de_vec2_t a, b;

			a.x = scr_pos->x + x;
			a.y = scr_pos->y;

			b.x = scr_pos->x + x;
			b.y = scr_pos->y + n->actual_size.y;

			de_gui_draw_list_push_line(dl, &a, &b, 1, &n->color);
		}
		for (i = 0; i < grid->rows.size; ++i)
		{
			float y = grid->rows.data[i].y;
			de_vec2_t a, b;

			a.x = scr_pos->x;
			a.y = scr_pos->y + y;

			b.x = scr_pos->x + n->actual_size.x;
			b.y = scr_pos->y + y;

			de_gui_draw_list_push_line(dl, &a, &b, 1, &n->color);
		}
	}

	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, n);
}


/*=======================================================================================*/
de_gui_node_t* de_gui_grid_create()
{
	de_gui_node_t* n;
	de_gui_grid_t* grid;
	
	static de_gui_dispatch_table_t dispatch_table;
	{
		static de_bool_t init = DE_FALSE;
		if (!init)
		{
			dispatch_table.deinit = de_gui_grid_deinit;
			dispatch_table.layout_children = de_gui_grid_perform_layout;
			dispatch_table.render = de_gui_grid_render;
			init = DE_TRUE;
		}
	}
	
	n = de_gui_node_alloc(DE_GUI_NODE_GRID, &dispatch_table);

	grid = &n->s.grid;
	grid->draw_borders = DE_FALSE;

	return n;
}

/*=======================================================================================*/
size_t de_gui_grid_find_child_on_column(de_gui_node_t* grid, size_t prev_child, size_t column)
{
	size_t i;
	DE_ASSERT_NODE_TYPE(grid, DE_GUI_NODE_GRID);
	for (i = prev_child; i < grid->children.size; ++i)
	{
		de_gui_node_t* child = grid->children.data[i];
		if (child->column == column)
		{
			break;
		}
	}
	return i;
}

/*=======================================================================================*/
size_t de_gui_grid_find_child_on_row(de_gui_node_t* grid, size_t prev_child, size_t row)
{
	size_t i;
	DE_ASSERT_NODE_TYPE(grid, DE_GUI_NODE_GRID);
	for (i = prev_child; i < grid->children.size; ++i)
	{
		de_gui_node_t* child = grid->children.data[i];
		if (child->row == row)
		{
			break;
		}
	}
	return i;
}

/*=======================================================================================*/
void de_gui_grid_enable_draw_borders(de_gui_node_t* grid, de_bool_t state)
{
	DE_ASSERT_NODE_TYPE(grid, DE_GUI_NODE_GRID);

	grid->s.grid.draw_borders = state;
}

/*=======================================================================================*/
void de_gui_grid_clear_rows(de_gui_node_t* grid)
{
	DE_ASSERT_NODE_TYPE(grid, DE_GUI_NODE_GRID);
	DE_ARRAY_CLEAR(grid->s.grid.rows);
}

/*=======================================================================================*/
void de_gui_grid_clear_columns(de_gui_node_t* grid)
{
	DE_ASSERT_NODE_TYPE(grid, DE_GUI_NODE_GRID);
	DE_ARRAY_CLEAR(grid->s.grid.columns);
}

/*=======================================================================================*/
void de_gui_grid_add_row(de_gui_node_t* node, float desired_height, de_gui_size_mode_t size_mode)
{
	de_gui_grid_row_t row = { 0 };
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_GRID);
	row.desired_height = desired_height;
	row.size_mode = size_mode;
	DE_ARRAY_APPEND(node->s.grid.rows, row);
}

/*=======================================================================================*/
void de_gui_grid_add_column(de_gui_node_t* node, float desired_width, de_gui_size_mode_t size_mode)
{
	de_gui_grid_column_t col = { 0 };
	DE_ASSERT_NODE_TYPE(node, DE_GUI_NODE_GRID);
	col.desired_width = desired_width;
	col.size_mode = size_mode;
	DE_ARRAY_APPEND(node->s.grid.columns, col);
}