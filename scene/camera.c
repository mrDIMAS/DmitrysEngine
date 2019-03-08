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

void de_camera_copy(de_camera_t* src_camera, de_camera_t* dest_camera) {
	/* will raise compile error if camera struct has changed, so 
	 * forcing programmer to fix copying */
	DE_STATIC_ASSERT(sizeof(de_camera_t) == 288, fix_camera_copying);
	dest_camera->fov = src_camera->fov;
	dest_camera->aspect = src_camera->aspect;
	dest_camera->z_near = src_camera->z_near;
	dest_camera->z_far = src_camera->z_far;
	dest_camera->view_matrix = src_camera->view_matrix;
	dest_camera->projection_matrix = src_camera->projection_matrix;
	dest_camera->view_projection_matrix = src_camera->view_projection_matrix;
	dest_camera->inv_view_proj = src_camera->inv_view_proj;
	dest_camera->viewport = src_camera->viewport;
}

bool de_camera_visit(de_object_visitor_t* visitor, de_camera_t* camera) {	
	DE_STATIC_ASSERT(sizeof(de_camera_t) == 288, fix_camera_visiting);	
	bool result = true;
	result &= de_object_visitor_visit_float(visitor, "Fov", &camera->fov);
	result &= de_object_visitor_visit_float(visitor, "Aspect", &camera->aspect);
	result &= de_object_visitor_visit_float(visitor, "ZNear", &camera->z_near);
	result &= de_object_visitor_visit_float(visitor, "ZFar", &camera->z_far);
	result &= de_object_visitor_visit_mat4(visitor, "ViewMatrix", &camera->view_matrix);
	result &= de_object_visitor_visit_mat4(visitor, "ProjMatrix", &camera->projection_matrix);
	result &= de_object_visitor_visit_mat4(visitor, "ViewProjMatrix", &camera->view_projection_matrix);
	result &= de_object_visitor_visit_mat4(visitor, "InvViewProjMatrix", &camera->inv_view_proj);
	result &= de_object_visitor_visit_rectf(visitor, "Viewport", &camera->viewport);
	return result;
}

void de_camera_init(de_camera_t* c) {
	de_node_t* node = de_node_from_camera(c);
	de_core_t* core = node->scene->core;	
	de_rectf_t viewport = { 0, 0, 1, 1 };
	de_camera_set_viewport(c, &viewport);
	c->fov = 75.0f;
	c->z_far = 1024.0f;
	c->z_near = 0.05f;
	c->aspect = core->params.video_mode.width / (float)core->params.video_mode.height;	
}

void de_camera_deinit(de_camera_t* camera) {
	DE_UNUSED(camera);
}

void de_camera_update(de_camera_t* cam) {
	de_vec3_t eye, look, up;
	de_node_t* node = de_node_from_camera(cam);

	assert(node);
	assert(node->type == DE_NODE_TYPE_CAMERA);

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

void de_camera_set_viewport(de_camera_t* c, const de_rectf_t* viewport) {
	assert(c);
	assert(viewport);
	c->viewport = *viewport;
	c->aspect = c->viewport.w / c->viewport.h;
}

void de_camera_set_fov(de_camera_t* camera, float fov) {
	camera->fov = fov;
}
