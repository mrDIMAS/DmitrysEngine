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

de_sound_source_t* de_sound_source_create(de_sound_device_t* dev) {
	de_sound_source_t* src = DE_NEW(de_sound_source_t);
	src->device = dev;
	src->pitch = 1.0f;
	src->playback_position = 0.0;
	src->pan = 0;
	DE_LINKED_LIST_APPEND(dev->sounds, src);
	return src;
}

void de_sound_source_free(de_sound_source_t* src) {
	DE_LINKED_LIST_REMOVE(src->device->sounds, src);
}

void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf) {
	src->buffer = buf;
}

