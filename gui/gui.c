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
#include "gui/check_box.c"

/* Declares bubbling router for specified event callback. Bubbling router is a function which 
 * will call itself with same arguments but different caller until event marked as handled.
 * In other words if this function is called from this hierarchy starting from Node1:
 * Node3
 *    |_Node2
 *         |_Node1 <- Entry
 * then same function will be called on Node2 and then Node3.
 */
#define DE_DECLARE_ROUTED_EVENT_ROUTER(name__, event__) \
	static void name__(de_gui_node_t* n, de_gui_routed_event_args_t* args) { \
		if (n->event__) { \
			n->event__(n, args); \
		} \
		if (n->parent && !args->handled) { \
			name__(n->parent, args); \
		} \
	} 

DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_down, mouse_down)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_up, mouse_up)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_leave, mouse_leave)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_move, mouse_move)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_got_focus, got_focus)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_lost_focus, lost_focus)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_wheel, mouse_wheel)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_mouse_enter, mouse_enter)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_text_entered, text_entered)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_key_down, key_down)
DE_DECLARE_ROUTED_EVENT_ROUTER(de_gui_node_route_key_up, key_up)

#undef DE_DECLARE_ROUTED_EVENT_ROUTER

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
		for (int i = 0; i < CHAR_COUNT; ++i) {
			char_set[i] = i;
		}
		gui->default_font = de_font_load_ttf_from_memory(core, (void*)de_builtin_font_inconsolata, 19, char_set, CHAR_COUNT);
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
	const int* indices = draw_list->index_buffer.data;
	const de_gui_vertex_t* vertices = draw_list->vertex_buffer.data;
	const size_t last = cmd->index_offset + cmd->triangle_count * 3;

	/* check each triangle from command for intersection with mouse pointer */
	for (size_t j = cmd->index_offset; j < last; j += 3) {
		const de_gui_vertex_t* va = vertices + indices[j];
		const de_gui_vertex_t* vb = vertices + indices[j + 1];
		const de_gui_vertex_t* vc = vertices + indices[j + 2];

		/* check if point is in triangle */
		de_vec2_t v0, v1, v2;
		de_vec2_sub(&v0, &vc->pos, &va->pos);
		de_vec2_sub(&v1, &vb->pos, &va->pos);
		de_vec2_sub(&v2, pos, &va->pos);

		const float dot00 = de_vec2_dot(&v0, &v0);
		const float dot01 = de_vec2_dot(&v0, &v1);
		const float dot02 = de_vec2_dot(&v0, &v2);
		const float dot11 = de_vec2_dot(&v1, &v1);
		const float dot12 = de_vec2_dot(&v1, &v2);

		const float denom = (dot00 * dot11 - dot01 * dot01);

		if (denom <= FLT_EPSILON) {
			/* we don't want floating-point exceptions */
			return false;
		}

		const float invDenom = 1.0f / denom;
		const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		if ((u >= 0) && (v >= 0) && (u + v < 1)) {
			return true;
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
	de_gui_draw_list_t* draw_list = &node->gui->draw_list;

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		return true;
	}

	bool clipped = true;

	for (size_t i = 0; i < node->geometry.size; ++i) {
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
	de_gui_draw_list_t* draw_list = &node->gui->draw_list;

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		return false;
	}

	if (!de_gui_node_is_clipped(node, point)) {
		for (size_t i = 0; i < node->geometry.size; ++i) {
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
	DE_ASSERT(node);
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
				if (revt.handled) {
					processed = true;
				}
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
				if (revt.handled) {
					processed = true;
				}
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
				if (revt.handled) {
					processed = true;
				}
			}
			break;
		case DE_EVENT_TYPE_MOUSE_WHEEL:
			if (gui->picked_node) {
				de_gui_routed_event_args_t revt;
				de_zero(&revt, sizeof(revt));
				revt.type = DE_GUI_ROUTED_EVENT_MOUSE_WHEEL;
				revt.source = gui->picked_node;
				revt.s.wheel.delta = evt->s.mouse_wheel.delta;
				revt.s.wheel.pos = (de_vec2_t) { (float)evt->s.mouse_wheel.x, (float)evt->s.mouse_wheel.y };
				de_gui_node_route_mouse_wheel(gui->picked_node, &revt);
				if (revt.handled) {
					processed = true;
				}
			}
			break;
		default:
			break;
	}

	if (!processed && gui->picked_node) {
		processed = de_gui_node_process_event(gui->picked_node, evt);
	}

	gui->prev_picked_node = gui->picked_node;

	return processed;
}

void de_gui_update(de_gui_t* gui)
{
	de_gui_node_t* n;

	/* Step 1. Recursive Measure pass from root nodes with screen size as constraint. */
	const de_vec2_t size_for_child = {
		(float)de_core_get_window_width(gui->core),
		(float)de_core_get_window_height(gui->core)
	};
	DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
	{
		if (n->parent == NULL) {
			de_gui_node_measure(n, &size_for_child);
		}
	}

	/* Step 2. Recursive Arrange pass from root nodes with screen rect as final rect. */
	DE_LINKED_LIST_FOR_EACH(gui->nodes, n)
	{
		if (n->parent == NULL) {
			const de_rectf_t rect = {
				n->desired_local_position.x,
				n->desired_local_position.y,
				(float)de_core_get_window_width(gui->core),
				(float)de_core_get_window_height(gui->core)
			};
			de_gui_node_arrange(n, &rect);
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

static de_gui_node_dispatch_table_t* de_gui_node_get_dispatch_table_by_type(de_gui_node_type_t type)
{
	static de_gui_node_dispatch_table_t stub;
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
		case DE_GUI_NODE_CHECK_BOX: return de_gui_check_box_get_dispatch_table();
		case DE_GUI_NODE_USER_CONTROL: return &stub; // TODO
        case DE_GUI_NODE_TEMPLATE: return &stub; // TODO
	}
	de_log("unhandled type");
	return NULL;
}

de_gui_node_t* de_gui_node_create(de_gui_t* gui, de_gui_node_type_t type)
{	
	de_gui_node_t* n = DE_NEW(de_gui_node_t);
	n->gui = gui;
	n->dispatch_table = de_gui_node_get_dispatch_table_by_type(type);
	if (type != DE_GUI_NODE_TEMPLATE) {
		/* templates just live in memory and does not participate in any routines */
		DE_LINKED_LIST_APPEND(gui->nodes, n);
	}
	n->type = type;
	DE_ARRAY_INIT(n->children);
	/* Set width and height to NaN to mark them as undefined, so layout system won't
	 * take these values into account. */
	n->width = NAN;
	n->height = NAN;
	n->min_size = (de_vec2_t) { 0, 0 };
	n->max_size = (de_vec2_t) { INFINITY, INFINITY };
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
	DE_ASSERT(node);
	node->desired_local_position.x = x;
	node->desired_local_position.y = y;
}

void de_gui_node_set_column(de_gui_node_t* node, size_t col)
{
	DE_ASSERT(node);
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
	if (!node) {
		return NULL;
	}
	for (size_t i = 0; i < node->children.size; ++i) {
		if (node->children.data[i]->type == type) {
			return node->children.data[i];
		}
	}
	return NULL;
}

de_gui_node_t* de_gui_node_find_child_of_type(de_gui_node_t* node, de_gui_node_type_t type)
{	
	de_gui_node_t* result;
	if (!node) {
		return NULL;
	}
	result = NULL;
	/* first look at direct children */
	for (size_t i = 0; i < node->children.size; ++i) {
		de_gui_node_t* child = node->children.data[i];
		if (child->type == type) {
			result = child;
			break;
		}
	}
	if (!result) {
		/* if nothing found, look at descendants */
		for (size_t i = 0; i < node->children.size; ++i) {
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
	DE_ASSERT(node);
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
	DE_ASSERT(node);
	node->visibility = vis;
	if (vis != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		de_gui_node_drop_keyboard_focus(node);
	}
}

void de_gui_node_set_vertical_alignment(de_gui_node_t* node, de_gui_vertical_alignment_t va)
{
	DE_ASSERT(node);
	node->vertical_alignment = va;
}

void de_gui_node_set_horizontal_alignment(de_gui_node_t* node, de_gui_horizontal_alignment_t ha)
{
	DE_ASSERT(node);
	node->horizontal_alignment = ha;
}

void de_gui_node_set_margin(de_gui_node_t* node, float left, float top, float right, float bottom)
{
	DE_ASSERT(node);
	node->margin.left = left;
	node->margin.top = top;
	node->margin.right = right;
	node->margin.bottom = bottom;
}

void de_gui_node_set_size(de_gui_node_t* node, float w, float h)
{
	DE_ASSERT(node);
	node->width = w;
	node->height = h;
}

bool de_gui_node_set_property(de_gui_node_t* n, const char* name, const void* value, size_t data_size)
{
	DE_ASSERT(n);
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
	DE_ASSERT(n);
	DE_ASSERT(name);
	DE_ASSERT(value);
	DE_ASSERT(data_size);
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
	DE_ASSERT(n);
	n->is_hit_test_visible = visibility;
}

void de_gui_node_set_color(de_gui_node_t* node, const de_color_t* color)
{
	DE_ASSERT(node);
	DE_ASSERT(color);
	node->color = *color;
}

void de_gui_node_set_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	DE_ASSERT(node);
	node->color.r = r;
	node->color.g = g;
	node->color.b = b;
	node->color.a = a;
}

bool de_gui_node_attach(de_gui_node_t* node, de_gui_node_t* parent)
{
	DE_ASSERT(node);
	DE_ASSERT(parent);
	de_gui_node_detach(node);
	DE_ARRAY_APPEND(parent->children, node);
	node->parent = parent;
	return true;
}

void de_gui_node_detach(de_gui_node_t* node)
{
	DE_ASSERT(node);
	if (node->parent) {
		DE_ARRAY_REMOVE(node->parent->children, node);
		node->parent = NULL;
	}
}

void de_gui_node_free(de_gui_node_t* n)
{
	DE_ASSERT(n);	
	if (n->dispatch_table->deinit) {
		n->dispatch_table->deinit(n);
	}
	DE_LINKED_LIST_REMOVE(n->gui->nodes, n);
	/* free children first */
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_free(n->children.data[i]);
	}
	DE_ARRAY_FREE(n->children);
	DE_ARRAY_FREE(n->geometry);
	de_free(n);
}

de_vec2_t de_gui_node_default_measure_override(de_gui_node_t* n, const de_vec2_t* available_size)
{
	DE_ASSERT(n);
	DE_ASSERT(available_size);
	de_vec2_t size = { 0, 0 };
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];

		de_gui_node_measure(child, available_size);

		if (child->desired_size.x > size.x) {
			size.x = child->desired_size.x;
		}
		if (child->desired_size.y > size.y) {
			size.y = child->desired_size.y;
		}
	}
	return size;
}

void de_gui_node_measure(de_gui_node_t* node, const de_vec2_t* available_size)
{
	DE_ASSERT(node);
	DE_ASSERT(available_size);

	const float margin_x = node->margin.left + node->margin.right;
	const float margin_y = node->margin.top + node->margin.bottom;

	de_vec2_t size_for_child = {
		node->width > 0 ? node->width : de_maxf(0.0f, available_size->x - margin_x),
		node->height > 0 ? node->height : de_maxf(0.0f, available_size->y - margin_y)
	};

	if (size_for_child.x > node->max_size.x) {
		size_for_child.x = node->max_size.x;
	} else if (size_for_child.x < node->min_size.x) {
		size_for_child.x = node->min_size.x;
	}

	if (size_for_child.y > node->max_size.y) {
		size_for_child.y = node->max_size.y;
	} else if (size_for_child.y < node->min_size.y) {
		size_for_child.y = node->min_size.y;
	}

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		node->desired_size = (de_vec2_t) { 0, 0 };
	} else {
		if (node->dispatch_table->measure_override) {
			/* use custom override */
			node->desired_size = node->dispatch_table->measure_override(node, &size_for_child);
		} else {
			/* use standard override */
			node->desired_size = de_gui_node_default_measure_override(node, &size_for_child);
		}
	}

	/* Set desired size to Width and Height only if they have well defined values. */
	if (!isnan(node->width)) {
		node->desired_size.x = node->width;
	}

	if (node->desired_size.x > node->max_size.x) {
		node->desired_size.x = node->max_size.x;
	} else if (node->desired_size.x < node->min_size.x) {
		node->desired_size.x = node->min_size.x;
	}

	if (node->desired_size.y > node->max_size.y) {
		node->desired_size.y = node->max_size.y;
	} else if (node->desired_size.y < node->min_size.y) {
		node->desired_size.y = node->min_size.y;
	}

	if (!isnan(node->height)) {
		node->desired_size.y = node->height;
	}

	node->desired_size.x += margin_x;
	node->desired_size.y += margin_y;

	/* Make sure that node won't go outside of available bounds. */
	if (node->desired_size.x > available_size->x) {
		node->desired_size.x = available_size->x;
	}
	if (node->desired_size.y > available_size->y) {
		node->desired_size.y = available_size->y;
	}
}

de_vec2_t de_gui_node_default_arrange_override(de_gui_node_t* n, const de_vec2_t* final_size)
{
	DE_ASSERT(n);
	DE_ASSERT(final_size);
	const de_rectf_t rect = { 0, 0, final_size->x, final_size->y };
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_t* child = n->children.data[i];
		de_gui_node_arrange(child, &rect);
	}
	return *final_size;
}

void de_gui_node_arrange(de_gui_node_t* node, const de_rectf_t* final_rect)
{
	DE_ASSERT(node);
	DE_ASSERT(final_rect);

	if (node->visibility != DE_GUI_NODE_VISIBILITY_VISIBLE) {
		return;
	}

	const float margin_x = node->margin.left + node->margin.right;
	const float margin_y = node->margin.top + node->margin.bottom;

	float origin_x = final_rect->x + node->margin.left;
	float origin_y = final_rect->y + node->margin.top;

	de_vec2_t size = {
		de_maxf(0.0f, final_rect->w - margin_x),
		de_maxf(0.0f, final_rect->h - margin_y)
	};

	const de_vec2_t size_without_margin = size;

	if (node->horizontal_alignment != DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH) {
		size.x = de_minf(size.x, node->desired_size.x - margin_x);
	}
	if (node->vertical_alignment != DE_GUI_VERTICAL_ALIGNMENT_STRETCH) {
		size.y = de_minf(size.y, node->desired_size.y - margin_y);
	}

	if (node->width > 0) {
		size.x = node->width;
	}
	if (node->height > 0) {
		size.y = node->height;
	}

	if (node->dispatch_table->arrange_override) {
		size = node->dispatch_table->arrange_override(node, &size);
	} else {
		size = de_gui_node_default_arrange_override(node, &size);
	}

	if (size.x > final_rect->w) {
		size.x = final_rect->w;
	}
	if (size.y > final_rect->h) {
		size.y = final_rect->h;
	}

	switch (node->horizontal_alignment) {
		case DE_GUI_HORIZONTAL_ALIGNMENT_CENTER:
		case DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH:
			origin_x += (size_without_margin.x - size.x) * 0.5f;
			break;
		case DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT:
			origin_x += size_without_margin.x - size.x;
			break;
		default:
			break;
	}

	switch (node->vertical_alignment) {
		case DE_GUI_VERTICAL_ALIGNMENT_CENTER:
		case DE_GUI_VERTICAL_ALIGNMENT_STRETCH:
			origin_y += (size_without_margin.y - size.y) * 0.5f;
			break;
		case DE_GUI_VERTICAL_ALIGNMENT_BOTTOM:
			origin_y += size_without_margin.y - size.y;
			break;
		default:
			break;
	}

	node->actual_size = size;
	node->actual_local_position = (de_vec2_t) { origin_x, origin_y };
}

void de_gui_node_set_row(de_gui_node_t* node, size_t row)
{
	DE_ASSERT(node);
	node->row = row;
}

static bool de_gui_node_needs_rendering(const de_gui_node_t* n)
{
	DE_ASSERT(n);
	/* node is degenerated (collapsed) */
	if (n->actual_size.x == 0 && n->actual_size.y == 0) {
		return false;
	}
	bool result = n->visibility == DE_GUI_NODE_VISIBILITY_VISIBLE;
	if (n->parent) {
		result &= de_gui_node_needs_rendering(n->parent);
	}
	return result;
}

static void de_gui_node_draw(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{	
	DE_ASSERT(dl);
	DE_ASSERT(n);

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
	for (size_t i = 0; i < n->children.size; ++i) {
		de_gui_node_draw(dl, n->children.data[i], nesting);
	}

	de_gui_draw_list_revert_clip_geom(dl);
}

de_gui_draw_list_t* de_gui_render(de_gui_t* gui)
{
	DE_ASSERT(gui);

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

	DE_ASSERT(dl->triangles_to_commit == 0); /* forgot to call de_gui_draw_list_commit */

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
	DE_ASSERT(n);
	de_gui_node_set_column(n, desc->column);
	de_gui_node_set_row(n, desc->row);
	de_gui_node_set_desired_local_position(n, desc->x, desc->y);
	if (desc->width != 0) {
		n->width = desc->width;
	}
	if (desc->height != 0) {
		n->height = desc->height;
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
	if (desc->color.r != 0 && desc->color.g != 0 && desc->color.b != 0) {
		de_gui_node_set_color(n, &desc->color);
	}
}

void de_gui_node_apply_template(de_gui_node_t* node, const de_gui_node_t* ctemplate)
{
	DE_ASSERT(node);
	/* TODO */
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_TEMPLATE);
	/* purge node */
	for (size_t i = 0; i < node->children.size; ++i) {
		de_gui_node_free(node->children.data[i]);
	}
	if (node->dispatch_table->apply_template) {
		node->dispatch_table->apply_template(node, ctemplate);
	}
}

void de_gui_node_set_min_width(de_gui_node_t* node, float min_width)
{
	DE_ASSERT(node);
	DE_ASSERT(min_width >= 0);
	node->min_size.x = min_width;
}

void de_gui_node_set_min_height(de_gui_node_t* node, float min_height)
{
	DE_ASSERT(node);
	DE_ASSERT(min_height >= 0);
	node->min_size.y = min_height;
}

void de_gui_node_set_max_width(de_gui_node_t* node, float max_width)
{
	DE_ASSERT(node);
	DE_ASSERT(!isnan(max_width));
	node->max_size.x = max_width;
}

void de_gui_node_set_max_height(de_gui_node_t* node, float max_height)
{
	DE_ASSERT(node);
	DE_ASSERT(!isnan(max_height));
	node->max_size.y = max_height;
}

float de_gui_node_get_min_width(de_gui_node_t* node)
{
	DE_ASSERT(node);
	return node->min_size.x;
}

float de_gui_node_get_min_height(de_gui_node_t* node)
{
	DE_ASSERT(node);
	return node->min_size.y;
}

float de_gui_node_get_max_width(de_gui_node_t* node)
{
	DE_ASSERT(node);
	return node->max_size.x;
}

float de_gui_node_get_max_height(de_gui_node_t* node)
{
	DE_ASSERT(node);
	return node->max_size.y;
}