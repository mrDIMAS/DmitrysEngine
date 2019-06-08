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

static void de_gui_image_apply_descriptor(de_gui_node_t* n, const de_gui_node_descriptor_t* desc)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_IMAGE);
	const de_gui_image_descriptor_t* img_desc = &desc->s.image;
	de_gui_image_set_texture(n, img_desc->texture);
}

static void de_gui_image_render(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting)
{
	const de_vec2_t* scr_pos = &n->screen_position;
	const de_gui_image_t* img = &n->s.image;
	de_vec2_t tex_coords[4] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_IMAGE);

	DE_UNUSED(nesting);

	de_gui_draw_list_push_rect_filled(dl, scr_pos, &n->actual_size, &n->color, tex_coords);
	de_gui_draw_list_commit(dl, DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, img->texture ? img->texture->id : 0, n);
}

struct de_gui_node_dispatch_table_t* de_gui_image_get_dispatch_table()
{
	static de_gui_node_dispatch_table_t dispatch_table = {
		.render = de_gui_image_render,
		.apply_descriptor = de_gui_image_apply_descriptor
	};
	return &dispatch_table;
}

void de_gui_image_set_texture(de_gui_node_t* n, de_texture_t* tex)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_IMAGE);
	de_gui_image_t* img = &n->s.image;
	if(img->texture) {
		de_resource_release(de_resource_from_texture(img->texture));
	}
	img->texture = tex;
	if(img->texture) {
		de_resource_add_ref(de_resource_from_texture(img->texture));
	}
}

de_texture_t* de_gui_image_get_texture(de_gui_node_t* n)
{
	DE_ASSERT_GUI_NODE_TYPE(n, DE_GUI_NODE_IMAGE);
	const de_gui_image_t* img = &n->s.image;
	return img->texture;
}