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

typedef enum de_event_type_t {
	DE_EVENT_TYPE_CLOSE,
	DE_EVENT_TYPE_MOUSE_DOWN,
	DE_EVENT_TYPE_MOUSE_UP,
	DE_EVENT_TYPE_MOUSE_WHEEL,
	DE_EVENT_TYPE_MOUSE_MOVE,
	DE_EVENT_TYPE_MOUSE_LEAVE,
	DE_EVENT_TYPE_MOUSE_ENTER,
	DE_EVENT_TYPE_KEY_DOWN,
	DE_EVENT_TYPE_KEY_UP,
	DE_EVENT_TYPE_TEXT,
	DE_EVENT_TYPE_LOST_FOCUS,
	DE_EVENT_TYPE_GOT_FOCUS,
	DE_EVENT_TYPE_RESIZE
} de_event_type_t;

typedef struct de_event_t {
	de_event_type_t type;

	union {
		struct {
			enum de_key key;
			int alt : 1;
			int control : 1;
			int shift : 1;
			int system : 1;
		} key;

		struct {
			enum de_mouse_button button;
			int x, y; /* position */
		} mouse_down;

		struct {
			enum de_mouse_button button;
			int x, y; /* position */
		} mouse_up;

		struct {
			int delta;
			int x, y; /* position */
		} mouse_wheel;

		struct {
			int x, y; /* position */
			int vx, vy; /* velocity */
		} mouse_move;

		struct {
			uint32_t code;
		} text;

		struct {
			int w, h;
		} resize;
	} s;
} de_event_t;