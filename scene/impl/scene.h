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
de_scene_t* de_scene_create(void)
{
	de_scene_t* s = DE_NEW(de_scene_t);
	DE_LINKED_LIST_INIT(s->nodes);
	DE_LINKED_LIST_APPEND(de_core->scenes, s);
	return s;
}

/*=======================================================================================*/
void de_scene_free(de_scene_t* s)
{
	/* free nodes */
	while (s->nodes.head)
	{
		de_node_free(s->nodes.head);
	}

	/* free bodies */
	while (s->bodies.head)
	{
		de_scene_free_body(s, s->bodies.head);
	}

	/* free geoms */
	while (s->static_geometries.head)
	{
		de_scene_free_static_geometry(s, s->static_geometries.head);
	}

	/* free animations */
	while (s->animations.head)
	{
		de_animation_free(s->animations.head);
	}

	DE_LINKED_LIST_REMOVE(de_core->scenes, s);

	de_free(s);
}

/*=======================================================================================*/
de_body_t* de_scene_create_body(de_scene_t* s)
{
	de_body_t* body;

	assert(s);

	body = DE_NEW(de_body_t);

	DE_LINKED_LIST_APPEND(s->bodies, body);

	body->parent_scene = s;
	body->radius = 1.0f;
	body->friction = 0.985f;
	de_vec3_set(&body->scale, 1, 1, 1);
	de_vec3_set(&body->gravity, 0, -9.81f, 0);

	return body;
}

/*=======================================================================================*/
void de_scene_free_body(de_scene_t* s, de_body_t* body)
{
	assert(s);
	DE_LINKED_LIST_REMOVE(s->bodies, body);
	de_free(body);
}

/*=======================================================================================*/
de_static_geometry_t* de_scene_create_static_geometry(de_scene_t* s)
{
	de_static_geometry_t* geom;
	assert(s);
	geom = DE_NEW(de_static_geometry_t);
	DE_LINKED_LIST_APPEND(s->static_geometries, geom);
	geom->parent_scene = s;
	return geom;
}

/*=======================================================================================*/
void de_scene_free_static_geometry(de_scene_t* s, de_static_geometry_t* geom)
{
	assert(s);
	DE_LINKED_LIST_REMOVE(s->static_geometries, geom);
	DE_ARRAY_FREE(geom->triangles);
	de_free(geom);
}

/*=======================================================================================*/
void de_scene_add_node(de_scene_t* s, de_node_t* node)
{
	DE_LINKED_LIST_APPEND(s->nodes, node);

	node->parent_scene = s;

	if (node->type == DE_NODE_CAMERA)
	{
		s->active_camera = node;
	}
}

/*=======================================================================================*/
void de_scene_remove_node(de_scene_t* s, de_node_t* node)
{
	de_animation_t* anim;
		
	if (node == s->active_camera)
	{
		s->active_camera = NULL;
	}

	/* make sure that we do not store pointer to node in animations */
	DE_LINKED_LIST_FOR_EACH(s->animations, anim)
	{
		size_t i;

		for (i = 0; i < anim->tracks.size; ++i)
		{
			de_animation_track_t* track = anim->tracks.data[i];

			if (track->node == node)
			{
				track->node = NULL;
			}
		}
	}

	node->parent_scene = NULL;

	DE_LINKED_LIST_REMOVE(s->nodes, node);
}

/*=======================================================================================*/
de_animation_t* de_animation_create(de_scene_t* s)
{
	de_animation_t* animation;

	animation = DE_NEW(de_animation_t);
	
	animation->parent_scene = s;
	animation->weight = 1.0f;
	animation->speed = 1.0f;
	animation->flags = DE_ANIMATION_FLAG_ENABLED | DE_ANIMATION_FLAG_LOOPED;

	DE_LINKED_LIST_APPEND(s->animations, animation);

	return animation;
}

/*=======================================================================================*/
de_node_t* de_scene_find_node(const de_scene_t* s, const char* name)
{
	de_node_t* node = NULL;

	DE_LINKED_LIST_FOR_EACH(s->nodes, node)
	{
		if (node->name && (strcmp(node->name, name) == 0))
		{
			break;
		}
	}

	return node;
}

/*=======================================================================================*/
void de_scene_update(de_scene_t* s, float dt)
{
	de_animation_t* anim;
	de_node_t* node;
	
#if 1
	/**
	 * Animations prepass - reset local transform of associated track nodes 
	 * for blending 
	 **/
	DE_LINKED_LIST_FOR_EACH(s->animations, anim)
	{
		size_t i;

		for (i = 0; i < anim->tracks.size; ++i)
		{
			de_animation_track_t* track = anim->tracks.data[i];

			if (track->node)
			{
				de_vec3_zero(&track->node->position);
				de_quat_set(&track->node->rotation, 0, 0, 0, 1);
				de_vec3_set(&track->node->scale, 1, 1, 1);
			}
		}
	}

	/**
	 * Animation pass
	 */
	DE_LINKED_LIST_FOR_EACH(s->animations, anim)
	{
		de_animation_update(anim, dt);
	}

#endif

	/** 
	 * Calculate transforms of nodes
	 */
	DE_LINKED_LIST_FOR_EACH(s->nodes, node)
	{
		de_node_calculate_transforms(node);
	}
}