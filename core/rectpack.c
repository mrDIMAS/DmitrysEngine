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


de_rectpack_node_t* de_rectpack_create_node(int x, int y, int w, int h)
{
	de_rectpack_node_t* node = DE_NEW(de_rectpack_node_t);
	node->x = x;
	node->y = y;
	node->w = w;
	node->h = h;
	return node;
}


de_rectpack_node_t* de_rectpack_get_free(de_rectpack_node_t* node, int w, int h, void* data)
{
	if (node->split) {
		de_rectpack_node_t* newNode = de_rectpack_get_free(node->children[0], w, h, data);
		if (newNode) {
			return newNode;
		}
		return de_rectpack_get_free(node->children[1], w, h, data);
	}
	if (node->data || node->w < w || node->h < h) {
		return NULL;
	}
	if (node->w == w && node->h == h) {
		node->data = data;
		return node;
	}
	if (node->w - w > node->h - h) {
		node->children[0] = de_rectpack_create_node(node->x, node->y, w, node->h);
		node->children[1] = de_rectpack_create_node(node->x + w, node->y, node->w - w, node->h);
	} else {
		node->children[0] = de_rectpack_create_node(node->x, node->y, node->w, h);
		node->children[1] = de_rectpack_create_node(node->x, node->y + h, node->w, node->h - h);
	}
	node->split = 1;
	return de_rectpack_get_free(node->children[0], w, h, data);
}


void de_rectpack_free(de_rectpack_node_t * node)
{
	if (node && node->split) {
		de_rectpack_free(node->children[0]);
		de_rectpack_free(node->children[1]);
	}
	de_free(node);
}
