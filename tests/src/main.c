#include "de_main.h"

typedef struct game_t game_t;
typedef struct main_menu_t main_menu_t;
typedef struct level_t level_t;
typedef struct player_t player_t;
typedef struct weapon_t weapon_t;

struct game_t {
	de_core_t* core;
	level_t* level;
	main_menu_t* main_menu;
	de_gui_node_t* fps_text;
};

struct main_menu_t {
	de_sound_source_t* music;
	game_t* game;
	de_gui_node_t* window;
	de_video_mode_array_t video_modes;
	bool visible;
};

struct level_t {
	game_t* game;
	de_scene_t* scene;
	player_t* player;
};

typedef enum weapon_type_t {
	WEAPON_TYPE_SHOTGUN
} weapon_type_t;

struct weapon_t {
	weapon_type_t type;
	de_node_t* model;
};

typedef struct player_controller_t {
	bool move_forward, move_backward;
	bool strafe_left, strafe_right;
	bool crouch, run, jumped;
} player_controller_t;

struct player_t {
	level_t* parent_level;
	de_node_t* pivot;
	de_node_t* camera;
	de_node_t* flash_light;
	de_body_t* body;
	de_node_t* weapon_pivot;
	float pitch;
	float desired_pitch;
	float yaw;
	float desired_yaw;
	float move_speed;
	float stand_body_radius;
	float crouch_body_radius;
	float stand_up_speed;
	float sit_down_speed;
	float run_speed_multiplier;
	float camera_wobble;
	float path_len;
	de_vec3_t camera_offset;
	de_vec3_t camera_dest_offset;
	de_vec3_t camera_position;
	DE_ARRAY_DECLARE(weapon_t*, weapons);
	player_controller_t controller;
	DE_ARRAY_DECLARE(de_sound_source_t*, footsteps);
};

weapon_t* weapon_create(level_t* level, weapon_type_t type) {
	weapon_t* wpn = DE_NEW(weapon_t);

	wpn->type = type;

	if (type == WEAPON_TYPE_SHOTGUN) {
		wpn->model = de_fbx_load_to_scene(level->scene, "data/models/shotgun.fbx");
	} else {
		de_log("invalid weapon type");
	}

	return wpn;
}

void weapon_free(weapon_t* wpn) {
	de_node_free(wpn->model);
	de_free(wpn);
}

void weapon_update(weapon_t* wpn) {
	DE_UNUSED(wpn);
}

bool player_process_event(player_t* p, const de_event_t* evt) {
	bool processed = false;
	switch (evt->type) {
		case DE_EVENT_TYPE_MOUSE_MOVE:
			p->desired_yaw -= evt->s.mouse_move.vx;
			p->desired_pitch -= evt->s.mouse_move.vy;
			if (p->desired_pitch > 90.0) {
				p->desired_pitch = 90.0;
			}
			if (p->desired_pitch < -90.0) {
				p->desired_pitch = -90.0;
			}
			break;
		case DE_EVENT_TYPE_KEY_DOWN:
			switch (evt->s.key.key) {
				case DE_KEY_W:
					p->controller.move_forward = true;
					break;
				case DE_KEY_S:
					p->controller.move_backward = true;
					break;
				case DE_KEY_A:
					p->controller.strafe_left = true;
					break;
				case DE_KEY_D:
					p->controller.strafe_right = true;
					break;
				case DE_KEY_C:
					p->controller.crouch = true;
					break;
				case DE_KEY_Space:
					if (!p->controller.jumped) {
						size_t k, contact_count;
						contact_count = de_body_get_contact_count(p->body);
						for (k = 0; k < contact_count; ++k) {
							de_contact_t* contact = de_body_get_contact(p->body, k);
							if (contact->normal.y > 0.7f) {
								de_body_set_y_velocity(p->body, 0.075f);
								break;
							}
						}
						p->controller.jumped = true;
					}
					break;
				case DE_KEY_LSHIFT:
					p->controller.run = true;
					break;
				default:
					break;
			}
			break;
		case DE_EVENT_TYPE_KEY_UP:
			switch (evt->s.key.key) {
				case DE_KEY_W:
					p->controller.move_forward = false;
					break;
				case DE_KEY_S:
					p->controller.move_backward = false;
					break;
				case DE_KEY_A:
					p->controller.strafe_left = false;
					break;
				case DE_KEY_D:
					p->controller.strafe_right = false;
					break;
				case DE_KEY_C:
					p->controller.crouch = false;
					break;
				case DE_KEY_Space:
					p->controller.jumped = false;
					break;
				case DE_KEY_LSHIFT:
					p->controller.run = false;
					break;
				default:
					break;
			}
			break;
		case DE_EVENT_TYPE_MOUSE_DOWN:
			break;
		default:
			break;
	}
	return processed;
}

player_t* player_create(level_t* level) {
	int i;
	de_sound_context_t* ctx;
	player_t* p;

	p = DE_NEW(player_t);

	p->parent_level = level;
	p->move_speed = 0.028f;
	p->stand_body_radius = 0.5f;
	p->crouch_body_radius = 0.35f;
	p->sit_down_speed = 0.045f;
	p->stand_up_speed = 0.06f;
	p->run_speed_multiplier = 1.75f;
	p->camera_position = (de_vec3_t) { 0, p->stand_body_radius, 0 };

	p->body = de_body_create(level->scene);
	de_body_set_gravity(p->body, &(de_vec3_t) {.y = -20 });
	de_body_set_radius(p->body, p->stand_body_radius);

	p->pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->pivot);
	de_node_set_body(p->pivot, p->body);

	p->camera = de_node_create(level->scene, DE_NODE_TYPE_CAMERA);
	de_scene_add_node(level->scene, p->camera);
	de_node_set_local_position(p->camera, &p->camera_position);
	de_node_attach(p->camera, p->pivot);

	p->flash_light = de_node_create(level->scene, DE_NODE_TYPE_LIGHT);
	de_light_set_radius(de_node_to_light(p->flash_light), 4);
	de_scene_add_node(level->scene, p->flash_light);
	de_node_attach(p->flash_light, p->camera);

	p->weapon_pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->weapon_pivot);
	de_node_attach(p->weapon_pivot, p->camera);
	de_node_set_local_position(p->weapon_pivot, &(de_vec3_t) { 0.03f, -0.052f, -0.02f });

	ctx = de_core_get_sound_context(level->game->core);

	for (i = 0; i < 4; ++i) {
		char buf[PATH_MAX];
		snprintf(buf, sizeof(buf), "data/sounds/footsteps/FootStep_shoe_metal_step%d.wav", i + 1);
		de_path_t path;
		de_path_from_cstr_as_view(&path, buf);
		de_resource_t* res = de_core_request_resource(level->game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path, 0);
		de_sound_source_t* src = de_sound_source_create(ctx, DE_SOUND_SOURCE_TYPE_3D);
		de_sound_source_set_buffer(src, de_resource_to_sound_buffer(res));
		DE_ARRAY_APPEND(p->footsteps, src);
	}

#if 0
	{
		weapon_t* shotgun = weapon_create(level, WEAPON_TYPE_SHOTGUN);
		de_node_attach(shotgun->model, p->weapon_pivot);
		DE_ARRAY_APPEND(p->weapons, shotgun);
	}
#endif

	return p;
}

bool level_visit(de_object_visitor_t* visitor, level_t* level);

bool player_visit(de_object_visitor_t* visitor, player_t* player) {
	bool result = true;
	if (visitor->is_reading) {
		game_t* game = de_core_get_user_pointer(visitor->core);
		player->parent_level = game->level;
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Level", &player->parent_level, level_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Pivot", &player->pivot, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Camera", &player->camera, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "FlashLight", &player->flash_light, de_node_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Body", &player->body, de_body_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "WeaponPivot", &player->weapon_pivot, de_node_visit);
	result &= de_object_visitor_visit_float(visitor, "Pitch", &player->pitch);
	result &= de_object_visitor_visit_float(visitor, "DesiredPitch", &player->desired_pitch);
	result &= de_object_visitor_visit_float(visitor, "Yaw", &player->yaw);
	result &= de_object_visitor_visit_float(visitor, "DesiredYaw", &player->desired_yaw);
	result &= de_object_visitor_visit_float(visitor, "MoveSpeed", &player->move_speed);
	result &= de_object_visitor_visit_float(visitor, "StandBodyRadius", &player->stand_body_radius);
	result &= de_object_visitor_visit_float(visitor, "CrouchBodyRadius", &player->crouch_body_radius);
	result &= de_object_visitor_visit_float(visitor, "StandUpSpeed", &player->stand_up_speed);
	result &= de_object_visitor_visit_float(visitor, "SitDownSpeed", &player->sit_down_speed);
	result &= de_object_visitor_visit_float(visitor, "RunSpeedMultiplier", &player->run_speed_multiplier);
	result &= de_object_visitor_visit_float(visitor, "CameraWobble", &player->camera_wobble);
	result &= de_object_visitor_visit_float(visitor, "PathLength", &player->path_len);
	result &= de_object_visitor_visit_vec3(visitor, "CameraOffset", &player->camera_offset);
	result &= de_object_visitor_visit_vec3(visitor, "CameraDestOffset", &player->camera_dest_offset);
	result &= de_object_visitor_visit_vec3(visitor, "CameraPosition", &player->camera_position);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Footsteps", player->footsteps, de_sound_source_visit);
	return result;
}

void level_create_collider(level_t* level) {
	de_node_t* polygon = de_scene_find_node(level->scene, "Polygon");
	if (polygon) {
		de_static_geometry_t* map_collider;
		assert(polygon->type == DE_NODE_TYPE_MESH);
		map_collider = de_scene_create_static_geometry(level->scene);
		de_node_calculate_transforms(polygon);
		de_static_geometry_fill(map_collider, de_node_to_mesh(polygon), polygon->global_matrix);
	}
}

bool level_visit(de_object_visitor_t* visitor, level_t* level) {
	bool result = true;
	if (visitor->is_reading) {
		level->game = de_core_get_user_pointer(visitor->core);
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Scene", &level->scene, de_scene_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Player", &level->player, player_visit);
	if (visitor->is_reading) {
		level_create_collider(level);
	}
	return result;
}

void player_free(player_t* p) {
	for (size_t i = 0; i < p->weapons.size; ++i) {
		weapon_free(p->weapons.data[i]);
	}
	DE_ARRAY_FREE(p->weapons);
	for (size_t i = 0; i < p->footsteps.size; ++i) {
		de_sound_source_free(p->footsteps.data[i]);
	}
	DE_ARRAY_FREE(p->footsteps);
	de_node_free(p->pivot);
	de_free(p);
}

void player_update(player_t* p) {
	de_node_t *pivot, *camera;
	de_vec3_t right_axis = { 1, 0, 0 };
	de_vec3_t up_axis = { 0, 1, 0 };
	de_vec3_t look, side, offset;
	de_quat_t pitch_rot, yaw_rot;
	float speed_multiplier = 1.0f;
	bool is_moving = false;

	pivot = p->pivot;
	camera = p->camera;

	offset = (de_vec3_t) { 0 };
	look = (de_vec3_t) { 0 };
	side = (de_vec3_t) { 0 };
	de_node_get_look_vector(pivot, &look);
	de_node_get_side_vector(pivot, &side);

	/* movement */
	if (p->controller.move_forward) {
		de_vec3_sub(&offset, &offset, &look);
		is_moving = true;
	}
	if (p->controller.move_backward) {
		de_vec3_add(&offset, &offset, &look);
		is_moving = true;
	}
	if (p->controller.strafe_left) {
		de_vec3_sub(&offset, &offset, &side);
		is_moving = true;
	}
	if (p->controller.strafe_right) {
		de_vec3_add(&offset, &offset, &side);
		is_moving = true;
	}

	/* crouch */
	if (p->controller.crouch) {
		float radius = de_body_get_radius(p->body);
		radius -= p->sit_down_speed;
		if (radius < p->crouch_body_radius) {
			radius = p->crouch_body_radius;
		}
		de_body_set_radius(p->body, radius);
	} else {
		float radius = de_body_get_radius(p->body);
		radius += p->stand_up_speed;
		if (radius > p->stand_body_radius) {
			radius = p->stand_body_radius;
		}
		de_body_set_radius(p->body, radius);
	}

	/* make sure that camera will be always at the top of the body */
	p->camera_position.y = de_body_get_radius(p->body);

	/* apply camera wobbling */
	if (is_moving && de_body_get_contact_count(p->body) > 0) {
		p->camera_dest_offset.x = 0.05f * (float)cos(p->camera_wobble * 0.5f);
		p->camera_dest_offset.y = 0.1f * (float)sin(p->camera_wobble);

		p->camera_wobble += 0.25f;
	} else {
		p->camera_dest_offset.x = 0;
		p->camera_dest_offset.y = 0;
	}

	/* camera offset will follow destination offset -> smooth movements */
	p->camera_offset.x += (p->camera_dest_offset.x - p->camera_offset.x) * 0.1f;
	p->camera_offset.y += (p->camera_dest_offset.y - p->camera_offset.y) * 0.1f;
	p->camera_offset.z += (p->camera_dest_offset.z - p->camera_offset.z) * 0.1f;

	/* set actual camera position */
	{
		de_vec3_t combined_position;
		de_vec3_add(&combined_position, &p->camera_position, &p->camera_offset);
		de_node_set_local_position(p->camera, &combined_position);
	}

	/* run */
	if (p->controller.run) {
		speed_multiplier = p->run_speed_multiplier;
	}
	
	if (de_vec3_sqr_len(&offset) > 0) {
		de_vec3_normalize(&offset, &offset);
		de_vec3_scale(&offset, &offset, speed_multiplier * p->move_speed);
		p->path_len += 0.05f;
		if (p->path_len >= 1) {
			de_sound_source_t* src = p->footsteps.data[rand() % p->footsteps.size];
			if (src) {
				de_sound_source_play(src);
				de_vec3_t pos;
				de_node_get_global_position(p->pivot, &pos);
				de_sound_source_set_position(src, &pos);
			}
			p->path_len = 0;
		}
	}

	p->yaw += (p->desired_yaw - p->yaw) * 0.22f;
	p->pitch += (p->desired_pitch - p->pitch) * 0.22f;

	de_body_set_x_velocity(p->body, 0);
	de_body_set_z_velocity(p->body, 0);

	de_body_move(p->body, &offset);

	de_node_set_local_rotation(pivot, de_quat_from_axis_angle(&yaw_rot, &up_axis, de_deg_to_rad(p->yaw)));
	de_node_set_local_rotation(camera, de_quat_from_axis_angle(&pitch_rot, &right_axis, de_deg_to_rad(p->pitch)));

	/* listener */
	de_vec3_t pos;
	de_sound_context_t* ctx = de_core_get_sound_context(p->parent_level->game->core);
	de_listener_t* lst = de_sound_context_get_listener(ctx);
	de_node_get_global_position(p->camera, &pos);
	de_listener_set_orientation(lst, &look, &up_axis);
	de_listener_set_position(lst, &pos);
}

level_t* level_create_test(game_t* game) {
	level_t* level;

	level = DE_NEW(level_t);
	level->game = game;
	level->scene = de_scene_create(game->core);

	/* Ripper */
	{
		de_path_t res_path;
		de_path_init(&res_path);
		de_path_append_cstr(&res_path, "data/models/ripper.fbx");
		de_resource_t* mdl_res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_MODEL, &res_path, 0);
		de_model_t* mdl = de_resource_to_model(mdl_res);
		de_node_t* ripper1 = de_model_instantiate(mdl, level->scene);
		de_node_set_local_position(ripper1, &(de_vec3_t){ -1, 0, -1 });
		de_node_t* ripper2 = de_model_instantiate(mdl, level->scene);
		de_node_set_local_position(ripper2, &(de_vec3_t){ 1, 0, -1 });
		de_node_t* ripper3 = de_model_instantiate(mdl, level->scene);
		de_node_set_local_position(ripper3, &(de_vec3_t){ 1, 0, 1 });
		de_node_t* ripper4 = de_model_instantiate(mdl, level->scene);
		de_node_set_local_position(ripper4, &(de_vec3_t){ -1, 0, 1 });
		de_path_free(&res_path);
	}

	/* Level */
	{
		de_path_t res_path;
		de_path_init(&res_path);
		de_path_append_cstr(&res_path, "data/models/map2_bin.fbx");
		de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_MODEL, &res_path, 0);
		de_model_t* mdl = de_resource_to_model(res);
		de_model_instantiate(mdl, level->scene);
		de_path_free(&res_path);
	}

	level_create_collider(level);

	level->player = player_create(level);	
	de_node_t* pp = de_scene_find_node(level->scene, "PlayerPosition");
	if (pp) {
		de_vec3_t pos;
		de_node_get_global_position(pp, &pos);
		de_node_set_local_position(level->player->pivot, &pos);
	}

	return level;
}

void level_update(level_t* level) {
	player_update(level->player);
}

void level_free(level_t* level) {
	player_free(level->player);
	de_scene_free(level->scene);
	de_free(level);
}

void quit_on_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	de_core_stop(game->core);
}

void main_menu_set_visible(main_menu_t* menu, bool visibility) {
	de_gui_node_set_visibility(menu->window, visibility ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	menu->visible = visibility;
}

void main_menu_on_new_game_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;

	DE_UNUSED(node);

	if (!game->level) {
		game->level = level_create_test(game);
	}

	main_menu_set_visible(game->main_menu, false);
}

bool game_save(game_t* game) {
	bool result = false;
	if (game->level) {
		result = true;
		de_object_visitor_t visitor;
		de_object_visitor_init(game->core, &visitor);
		result &= de_core_visit(&visitor, game->core);
		result &= DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Level", &game->level, level_visit);
		/* debug output */
		{
			FILE* temp = fopen("save1.txt", "w");
			de_object_visitor_print_tree(&visitor, temp);
			fclose(temp);
		}
		de_object_visitor_save_binary(&visitor, "save1.bin");
		de_object_visitor_free(&visitor);
	}
	return result;
}

bool game_load(game_t* game) {
	de_object_visitor_t visitor;
	bool result = true;
	de_object_visitor_load_binary(game->core, &visitor, "save1.bin");
	result &= de_core_visit(&visitor, game->core);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Level", &game->level, level_visit);
	de_object_visitor_free(&visitor);
	main_menu_set_visible(game->main_menu, false);
	return result;
}

void main_menu_on_save_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_save(game);
}

void main_menu_on_load_click(de_gui_node_t* node, void* user_data) {
	game_t* game = (game_t*)user_data;
	DE_UNUSED(node);
	game_load(game);
}

static void videomode_selector_item_getter(void* items, int n, char* out_buffer, int out_buffer_size) {
	de_video_mode_t* videomode = ((de_video_mode_t*)items) + n;
	snprintf(out_buffer, out_buffer_size, "%dx%d@%d", videomode->width, videomode->height, videomode->bits_per_pixel);
}

static void videomode_selector_on_selection_changed(de_gui_node_t* node, int n) {
	main_menu_t* menu;
	DE_ASSERT_GUI_NODE_TYPE(node, DE_GUI_NODE_SLIDE_SELECTOR);
	menu = (main_menu_t*)node->user_data;
	de_core_set_video_mode(menu->game->core, &menu->video_modes.data[n]);
}

main_menu_t* main_menu_create(game_t* game) {
	de_gui_t* gui = de_core_get_gui(game->core);
	main_menu_t* menu = DE_NEW(main_menu_t);
	menu->game = game;

	{
		de_path_t path;
		de_path_from_cstr_as_view(&path, "data/sounds/test.wav");
		de_resource_t* res = de_core_request_resource(game->core, DE_RESOURCE_TYPE_SOUND_BUFFER, &path, 
			DE_RESOURCE_FLAG_PERSISTENT);
		de_sound_buffer_t* buffer = de_resource_to_sound_buffer(res);
		de_sound_buffer_set_flags(buffer, DE_SOUND_BUFFER_FLAGS_STREAM);
		menu->music = de_sound_source_create(de_core_get_sound_context(game->core), DE_SOUND_SOURCE_TYPE_2D);
		de_sound_source_set_buffer(menu->music, de_resource_to_sound_buffer(res));
		de_sound_source_play(menu->music);
	}
	/* main window */
	{
		de_gui_node_t* grid;
		float window_width = 400;
		float window_height = 500;
		float window_x = (de_core_get_window_width(game->core) - window_width) * 0.5f;
		float window_y = (de_core_get_window_height(game->core) - window_height) * 0.5f;

		menu->window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
		de_gui_node_set_desired_size(menu->window, window_width, window_height);
		de_gui_node_set_desired_local_position(menu->window, window_x, window_y);
		de_gui_window_set_flags(menu->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);

		grid = de_gui_node_create(gui, DE_GUI_NODE_GRID);
		de_gui_node_set_desired_size(grid, 100, 100);
		de_gui_node_set_desired_local_position(grid, 10, 10);
		de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 30, DE_GUI_SIZE_MODE_STRICT);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);

		/* title */
		{
			de_gui_node_t* title = de_gui_node_create(gui, DE_GUI_NODE_TEXT);
			de_gui_text_set_text_utf8(title, "CGame");
			de_gui_node_set_row(title, 0);
			de_gui_node_set_column(title, 0);
			de_gui_node_attach(title, grid);
			de_gui_text_set_vertical_alignment(title, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
			de_gui_text_set_horizontal_alignment(title, DE_GUI_HORIZONTAL_ALIGNMENT_CENTER);
		}

		/* new game */
		{
			de_gui_node_t* new_game = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
			de_gui_text_set_text_utf8(de_gui_button_get_text(new_game), "New Game");
			de_gui_node_set_row(new_game, 1);
			de_gui_node_attach(new_game, grid);
			de_gui_node_set_margin_uniform(new_game, 10);
			de_gui_button_set_click(new_game, main_menu_on_new_game_click, game);
		}

		/* save game */
		{
			de_gui_node_t* save = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
			de_gui_text_set_text_utf8(de_gui_button_get_text(save), "Save Game");
			de_gui_node_set_row(save, 2);
			de_gui_node_attach(save, grid);
			de_gui_node_set_margin_uniform(save, 10);
			de_gui_button_set_click(save, main_menu_on_save_click, game);
		}

		/* load game */
		{
			de_gui_node_t* load = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
			de_gui_text_set_text_utf8(de_gui_button_get_text(load), "Load Game");
			de_gui_node_set_row(load, 3);
			de_gui_node_attach(load, grid);
			de_gui_node_set_margin_uniform(load, 10);
			de_gui_button_set_click(load, main_menu_on_load_click, game);
		}

		/* settings */
		{
			de_gui_node_t* settings = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
			de_gui_text_set_text_utf8(de_gui_button_get_text(settings), "Settings");
			de_gui_node_set_row(settings, 4);
			de_gui_node_attach(settings, grid);
			de_gui_node_set_margin_uniform(settings, 10);
		}

		/* quit */
		{
			de_gui_node_t* quit = de_gui_node_create(gui, DE_GUI_NODE_BUTTON);
			de_gui_text_set_text_utf8(de_gui_button_get_text(quit), "Quit");
			de_gui_node_set_row(quit, 5);
			de_gui_node_attach(quit, grid);
			de_gui_node_set_margin_uniform(quit, 10);
			de_gui_button_set_click(quit, quit_on_click, game);
		}

		de_gui_window_set_content(menu->window, grid);
	}

	/* settings window */
	{
		de_gui_node_t* settings_window = de_gui_node_create(gui, DE_GUI_NODE_WINDOW);
		de_gui_node_set_desired_size(settings_window, 300, 400);
		de_gui_text_set_text_utf8(de_gui_window_get_title(settings_window), "Settings");

		/* content */
		{
			de_gui_node_t* grid = de_gui_node_create(gui, DE_GUI_NODE_GRID);
			de_gui_grid_add_column(grid, 100, DE_GUI_SIZE_MODE_STRICT);
			de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
			de_gui_grid_add_row(grid, 30, DE_GUI_SIZE_MODE_STRICT);
			de_gui_grid_add_row(grid, 100, DE_GUI_SIZE_MODE_STRICT);

			/* videomode */
			{
				de_gui_node_t* selector;
				de_gui_node_t* videomode_text =de_gui_node_create(gui, DE_GUI_NODE_TEXT);
				de_gui_text_set_text_utf8(videomode_text, "Video Mode");
				de_gui_node_set_row(videomode_text, 0);
				de_gui_node_set_column(videomode_text, 0);
				de_gui_text_set_vertical_alignment(videomode_text, DE_GUI_VERTICAL_ALIGNMENT_CENTER);
				de_gui_node_attach(videomode_text, grid);

				selector = de_gui_node_create(gui, DE_GUI_NODE_SLIDE_SELECTOR);
				de_gui_node_set_user_data(selector, menu);
				de_gui_node_set_row(selector, 0);
				de_gui_node_set_column(selector, 1);
				de_gui_node_attach(selector, grid);
				de_gui_slide_selector_set_selection_changed(selector, videomode_selector_on_selection_changed);
				menu->video_modes = de_enum_video_modes();
				de_gui_slide_selector_set_items(selector, menu->video_modes.data, menu->video_modes.size, videomode_selector_item_getter);
			}

			/* password */
			{
				de_gui_node_t* text_box = de_gui_node_create(gui, DE_GUI_NODE_TEXT_BOX);
				de_gui_node_set_row(text_box, 1);
				de_gui_node_set_column(text_box, 1);
				de_gui_node_attach(text_box, grid);
			}

			de_gui_window_set_content(settings_window, grid);
		}
	}

	return menu;
}

void main_menu_free(main_menu_t* menu) {
	DE_ARRAY_FREE(menu->video_modes);
	de_sound_source_free(menu->music);
	de_free(menu);
}

game_t* game_create(void) {
	game_t* game;
	de_engine_params_t params;

	game = DE_NEW(game_t);
	de_log_open("dengine.log");

	/* Init core */
	de_zero(&params, sizeof(params));
	params.video_mode.width = 1200;
	params.video_mode.height = 1000;
	params.video_mode.bits_per_pixel = 32;
	params.video_mode.fullscreen = false;
	game->core = de_core_init(&params);
	de_core_set_user_pointer(game->core, game);
	de_renderer_set_framerate_limit(de_core_get_renderer(game->core), 0);

	/* Create menu */
	game->main_menu = main_menu_create(game);

	/* Create overlay */
	game->fps_text = de_gui_node_create(de_core_get_gui(game->core), DE_GUI_NODE_TEXT);

	return game;
}

void game_main_loop(game_t* game) {
	double game_clock, fixed_fps, fixed_timestep, dt;
	de_event_t evt;
	de_renderer_t* renderer;
	de_gui_t* gui;

	renderer = de_core_get_renderer(game->core);
	gui = de_core_get_gui(game->core);

	fixed_fps = 60.0;
	fixed_timestep = 1.0f / fixed_fps;
	game_clock = de_time_get_seconds();
	while (de_core_is_running(game->core)) {
		dt = de_time_get_seconds() - game_clock;
		while (dt >= fixed_timestep) {
			if (dt >= 4 * fixed_timestep) {
				game_clock = de_time_get_seconds();
				break;
			}

			dt -= fixed_timestep;
			game_clock += fixed_timestep;

			while (de_core_poll_event(game->core, &evt)) {
				bool processed = false;
				if (evt.type == DE_EVENT_TYPE_KEY_DOWN && evt.s.key.key == DE_KEY_ESC) {
					main_menu_set_visible(game->main_menu, true);
					processed = true;
				}
				if (!processed) {
					processed = de_gui_process_event(gui, &evt);
				}
				if (!processed) {
					if (game->level) {
						processed = player_process_event(game->level->player, &evt);
					}
				}
			}

			de_sound_context_update(de_core_get_sound_context(game->core));
			de_gui_update(gui);
			de_physics_step(game->core, fixed_timestep);
			DE_LINKED_LIST_FOR_EACH_H(de_scene_t*, scene, de_core_get_first_scene(game->core)) {
				de_scene_update(scene, fixed_timestep);
			}

			if (game->level && !game->main_menu->visible) {
				level_update(game->level);
			}
		}

		de_renderer_render(renderer);

		/* print statistics */
		{
			char buffer[256];
			double frame_time;
			size_t fps;

			frame_time = de_render_get_frame_time(renderer);
			fps = de_renderer_get_fps(renderer);

			snprintf(buffer, sizeof(buffer), "Frame time: %.2f ms\nFPS: %d\nAllocations: %d",
				frame_time,
				(int)fps,
				(int)de_get_alloc_count());
			de_gui_text_set_text_utf8(game->fps_text, buffer);
		}
	}
}

void game_close(game_t* game) {
	if (game->level) {
		level_free(game->level);
	}

	main_menu_free(game->main_menu);

	de_core_shutdown(game->core);

	de_free(game);
}

int main(int argc, char** argv) {
	game_t* game;

	DE_UNUSED(argc);
	DE_UNUSED(argv);

	game = game_create();

	game_main_loop(game);

	game_close(game);

	return 0;
}
