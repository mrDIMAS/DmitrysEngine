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

typedef struct de_color_gradient_point_t {
	float location;
	de_color_t color;
} de_color_gradient_point_t;

/**
* @brief 1D color gradient.
*/
typedef struct de_color_gradient_t {
	DE_ARRAY_DECLARE(de_color_gradient_point_t, points);
} de_color_gradient_t;

void de_color_gradient_init(de_color_gradient_t* gradient);

/**
* @brief Adds new color point to gradient at specified location. Location must be in [0; 1] range!
*/
void de_color_gradient_add_point(de_color_gradient_t* gradient, float location, const de_color_t* color);

de_color_t de_color_gradient_get_color(const de_color_gradient_t* gradient, float location);

void de_color_gradient_clear(de_color_gradient_t* gradient);

void de_color_gradient_free(de_color_gradient_t* gradient);

bool de_color_gradient_visit(de_object_visitor_t* visitor, de_color_gradient_t* gradient);

void de_color_gradient_tests();