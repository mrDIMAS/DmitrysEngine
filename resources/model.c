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

static void de_model_deinit(de_resource_t* res) {
	DE_ASSERT(res->type == DE_RESOURCE_TYPE_MODEL);
	de_model_t* mdl = &res->s.model;
	de_scene_free(mdl->scene);
}

static de_resource_dispatch_table_t* de_model_get_dispatch_table(void) {
	static bool init;
	static de_resource_dispatch_table_t tbl;
	if (!init) {
		tbl.deinit = de_model_deinit;
		init = true;
	}
	return &tbl;
}

de_resource_t* de_model_load(de_core_t* core, const de_path_t* path) {
	de_resource_t* res = de_resource_alloc(core, DE_RESOURCE_TYPE_MODEL, de_model_get_dispatch_table());
	de_model_t* mdl = &res->s.model;
	mdl->scene = de_scene_create(core);
	DE_ARRAY_REMOVE(core->scenes, mdl->scene);
	de_fbx_load_to_scene(mdl->scene, de_path_cstr(path));
	mdl->resource = res;
	return res;
}
