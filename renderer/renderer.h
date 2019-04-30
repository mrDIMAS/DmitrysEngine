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


PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

#ifdef _WIN32
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
#endif

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;

PFNGLGETSTRINGIPROC glGetStringi;

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

/**
* Shader for G-Buffer filling
*/
typedef struct de_gbuffer_shader_t {
	GLuint program;

	GLuint world_matrix;
	GLuint wvp_matrix;
	GLuint use_skeletal_animation;
	GLuint bone_matrices;

	GLint diffuse_texture; /**< Diffuse texture uniform location */
	GLuint normal_texture;
	GLuint diffuse_color;
	GLuint self_emittance;
} de_gbuffer_shader_t;

typedef struct de_gbuffer_light_shader_t {
	GLuint program;

	GLuint wvp_matrix;

	GLuint depth_sampler;
	GLuint color_sampler;
	GLuint normal_sampler;
	GLuint light_position;
	GLuint light_radius;
	GLuint light_color;
	GLuint light_direction;
	GLuint light_cone_angle_cos;
	GLuint inv_view_proj_matrix;
	GLuint camera_position;
} de_deferred_light_shader_t;

/**
* Simple shader with texturing and without lighting
*/
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

struct de_renderer_t {
	de_core_t* core;

	size_t frame_rate_limit;     /**< Maximum frames per seconds for renderer. 0 - unlimited */

	de_flat_shader_t flat_shader;
	de_gui_shader_t gui_shader;
	de_ambient_shader_t ambient_shader;

	de_gbuffer_t gbuffer;
	de_gbuffer_shader_t gbuffer_shader;
	de_deferred_light_shader_t lighting_shader;

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

	/* statistics (time in milliseconds) */
	double frame_time;
	size_t frames_per_second;
};

/**
 * @brief Initializes rendering pipeline.
 */
de_renderer_t* de_renderer_init(de_core_t* core);

/**
 * @brief Frees all allocated resources.
 */
void de_renderer_free(de_renderer_t* r);

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
 * @brief Performs rendering of every scene
 */
void de_renderer_render(de_renderer_t* r);

/**
 * @brief Returns FPS.
 */
size_t de_renderer_get_fps(de_renderer_t* r);

/**
 * @brief Returns time consumed by the renderer to draw one frame.
 */
double de_render_get_frame_time(de_renderer_t* r);