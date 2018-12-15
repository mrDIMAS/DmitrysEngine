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
void de_camera_init(de_node_t* node, de_camera_t* c)
{
	de_rectf_t viewport = { 0, 0, 1, 1 };
	de_camera_set_viewport(c, &viewport);
	c->parent_node = node;
	c->fov = 75.0f;
	c->z_far = 1024.0f;
	c->z_near = 0.05f;
	c->aspect = node->parent_scene->core->params.width / (float)node->parent_scene->core->params.height;
}

/*=======================================================================================*/
void de_camera_deinit(de_camera_t* c)
{
	assert(c);
	DE_UNUSED(c);
}

/*=======================================================================================*/
void de_camera_update(de_camera_t* cam)
{
	de_vec3_t eye, look, up;
	de_node_t* node = cam->parent_node;

	assert(node);
	assert(node->type == DE_NODE_CAMERA);

	de_node_calculate_transforms(node);
	de_node_get_global_position(node, &eye);
	de_node_get_look_vector(node, &look);
	de_node_get_up_vector(node, &up);

	de_vec3_add(&look, &look, &eye);
	de_mat4_look_at(&cam->view_matrix, &eye, &look, &up);

	de_mat4_perspective(&cam->projection_matrix, de_deg_to_rad(cam->fov), cam->aspect, cam->z_near, cam->z_far);
	de_mat4_mul(&cam->view_projection_matrix, &cam->projection_matrix, &cam->view_matrix);

	de_mat4_inverse(&cam->inv_view_proj, &cam->view_projection_matrix);
}

/*=======================================================================================*/
void de_camera_set_viewport(de_camera_t* c, const de_rectf_t* viewport)
{
	assert(c);
	assert(viewport);
	c->viewport = *viewport;
	c->aspect = c->viewport.w / c->viewport.h;
}