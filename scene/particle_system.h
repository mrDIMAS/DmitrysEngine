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
	float size; /**< Size of particle. */
	float rotation; /**< Rotation of particle around axis to camera. */
	uint32_t color; /**< Packed RGBA color. */
} de_particle_vertex_t;

typedef struct de_particle_t {
	struct de_particle_system_emitter_t* owner;
	de_vec3_t position;
	de_vec3_t velocity;
	float size;
	bool alive;
	float size_modifier; /**< Modifier for size which will be added to size each update tick. */
	float lifetime; /**< Particle is alive if lifetime > 0 */
	float initial_lifetime;
	float rotation_speed;
	float rotation;	
	de_color_t color;
	float sqr_distance_to_camera; /**< Non-serializable. Valid only for 1 frame. */
} de_particle_t;

typedef enum de_particle_system_emitter_type_t {
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_POINT,
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX,
	DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE
} de_particle_system_emitter_type_t;

typedef struct de_particle_system_box_emitter_t {
	float half_width;
	float half_height;
	float half_depth;
} de_particle_system_box_emitter_t;

typedef struct de_particle_system_sphere_emitter_t {
	float radius; /**< Radius of a sphere around of center of emitter */
} de_particle_system_sphere_emitter_t;

typedef struct de_particle_system_emitter_t {
	struct de_particle_system_t* particle_system; /** Owning particle system */
	de_particle_system_emitter_type_t type; /**< Actual type of emitter. */
	de_vec3_t position; /**< Offset from center of particle system. */
	int32_t particle_spawn_rate; /**< Particle spawn rate in unit-per-second. If < 0, spawns 'max_particles', spawns nothing if 'max_particles' < 0 */
	int32_t max_particles; /**< Maximum amount of particles emitter can emit. Unlimited if < 0 */
	float min_lifetime, max_lifetime; /**< Range of initial lifetime of a particle */
	float min_size, max_size; /**< Range of initial size of a particle */
	float min_size_modifier, max_size_modifier; /**< Range of initial size modifier of a particle */
	float min_x_velocity, max_x_velocity; /**< Range of initial X-component of velocity for a particle */
	float min_y_velocity, max_y_velocity; /**< Range of initial Y-component of velocity for a particle */
	float min_z_velocity, max_z_velocity; /**< Range of initial Z-component of velocity for a particle */	
	float min_rotation_speed, max_rotation_speed; /**< Range of initial rotation speed for a particle */
	float min_rotation, max_rotation; /**< Range of initial rotation for a particle */
	/* Private */
	int32_t alive_particles; /**< Count of particle already spawned by this emitter. */
	float time; /**< Time accumulator for update purposes. */
	union {
		de_particle_system_box_emitter_t box;
		de_particle_system_sphere_emitter_t sphere;
	} s;
} de_particle_system_emitter_t;

typedef struct de_particle_system_t {
	de_vec3_t acceleration; /**< Acceleration for each particle in m/s^2. For gravity use (0.0, -9.81, 0.0), default is (0.0) */
	DE_ARRAY_DECLARE(de_particle_t, particles);
	DE_ARRAY_DECLARE(int, free_particles);
	DE_ARRAY_DECLARE(de_particle_system_emitter_t*, emitters);
	DE_ARRAY_DECLARE(de_particle_vertex_t, vertices);
	DE_ARRAY_DECLARE(int, indices);
	DE_ARRAY_DECLARE(de_particle_t*, sorted_particles); /**< Array of pointers to alive particles sorted in back-to-front order, valid only 1 frame! */
	de_color_gradient_t color_gradient_over_lifetime;
	de_texture_t* texture;
	unsigned int vertex_buffer;
	unsigned int index_buffer;
	unsigned int vertex_array_object;
} de_particle_system_t;

/**
 * @brief Internal. Returns vtable for particle system.
 */
struct de_node_dispatch_table_t* de_particle_system_get_dispatch_table();

/**
 * @brief Update emitters and particles.
 */
void de_particle_system_update(de_particle_system_t* particle_system, float dt);

/**
 * @brief Internal. Generates vertices and indices for particles for rendering.
 */
void de_particle_system_generate_vertices(de_particle_system_t* particle_system, const de_vec3_t* camera_pos);

/**
 * @brief Sets mask texture for particles. Only Red channel will be used as alpha value.
 */
void de_particle_system_set_texture(de_particle_system_t* particle_system, de_texture_t* texture);

/**
 * @brief Creates and adds new emitter to particle system.
 */
de_particle_system_emitter_t* de_particle_system_emitter_create(de_particle_system_t* particle_system, de_particle_system_emitter_type_t type);

/**
 * @brief Returns pointer to color gradient which can be used to
 * setup color behaviour of particles during lifetime.
 */
de_color_gradient_t* de_particle_system_get_color_gradient_over_lifetime(de_particle_system_t* particle_system);