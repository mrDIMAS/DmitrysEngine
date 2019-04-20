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

static void de_texture_init(de_resource_t* res)
{
	de_texture_t* tex = de_resource_to_texture(res);
	tex->width = 0;
	tex->height = 0;
	tex->byte_per_pixel = 0;
	tex->depth = 0;
	tex->id = 0;
	tex->type = DE_TEXTURE_TYPE_2D;
	tex->pixels = 0;
	tex->need_upload = true;
}

static void de_texture_deinit(de_resource_t* res)
{
	de_texture_t* tex = de_resource_to_texture(res);
	de_renderer_remove_texture(res->core->renderer, tex);
	de_free(tex->pixels);
}

static bool de_texture_load(de_resource_t* res)
{
	de_texture_t* tex = de_resource_to_texture(res);
	de_image_t img = { 0 };
	if (de_image_load_tga(de_path_cstr(&res->source), &img)) {
		tex->width = img.width;
		tex->height = img.height;
		tex->byte_per_pixel = img.byte_per_pixel;
		tex->pixels = img.data;
		tex->need_upload = true;
		return true;
	}
	return false;
}

void de_texture_alloc_pixels(de_texture_t* tex, int w, int h, size_t byte_per_pixel)
{
	tex->width = w;
	tex->height = h;
	tex->byte_per_pixel = byte_per_pixel;
	tex->pixels = de_calloc(w*h, byte_per_pixel);
	tex->need_upload = true;
}

static bool de_texture_visit(de_object_visitor_t* visitor, de_resource_t* res)
{
	de_texture_t* tex = de_resource_to_texture(res);
	DE_UNUSED(tex);
	DE_UNUSED(visitor);
	bool result = true;
	/* todo: visit dynamic textures here */
	return result;
}

de_resource_dispatch_table_t* de_texture_get_dispatch_table(void)
{
	static de_resource_dispatch_table_t table = {
		.init = de_texture_init,
		.deinit = de_texture_deinit,
		.load = de_texture_load,
		.visit = de_texture_visit,
	};
	return &table;
}