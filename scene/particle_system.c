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

static bool de_particle_system_visit(de_object_visitor_t* visitor, de_node_t* node);
static bool de_particle_system_emitter_visit(de_object_visitor_t* visitor, de_particle_system_emitter_t* emitter);

static void de_particle_system_init(de_node_t* node)
{
	DE_UNUSED(node);
}

static void de_particle_system_emitter_free(de_particle_system_emitter_t* emitter)
{
	de_free(emitter);
}

static void de_particle_system_free(de_node_t* node)
{
	de_particle_system_t* particle_system = &node->s.particle_system;
	for (size_t i = 0; i < particle_system->emitters.size; ++i) {
		de_particle_system_emitter_free(particle_system->emitters.data[i]);
	}
	DE_ARRAY_FREE(particle_system->emitters);
	DE_ARRAY_FREE(particle_system->particles);
	DE_ARRAY_FREE(particle_system->vertices);
	DE_ARRAY_FREE(particle_system->indices);
	DE_ARRAY_FREE(particle_system->free_particles);
	de_color_gradient_free(&particle_system->color_gradient_over_lifetime);
	if (particle_system->texture) {
		de_resource_release(de_resource_from_texture(particle_system->texture));
	}
}

static bool de_particle_visit(de_object_visitor_t* visitor, de_particle_t* particle)
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Owner", &particle->owner, de_particle_system_emitter_visit);
	result &= de_object_visitor_visit_vec3(visitor, "Position", &particle->position);
	result &= de_object_visitor_visit_vec3(visitor, "Velocity", &particle->velocity);
	result &= de_object_visitor_visit_float(visitor, "Size", &particle->size);
	result &= de_object_visitor_visit_bool(visitor, "Alive", &particle->alive);
	result &= de_object_visitor_visit_float(visitor, "SizeMod", &particle->size_modifier);
	result &= de_object_visitor_visit_float(visitor, "Lifetime", &particle->lifetime);
	result &= de_object_visitor_visit_float(visitor, "InitialLifetime", &particle->initial_lifetime);
	result &= de_object_visitor_visit_uint32(visitor, "Color", (uint32_t*)&particle->color);
	return result;
}

static bool de_particle_system_emitter_visit(de_object_visitor_t* visitor, de_particle_system_emitter_t* emitter)
{
	bool result = true;
	de_node_t* particle_system_node;
	if (!visitor->is_reading) {
		particle_system_node = de_node_from_particle_system(emitter->particle_system);
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "ParticleSystem", &particle_system_node, de_particle_system_visit);
	if(visitor->is_reading) {
		emitter->particle_system = de_node_to_particle_system(particle_system_node);
	}
	result &= de_object_visitor_visit_int32(visitor, "Type", (int32_t*)&emitter->type);
	result &= de_object_visitor_visit_vec3(visitor, "Position", &emitter->position);
	result &= de_object_visitor_visit_int32(visitor, "MaxParticles", &emitter->max_particles);
	result &= de_object_visitor_visit_int32(visitor, "AliveParticles", &emitter->alive_particles);
	result &= de_object_visitor_visit_int32(visitor, "SpawnRate", &emitter->particle_spawn_rate);	
	result &= de_object_visitor_visit_float(visitor, "Time", &emitter->time);
	result &= de_object_visitor_visit_float(visitor, "MinLifeTime", &emitter->min_lifetime);
	result &= de_object_visitor_visit_float(visitor, "MaxLifeTime", &emitter->max_lifetime);
	result &= de_object_visitor_visit_float(visitor, "MinSize", &emitter->min_size);
	result &= de_object_visitor_visit_float(visitor, "MaxSize", &emitter->max_size);
	result &= de_object_visitor_visit_float(visitor, "MinXVelocity", &emitter->min_x_velocity);
	result &= de_object_visitor_visit_float(visitor, "MaxXVelocity", &emitter->max_x_velocity);
	result &= de_object_visitor_visit_float(visitor, "MinYVelocity", &emitter->min_y_velocity);
	result &= de_object_visitor_visit_float(visitor, "MaxYVelocity", &emitter->max_y_velocity);
	result &= de_object_visitor_visit_float(visitor, "MinZVelocity", &emitter->min_z_velocity);
	result &= de_object_visitor_visit_float(visitor, "MaxZVelocity", &emitter->max_z_velocity);
	result &= de_object_visitor_visit_float(visitor, "MinSizeModifier", &emitter->min_size_modifier);
	result &= de_object_visitor_visit_float(visitor, "MaxSizeModifier", &emitter->max_size_modifier);
	switch(emitter->type) {
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX: {
			de_particle_system_box_emitter_t* box_emitter = &emitter->s.box;
			result &= de_object_visitor_visit_float(visitor, "HalfWidth", &box_emitter->half_width);
			result &= de_object_visitor_visit_float(visitor, "HalfHeight", &box_emitter->half_height);
			result &= de_object_visitor_visit_float(visitor, "HalfDepth", &box_emitter->half_depth);
			break;
		}
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE: {
			de_particle_system_sphere_emitter_t* sphere_emitter = &emitter->s.sphere;
			result &= de_object_visitor_visit_float(visitor, "Radius", &sphere_emitter->radius);
			break;
		}
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_POINT:
			/* nothing special, just make compiler happy */
			break;
        default:
            break;
	}
	return result;
}

static bool de_particle_system_visit(de_object_visitor_t* visitor, de_node_t* node)
{
	bool result = true;
	de_particle_system_t* particle_system = &node->s.particle_system;
	result &= DE_OBJECT_VISITOR_VISIT_ARRAY(visitor, "Particles", particle_system->particles, de_particle_visit);
	de_resource_t* tex_resource = particle_system->texture ? de_resource_from_texture(particle_system->texture) : NULL;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Texture", &tex_resource, de_resource_visit);
	if (visitor->is_reading && tex_resource) {
		de_particle_system_set_texture(particle_system, de_resource_to_texture(tex_resource));
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Emitters", particle_system->emitters, de_particle_system_emitter_visit);
	result &= DE_OBJECT_VISITOR_VISIT_PRIMITIVE_ARRAY(visitor, "FreeParticles", particle_system->free_particles);
	result &= de_color_gradient_visit(visitor, &particle_system->color_gradient_over_lifetime);
	return result;
}

static de_particle_t* de_particle_system_spawn_particle(de_particle_system_t* particle_system, de_particle_system_emitter_t* owner)
{
	de_particle_t* particle;
	if (particle_system->free_particles.size > 0) {
		int alive_index = DE_ARRAY_POP(particle_system->free_particles);
		particle = particle_system->particles.data + alive_index;
	} else {
		particle = DE_ARRAY_GROW(particle_system->particles, 1);
	}
	particle->owner = owner;
	particle->alive = true;
	++owner->alive_particles;
	return particle;
}

static void de_particle_system_kill_particle(de_particle_system_t* particle_system, de_particle_t* particle, int index)
{
	DE_ARRAY_APPEND(particle_system->free_particles, index);
	--particle->owner->alive_particles;
	particle->alive = false;
	particle->lifetime = particle->initial_lifetime;
}

static void de_particle_system_emitter_emit(de_particle_system_emitter_t* emitter, float dt)
{
	emitter->time += dt;
	const float time_amount_per_particle = 1.0f / emitter->particle_spawn_rate; /* in seconds */
	/* determine how much particles we must spawn per update tick */
	int particle_count = (int)(emitter->time / time_amount_per_particle);
	if (particle_count > 0) {
		/* modify time so we do not lose fraction */
		emitter->time -= time_amount_per_particle * particle_count;
		/* spawn particles */
		if (emitter->max_particles < 0 || emitter->alive_particles < emitter->max_particles) {
			if (emitter->alive_particles + particle_count > emitter->max_particles) {
				/* make sure that we do not exceed maximum amount of particles */
				particle_count = emitter->max_particles - particle_count;
			}
			for (int i = 0; i < particle_count; ++i) {
				de_particle_t* particle = de_particle_system_spawn_particle(emitter->particle_system, emitter);
				particle->lifetime = 0.0f;
				particle->initial_lifetime = de_frand(emitter->min_lifetime, emitter->max_lifetime);
				particle->color = (de_color_t) { 255, 255, 255, 255 };
				particle->size = de_frand(emitter->min_size, emitter->max_size);
				particle->size_modifier = de_frand(emitter->min_size_modifier, emitter->max_size_modifier);
				particle->velocity = (de_vec3_t) {
					.x = de_frand(emitter->min_x_velocity, emitter->max_x_velocity),
					.y = de_frand(emitter->min_y_velocity, emitter->max_y_velocity),
					.z = de_frand(emitter->min_z_velocity, emitter->max_z_velocity)
				};
				/* position defined by emitter type */
				switch (emitter->type) {
					case DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX: {						
						de_particle_system_box_emitter_t* box_emitter = &emitter->s.box;
						particle->position = (de_vec3_t) { 
							.x = emitter->position.x + de_frand(-box_emitter->half_width, box_emitter->half_width),
							.y = emitter->position.y + de_frand(-box_emitter->half_height, box_emitter->half_height),
							.z = emitter->position.z + de_frand(-box_emitter->half_depth, box_emitter->half_depth)
						};								
						break;
					}
					case DE_PARTICLE_SYSTEM_EMITTER_TYPE_POINT: {
						particle->position = (de_vec3_t) { .x = 0, .y = 0, .z = 0 };
						break;
					}
					case DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE: {
						de_particle_system_sphere_emitter_t* sphere_emitter = &emitter->s.sphere;
						/* generate random spherical coordinates and convert to cartesian */
						const float phi = de_frand(0.0f, (float)M_PI);
						const float theta = de_frand(0.0f, 2.0f * (float)M_PI);
						const float radius = de_frand(0.0f, sphere_emitter->radius);
						const float cos_theta = (float)cos(theta);
						const float sin_theta = (float)sin(theta);
						const float cos_phi = (float)cos(phi);
						const float sin_phi = (float)sin(phi);
						particle->position = (de_vec3_t) { 
							.x = radius * sin_theta * cos_phi,
							.y = radius * sin_theta * sin_phi,
							.z = radius * cos_theta
						};
						break;
                    default:
                        break;
					}
				}
			}
		}
	}
}

de_node_dispatch_table_t* de_particle_system_get_dispatch_table()
{
	static de_node_dispatch_table_t table = {
		.init = de_particle_system_init,
		.free = de_particle_system_free,
		.visit = de_particle_system_visit,
	};
	return &table;
}

void de_particle_system_update(de_particle_system_t* particle_system, float dt)
{
	for (size_t i = 0; i < particle_system->emitters.size; ++i) {
		de_particle_system_emitter_t* emitter = particle_system->emitters.data[i];
		de_particle_system_emitter_emit(emitter, dt);
	}

	for (size_t i = 0; i < particle_system->particles.size; ++i) {
		de_particle_t* particle = particle_system->particles.data + i;

		if (particle->alive) {
			particle->lifetime += dt;
			if (particle->lifetime >= particle->initial_lifetime) {
				de_particle_system_kill_particle(particle_system, particle, i);
			} else {
				de_vec3_add(&particle->position, &particle->position, &particle->velocity);
				particle->size += particle->size_modifier;
				particle->color = de_color_gradient_get_color(&particle_system->color_gradient_over_lifetime, particle->lifetime / particle->initial_lifetime);
			}
		}
	}
}

void de_particle_system_generate_vertices(de_particle_system_t* particle_system)
{
	int alive_particles = 0;
	DE_ARRAY_CLEAR(particle_system->indices);
	DE_ARRAY_CLEAR(particle_system->vertices);
	for (size_t i = 0; i < particle_system->particles.size; ++i) {
		const de_particle_t* particle = particle_system->particles.data + i;

		if (!particle->alive) {
			continue;
		}

		const uint32_t packed_color = de_color_to_int(&particle->color);

		/* Prepare quad vertices */
		de_particle_vertex_t* vertex = DE_ARRAY_GROW(particle_system->vertices, 4);
		vertex->position = particle->position;
		vertex->corner = (de_vec2_t) { -1.0f, 1.0f };
		vertex->tex_coord = (de_vec2_t) { 0.0f, 0.0f };
		vertex->size = particle->size;
		vertex->color = packed_color;

		++vertex;
		vertex->position = particle->position;
		vertex->corner = (de_vec2_t) { 1.0f, 1.0f };
		vertex->tex_coord = (de_vec2_t) { 1.0f, 0.0f };
		vertex->size = particle->size;
		vertex->color = packed_color;

		++vertex;
		vertex->position = particle->position;
		vertex->corner = (de_vec2_t) { 1.0f, -1.0f };
		vertex->tex_coord = (de_vec2_t) { 1.0f, 1.0f };
		vertex->size = particle->size;
		vertex->color = packed_color;

		++vertex;
		vertex->position = particle->position;
		vertex->corner = (de_vec2_t) { -1.0f, -1.0f };
		vertex->tex_coord = (de_vec2_t) { 0.0f, 1.0f };
		vertex->size = particle->size;
		vertex->color = packed_color;

		/* Prepare indices */
		int* index = DE_ARRAY_GROW(particle_system->indices, 6);
		const int base_index = alive_particles * 4;

		*index = base_index;
		*(++index) = base_index + 1;
		*(++index) = base_index + 2;

		*(++index) = base_index;
		*(++index) = base_index + 2;
		*(++index) = base_index + 3;

		++alive_particles;
	}
}

void de_particle_system_set_texture(de_particle_system_t* particle_system, de_texture_t* texture)
{
	if (particle_system->texture) {
		de_resource_release(de_resource_from_texture(particle_system->texture));
	}

	particle_system->texture = texture;

	if (particle_system->texture) {
		de_resource_add_ref(de_resource_from_texture(particle_system->texture));
	}
}

de_particle_system_emitter_t* de_particle_system_emitter_create(de_particle_system_t* particle_system, de_particle_system_emitter_type_t type)
{
	de_particle_system_emitter_t* emitter = DE_NEW(de_particle_system_emitter_t);
	emitter->type = type;
	emitter->particle_system = particle_system;
	DE_ARRAY_APPEND(particle_system->emitters, emitter);
	/* default values */
	emitter->min_lifetime = 5.0f;
	emitter->max_lifetime = 10.0f;
	emitter->min_size = 0.125f;
	emitter->max_size = 0.250f;
	emitter->min_size_modifier = 0.0005f;
	emitter->max_size_modifier = 0.0010f;
	emitter->min_x_velocity = -0.001f;
	emitter->max_x_velocity = 0.001f;
	emitter->min_y_velocity = -0.001f;
	emitter->max_y_velocity = 0.001f;
	emitter->min_z_velocity = -0.001f;
	emitter->max_z_velocity = 0.001f;
	switch(type) {
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX: {
			de_particle_system_box_emitter_t* box_emitter = &emitter->s.box;
			box_emitter->half_width = 0.5f;
			box_emitter->half_height = 0.5f;
			box_emitter->half_depth = 0.5f;
			break;
		}
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE: {
			de_particle_system_sphere_emitter_t* sphere_emitter = &emitter->s.sphere;
			sphere_emitter->radius = 1.0f;
			break;
		}
        default:
            break;
	}
	return emitter;
}

de_color_gradient_t* de_particle_system_get_color_gradient_over_lifetime(de_particle_system_t* particle_system) 
{
	DE_ASSERT(particle_system);
	return &particle_system->color_gradient_over_lifetime;
}