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
		de_fatal_error("OpenGL error \"%s\" occured at function:\n%s\nat line %d in file %s", desc, func, line, file);
	}
}

GLuint de_renderer_create_gpu_program(const char* vertexSource, const char* fragmentSource);
static void de_renderer_upload_surface(de_surface_t* s);

static void de_renderer_load_extensions()
{
#define GET_GL_EXT(type, func) func = (type)de_core_platform_get_proc_address(#func); \
								   if(!func) de_fatal_error("Unable to load "#func" function pointer");

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

	de_log("Extensions loaded!");
}

static void de_renderer_create_gbuffer(de_renderer_t* r, int width, int height)
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to construct G-Buffer FBO.");
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		de_fatal_error("Unable to initialize Stencil FBO.");
	}

	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

static void de_renderer_create_shaders(de_renderer_t* r)
{
	/* Built-in shaders */

	/**
	 * Flat (no lighting) shader.
	 **/
	static const char* de_flat_fs =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"

		"void main()"
		"{"
		"	FragColor = texture(diffuseTexture, texCoord);"
		"}";

	static const char* de_flat_vs =
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

	/**
	 * Ambient lighting shader
	 **/
	static const char* de_ambient_fs =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"
		"uniform vec4 ambientColor;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"

		"void main()"
		"{"
		"	FragColor = ambientColor * texture(diffuseTexture, texCoord);"
		"}";

	static const char* de_ambient_vs =
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

	/**
	 * Gui shader.
	 **/
	static const char* de_gui_fs =
		"#version 330 core\n"

		"uniform sampler2D diffuseTexture;"

		"out vec4 FragColor;"
		"in vec2 texCoord;"
		"in vec4 color;"

		"void main()"
		"{"
		"	FragColor = color * texture(diffuseTexture, texCoord);"
		"}";

	static const char* de_gui_vs =
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

	/**
	 * G-Buffer filling shader
	 **/
	static const char* de_gbuffer_fs =
		"#version 330 core\n"

		"layout(location = 0) out float outDepth;"
		"layout(location = 1) out vec4 outColor;"
		"layout(location = 2) out vec4 outNormal;"

		"uniform sampler2D diffuseTexture;"
		"uniform sampler2D normalTexture;"
		"uniform sampler2D specularTexture;"
		"uniform vec4 diffuseColor;"
		"uniform float selfEmittance;"

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

	static const char de_gbuffer_vs[] =
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

	/**
	 * Deferred lighting shader
	 **/
	static const char* const de_light_fs =
		"#version 330 core\n"
		"uniform sampler2D depthTexture;"
		"uniform sampler2D colorTexture;"
		"uniform sampler2D normalTexture;"

		"uniform vec3 lightPos;"
		"uniform float lightRadius;"
		"uniform vec4 lightColor;"
		"uniform vec3 lightDirection;"
		"uniform float coneAngleCos;"
		"uniform mat4 invViewProj;"
		"uniform vec3 cameraPosition;"

		"in vec2 texCoord;"
		"out vec4 FragColor;"

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
		"	worldPosition.xyz /= worldPosition.w;"
		"	vec3 lightVector = lightPos - worldPosition.xyz;"
		"	float d = min(length(lightVector), lightRadius);"
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
		"   FragColor = texture2D(colorTexture, texCoord);"
		"   FragColor.xyz += specular;"
		"	FragColor *= k * attenuation * lightColor;"
		"}";

	static const  char* de_light_vs =
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

	static const char* particle_vs =
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

	static const char* particle_fs =
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

	static const char* shadow_map_fs =
		"#version 330 core"
		"";

	/* Build flat shader */
	{
		de_flat_shader_t* s = &r->flat_shader;

		s->program = de_renderer_create_gpu_program(de_flat_vs, de_flat_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
	}

	/* Build ambient */
	{
		de_ambient_shader_t* s = &r->ambient_shader;

		s->program = de_renderer_create_gpu_program(de_ambient_vs, de_ambient_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
		s->ambient_color = glGetUniformLocation(s->program, "ambientColor");
	}

	/* Build gui shader */
	{
		de_gui_shader_t* s = &r->gui_shader;

		s->program = de_renderer_create_gpu_program(de_gui_vs, de_gui_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
	}

	/* Build GBuffer shader */
	{
		de_gbuffer_shader_t* s = &r->gbuffer_shader;

		s->program = de_renderer_create_gpu_program(de_gbuffer_vs, de_gbuffer_fs);
		s->wvp_matrix = glGetUniformLocation(s->program, "worldViewProjection");
		s->world_matrix = glGetUniformLocation(s->program, "worldMatrix");
		s->use_skeletal_animation = glGetUniformLocation(s->program, "useSkeletalAnimation");
		s->bone_matrices = glGetUniformLocation(s->program, "boneMatrices");

		s->diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
		s->normal_texture = glGetUniformLocation(s->program, "normalTexture");
		s->diffuse_color = glGetUniformLocation(s->program, "diffuseColor");
		s->self_emittance = glGetUniformLocation(s->program, "selfEmittance");
	}

	/* Build deferred lighting shader */
	{
		de_deferred_light_shader_t* s = &r->lighting_shader;

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
		s->camera_position = glGetUniformLocation(s->program, "cameraPosition");
	}

	/* Build particle system shader */
	{
		de_particle_system_shader_t* s = &r->particle_system_shader;
		s->program = de_renderer_create_gpu_program(particle_vs, particle_fs);

		s->vs.view_projection_matrix = glGetUniformLocation(s->program, "viewProjectionMatrix");
		s->vs.world_matrix = glGetUniformLocation(s->program, "worldMatrix");
		s->vs.camera_side_vector = glGetUniformLocation(s->program, "cameraSideVector");
		s->vs.camera_up_vector = glGetUniformLocation(s->program, "cameraUpVector");

		s->fs.diffuse_texture = glGetUniformLocation(s->program, "diffuseTexture");
		s->fs.depth_buffer_texture = glGetUniformLocation(s->program, "depthBufferTexture");
		s->fs.inv_screen_size = glGetUniformLocation(s->program, "invScreenSize");
		s->fs.proj_params = glGetUniformLocation(s->program, "projParams");
	}
}

de_renderer_t* de_renderer_init(de_core_t* core)
{
	de_renderer_t* r = DE_NEW(de_renderer_t);
	r->core = core;
	r->min_fps = 32768;
	de_log("GPU Vendor: %s", glGetString(GL_VENDOR));
	de_log("GPU: %s", glGetString(GL_RENDERER));
	de_log("OpenGL Version: %s", glGetString(GL_VERSION));
	de_log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	de_renderer_load_extensions();
	de_renderer_create_shaders(r);

#if VERBOSE_INIT
	int i, num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (i = 0; i < num_extensions; ++i) {
		de_log((char*)glGetStringi(GL_EXTENSIONS, i));
	}
#endif

	DE_GL_CALL(glEnable(GL_DEPTH_TEST));
	DE_GL_CALL(glEnable(GL_CULL_FACE));
	glCullFace(GL_BACK);

	de_renderer_create_gbuffer(r, core->params.video_mode.width, core->params.video_mode.height);

	/* Create fullscreen quad */
	{
		float w = (float)core->params.video_mode.width;
		float h = (float)core->params.video_mode.height;

		de_surface_shared_data_t* data = de_surface_shared_data_create(4, 6);

		int faces[] = { 0, 1, 2, 0, 2, 3 };
		memcpy(data->indices, faces, sizeof(faces));
		data->index_count = 6;

		data->positions[0] = (de_vec3_t) { 0, 0, 0 };
		data->positions[1] = (de_vec3_t) { w, 0, 0 };
		data->positions[2] = (de_vec3_t) { w, h, 0 };
		data->positions[3] = (de_vec3_t) { 0, h, 0 };
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
		de_resource_t* res = de_resource_create(core, NULL, DE_RESOURCE_TYPE_TEXTURE, DE_RESOURCE_FLAG_INTERNAL);
		de_resource_add_ref(res);
		r->white_dummy = de_resource_to_texture(res);
		de_texture_alloc_pixels(r->white_dummy, 1, 1, 4);
		pixel = (de_rgba8_t*)r->white_dummy->pixels;
		pixel->r = pixel->g = pixel->b = pixel->a = 255;
	}

	/* dummy normal map with (0,0,1) vector */
	{
		de_rgba8_t* pixel;
		de_resource_t* res = de_resource_create(core, NULL, DE_RESOURCE_TYPE_TEXTURE, DE_RESOURCE_FLAG_INTERNAL);
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

static void de_renderer_upload_texture(de_texture_t* texture)
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
#if 0
	GLfloat max_anisotropy;
	DE_GL_CALL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_anisotropy));
	DE_GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy));
#endif
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	texture->need_upload = false;
}

static void de_renderer_render_surface(de_renderer_t* r, de_surface_t* surf)
{
	DE_UNUSED(r);
	DE_GL_CALL(glBindVertexArray(surf->shared_data->vertex_array_object));
	DE_GL_CALL(glDrawElements(GL_TRIANGLES, surf->shared_data->index_count, GL_UNSIGNED_INT, NULL));
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

static void de_renderer_draw_mesh(de_renderer_t* r, de_mesh_t* mesh)
{	
	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		de_surface_t* surf = mesh->surfaces.data[i];
		const bool is_skinned = de_surface_is_skinned(surf);

		if (surf->need_upload) {
			de_renderer_upload_surface(surf);
		}

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

		DE_GL_CALL(glUniform1i(r->gbuffer_shader.use_skeletal_animation, is_skinned));
		if (is_skinned) {
			de_mat4_t matrices[DE_RENDERER_MAX_SKINNING_MATRICES] = { { { 0 } } };
			de_surface_get_skinning_matrices(surf, matrices, DE_RENDERER_MAX_SKINNING_MATRICES);

			glUniformMatrix4fv(r->gbuffer_shader.bone_matrices, DE_RENDERER_MAX_SKINNING_MATRICES, GL_FALSE, (const float*)&matrices[0]);
		}

		de_renderer_render_surface(r, surf);
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

static void de_renderer_set_viewport(const de_rectf_t* viewport, unsigned int window_width, unsigned int window_height)
{
	const int viewport_x = (int)(viewport->x * window_width);
	const int viewport_y = (int)(viewport->y * window_height);
	const int viewport_w = (int)(viewport->w * window_width);
	const int viewport_h = (int)(viewport->h * window_height);

	DE_GL_CALL(glViewport(viewport_x, viewport_y, viewport_w, viewport_h));
}

static void de_renderer_upload_textures(de_renderer_t* r)
{
	for (size_t i = 0; i < r->core->resources.size; ++i) {
		de_resource_t* res = r->core->resources.data[i];
		if (res->type == DE_RESOURCE_TYPE_TEXTURE) {
			de_texture_t* texture = de_resource_to_texture(res);
			if (texture->need_upload) {
				de_renderer_upload_texture(texture);
			}
		}
	}
}

static void de_renderer_draw_light_sphere(de_renderer_t* r, de_camera_t* camera, de_light_t* light)
{
	de_node_t* node = de_node_from_light(light);

	de_vec3_t pos;
	de_node_get_global_position(node, &pos);

	de_mat4_t world;
	de_mat4_translation(&world, &pos);

	de_mat4_t scale;
	float s = light->radius * 1.05f;
	de_mat4_scale(&scale, &(de_vec3_t){ s, s, s });
	de_mat4_mul(&world, &world, &scale);

	de_mat4_t wvp_matrix;
	de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, &world);

	DE_GL_CALL(glUniformMatrix4fv(r->lighting_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));
	de_renderer_render_surface(r, r->light_unit_sphere);
}

void de_renderer_render(de_renderer_t* r)
{
	de_core_t* core = r->core;
	static int last_time_ms;
	de_mat4_t y_flip_ortho, ortho;	
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	float w = (float)core->params.video_mode.width;
	float h = (float)core->params.video_mode.height;
	double frame_start_time = de_time_get_seconds();

	/* Upload textures first */
	de_renderer_upload_textures(r);

	de_mat4_t identity;
	de_mat4_identity(&identity);

	if (core->scenes.head) {
		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.fbo));
		DE_GL_CALL(glDrawBuffers(3, buffers));
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

	/* render each scene */
	DE_LINKED_LIST_FOR_EACH_T(de_scene_t*, scene, core->scenes)
	{
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

		de_renderer_set_viewport(&camera->viewport, core->params.video_mode.width, core->params.video_mode.height);

		/* Render each node */
		DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, scene->nodes)
		{
			if (node->global_visibility && node->type == DE_NODE_TYPE_MESH) {
				de_mesh_t* mesh = &node->s.mesh;
				const bool is_skinned = de_mesh_is_skinned(mesh);

				if (node->depth_hack != 0) {
					de_camera_enter_depth_hack(camera, node->depth_hack);
				}

				de_mat4_t wvp_matrix;
				de_mat4_mul(&wvp_matrix, &camera->view_projection_matrix, is_skinned ? &identity : &node->global_matrix);

				DE_GL_CALL(glUniformMatrix4fv(r->gbuffer_shader.wvp_matrix, 1, GL_FALSE, wvp_matrix.f));
				DE_GL_CALL(glUniformMatrix4fv(r->gbuffer_shader.world_matrix, 1, GL_FALSE, is_skinned ? identity.f : node->global_matrix.f));

				de_renderer_draw_mesh(r, mesh);

				if (camera->in_depth_hack_mode) {
					de_camera_leave_depth_hack(camera);
				}
			}
		}

		DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, r->gbuffer.opt_fbo));
		const GLenum lightBuffers[] = { GL_COLOR_ATTACHMENT0_EXT };
		DE_GL_CALL(glDrawBuffers(1, lightBuffers));		
		DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
				
		DE_GL_CALL(glDisable(GL_BLEND));
		DE_GL_CALL(glDepthMask(GL_FALSE));
		DE_GL_CALL(glDisable(GL_STENCIL_TEST));
		DE_GL_CALL(glStencilMask(0xFF));
		DE_GL_CALL(glDisable(GL_CULL_FACE));

		/* add ambient lighting */
		DE_GL_CALL(glUseProgram(r->ambient_shader.program));

		de_mat4_ortho(&y_flip_ortho, 0, w, h, 0, -1, 1);
		DE_GL_CALL(glUniformMatrix4fv(r->ambient_shader.wvp_matrix, 1, GL_FALSE, y_flip_ortho.f));

		DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
		DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.color_texture));
		DE_GL_CALL(glUniform1i(r->ambient_shader.diffuse_texture, 0));
		DE_GL_CALL(glUniform4f(r->ambient_shader.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f));

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

		DE_GL_CALL(glEnable(GL_STENCIL_TEST));

		DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, scene->nodes)
		{
			if (node->type == DE_NODE_TYPE_LIGHT) {
				de_light_t* light = &node->s.light;

				de_vec3_t pos;
				de_node_get_global_position(node, &pos);

				if (light->type == DE_LIGHT_TYPE_POINT || light->type == DE_LIGHT_TYPE_SPOT) {
					/* todo: spot light can be culled more accurately, but for now we cull it as
					   if it point light with some radius. */
					if (!de_frustum_sphere_intersection(&frustum, &pos, light->radius)) {
						continue;
					}
				}

				const float clr[] = {
					light->color.r / 255.0f,
					light->color.g / 255.0f,
					light->color.b / 255.0f,
					1.0f
				};

				de_vec3_t dir;
				de_node_get_up_vector(node, &dir);
				de_vec3_normalize(&dir, &dir);
				
				DE_GL_CALL(glColorMask(0, 0, 0, 0));

				DE_GL_CALL(glEnable(GL_CULL_FACE));

				DE_GL_CALL(glCullFace(GL_FRONT));
				DE_GL_CALL(glStencilFunc(GL_ALWAYS, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_INCR, GL_KEEP));
				de_renderer_draw_light_sphere(r, camera, light);

				DE_GL_CALL(glCullFace(GL_BACK));
				DE_GL_CALL(glStencilFunc(GL_ALWAYS, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_DECR, GL_KEEP));
				de_renderer_draw_light_sphere(r, camera, light);

				DE_GL_CALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
				DE_GL_CALL(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));

				DE_GL_CALL(glColorMask(1, 1, 1, 1));

				DE_GL_CALL(glDisable(GL_CULL_FACE));

				de_deferred_light_shader_t* s = &r->lighting_shader;
				DE_GL_CALL(glUniform3f(s->light_position, pos.x, pos.y, pos.z));
				DE_GL_CALL(glUniform1f(s->light_radius, light->radius));
				DE_GL_CALL(glUniformMatrix4fv(s->inv_view_proj_matrix, 1, GL_FALSE, camera->inv_view_proj.f));
				DE_GL_CALL(glUniform4f(s->light_color, clr[0], clr[1], clr[2], clr[3]));
				DE_GL_CALL(glUniform1f(s->light_cone_angle_cos, light->cone_angle_cos));
				DE_GL_CALL(glUniform3f(s->light_direction, dir.x, dir.y, dir.z));
				DE_GL_CALL(glUniformMatrix4fv(s->wvp_matrix, 1, GL_FALSE, y_flip_ortho.f));

				de_renderer_draw_fullscreen_quad(r);
			}
		}

		DE_GL_CALL(glDisable(GL_STENCIL_TEST));
		DE_GL_CALL(glDisable(GL_BLEND));

		DE_GL_CALL(glDepthMask(GL_TRUE));

		/* Debug render, renders node data even if node is invisible */
		if (r->render_normals) {
			DE_GL_CALL(glUseProgram(r->flat_shader.program));

			de_renderer_set_viewport(&camera->viewport, core->params.video_mode.width, core->params.video_mode.height);

			/* Render each node */
			DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, scene->nodes)
			{
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

			de_renderer_set_viewport(&camera->viewport, core->params.video_mode.width, core->params.video_mode.height);

			/* Render each node */
			DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, scene->nodes)
			{
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
	DE_LINKED_LIST_FOR_EACH_T(de_scene_t*, scene, core->scenes)
	{
		de_node_t* camera_node = scene->active_camera;

		if (!camera_node) {
			continue;
		}

		de_camera_t* camera = &camera_node->s.camera;

		de_mat4_t inv_view;
		de_mat4_inverse(&inv_view, &camera->view_matrix);

		de_vec3_t camera_up;
		de_mat4_up(&inv_view, &camera_up);

		de_vec3_t camera_side;
		de_mat4_side(&inv_view, &camera_side);

		de_vec3_t camera_position;
		de_node_get_global_position(camera_node, &camera_position);

		DE_LINKED_LIST_FOR_EACH_T(de_node_t*, node, scene->nodes)
		{
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
			de_particle_system_shader_t* shader = &r->particle_system_shader;
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

			const float inv_width = 1.0f / core->params.video_mode.width;
			const float inv_height = 1.0f / core->params.video_mode.height;
			DE_GL_CALL(glUniform2f(shader->fs.inv_screen_size, inv_width, inv_height));

			DE_GL_CALL(glUniform2f(shader->fs.proj_params, camera->z_far, camera->z_near));			 

			DE_GL_CALL(glDrawElements(GL_TRIANGLES, particle_system->indices.size, GL_UNSIGNED_INT, NULL));
		}
	}
	
	DE_GL_CALL(glDisable(GL_BLEND));
	DE_GL_CALL(glDepthMask(GL_TRUE));

	/* Render final frame from deferred lighting pass to back buffer */
	DE_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	DE_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	
	const de_rectf_t gui_viewport = { 0, 0, 1, 1 };
	de_renderer_set_viewport(&gui_viewport, core->params.video_mode.width, core->params.video_mode.height);

	DE_GL_CALL(glActiveTexture(GL_TEXTURE0));
	DE_GL_CALL(glUseProgram(r->flat_shader.program));
	DE_GL_CALL(glUniformMatrix4fv(r->flat_shader.wvp_matrix, 1, GL_FALSE, y_flip_ortho.f));
	DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, r->gbuffer.frame_texture));

	de_renderer_draw_fullscreen_quad(r);
	
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
		for (size_t i = 0; i < draw_list->commands.size; ++i) {
			de_gui_draw_command_t* cmd = draw_list->commands.data + i;
			size_t index_count = cmd->triangle_count * 3;
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
				DE_GL_CALL(glBindTexture(GL_TEXTURE_2D, cmd->texture ? (cmd->texture->id ? cmd->texture->id : r->white_dummy->id) : r->white_dummy->id));
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

	de_core_platform_swap_buffers(r->core);

	/* FPS limiter */
	if (r->frame_rate_limit > 0) {
		int time_limit_ms = 1000 / r->frame_rate_limit;
		int current_time_ms = (int)(1000 * de_time_get_seconds());
		de_sleep(time_limit_ms - (current_time_ms - last_time_ms));
		last_time_ms = (int)(1000 * de_time_get_seconds());
	}

	/* Measure mean, current and min FPS. */
	const double current_time = de_time_get_seconds();
	r->frame_time = 1000.0 * (current_time - frame_start_time);
	r->current_fps = (size_t)(1000.0 / r->frame_time);
	if (r->current_fps < r->min_fps) {
		r->min_fps = r->current_fps;
	}
	r->frame_time_accumulator += r->frame_time;
	++r->frame_time_measurements;
	if (current_time - r->time_last_fps_measured >= 1.0) {		
		r->mean_fps = (size_t)(1000.0 / (r->frame_time_accumulator / r->frame_time_measurements));
		r->time_last_fps_measured = current_time;
		r->frame_time_measurements = 0;
		r->frame_time_accumulator = 0;
	}
}

void de_renderer_set_framerate_limit(de_renderer_t* r, int limit)
{
	r->frame_rate_limit = limit;
}

size_t de_renderer_get_mean_fps(de_renderer_t* r)
{
	return r->mean_fps;
}

double de_render_get_frame_time(de_renderer_t* r)
{
	return r->frame_time;
}