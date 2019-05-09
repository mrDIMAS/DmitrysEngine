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
	de_vec2_t corner; /**< Offset for corners: left_top(-1,-1), right_top(1,-1) and etc. */
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

typedef struct de_particle_system_t {
	DE_ARRAY_DECLARE(de_particle_t, particles);
	DE_ARRAY_DECLARE(de_particle_vertex_t, vertices);
	DE_ARRAY_DECLARE(int, indices);	
	de_texture_t* texture;
	unsigned int vertex_buffer;
	unsigned int index_buffer;
	unsigned int vertex_array_object;
} de_particle_system_t;

struct de_node_dispatch_table_t* de_particle_system_get_dispatch_table();

void de_particle_system_create_particles(de_particle_system_t* particle_system, size_t n);

void de_particle_system_update(de_particle_system_t* particle_system, float dt);

void de_particle_system_generate_vertices(de_particle_system_t* particle_system);

void de_particle_system_set_texture(de_particle_system_t* particle_system, de_texture_t* texture);