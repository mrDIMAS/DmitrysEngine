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
* @brief Possible light types
*/
typedef enum de_light_type_t {
	DE_LIGHT_TYPE_POINT,
	DE_LIGHT_TYPE_DIRECTIONAL,
	DE_LIGHT_TYPE_SPOT
} de_light_type_t;

/**
* @brief Common light component.
*
* Can be any possible light type (point, directional, spot)
*/
struct de_light_t {
	de_light_type_t type; /**< Actual type of light */
	float radius;         /**< Radius of point light */
	de_color_t color;     /**< Color of light */
	float cone_angle;     /**< Angle at cone vertex in radians. Do not set directly! Use de_light_set_cone_angle.*/
	float cone_angle_cos; /**< Precomputed cosine of angle at cone vertex. */
};

struct de_node_dispatch_table_t* de_light_get_dispatch_table(void);

/**
 * @brief
 */
void de_light_set_radius(de_light_t* light, float radius);

/**
 * @brief Sets angle in radians at cone vertex of a spot light.
 */
void de_light_set_cone_angle(de_light_t* light, float angle);

/**
 * @brief Returns angle in radians at cone vertex of a spot light.
 */
float de_light_get_cone_angle(const de_light_t* light);

/**
* @brief Sets current color of a light source.
*/
void de_light_set_color(de_light_t* light, const de_color_t* color);

/**
 * @brief Returns current color of a light source.
 */
void de_light_get_color(const de_light_t* light, de_color_t* color);