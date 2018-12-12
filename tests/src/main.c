#define DE_IMPLEMENTATION
#include "de_main.h"

typedef struct level_t
{
	de_scene_t* scene;
} level_t;

typedef struct player_t
{
	level_t* parent_level;
	de_node_t* pivot;
	de_node_t* camera;
	de_node_t* flash_light;
	float pitch;
	float yaw;
} player_t;

player_t* player_create(level_t* level)
{
	de_vec3_t pos;
	de_body_t* body;
	player_t* p;

	p = DE_NEW(player_t);

	p->parent_level = level;

	body = de_scene_create_body(level->scene);
	de_body_set_radius(body, 0.25);
	#if 0
	{
		de_vec3_t gravity = { 0, 0, 0 };
		de_body_set_gravity(body, &gravity);
	}
	#endif

	p->pivot = de_node_create(DE_NODE_BASE);
	de_scene_add_node(level->scene, p->pivot);
	de_node_set_body(p->pivot, body);
	de_node_set_local_position(p->pivot, de_vec3_set(&pos, 0, 1, 0));

	p->camera = de_node_create(DE_NODE_CAMERA);
	de_scene_add_node(level->scene, p->camera);
	de_node_set_local_position(p->camera, de_vec3_set(&pos, 0, 0.88f, 0));
	de_node_attach(p->camera, p->pivot);

	p->flash_light = de_node_create(DE_NODE_LIGHT);
	de_light_set_radius(p->flash_light, 4);
	de_scene_add_node(level->scene, p->flash_light);
	de_node_attach(p->flash_light, p->camera);

	return p;
}

void player_free(player_t* p)
{
	de_node_free(p->pivot);
	de_free(p);
}

void player_update(player_t* p)
{
	int k;
	de_node_t *pivot, *camera;
	de_vec3_t right_axis = { 1, 0, 0 };
	de_vec3_t up_axis = { 0, 1, 0 };
	de_vec3_t look, side, offset;
	de_quat_t pitch_rot, yaw_rot;
	de_vec2_t mouse_vel;
	float move_speed = 0.1f;
	de_body_t* body;

	pivot = p->pivot;
	camera = p->camera;

	body = pivot->body;

	de_get_mouse_velocity(&mouse_vel);

	de_vec3_zero(&offset);
	de_node_get_look_vector(pivot, &look);
	de_node_get_side_vector(pivot, &side);

	if (de_is_key_pressed(DE_KEY_W))
	{
		/*de_vec3_add(&offset, &offset, &look);*/
		de_vec3_sub(&offset, &offset, &look);
	}
	if (de_is_key_pressed(DE_KEY_S))
	{
		/*de_vec3_sub(&offset, &offset, &look);*/
		de_vec3_add(&offset, &offset, &look);
	}
	if (de_is_key_pressed(DE_KEY_A))
	{
		de_vec3_sub(&offset, &offset, &side);
	}
	if (de_is_key_pressed(DE_KEY_D))
	{
		de_vec3_add(&offset, &offset, &side);
	}
	if (de_is_key_pressed(DE_KEY_Space))
	{
		for (k = 0; k < body->contact_count; ++k)
		{
			if (body->contacts[k].normal.y > 0.7f)
			{
				de_body_set_y_velocity(body, 0.075f);
				break;
			}
		}
	}
	if (de_is_key_pressed(DE_KEY_C))
	{
		de_vec3_sub(&offset, &offset, &up_axis);
	}

	de_vec3_normalize(&offset, &offset);
	de_vec3_scale(&offset, &offset, move_speed);

	p->yaw -= mouse_vel.x;
	p->pitch -= mouse_vel.y;

	if (p->pitch > 90.0)
	{
		p->pitch = 90.0;
	}
	if (p->pitch < -90.0)
	{
		p->pitch = -90.0;
	}

	de_body_set_x_velocity(body, 0);
	de_body_set_z_velocity(body, 0);

	de_body_move(body, &offset);

	de_node_set_local_rotation(pivot, de_quat_from_axis_angle(&yaw_rot, &up_axis, de_deg_to_rad(p->yaw)));
	de_node_set_local_rotation(camera, de_quat_from_axis_angle(&pitch_rot, &right_axis, de_deg_to_rad(p->pitch)));
}

level_t* level_create_test()
{
	level_t* level;
	de_node_t* polygon;

	level = DE_NEW(level_t);

	level->scene = de_scene_create();

	de_fbx_load_to_scene(level->scene, "data/models/map2.fbx");
	polygon = de_scene_find_node(level->scene, "Polygon");

	if (polygon)
	{
		de_static_geometry_t* map_collider;
		assert(polygon->type == DE_NODE_MESH);
		map_collider = de_scene_create_static_geometry(level->scene);
		de_node_calculate_transforms(polygon);
		de_static_geometry_fill(map_collider, &polygon->s.mesh, polygon->global_matrix);
	}
	return level;
}

void level_update(level_t* level)
{
	(void)level;
}

void level_free(level_t* level)
{
	de_scene_free(level->scene);
	de_free(level);
}

void quit_on_click(de_gui_node_t* node)
{
	DE_UNUSED(node);
	de_stop();
}

void main_menu_create(void)
{
	#if 1
	/* test grid */
	{
		de_gui_node_t* test_grid = de_gui_grid_create();
		de_gui_node_set_desired_size(test_grid, 300, 630);
		de_gui_node_parse_property(test_grid, "DesiredPosition", "320, 10");		
		de_gui_grid_add_column(test_grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_add_row(test_grid, 0, DE_GUI_SIZE_MODE_STRETCH);
		de_gui_grid_enable_draw_borders(test_grid, DE_FALSE);

		/* scroll viewer */
		{
			de_gui_node_t* sv = de_gui_scroll_viewer_create();
			de_gui_node_set_row(sv, 6);
			de_gui_node_set_column(sv, 1);
			de_gui_node_attach(sv, test_grid);
			de_gui_node_set_margin_uniform(sv, 10);

			/* scroll viewer content */
			{
				de_gui_node_t* quit;
				de_gui_node_t* grid;

				grid = de_gui_grid_create();
				de_gui_node_set_desired_size(grid, 700, 700);
				de_gui_node_set_horizontal_alignment(grid, DE_GUI_HORIZONTAL_ALIGNMENT_LEFT);
				de_gui_node_set_vertical_alignment(grid, DE_GUI_VERTICAL_ALIGNMENT_TOP);
				de_gui_node_set_color_rgba(grid, 120, 120, 120, 255);
				de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
				de_gui_grid_add_column(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
				de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);
				de_gui_grid_add_row(grid, 0, DE_GUI_SIZE_MODE_STRETCH);

				de_gui_scroll_viewer_set_content(sv, grid);

				quit = de_gui_button_create();
				de_gui_text_set_text(de_gui_button_get_text(quit), "Quit");
				de_gui_text_set_alignment(de_gui_button_get_text(quit), DE_GUI_TA_CENTER);
				de_gui_node_set_row(quit, 1);
				de_gui_node_set_column(quit, 1);
				de_gui_node_attach(quit, grid);
				de_gui_node_set_margin_uniform(quit, 10);
				de_gui_button_set_click(quit, quit_on_click);
			}
		}

		/* test window */
		{
			de_gui_node_t* window = de_gui_window_create();
			de_gui_node_set_desired_size(window, 400, 500);
			de_gui_node_set_desired_local_position(window, 700, 500);
			de_gui_window_set_content(window, test_grid);
		}
	}

	/* layout */
	{
		de_gui_node_t* grid = de_gui_grid_create();
		de_gui_node_set_desired_size(grid, 300, 630);
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
			de_gui_node_t* title = de_gui_text_create();
			de_gui_text_set_text(title, "CGame");
			de_gui_node_set_row(title, 0);
			de_gui_node_set_column(title, 0);
			de_gui_node_attach(title, grid);
			de_gui_text_set_alignment(title, DE_GUI_TA_CENTER);
		}

		/* new game */
		{
			de_gui_node_t* new_game = de_gui_button_create();
			de_gui_text_set_text(de_gui_button_get_text(new_game), "New Game");
			de_gui_text_set_alignment(de_gui_button_get_text(new_game), DE_GUI_TA_CENTER);
			de_gui_node_set_row(new_game, 1);
			de_gui_node_attach(new_game, grid);
			de_gui_node_set_margin_uniform(new_game, 10);
		}

		/* save game */
		{
			de_gui_node_t* save = de_gui_button_create();
			de_gui_text_set_text(de_gui_button_get_text(save), "Save Game");
			de_gui_text_set_alignment(de_gui_button_get_text(save), DE_GUI_TA_CENTER);
			de_gui_node_set_row(save, 2);
			de_gui_node_attach(save, grid);
			de_gui_node_set_margin_uniform(save, 10);
		}

		/* load game */
		{
			de_gui_node_t* load = de_gui_button_create();
			de_gui_text_set_text(de_gui_button_get_text(load), "Load Game");
			de_gui_text_set_alignment(de_gui_button_get_text(load), DE_GUI_TA_CENTER);
			de_gui_node_set_row(load, 3);
			de_gui_node_attach(load, grid);
			de_gui_node_set_margin_uniform(load, 10);
		}

		/* settings */
		{
			de_gui_node_t* settings = de_gui_button_create();
			de_gui_text_set_text(de_gui_button_get_text(settings), "Settings");
			de_gui_text_set_alignment(de_gui_button_get_text(settings), DE_GUI_TA_CENTER);
			de_gui_node_set_row(settings, 4);
			de_gui_node_attach(settings, grid);
			de_gui_node_set_margin_uniform(settings, 10);
		}

		/* quit */
		{
			de_gui_node_t* quit = de_gui_button_create();
			de_gui_text_set_text(de_gui_button_get_text(quit), "Quit");
			de_gui_text_set_alignment(de_gui_button_get_text(quit), DE_GUI_TA_CENTER);
			de_gui_node_set_row(quit, 5);
			de_gui_node_attach(quit, grid);
			de_gui_node_set_margin_uniform(quit, 10);
			de_gui_button_set_click(quit, quit_on_click);

			{
				de_color_t c = { 255, 0, 0, 255 };
				de_gui_node_set_property(quit, "HoverColor", &c, sizeof(c));
			}
		}
	}
	#endif
}

int main(int argc, char** argv)
{
	float gameClock, fixedFPS, fixedTimeStep, dt;
	de_engine_params_t params;
	player_t* player;
	level_t* level;
	de_node_t* fbx2;
	de_vec3_t rp = { -1, 1, 1 };

	DE_UNUSED(argc);
	DE_UNUSED(argv);

	params.width = 1200;
	params.height = 1000;

	de_init(&params);
	de_set_framerate_limit(0);

	level = level_create_test();
	player = player_create(level);

	fbx2 = de_fbx_load_to_scene(level->scene, "data/models/ripper.fbx");
	de_node_set_local_position(fbx2, &rp);

	{
		de_node_t* pp = de_scene_find_node(level->scene, "PlayerPosition");
		if (pp)
		{
			de_vec3_t pos;
			de_node_get_global_position(pp, &pos);

			de_node_set_local_position(player->pivot, &pos);
		}
	}

	main_menu_create();

	fixedFPS = 60.0;
	fixedTimeStep = 1.0f / fixedFPS;
	gameClock = de_time_get_seconds();
	while (de_is_running())
	{
		if (de_is_key_pressed(DE_KEY_ESC))
		{
			de_stop();
		}

		dt = de_time_get_seconds() - gameClock;
		while (dt >= fixedTimeStep)
		{
			dt -= fixedTimeStep;
			gameClock += fixedTimeStep;

			de_gui_update();
			de_poll_messages();
			de_physics_step(fixedTimeStep);

			level_update(level);
			player_update(player);
		}

		de_render();
	}

	player_free(player);
	level_free(level);
	de_shutdown();
	return 0;
}
