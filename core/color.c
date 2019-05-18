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

void de_color_set(de_color_t* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
}

uint32_t de_color_to_int(const de_color_t* color)
{
	return *((uint32_t*)color); /* CAVEAT: will fail if struct will be changed! */
}

de_color_t de_color_interpolate(const de_color_t* a, const de_color_t* b, float t) {
	return (de_color_t) {
		.r = (uint8_t)((int)a->r + (int)(t * ((int)b->r - (int)a->r))),
		.g = (uint8_t)((int)a->g + (int)(t * ((int)b->g - (int)a->g))),
		.b = (uint8_t)((int)a->b + (int)(t * ((int)b->b - (int)a->b))),
		.a = (uint8_t)((int)a->a + (int)(t * ((int)b->a - (int)a->a))),
	};
}