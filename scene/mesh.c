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

de_node_t* de_mesh_get_model_root(de_node_t* node)
{
	do {
		if ((!node->parent || node->parent->model_resource != node->model_resource) && node->model_resource) {
			/* node is root */
			return node;
		}
		node = node->parent;
	} while (node);
	return node;
}

static void de_mesh_copy(de_node_t* src, de_node_t* dest)
{
	de_mesh_t* src_mesh = de_node_to_mesh(src);
	de_mesh_t* dest_mesh = de_node_to_mesh(dest);
	dest_mesh->cast_shadows = src_mesh->cast_shadows;
	for (size_t i = 0; i < src_mesh->surfaces.size; ++i) {
		DE_ARRAY_APPEND(dest_mesh->surfaces, de_surface_copy(src_mesh->surfaces.data[i]));
	}
}

static void de_mesh_resolve(de_node_t* node)
{
	de_mesh_t* mesh = de_node_to_mesh(node);
	de_node_t* root = de_mesh_get_model_root(node);
	if (!root) {
		de_log("Mesh resolve failed - unable to find model root node for node %s. Skinning may be invalid!", de_str8_cstr(&node->name));
		return;
	}
	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		de_surface_t* surf = mesh->surfaces.data[i];
		for (size_t j = 0; j < surf->bones.size; ++j) {
			de_node_t* old_bone = surf->bones.data[j];
			de_node_t* instance = de_node_find_copy_of(root, old_bone);
			surf->bones.data[j] = instance;
		}
	}
}

static bool de_mesh_visit(de_object_visitor_t* visitor, de_node_t* node)
{
	de_mesh_t* mesh = de_node_to_mesh(node);
	if (visitor->is_reading) {
		/* resolve changes */
		if (node->model_resource) {
			de_model_t* mdl = de_resource_to_model(node->model_resource);
			de_node_t* ref_node = de_scene_find_node(mdl->scene, de_str8_cstr(&node->name));
			if (ref_node != NULL) {
				/* resolve surface changes: just copy surfaces from source model */
				de_mesh_t* ref_mesh = de_node_to_mesh(ref_node);
				for (size_t i = 0; i < ref_mesh->surfaces.size; ++i) {
					DE_ARRAY_APPEND(mesh->surfaces, de_surface_copy(ref_mesh->surfaces.data[i]));
				}
			} else {
				de_log("unable to resolve mesh: model reference node was not found!");
			}
		}
	}
	bool result = true;
	result &= de_object_visitor_visit_bool(visitor, "CastShadows", &mesh->cast_shadows);
	/* todo: add dynamic surfaces saving */
	return result;
}

static void de_mesh_init(de_node_t* node)
{
	de_mesh_t* mesh = de_node_to_mesh(node);
	mesh->cast_shadows = true;
	DE_ARRAY_INIT(mesh->surfaces);
}

static void de_mesh_free(de_node_t* node)
{
	de_mesh_t* mesh = de_node_to_mesh(node);
	/* Free surfaces */
	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		de_renderer_free_surface(mesh->surfaces.data[i]);
	}
	DE_ARRAY_FREE(mesh->surfaces);
}

struct de_node_dispatch_table_t* de_mesh_get_dispatch_table(void)
{
	static de_node_dispatch_table_t tbl = {
		.free = de_mesh_free,
		.init = de_mesh_init,
		.visit = de_mesh_visit,
		.resolve = de_mesh_resolve,
		.copy = de_mesh_copy,
	};
	return &tbl;
}

void de_mesh_calculate_normals(de_mesh_t * mesh)
{
	for (size_t n = 0; n < mesh->surfaces.size; ++n) {
		de_surface_t * surf = mesh->surfaces.data[n];
		de_surface_calculate_normals(surf);
	}
}

void de_mesh_add_surface(de_mesh_t* mesh, de_surface_t* surf)
{
	if (!mesh || !surf) {
		return;
	}

	DE_ARRAY_APPEND(mesh->surfaces, surf);
}

bool de_mesh_is_skinned(const de_mesh_t* mesh)
{
	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		if (de_surface_is_skinned(mesh->surfaces.data[i])) {
			return true;
		}
	}

	return false;
}

void de_mesh_set_texture(de_mesh_t* mesh, de_texture_t* texture)
{
	if (!mesh || !texture) {
		return;
	}

	for (size_t i = 0; i < mesh->surfaces.size; ++i) {
		de_surface_set_diffuse_texture(mesh->surfaces.data[i], texture);
	}
}

void de_mesh_set_cast_shadows(de_mesh_t* mesh, bool value)
{
	DE_ASSERT(mesh);
	mesh->cast_shadows = value;
}

bool de_mesh_get_cast_shadows(const de_mesh_t* mesh)
{
	DE_ASSERT(mesh);
	return mesh->cast_shadows;
}