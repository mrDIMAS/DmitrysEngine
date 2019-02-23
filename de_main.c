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

#ifdef __cplusplus
extern "C" {
#endif

#include "de_main.h"
	
#define TINFL_IMPLEMENTATION
#include "external/miniz_tinfl_impl.h"
#include "resources/impl/image.h"
#include "core/impl/byteorder.h"
#include "core/impl/array.h"
#include "core/impl/color.h"
#include "core/impl/pool.h"
#include "core/impl/log.h" 
#include "core/impl/memmgr.h"
#include "core/impl/base64.h"
#include "core/string_utils_impl.h"
#include "core/string_impl.h"
#include "core/utf32string_impl.h"
#include "core/path_impl.h"
#include "core/impl/rectpack.h"
#include "core/impl/rect.h"
#include "core/impl/utility.h"
#include "core/impl/serialization.h"
#include "core/impl/core.h"
#include "physics/impl/octree.h"
#include "physics/impl/collision.h"
#include "fbx/impl/fbx.h"
#include "font/impl/font.h"
#include "math/impl/mathlib.h"
#include "math/impl/triangulator.h"
#include "scene/impl/animation.h"
#include "scene/impl/camera.h"
#include "scene/impl/light.h"
#include "scene/impl/mesh.h"
#include "scene/impl/node.h"
#include "scene/impl/scene.h"
#include "renderer/impl/renderer.h"
#include "renderer/impl/surface.h"
#include "resources/impl/texture.h"
#include "gui/impl/gui.h" 
#include "vg/impl/vgraster.h"
#include "core/thread_impl.h"
#include "sound/sound_impl.h"

#ifdef __cplusplus
}
#endif