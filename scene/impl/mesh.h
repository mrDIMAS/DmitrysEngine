/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

/*=======================================================================================*/
void de_mesh_init(de_node_t* node, de_mesh_t* mesh)
{
	DE_ARRAY_INIT(mesh->surfaces);

	mesh->parent_node = node;
}

/*=======================================================================================*/
void de_mesh_deinit(de_mesh_t* mesh)
{
	size_t i;

	/* Free surfaces */
	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_renderer_free_surface(mesh->surfaces.data[i]);
	}
	DE_ARRAY_FREE(mesh->surfaces);
}

/*=======================================================================================*/
void de_mesh_make_cube(de_mesh_t* mesh)
{
	static de_vertex_t vertices[] = {
		/* front */
		{ { -0.5, -0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5, 0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, 0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, -0.5, 0.5 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },

		/* back */
		{ { -0.5, -0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5, 0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, 0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5, -0.5, -0.5 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },

		/* left */
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },

		/* right */
		{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },

		/* top */
		{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },

		/* bottom */
		{ { -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } },
		{ { 0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0 }, { 0 } }
	};

	static int faces[] = {
		2, 1, 0,
		3, 2, 0,

		4, 5, 6,
		4, 6, 7,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		18, 17, 16,
		19, 18, 16,

		20, 21, 22,
		20, 22, 23
	};

	de_surface_t* surf;

	surf = de_renderer_create_surface(mesh->parent_node->scene->core->renderer);

	de_surface_load_data(surf, vertices, DE_ARRAY_SIZE(vertices), faces, DE_ARRAY_SIZE(faces));

	de_mesh_add_surface(mesh, surf);

	de_mesh_calculate_normals(mesh);
}

/*=======================================================================================*/
void de_mesh_calculate_normals(de_mesh_t * mesh)
{
	size_t n;
	for (n = 0; n < mesh->surfaces.size; ++n)
	{
		de_surface_t * surf = mesh->surfaces.data[n];
		de_surface_calculate_normals(surf);
	}
}

/*=======================================================================================*/
void de_mesh_add_surface(de_mesh_t* mesh, de_surface_t* surf)
{
	if (!mesh || !surf)
	{
		return;
	}

	DE_ARRAY_APPEND(mesh->surfaces, surf);
}

/*=======================================================================================*/
void de_mesh_set_texture(de_mesh_t* mesh, de_texture_t* texture)
{
	size_t i;

	if (!mesh || !texture)
	{
		return;
	}

	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		de_surface_set_texture(mesh->surfaces.data[i], texture);
	}
}