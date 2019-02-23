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

#include "de_main.h"
#include "external/miniz_tinfl.c"
#include "resources/image.c"
#include "core/byteorder.c"
#include "core/array.c"
#include "core/color.c"
#include "core/pool.c"
#include "core/log.c" 
#include "core/memmgr.c"
#include "core/base64.c"
#include "core/string_utils.c"
#include "core/string.c"
#include "core/utf32string.c"
#include "core/path.c"
#include "core/rectpack.c"
#include "core/rect.c"
#include "core/utility.c"
#include "core/serialization.c"
#include "core/core.c"
#include "physics/octree.c"
#include "physics/collision.c"
#include "fbx/fbx.c"
#include "font/font.c"
#include "math/mathlib.c"
#include "math/triangulator.c"
#include "scene/animation.c"
#include "scene/camera.c"
#include "scene/light.c"
#include "scene/mesh.c"
#include "scene/node.c"
#include "scene/scene.c"
#include "renderer/renderer.c"
#include "renderer/surface.c"
#include "resources/texture.c"
#include "gui/gui.c" 
#include "vg/vgraster.c"
#include "core/thread.c"
#include "sound/sound.c"