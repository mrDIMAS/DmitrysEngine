#define DE_RENDERER_MAX_SKINNING_MATRICES 60
#define DE_STRINGIZE_(x) #x
#define DE_STRINGIZE(x) DE_STRINGIZE_(x)

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
"layout(location = 0) in vec3 vertexPosition;"
"layout(location = 1) in vec2 vertexTexCoord;"
"layout(location = 2) in vec3 vertexNormal;"
"layout(location = 3) in vec4 boneWeights;"
"layout(location = 4) in vec4 boneIndices;"
"uniform mat4 worldMatrix;"
"uniform mat4 worldViewProjection;"
"uniform bool useSkeletalAnimation;"
"uniform mat4 boneMatrices[" DE_STRINGIZE(DE_RENDERER_MAX_SKINNING_MATRICES) "];"
"out vec4 position;"
"out vec3 normal;"
"out vec2 texCoord;"
"void main()"
"{"
"   vec4 localPosition = vec4(0);"
"   vec3 localNormal = vec3(0);"
"   if(useSkeletalAnimation)"
"   {"
"       vec4 vertex = vec4(vertexPosition, 1.0);"

"       localPosition += boneMatrices[int(boneIndices.x)] * vertex * boneWeights.x;"
"       localPosition += boneMatrices[int(boneIndices.y)] * vertex * boneWeights.z;"
"       localPosition += boneMatrices[int(boneIndices.z)] * vertex * boneWeights.y;"
"       localPosition += boneMatrices[int(boneIndices.w)] * vertex * boneWeights.w;"

"       localNormal += mat3(boneMatrices[int(boneIndices.x)]) * vertexNormal * boneWeights.x;"
"       localNormal += mat3(boneMatrices[int(boneIndices.y)]) * vertexNormal * boneWeights.z;"
"       localNormal += mat3(boneMatrices[int(boneIndices.z)]) * vertexNormal * boneWeights.y;"
"       localNormal += mat3(boneMatrices[int(boneIndices.w)]) * vertexNormal * boneWeights.w;"
"   }"
"   else"
"   {"
"       localPosition = vec4(vertexPosition, 1.0);"
"       localNormal = vertexNormal;"
"   }"
"	gl_Position = worldViewProjection * localPosition;"
"   normal = mat3(worldMatrix) * localNormal;"
"	texCoord = vertexTexCoord;"
"	position = gl_Position;"
"}";

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
#define GET_GL_EXT(type, func) func = (type)de_engine_platform_get_proc_address(#func); \
								   if(!func) de_error("Unable to load "#func" function pointer");

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
	GET_GL_EXT(PFNGLUNIFORM1FPROC, glUniform1f);

	GET_GL_EXT(PFNGLGENBUFFERSPROC, glGenBuffers);
	GET_GL_EXT(PFNGLBINDBUFFERPROC, glBindBuffer);
	GET_GL_EXT(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	GET_GL_EXT(PFNGLBUFFERDATAPROC, glBufferData);

	GET_GL_EXT(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
	GET_GL_EXT(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
	GET_GL_EXT(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
	GET_GL_EXT(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
	GET_GL_EXT(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);

#ifdef _WIN32	
	/* Windows does support ony OpenGL 1.1 and we must obtain these pointers */
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

	de_log("Extensions loaded!");
}

/*=======================================================================================*/
static void de_create_gbuffer(de_renderer_t* r, int width, int height)
{
	de_gbuffer_t * gbuf = &r->gbuffer;

	DE_GL_CALL(glGenFramebuffers(1, &gbuf->fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gbuf->fbo));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuf->depth_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuf->depth_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuf->depth_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuf->color_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuf->color_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, gbuf->color_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuf->normal_rt));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuf->normal_rt));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, gbuf->normal_rt));

	DE_GL_CALL(glGenRenderbuffers(1, &gbuf->depth_buffer));
	DE_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, gbuf->depth_buffer));
	DE_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuf->depth_buffer));

	DE_GL_CALL(glGenTextures(1, &gbuf->depth_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->depth_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_BGRA, GL_FLOAT, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuf->depth_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuf->color_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->color_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuf->color_texture, 0));

	DE_GL_CALL(glGenTextures(1, &gbuf->normal_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->normal_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuf->normal_texture, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		de_error("Unable to construct G-Buffer FBO.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	/* Create another framebuffer for stencil optimizations */
	DE_GL_CALL(glGenFramebuffers(1, &gbuf->opt_fbo));
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gbuf->opt_fbo));

	DE_GL_CALL(glGenTextures(1, &gbuf->frame_texture));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, gbuf->frame_texture));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	DE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));

	DE_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuf->frame_texture, 0));

	DE_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuf->depth_buffer));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		de_error("Unable to initialize Stencil FBO.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

/*=======================================================================================*/
static void de_create_builtin_shaders(de_renderer_t* r)
{
	/* Flat shader */
	{
		de_flag_shader_t* s = &r->flat_shader;

		s->program = de_renderer_create_gpu_program(de_flat_vs, de_flat_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
	}

	/* gui shader */
	{
		de_gui_shader_t* s = &r->gui_shader;

		s->program = de_renderer_create_gpu_program(de_gui_vs, de_gui_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
	}

	/* GBuffer shader */
	{
		de_gbuffer_shader_t* s = &r->gbuffer_shader;

		s->program = de_renderer_create_gpu_program(de_gbuffer_vs, de_gbuffer_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->world_matrix = glGetUniformLocation(s->program, "worldMatrix");
		s->use_skeletal_animation = glGetUniformLocation(s->program, "useSkeletalAnimation");
		s->bone_matrices = glGetUniformLocation(s->program, "boneMatrices");

		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
		s->diffuse_color = glGetUniformLocation(s->program, "diffuseColor");
		s->self_emittance = glGetUniformLocation(s->program, "selfEmittance");
	}

	/* Lighting Shader */
	{
		de_gbuffer_light_shader_t* s = &r->lighting_shader;

		s->program = de_renderer_create_gpu_program(de_light_vs, de_light_fs);

		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");

		s->depth_sampler = glGetUniformLocation(s->program, "depthTexture");
		s->color_sampler = glGetUniformLocation(s->program, "colorTexture");
		s->normal_sampler = glGetUniformLocation(s->program, "normalTexture");

		s->light_position = glGetUniformLocation(s->program, "lightPos");
		s->light_radius = glGetUniformLocation(s->program, "lightRadius");
		s->light_color = glGetUniformLocation(s->program, "lightColor");
		s->light_direction = glGetUniformLocation(s->program, "lightDirection");
		s->light_cone_angle_cos = glGetUniformLocation(s->program, "coneAngleCos");
		s->inv_view_proj_matrix = glGetUniformLocation(s->program, "invViewProj");
	}
}

/*=======================================================================================*/
de_renderer_t* de_renderer_init(de_core_t* core)
{
#if VERBOSE_INIT
	int i, num_extensions;
#endif
	de_renderer_t* r = DE_NEW(de_renderer_t);
	r->core = core;

	de_log("GPU Vendor: %s", glGetString(GL_VENDOR));
	de_log("GPU: %s", glGetString(GL_RENDERER));
	de_log("OpenGL Version: %s", glGetString(GL_VERSION));
	de_log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	de_renderer_load_extensions();
	de_create_builtin_shaders(r);

#if VERBOSE_INIT
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (i = 0; i < num_extensions; ++i)
	{
		de_log((char*)glGetStringi(GL_EXTENSIONS, i));
	}
#endif

	DE_GL_CALL(glEnable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_CULL_FACE));
	glCullFace(GL_BACK);

	de_create_gbuffer(r, core->params.width, core->params.height);

	/* Create fullscreen quad */
	{
		float w = (float)core->params.width;
		float h = (float)core->params.height;

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

	glGenVertexArrays(1, &r->gui_render_buffers.vao);
	glGenBuffers(1, &r->gui_render_buffers.vbo);
	glGenBuffers(1, &r->gui_render_buffers.ebo);

	r->test_surface = de_renderer_create_surface(r);
	{
		de_rgba8_t* pixel;
		r->white_dummy = de_renderer_create_texture(r, 1, 1, 4);
		pixel = (de_rgba8_t*)r->white_dummy->pixels;
		pixel->r = pixel->g = pixel->b = pixel->a = 255;
	}

	return r;
}

/*=======================================================================================*/
void de_renderer_free(de_renderer_t* r)
{
	de_renderer_free_surface(r->quad);
	de_renderer_free_surface(r->test_surface);
	de_texture_release(r->white_dummy);

	de_free(r);
}

/*=======================================================================================*/
static void de_render_surface_normals(de_renderer_t* r, de_surface_t* surface)
{
	size_t i;

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

	DE_GL_CALL(glBindVertexArray(r->test_surface->vao));
	DE_GL_CALL(glDrawElements(GL_LINES, r->test_surface->indices.size, GL_UNSIGNED_INT, NULL));
}

/*=======================================================================================*/
GLuint de_renderer_create_shader(GLenum type, const char* source)
{
	GLint compiled;
	GLuint shader;

	shader = glCreateShader(type);
	DE_GL_CALL(glShaderSource(shader, 1, &source, NULL));

	DE_GL_CALL(glCompileShader(shader));

	DE_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));

	if (!compiled)
	{
		static char buffer[2048];
		DE_GL_CALL(glGetShaderInfoLog(shader, 2048, NULL, buffer));
		de_error(buffer);
	}

	return shader;
}

/*=======================================================================================*/
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

	if (!linked)
	{
		static char buffer[2048];
		DE_GL_CALL(glGetProgramInfoLog(program, 2048, NULL, buffer));
		de_error(buffer);
	}

	return program;
}

/*=======================================================================================*/
static void de_renderer_upload_surface(de_surface_t* s)
{
	glBindVertexArray(s->vao);

	glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
	glBufferData(GL_ARRAY_BUFFER, DE_ARRAY_SIZE_BYTES(s->vertices), s->vertices.data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, DE_ARRAY_SIZE_BYTES(s->indices), s->indices.data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, tex_coord));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, bone_weights));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(de_vertex_t), (void*)offsetof(de_vertex_t, bones));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);

	s->need_upload = DE_FALSE;
}

/*=======================================================================================*/
de_surface_t* de_renderer_create_surface(de_renderer_t* r)
{
	de_surface_t* surf = DE_NEW(de_surface_t);

	surf->renderer = r;
	surf->need_upload = DE_TRUE;

	/* Create gpu-side buffers */
	glGenVertexArrays(1, &surf->vao);
	glGenBuffers(1, &surf->vbo);
	glGenBuffers(1, &surf->ebo);

	return surf;
}

/*=======================================================================================*/
void de_renderer_free_surface(de_surface_t* surf)
{
	/* Unref texture */
	de_texture_release(surf->texture);

	/* Delete gpu buffers */
	glDeleteBuffers(1, &surf->vbo);
	glDeleteBuffers(1, &surf->ebo);
	glDeleteVertexArrays(1, &surf->vao);

	/* Delete buffers */
	DE_ARRAY_FREE(surf->indices);
	DE_ARRAY_FREE(surf->vertices);
	DE_ARRAY_FREE(surf->skinning_data);
	DE_ARRAY_FREE(surf->bones);

	de_free(surf);
}

/*=======================================================================================*/
de_texture_t* de_renderer_request_texture(de_renderer_t* r, const char* file)
{
	de_texture_t* tex;
	de_image_t img = { 0 };

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
	tex->renderer = r;
	tex->width = img.width;
	tex->height = img.height;
	tex->byte_per_pixel = img.byte_per_pixel;
	tex->name = de_str_copy(file);
	tex->depth = 0;
	tex->id = 0;
	tex->type = DE_TEXTURE_TYPE_2D;
	tex->pixels = img.data;
	tex->ref_count = 0;
	tex->need_upload = 1;

	DE_LINKED_LIST_APPEND(r->textures, tex);

	de_log("Texture %s is loaded!", file);

	return tex;
}

/*=======================================================================================*/
de_texture_t* de_renderer_create_texture(de_renderer_t* r, size_t w, size_t h, size_t byte_per_pixel)
{
	de_texture_t* tex;

	tex = DE_NEW(de_texture_t);
	tex->renderer = r;
	tex->width = w;
	tex->height = h;
	tex->byte_per_pixel = byte_per_pixel;
	tex->name = NULL;
	tex->depth = 0;
	tex->id = 0;
	tex->type = DE_TEXTURE_TYPE_2D;
	tex->pixels = (char*)de_calloc(w * h * byte_per_pixel, 1);
	tex->ref_count = 1;
	tex->need_upload = 1;

	DE_LINKED_LIST_APPEND(r->textures, tex);
	return tex;
}

/*=======================================================================================*/
static void de_renderer_remove_texture(de_renderer_t* r, de_texture_t* tex)
{
	glDeleteTextures(1, &tex->id);
	DE_LINKED_LIST_REMOVE(r->textures, tex);
}

/*=======================================================================================*/
static void de_upload_texture(de_texture_t* texture)
{
	GLint internalFormat;
	GLint format;
	GLfloat max_anisotropy;

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
	DE_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	DE_GL_CALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy));
	DE_GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	texture->need_upload = DE_FALSE;
}

/*=======================================================================================*/
static void de_render_fullscreen_quad(de_renderer_t* r)
{
	DE_GL_CALL(glBindVertexArray(r->quad->vao));
	DE_GL_CALL(glDrawElements(GL_TRIANGLES, r->quad->indices.size, GL_UNSIGNED_INT, NULL));
}

/*=======================================================================================*/
static void de_render_mesh(de_renderer_t* r, de_mesh_t* mesh)
{
	size_t i, j;

	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_surface_t* surf = mesh->surfaces.data[i];
		de_bool_t is_skinned = de_surface_is_skinned(surf);

		if (surf->need_upload)
		{
			de_renderer_upload_surface(surf);
		}

		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
		if (surf->texture)
		{
			DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, surf->texture->id));
		}
		else
		{
			DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->white_dummy->id));
		}

		DE_GL_CALL(glUniform1i(r->gbuffer_shader.use_skeletal_animation, is_skinned));
		if (is_skinned)
		{
			de_mat4_t matrices[DE_RENDERER_MAX_SKINNING_MATRICES] = { 0 };
			de_surface_get_skinning_matrices(surf, &mesh->parent_node->local_matrix, matrices, DE_RENDERER_MAX_SKINNING_MATRICES);
			
			glUniformMatrix4fv(r->gbuffer_shader.bone_matrices, DE_RENDERER_MAX_SKINNING_MATRICES, GL_FALSE, (const float*)&matrices[0]);
		}

		DE_GL_CALL(glBindVertexArray(surf->vao));
		DE_GL_CALL(glDrawElements(GL_TRIANGLES, surf->indices.size, GL_UNSIGNED_INT, NULL));
	}
}

/*=======================================================================================*/
static void de_render_mesh_normals(de_renderer_t* r, de_mesh_t* mesh)
{
	size_t i;
	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_render_surface_normals(r, mesh->surfaces.data[i]);
	}
}

/*=======================================================================================*/
static void de_set_viewport(const de_rectf_t* viewport, unsigned int window_width, unsigned int window_height)
{
	int viewport_x = (int)(viewport->x * window_width);
	int viewport_y = (int)(viewport->y * window_height);
	int viewport_w = (int)(viewport->w * window_width);
	int viewport_h = (int)(viewport->h * window_height);

	DE_GL_CALL(glViewport(viewport_x, viewport_y, viewport_w, viewport_h));
}

/*=======================================================================================*/
static void de_upload_textures(de_renderer_t* r)
{
	de_texture_t* texture;

	DE_LINKED_LIST_FOR_EACH(r->textures, texture)
	{
		if (texture->need_upload)
		{
			de_upload_texture(texture);
		}
	}
}

/*=======================================================================================*/
void de_renderer_render(de_renderer_t* r)
{
	de_core_t* core = r->core;
	static int last_time_ms;
	int current_time_ms;
	int time_limit_ms;
	size_t i;
	de_scene_t* scene;
	de_mat4_t y_flip_ortho, ortho;
	de_mat4_t identity;
	de_rectf_t gui_viewport = { 0, 0, 1, 1 };
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	float w = (float)core->params.width;
	float h = (float)core->params.height;
	float frame_start_time = de_time_get_seconds();

	/* Upload textures first */
	de_upload_textures(r);

	de_mat4_identity(&identity);

	if (core->scenes.head)
	{
		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.fbo));
		DE_GL_CALL(glDrawBuffers(3, buffers));
	}
	else
	{
		/* bind back buffer if no scenes */
		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	glClearColor(0.1f, 0.1f, 0.1f, 0);
	DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	glClearColor(0.0f, 0.0f, 0.0f, 0);

	DE_GL_CALL(glUseProgram(r->gbuffer_shader.program));
	DE_GL_CALL(glUniform1i(r->gbuffer_shader.diffuse_texture, 0));
	DE_GL_CALL(glEnable(GL_CULL_FACE));

	/* render each scene */
	DE_LINKED_LIST_FOR_EACH(core->scenes, scene)
	{
		de_node_t* node;
		de_camera_t* camera;

		camera = &scene->active_camera->s.camera;

		de_camera_update(camera);

		de_set_viewport(&camera->viewport, core->params.width, core->params.height);

		/* Render each node */
		DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
		{
			if (node->type == DE_NODE_TYPE_MESH)
			{
				de_mat4_t wvp_matrix;
				de_bool_t is_skinned;
				de_mesh_t* mesh;

				mesh = &node->s.mesh;

				is_skinned = de_mesh_is_skinned(mesh);
								
				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, is_skinned ? &identity : &node->global_matrix);

				DE_GL_CALL(glUniformMatrix4fv(r->gbuffer_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));
				DE_GL_CALL(glUniformMatrix4fv(r->gbuffer_shader.world_matrix, 1, GL_FALSE, is_skinned ? &identity : node->global_matrix.f));

				de_render_mesh(r, mesh);
			}
		}

		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		DE_GL_CALL(glUseProgram(r->lighting_shader.program));

		DE_GL_CALL(glDisable(GL_CULL_FACE));
		DE_GL_CALL(glDisable(GL_DEPTH_TEST));

		de_mat4_ortho(&y_flip_ortho, 0, w, 0, h, -1, 1);
		DE_GL_CALL(glUniformMatrix4fv(r->lighting_shader.wvp_matrix, 1, GL_FALSE, y_flip_ortho.f));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.depth_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.depth_sampler, 0));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE1));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.color_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.color_sampler, 1));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE2));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.normal_texture));
		DE_GL_CALL(glUniform1i(r->lighting_shader.normal_sampler, 2));

		DE_GL_CALL(glEnable(GL_BLEND));
		DE_GL_CALL(glBlendFunc(GL_ONE, GL_ONE));

		DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
		{
			if (node->type == DE_NODE_TYPE_LIGHT)
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

				DE_GL_CALL(glUniform3f(r->lighting_shader.light_position, pos.x, pos.y, pos.z));
				DE_GL_CALL(glUniform1f(r->lighting_shader.light_radius, light->radius));
				DE_GL_CALL(glUniformMatrix4fv(r->lighting_shader.inv_view_proj_matrix, 1, GL_FALSE, camera->inv_view_proj.f));
				DE_GL_CALL(glUniform4f(r->lighting_shader.light_color, clr[0], clr[1], clr[2], clr[3]));
				DE_GL_CALL(glUniform1f(r->lighting_shader.light_cone_angle_cos, -1));
				DE_GL_CALL(glUniform3f(r->lighting_shader.light_direction, 0, 0, 1));

				de_render_fullscreen_quad(r);
			}
		}

		DE_GL_CALL(glEnable(GL_DEPTH_TEST));
		DE_GL_CALL(glDisable(GL_BLEND));

		if (r->render_normals)
		{
			DE_GL_CALL(glUseProgram(r->flat_shader.program));

			de_set_viewport(&camera->viewport, core->params.width, core->params.height);

			/* Render each node */
			DE_LINKED_LIST_FOR_EACH(scene->nodes, node)
			{
				de_mat4_t wvp_matrix;

				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &node->global_matrix);
				DE_GL_CALL(glUniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));

				if (node->type == DE_NODE_TYPE_MESH)
				{
					de_render_mesh_normals(r, &node->s.mesh);
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

	de_set_viewport(&gui_viewport, core->params.width, core->params.height);

	DE_GL_CALL(glDisable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_BLEND));
	DE_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	DE_GL_CALL(glEnable(GL_STENCIL_TEST));
	DE_GL_CALL(glDisable(GL_CULL_FACE));
	/* Render GUI */
	{
		int index_bytes, vertex_bytes;
		de_gui_draw_list_t* draw_list = de_gui_render(core->gui);

		DE_GL_CALL(glUseProgram(r->gui_shader.program));
		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));

		index_bytes = DE_ARRAY_SIZE_BYTES(draw_list->index_buffer);
		vertex_bytes = DE_ARRAY_SIZE_BYTES(draw_list->vertex_buffer);

		/* upload to gpu */
		DE_GL_CALL(glBindVertexArray(r->gui_render_buffers.vao));

		DE_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, r->gui_render_buffers.vbo));
		DE_GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertex_bytes, draw_list->vertex_buffer.data, GL_STATIC_DRAW));

		DE_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->gui_render_buffers.ebo));
		DE_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_bytes, draw_list->index_buffer.data, GL_STATIC_DRAW));

		DE_GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)0));
		DE_GL_CALL(glEnableVertexAttribArray(0));

		DE_GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, tex_coord)));
		DE_GL_CALL(glEnableVertexAttribArray(1));

		DE_GL_CALL(glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(de_gui_vertex_t), (void*)offsetof(de_gui_vertex_t, color)));
		DE_GL_CALL(glEnableVertexAttribArray(2));

		de_mat4_ortho(&ortho, 0, w, h, 0, -1, 1);
		DE_GL_CALL(glUniformMatrix4fv(r->gui_shader.wvp_matrix, 1, GL_FALSE, ortho.f));

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

		glBindVertexArray(0);
	}
	DE_GL_CALL(glDisable(GL_STENCIL_TEST));
	DE_GL_CALL(glDisable(GL_BLEND));
	DE_GL_CALL(glEnable(GL_DEPTH_TEST));

	r->frame_time = 1000.0f * (de_time_get_seconds() - frame_start_time);
	r->frames_per_second = (size_t)(1000.0f / r->frame_time);

	de_engine_platform_swap_buffers(r->core);

	if (r->frame_rate_limit > 0)
	{
		time_limit_ms = 1000 / r->frame_rate_limit;
		current_time_ms = (int)(1000 * de_time_get_seconds());
		de_sleep(time_limit_ms - (current_time_ms - last_time_ms));
		last_time_ms = (int)(1000 * de_time_get_seconds());
	}
}

/*=======================================================================================*/
void de_renderer_set_framerate_limit(de_renderer_t* r, int limit)
{
	r->frame_rate_limit = limit;
}

/*=======================================================================================*/
size_t de_renderer_get_fps(de_renderer_t* r)
{
	return r->frames_per_second;
}

/*=======================================================================================*/
float de_render_get_frame_time(de_renderer_t* r)
{
	return r->frame_time;
}