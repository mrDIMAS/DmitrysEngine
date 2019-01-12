#define DE_IMPLEMENTATION
#include "de_main.h"

typedef struct game_t game_t;
typedef struct main_menu_t main_menu_t;
typedef struct level_t level_t;
typedef struct player_t player_t;
typedef struct weapon_t weapon_t;

struct game_t
{
	de_core_t* core;
	level_t* level;
	main_menu_t* main_menu;
	de_gui_node_t* fps_text;
};

struct main_menu_t
{
	de_gui_node_t* window;
	bool visible;
};

struct level_t
{
	de_scene_t* scene;
	de_node_t* test_fbx;
	player_t* player;
};

typedef enum weapon_type_t
{
	WEAPON_TYPE_SHOTGUN,
} weapon_type_t;

struct weapon_t
{
	weapon_type_t type;
	de_node_t* model;
};

struct player_t
{
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
	de_vec3_t camera_offset;
	de_vec3_t camera_dest_offset;
	de_vec3_t camera_position;
	DE_ARRAY_DECLARE(weapon_t*, weapons);
};

/*=======================================================================================*/
weapon_t* weapon_create(level_t* level, weapon_type_t type)
{
	weapon_t* wpn = DE_NEW(weapon_t);

	wpn->type = type;

	if (type == WEAPON_TYPE_SHOTGUN)
	{
		wpn->model = de_fbx_load_to_scene(level->scene, "data/models/shotgun.fbx");
	}
	else
	{
		de_log("invalid weapon type");
	}

	return wpn;
}

/*=======================================================================================*/
void weapon_free(weapon_t* wpn)
{
	de_node_free(wpn->model);
	de_free(wpn);
}

/*=======================================================================================*/
void weapon_update(weapon_t* wpn)
{
	DE_UNUSED(wpn);
}

/*=======================================================================================*/
player_t* player_create(level_t* level)
{
	player_t* p;

	p = DE_NEW(player_t);

	p->parent_level = level;
	p->move_speed = 0.028f;
	p->stand_body_radius = 0.5f;
	p->crouch_body_radius = 0.35f;
	p->sit_down_speed = 0.045f;
	p->stand_up_speed = 0.06f;
	p->run_speed_multiplier = 1.75f;
	de_vec3_set(&p->camera_position, 0, p->stand_body_radius, 0);

	p->body = de_scene_create_body(level->scene);
	de_body_set_radius(p->body, p->stand_body_radius);

	p->pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->pivot);
	de_node_set_body(p->pivot, p->body);

	p->camera = de_node_create(level->scene, DE_NODE_TYPE_CAMERA);
	de_scene_add_node(level->scene, p->camera);
	de_node_set_local_position(p->camera, &p->camera_position);
	de_node_attach(p->camera, p->pivot);

	p->flash_light = de_node_create(level->scene, DE_NODE_TYPE_LIGHT);
	de_light_set_radius(p->flash_light, 4);
	de_scene_add_node(level->scene, p->flash_light);
	de_node_attach(p->flash_light, p->camera);

	p->weapon_pivot = de_node_create(level->scene, DE_NODE_TYPE_BASE);
	de_scene_add_node(level->scene, p->weapon_pivot);
	de_node_attach(p->weapon_pivot, p->camera);
	de_node_set_local_position_xyz(p->weapon_pivot, 0.03f, -0.052f, -0.02f);

#if 0
	{
		weapon_t* shotgun = weapon_create(level, WEAPON_TYPE_SHOTGUN);
		de_node_attach(shotgun->model, p->weapon_pivot);
		DE_ARRAY_APPEND(p->weapons, shotgun);
	}
#endif

	return p;
}

/*=======================================================================================*/
void player_free(player_t* p)
{
	size_t i;

	for (i = 0; i < p->weapons.size; ++i)
	{
		weapon_free(p->weapons.data[i]);
	}
	DE_ARRAY_FREE(p->weapons);

	de_node_free(p->pivot);
	de_free(p);
}

/*=======================================================================================*/
void player_update(player_t* p)
{
	size_t k;
	de_core_t* core = p->parent_level->scene->core;
	de_node_t *pivot, *camera;
	de_vec3_t right_axis = { 1, 0, 0 };
	de_vec3_t up_axis = { 0, 1, 0 };
	de_vec3_t look, side, offset;
	de_quat_t pitch_rot, yaw_rot;
	de_vec2_t mouse_vel;
	float speed_multiplier = 1.0f;
	bool is_moving = false;

	pivot = p->pivot;
	camera = p->camera;

	de_get_mouse_velocity(core, &mouse_vel);

	de_vec3_zero(&offset);
	de_node_get_look_vector(pivot, &look);
	de_node_get_side_vector(pivot, &side);

	/* movement */
	if (de_is_key_pressed(core, DE_KEY_W))
	{
		de_vec3_sub(&offset, &offset, &look);

		is_moving = true;
	}
	if (de_is_key_pressed(core, DE_KEY_S))
	{
		de_vec3_add(&offset, &offset, &look);

		is_moving = true;
	}
	if (de_is_key_pressed(core, DE_KEY_A))
	{
		de_vec3_sub(&offset, &offset, &side);

		is_moving = true;
	}
	if (de_is_key_pressed(core, DE_KEY_D))
	{
		de_vec3_add(&offset, &offset, &side);

		is_moving = true;
	}

	/* jump */
	if (de_is_key_pressed(core, DE_KEY_Space))
	{
		size_t contact_count = de_body_get_contact_count(p->body);

		for (k = 0; k < contact_count; ++k)
		{
			de_contact_t* contact = de_body_get_contact(p->body, k);

			if (contact->normal.y > 0.7f)
			{
				de_body_set_y_velocity(p->body, 0.075f);

				break;
			}
		}
	}

	/* crouch */
	if (de_is_key_pressed(core, DE_KEY_C))
	{
		float radius = de_body_get_radius(p->body);
		radius -= p->sit_down_speed;
		if (radius < p->crouch_body_radius)
		{
			radius = p->crouch_body_radius;
		}
		de_body_set_radius(p->body, radius);
	}
	else
	{
		float radius = de_body_get_radius(p->body);
		radius += p->stand_up_speed;
		if (radius > p->stand_body_radius)
		{
			radius = p->stand_body_radius;
		}
		de_body_set_radius(p->body, radius);
	}

	/* make sure that camera will be always at the top of the body */
	p->camera_position.y = p->body->radius;

	/* apply camera wobbling */
	if (is_moving && de_body_get_contact_count(p->body) > 0)
	{
		p->camera_dest_offset.x = 0.05f * (float)cos(p->camera_wobble * 0.5f);
		p->camera_dest_offset.y = 0.1f * (float)sin(p->camera_wobble);

		p->camera_wobble += 0.25f;
	}
	else
	{
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
	if (de_is_key_pressed(core, DE_KEY_LSHIFT))
	{
		speed_multiplier = p->run_speed_multiplier;
	}

	de_vec3_normalize(&offset, &offset);
	de_vec3_scale(&offset, &offset, speed_multiplier * p->move_speed);

	p->desired_yaw -= mouse_vel.x;
	p->desired_pitch -= mouse_vel.y;

	if (p->desired_pitch > 90.0)
	{
		p->desired_pitch = 90.0;
	}
	if (p->desired_pitch < -90.0)
	{
		p->desired_pitch = -90.0;
	}

	p->yaw += (p->desired_yaw - p->yaw) * 0.22f;
	p->pitch += (p->desired_pitch - p->pitch) * 0.22f;

	de_body_set_x_velocity(p->body, 0);
	de_body_set_z_velocity(p->body, 0);

	de_body_move(p->body, &offset);

	de_node_set_local_rotation(pivot, de_quat_from_axis_angle(&yaw_rot, &up_axis, de_deg_to_rad(p->yaw)));
	de_node_set_local_rotation(camera, de_quat_from_axis_angle(&pitch_rot, &right_axis, de_deg_to_rad(p->pitch)));
}

/*=======================================================================================*/
level_t* level_create_test(game_t* game)
{
	level_t* level;
	de_node_t* polygon;
	de_vec3_t rp = { -1, 1, 1 };

	level = DE_NEW(level_t);

	level->scene = de_scene_create(game->core);

	level->test_fbx = de_fbx_load_to_scene(level->scene, "data/models/ripper.fbx");
	de_node_set_local_position(level->test_fbx, &rp);

	de_fbx_load_to_scene(level->scene, "data/models/map2_bin.fbx");
	polygon = de_scene_find_node(level->scene, "Polygon");

	if (polygon)
	{
		de_static_geometry_t* map_collider;
		assert(polygon->type == DE_NODE_TYPE_MESH);
		map_collider = de_scene_create_static_geometry(level->scene);
		de_node_calculate_transforms(polygon);
		de_static_geometry_fill(map_collider, de_node_to_mesh(polygon), polygon->global_matrix);
	}

	level->player = player_create(level);
	{
		de_node_t* pp = de_scene_find_node(level->scene, "PlayerPosition");
		if (pp)
		{
			de_vec3_t pos;
			de_node_get_global_position(pp, &pos);

			de_node_set_local_position(level->player->pivot, &pos);
		}
	}

	return level;
}

/*=======================================================================================*/
void level_update(level_t* level)
{
	player_update(level->player);
}

/*=======================================================================================*/
void level_free(level_t* level)
{
	player_free(level->player);
	de_scene_free(level->scene);
	de_free(level);
}

/*=======================================================================================*/
void quit_on_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;

	DE_UNUSED(node);

	de_core_stop(game->core);
}

/*=======================================================================================*/
void main_menu_set_visible(main_menu_t* menu, bool visibility)
{
	de_gui_node_set_visibility(menu->window, visibility ? DE_GUI_NODE_VISIBILITY_VISIBLE : DE_GUI_NODE_VISIBILITY_COLLAPSED);
	menu->visible = visibility;
}

/*=======================================================================================*/
void main_menu_on_new_game_click(de_gui_node_t* node, void* user_data)
{
	game_t* game = (game_t*)user_data;

	DE_UNUSED(node);

	if (!game->level)
	{
		game->level = level_create_test(game);
	}

	main_menu_set_visible(game->main_menu, false);
}

/*=======================================================================================*/
main_menu_t* main_menu_create(game_t* game)
{
	de_gui_t* gui = game->core->gui;
	main_menu_t* menu = DE_NEW(main_menu_t);

	/* main window */
	{
		float window_width = 400;
		float window_height = 500;
		float window_x = (de_core_get_window_width(game->core) - window_width) * 0.5f;
		float window_y = (de_core_get_window_height(game->core) - window_height) * 0.5f;

		menu->window = de_gui_window_create(gui);
		de_gui_node_set_desired_size(menu->window, window_width, window_height);
		de_gui_node_set_desired_local_position(menu->window, window_x, window_y);
		de_gui_window_set_flags(menu->window, DE_GUI_WINDOW_FLAGS_NO_MOVE);

		de_gui_node_t* grid = de_gui_grid_create(gui);
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
			de_gui_node_t* title = de_gui_text_create(gui);
			de_gui_text_set_text(title, "CGame");
			de_gui_node_set_row(title, 0);
			de_gui_node_set_column(title, 0);
			de_gui_node_attach(title, grid);
			de_gui_text_set_alignment(title, DE_GUI_TA_CENTER);
		}

		/* new game */
		{
			de_gui_node_t* new_game = de_gui_button_create(gui);
			de_gui_text_set_text(de_gui_button_get_text(new_game), "New Game");
			de_gui_node_set_row(new_game, 1);
			de_gui_node_attach(new_game, grid);
			de_gui_node_set_margin_uniform(new_game, 10);
			de_gui_button_set_click(new_game, main_menu_on_new_game_click, game);
		}

		/* save game */
		{
			de_gui_node_t* save = de_gui_button_create(gui);
			de_gui_text_set_text(de_gui_button_get_text(save), "Save Game");
			de_gui_node_set_row(save, 2);
			de_gui_node_attach(save, grid);
			de_gui_node_set_margin_uniform(save, 10);
		}

		/* load game */
		{
			de_gui_node_t* load = de_gui_button_create(gui);
			de_gui_text_set_text(de_gui_button_get_text(load), "Load Game");
			de_gui_node_set_row(load, 3);
			de_gui_node_attach(load, grid);
			de_gui_node_set_margin_uniform(load, 10);
		}

		/* settings */
		{
			de_gui_node_t* settings = de_gui_button_create(gui);
			de_gui_text_set_text(de_gui_button_get_text(settings), "Settings");
			de_gui_node_set_row(settings, 4);
			de_gui_node_attach(settings, grid);
			de_gui_node_set_margin_uniform(settings, 10);
		}

		/* quit */
		{
			de_gui_node_t* quit = de_gui_button_create(gui);
			de_gui_text_set_text(de_gui_button_get_text(quit), "Quit");
			de_gui_node_set_row(quit, 5);
			de_gui_node_attach(quit, grid);
			de_gui_node_set_margin_uniform(quit, 10);
			de_gui_button_set_click(quit, quit_on_click, game);
		}

		de_gui_window_set_content(menu->window, grid);
	}

	return menu;
}

/*=======================================================================================*/
void main_menu_free(main_menu_t* menu)
{
	de_free(menu);
}

/*=======================================================================================*/
game_t* game_create(void)
{
	game_t* game;

	game = DE_NEW(game_t);

	/* Init core */
	{
		de_engine_params_t params;
		params.width = 1200;
		params.height = 1000;

		game->core = de_core_init(&params);
		de_renderer_set_framerate_limit(game->core->renderer, 0);
	}

	/* Create menu */
	{
		game->main_menu = main_menu_create(game);
	}

	/* Create overlay */
	{
		game->fps_text = de_gui_text_create(game->core->gui);
	}

	//game->level = level_create_test(game);

	return game;
}

/*=======================================================================================*/
void game_main_loop(game_t* game)
{
	float game_clock;
	float fixed_fps;
	float fixed_timestep;
	float dt;
	de_renderer_t* renderer;

	renderer = de_core_get_renderer(game->core);

	fixed_fps = 60.0;
	fixed_timestep = 1.0f / fixed_fps;
	game_clock = de_time_get_seconds();
	while (de_core_is_running(game->core))
	{
		if (de_is_key_pressed(game->core, DE_KEY_ESC))
		{
			main_menu_set_visible(game->main_menu, true);
		}

		dt = de_time_get_seconds() - game_clock;
		while (dt >= fixed_timestep)
		{
			if (dt >= 4 * fixed_timestep)
			{
				game_clock = de_time_get_seconds();
				break;
			}

			dt -= fixed_timestep;
			game_clock += fixed_timestep;

			de_core_update(game->core, fixed_timestep);

			if (game->level && !game->main_menu->visible)
			{
				level_update(game->level);
			}
		}

		de_renderer_render(renderer);

		/* print statistics */
		{
			char buffer[256];
			float frame_time;
			size_t fps;

			frame_time = de_render_get_frame_time(renderer);
			fps = de_renderer_get_fps(renderer);

			snprintf(buffer, sizeof(buffer), "Frame time: %.2f ms\nFPS: %d\nAllocations: %d",
				frame_time,
				fps,
				de_get_alloc_count());
			de_gui_text_set_text(game->fps_text, buffer);
		}
	}
}

/*=======================================================================================*/
void game_close(game_t* game)
{
	if (game->level)
	{
		level_free(game->level);
	}

	main_menu_free(game->main_menu);

	de_core_shutdown(game->core);

	de_free(game);
}

void test_visitor(game_t* game)
{
	DE_UNUSED(game);

#if 0
	{
		de_object_visitor_t visitor;
		de_object_visitor_init(&visitor);

		de_scene_t* scene = de_scene_create(game->core);

		de_node_t* node = de_node_create(scene, DE_NODE_TYPE_BASE);
		de_scene_add_node(scene, node);
		de_node_set_name(node, "FooNode");

		de_node_t* node2 = de_node_create(scene, DE_NODE_TYPE_BASE);
		de_scene_add_node(scene, node2);
		de_node_set_name(node2, "Node");
		de_node_attach(node2, node);

		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Scene", &scene, de_scene_visit);
		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Node", &node, de_node_visit);
		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Node2", &node2, de_node_visit);
				
		FILE* temp = fopen("save.txt", "w");
		de_object_visitor_print_tree(&visitor, temp);
		fclose(temp);

		de_object_visitor_save_binary(&visitor, "save.bin");

		de_object_visitor_free(&visitor);
	}
#endif

#if 1
	{
		de_object_visitor_t visitor;
		de_object_visitor_load_binary(&visitor, "save.bin");

		de_scene_t* scene;
		de_node_t* node;
		de_node_t* node2;
		
		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Scene", &scene, de_scene_visit);
		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Node", &node, de_node_visit);
		DE_OBJECT_VISITOR_VISIT_POINTER(&visitor, "Node2", &node2, de_node_visit);

		de_object_visitor_free(&visitor);
	}
#endif
}

/*=======================================================================================*/
int main(int argc, char** argv)
{
	DE_UNUSED(argc);
	DE_UNUSED(argv);

	game_t* game = game_create();

	test_visitor(game);

	game_main_loop(game);

	game_close(game);

	return 0;
}
