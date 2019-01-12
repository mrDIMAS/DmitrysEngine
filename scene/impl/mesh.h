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

/*=======================================================================================*/
void de_mesh_init(de_node_t* node)
{
	de_mesh_t* mesh = &node->s.mesh;

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
bool de_mesh_is_skinned(de_mesh_t* mesh)
{
	size_t i;

	for (i = 0; i < mesh->surfaces.size; ++i)
	{
		if (de_surface_is_skinned(mesh->surfaces.data[i]))
		{
			return true;
		}
	}

	return false;
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
		de_surface_set_diffuse_texture(mesh->surfaces.data[i], texture);
	}
}