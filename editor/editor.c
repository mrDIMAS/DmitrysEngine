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

/* Postponed */

static void de_editor_create_property_window(de_editor_t* ed) {
	de_gui_t* gui = ed->core->gui;
	de_editor_property_window_t* pw = &ed->prop;
	pw->owner = ed;
	pw->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);

	pw->grid = de_gui_node_create(gui, DE_GUI_NODE_GRID);
	de_gui_grid_add_column(pw->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_column(pw->grid, 0, DE_GUI_SIZE_MODE_STRETCH);
	de_gui_grid_add_row(pw->grid, 30, DE_GUI_SIZE_MODE_STRICT);

	pw->name_label = de_gui_node_create(gui, DE_GUI_NODE_TEXT);
	de_gui_text_set_text_utf8(pw->name_label, "Name");
	de_gui_node_set_row(pw->name_label, 0);
	de_gui_node_set_column(pw->name_label, 0);
	de_gui_node_attach(pw->name_label, pw->grid);

	pw->name_text = de_gui_node_create(gui, DE_GUI_NODE_TEXT_BOX);
	de_gui_node_attach(pw->name_text, pw->grid);
	de_gui_node_set_row(pw->name_text, 0);
	de_gui_node_set_column(pw->name_text, 1);
}

static void de_editor_free_property_window(de_editor_t* ed) {
	de_editor_property_window_t* pw = &ed->prop;
	de_gui_node_free(pw->window);
}

static void de_editor_setup_camera_controller(de_editor_t* ed) {
	de_editor_camera_controller_t* ctl = &ed->camera_ctrl;

	ctl->camera_pivot = de_node_create(ed->scene, DE_NODE_TYPE_BASE);

	ctl->camera = de_node_create(ed->scene, DE_NODE_TYPE_CAMERA);
	de_node_attach(ctl->camera, ctl->camera_pivot);
}

static void de_editor_setup_scene(de_editor_t* ed) {
	ed->scene = de_scene_create(ed->core);	
}

de_editor_t* de_editor_create(de_core_t* core) {
	de_editor_t* ed = DE_NEW(de_editor_t);
	ed->core = core;
	de_editor_create_property_window(ed);
    de_editor_setup_scene(ed);
    de_editor_setup_camera_controller(ed);
	return ed;
}

void de_editor_free(de_editor_t* ed) {
	de_editor_free_property_window(ed);
	de_free(ed);
}

void de_editor_process_event(de_editor_t* ed, const de_event_t* evt) {
	de_editor_camera_controller_t* ctl = &ed->camera_ctrl;
	switch(evt->type) {
		case DE_EVENT_TYPE_KEY_DOWN:
			switch(evt->s.key.key) {
				case DE_KEY_W:
					ctl->move_forward = true;
					break;
				case DE_KEY_S:
					ctl->move_backward = true;
					break;
				case DE_KEY_A:
					ctl->move_left = true;
					break;
				case DE_KEY_D:
					ctl->move_right = true;
					break;
                default:
                    break;
			}
			break;
		case DE_EVENT_TYPE_KEY_UP:
			switch (evt->s.key.key) {
				case DE_KEY_W:
					ctl->move_forward = false;
					break;
				case DE_KEY_S:
					ctl->move_backward = false;
					break;
				case DE_KEY_A:
					ctl->move_left = false;
					break;
				case DE_KEY_D:
					ctl->move_right = false;
					break;
                default:
                    break;
			}
			break;
		case DE_EVENT_TYPE_MOUSE_MOVE:
			break;
        default:
            break;
	}
}

void de_editor_update(de_editor_t* ed) {
	de_editor_camera_controller_t* ctl = &ed->camera_ctrl;
	if(ctl->move_forward) {

	}
}