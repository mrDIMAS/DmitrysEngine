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

#define DE_RENDERER_MAX_SKINNING_MATRICES 60

#ifdef NDEBUG
#  define DE_GL_CALL(func) func
#else
#  define DE_GL_CALL(func) func; de_renderer_check_opengl_error(#func, __FILE__, __LINE__)
#endif

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
PFNGLUNIFORM2FPROC glUniform2f;
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

/* Debugging */
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;

PFNGLGETSTRINGIPROC glGetStringi;

void de_renderer_check_opengl_error(const char* func, const char* file, int line)
{
	const char* desc = "Unknown";
	const int err = glGetError();
	if (err != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_ENUM:
				desc = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				desc = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				desc = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				desc = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				desc = "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				desc = "GL_OUT_OF_MEMORY";
				break;
		}

		/* Get more detailed description if available */
		if (glGetDebugMessageLog) {
			GLint max_message_length = 0;
			glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &max_message_length);

			GLint max_logged_messages = 0;
			glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &max_logged_messages);

			GLsizei buffer_size = max_message_length * max_logged_messages * sizeof(GLchar);
			GLchar* message_buffer = de_malloc(buffer_size);
			GLenum* sources = de_malloc(max_logged_messages * sizeof(GLenum));
			GLenum* types = de_malloc(max_logged_messages * sizeof(GLenum));
			GLuint* ids = de_malloc(max_logged_messages * sizeof(GLuint));
			GLenum* severities = de_malloc(max_logged_messages * sizeof(GLenum));
			GLsizei* lengths = de_malloc(max_logged_messages * sizeof(GLsizei));

			GLuint message_count = glGetDebugMessageLog(max_logged_messages, buffer_size, sources, types, ids, severities, lengths, message_buffer);

			GLchar* message = message_buffer;
			for (GLuint i = 0; i < message_count; ++i) {
				const GLenum source = sources[i];
				const GLenum type = types[i];
				const GLenum severity = severities[i];
				const GLuint id = ids[i];

				char* source_str = "";
				switch (source) {
					case GL_DEBUG_SOURCE_API:
						source_str = "API";
						break;
					case GL_DEBUG_SOURCE_SHADER_COMPILER:
						source_str = "Shader Compiler";
						break;
					case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
						source_str = "Window System";
						break;
					case GL_DEBUG_SOURCE_THIRD_PARTY:
						source_str = "Third Party";
						break;
					case GL_DEBUG_SOURCE_APPLICATION:
						source_str = "Application";
						break;
					case GL_DEBUG_SOURCE_OTHER:
						source_str = "Other";
						break;
				}

				char* type_str = "";
				switch (type) {
					case GL_DEBUG_TYPE_ERROR:
						type_str = "Error";
						break;
					case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
						type_str = "Deprecated Behavior";
						break;
					case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
						type_str = "Undefined Behavior";
						break;
					case GL_DEBUG_TYPE_PERFORMANCE:
						type_str = "Performance";
						break;
					case GL_DEBUG_TYPE_PORTABILITY:
						type_str = "Portability";
						break;
					case GL_DEBUG_TYPE_OTHER:
						type_str = "Other";
						break;
				}

				char* severity_str = "";
				switch (severity) {
					case GL_DEBUG_SEVERITY_HIGH:
						severity_str = "High";
						break;
					case GL_DEBUG_SEVERITY_MEDIUM:
						severity_str = "Medium";
						break;
					case GL_DEBUG_SEVERITY_LOW:
						severity_str = "Low";
						break;
					case GL_DEBUG_SEVERITY_NOTIFICATION:
						severity_str = "Notification";
						break;
				}

				de_log("OpenGL message\nSource: %s\nType: %s\nId: %d\nSeverity: %s\nMessage: %s\n", source_str, type_str, id, severity_str, message);

				message += lengths[i];
			}

			de_free(message_buffer);
			de_free(sources);
			de_free(types);
			de_free(ids);
			de_free(severities);
			de_free(lengths);
		}

		de_fatal_error("OpenGL error \"%s\" occured at function:\n%s\nat line %d in file %s. See above messages for details (available only in debug opengl context!).", desc, func, line, file);
	}
}

GLuint de_renderer_create_gpu_program(const char* vertexSource, const char* fragmentSource);
static void de_renderer_upload_surface(de_surface_t* s);

static void de_renderer_load_extensions()
{
#define GET_GL_EXT(type, func) func = (type)de_get_proc_address(#func); \
								   if(!func) de_fatal_error("Unable to load "#func" function pointer");

#define GET_GL_EXT_OPTIONAL(type, func) func = (type)de_get_proc_address(#func);

	GET_GL_EXT(PFNGLCREATESHADERPROC, glCreateShader);
	GET_GL_EXT(PFNGLDELETESHADERPROC, glDeleteShader);
	GET_GL_EXT(PFNGLSHADERSOURCEPROC, glShaderSource);
	GET_GL_EXT(PFNGLCOMPILESHADERPROC, glCompileShader);
	GET_GL_EXT(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
	GET_GL_EXT(PFNGLGETSHADERIVPROC, glGetShaderiv);

	GET_GL_EXT(PFNGLCREATEPROGRAMPROC, glCreateProgram);
	GET_GL_EXT(PFNGLATTACHSHADERPROC, glAttachShader);
	GET_GL_EXT(PFNGLLINKPROGRAMPROC, glLinkProgram);
	GET_GL_EXT(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
	GET_GL_EXT(PFNGLUSEPROGRAMPROC, glUseProgram);
	GET_GL_EXT(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
	GET_GL_EXT(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
	GET_GL_EXT(PFNGLUNIFORM4FPROC, glUniform4f);
	GET_GL_EXT(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
	GET_GL_EXT(PFNGLUNIFORM1IPROC, glUniform1i);
	GET_GL_EXT(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
	GET_GL_EXT(PFNGLUNIFORM3FPROC, glUniform3f);
	GET_GL_EXT(PFNGLUNIFORM2FPROC, glUniform2f);
	GET_GL_EXT(PFNGLUNIFORM1FPROC, glUniform1f);

	GET_GL_EXT(PFNGLGENBUFFERSPROC, glGenBuffers);
	GET_GL_EXT(PFNGLBINDBUFFERPROC, glBindBuffer);
	GET_GL_EXT(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	GET_GL_EXT(PFNGLBUFFERDATAPROC, glBufferData);
	GET_GL_EXT(PFNGLBUFFERSUBDATAPROC, glBufferSubData);

	GET_GL_EXT(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
	GET_GL_EXT(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
	GET_GL_EXT(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
	GET_GL_EXT(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
	GET_GL_EXT(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);

	GET_GL_EXT_OPTIONAL(PFNGLGETDEBUGMESSAGELOGPROC, glGetDebugMessageLog);
	GET_GL_EXT_OPTIONAL(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl);


#ifdef _WIN32	
	/* Windows does support only OpenGL 1.1 and we must obtain these pointers */
	GET_GL_EXT(PFNGLACTIVETEXTUREPROC, glActiveTexture);
	GET_GL_EXT(PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture);
#endif
	GET_GL_EXT(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);

	GET_GL_EXT(PFNGLGETSTRINGIPROC, glGetStringi);

	GET_GL_EXT(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
	GET_GL_EXT(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
	GET_GL_EXT(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
	GET_GL_EXT(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
	GET_GL_EXT(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
	GET_GL_EXT(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
	GET_GL_EXT(PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture);
	GET_GL_EXT(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
	GET_GL_EXT(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
	GET_GL_EXT(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);

#undef GET_GL_EXT
#undef GET_GL_EXT_OPTIONAL

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	if (glDebugMessageControl) {
		/* We not interested in notifications */
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	}

	de_log("Extensions loaded!");
}

static void de_renderer_create_gbuffer(de_renderer_t* r, uint32_t width, uint32_t height)
{
	de_gbuffer_t* gbuffer = &r->gbuffer;

	DE_GL_CALL(glGenFramebuffers(1, &gbuffer->fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->fbo));

	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0_EXT,
		GL_COLOR_ATTACHMENT1_EXT,
		GL_COLOR_ATTACHMENT2_EXT
	};
	DE_GL_CALL(glDrawBuffers(3, buffers));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuffer->depth_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->depth_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuffer->depth_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuffer->color_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->color_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, gbuffer->color_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuffer->normal_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->normal_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, gbuffer->normal_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuffer->depth_buffer));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->depth_buffer));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->depth_buffer));

	DE_GL_CALL(glGenTextures(1, &gbuffer->depth_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->depth_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_BGRA, GL_FLOAT, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->depth_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuffer->color_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->color_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuffer->color_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuffer->normal_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->normal_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuffer->normal_texture, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to construct G-Buffer FBO.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	/* Create another framebuffer for stencil optimizations */
	DE_GL_CALL(glGenFramebuffers(1, &gbuffer->opt_fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->opt_fbo));

	const GLenum lightBuffers[] = {
		GL_COLOR_ATTACHMENT0_EXT
	};
	DE_GL_CALL(glDrawBuffers(1, lightBuffers));

	DE_GL_CALL(glGenTextures(1, &gbuffer->frame_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->frame_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->frame_texture, 0));

	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->depth_buffer));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to initialize Stencil FBO.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

static void de_renderer_set_gbuffer_size(de_renderer_t* r, uint32_t width, uint32_t height)
{
	de_gbuffer_t* gbuffer = &r->gbuffer;

	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->depth_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height));

	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->color_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));

	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->normal_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));

	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->depth_buffer));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->depth_texture));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_BGRA, GL_FLOAT, NULL));

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->color_texture));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->normal_texture));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuffer->frame_texture));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
}

static void de_renderer_create_spot_shadow_map(de_renderer_t* r, size_t size)
{
	de_spot_shadow_map_t* sm = &r->spot_shadow_map;

	DE_GL_CALL(glGenFramebuffers(1, &sm->fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sm->fbo));

	DE_GL_CALL(glDrawBuffer(GL_NONE));

	DE_GL_CALL(glGenTextures(1, &sm->texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, sm->texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm->texture, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to initialize shadow map.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

static void de_renderer_create_point_shadow_map(de_renderer_t* r, size_t size)
{
	de_point_shadow_map_t* sm = &r->point_shadow_map;

	DE_GL_CALL(glGenFramebuffers(1, &sm->fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sm->fbo));

	DE_GL_CALL(glDrawBuffer(GL_NONE));

	DE_GL_CALL(glGenTextures(1, &sm->depth_buffer));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, sm->depth_buffer));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	DE_GL_CALL(glGenTextures(1, &sm->texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sm->texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, color);

	for (size_t i = 0; i < 6; ++i) {
		DE_GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, NULL));
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm->depth_buffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to initialize shadow map.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

static GLint de_renderer_get_uniform(GLuint program, const char* name)
{
	GLint location = glGetUniformLocation(program, name);

	DE_ASSERT(location >= 0);

	return location;
}

static void de_renderer_create_flat_shader(de_renderer_t* r)
{
	const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"

		"void main()"
		"{"
		"	FragColor = texture(diffuseTexture, texCoord);"
		"}";

	const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"

		"uniform mat4 worldViewProjection;"

		"out vec2 texCoord;"

		"void main()"
		"{"
		"	texCoord = vertexTexCoord;"
		"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);"
		"}";

	de_flat_shader_t* s = &r->flat_shader;
	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);
	s->wvp_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
}

static void de_renderer_create_ambient_lighting_shader(de_renderer_t* r)
{
	const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"
		"uniform vec4 ambientColor;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"

		"void main()"
		"{"
		"	FragColor = ambientColor * texture(diffuseTexture, texCoord);"
		"}";

	const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"

		"uniform mat4 worldViewProjection;"

		"out vec2 texCoord;"

		"void main()"
		"{"
		"	texCoord = vertexTexCoord;"
		"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);"
		"}";

	de_ambient_shader_t* s = &r->ambient_shader;
	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);
	s->wvp_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
	s->ambient_color = de_renderer_get_uniform(s->program, "ambientColor");
}

static void de_renderer_create_gui_shader(de_renderer_t* r)
{
	const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"
		"in vec4 color;"

		"void main()"
		"{"
		"	FragColor = color * texture(diffuseTexture, texCoord);"
		"}";

	const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"
		"layout(location = 2) in vec4 vertexColor;"

		"uniform mat4 worldViewProjection;"

		"out vec2 texCoord;"
		"out vec4 color;"

		"void main()"
		"{"
		"	texCoord = vertexTexCoord;"
		"   color = vertexColor;"
		"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);"
		"}";

	de_gui_shader_t* s = &r->gui_shader;
	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);
	s->wvp_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
}

static void de_renderer_create_gbuffer_shader(de_renderer_t* r)
{
	const char* fragment_source =
		"#version 330 core\n"

		"layout(location = 0) out float outDepth;"
		"layout(location = 1) out vec4 outColor;"
		"layout(location = 2) out vec4 outNormal;"

		"uniform sampler2D diffuseTexture;"
		"uniform sampler2D normalTexture;"
		"uniform sampler2D specularTexture;"

		"in vec4 position;"
		"in vec3 normal;"
		"in vec2 texCoord;"
		"in vec3 tangent;"
		"in vec3 binormal;"

		"void main()"
		"{"
		"	outDepth = position.z / position.w;"
		"	outColor = texture2D(diffuseTexture, texCoord);"
		"   if(outColor.a < 0.5) discard;"
		"	outColor.a = 1;"
		"   vec4 n = normalize(texture2D(normalTexture, texCoord) * 2.0 - 1.0);"
		"   mat3 tangentSpace = mat3(tangent, binormal, normal);"
		"	outNormal.xyz = normalize(tangentSpace * n.xyz) * 0.5 + 0.5;"
		"   outNormal.w = texture2D(specularTexture, texCoord).r;"
		"}";

	const char vertex_source[] =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"
		"layout(location = 2) in vec3 vertexNormal;"
		"layout(location = 3) in vec4 vertexTangent;"
		"layout(location = 4) in vec4 boneWeights;"
		"layout(location = 5) in vec4 boneIndices;"

		"uniform mat4 worldMatrix;"
		"uniform mat4 worldViewProjection;"
		"uniform bool useSkeletalAnimation;"
		"uniform mat4 boneMatrices[" DE_STRINGIZE(DE_RENDERER_MAX_SKINNING_MATRICES) "];"

		"out vec4 position;"
		"out vec3 normal;"
		"out vec2 texCoord;"
		"out vec3 tangent;"
		"out vec3 binormal;"

		"void main()"
		"{"
		"   vec4 localPosition = vec4(0);"
		"   vec3 localNormal = vec3(0);"
		"   vec3 localTangent = vec3(0);"
		"   if(useSkeletalAnimation)"
		"   {"
		"       vec4 vertex = vec4(vertexPosition, 1.0);"

		"       int i0 = int(boneIndices.x);"
		"       int i1 = int(boneIndices.y);"
		"       int i2 = int(boneIndices.z);"
		"       int i3 = int(boneIndices.w);"

		"       localPosition += boneMatrices[i0] * vertex * boneWeights.x;"
		"       localPosition += boneMatrices[i1] * vertex * boneWeights.y;"
		"       localPosition += boneMatrices[i2] * vertex * boneWeights.z;"
		"       localPosition += boneMatrices[i3] * vertex * boneWeights.w;"

		"       localNormal += mat3(boneMatrices[i0]) * vertexNormal * boneWeights.x;"
		"       localNormal += mat3(boneMatrices[i1]) * vertexNormal * boneWeights.y;"
		"       localNormal += mat3(boneMatrices[i2]) * vertexNormal * boneWeights.z;"
		"       localNormal += mat3(boneMatrices[i3]) * vertexNormal * boneWeights.w;"

		"       localTangent += mat3(boneMatrices[i0]) * vertexTangent.xyz * boneWeights.x;"
		"       localTangent += mat3(boneMatrices[i1]) * vertexTangent.xyz * boneWeights.y;"
		"       localTangent += mat3(boneMatrices[i2]) * vertexTangent.xyz * boneWeights.z;"
		"       localTangent += mat3(boneMatrices[i3]) * vertexTangent.xyz * boneWeights.w;"
		"   }"
		"   else"
		"   {"
		"       localPosition = vec4(vertexPosition, 1.0);"
		"       localNormal = vertexNormal;"
		"       localTangent = vertexTangent.xyz;"
		"   }"
		"	gl_Position = worldViewProjection * localPosition;"
		"   normal = normalize(mat3(worldMatrix) * localNormal);"
		"   tangent = normalize(mat3(worldMatrix) * localTangent);"
		"   binormal = normalize(vertexTangent.w * cross(tangent, normal));"
		"	texCoord = vertexTexCoord;"
		"	position = gl_Position;"
		"}";

	de_gbuffer_shader_t* s = &r->gbuffer_shader;

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);
	s->wvp_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->world_matrix = de_renderer_get_uniform(s->program, "worldMatrix");
	s->use_skeletal_animation = de_renderer_get_uniform(s->program, "useSkeletalAnimation");
	s->bone_matrices = de_renderer_get_uniform(s->program, "boneMatrices");

	s->diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
	s->normal_texture = de_renderer_get_uniform(s->program, "normalTexture");
}

void de_renderer_create_ssao_shader(de_renderer_t* r)
{
	de_ssao_shader_t* s = &r->ssao_shader;

	const char* fragment_source =
		"#version 330 core\n";

	const char vertex_source[] =
		"#version 330 core\n";

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);
}

static void de_renderer_create_deferred_lighting_shader(de_renderer_t* r)
{
	const char* const fragment_source =
		"#version 330 core\n"
		"uniform sampler2D depthTexture;"
		"uniform sampler2D colorTexture;"
		"uniform sampler2D normalTexture;"
		"uniform sampler2D spotShadowTexture;"
		"uniform samplerCube pointShadowTexture;"

		"uniform mat4 lightViewProjMatrix;"
		"uniform vec3 lightPos;"
		"uniform float lightRadius;"
		"uniform vec4 lightColor;"
		"uniform vec3 lightDirection;"
		"uniform float coneAngleCos;"
		"uniform mat4 invViewProj;"
		"uniform vec3 cameraPosition;"
		"uniform int lightType;" /* de_light_type_t, invalid value == no shadows */
		"uniform bool softShadows;"
		"uniform float shadowMapInvSize;"

		"in vec2 texCoord;"
		"out vec4 FragColor;"

		"vec3 GetProjection(vec3 worldPosition, mat4 viewProjectionMatrix)"
		"{"
		"   vec4 projPos = viewProjectionMatrix * vec4(worldPosition, 1);"
		"   projPos /= projPos.w;"
		"   return vec3(projPos.x * 0.5 + 0.5, projPos.y * 0.5 + 0.5, projPos.z * 0.5 + 0.5);"
		"}"

		"void main()"
		"{"
		"   vec4 normalSpecular = texture2D(normalTexture, texCoord);"
		"	vec3 normal = normalize(normalSpecular.xyz * 2.0 - 1.0);"

		"	vec4 screenPosition;"
		"	screenPosition.x = texCoord.x * 2.0 - 1.0;"
		"	screenPosition.y = texCoord.y * 2.0 - 1.0;"
		"	screenPosition.z = texture2D(depthTexture, texCoord).r;"
		"	screenPosition.w = 1.0;"

		"	vec4 worldPosition = invViewProj * screenPosition;"
		"	worldPosition /= worldPosition.w;"

		"	vec3 lightVector = lightPos - worldPosition.xyz;"
		"   float distanceToLight = length(lightVector);"
		"	float d = min(distanceToLight, lightRadius);"
		"	vec3 normLightVector = lightVector / d;"
		"   vec3 h = normalize(lightVector + (cameraPosition - worldPosition.xyz));"
		"   vec3 specular = normalSpecular.w * vec3(0.4 * pow(clamp(dot(normal, h), 0.0, 1.0), 80));"
		"	float y = dot(lightDirection, normLightVector);"
		"	float k = max(dot(normal, normLightVector), 0);"
		"	float attenuation = 1.0 + cos((d / lightRadius) * 3.14159);"
		"	if (y < coneAngleCos)"
		"	{"
		"		attenuation *= smoothstep(coneAngleCos - 0.1, coneAngleCos, y);"
		"	}"

		"   float shadow = 1.0;"
		"   if (lightType == 2)" /* Spot light shadows */
		"   {"
		"      vec3 lightSpacePosition = GetProjection(worldPosition.xyz, lightViewProjMatrix); "
		"      const float bias = 0.00005;"
		"      if (softShadows)"
		"      {"
		"         for (float y = -1.5; y <= 1.5; y += 0.5)"
		"         {"
		"            for (float x = -1.5; x <= 1.5; x += 0.5)"
		"            {"
		"               vec2 fetchTexCoord = lightSpacePosition.xy + vec2(x, y) * shadowMapInvSize;"
		"               if (lightSpacePosition.z - bias > texture(spotShadowTexture, fetchTexCoord).r)"
		"               {"
		"                  shadow += 1.0;"
		"               }"
		"            }"
		"         }"

		"         shadow = clamp(1.0 - shadow / 9.0, 0.0, 1.0);"
		"      }"
		"      else"
		"      {"
		"         if (lightSpacePosition.z - bias > texture(spotShadowTexture, lightSpacePosition.xy).r)"
		"         {"
		"            shadow = 0.0;"
		"         }"
		"      }"
		"   }"
		"   else if(lightType == 0)" /* Point light shadows */
		"   {"
		"      const float bias = 0.01;"
		"      if (softShadows)"
		"      {"
		"         const int samples = 20;"

		"         const vec3 directions[samples] = vec3[samples] ("
		"            vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),"
		"            vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),"
		"            vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),"
		"            vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),"
		"            vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1) "
		"         );"

		"         const float diskRadius = 0.0025;"

		"         for (int i = 0; i < samples; ++i)"
		"         {"
		"            vec3 fetchDirection = -normLightVector + directions[i] * diskRadius;"
		"            float shadowDistanceToLight = texture(pointShadowTexture, fetchDirection).r;"
		"            if (distanceToLight - bias > shadowDistanceToLight)"
		"            {"
		"               shadow += 1.0;"
		"            }"
		"         }"

		"         shadow = clamp(1.0 - shadow / float(samples), 0.0, 1.0);"
		"      }"
		"      else"
		"      {"
		"         float shadowDistanceToLight = texture(pointShadowTexture, -normLightVector).r;"
		"         if (distanceToLight - bias > shadowDistanceToLight)"
		"         {"
		"            shadow = 0.0;"
		"         }"
		"      }"
		"   }"

		"   FragColor = texture2D(colorTexture, texCoord);"
		"   FragColor.xyz += specular;"
		"	FragColor *= k * shadow * attenuation * lightColor;"
		"}";

	const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"

		"uniform mat4 worldViewProjection;"

		"out vec2 texCoord;"

		"void main()"
		"{"
		"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);"
		"	texCoord = vertexTexCoord;"
		"}";

	de_deferred_light_shader_t* s = &r->lighting_shader;

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);

	s->wvp_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");

	s->depth_sampler = de_renderer_get_uniform(s->program, "depthTexture");
	s->color_sampler = de_renderer_get_uniform(s->program, "colorTexture");
	s->normal_sampler = de_renderer_get_uniform(s->program, "normalTexture");
	s->spot_shadow_texture = de_renderer_get_uniform(s->program, "spotShadowTexture");
	s->point_shadow_texture = de_renderer_get_uniform(s->program, "pointShadowTexture");
	s->light_view_proj_matrix = de_renderer_get_uniform(s->program, "lightViewProjMatrix");
	s->light_type = de_renderer_get_uniform(s->program, "lightType");
	s->soft_shadows = de_renderer_get_uniform(s->program, "softShadows");
	s->shadow_map_inv_size = de_renderer_get_uniform(s->program, "shadowMapInvSize");
	s->light_position = de_renderer_get_uniform(s->program, "lightPos");
	s->light_radius = de_renderer_get_uniform(s->program, "lightRadius");
	s->light_color = de_renderer_get_uniform(s->program, "lightColor");
	s->light_direction = de_renderer_get_uniform(s->program, "lightDirection");
	s->light_cone_angle_cos = de_renderer_get_uniform(s->program, "coneAngleCos");
	s->inv_view_proj_matrix = de_renderer_get_uniform(s->program, "invViewProj");
	s->camera_position = de_renderer_get_uniform(s->program, "cameraPosition");
}

static void de_renderer_create_particle_system_shader(de_renderer_t* r)
{
	const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"
		"layout(location = 2) in float particleSize;"
		"layout(location = 3) in float particleRotation;"
		"layout(location = 4) in vec4 vertexColor;"

		"uniform mat4 viewProjectionMatrix;"
		"uniform mat4 worldMatrix;"
		"uniform vec4 cameraUpVector;"
		"uniform vec4 cameraSideVector;"

		"out vec2 texCoord;"
		"out vec4 color;"

		"vec2 rotateVec2(vec2 v, float angle)"
		"{"
		"   float c = cos(angle);"
		"   float s = sin(angle);"
		"   mat2 m = mat2(c, -s, s, c);"
		"   return m * v;"
		"}"

		"void main()"
		"{"
		"	color = vertexColor;"
		"	texCoord = vertexTexCoord;"
		"   vec2 vertexOffset = rotateVec2(vertexTexCoord * 2.0 - 1.0, particleRotation);"
		"	vec4 worldPosition = worldMatrix * vec4(vertexPosition, 1.0);"
		"	vec4 offset = (vertexOffset.x * cameraSideVector + vertexOffset.y * cameraUpVector) * particleSize;"
		"	gl_Position = viewProjectionMatrix * (worldPosition + offset);"
		"}";

	const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"
		"uniform sampler2D depthBufferTexture;"
		"uniform vec2 invScreenSize;"
		"uniform vec2 projParams;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"
		"in vec4 color;"

		"float toProjSpace(float z)"
		"{"
		"   float far = projParams.x;"
		"   float near = projParams.y;"
		"	return (far * near) / (far - z * (far + near));"
		"}"

		"void main()"
		"{"
		"   float sceneDepth = toProjSpace(texture(depthBufferTexture, gl_FragCoord.xy * invScreenSize).r);"
		"   float depthOpacity = clamp((sceneDepth - gl_FragCoord.z / gl_FragCoord.w) * 2.0f, 0.0, 1.0);"
		"	FragColor = color * texture(diffuseTexture, texCoord).r;"
		"   FragColor.a *= depthOpacity;"
		"}";

	de_particle_system_shader_t* s = &r->particle_system_shader;

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);

	s->vs.view_projection_matrix = de_renderer_get_uniform(s->program, "viewProjectionMatrix");
	s->vs.world_matrix = de_renderer_get_uniform(s->program, "worldMatrix");
	s->vs.camera_side_vector = de_renderer_get_uniform(s->program, "cameraSideVector");
	s->vs.camera_up_vector = de_renderer_get_uniform(s->program, "cameraUpVector");

	s->fs.diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
	s->fs.depth_buffer_texture = de_renderer_get_uniform(s->program, "depthBufferTexture");
	s->fs.inv_screen_size = de_renderer_get_uniform(s->program, "invScreenSize");
	s->fs.proj_params = de_renderer_get_uniform(s->program, "projParams");
}

static void de_renderer_create_spot_shadow_map_shader(de_renderer_t* r)
{
	static const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"

		"in vec2 texCoord;"

		"void main() "
		"{"
		"   if(texture(diffuseTexture, texCoord).a < 0.2) discard;"
		"}";

	static const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"
		"layout(location = 4) in vec4 boneWeights;"
		"layout(location = 5) in vec4 boneIndices;"

		"uniform mat4 worldViewProjection;"
		"uniform bool useSkeletalAnimation;"
		"uniform mat4 boneMatrices[" DE_STRINGIZE(DE_RENDERER_MAX_SKINNING_MATRICES) "];"

		"out vec2 texCoord;"

		"void main()"
		"{"
		"   vec4 localPosition = vec4(0);"

		"   if(useSkeletalAnimation)"
		"   {"
		"       vec4 vertex = vec4(vertexPosition, 1.0);"

		"       localPosition += boneMatrices[int(boneIndices.x)] * vertex * boneWeights.x;"
		"       localPosition += boneMatrices[int(boneIndices.y)] * vertex * boneWeights.y;"
		"       localPosition += boneMatrices[int(boneIndices.z)] * vertex * boneWeights.z;"
		"       localPosition += boneMatrices[int(boneIndices.w)] * vertex * boneWeights.w;"
		"   }"
		"   else"
		"   {"
		"       localPosition = vec4(vertexPosition, 1.0);"
		"   }"

		"	gl_Position = worldViewProjection * localPosition;"
		"   texCoord = vertexTexCoord;"
		"}";

	de_spot_shadow_map_shader_t* s = &r->spot_shadow_map_shader;

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);

	s->vs.bone_matrices = de_renderer_get_uniform(s->program, "boneMatrices");
	s->vs.world_view_projection_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->vs.use_skeletal_animation = de_renderer_get_uniform(s->program, "useSkeletalAnimation");

	s->fs.diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
}

static void de_renderer_create_point_shadow_map_shader(de_renderer_t* r)
{
	static const char* fragment_source =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"
		"uniform vec3 lightPosition;"

		"in vec2 texCoord;"
		"in vec3 worldPosition;"

		"layout(location = 0) out float depth;"

		"void main() "
		"{"
		"   if(texture(diffuseTexture, texCoord).a < 0.2) discard;"
		"   depth = length(lightPosition - worldPosition);"
		"}";

	static const char* vertex_source =
		"#version 330 core\n"

		"layout(location = 0) in vec3 vertexPosition;"
		"layout(location = 1) in vec2 vertexTexCoord;"
		"layout(location = 4) in vec4 boneWeights;"
		"layout(location = 5) in vec4 boneIndices;"

		"uniform mat4 worldMatrix;"
		"uniform mat4 worldViewProjection;"
		"uniform bool useSkeletalAnimation;"
		"uniform mat4 boneMatrices[" DE_STRINGIZE(DE_RENDERER_MAX_SKINNING_MATRICES) "];"

		"out vec2 texCoord;"
		"out vec3 worldPosition;"

		"void main()"
		"{"
		"   vec4 localPosition = vec4(0);"

		"   if(useSkeletalAnimation)"
		"   {"
		"       vec4 vertex = vec4(vertexPosition, 1.0);"

		"       localPosition += boneMatrices[int(boneIndices.x)] * vertex * boneWeights.x;"
		"       localPosition += boneMatrices[int(boneIndices.y)] * vertex * boneWeights.y;"
		"       localPosition += boneMatrices[int(boneIndices.z)] * vertex * boneWeights.z;"
		"       localPosition += boneMatrices[int(boneIndices.w)] * vertex * boneWeights.w;"
		"   }"
		"   else"
		"   {"
		"       localPosition = vec4(vertexPosition, 1.0);"
		"   }"

		"	gl_Position = worldViewProjection * localPosition;"
		"   worldPosition = (worldMatrix * localPosition).xyz;"
		"   texCoord = vertexTexCoord;"
		"}";

	de_point_shadow_map_shader_t* s = &r->point_shadow_map_shader;

	s->program = de_renderer_create_gpu_program(vertex_source, fragment_source);

	s->vs.world_matrix = de_renderer_get_uniform(s->program, "worldMatrix");
	s->vs.bone_matrices = de_renderer_get_uniform(s->program, "boneMatrices");
	s->vs.world_view_projection_matrix = de_renderer_get_uniform(s->program, "worldViewProjection");
	s->vs.use_skeletal_animation = de_renderer_get_uniform(s->program, "useSkeletalAnimation");

	s->fs.diffuse_texture = de_renderer_get_uniform(s->program, "diffuseTexture");
	s->fs.light_position = de_renderer_get_uniform(s->program, "lightPosition");
}

static void de_renderer_create_shaders(de_renderer_t* r)
{
	de_renderer_create_flat_shader(r);
	de_renderer_create_ambient_lighting_shader(r);
	de_renderer_create_gui_shader(r);
	de_renderer_create_gbuffer_shader(r);
	de_renderer_create_deferred_lighting_shader(r);
	/*de_renderer_create_ssao_shader(r);*/ 
	de_renderer_create_particle_system_shader(r);
	de_renderer_create_spot_shadow_map_shader(r);
	de_renderer_create_point_shadow_map_shader(r);
}

de_renderer_quality_settings_t de_renderer_get_default_quality_settings(de_renderer_t* r)
{
	DE_UNUSED(r);

	return(de_renderer_quality_settings_t) {
		.point_shadow_map_size = 1024,
		.point_shadows_distance = 10,
		.point_shadows_enabled = true,
		.point_soft_shadows = true,

		.spot_shadow_map_size = 1024,
		.spot_shadows_distance = 10,
		.spot_shadows_enabled = true,
		.spot_soft_shadows = true,

		.anisotropy_pow2 = 4
	};
}

de_renderer_quality_settings_t de_renderer_get_quality_settings(de_renderer_t* r)
{
	DE_ASSERT(r);
	return r->quality_settings;
}

void de_renderer_apply_quality_settings(de_renderer_t* r, const de_renderer_quality_settings_t* settings)
{
	DE_ASSERT(r);

	r->quality_settings = *settings;
	
	/* Point shadow map */
	DE_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, r->point_shadow_map.texture));
	for (size_t i = 0; i < 6; ++i) {
		DE_GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, settings->point_shadow_map_size,
			settings->point_shadow_map_size, 0, GL_RED, GL_FLOAT, NULL));
	}
	DE_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

	/* Spot shadow map */
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->spot_shadow_map.texture));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, settings->spot_shadow_map_size,
		settings->spot_shadow_map_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

de_renderer_t* de_renderer_init(de_core_t* core)
{
	de_renderer_t* r = DE_NEW(de_renderer_t);

	r->core = core;
	r->min_fps = 32768;

	r->quality_settings = de_renderer_get_default_quality_settings(r);
	r->ambient_light_color = (de_color_t) { .r = 120, .g = 120, .b = 120, .a = 255 };

	de_log("GPU Vendor: %s", glGetString(GL_VENDOR));
	de_log("GPU: %s", glGetString(GL_RENDERER));
	de_log("OpenGL Version: %s", glGetString(GL_VERSION));
	de_log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	de_renderer_load_extensions();
	de_renderer_create_shaders(r);

#if VERBOSE_INIT
	int num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (int i = 0; i < num_extensions; ++i) {
		de_log((char*)glGetStringi(GL_EXTENSIONS, i));
	}
#endif
		
	DE_GL_CALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &r->limits.max_anisotropy));

	DE_GL_CALL(glEnable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_CULL_FACE));
	glCullFace(GL_BACK);

	de_renderer_create_gbuffer(r, core->params.video_mode.width, core->params.video_mode.height);
	de_renderer_create_spot_shadow_map(r, r->quality_settings.spot_shadow_map_size);
	de_renderer_create_point_shadow_map(r, r->quality_settings.point_shadow_map_size);

	/* Create unit quad that will be scaled to fullscreen by matrix */
	{
		de_surface_shared_data_t* data = de_surface_shared_data_create(4, 6);

		const int faces[] = { 0, 1, 2, 0, 2, 3 };
		memcpy(data->indices, faces, sizeof(faces));
		data->index_count = 6;

		data->positions[0] = (de_vec3_t) { 0, 0, 0 };
		data->positions[1] = (de_vec3_t) { 1, 0, 0 };
		data->positions[2] = (de_vec3_t) { 1, 1, 0 };
		data->positions[3] = (de_vec3_t) { 0, 1, 0 };
		data->tex_coords[0] = (de_vec2_t) { 0, 1 };
		data->tex_coords[1] = (de_vec2_t) { 1, 1 };
		data->tex_coords[2] = (de_vec2_t) { 1, 0 };
		data->tex_coords[3] = (de_vec2_t) { 0, 0 };
		data->vertex_count = 4;

		r->quad = de_renderer_create_surface(r);
		de_surface_set_data(r->quad, data);
		de_renderer_upload_surface(r->quad);
	}

	/* Create light sphere */
	{
		r->light_unit_sphere = de_renderer_create_surface(r);
		de_surface_make_sphere(r->light_unit_sphere, 6, 6, 1);
		de_renderer_upload_surface(r->light_unit_sphere);
	}

	glGenVertexArrays(1, &r->gui_render_buffers.vao);
	glGenBuffers(1, &r->gui_render_buffers.vbo);
	glGenBuffers(1, &r->gui_render_buffers.ebo);

	r->test_surface = de_renderer_create_surface(r);

	/* white dummy texture for surfaces without texture */
	{
		de_rgba8_t* pixel;
		de_resource_t* res = de_resource_create(core, NULL, DE_RESOURCE_TYPE_TEXTURE);
		de_resource_set_flags(res, DE_RESOURCE_FLAG_INTERNAL);
		de_resource_add_ref(res);
		r->white_dummy = de_resource_to_texture(res);
		de_texture_alloc_pixels(r->white_dummy, 1, 1, 4);
		pixel = (de_rgba8_t*)r->white_dummy->pixels;
		pixel->r = pixel->g = pixel->b = pixel->a = 255;
	}

	/* dummy normal map with (0,0,1) vector */
	{
		de_rgba8_t* pixel;
		de_resource_t* res = de_resource_create(core, NULL, DE_RESOURCE_TYPE_TEXTURE);
		de_resource_set_flags(res, DE_RESOURCE_FLAG_INTERNAL);
		de_resource_add_ref(res);
		r->normal_map_dummy = de_resource_to_texture(res);
		de_texture_alloc_pixels(r->normal_map_dummy, 1, 1, 4);
		pixel = (de_rgba8_t*)r->normal_map_dummy->pixels;
		pixel->r = 128;
		pixel->g = 128;
		pixel->b = 255;
		pixel->a = 255;
	}

	r->render_normals = false;
	r->render_bones = true;

	return r;
}

void de_renderer_notify_video_mode_changed(de_renderer_t* r, const de_video_mode_t* new_video_mode) 
{
	de_renderer_set_gbuffer_size(r, new_video_mode->width, new_video_mode->height);
}

void de_renderer_free(de_renderer_t* r)
{
	de_renderer_free_surface(r->quad);
	de_renderer_free_surface(r->test_surface);
	de_renderer_free_surface(r->light_unit_sphere);
	de_resource_release(de_resource_from_texture(r->white_dummy));
	de_resource_release(de_resource_from_texture(r->normal_map_dummy));
	de_free(r);
}

#if 0
static void de_render_surface_normals(de_renderer_t* r, de_surface_t* surface)
{
	size_t i;

	DE_ARRAY_CLEAR(r->test_surface->vertices);
	DE_ARRAY_CLEAR(r->test_surface->indices);

	for (i = 0; i < surface->vertices.size; ++i) {
		de_vertex_t* vertex;
		de_vertex_t begin, end;
		int ia, ib;

		vertex = surface->vertices.data + i;

		begin.position = vertex->position;
		de_vec3_add(&end.position, &begin.position, &vertex->normal);

		DE_ARRAY_APPEND(r->test_surface->vertices, begin);
		DE_ARRAY_APPEND(r->test_surface->vertices, end);

		ia = i * 2;
		ib = ia + 1;
		DE_ARRAY_APPEND(r->test_surface->indices, ia);
		DE_ARRAY_APPEND(r->test_surface->indices, ib);
	}

	de_renderer_upload_surface(r->test_surface);

	DE_GL_CALL(glBindVertexArray(r->test_surface->vao));
	DE_GL_CALL(glDrawElements(GL_LINES, r->test_surface->indices.size, GL_UNSIGNED_INT, NULL));
	++r->draw_calls;
}


static void de_renderer_draw_surface_bones(de_renderer_t* r, de_surface_t* surface)
{
	size_t i, j, index = 0;

	DE_ARRAY_CLEAR(r->test_surface->vertices);
	DE_ARRAY_CLEAR(r->test_surface->indices);

	for (i = 0; i < surface->weights.size; ++i) {
		de_node_t* bone = surface->weights.data[i];
		de_vertex_t begin, end;

		for (j = 0; j < bone->children.size; ++j) {
			de_node_t* next_bone = bone->children.data[j];

			if (!next_bone->is_bone) {
				continue;
			}
			de_node_get_global_position(bone, &begin.position);
			de_node_get_global_position(next_bone, &end.position);

			DE_ARRAY_APPEND(r->test_surface->vertices, begin);
			DE_ARRAY_APPEND(r->test_surface->vertices, end);

			DE_ARRAY_APPEND(r->test_surface->indices, index++);
			DE_ARRAY_APPEND(r->test_surface->indices, index++);
		}
	}

	de_renderer_upload_surface(r->test_surface);

	DE_GL_CALL(glBindVertexArray(r->test_surface->vao));
	DE_GL_CALL(glDrawElements(GL_LINES, r->test_surface->shared_data->index_count, GL_UNSIGNED_INT, NULL));
	++r->draw_calls;
}
#endif

GLuint de_renderer_create_shader(GLenum type, const char* source)
{
	GLint compiled;
	GLuint shader;

	shader = glCreateShader(type);
	DE_GL_CALL(glShaderSource(shader, 1, &source, NULL));

	DE_GL_CALL(glCompileShader(shader));

	DE_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));

	if (!compiled) {
		static char buffer[2048];
		DE_GL_CALL(glGetShaderInfoLog(shader, 2048, NULL, buffer));
		de_fatal_error(buffer);
	}

	return shader;
}


GLuint de_renderer_create_gpu_program(const char* vertexSource, const char* fragmentSource)
{
	GLint linked;
	GLuint vertex_shader, fragment_shader, program;

	vertex_shader = de_renderer_create_shader(GL_VERTEX_SHADER, vertexSource);
	fragment_shader = de_renderer_create_shader(GL_FRAGMENT_SHADER, fragmentSource);
	program = glCreateProgram();

	DE_GL_CALL(glAttachShader(program, vertex_shader));
	DE_GL_CALL(glDeleteShader(vertex_shader));

	DE_GL_CALL(glAttachShader(program, fragment_shader));
	DE_GL_CALL(glDeleteShader(fragment_shader));

	DE_GL_CALL(glLinkProgram(program));

	DE_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &linked));

	if (!linked) {
		static char buffer[2048];
		DE_GL_CALL(glGetProgramInfoLog(program, 2048, NULL, buffer));
		de_fatal_error(buffer);
	}

	return program;
}

static void de_renderer_upload_surface(de_surface_t* s)
{
	de_surface_shared_data_t* data = s->shared_data;

	if (!data->vertex_buffer) {
		glGenBuffers(1, &data->vertex_buffer);
	}
	if (!data->index_buffer) {
		glGenBuffers(1, &data->index_buffer);
	}
	if (!data->vertex_array_object) {
		glGenVertexArrays(1, &data->vertex_array_object);
	}

	DE_GL_CALL(glBindVertexArray(data->vertex_array_object));

	/* Upload indices */
	DE_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->index_buffer));
	DE_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*data->indices) * data->index_count, data->indices, GL_STATIC_DRAW));

	/* Upload vertices */
	size_t total_size_bytes = data->vertex_count * (
		sizeof(*data->positions) +
		sizeof(*data->tex_coords) +
		sizeof(*data->normals) +
		sizeof(*data->tangents) +
		sizeof(*data->bone_weights) +
		sizeof(*data->bone_indices));

	DE_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, data->vertex_buffer));
	DE_GL_CALL(glBufferData(GL_ARRAY_BUFFER, total_size_bytes, NULL, GL_STATIC_DRAW));

	/* Upload parts */
	size_t pos_offset = 0;
	size_t size = sizeof(*data->positions) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, pos_offset, size, data->positions));

	size_t tex_coord_offset = pos_offset + size;
	size = sizeof(*data->tex_coords) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, tex_coord_offset, size, data->tex_coords));

	size_t normals_offset = tex_coord_offset + size;
	size = sizeof(*data->normals) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, normals_offset, size, data->normals));

	size_t tangents_offset = normals_offset + size;
	size = sizeof(*data->tangents) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, size, data->tangents));

	size_t weights_offset = tangents_offset + size;
	size = sizeof(*data->bone_weights) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, weights_offset, size, data->bone_weights));

	size_t indices_offset = weights_offset + size;
	size = sizeof(*data->bone_indices) * data->vertex_count;
	DE_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, indices_offset, size, data->bone_indices));

	/* Setup attribute locations for shared data */
	DE_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*data->positions), (void*)pos_offset));
	DE_GL_CALL(glEnableVertexAttribArray(0));

	DE_GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*data->tex_coords), (void*)tex_coord_offset));
	DE_GL_CALL(glEnableVertexAttribArray(1));

	DE_GL_CALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*data->normals), (void*)normals_offset));
	DE_GL_CALL(glEnableVertexAttribArray(2));

	DE_GL_CALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(*data->tangents), (void*)tangents_offset));
	DE_GL_CALL(glEnableVertexAttribArray(3));

	DE_GL_CALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(*data->bone_weights), (void*)weights_offset));
	DE_GL_CALL(glEnableVertexAttribArray(4));

	DE_GL_CALL(glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*data->bone_indices), (void*)indices_offset));
	DE_GL_CALL(glEnableVertexAttribArray(5));

	DE_GL_CALL(glBindVertexArray(0));

	s->need_upload = false;
}

de_surface_t* de_renderer_create_surface(de_renderer_t* r)
{
	de_surface_t* surf = DE_NEW(de_surface_t);
	surf->renderer = r;
	surf->need_upload = true;
	return surf;
}

void de_renderer_free_surface(de_surface_t* surf)
{
	/* Unref texture */
	if (surf->diffuse_map) {
		de_resource_release(de_resource_from_texture(surf->diffuse_map));
	}
	if (surf->normal_map) {
		de_resource_release(de_resource_from_texture(surf->normal_map));
	}

	/* Delete buffers */
	de_surface_shared_data_t* data = surf->shared_data;
	if (data) {
		--data->ref_count;
		if (data->ref_count <= 0) {
			glDeleteBuffers(1, &data->vertex_buffer);
			glDeleteBuffers(1, &data->index_buffer);
			glDeleteVertexArrays(1, &data->vertex_array_object);
			de_surface_shared_data_free(data);
		}
	}

	DE_ARRAY_FREE(surf->vertex_weights);
	DE_ARRAY_FREE(surf->bones);

	de_free(surf);
}

static void de_renderer_remove_texture(de_renderer_t* r, de_texture_t* tex)
{
	DE_UNUSED(r);
	glDeleteTextures(1, &tex->id);
}

static void de_renderer_upload_texture(de_renderer_t* r, de_texture_t* texture)
{
	GLint internalFormat;
	GLint format;
	switch (texture->byte_per_pixel) {
		case 3:
			internalFormat = GL_RGB;
			format = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			break;
		default:
			de_fatal_error("Unknown texture byte per pixel: %d", texture->byte_per_pixel);
			return;
	}

	if (!texture->id) {
		DE_GL_CALL(glGenTextures(1, &texture->id));
	}

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->pixels));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	DE_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

	float anisotropy = (float)pow(2.0, r->quality_settings.anisotropy_pow2);
	if (anisotropy > r->limits.max_anisotropy) {
		anisotropy = (float)r->limits.max_anisotropy;
	}

	DE_GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy));

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	texture->need_upload = false;
}

static void de_renderer_render_surface(de_renderer_t* r, const de_surface_t* surf)
{
	DE_UNUSED(r);
	DE_GL_CALL(glBindVertexArray(surf->shared_data->vertex_array_object));
	DE_GL_CALL(glDrawElements(GL_TRIANGLES, surf->shared_data->index_count, GL_UNSIGNED_INT, NULL));
	++r->draw_calls;
}

static void de_renderer_draw_fullscreen_quad(de_renderer_t* r)
{
	de_renderer_render_surface(r, r->quad);
}

static void de_renderer_draw_mesh_bones(de_renderer_t* r, de_mesh_t* mesh)
{
	DE_UNUSED(r);
	size_t i;
	for (i = 0; i < mesh->surfaces.size; ++i) {
		//		de_renderer_draw_surface_bones(r, mesh->surfaces.data[i]);
	}
}

static void de_renderer_draw_mesh_normals(de_renderer_t* r, de_mesh_t* mesh)
{
	DE_UNUSED(r);
	size_t i;
	for (i = 0; i < mesh->surfaces.size; ++i) {
		//de_render_surface_normals(r, mesh->surfaces.data[i]);
	}
}

static void de_renderer_set_viewport(const de_rectf_t* viewport, unsigned int width, unsigned int height)
{
	const int viewport_x = (int)(viewport->x * width);
	const int viewport_y = (int)(viewport->y * height);
	const int viewport_w = (int)(viewport->w * width);
	const int viewport_h = (int)(viewport->h * height);

	DE_GL_CALL(glViewport(viewport_x, viewport_y, viewport_w, viewport_h));
}

static void de_renderer_upload_textures(de_renderer_t* r)
{
	for (size_t i = 0; i < r->core->resources.size; ++i) {
		de_resource_t* res = r->core->resources.data[i];
		if (res->type == DE_RESOURCE_TYPE_TEXTURE) {
			de_texture_t* texture = de_resource_to_texture(res);
			if (texture->need_upload) {
				de_renderer_upload_texture(r, texture);
			}
		}
	}
}

static void de_renderer_draw_light_sphere(de_renderer_t* r, const de_camera_t* camera, const de_vec3_t* pos, float radius, GLint wvp_matrix_location)
{
	de_mat4_t world;
	de_mat4_translation(&world, pos);

	de_mat4_t scale;
	float s = radius * 1.05f;
	de_mat4_scale(&scale, &(de_vec3_t){ s, s, s });
	de_mat4_mul(&world, &world, &scale);

	de_mat4_t wvp_matrix;
	de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &world);

	DE_GL_CALL(glUniformMatrix4fv(wvp_matrix_location, 1, GL_FALSE, wvp_matrix.f));
	de_renderer_render_surface(r, r->light_unit_sphere);
}

void de_renderer_render(de_renderer_t* r)
{
	de_core_t* core = r->core;
	static int last_time_ms;
	
	const uint32_t frame_width = core->params.video_mode.width;
	const uint32_t frame_height = core->params.video_mode.height;
	const double frame_start_time = de_time_get_seconds();

	de_mat4_t y_flip_ortho;
	de_mat4_ortho(&y_flip_ortho, 0, (float)frame_width, (float)frame_height, 0, -1, 1);

	de_mat4_t frame_scale_matrix;
	de_mat4_scale(&frame_scale_matrix, &(de_vec3_t) { (float)frame_width, (float)frame_height, 0.0 });

	de_mat4_t frame_mvp_matrix;
	de_mat4_mul(&frame_mvp_matrix, &y_flip_ortho, &frame_scale_matrix);

	r->draw_calls = 0;

	/* Upload textures first */
	de_renderer_upload_textures(r);

	de_mat4_t identity;
	de_mat4_identity(&identity);

	if (core->scenes.head) {
		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.fbo));
	} else {
		/* bind back buffer if no scenes */
		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	glClearColor(0.1f, 0.1f, 0.1f, 0);
	DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	glClearColor(0.0f, 0.0f, 0.0f, 0);

	DE_GL_CALL(glUseProgram(r->gbuffer_shader.program));
	DE_GL_CALL(glUniform1i(r->gbuffer_shader.diffuse_texture, 0));
	DE_GL_CALL(glUniform1i(r->gbuffer_shader.normal_texture, 1));
	DE_GL_CALL(glEnable(GL_CULL_FACE));

	/* Upload surface data to GPU */
	for (de_scene_t* scene = core->scenes.head; scene; scene = scene->next) {
		for (de_node_t* node = scene->nodes.head; node; node = node->next) {
			if (node->type == DE_NODE_TYPE_MESH) {
				de_mesh_t* mesh = &node->s.mesh;

				for (size_t i = 0; i < mesh->surfaces.size; ++i) {
					de_surface_t* surf = mesh->surfaces.data[i];

					if (surf->need_upload) {
						de_renderer_upload_surface(surf);
					}
				}
			}
		}
	}

	/* render each scene */
	for (de_scene_t* scene = core->scenes.head; scene; scene = scene->next) {
		if (!scene->active_camera) {
			continue;
		}

		de_camera_t* camera = &scene->active_camera->s.camera;

		de_vec3_t camera_position = (de_vec3_t) { 0 };
		de_node_get_global_position(de_node_from_camera(camera), &camera_position);

		de_camera_update(camera);

		/* build frustum */
		de_frustum_t frustum;
		de_frustum_from_matrix(&frustum, &camera->view_projection_matrix);

		de_renderer_set_viewport(&camera->viewport, frame_width, frame_height);

		/* Render each node */
		for (de_node_t* node = scene->nodes.head; node; node = node->next) {
			if (node->global_visibility && node->type == DE_NODE_TYPE_MESH) {

				if (!de_frustum_box_intersection_transform(&frustum, &node->bounding_box, &node->global_matrix)) {
					continue;
				}

				if (node->depth_hack != 0) {
					de_camera_enter_depth_hack(camera, node->depth_hack);
				}

				const de_gbuffer_shader_t* shader = &r->gbuffer_shader;

				const de_mesh_t* mesh = &node->s.mesh;
				const bool is_skinned = de_mesh_is_skinned(mesh);
				de_mat4_t wvp_matrix;
				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, is_skinned ? &identity : &node->global_matrix);

				DE_GL_CALL(glUniformMatrix4fv(shader->wvp_matrix, 1, GL_FALSE, wvp_matrix.f));
				DE_GL_CALL(glUniformMatrix4fv(shader->world_matrix, 1, GL_FALSE, is_skinned ? identity.f : node->global_matrix.f));

				for (size_t i = 0; i < mesh->surfaces.size; ++i) {
					const de_surface_t* surf = mesh->surfaces.data[i];

					/* bind diffuse map */
					DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
					if (surf->diffuse_map) {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->diffuse_map->id));
					} else {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
					}

					/* bind normal map */
					DE_GL_CALL(glActiveTexture(GL_TEXTURE1));
					if (surf->normal_map) {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->normal_map->id));
					} else {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->normal_map_dummy->id));
					}

					/* bind specular map */
					DE_GL_CALL(glActiveTexture(GL_TEXTURE2));
					if (surf->specular_map) {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->specular_map->id));
					} else {
						DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
					}

					DE_GL_CALL(glUniform1i(shader->use_skeletal_animation, is_skinned));
					if (is_skinned) {
						de_mat4_t matrices[DE_RENDERER_MAX_SKINNING_MATRICES] = { { { 0 } } };
						de_surface_get_skinning_matrices(surf, matrices, DE_RENDERER_MAX_SKINNING_MATRICES);

						glUniformMatrix4fv(shader->bone_matrices, DE_RENDERER_MAX_SKINNING_MATRICES, GL_FALSE, (const float*)&matrices[0]);
					}

					de_renderer_render_surface(r, surf);
				}

				if (camera->in_depth_hack_mode) {
					de_camera_leave_depth_hack(camera);
				}
			}
		}

		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.opt_fbo));
		DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		DE_GL_CALL(glDisable(GL_BLEND));
		DE_GL_CALL(glDepthMask(GL_FALSE));
		DE_GL_CALL(glDisable(GL_STENCIL_TEST));
		DE_GL_CALL(glStencilMask(0xFF));
		DE_GL_CALL(glDisable(GL_CULL_FACE));

		/* add ambient lighting */
		DE_GL_CALL(glUseProgram(r->ambient_shader.program));
	
		DE_GL_CALL(glUniformMatrix4fv(r->ambient_shader.wvp_matrix, 1, GL_FALSE, frame_mvp_matrix.f));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.color_texture));
		DE_GL_CALL(glUniform1i(r->ambient_shader.diffuse_texture, 0));
		DE_GL_CALL(glUniform4f(r->ambient_shader.ambient_color, 
			r->ambient_light_color.r / 255.0f, 
			r->ambient_light_color.g / 255.0f, 
			r->ambient_light_color.b / 255.0f,
			1.0f));

		de_renderer_draw_fullscreen_quad(r);

		/* add lighting */
		DE_GL_CALL(glEnable(GL_BLEND));
		DE_GL_CALL(glBlendFunc(GL_ONE, GL_ONE));
		DE_GL_CALL(glUseProgram(r->lighting_shader.program));

		DE_GL_CALL(glUniform3f(r->lighting_shader.camera_position, camera_position.x, camera_position.y, camera_position.z));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.depth_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.depth_sampler, 0));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE1));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.color_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.color_sampler, 1));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE2));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.normal_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.normal_sampler, 2));

		for (de_node_t* light_node = scene->nodes.head; light_node; light_node = light_node->next) {
			if (light_node->type == DE_NODE_TYPE_LIGHT) {
				const de_light_t* light = &light_node->s.light;

				de_vec3_t light_pos;
				de_node_get_global_position(light_node, &light_pos);

				de_vec3_t light_emit_direction;
				de_node_get_up_vector(light_node, &light_emit_direction);
				de_vec3_normalize(&light_emit_direction, &light_emit_direction);

				if (light->type == DE_LIGHT_TYPE_POINT || light->type == DE_LIGHT_TYPE_SPOT) {
					/* TODO: spot light can be culled more accurately, but for now we cull it as
					   if it point light with some radius. */
					if (!de_frustum_sphere_intersection(&frustum, &light_pos, light->radius)) {
						continue;
					}
				}

				/* Render shadows */
				const bool render_shadows = light->cast_shadows &&
					de_vec3_distance(&camera_position, &light_pos) <= r->quality_settings.point_shadows_distance;
				
				de_mat4_t light_view_projection_matrix;

				if (light->type == DE_LIGHT_TYPE_SPOT && r->quality_settings.spot_shadows_enabled && render_shadows) {
					const de_spot_shadow_map_t* shadow_map = &r->spot_shadow_map;

					DE_GL_CALL(glDepthMask(GL_TRUE));
					DE_GL_CALL(glDisable(GL_BLEND));
					DE_GL_CALL(glDisable(GL_STENCIL_TEST));
					DE_GL_CALL(glEnable(GL_CULL_FACE));

					DE_GL_CALL(glViewport(0, 0, r->quality_settings.spot_shadow_map_size, r->quality_settings.spot_shadow_map_size));
					DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map->fbo));
					DE_GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));

					de_mat4_t light_projection_matrix;
					de_mat4_perspective(&light_projection_matrix, light->cone_angle * 2.5f, 1.0, 0.01f, light->radius);

					de_vec3_t light_look_at;
					de_vec3_sub(&light_look_at, &light_pos, &light_emit_direction);

					de_vec3_t light_up_vec;
					de_node_get_look_vector(light_node, &light_up_vec);

					de_mat4_t light_view_matrix;
					de_mat4_look_at(&light_view_matrix, &light_pos, &light_look_at, &light_up_vec);

					de_mat4_mul(&light_view_projection_matrix, &light_projection_matrix, &light_view_matrix);

					de_frustum_t light_frustum;
					de_frustum_from_matrix(&light_frustum, &light_view_projection_matrix);

					/* Bind and setup shader */
					de_spot_shadow_map_shader_t* shader = &r->spot_shadow_map_shader;
					DE_GL_CALL(glUseProgram(shader->program));

					/* Render each node into shadow map */
					for (de_node_t* mesh_node = scene->nodes.head; mesh_node; mesh_node = mesh_node->next) {
						if (mesh_node->global_visibility && mesh_node->type == DE_NODE_TYPE_MESH) {

							if (!de_frustum_box_intersection_transform(&light_frustum, &mesh_node->bounding_box, &mesh_node->global_matrix)) {
								continue;
							}

							de_mesh_t* mesh = &mesh_node->s.mesh;
							const bool is_skinned = de_mesh_is_skinned(mesh);

							de_mat4_t wvp_matrix;
							de_mat4_mul(&wvp_matrix, &light_view_projection_matrix, is_skinned ? &identity : &mesh_node->global_matrix);

							DE_GL_CALL(glUniformMatrix4fv(shader->vs.world_view_projection_matrix, 1, GL_FALSE, wvp_matrix.f));

							for (size_t i = 0; i < mesh->surfaces.size; ++i) {
								const de_surface_t* surf = mesh->surfaces.data[i];

								DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
								if (surf->diffuse_map) {
									DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->diffuse_map->id));
								} else {
									DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
								}
								DE_GL_CALL(glUniform1i(shader->fs.diffuse_texture, 0));

								DE_GL_CALL(glUniform1i(shader->vs.use_skeletal_animation, is_skinned));

								if (is_skinned) {
									de_mat4_t matrices[DE_RENDERER_MAX_SKINNING_MATRICES] = { { { 0 } } };
									de_surface_get_skinning_matrices(surf, matrices, DE_RENDERER_MAX_SKINNING_MATRICES);

									glUniformMatrix4fv(shader->vs.bone_matrices, DE_RENDERER_MAX_SKINNING_MATRICES, GL_FALSE, (const float*)&matrices[0]);
								}

								de_renderer_render_surface(r, surf);
							}
						}
					}

					DE_GL_CALL(glDepthMask(GL_FALSE));
					DE_GL_CALL(glEnable(GL_BLEND));
					DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.opt_fbo));
					de_renderer_set_viewport(&camera->viewport, frame_width, frame_height);
				} else if (light->type == DE_LIGHT_TYPE_POINT && r->quality_settings.point_shadows_enabled && render_shadows) {
					const de_point_shadow_map_t* shadow_map = &r->point_shadow_map;

					DE_GL_CALL(glDepthMask(GL_TRUE));
					DE_GL_CALL(glDisable(GL_BLEND));
					DE_GL_CALL(glDisable(GL_STENCIL_TEST));
					DE_GL_CALL(glEnable(GL_CULL_FACE));
					DE_GL_CALL(glCullFace(GL_BACK));

					DE_GL_CALL(glViewport(0, 0, r->quality_settings.point_shadow_map_size, r->quality_settings.point_shadow_map_size));
					DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, shadow_map->fbo));

					de_mat4_t light_projection_matrix;
					de_mat4_perspective(&light_projection_matrix, (float)(M_PI / 2.0), 1.0, 0.01f, light->radius);

					/* Bind and setup shader */
					de_point_shadow_map_shader_t* shader = &r->point_shadow_map_shader;
					DE_GL_CALL(glUseProgram(shader->program));

					DE_GL_CALL(glUniform3f(shader->fs.light_position, light_pos.x, light_pos.y, light_pos.z));

					typedef struct face_definition_t {
						GLenum face;
						de_vec3_t look;
						de_vec3_t up;
					} face_definition_t;

					const face_definition_t face_definitions[6] = {
						{ .face = GL_TEXTURE_CUBE_MAP_POSITIVE_X, .look = { 1.0f, 0.0f, 0.0f }, .up = { 0.0f, -1.0f, 0.0f } },
						{ .face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X, .look = { -1.0f, 0.0f, 0.0f }, .up = { 0.0f, -1.0f, 0.0f } },
						{ .face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y, .look = { 0.0f, 1.0f, 0.0f }, .up = { 0.0f, 0.0f, 1.0f } },
						{ .face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, .look = { 0.0f, -1.0f, 0.0f }, .up = { 0.0f, 0.0f, -1.0f } },
						{ .face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z, .look = { 0.0f, 0.0f, 1.0f }, .up = { 0.0f, -1.0f, 0.0f } },
						{ .face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, .look = { 0.0f, 0.0f, -1.0f }, .up = { 0.0f, -1.0f, 0.0f } },
					};

					for (size_t face = 0; face < 6; ++face) {
						const face_definition_t* face_definition = face_definitions + face;

						DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face_definition->face, shadow_map->texture, 0));
						DE_GL_CALL(glDrawBuffer(GL_COLOR_ATTACHMENT0));
						glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
						DE_GL_CALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

						de_vec3_t light_look_at;
						de_vec3_add(&light_look_at, &light_pos, &face_definition->look);

						de_mat4_t light_view_matrix;
						de_mat4_look_at(&light_view_matrix, &light_pos, &light_look_at, &face_definition->up);

						de_mat4_mul(&light_view_projection_matrix, &light_projection_matrix, &light_view_matrix);

						de_frustum_t light_frustum;
						de_frustum_from_matrix(&light_frustum, &light_view_projection_matrix);

						/* Render each node into shadow map */
						for (de_node_t* mesh_node = scene->nodes.head; mesh_node; mesh_node = mesh_node->next) {
							if (mesh_node->global_visibility && mesh_node->type == DE_NODE_TYPE_MESH) {

								if (!de_frustum_box_intersection_transform(&light_frustum, &mesh_node->bounding_box, &mesh_node->global_matrix)) {
									continue;
								}

								de_mesh_t* mesh = &mesh_node->s.mesh;
								const bool is_skinned = de_mesh_is_skinned(mesh);

								de_mat4_t wvp_matrix;
								de_mat4_mul(&wvp_matrix, &light_view_projection_matrix, is_skinned ? &identity : &mesh_node->global_matrix);

								DE_GL_CALL(glUniformMatrix4fv(shader->vs.world_view_projection_matrix, 1, GL_FALSE, wvp_matrix.f));
								DE_GL_CALL(glUniformMatrix4fv(shader->vs.world_matrix, 1, GL_FALSE, mesh_node->global_matrix.f));

								for (size_t i = 0; i < mesh->surfaces.size; ++i) {
									de_surface_t* surf = mesh->surfaces.data[i];

									DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
									if (surf->diffuse_map) {
										DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->diffuse_map->id));
									} else {
										DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
									}
									DE_GL_CALL(glUniform1i(shader->fs.diffuse_texture, 0));

									DE_GL_CALL(glUniform1i(shader->vs.use_skeletal_animation, is_skinned));

									if (is_skinned) {
										de_mat4_t matrices[DE_RENDERER_MAX_SKINNING_MATRICES] = { { { 0 } } };
										de_surface_get_skinning_matrices(surf, matrices, DE_RENDERER_MAX_SKINNING_MATRICES);

										glUniformMatrix4fv(shader->vs.bone_matrices, DE_RENDERER_MAX_SKINNING_MATRICES, GL_FALSE, (const float*)&matrices[0]);
									}

									de_renderer_render_surface(r, surf);
								}
							}
						}
					}

					DE_GL_CALL(glDepthMask(GL_FALSE));
					DE_GL_CALL(glEnable(GL_BLEND));
					DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.opt_fbo));
					de_renderer_set_viewport(&camera->viewport, frame_width, frame_height);
				}

				/* Render fullscreen quad with lighting */
				const float clr[] = {
					light->color.r / 255.0f,
					light->color.g / 255.0f,
					light->color.b / 255.0f,
					1.0f
				};

				DE_GL_CALL(glUseProgram(r->flat_shader.program));

				DE_GL_CALL(glEnable(GL_STENCIL_TEST));
				DE_GL_CALL(glStencilMask(0xFF));
				DE_GL_CALL(glColorMask(0, 0, 0, 0));

				DE_GL_CALL(glEnable(GL_CULL_FACE));

				DE_GL_CALL(glCullFace(GL_FRONT));
				DE_GL_CALL(glStencilFunc(GL_ALWAYS, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_INCR, GL_KEEP));
				de_renderer_draw_light_sphere(r, camera, &light_pos, light->radius, r->flat_shader.wvp_matrix);

				DE_GL_CALL(glCullFace(GL_BACK));
				DE_GL_CALL(glStencilFunc(GL_ALWAYS, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_DECR, GL_KEEP));
				de_renderer_draw_light_sphere(r, camera, &light_pos, light->radius, r->flat_shader.wvp_matrix);

				DE_GL_CALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));

				DE_GL_CALL(glColorMask(1, 1, 1, 1));

				DE_GL_CALL(glDisable(GL_CULL_FACE));

				const de_deferred_light_shader_t* shader = &r->lighting_shader;
				DE_GL_CALL(glUseProgram(shader->program));

				if (light->type == DE_LIGHT_TYPE_SPOT) {
					DE_GL_CALL(glActiveTexture(GL_TEXTURE3));
					DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->spot_shadow_map.texture));
					DE_GL_CALL(glUniform1i(shader->spot_shadow_texture, 3));
					DE_GL_CALL(glUniformMatrix4fv(shader->light_view_proj_matrix, 1, GL_FALSE, light_view_projection_matrix.f));
					DE_GL_CALL(glUniform1i(shader->soft_shadows, r->quality_settings.spot_soft_shadows));
				} else if (light->type == DE_LIGHT_TYPE_POINT) {
					DE_GL_CALL(glActiveTexture(GL_TEXTURE3));
					DE_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, r->point_shadow_map.texture));
					DE_GL_CALL(glUniform1i(shader->point_shadow_texture, 3));
					DE_GL_CALL(glUniform1i(shader->soft_shadows, r->quality_settings.point_soft_shadows));
				}

				DE_GL_CALL(glUniform3f(shader->light_position, light_pos.x, light_pos.y, light_pos.z));
				DE_GL_CALL(glUniform1f(shader->light_radius, light->radius));
				DE_GL_CALL(glUniformMatrix4fv(shader->inv_view_proj_matrix, 1, GL_FALSE, camera->inv_view_proj.f));
				DE_GL_CALL(glUniform4f(shader->light_color, clr[0], clr[1], clr[2], clr[3]));
				DE_GL_CALL(glUniform1f(shader->light_cone_angle_cos, light->cone_angle_cos));
				DE_GL_CALL(glUniform3f(shader->light_direction, light_emit_direction.x, light_emit_direction.y, light_emit_direction.z));
				DE_GL_CALL(glUniformMatrix4fv(shader->wvp_matrix, 1, GL_FALSE, frame_mvp_matrix.f));

				if (render_shadows && (r->quality_settings.spot_shadows_enabled || r->quality_settings.point_shadows_enabled)) {
					DE_GL_CALL(glUniform1i(shader->light_type, light->type));
				} else {
					/* Shadows will be disabled by passing invalid light type. */
					DE_GL_CALL(glUniform1i(shader->light_type, -1));
				}

				DE_GL_CALL(glUniform1f(shader->shadow_map_inv_size, 1.0f / r->quality_settings.spot_shadow_map_size));

				DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.depth_texture));

				de_renderer_draw_fullscreen_quad(r);

				/* Make sure to unbind all shadow textures */
				DE_GL_CALL(glActiveTexture(GL_TEXTURE3));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
			}
		}

		DE_GL_CALL(glDisable(GL_STENCIL_TEST));
		DE_GL_CALL(glDisable(GL_BLEND));

		DE_GL_CALL(glDepthMask(GL_TRUE));

		/* Debug render, renders node data even if node is invisible */
		if (r->render_normals) {
			DE_GL_CALL(glUseProgram(r->flat_shader.program));

			de_renderer_set_viewport(&camera->viewport, frame_width, frame_height);

			/* Render each node */
			for (de_node_t* node = scene->nodes.head; node; node = node->next) {
				de_mat4_t wvp_matrix;

				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &node->global_matrix);
				DE_GL_CALL(glUniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));

				if (node->type == DE_NODE_TYPE_MESH) {
					de_renderer_draw_mesh_normals(r, &node->s.mesh);
				}
			}
		}

		if (r->render_bones) {
			DE_GL_CALL(glUseProgram(r->flat_shader.program));

			de_renderer_set_viewport(&camera->viewport, frame_width, frame_height);

			/* Render each node */
			for (de_node_t* node = scene->nodes.head; node; node = node->next) {
				de_mat4_t wvp_matrix;
				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &identity);
				DE_GL_CALL(glUniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));

				if (node->type == DE_NODE_TYPE_MESH) {
					de_renderer_draw_mesh_bones(r, &node->s.mesh);
				}
			}
		}
	}

	/* Unbind FBO textures */
	DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	DE_GL_CALL(glActiveTexture(GL_TEXTURE1));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	DE_GL_CALL(glActiveTexture(GL_TEXTURE2));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	/* Render particle systems */
	DE_GL_CALL(glDisable(GL_CULL_FACE));
	DE_GL_CALL(glEnable(GL_BLEND));
	DE_GL_CALL(glDepthMask(GL_FALSE));
	DE_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	DE_GL_CALL(glUseProgram(r->particle_system_shader.program));

	for (de_scene_t* scene = core->scenes.head; scene; scene = scene->next) {
		const de_node_t* camera_node = scene->active_camera;

		if (!camera_node) {
			continue;
		}

		const de_camera_t* camera = &camera_node->s.camera;

		de_mat4_t inv_view;
		de_mat4_inverse(&inv_view, &camera->view_matrix);

		de_vec3_t camera_up;
		de_mat4_up(&inv_view, &camera_up);

		de_vec3_t camera_side;
		de_mat4_side(&inv_view, &camera_side);

		de_vec3_t camera_position;
		de_node_get_global_position(camera_node, &camera_position);

		for (de_node_t* node = scene->nodes.head; node; node = node->next) {
			if (node->type != DE_NODE_TYPE_PARTICLE_SYSTEM) {
				continue;
			}

			de_particle_system_t* particle_system = &node->s.particle_system;
			de_particle_system_generate_vertices(particle_system, &camera_position);

			/* Upload buffers */
			if (!particle_system->vertex_buffer) {
				glGenBuffers(1, &particle_system->vertex_buffer);
			}
			if (!particle_system->index_buffer) {
				glGenBuffers(1, &particle_system->index_buffer);
			}
			if (!particle_system->vertex_array_object) {
				glGenVertexArrays(1, &particle_system->vertex_array_object);
			}

			DE_GL_CALL(glBindVertexArray(particle_system->vertex_array_object));

			/* Upload indices */
			DE_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_system->index_buffer));
			DE_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*particle_system->indices.data) * particle_system->indices.size, particle_system->indices.data, GL_DYNAMIC_DRAW));

			/* Upload vertices */
			const size_t vertex_size = sizeof(*particle_system->vertices.data);
			const size_t total_size_bytes = vertex_size * particle_system->vertices.size;

			DE_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particle_system->vertex_buffer));
			DE_GL_CALL(glBufferData(GL_ARRAY_BUFFER, total_size_bytes, particle_system->vertices.data, GL_DYNAMIC_DRAW));

			/* Setup attribute locations for shared data */
			DE_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(de_particle_vertex_t, position)));
			DE_GL_CALL(glEnableVertexAttribArray(0));

			DE_GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(de_particle_vertex_t, tex_coord)));
			DE_GL_CALL(glEnableVertexAttribArray(1));

			DE_GL_CALL(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(de_particle_vertex_t, size)));
			DE_GL_CALL(glEnableVertexAttribArray(2));

			DE_GL_CALL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(de_particle_vertex_t, rotation)));
			DE_GL_CALL(glEnableVertexAttribArray(3));

			DE_GL_CALL(glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertex_size, (void*)offsetof(de_particle_vertex_t, color)));
			DE_GL_CALL(glEnableVertexAttribArray(4));

			/* Set uniforms */
			const de_particle_system_shader_t* shader = &r->particle_system_shader;
			DE_GL_CALL(glUniformMatrix4fv(shader->vs.view_projection_matrix, 1, GL_FALSE, camera->view_projection_matrix.f));
			DE_GL_CALL(glUniformMatrix4fv(shader->vs.world_matrix, 1, GL_FALSE, node->global_matrix.f));
			DE_GL_CALL(glUniform4f(shader->vs.camera_up_vector, camera_up.x, camera_up.y, camera_up.z, 0.0f));
			DE_GL_CALL(glUniform4f(shader->vs.camera_side_vector, camera_side.x, camera_side.y, camera_side.z, 0.0f));

			DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
			if (particle_system->texture) {
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, particle_system->texture->id));
			} else {
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
			}
			DE_GL_CALL(glUniform1i(shader->fs.diffuse_texture, 0));

			DE_GL_CALL(glActiveTexture(GL_TEXTURE1));
			DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.depth_texture));
			DE_GL_CALL(glUniform1i(shader->fs.depth_buffer_texture, 1));

			const float inv_width = 1.0f / frame_width;
			const float inv_height = 1.0f / frame_height;
			DE_GL_CALL(glUniform2f(shader->fs.inv_screen_size, inv_width, inv_height));

			DE_GL_CALL(glUniform2f(shader->fs.proj_params, camera->z_far, camera->z_near));

			DE_GL_CALL(glDrawElements(GL_TRIANGLES, particle_system->indices.size, GL_UNSIGNED_INT, NULL));
			++r->draw_calls;
		}
	}

	DE_GL_CALL(glDisable(GL_BLEND));
	DE_GL_CALL(glDepthMask(GL_TRUE));

	/* Render final frame from deferred lighting pass to back buffer */
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	const de_rectf_t gui_viewport = { 0, 0, 1, 1 };
	de_renderer_set_viewport(&gui_viewport, frame_width, frame_height);

	DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
	DE_GL_CALL(glUseProgram(r->flat_shader.program));
	DE_GL_CALL(glUniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, frame_mvp_matrix.f));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.frame_texture));

	de_renderer_draw_fullscreen_quad(r);

	DE_GL_CALL(glDisable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_BLEND));
	DE_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	DE_GL_CALL(glEnable(GL_STENCIL_TEST));
	DE_GL_CALL(glDisable(GL_CULL_FACE));
	/* Render GUI */
	{
		const de_gui_draw_list_t* draw_list = de_gui_render(core->gui);

		DE_GL_CALL(glUseProgram(r->gui_shader.program));
		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));

		const int index_bytes = DE_ARRAY_SIZE_BYTES(draw_list->index_buffer);
		const int vertex_bytes = DE_ARRAY_SIZE_BYTES(draw_list->vertex_buffer);

		/* upload to gpu */
		DE_GL_CALL(glBindVertexArray(r->gui_render_buffers.vao));

		DE_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, r->gui_render_buffers.vbo));
		DE_GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertex_bytes, draw_list->vertex_buffer.data, GL_DYNAMIC_DRAW));

		DE_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->gui_render_buffers.ebo));
		DE_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_bytes, draw_list->index_buffer.data, GL_DYNAMIC_DRAW));

		DE_GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)0));
		DE_GL_CALL(glEnableVertexAttribArray(0));

		DE_GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, tex_coord)));
		DE_GL_CALL(glEnableVertexAttribArray(1));

		DE_GL_CALL(glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, color)));
		DE_GL_CALL(glEnableVertexAttribArray(2));

		de_mat4_t ortho;
		de_mat4_ortho(&ortho, 0, (float)frame_width, (float)frame_height, 0, -1, 1);
		DE_GL_CALL(glUniformMatrix4fv(r->gui_shader.wvp_matrix, 1, GL_FALSE, ortho.f));

		/* draw */
		for (size_t i = 0; i < draw_list->commands.size; ++i) {
			const de_gui_draw_command_t* cmd = draw_list->commands.data + i;
			const size_t index_count = cmd->triangle_count * 3;
			if (cmd->type == DE_GUI_DRAW_COMMAND_TYPE_CLIP) {
				bool is_root_nesting = cmd->nesting == 1;
				if (is_root_nesting) {
					DE_GL_CALL(glClear(GL_STENCIL_BUFFER_BIT));
				}
				if (cmd->nesting != 0) {
					glEnable(GL_STENCIL_TEST);
				} else {
					glDisable(GL_STENCIL_TEST);
				}
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
				/* make sure that clipping rect will be drawn at previous nesting level only (clip to parent) */
				DE_GL_CALL(glStencilFunc(GL_EQUAL, cmd->nesting - 1, 0xFF));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
				/* draw clipping geometry to stencil buffer */
				DE_GL_CALL(glStencilMask(0xFF));
			} else {
				/* make sure to draw geometry only on clipping geometry with current nesting level */
				DE_GL_CALL(glStencilFunc(GL_EQUAL, cmd->nesting, 0xFF));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, cmd->texture ? cmd->texture : r->white_dummy->id));
				/* do not draw geometry to stencil buffer */
				DE_GL_CALL(glStencilMask(0x00));
			}
			DE_GL_CALL(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (void*)(cmd->index_offset * sizeof(GLuint))));
			++r->draw_calls;
		}

		glBindVertexArray(0);
	}
	DE_GL_CALL(glDisable(GL_STENCIL_TEST));
	DE_GL_CALL(glDisable(GL_BLEND));
	DE_GL_CALL(glEnable(GL_DEPTH_TEST));

	const double frame_end_time = de_time_get_seconds();
	r->frame_time = 1000.0 * (frame_end_time - frame_start_time);

	de_core_platform_swap_buffers(r->core);
	
	/* FPS limiter */
	if (r->frame_rate_limit > 0) {
		const int time_limit_ms = 1000 / r->frame_rate_limit;
		const int current_time_ms = (int)(1000 * de_time_get_seconds());
		de_sleep(time_limit_ms - (current_time_ms - last_time_ms));
		last_time_ms = (int)(1000 * de_time_get_seconds());
	}

	/* Measure mean, current and min FPS. */
	const double total_time = de_time_get_seconds() - frame_start_time;	
	r->current_fps = (size_t)(1.0 / total_time);
	if (r->current_fps < r->min_fps) {
		r->min_fps = r->current_fps;
	}
	r->frame_time_accumulator += total_time;
	++r->frame_time_measurements;
	if (de_time_get_seconds() - r->time_last_fps_measured >= 1.0) {
		r->mean_fps = (size_t)(1.0 / (r->frame_time_accumulator / r->frame_time_measurements));
		r->time_last_fps_measured = total_time;
		r->frame_time_measurements = 0;
		r->frame_time_accumulator = 0;
	}
}

void de_renderer_set_framerate_limit(de_renderer_t* r, int limit)
{
	DE_ASSERT(r);
	r->frame_rate_limit = limit;
}

size_t de_renderer_get_mean_fps(de_renderer_t* r)
{
	DE_ASSERT(r);
	return r->mean_fps;
}

double de_render_get_frame_time(de_renderer_t* r)
{
	DE_ASSERT(r);
	return r->frame_time;
}

void de_renderer_set_ambient_light_color(de_renderer_t* r, const de_color_t* clr)
{
	DE_ASSERT(r);
	r->ambient_light_color = *clr;
}

de_color_t de_renderer_get_ambient_light_color(de_renderer_t* r) 
{
	DE_ASSERT(r);
	return r->ambient_light_color;
}