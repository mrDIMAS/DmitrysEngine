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

typedef struct de_gbuffer_t {
	GLuint fbo;

	GLuint depth_rt;
	GLuint depth_buffer;
	GLuint depth_texture;

	GLuint color_rt;
	GLuint color_texture;

	GLuint normal_rt;
	GLuint normal_texture;

	GLuint opt_fbo;
	GLuint frame_texture;
} de_gbuffer_t;

typedef struct de_spot_shadow_map_t {
	GLuint fbo;
	GLuint texture;
} de_spot_shadow_map_t;

typedef struct de_point_shadow_map_t {
	GLuint fbo;
	GLuint texture;
	GLuint depth_buffer;
} de_point_shadow_map_t;

/**
* Shader for G-Buffer filling
*/
typedef struct de_gbuffer_shader_t {
	GLuint program;

	GLint world_matrix;
	GLint wvp_matrix;
	GLint use_skeletal_animation;
	GLint bone_matrices;

	GLint diffuse_texture; /**< Diffuse texture uniform location */
	GLint normal_texture;
} de_gbuffer_shader_t;

typedef struct de_gbuffer_light_shader_t {
	GLuint program;

	GLint wvp_matrix;

	GLint depth_sampler;
	GLint color_sampler;
	GLint normal_sampler;
	GLint spot_shadow_texture;
	GLint point_shadow_texture;
	GLint light_view_proj_matrix;
	GLint light_type;
	GLint soft_shadows;
	GLint shadow_map_inv_size;
	GLint light_position;
	GLint light_radius;
	GLint light_color;
	GLint light_direction;
	GLint light_cone_angle_cos;
	GLint inv_view_proj_matrix;
	GLint camera_position;
} de_deferred_light_shader_t;

typedef struct de_flat_shader_t {
	GLuint program;
	GLint wvp_matrix;
	GLint diffuse_texture;
} de_flat_shader_t;

typedef struct de_ambient_shader_t {
	GLuint program;
	GLint wvp_matrix;
	GLint diffuse_texture;
	GLint ambient_color;
} de_ambient_shader_t;

typedef struct de_gui_shader_t {
	GLuint program;
	GLint wvp_matrix;
	GLint diffuse_texture;
} de_gui_shader_t;

typedef struct de_particle_system_shader_t {
	GLuint program;
	struct {
		GLint view_projection_matrix;
		GLint world_matrix;
		GLint camera_up_vector;
		GLint camera_side_vector;
	} vs;
	struct {
		GLint diffuse_texture;
		GLint depth_buffer_texture;
		GLint inv_screen_size;
		GLint proj_params;
	} fs;
} de_particle_system_shader_t;

typedef struct de_spot_shadow_map_shader_t {
	GLuint program;

	struct {
		GLint world_view_projection_matrix;
		GLint use_skeletal_animation;
		GLint bone_matrices;
	} vs;

	struct {
		GLint diffuse_texture;
	} fs;
} de_spot_shadow_map_shader_t;

typedef struct de_point_shadow_map_shader_t {
	GLuint program;

	struct {
		GLint world_matrix;
		GLint world_view_projection_matrix;
		GLint use_skeletal_animation;
		GLint bone_matrices;
	} vs;

	struct {
		GLint diffuse_texture;
		GLint light_position;
	} fs;
} de_point_shadow_map_shader_t;

/* Screen-Space Ambient Occlusion (SSAO) shader */
typedef struct de_ssao_shader_t {
	GLuint program;

	struct {
		GLint world_view_projection_matrix;
	} vs;

	struct {
		GLint depth_texture;
		GLint normal_texture;
		GLint diffuse_texture;
	} ps;
} de_ssao_shader_t;

typedef struct de_renderer_quality_settings_t {
	/* Point shadows */
	size_t point_shadow_map_size; /**< Size of cube map face of shadow map texture in pixels. */
	bool point_soft_shadows; /**< Use or not percentage close filtering (smoothing) for point shadows. */
	bool point_shadows_enabled; /**< Point shadows enabled or not. */
	float point_shadows_distance; /**< Maximum distance from camera to draw shadows. */

	/* Spot shadows */
	size_t spot_shadow_map_size; /**< Size of square shadow map texture in pixels */
	bool spot_soft_shadows; /**< Use or not percentage close filtering (smoothing) for spot shadows. */
	bool spot_shadows_enabled; /**< Spot shadows enabled or not. */
	float spot_shadows_distance; /**< Maximum distance from camera to draw shadows. */

	uint32_t anisotropy_pow2; /**< Anisotropy level for textures 2^N -> 1, 2, 4, 8, 16. Maximum anisotropy level defined by GPU. */
} de_renderer_quality_settings_t;

typedef struct de_renderer_limits_t {
	float max_anisotropy;
} de_renderer_limits_t;

struct de_renderer_t {
	de_core_t* core;

	size_t frame_rate_limit;     /**< Maximum frames per seconds for renderer. 0 - unlimited */

	de_flat_shader_t flat_shader;
	de_gui_shader_t gui_shader;
	de_ambient_shader_t ambient_shader;
	de_particle_system_shader_t particle_system_shader;
	de_spot_shadow_map_shader_t spot_shadow_map_shader;
	de_point_shadow_map_shader_t point_shadow_map_shader;
	de_gbuffer_t gbuffer;
	de_gbuffer_shader_t gbuffer_shader;
	de_deferred_light_shader_t lighting_shader;
	de_ssao_shader_t ssao_shader;
	de_spot_shadow_map_t spot_shadow_map;
	de_point_shadow_map_t point_shadow_map;
	de_renderer_quality_settings_t quality_settings;
	de_renderer_limits_t limits;
	de_color_t ambient_light_color;

	de_surface_t* quad;
	de_surface_t* light_unit_sphere;

	de_surface_t * test_surface;
	de_texture_t* white_dummy;
	de_texture_t* normal_map_dummy;

	/* Debugging switches */
	bool render_normals;
	bool render_bones;

	struct {
		GLuint vbo;      /**< Vertex buffer object id */
		GLuint vao;      /**< Vertex array object id */
		GLuint ebo;      /**< Element buffer object id */
	} gui_render_buffers;

	/* Statistics (times given in milliseconds) */
	size_t draw_calls; /**< Exact amount of draw calls for one frame. */
	double frame_time; /**< Actual time amount last frame took to be rendered. */
	double frame_time_accumulator; /**< Total time of frames since last FPS was committed. */
	size_t frame_time_measurements; /**< Count of render calls since last FPS value was committed. */
	double time_last_fps_measured; /**< Time when FPS was committed. */
	size_t mean_fps; /**< Mean FPS. */
	size_t min_fps; /**< Minimum FPS. */
	size_t current_fps; /**< Current FPS. */
};

/**
* @brief Creates new surface.
*
* Also creates buffers on GPU side.
*/
de_surface_t* de_renderer_create_surface(de_renderer_t* r);

/**
* @brief Frees resources, deletes GPU-side buffers
* @param surf pointer to surface
*
* Pointer to surface will become invalid after calling this function
*/
void de_renderer_free_surface(de_surface_t* surf);

/**
* @brief Sets frame rate limit
* @param limit any positive value (i.e. 60)
*/
void de_renderer_set_framerate_limit(de_renderer_t* r, int limit);

/**
 * @brief Returns copy of current quality settings of the renderer.
 */
de_renderer_quality_settings_t de_renderer_get_quality_settings(de_renderer_t* r);

/**
 * @brief Gets default quality settings.
 */
de_renderer_quality_settings_t de_renderer_get_default_quality_settings(de_renderer_t* r);

/**
 * @brief Applies current quality settings.
 */
void de_renderer_apply_quality_settings(de_renderer_t* r, const de_renderer_quality_settings_t* settings);

/**
 * @brief Performs rendering of every scene
 */
void de_renderer_render(de_renderer_t* r);

/**
 * @brief Returns FPS.
 */
size_t de_renderer_get_mean_fps(de_renderer_t* r);

/**
 * @brief Returns time consumed by the renderer to draw one frame.
 */
double de_render_get_frame_time(de_renderer_t* r);

void de_renderer_set_ambient_light_color(de_renderer_t* r, const de_color_t* clr);

de_color_t de_renderer_get_ambient_light_color(de_renderer_t* r);

/**
 * Internal functions.
 * Do not use!
 */

/**
 * @brief Internal. Initializes rendering pipeline.
 */
de_renderer_t* de_renderer_init(de_core_t* core);

/**
 * @brief Internal. Frees all allocated resources.
 */
void de_renderer_free(de_renderer_t* r);

/**
 * @brief Internal. Notifies renderer that video mode has changed so renderer
 * can recreate framebuffers and other video mode-dependant stuff.
 */
void de_renderer_notify_video_mode_changed(de_renderer_t* r, const de_video_mode_t* new_video_mode);