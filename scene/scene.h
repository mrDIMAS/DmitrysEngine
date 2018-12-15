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

struct de_scene_t
{
	de_core_t* core;

	DE_LINKED_LIST_ITEM(struct de_scene_t);

	DE_LINKED_LIST_DECLARE(de_node_t, nodes);
	DE_LINKED_LIST_DECLARE(de_body_t, bodies);
	DE_LINKED_LIST_DECLARE(de_static_geometry_t, static_geometries);
	DE_LINKED_LIST_DECLARE(de_animation_t, animations);

	de_node_t* active_camera;
};

/**
 * @brief Creates new empty scene and attaches it to engine instance.
 */
de_scene_t* de_scene_create(de_core_t* core);

/**
 * @brief Destroys all scene resources and deallocates memory.
 */
void de_scene_free(de_scene_t* s);

/**
 * @brief Creates new body with radius 1.0
 * @return pointer to new body
 */
de_body_t* de_scene_create_body(de_scene_t* s);

/**
 * @brief Frees all resources associated with body
 * @param body pointer to body
 */
void de_scene_free_body(de_scene_t* s, de_body_t* body);

/**
 * @brief Creates new empty static geometry
 * @return pointer to new static geometry
 */
de_static_geometry_t* de_scene_create_static_geometry(de_scene_t* s);

/**
 * @brief Frees static geometry. Removes it from scene.
 */
void de_scene_free_static_geometry(de_scene_t* s, de_static_geometry_t* geom);

/**
 * @brief Adds node to scene. Only attached nodes can interact and be renderered.
 */
void de_scene_add_node(de_scene_t* s, de_node_t* node);

/**
 * @brief Removes node from scene (does not frees node!). Node will not be renderered and
 *        interaction will be suspended.
 */
void de_scene_remove_node(de_scene_t* s, de_node_t* node);

/**
 * @brief Creates new empty animation and attaches it to scene
 */
de_animation_t* de_animation_create(de_scene_t* s);

/**
 * @brief Tries to find a node with specified name. Performs linear search O(n).
 */
de_node_t* de_scene_find_node(const de_scene_t* s, const char* name);

/**
 * @brief Update scene components (i.e. animations)
 */
void de_scene_update(de_scene_t* s, float dt);