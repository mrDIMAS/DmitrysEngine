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

void de_listener_init(de_listener_t* l)
{
	l->position = (de_vec3_t) { 0 };
	l->look = (de_vec3_t) { 0, 0, 1 };
	l->up = (de_vec3_t) { 0, 1, 0 };
}

void de_listener_set_position(de_listener_t* l, const de_vec3_t* pos)
{
	DE_ASSERT(l);
	l->position = *pos;
}

void de_listener_get_position(de_listener_t* l, de_vec3_t* pos)
{
	DE_ASSERT(l);
	*pos = l->position;
}

void de_listener_set_orientation(de_listener_t* l, const de_vec3_t* look, const de_vec3_t* up)
{
	DE_ASSERT(l);
	DE_ASSERT(look);
	DE_ASSERT(up);
	de_vec3_cross(&l->ear_axis, look, up);
	de_vec3_normalize(&l->ear_axis, &l->ear_axis);
	de_vec3_normalize(&l->look, look);
	de_vec3_normalize(&l->up, up);
}