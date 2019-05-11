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

static void de_particle_system_init(de_node_t* node)
{
	DE_UNUSED(node);
}

static void de_particle_system_free(de_node_t* node)
{
	de_particle_system_t* particle_system = &node->s.particle_system;
	DE_ARRAY_FREE(particle_system->particles);
	DE_ARRAY_FREE(particle_system->vertices);
	DE_ARRAY_FREE(particle_system->indices);
	if (particle_system->texture) {
		de_resource_release(de_resource_from_texture(particle_system->texture));
	}
}

static bool de_particle_visit(de_object_visitor_t* visitor, de_particle_t* p)
{
	bool result = true;
	result &= de_object_visitor_visit_vec3(visitor, "Position", &p->position);
	result &= de_object_visitor_visit_vec3(visitor, "Velocity", &p->velocity);
	result &= de_object_visitor_visit_float(visitor, "Size", &p->size);
	result &= de_object_visitor_visit_float(visitor, "Lifetime", &p->lifetime);
	result &= de_object_visitor_visit_uint32(visitor, "Color", (uint32_t*)&p->color);
	return result;
}

static bool de_particle_system_emitter_visit(de_object_visitor_t* visitor, de_particle_system_emitter_t* emitter)
{
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "ParticleSystem", &emitter->particle_system, de_particle_system_visit);
	result &= de_object_visitor_visit_int32(visitor, "Type", (int32_t*)&emitter->type);
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
	return result;
}

static void de_particle_system_emitter_emit(de_particle_system_emitter_t* emitter, float dt)
{
	emitter->time += dt;

	const int particle_count = (int)(emitter->particle_spawn_rate * emitter->time);
	if (particle_count > 0) {
		emitter->time = 0;
	}

	switch (emitter->type) {
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_BOX:
			break;
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_POINT:
			break;
		case DE_PARTICLE_SYSTEM_EMITTER_TYPE_SPHERE:
			break;
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

void de_particle_system_create_particles(de_particle_system_t* particle_system, size_t n)
{
	DE_ARRAY_CLEAR(particle_system->particles);
	for (size_t i = 0; i < n; ++i) {
		de_particle_t particle = {
			.lifetime = 1000.0f,
			.position = (de_vec3_t) { de_frand(-1.0f, 1.0f), de_frand(-1.0f, 1.0f), de_frand(-1.0f, 1.0f) },
			.color = (de_color_t) { 255, 255, 255, 255 },
			.size = 0.125f,
			.velocity = (de_vec3_t) { de_frand(-0.001f, 0.001f), de_frand(-0.001f, 0.001f), de_frand(-0.001f, 0.001f) },
		};
		DE_ARRAY_APPEND(particle_system->particles, particle);
	}
}

void de_particle_system_update(de_particle_system_t* particle_system, float dt)
{
	for (size_t i = 0; i < particle_system->emitters.size; ++i) {
		de_particle_system_emitter_t* emitter = particle_system->emitters.data[i];
		de_particle_system_emitter_emit(emitter, dt);
	}	

	for (size_t i = 0; i < particle_system->particles.size; ++i) {
		de_particle_t* particle = particle_system->particles.data + i;
		particle->lifetime -= dt;
		if (particle->lifetime > 0) {
			de_vec3_add(&particle->position, &particle->position, &particle->velocity);
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

		if (particle->lifetime <= 0) {
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

void de_particle_system_add_emitter(de_particle_system_t* particle_system, de_particle_system_emitter_t* emitter)
{
	emitter->particle_system = particle_system;
	DE_ARRAY_APPEND(particle_system->emitters, emitter);
}