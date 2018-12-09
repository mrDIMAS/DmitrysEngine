/* Built-in shaders */
static const  char* de_flat_fs =
"#version 330 core\n"
"uniform sampler2D diffuseTexture;\n"
"out vec4 FragColor;\n"
"in vec2 texCoord;\n"
"void main()\n"
"{\n"
"	FragColor = texture(diffuseTexture, texCoord);\n"
"}\n";

static const  char* de_flat_vs =
"#version 330 core\n"
"layout(location = 0) in vec3 vertexPosition;\n"
"layout(location = 1) in vec2 vertexTexCoord;\n"
"layout(location = 2) in vec4 vertexColor;\n"
"uniform mat4 worldViewProjection;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"	texCoord = vertexTexCoord;\n"
"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);\n"
"}\n";

static const char* de_gui_fs =
"#version 330 core\n"
"uniform sampler2D diffuseTexture;\n"
"out vec4 FragColor;\n"
"in vec2 texCoord;\n"
"in vec4 color;\n"
"void main()\n"
"{\n"
"	FragColor = color * texture(diffuseTexture, texCoord);\n"
"}\n";

static const  char* de_gui_vs =
"#version 330 core\n"
"layout(location = 0) in vec3 vertexPosition;\n"
"layout(location = 1) in vec2 vertexTexCoord;\n"
"layout(location = 2) in vec4 vertexColor;\n"
"uniform mat4 worldViewProjection;\n"
"out vec2 texCoord;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"	texCoord = vertexTexCoord;\n"
"   color = vertexColor;\n"
"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);\n"
"}\n";

static const  char* de_gbuffer_fs =
"#version 330 core\n"
"layout(location = 0) out float outDepth;\n"
"layout(location = 1) out vec4 outColor;\n"
"layout(location = 2) out vec3 outNormal;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform vec4 diffuseColor;\n"
"uniform float selfEmittance;\n"
"in vec4 position;\n"
"in vec3 normal;\n"
"in vec2 texCoord;\n"
"void main()\n"
"{\n"
"	outDepth = position.z / position.w;\n"
"	outColor = texture2D(diffuseTexture, texCoord);\n"
"	outColor.a = 1;\n"
"	outNormal = normalize(normal) * 0.5 + 0.5;\n"
"}\n";

static const char de_gbuffer_vs[] =
"#version 330 core\n"
"layout(location = 0) in vec3 vertexPosition;\n"
"layout(location = 1) in vec2 vertexTexCoord;\n"
"layout(location = 2) in vec3 vertexNormal;\n"
"uniform mat4 worldMatrix;\n"
"uniform mat4 worldViewProjection;\n"
"out vec4 position;\n"
"out vec3 normal;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);\n"
"	texCoord = vertexTexCoord;\n"
"	normal = mat3(worldMatrix) * vertexNormal;\n"
"	position = gl_Position;\n"
"}\n";

static const char* const de_light_fs =
"#version 330 core\n"
"uniform sampler2D depthTexture;\n"
"uniform sampler2D colorTexture;\n"
"uniform sampler2D normalTexture;\n"
"uniform vec3 lightPos;\n"
"uniform float lightRadius;\n"
"uniform vec4 lightColor;\n"
"uniform vec3 lightDirection;\n"
"uniform float coneAngleCos;\n"
"uniform mat4 invViewProj;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	vec3 normal = texture2D(normalTexture, texCoord).xyz * 2.0 - 1.0;\n"
"	vec4 screenPosition;\n"
"	screenPosition.x = texCoord.x * 2.0 - 1.0;\n"
"	screenPosition.y = texCoord.y * 2.0 - 1.0;\n"
"	screenPosition.z = texture2D(depthTexture, texCoord).r;\n"
"	screenPosition.w = 1.0;\n"
"	vec4 worldPosition = invViewProj * screenPosition;\n"
"	worldPosition.xyz /= worldPosition.w;\n"
"	vec3 lightVector = lightPos - worldPosition.xyz;\n"
"	float d = min(length(lightVector), lightRadius);\n"
"	vec3 normLightVector = lightVector / d;\n"
"	float y = dot(lightDirection, normLightVector);\n"
"	float k = max(dot(normal, normLightVector), 0);\n"
"	float attenuation = 1.0 + cos((d / lightRadius) * 3.14159);\n"
"	if (y < coneAngleCos)\n"
"	{\n"
"		attenuation *= smoothstep(coneAngleCos - 0.1, coneAngleCos, y);\n"
"	}\n"
"	FragColor = k * attenuation * texture2D(colorTexture, texCoord) * lightColor;\n"
"}\n";

static const  char* de_light_vs =
"#version 330 core\n"
"layout(location = 0) in vec3 vertexPosition;\n"
"layout(location = 1) in vec2 vertexTexCoord;\n"
"uniform mat4 worldViewProjection;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = worldViewProjection * vec4(vertexPosition, 1.0);\n"
"	texCoord = vertexTexCoord;\n"
"}\n";

#define DE_GL_CALL(func) func; de_check_opengl_error(#func, __FILE__, __LINE__)

/*=======================================================================================*/
void de_check_opengl_error(const char* func, const char* file, int line)
{
	int err;
	const char* errorDesc = "Unknown";

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		switch (err)
		{
		case GL_INVALID_ENUM:
			errorDesc = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorDesc = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errorDesc = "GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			errorDesc = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			errorDesc = "GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			errorDesc = "GL_OUT_OF_MEMORY";
			break;
		}
		de_error("OpenGL error \"%s\" occured at function:\n%s\nat line %d in file %s", errorDesc, func, line, file);
	}
}

GLuint de_renderer_create_gpu_program(const char* vertexSource, const char* fragmentSource);
static void de_renderer_upload_surface(de_surface_t* s);

/*=======================================================================================*/
static void de_renderer_load_extensions()
{
	de_renderer_t* r = &de_engine->renderer;

	#define GET_GL_EXT(type, func) r->gl.func = (type)de_engine_platform_get_proc_address("gl"#func); \
							   if(!r->gl.func) de_error("Unable to load gl"#func" function pointer");

	GET_GL_EXT(PFNGLCREATESHADERPROC, CreateShader);
	GET_GL_EXT(PFNGLDELETESHADERPROC, DeleteShader);
	GET_GL_EXT(PFNGLSHADERSOURCEPROC, ShaderSource);
	GET_GL_EXT(PFNGLCOMPILESHADERPROC, CompileShader);
	GET_GL_EXT(PFNGLGETSHADERINFOLOGPROC, GetShaderInfoLog);
	GET_GL_EXT(PFNGLGETSHADERIVPROC, GetShaderiv);

	GET_GL_EXT(PFNGLCREATEPROGRAMPROC, CreateProgram);
	GET_GL_EXT(PFNGLATTACHSHADERPROC, AttachShader);
	GET_GL_EXT(PFNGLLINKPROGRAMPROC, LinkProgram);
	GET_GL_EXT(PFNGLGETPROGRAMINFOLOGPROC, GetProgramInfoLog);
	GET_GL_EXT(PFNGLUSEPROGRAMPROC, UseProgram);
	GET_GL_EXT(PFNGLDELETEPROGRAMPROC, DeleteProgram);
	GET_GL_EXT(PFNGLGETUNIFORMLOCATIONPROC, GetUniformLocation);
	GET_GL_EXT(PFNGLUNIFORM4FPROC, Uniform4f);
	GET_GL_EXT(PFNGLUNIFORMMATRIX4FVPROC, UniformMatrix4fv);
	GET_GL_EXT(PFNGLUNIFORM1IPROC, Uniform1i);
	GET_GL_EXT(PFNGLGETPROGRAMIVPROC, GetProgramiv);
	GET_GL_EXT(PFNGLUNIFORM3FPROC, Uniform3f);
	GET_GL_EXT(PFNGLUNIFORM1FPROC, Uniform1f);

	GET_GL_EXT(PFNGLGENBUFFERSPROC, GenBuffers);
	GET_GL_EXT(PFNGLBINDBUFFERPROC, BindBuffer);
	GET_GL_EXT(PFNGLDELETEBUFFERSPROC, DeleteBuffers);
	GET_GL_EXT(PFNGLBUFFERDATAPROC, BufferData);

	GET_GL_EXT(PFNGLVERTEXATTRIBPOINTERPROC, VertexAttribPointer);
	GET_GL_EXT(PFNGLENABLEVERTEXATTRIBARRAYPROC, EnableVertexAttribArray);
	GET_GL_EXT(PFNGLGENVERTEXARRAYSPROC, GenVertexArrays);
	GET_GL_EXT(PFNGLBINDVERTEXARRAYPROC, BindVertexArray);
	GET_GL_EXT(PFNGLDELETEVERTEXARRAYSPROC, DeleteVertexArrays);

	GET_GL_EXT(PFNGLACTIVETEXTUREPROC, ActiveTexture);
	GET_GL_EXT(PFNGLCLIENTACTIVETEXTUREPROC, ClientActiveTexture);
	GET_GL_EXT(PFNGLGENERATEMIPMAPPROC, GenerateMipmap);

	GET_GL_EXT(PFNGLGETSTRINGIPROC, GetStringi);

	GET_GL_EXT(PFNGLFRAMEBUFFERTEXTURE2DPROC, FramebufferTexture2D);
	GET_GL_EXT(PFNGLGENFRAMEBUFFERSPROC, GenFramebuffers);
	GET_GL_EXT(PFNGLGENRENDERBUFFERSPROC, GenRenderbuffers);
	GET_GL_EXT(PFNGLBINDRENDERBUFFERPROC, BindRenderbuffer);
	GET_GL_EXT(PFNGLRENDERBUFFERSTORAGEPROC, RenderbufferStorage);
	GET_GL_EXT(PFNGLFRAMEBUFFERRENDERBUFFERPROC, FramebufferRenderbuffer);
	GET_GL_EXT(PFNGLFRAMEBUFFERTEXTUREPROC, FramebufferTexture);
	GET_GL_EXT(PFNGLDRAWBUFFERSPROC, DrawBuffers);
	GET_GL_EXT(PFNGLCHECKFRAMEBUFFERSTATUSPROC, CheckFramebufferStatus);
	GET_GL_EXT(PFNGLBINDFRAMEBUFFERPROC, BindFramebuffer);

	#undef GET_GL_EXT

	de_log("Extensions loaded!");
}

/*=======================================================================================*/
static void de_create_gbuffer(int width, int height)
{
	de_renderer_t* r = &de_engine->renderer;
	de_gbuffer_t * gbuf = &r->gbuffer;
	de_gl_ext_t* gl = &r->gl;

	DE_GL_CALL(gl->GenFramebuffers(1, &gbuf->fbo));
	DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, gbuf->fbo));

	DE_GL_CALL(gl->GenRenderbuffers(1, &gbuf->depth_rt));
	DE_GL_CALL(gl->BindRenderbuffer(GL_RENDERBUFFER, gbuf->depth_rt));
	DE_GL_CALL(gl->RenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height));
	DE_GL_CALL(gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuf->depth_rt));

	DE_GL_CALL(gl->GenRenderbuffers(1, &gbuf->color_rt));
	DE_GL_CALL(gl->BindRenderbuffer(GL_RENDERBUFFER, gbuf->color_rt));
	DE_GL_CALL(gl->RenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, gbuf->color_rt));

	DE_GL_CALL(gl->GenRenderbuffers(1, &gbuf->normal_rt));
	DE_GL_CALL(gl->BindRenderbuffer(GL_RENDERBUFFER, gbuf->normal_rt));
	DE_GL_CALL(gl->RenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, gbuf->normal_rt));

	DE_GL_CALL(gl->GenRenderbuffers(1, &gbuf->depth_buffer));
	DE_GL_CALL(gl->BindRenderbuffer(GL_RENDERBUFFER, gbuf->depth_buffer));
	DE_GL_CALL(gl->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	DE_GL_CALL(gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuf->depth_buffer));

	DE_GL_CALL(glGenTextures(1, &gbuf->depth_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->depth_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_BGRA, GL_FLOAT, NULL));

	DE_GL_CALL(gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuf->depth_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuf->color_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->color_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuf->color_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuf->normal_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->normal_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuf->normal_texture, 0));

	if (gl->CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		de_error("Unable to construct G-Buffer FBO.");
	}

	DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, 0));

	/* Create another framebuffer for stencil optimizations */
	DE_GL_CALL(gl->GenFramebuffers(1, &gbuf->opt_fbo));
	DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, gbuf->opt_fbo));

	DE_GL_CALL(glGenTextures(1, &gbuf->frame_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->frame_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuf->frame_texture, 0));

	DE_GL_CALL(gl->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuf->depth_buffer));

	if (gl->CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		de_error("Unable to initialize Stencil FBO.");
	}

	DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, 0));
}

/*=======================================================================================*/
static void de_create_builtin_shaders()
{
	de_renderer_t* r = &de_engine->renderer;
	de_gl_ext_t* gl = &r->gl;

	/* Flat shader */
	{
		de_flag_shader_t* s = &r->flat_shader;

		s->program = de_renderer_create_gpu_program(de_flat_vs, de_flat_fs);
		s->wvp_matrix = gl->GetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = gl->GetUniformLocation(s->program, "diffuseTexture");
	}

	/* gui shader */
	{
		de_gui_shader_t* s = &r->gui_shader;

		s->program = de_renderer_create_gpu_program(de_gui_vs, de_gui_fs);
		s->wvp_matrix = gl->GetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = gl->GetUniformLocation(s->program, "diffuseTexture");
	}

	/* GBuffer shader */
	{
		de_gbuffer_shader_t* s = &r->gbuffer_shader;

		s->program = de_renderer_create_gpu_program(de_gbuffer_vs, de_gbuffer_fs);
		s->wvp_matrix = gl->GetUniformLocation(s->program, "worldViewProjection");
		s->world_matrix = gl->GetUniformLocation(s->program, "worldMatrix");

		s->diffuse_texture = gl->GetUniformLocation(s->program, "diffuseTexture");
		s->diffuse_color = gl->GetUniformLocation(s->program, "diffuseColor");
		s->self_emittance = gl->GetUniformLocation(s->program, "selfEmittance");
	}

	/* Lighting Shader */
	{
		de_gbuffer_light_shader_t* s = &r->lighting_shader;

		s->program = de_renderer_create_gpu_program(de_light_vs, de_light_fs);

		s->wvp_matrix = gl->GetUniformLocation(s->program, "worldViewProjection");

		s->depth_sampler = gl->GetUniformLocation(s->program, "depthTexture");
		s->color_sampler = gl->GetUniformLocation(s->program, "colorTexture");
		s->normal_sampler = gl->GetUniformLocation(s->program, "normalTexture");

		s->light_position = gl->GetUniformLocation(s->program, "lightPos");
		s->light_radius = gl->GetUniformLocation(s->program, "lightRadius");
		s->light_color = gl->GetUniformLocation(s->program, "lightColor");
		s->light_direction = gl->GetUniformLocation(s->program, "lightDirection");
		s->light_cone_angle_cos = gl->GetUniformLocation(s->program, "coneAngleCos");
		s->inv_view_proj_matrix = gl->GetUniformLocation(s->program, "invViewProj");
	}
}

/*=======================================================================================*/
void de_engine_init_renderer()
{
	#if VERBOSE_INIT
	int i, num_extensions;
	#endif
	de_renderer_t* r = &de_engine->renderer;

	de_log("GPU Vendor: %s", glGetString(GL_VENDOR));
	de_log("GPU: %s", glGetString(GL_RENDERER));
	de_log("OpenGL Version: %s", glGetString(GL_VERSION));
	de_log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	de_renderer_load_extensions();
	de_create_builtin_shaders();

	#if VERBOSE_INIT
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (i = 0; i < num_extensions; ++i)
	{
		de_log((char*)glGetStringi(GL_EXTENSIONS, i));
	}
	#endif

	DE_GL_CALL(glEnable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_CULL_FACE));
	glCullFace(GL_FRONT);

	de_create_gbuffer(de_engine->params.width, de_engine->params.height);

	/* Create fullscreen quad */
	{
		float w = (float)de_engine->params.width;
		float h = (float)de_engine->params.height;

		int faces[] = {
			0, 1, 2,
			0, 2, 3,
		};

		de_vertex_t vertices[4];

		memset(vertices, 0, sizeof(vertices));

		de_vec3_set(&vertices[0].position, 0, 0, 0);
		de_vec3_set(&vertices[1].position, w, 0, 0);
		de_vec3_set(&vertices[2].position, w, h, 0);
		de_vec3_set(&vertices[3].position, 0, h, 0);

		de_vec2_set(&vertices[0].tex_coord, 0, 0);
		de_vec2_set(&vertices[1].tex_coord, 1, 0);
		de_vec2_set(&vertices[2].tex_coord, 1, 1);
		de_vec2_set(&vertices[3].tex_coord, 0, 1);

		r->quad = de_renderer_create_surface(r);
		de_surface_load_data(r->quad, vertices, DE_ARRAY_SIZE(vertices), faces, DE_ARRAY_SIZE(faces));
		de_renderer_upload_surface(r->quad);
	}

	r->gl.GenVertexArrays(1, &r->gui_render_buffers.vao);
	r->gl.GenBuffers(1, &r->gui_render_buffers.vbo);
	r->gl.GenBuffers(1, &r->gui_render_buffers.ebo);

	r->test_surface = de_renderer_create_surface(r);
	{
		de_rgba8_t* pixel;
		r->white_dummy = de_renderer_create_texture(1, 1, 4);
		pixel = (de_rgba8_t*)r->white_dummy->pixels;
		pixel->r = pixel->g = pixel->b = pixel->a = 255;
	}
}

/*=======================================================================================*/
static void de_render_surface_normals(de_surface_t* surface)
{
	size_t i;
	de_renderer_t* r = &de_engine->renderer;

	DE_ARRAY_CLEAR(r->test_surface->vertices);
	DE_ARRAY_CLEAR(r->test_surface->indices);
	r->test_surface->need_upload = DE_TRUE;
	for (i = 0; i < surface->vertices.size; ++i)
	{
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
	de_surface_upload(r->test_surface);

	DE_GL_CALL(r->gl.BindVertexArray(r->test_surface->vao));
	DE_GL_CALL(glDrawElements(GL_LINES, r->test_surface->indices.size, GL_UNSIGNED_INT, NULL));
}

/*=======================================================================================*/
GLuint de_renderer_create_shader(GLenum type, const char* source)
{
	GLint compiled;
	GLuint shader;

	de_gl_ext_t* gl = &de_engine->renderer.gl;

	shader = gl->CreateShader(type);
	DE_GL_CALL(gl->ShaderSource(shader, 1, &source, NULL));

	DE_GL_CALL(gl->CompileShader(shader));

	DE_GL_CALL(gl->GetShaderiv(shader, GL_COMPILE_STATUS, &compiled));

	if (!compiled)
	{
		static char buffer[2048];
		DE_GL_CALL(gl->GetShaderInfoLog(shader, 2048, NULL, buffer));
		de_error(buffer);
	}

	return shader;
}

/*=======================================================================================*/
GLuint de_renderer_create_gpu_program(const char* vertexSource, const char* fragmentSource)
{
	GLint linked;
	GLuint vertex_shader, fragment_shader, program;
	de_gl_ext_t* gl = &de_engine->renderer.gl;

	vertex_shader = de_renderer_create_shader(GL_VERTEX_SHADER, vertexSource);
	fragment_shader = de_renderer_create_shader(GL_FRAGMENT_SHADER, fragmentSource);
	program = gl->CreateProgram();

	DE_GL_CALL(gl->AttachShader(program, vertex_shader));
	DE_GL_CALL(gl->DeleteShader(vertex_shader));

	DE_GL_CALL(gl->AttachShader(program, fragment_shader));
	DE_GL_CALL(gl->DeleteShader(fragment_shader));

	DE_GL_CALL(gl->LinkProgram(program));

	DE_GL_CALL(gl->GetProgramiv(program, GL_LINK_STATUS, &linked));

	if (!linked)
	{
		static char buffer[2048];
		DE_GL_CALL(gl->GetProgramInfoLog(program, 2048, NULL, buffer));
		de_error(buffer);
	}

	return program;
}

/*=======================================================================================*/
static void de_renderer_upload_surface(de_surface_t* s)
{
	de_gl_ext_t* gl = &de_engine->renderer.gl;

	gl->BindVertexArray(s->vao);

	gl->BindBuffer(GL_ARRAY_BUFFER, s->vbo);
	gl->BufferData(GL_ARRAY_BUFFER, DE_ARRAY_SIZE_BYTES(s->vertices), s->vertices.data, GL_STATIC_DRAW);

	gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->ebo);
	gl->BufferData(GL_ELEMENT_ARRAY_BUFFER, DE_ARRAY_SIZE_BYTES(s->indices), s->indices.data, GL_STATIC_DRAW);

	gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)0);
	gl->EnableVertexAttribArray(0);

	gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, tex_coord));
	gl->EnableVertexAttribArray(1);

	gl->VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, normal));
	gl->EnableVertexAttribArray(2);

	gl->BindVertexArray(0);

	s->need_upload = DE_FALSE;
}

/*=======================================================================================*/
de_surface_t* de_renderer_create_surface()
{
	de_renderer_t* r = &de_engine->renderer;
	de_surface_t* surf = DE_NEW(de_surface_t);

	surf->need_upload = DE_TRUE;

	/* Create gpu-side buffers */
	r->gl.GenVertexArrays(1, &surf->vao);
	r->gl.GenBuffers(1, &surf->vbo);
	r->gl.GenBuffers(1, &surf->ebo);

	return surf;
}

/*=======================================================================================*/
void de_renderer_free_surface(de_surface_t* surf)
{
	de_renderer_t* r = &de_engine->renderer;

	/* Unref texture */
	de_texture_release(surf->texture);

	/* Delete gpu buffers */
	r->gl.DeleteBuffers(1, &surf->vbo);
	r->gl.DeleteBuffers(1, &surf->ebo);
	r->gl.DeleteVertexArrays(1, &surf->vao);

	/* Delete buffers */
	DE_ARRAY_FREE(surf->indices);
	DE_ARRAY_FREE(surf->vertices);
	de_free(surf);
}

/*=======================================================================================*/
de_texture_t* de_renderer_request_texture(const char* file)
{
	de_texture_t* tex;
	de_image_t img = { 0 };
	de_renderer_t* r = &de_engine->renderer;

	/* Look for already loaded textures */
	{
		de_texture_t* texture = NULL;
		DE_LINKED_LIST_FOR_EACH(r->textures, texture)
		{
			if (texture->name)
			{
				if (strcmp(texture->name, file) == 0)
				{
					return texture;
				}
			}
		}
	}

	/* No already loaded textures, try load new from file */
	if (strstr(file, ".tga"))
	{
		if (!de_image_load_tga(file, &img))
		{
			return NULL;
		}
	}
	else
	{
		de_log("Warning: Unsupported texture format! %s is not loaded!", file);

		return NULL;
	}

	tex = DE_NEW(de_texture_t);
	tex->width = img.width;
	tex->height = img.height;
	tex->byte_per_pixel = img.byte_per_pixel;
	tex->name = de_str_copy(file);
	tex->depth = 0;
	tex->id = 0;
	tex->type = DE_TEXTURE_TYPE_2D;
	tex->pixels = img.data;
	tex->ref_count = 1;
	tex->need_upload = 1;

	DE_LINKED_LIST_APPEND(r->textures, tex);

	de_log("Texture %s is loaded!", file);

	return tex;
}

/*=======================================================================================*/
de_texture_t* de_renderer_create_texture(size_t w, size_t h, size_t byte_per_pixel)
{
	de_texture_t* tex;
	de_renderer_t* r = &de_engine->renderer;

	tex = DE_NEW(de_texture_t);
	tex->width = w;
	tex->height = h;
	tex->byte_per_pixel = byte_per_pixel;
	tex->name = NULL;
	tex->depth = 0;
	tex->id = 0;
	tex->type = DE_TEXTURE_TYPE_2D;
	tex->pixels = de_calloc(w * h * byte_per_pixel, 1);
	tex->ref_count = 1;
	tex->need_upload = 1;

	DE_LINKED_LIST_APPEND(r->textures, tex);
	return tex;
}

/*=======================================================================================*/
static void de_renderer_remove_texture(de_texture_t* tex)
{
	glDeleteTextures(1, &tex->id);
	DE_LINKED_LIST_REMOVE(de_engine->renderer.textures, tex);
}

/*=======================================================================================*/
static void de_upload_texture(de_texture_t* texture)
{
	GLint internalFormat;
	GLint format;
	GLfloat max_anisotropy;
	de_renderer_t* r = &de_engine->renderer;

	switch (texture->byte_per_pixel)
	{
	case 3:
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA;
		format = GL_RGBA;
		break;
	default:
		de_error("Unknown texture byte per pixel: %d", texture->byte_per_pixel);
		return;
	}

	if (!texture->id)
	{
		DE_GL_CALL(glGenTextures(1, &texture->id));
	}

	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->pixels));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	DE_GL_CALL(r->gl.GenerateMipmap(GL_TEXTURE_2D));
	DE_GL_CALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy));
	DE_GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	texture->need_upload = DE_FALSE;
}

static void de_render_fullscreen_quad()
{
	de_renderer_t* r = &de_engine->renderer;
	DE_GL_CALL(r->gl.BindVertexArray(r->quad->vao));
	DE_GL_CALL(glDrawElements(GL_TRIANGLES, r->quad->indices.size, GL_UNSIGNED_INT, NULL));
}

static void de_render_mesh(de_mesh_t* mesh)
{
	size_t i;
	de_gl_ext_t* gl = &de_engine->renderer.gl;

	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_surface_t* surf = mesh->surfaces.data[i];

		if (surf->need_upload)
		{
			de_renderer_upload_surface(surf);
		}

		DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE0));
		if (surf->texture)
		{
			DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->texture->id));
		}
		else
		{
			DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, de_engine->renderer.white_dummy->id));
		}

		DE_GL_CALL(gl->BindVertexArray(surf->vao));
		DE_GL_CALL(glDrawElements(GL_TRIANGLES, surf->indices.size, GL_UNSIGNED_INT, NULL));
	}
}

static void de_render_mesh_normals(de_mesh_t* mesh)
{
	size_t i;
	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_render_surface_normals(mesh->surfaces.data[i]);
	}
}

static void de_set_viewport(const de_rectf_t* viewport, unsigned int window_width, unsigned int window_height)
{
	int viewport_x = (int)(viewport->x * window_width);
	int viewport_y = (int)(viewport->y * window_height);
	int viewport_w = (int)(viewport->w * window_width);
	int viewport_h = (int)(viewport->h * window_height);

	DE_GL_CALL(glViewport(viewport_x, viewport_y, viewport_w, viewport_h));
}

static void de_upload_textures()
{
	de_texture_t* texture;

	DE_LINKED_LIST_FOR_EACH(de_engine->renderer.textures, texture)
	{
		if (texture->need_upload)
		{
			de_upload_texture(texture);
		}
	}
}

void de_render()
{
	de_renderer_t* r = &de_engine->renderer;
	de_gl_ext_t* gl = &r->gl;
	static int last_time_ms;
	int current_time_ms;
	int time_limit_ms;
	size_t i;
	de_scene_t* scene;
	de_mat4_t y_flip_ortho, ortho;
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	float w = (float)de_engine->params.width;
	float h = (float)de_engine->params.height;

	/* Upload textures first */
	de_upload_textures(r);

	DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.fbo));
	glClearColor(0.1f, 0.1f, 0.1f, 0);
	DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	glClearColor(0.0f, 0.0f, 0.0f, 0);

	DE_GL_CALL(gl->DrawBuffers(3, buffers));

	DE_GL_CALL(gl->UseProgram(r->gbuffer_shader.program));
	DE_GL_CALL(gl->Uniform1i(r->gbuffer_shader.diffuse_texture, 0));

	/* render each scene */
	DE_LINKED_LIST_FOR_EACH(de_engine->scenes, scene)
	{
		de_node_t* node;
		de_camera_t* camera;

		camera = &scene->active_camera->s.camera;

		de_camera_update(camera);

		de_set_viewport(&camera->viewport, de_engine->params.width, de_engine->params.height);

		/* Render each node */
		DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
		{
			de_mat4_t wvp_matrix;

			de_node_calculate_transforms(node);

			de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &node->global_matrix);

			DE_GL_CALL(gl->UniformMatrix4fv(r->gbuffer_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));
			DE_GL_CALL(gl->UniformMatrix4fv(r->gbuffer_shader.world_matrix, 1, GL_FALSE, node->global_matrix.f));

			if (node->type == DE_NODE_MESH)
			{
				de_render_mesh(&node->s.mesh);
			}
		}

		DE_GL_CALL(gl->BindFramebuffer(GL_FRAMEBUFFER, 0));
		DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		DE_GL_CALL(gl->UseProgram(r->lighting_shader.program));

		DE_GL_CALL(glDisable(GL_CULL_FACE));
		DE_GL_CALL(glDisable(GL_DEPTH_TEST));

		de_mat4_ortho(&y_flip_ortho, 0, w, 0, h, -1, 1);
		DE_GL_CALL(gl->UniformMatrix4fv(r->lighting_shader.wvp_matrix, 1, GL_FALSE, y_flip_ortho.f));

		DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE0));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.depth_texture));
		DE_GL_CALL(gl->Uniform1i(r->lighting_shader.depth_sampler, 0));

		DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE1));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.color_texture));
		DE_GL_CALL(gl->Uniform1i(r->lighting_shader.color_sampler, 1));

		DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE2));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.normal_texture));
		DE_GL_CALL(gl->Uniform1i(r->lighting_shader.normal_sampler, 2));

		DE_GL_CALL(glEnable(GL_BLEND));
		DE_GL_CALL(glBlendFunc(GL_ONE, GL_ONE));

		DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
		{
			if (node->type == DE_NODE_LIGHT)
			{
				de_vec3_t pos;
				de_light_t* light;
				float clr[4];

				light = &node->s.light;

				de_node_get_global_position(node, &pos);

				clr[0] = light->color.r / 255.0f;
				clr[1] = light->color.g / 255.0f;
				clr[2] = light->color.b / 255.0f;
				clr[3] = 1.0f;

				DE_GL_CALL(gl->Uniform3f(r->lighting_shader.light_position, pos.x, pos.y, pos.z));
				DE_GL_CALL(gl->Uniform1f(r->lighting_shader.light_radius, light->radius));
				DE_GL_CALL(gl->UniformMatrix4fv(r->lighting_shader.inv_view_proj_matrix, 1, GL_FALSE, camera->inv_view_proj.f));
				DE_GL_CALL(gl->Uniform4f(r->lighting_shader.light_color, clr[0], clr[1], clr[2], clr[3]));
				DE_GL_CALL(gl->Uniform1f(r->lighting_shader.light_cone_angle_cos, -1));
				DE_GL_CALL(gl->Uniform3f(r->lighting_shader.light_direction, 0, 0, 1));

				de_render_fullscreen_quad(r);
			}
		}

		/*DE_GL_CALL(glEnable(GL_CULL_FACE));*/
		DE_GL_CALL(glEnable(GL_DEPTH_TEST));
		DE_GL_CALL(glDisable(GL_BLEND));

		if (r->render_normals)
		{
			DE_GL_CALL(gl->UseProgram(r->flat_shader.program));

			de_set_viewport(&camera->viewport, de_engine->params.width, de_engine->params.height);

			/* Render each node */
			DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
			{
				de_mat4_t wvp_matrix;

				de_node_calculate_transforms(node);
				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &node->global_matrix);
				DE_GL_CALL(gl->UniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));

				if (node->type == DE_NODE_MESH)
				{
					de_render_mesh_normals(&node->s.mesh);
				}
			}
		}
	}

	DE_GL_CALL(glDisable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_BLEND));
	DE_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	DE_GL_CALL(glEnable(GL_STENCIL_TEST));
	/* Render GUI */
	{
		int index_bytes, vertex_bytes;
		de_gui_draw_list_t* draw_list = de_gui_render(&de_engine->gui);

		DE_GL_CALL(gl->UseProgram(r->gui_shader.program));
		DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE0));

		index_bytes = DE_ARRAY_SIZE_BYTES(draw_list->index_buffer);
		vertex_bytes = DE_ARRAY_SIZE_BYTES(draw_list->vertex_buffer);

		/* upload to gpu */
		DE_GL_CALL(gl->BindVertexArray(r->gui_render_buffers.vao));

		DE_GL_CALL(gl->BindBuffer(GL_ARRAY_BUFFER, r->gui_render_buffers.vbo));
		DE_GL_CALL(gl->BufferData(GL_ARRAY_BUFFER, vertex_bytes, draw_list->vertex_buffer.data, GL_STATIC_DRAW));

		DE_GL_CALL(gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->gui_render_buffers.ebo));
		DE_GL_CALL(gl->BufferData(GL_ELEMENT_ARRAY_BUFFER, index_bytes, draw_list->index_buffer.data, GL_STATIC_DRAW));

		DE_GL_CALL(gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)0));
		DE_GL_CALL(gl->EnableVertexAttribArray(0));

		DE_GL_CALL(gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, tex_coord)));
		DE_GL_CALL(gl->EnableVertexAttribArray(1));

		DE_GL_CALL(gl->VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, color)));
		DE_GL_CALL(gl->EnableVertexAttribArray(2));

		de_mat4_ortho(&ortho, 0, w, h, 0, -1, 1);
		DE_GL_CALL(gl->UniformMatrix4fv(r->gui_shader.wvp_matrix, 1, GL_FALSE, ortho.f));

		/* draw */
		for (i = 0; i < draw_list->commands.size; ++i)
		{
			de_gui_draw_command_t* cmd = draw_list->commands.data + i;
			size_t index_count = cmd->triangle_count * 3;
			if (cmd->type == DE_GUI_DRAW_COMMAND_TYPE_CLIP)
			{
				de_bool_t is_root_nesting = cmd->nesting == 1;
				if (is_root_nesting)
				{
					DE_GL_CALL(glClear(GL_STENCIL_BUFFER_BIT));
				}
				if (cmd->nesting != 0)
				{
					glEnable(GL_STENCIL_TEST);
				}
				else
				{
					glDisable(GL_STENCIL_TEST);
				}
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
				/* make sure that clipping rect will be drawn at previous nesting level only (clip to parent) */
				DE_GL_CALL(glStencilFunc(GL_EQUAL, cmd->nesting - 1, 0xFF));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
				/* draw clipping geometry to stencil buffer */
				DE_GL_CALL(glStencilMask(0xFF));
			}
			else
			{
				/* make sure to draw geometry only on clipping geometry with current nesting level */
				DE_GL_CALL(glStencilFunc(GL_EQUAL, cmd->nesting, 0xFF));
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, cmd->texture ? cmd->texture : r->white_dummy->id));
				/* do not draw geometry to stencil buffer */
				DE_GL_CALL(glStencilMask(0x00));
			}
			DE_GL_CALL(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (void*)(cmd->index_offset * sizeof(GLuint))));
		}

		gl->BindVertexArray(0);
	}
	DE_GL_CALL(glDisable(GL_STENCIL_TEST));
	DE_GL_CALL(glDisable(GL_BLEND));
	DE_GL_CALL(glEnable(GL_DEPTH_TEST));

	/* Unbind FBO textures */
	DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE0));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE1));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	DE_GL_CALL(gl->ActiveTexture(GL_TEXTURE2));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	de_engine_platform_swap_buffers(de_engine);

	if (r->frame_rate_limit > 0)
	{
		time_limit_ms = 1000 / r->frame_rate_limit;
		current_time_ms = (int)(1000 * de_time_get_seconds());
		de_sleep(time_limit_ms - (current_time_ms - last_time_ms));
		last_time_ms = (int)(1000 * de_time_get_seconds());
	}
}

