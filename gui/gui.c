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

/* Forward declarations for controls */
static void de_gui_node_default_layout(de_gui_node_t* n);
static bool de_gui_node_contains_point(de_gui_node_t* node, const de_vec2_t* point);

/* Insert logic of all controls */
#include "gui/draw.c"
#include "gui/button.c"
#include "gui/border.c"
#include "gui/canvas.c"
#include "gui/grid.c"
#include "gui/scroll_bar.c"
#include "gui/scroll_content_presenter.c"
#include "gui/scroll_viewer.c"
#include "gui/text.c"
#include "gui/text_box.c"
#include "gui/window.c"
#include "gui/slide_selector.c"
#include "gui/image.c"

#define DE_DECLARE_ROUTED_EVENT_TRACER(name__, event__) \
	static void name__(de_gui_node_t* n, de_gui_routed_event_args_t* args) { \
		if (n->event__) { \
			n->event__(n, args); \
		} \
		if (n->parent && !args->handled) { \
			name__(n->parent, args); \
		} \
	} 

DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_mouse_down, mouse_down)
DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_mouse_up, mouse_up)
DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_mouse_leave, mouse_leave)
DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_mouse_move, mouse_move)
DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_got_focus, got_focus)
DE_DECLARE_ROUTED_EVENT_TRACER(de_gui_node_route_lost_focus, lost_focus)

#undef DE_DECLARE_ROUTED_EVENT_TRACER

static void de_gui_node_route_mouse_enter(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	if (n->mouse_enter) {
		n->mouse_enter(n, args);
	}
	if (n->parent && !args->handled) {
		de_gui_node_route_mouse_enter(n->parent, args);
	}
}

static void de_gui_node_route_text_entered(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	if (n->text_entered) {
		n->text_entered(n, args);
	}
	if (n->parent && !args->handled) {
		de_gui_node_route_text_entered(n->parent, args);
	}
}

static void de_gui_node_route_key_down(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	if (n->key_down) {
		n->key_down(n, args);
	}
	if (n->parent && !args->handled) {
		de_gui_node_route_key_down(n->parent, args);
	}
}

static void de_gui_node_route_key_up(de_gui_node_t* n, de_gui_routed_event_args_t* args)
{
	if (n->key_up) {
		n->key_up(n, args);
	}
	if (n->parent && !args->handled) {
		de_gui_node_route_key_up(n->parent, args);
	}
}

static void de_gui_node_calculate_actual_size(de_gui_node_t* n)
{
	if (n->visibility == DE_GUI_NODE_VISIBILITY_COLLAPSED) {
		n->actual_size.x = 0;
		n->actual_size.y = 0;
	} else {
		n->actual_size.x = n->desired_size.x - 2 * n->margin.right;
		n->actual_size.y = n->desired_size.y - 2 * n->margin.bottom;
	}
}

static void de_gui_node_apply_layout(de_gui_node_t* n)
{
	n->actual_local_position.x = n->desired_local_position.x + n->margin.left;
	n->actual_local_position.y = n->desired_local_position.y + n->margin.top;

	de_gui_node_calculate_actual_size(n);

	if (n->dispatch_table->layout_children) {
		n->dispatch_table->layout_children(n);
	} else {
		for (size_t i = 0; i < n->children.size; ++i) {
			de_gui_node_default_layout(n->children.data[i]);
		}
	}

	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_apply_layout(n->children.data[i]);
	}
}

void de_gui_align_rect_in_rect(de_vec2_t* child_pos, de_vec2_t* child_size,
	const de_vec2_t* parent_pos, const de_vec2_t* parent_size,
	const de_gui_horizontal_alignment_t horizontal_alignment,
	const de_gui_vertical_alignment_t vertical_alignment)
{

	if (child_size->x > parent_size->x) {
		child_size->x = parent_size->x;
	} else if (child_size->x < 0) { /* check if user gone mad */
		child_size->x = 0;
	}

	if (child_size->y > parent_size->y) {
		child_size->y = parent_size->y;
	} else if (child_size->y < 0) { /* check if user gone mad */
		child_size->y = 0;
	}

	/* apply vertical alignment */
	switch (vertical_alignment) {
		case DE_GUI_VERTICAL_ALIGNMENT_TOP:
			child_pos->y = parent_pos->y;
			break;
		case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
			child_pos->y = parent_pos->y + (parent_size->y - child_size->y) * 0.5f;
			break;
		case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
			child_pos->y = parent_pos->y + (parent_size->y - child_size->y);
			break;
		case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
			child_pos->y = parent_pos->y;
			child_size->y = parent_size->y;
			break;
	}

	/* apply horizontal alignment */
	switch (horizontal_alignment) {
		case DE_GUI_HORIZONTAL_ALIGNMENT_LEFT:
			child_pos->x = parent_pos->x;
			break;
		case DE_GUI_HORIZONTAL_ALIGNMENT_CENTER:
			child_pos->x = parent_pos->x + (parent_size->x - child_size->x) * 0.5f;
			break;
		case DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT:
			child_pos->x = parent_pos->x + (parent_size->x - child_size->x);
			break;
		case DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH:
			child_pos->x = parent_pos->x;
			child_size->x = parent_size->x;
			break;
	}
}

static void de_gui_node_default_layout(de_gui_node_t* n)
{
	de_gui_node_t* parent = n->parent;

	if (!parent) {
		de_fatal_error("node must have parent here!");
	}

	de_vec2_t desired_pos = { 0, 0 };
	de_vec2_t desired_size = n->desired_size;
	de_gui_align_rect_in_rect(&desired_pos, &desired_size, &(de_vec2_t){0, 0},
		&parent->actual_size, n->horizontal_alignment, n->vertical_alignment);

	de_gui_node_set_desired_local_position(n, desired_pos.x, desired_pos.y);
	de_gui_node_set_desired_size(n, desired_size.x, desired_size.y);
}


de_gui_t* de_gui_init(de_core_t* core)
{
	de_gui_t* gui = DE_NEW(de_gui_t);
	gui->core = core;

	DE_LINKED_LIST_INIT(gui->nodes);

	DE_ARRAY_INIT(gui->draw_list.commands);
	DE_ARRAY_INIT(gui->draw_list.vertex_buffer);
	DE_ARRAY_INIT(gui->draw_list.index_buffer);

	gui->tab_width = 4;
	gui->text_buffer_size = 0x7FFF;
	gui->text_buffer = (uint32_t*)de_calloc(gui->text_buffer_size, sizeof(*gui->text_buffer));

	/* create default font */
	{
	#define CHAR_COUNT 255
		int char_set[CHAR_COUNT];
		int i;
		for (i = 0; i < CHAR_COUNT; ++i) {
			char_set[i] = i;
		}
		gui->default_font = de_font_load_ttf_from_memory(core, (void*)de_builtin_font_inconsolata, 18, char_set, CHAR_COUNT);
	#undef CHAR_COUNT
	}

	return gui;
}

void de_gui_shutdown(de_gui_t* gui)
{
/* free nodes */
	while (gui->nodes.head) {
		de_gui_node_free(gui->nodes.head);
	}

	de_font_free(gui->default_font);

	de_free(gui->text_buffer);

	DE_ARRAY_FREE(gui->draw_list.commands);
	DE_ARRAY_FREE(gui->draw_list.vertex_buffer);
	DE_ARRAY_FREE(gui->draw_list.index_buffer);
	DE_ARRAY_FREE(gui->draw_list.clip_cmd_stack);
	DE_ARRAY_FREE(gui->draw_list.opacity_stack);

	de_free(gui);
}

void de_gui_node_update_transform(de_gui_node_t* node)
{
	size_t i;
	if (!node) {
		de_fatal_error("node cannot be null here");
	}
	if (node->parent) {
		de_vec2_add(&node->screen_position, &node->parent->screen_position, &node->actual_local_position);
	} else {
		node->screen_position = node->actual_local_position;
	}
	/* update children recursively */
	for (i = 0; i < node->children.size; ++i) {
		de_gui_node_t* child = node->children.data[i];
		de_gui_node_update_transform(child);
	}
}

static de_gui_node_t* de_gui_node_pick(de_gui_node_t* n, float x, float y, int* level)
{
	size_t i;
	de_gui_node_t* picked = NULL;
	int topmost_picked_level = 0;

	if (!n->is_hit_test_visible) {
		return NULL;
	}

	if (de_gui_node_contains_point(n, &(de_vec2_t){ x, y })) {
		picked = n;
		topmost_picked_level = *level;
	}

	for (i = 0; i < n->children.size; ++i) {
		de_gui_node_t* picked_child;

		++(*level);

		picked_child = de_gui_node_pick(n->children.data[i], x, y, level);

		if (picked_child && (*level) > topmost_picked_level) {
			topmost_picked_level = *level;
			picked = picked_child;
		}
	}

	return picked;
}

static bool de_gui_draw_command_contains_point(const de_gui_draw_list_t* draw_list, const de_gui_draw_command_t* cmd, const de_vec2_t* pos)
{
	size_t j;
	int* indices = draw_list->index_buffer.data;
	de_gui_vertex_t* vertices = draw_list->vertex_buffer.data;
	size_t last = cmd->index_offset + cmd->triangle_count * 3;

	/* check each triangle from command for intersection with mouse pointer */
	for (j = cmd->index_offset; j < last; j += 3) {
		int a = indices[j];
		int b = indices[j + 1];
		int c = indices[j + 2];
		de_gui_vertex_t* va = vertices + a;
		de_gui_vertex_t* vb = vertices + b;
		de_gui_vertex_t* vc = vertices + c;

		/* check if point is in triangle */
		{
			de_vec2_t v0, v1, v2;
			float dot00, dot01, dot02, dot11, dot12;
			float denom, invDenom, u, v;

			de_vec2_sub(&v0, &vc->pos, &va->pos);
			de_vec2_sub(&v1, &vb->pos, &va->pos);
			de_vec2_sub(&v2, pos, &va->pos);

			dot00 = de_vec2_dot(&v0, &v0);
			dot01 = de_vec2_dot(&v0, &v1);
			dot02 = de_vec2_dot(&v0, &v2);
			dot11 = de_vec2_dot(&v1, &v1);
			dot12 = de_vec2_dot(&v1, &v2);

			denom = (dot00 * dot11 - dot01 * dot01);

			if (denom <= FLT_EPSILON) {
				/* we don't want floating-point exceptions */
				return false;
			}

			invDenom = 1.0f / denom;
			u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			if ((u >= 0) && (v >= 0) && (u + v < 1)) {
				return true;
			}
		}
	}

	return false;
}

de_gui_thickness_t de_gui_thickness_uniform(float value)
{
	return (de_gui_thickness_t) { .left = value, .right = value, .top = value, .bottom = value };
}

static bool de_gui_node_is_clipped(de_gui_node_t* node, const de_vec2_t* point)
{
	size_t i;
	de_gui_draw_list_t* draw_list = &node->gui->draw_list;

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		return true;
	}

	bool clipped = true;

	for (i = 0; i < node->geometry.size; ++i) {
		de_gui_draw_command_t* cmd = draw_list->commands.data + node->geometry.data[i];

		if (cmd->type == DE_GUI_DRAW_COMMAND_TYPE_CLIP) {
			if (de_gui_draw_command_contains_point(draw_list, cmd, point)) {
				clipped = false;

				break;
			}
		}
	}

	/* point can be clipped by parent's clipping geometry */
	if (node->parent) {
		if (!clipped) {
			clipped |= de_gui_node_is_clipped(node->parent, point);
		}
	}

	return clipped;
}

static bool de_gui_node_contains_point(de_gui_node_t* node, const de_vec2_t* point)
{
	size_t i;
	de_gui_draw_list_t* draw_list = &node->gui->draw_list;

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		return false;
	}

	if (!de_gui_node_is_clipped(node, point)) {
		for (i = 0; i < node->geometry.size; ++i) {
			de_gui_draw_command_t* cmd = draw_list->commands.data + node->geometry.data[i];

			if (cmd->type == DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY) {
				if (de_gui_draw_command_contains_point(draw_list, cmd, point)) {
					return true;
				}
			}
		}
	}

	return false;
}

static void de_gui_node_fire_mouse_move_event(de_gui_node_t* n, const de_vec2_t* mouse_pos)
{
	de_gui_routed_event_args_t args;
	de_zero(&args, sizeof(args));
	args.source = n;
	args.type = DE_GUI_ROUTED_EVENT_MOUSE_MOVE;
	args.s.mouse_move.pos = *mouse_pos;
	de_gui_node_route_mouse_move(n, &args);
}

static void de_gui_node_fire_mouse_enter_event(de_gui_node_t* n)
{
	de_gui_routed_event_args_t args;
	de_zero(&args, sizeof(args));
	args.source = n;
	args.type = DE_GUI_ROUTED_EVENT_MOUSE_ENTER;
	de_gui_node_route_mouse_enter(n, &args);
}

static void de_gui_node_fire_mouse_up_event(de_gui_node_t* n, const de_vec2_t* mouse_pos)
{
	de_gui_routed_event_args_t evt;
	de_zero(&evt, sizeof(evt));
	evt.source = n;
	evt.type = DE_GUI_ROUTED_EVENT_MOUSE_UP;
	evt.s.mouse_up.pos = *mouse_pos;
	evt.s.mouse_up.button = DE_BUTTON_LEFT;
	de_gui_node_route_mouse_up(n, &evt);
}

static void de_gui_node_fire_mouse_down_event(de_gui_node_t* n, const de_vec2_t* mouse_pos)
{
	de_gui_routed_event_args_t evt;
	de_zero(&evt, sizeof(evt));
	evt.source = n;
	evt.type = DE_GUI_ROUTED_EVENT_MOUSE_DOWN;
	evt.s.mouse_down.pos = *mouse_pos;
	evt.s.mouse_down.button = DE_BUTTON_LEFT;
	de_gui_node_route_mouse_down(n, &evt);
}

static void de_gui_node_fire_lost_focus_event(de_gui_node_t* n)
{
	de_gui_routed_event_args_t evt;
	de_zero(&evt, sizeof(evt));
	evt.type = DE_GUI_ROUTED_EVENT_LOST_FOCUS;
	evt.source = n;
	de_gui_node_route_lost_focus(n, &evt);
}

static void de_gui_node_fire_got_focus_event(de_gui_node_t* n)
{
	de_gui_routed_event_args_t evt;
	de_zero(&evt, sizeof(evt));
	evt.source = n;
	evt.type = DE_GUI_ROUTED_EVENT_GOT_FOCUS;
	de_gui_node_route_got_focus(n, &evt);
}

void de_gui_drop_focus(de_gui_t* gui)
{
	gui->keyboard_focus->is_focused = false;
	de_gui_node_fire_lost_focus_event(gui->keyboard_focus);
	gui->keyboard_focus = NULL;
}

static bool de_gui_node_process_event(de_gui_node_t* n, const de_event_t* evt)
{
	bool processed = false;
	de_gui_t* gui = n->gui;
	de_vec2_t p;
	switch (evt->type) {
		case DE_EVENT_TYPE_MOUSE_MOVE:
			if (!n->is_mouse_over) {
				n->is_mouse_over = true;
				de_gui_node_fire_mouse_enter_event(n);
			}
			p.x = (float)evt->s.mouse_move.x;
			p.y = (float)evt->s.mouse_move.y;
			de_gui_node_fire_mouse_move_event(n, &p);
			processed = true;
			break;
		case DE_EVENT_TYPE_MOUSE_DOWN:
			p.x = (float)evt->s.mouse_down.x;
			p.y = (float)evt->s.mouse_down.y;
			if (evt->s.mouse_down.button == DE_BUTTON_LEFT) {
				if (!n->is_focused) {
					if (gui->keyboard_focus && gui->keyboard_focus != n) {
						de_gui_drop_focus(gui);
					}
					gui->keyboard_focus = n;
					n->is_focused = true;
					de_gui_node_fire_got_focus_event(n);
				}
				de_gui_node_fire_mouse_down_event(n, &p);
			}
			processed = true;
			break;
		case DE_EVENT_TYPE_MOUSE_UP:
			p.x = (float)evt->s.mouse_up.x;
			p.y = (float)evt->s.mouse_up.y;
			if (evt->s.mouse_up.button == DE_BUTTON_LEFT) {
				de_gui_node_fire_mouse_up_event(n, &p);
			}
			processed = true;
		default:
			break;
	}
	return processed;
}

void de_gui_node_set_user_data(de_gui_node_t* node, void* user_data)
{
	node->user_data = user_data;
}

de_gui_node_t* de_gui_hit_test(de_gui_t* gui, float x, float y)
{
	de_gui_node_t* n;
	de_gui_node_t* picked_node = gui->captured_node;
	if (!picked_node) {
		int topmost_picked_level = 0;
		DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
		{
			if (!n->parent && n->visibility == DE_GUI_NODE_VISIBILITY_VISIBLE) {
				int level = 0;
				de_gui_node_t* picked = de_gui_node_pick(n, x, y, &level);
				if (picked && level >= topmost_picked_level) {
					picked_node = picked;
					topmost_picked_level = level;
				}
			}
		}
	}
	return picked_node;
}

bool de_gui_process_event(de_gui_t* gui, const de_event_t* evt)
{
	bool processed = false;
	de_vec2_t p;
	switch (evt->type) {
		case DE_EVENT_TYPE_MOUSE_MOVE:
			p.x = (float)evt->s.mouse_move.x;
			p.y = (float)evt->s.mouse_move.y;
			gui->picked_node = de_gui_hit_test(gui, p.x, p.y);
			/* handle mouseleave for previous picked node */
			if (gui->picked_node != gui->prev_picked_node && gui->prev_picked_node) {
				if (gui->prev_picked_node->is_mouse_over) {
					de_gui_routed_event_args_t revt;
					de_zero(&revt, sizeof(revt));
					gui->prev_picked_node->is_mouse_over = false;
					revt.source = gui->prev_picked_node;
					revt.type = DE_GUI_ROUTED_EVENT_MOUSE_LEAVE;
					de_gui_node_route_mouse_leave(gui->prev_picked_node, &revt);
				}
			}
			break;
		case DE_EVENT_TYPE_MOUSE_DOWN:
			p.x = (float)evt->s.mouse_down.x;
			p.y = (float)evt->s.mouse_down.y;
			gui->picked_node = de_gui_hit_test(gui, p.x, p.y);
			break;
		case DE_EVENT_TYPE_TEXT:
			if (gui->keyboard_focus) {
				de_gui_routed_event_args_t revt;
				de_zero(&revt, sizeof(revt));
				revt.source = gui->keyboard_focus;
				revt.type = DE_GUI_ROUTED_EVENT_TEXT;
				revt.s.text.unicode = evt->s.text.code;
				de_gui_node_route_text_entered(gui->keyboard_focus, &revt);
			}
			break;
		case DE_EVENT_TYPE_KEY_DOWN:
			if (gui->keyboard_focus) {
				de_gui_routed_event_args_t revt;
				de_zero(&revt, sizeof(revt));
				revt.type = DE_GUI_ROUTED_EVENT_KEY_DOWN;
				revt.source = gui->keyboard_focus;
				revt.s.key.key = evt->s.key.key;
				revt.s.key.alt = evt->s.key.alt;
				revt.s.key.control = evt->s.key.control;
				revt.s.key.shift = evt->s.key.shift;
				revt.s.key.system = evt->s.key.system;
				de_gui_node_route_key_down(gui->keyboard_focus, &revt);
			}
			break;
		case DE_EVENT_TYPE_KEY_UP:
			if (gui->keyboard_focus) {
				de_gui_routed_event_args_t revt;
				de_zero(&revt, sizeof(revt));
				revt.type = DE_GUI_ROUTED_EVENT_KEY_UP;
				revt.source = gui->keyboard_focus;
				revt.s.key.key = evt->s.key.key;
				revt.s.key.alt = evt->s.key.alt;
				revt.s.key.control = evt->s.key.control;
				revt.s.key.shift = evt->s.key.shift;
				revt.s.key.system = evt->s.key.system;
				de_gui_node_route_key_up(gui->keyboard_focus, &revt);
			}
			break;
		default:
			break;
	}

	if (gui->picked_node) {
		processed = de_gui_node_process_event(gui->picked_node, evt);
	}

	gui->prev_picked_node = gui->picked_node;

	return processed;
}

void de_gui_update(de_gui_t* gui)
{
	de_gui_node_t* n;

	static int count = 0;

	/* TODO: layout update MUST be performed  only if it is actually  needed,
	* right  now there  is annoying bug which requires *two* calls  of layout
	* update  to correctly layout everything. This must be fixed! For  now we
	* will sacrifice  performance  to get  correct  layout. Also this must be
	* refactored to split layout in two steps: Measure and Arrange, currently
	* user should define desired size of a node manually which is not optimal,
	* because in some situations automatic measure is preferred. */
	{
		/* prepass which fills in actual size of each node based on desired size and margin */
		DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
		{
			de_gui_node_calculate_actual_size(n);
		}

		DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
		{
			if (n->parent == NULL) {
				de_gui_node_apply_layout(n);
			}
		}
	}

	DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
	{
		if (n->dispatch_table->update) {
			n->dispatch_table->update(n);
		}
	}

	/* calculate screen position */
	DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
	{
/* update transform from root nodes */
		if (n->parent == NULL) {
			de_gui_node_update_transform(n);
		}
	}
}

static de_gui_dispatch_table_t* de_gui_node_get_dispatch_table_by_type(de_gui_node_type_t type)
{
	static de_gui_dispatch_table_t stub;
	switch (type) {
		case DE_GUI_NODE_BASE: return &stub;
		case DE_GUI_NODE_TEXT: return de_gui_text_get_dispatch_table();
		case DE_GUI_NODE_BORDER: return de_gui_border_get_dispatch_table();
		case DE_GUI_NODE_WINDOW: return de_gui_window_get_dispatch_table();
		case DE_GUI_NODE_BUTTON: return de_gui_button_get_dispatch_table();
		case DE_GUI_NODE_SCROLL_BAR: return de_gui_scroll_bar_get_dispatch_table();
		case DE_GUI_NODE_SCROLL_VIEWER: return de_gui_scroll_viewer_get_dispatch_table();
		case DE_GUI_NODE_TEXT_BOX: return de_gui_text_box_get_dispatch_table();
		case DE_GUI_NODE_GRID: return de_gui_grid_get_dispatch_table();
		case DE_GUI_NODE_CANVAS: return de_gui_canvas_get_dispatch_table();
		case DE_GUI_NODE_SCROLL_CONTENT_PRESENTER: return de_gui_scroll_content_presenter_get_dispatch_table();
		case DE_GUI_NODE_SLIDE_SELECTOR: return de_gui_slide_selector_get_dispatch_table();
		case DE_GUI_NODE_IMAGE: return de_gui_image_get_dispatch_table();
		case DE_GUI_NODE_USER_CONTROL: return &stub; // TODO
	}
	de_log("unhandled type");
	return NULL;
}

de_gui_node_t* de_gui_node_create(de_gui_t* gui, de_gui_node_type_t type)
{
	de_gui_node_t* n;
	n = DE_NEW(de_gui_node_t);
	n->gui = gui;
	n->dispatch_table = de_gui_node_get_dispatch_table_by_type(type);
	if (type != DE_GUI_NODE_TEMPLATE) {
		/* templates just live in memory and does not participate in any routines */
		DE_LINKED_LIST_APPEND(gui->nodes, n);
	}
	n->type = type;
	DE_ARRAY_INIT(n->children);
	de_vec2_set(&n->desired_size, 200, 200);
	de_color_set(&n->color, 255, 255, 255, 255);
	n->is_hit_test_visible = true;
	n->visibility = DE_GUI_NODE_VISIBILITY_VISIBLE;
	n->vertical_alignment = DE_GUI_VERTICAL_ALIGNMENT_STRETCH;
	n->horizontal_alignment = DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH;
	if (n->dispatch_table->init) {
		n->dispatch_table->init(n);
	}
	return n;
}

de_gui_node_t* de_gui_node_create_with_desc(de_gui_t* gui, de_gui_node_type_t type, const de_gui_node_descriptor_t* desc)
{
	de_gui_node_t* node = de_gui_node_create(gui, type);
	de_gui_node_apply_descriptor(node, desc);
	return node;
}

void de_gui_node_set_desired_local_position(de_gui_node_t* node, float x, float y)
{
	node->desired_local_position.x = x;
	node->desired_local_position.y = y;
}

void de_gui_node_set_column(de_gui_node_t* node, size_t col)
{
	node->column = col;
}

void de_gui_node_set_margin_uniform(de_gui_node_t* node, float margin)
{
	de_gui_node_set_margin(node, margin, margin, margin, margin);
}

de_gui_node_t* de_gui_node_find_parent_of_type(de_gui_node_t* node, de_gui_node_type_t type)
{
	if (!node) {
		return NULL;
	}
	if (node->parent) {
		de_gui_node_t* result;
		if (node->parent->type == type) {
			return node->parent;
		}
		result = de_gui_node_find_parent_of_type(node->parent, type);
		if (result) {
			return result;
		}
	}
	return NULL;
}

de_gui_node_t* de_gui_node_find_direct_child_of_type(de_gui_node_t* node, de_gui_node_type_t type)
{
	size_t i;
	if (!node) {
		return NULL;
	}
	for (i = 0; i < node->children.size; ++i) {
		if (node->children.data[i]->type == type) {
			return node->children.data[i];
		}
	}
	return NULL;
}

de_gui_node_t* de_gui_node_find_child_of_type(de_gui_node_t* node, de_gui_node_type_t type)
{
	size_t i;
	de_gui_node_t* result;
	if (!node) {
		return NULL;
	}
	result = NULL;
	/* first look at direct children */
	for (i = 0; i < node->children.size; ++i) {
		de_gui_node_t* child = node->children.data[i];
		if (child->type == type) {
			result = child;
			break;
		}
	}
	if (!result) {
		/* if nothing found, look at descendants */
		for (i = 0; i < node->children.size; ++i) {
			de_gui_node_t* child_result = de_gui_node_find_child_of_type(node->children.data[i], type);
			if (child_result) {
				result = child_result;
				break;
			}
		}
	}
	return result;
}

void de_gui_node_capture_mouse(de_gui_node_t* node)
{
	de_gui_t* gui = node->gui;

	/* release previous captured node */
	if (gui->captured_node != NULL) {
		de_gui_node_release_mouse_capture(gui->captured_node);
	}

	if (gui->captured_node == NULL) {
		gui->captured_node = node;
	}
}

void de_gui_node_release_mouse_capture(de_gui_node_t* node)
{
	de_gui_t* gui = node->gui;
	if (!gui->captured_node) {
		return;
	}
	if (gui->captured_node == node) {
		gui->captured_node = NULL;
	} else {
		de_fatal_error("mismatch pair capture/release mouse!");
	}
}

void de_gui_node_set_mouse_down(de_gui_node_t* node, de_mouse_down_event_t evt)
{
	node->mouse_down = evt;
}

void de_gui_node_set_mouse_up(de_gui_node_t* node, de_mouse_up_event_t evt)
{
	node->mouse_up = evt;
}

void de_gui_node_set_mouse_move(de_gui_node_t* node, de_mouse_move_event_t evt)
{
	node->mouse_move = evt;
}

static void de_gui_node_drop_keyboard_focus(de_gui_node_t* node)
{
	if (node->gui->keyboard_focus == node) {
		de_gui_drop_focus(node->gui);
	} else {
		for (size_t i = 0; i < node->children.size; ++i) {
			de_gui_node_drop_keyboard_focus(node->children.data[i]);
		}
	}
}

void de_gui_node_set_visibility(de_gui_node_t* node, de_gui_node_visibility_t vis)
{
	node->visibility = vis;
	if (vis != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		de_gui_node_drop_keyboard_focus(node);
	}
}

void de_gui_node_set_vertical_alignment(de_gui_node_t* node, de_gui_vertical_alignment_t va)
{
	node->vertical_alignment = va;
}

void de_gui_node_set_horizontal_alignment(de_gui_node_t* node, de_gui_horizontal_alignment_t ha)
{
	node->horizontal_alignment = ha;
}

void de_gui_node_set_margin(de_gui_node_t* node, float left, float top, float right, float bottom)
{
	node->margin.left = left;
	node->margin.top = top;
	node->margin.right = right;
	node->margin.bottom = bottom;
}

void de_gui_node_set_desired_size(de_gui_node_t* node, float w, float h)
{
	node->desired_size.x = w;
	node->desired_size.y = h;
}

bool de_gui_node_set_property(de_gui_node_t* n, const char* name, const void* value, size_t data_size)
{
/* handle type-specific properties */
	if (n->dispatch_table->set_property) {
		if (n->dispatch_table->set_property(n, name, value, data_size)) {
			return true;
		}
	}
	/* standard properties */
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, desired_local_position, name, DE_GUI_NODE_DESIRED_POSITION_PROPERTY, value, data_size, n);
	return false;
}

bool de_gui_node_parse_property(de_gui_node_t* n, const char* name, const char* value)
{
	de_str8_t str;
	de_str8_view_array_t tokens;
	bool result = false;
	if (n->dispatch_table->parse_property) {
		result = n->dispatch_table->parse_property(n, name, value);
	}
	de_str8_set(&str, value);
	tokens = de_str8_tokenize(&str, ";, ");
	if (strcmp(name, DE_GUI_NODE_DESIRED_POSITION_PROPERTY) == 0) {
		if (tokens.size == 2) {
			n->desired_local_position.x = (float)atof(tokens.data[0].data);
			n->desired_local_position.y = (float)atof(tokens.data[1].data);
			result = true;
		}
	}
	de_str8_free(&str);
	DE_ARRAY_FREE(tokens);
	return result;
}

bool de_gui_node_get_property(de_gui_node_t* n, const char* name, void* value, size_t data_size)
{
/* handle type-specific properties */
	if (n->dispatch_table->get_property) {
		if (n->dispatch_table->get_property(n, name, value, data_size)) {
			return true;
		}
	}
	/* standard properties */
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, desired_local_position, name, DE_GUI_NODE_DESIRED_POSITION_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, actual_local_position, name, DE_GUI_NODE_ACTUAL_POSITION_PROPERTY, value, data_size, n);
	DE_DECLARE_PROPERTY_SETTER(de_gui_node_t, screen_position, name, DE_GUI_NODE_SCREEN_POSITION_PROPERTY, value, data_size, n);
	return false;
}

void de_gui_node_set_hit_test_visible(de_gui_node_t* n, bool visibility)
{
	n->is_hit_test_visible = visibility;
}

void de_gui_node_set_color(de_gui_node_t* node, const de_color_t* color)
{
	node->color = *color;
}

void de_gui_node_set_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	node->color.r = r;
	node->color.g = g;
	node->color.b = b;
	node->color.a = a;
}

bool de_gui_node_attach(de_gui_node_t* node, de_gui_node_t* parent)
{
	de_gui_node_detach(node);
	DE_ARRAY_APPEND(parent->children, node);
	node->parent = parent;
	return true;
}

void de_gui_node_detach(de_gui_node_t* node)
{
	if (node->parent) {
		DE_ARRAY_REMOVE(node->parent->children, node);
		node->parent = NULL;
	}
}

void de_gui_node_free(de_gui_node_t* n)
{
	size_t i;
	if (n->dispatch_table->deinit) {
		n->dispatch_table->deinit(n);
	}
	DE_LINKED_LIST_REMOVE(n->gui->nodes, n);
	/* free children first */
	for (i = 0; i < n->children.size; ++i) {
		de_gui_node_free(n->children.data[i]);
	}
	DE_ARRAY_FREE(n->children);
	DE_ARRAY_FREE(n->geometry);
	de_free(n);
}

void de_gui_node_measure(de_gui_node_t* node, const de_vec2_t* constraint)
{
	size_t i;
	if (node->dispatch_table->measure) {
		/* delegate to override method */
		node->dispatch_table->measure(node, constraint);
	} else {
		/* standard measure */
		node->desired_size.x = 0;
		node->desired_size.y = 0;
		for (i = 0; i < node->children.size; ++i) {
			de_gui_node_t* child = node->children.data[i];
			de_vec2_t child_constraint;
			child_constraint.x = de_maxf(0.0f, constraint->x - (node->margin.left + node->margin.right));
			child_constraint.y = de_maxf(0.0f, constraint->y - (node->margin.top + node->margin.bottom));
			de_gui_node_measure(child, &child_constraint);
			if (child->desired_size.x > node->desired_size.x) {
				node->desired_size.x = child->desired_size.x;
			}
			if (child->desired_size.y > node->desired_size.y) {
				node->desired_size.y = child->desired_size.y;
			}
		}
	}
}

void de_gui_node_set_row(de_gui_node_t* node, size_t row)
{
	node->row = row;
}

static bool de_gui_node_needs_rendering(const de_gui_node_t* n)
{
	bool result;
	/* node is degenerated (collapsed) */
	if (n->actual_size.x == 0 && n->actual_size.y == 0) {
		return false;
	}
	result = n->visibility == DE_GUI_NODE_VISIBILITY_VISIBLE;
	if (n->parent) {
		result &= de_gui_node_needs_rendering(n->parent);
	}
	return result;
}

static void de_gui_node_draw(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	size_t i;
	const de_vec2_t* scr_pos = &n->screen_position;
	const float bias = 0.01f;

	if (!de_gui_node_needs_rendering(n)) {
		return;
	}

	DE_ARRAY_CLEAR(n->geometry);

	de_gui_draw_list_set_nesting(dl, nesting);
	de_gui_draw_list_commit_clip_rect(dl, scr_pos->x - 0.5f, scr_pos->y - 0.5f, n->actual_size.x + (1.0f - bias), n->actual_size.y + (1.0f - bias), n);

	if (n->dispatch_table->render) {
		n->dispatch_table->render(dl, n, nesting);
	}

	++nesting;

	/* draw children */
	for (i = 0; i < n->children.size; ++i) {
		de_gui_node_draw(dl, n->children.data[i], nesting);
	}

	de_gui_draw_list_revert_clip_geom(dl);
}

de_gui_draw_list_t* de_gui_render(de_gui_t* gui)
{
	de_gui_node_t* n;
	de_gui_draw_list_t* dl = &gui->draw_list;
	de_core_t* core = gui->core;

	/* prepare draw list */
	de_gui_draw_list_clear(dl);

	/* push global clipping rect */
	de_gui_draw_list_set_nesting(dl, 1);
	de_gui_draw_list_commit_clip_rect(dl, 0, 0, (float)core->params.video_mode.width, (float)core->params.video_mode.height, NULL);

	/* render nodes down by visual tree starting from root nodes */
	DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
	{
		if (!n->parent) {
			de_gui_node_draw(dl, n, 1);
		}
	}

#if DE_GUI_ENABLE_GUI_DEBUGGING
	if (gui->picked_node) {
		de_color_t red = { 255, 0, 0, 255 };
		de_gui_draw_list_set_nesting(dl, 0);
		de_gui_draw_list_push_rect(dl, &gui->picked_node->screen_position, &gui->picked_node->actual_size, 1, &red);
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, 0);

		printf("p: %f %f\n", gui->picked_node->screen_position.x, gui->picked_node->screen_position.y);
		printf("s: %f %f\n", gui->picked_node->actual_size.x, gui->picked_node->actual_size.y);
	}
	if (gui->keyboard_focus) {
		de_color_t green = { 0, 200, 0, 255 };
		de_gui_draw_list_set_nesting(dl, 0);
		de_gui_draw_list_push_rect(dl, &gui->keyboard_focus->screen_position, &gui->keyboard_focus->actual_size, 1, &green);
		de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, 0, 0);
	}
#endif

	return dl;
}

void de_gui_node_apply_descriptor(de_gui_node_t* n, const de_gui_node_descriptor_t* desc)
{
	de_gui_node_set_column(n, desc->column);
	de_gui_node_set_row(n, desc->row);
	de_gui_node_set_desired_local_position(n, desc->desired_position.x, desc->desired_position.y);
	if (desc->desired_size.x != 0 && desc->desired_size.y != 0) {
		de_gui_node_set_desired_size(n, desc->desired_size.x, desc->desired_size.y);
	}
	de_gui_node_set_horizontal_alignment(n, desc->horizontal_alignment);
	de_gui_node_set_vertical_alignment(n, desc->vertical_alignment);
	de_gui_node_set_visibility(n, desc->visibility);
	de_gui_node_set_margin(n, desc->margin.left, desc->margin.top, desc->margin.right, desc->margin.bottom);
	if (desc->parent) {
		de_gui_node_attach(n, desc->parent);
	}
	de_gui_node_set_user_data(n, desc->user_data);
	if (n->dispatch_table->apply_descriptor) {
		n->dispatch_table->apply_descriptor(n, desc);
	}
}

void de_gui_node_apply_template(de_gui_node_t* node, const de_gui_node_t* ctemplate)
{
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEMPLATE);
	/* purge node */
	for (size_t i = 0; i < node->children.size; ++i) {
		de_gui_node_free(node->children.data[i]);
	}
	if (node->dispatch_table->apply_template) {
		node->dispatch_table->apply_template(node, ctemplate);
	}
}