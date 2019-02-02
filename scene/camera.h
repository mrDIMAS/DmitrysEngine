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

/**
* @brief Camera component.
*/
typedef struct de_camera_t {
	de_node_t* parent_node;
	float fov;                        /**< Field-of-view in degrees. Read-only. */
	float aspect;                     /**< Aspect ratio of camera viewport. Read-only. */
	float z_near;                     /**< Distance to near clipping plane */
	float z_far;                      /**< Distance to far clipping plane */
	de_mat4_t view_matrix;            /**< View matrix */
	de_mat4_t projection_matrix;      /**< Projection matrix */
	de_mat4_t view_projection_matrix; /**< Combined view * projection matrix. Read-only. */
	de_mat4_t inv_view_proj;
	de_rectf_t viewport;               /**< Viewport rectangle in ratio. Default: 0,0,1,1 */
} de_camera_t;

/**
 * @brief Specializes node as camera.
 */
void de_camera_init(de_node_t* node);

/**
 * @brief
 * @param
 */
void de_camera_deinit(de_camera_t* c);

/**
* @brief Builds camera matrices. Camera have to be attached to some node.
* @param camera pointer to camera component
*/
void de_camera_update(de_camera_t* c);

/**
* @brief Sets camera viewport in pixels
* @param camera pointer to camera
* @param x
* @param y
* @param w
* @param h
*/
void de_camera_set_viewport(de_camera_t* camera, const de_rectf_t* viewport);

void de_camera_set_fov(de_camera_t* camera, float fov) {
	camera->fov = fov;
}
