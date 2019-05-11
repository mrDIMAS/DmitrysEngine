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

typedef struct de_particle_vertex_t {
	de_vec3_t position;	/**< Position of particle. */
	de_vec2_t tex_coord; 
	de_vec2_t corner; /**< Offset for corners: left_top(-1,1), right_top(1,1) and etc. */
	float size; /**< Size of particle. */
	uint32_t color; /**< Packed RGBA color. */
} de_particle_vertex_t;

typedef struct de_particle_t {
	de_vec3_t position;
	de_vec3_t velocity;
	float size;	
	float lifetime; /**< Particle is alive if lifetime > 0 */
	de_color_t color;
} de_particle_t;

typedef enum de_particle_system_emitter_type_t {
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_POINT,
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX,
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE,
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_FORCE_SIZE = INT_MAX,
} de_particle_system_emitter_type_t;

DE_STATIC_ASSERT(sizeof(de_particle_system_emitter_type_t) == sizeof(int32_t), invalid_particle_system_emitter_type);

typedef struct de_particle_system_box_emitter_t {
	float width;
	float height;
	float depth;
} de_particle_system_box_emitter_t;

typedef struct de_particle_system_sphere_emitter_t {
	float radius; /**< Radius of a sphere around of center of emitter */
} de_particle_system_sphere_emitter_t;

typedef struct de_particle_system_emitter_t {
	struct de_particle_system_t* particle_system;
	de_particle_system_emitter_type_t type; /**< Actual type of emitter. */
	de_vec3_t position; /**< Offset from center of particle system. */
	bool auto_resurrect_particles; /**< True if emitter should constantly resurrect dead particles. */
	int initial_particle_count; /**< Initial particle count at the moment of creation of emitter. */
	int particle_spawn_rate; /**< Particle spawn rate in unit-per-second. */	
	int max_particles; /**< Maximum amount of particles emitter can emit. */
	float time;
	union {
		de_particle_system_box_emitter_t box;
		de_particle_system_sphere_emitter_t sphere;
	} s;
} de_particle_system_emitter_t;

typedef struct de_particle_system_t {
	DE_ARRAY_DECLARE(de_particle_t, particles);
	DE_ARRAY_DECLARE(de_particle_vertex_t, vertices);
	DE_ARRAY_DECLARE(int, indices);	
	DE_ARRAY_DECLARE(de_particle_system_emitter_t*, emitters);
	de_texture_t* texture;
	unsigned int vertex_buffer;
	unsigned int index_buffer;
	unsigned int vertex_array_object;
} de_particle_system_t;

struct de_node_dispatch_table_t* de_particle_system_get_dispatch_table();

void de_particle_system_create_particles(de_particle_system_t* particle_system, size_t n);

/**
 * @brief Update emitters and particles. 
 */
void de_particle_system_update(de_particle_system_t* particle_system, float dt);

void de_particle_system_generate_vertices(de_particle_system_t* particle_system);

void de_particle_system_set_texture(de_particle_system_t* particle_system, de_texture_t* texture);

void de_particle_system_add_emitter(de_particle_system_t* particle_system, de_particle_system_emitter_t* emitter);