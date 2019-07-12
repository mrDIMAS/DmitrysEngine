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

 /*
  * Important notes:
  * - Minumum supported version of Windows is Windows Vista (due to using of
  *   Windows conditional variables in implentation of threading routines, can be
  *   fixed if someone needs WinXP support)
  * - Engine uses right-handed coordinate system which means Z axis points towards
  *   screen, Y axis points up, and X axis points right.
  * - Vectors are single-column matrices.
  *
  * VERY IMPORTANT NOTES:
  * 1) Every method in engine is *NOT* thread-safe unless noted otherwise.
  *    For example most of sound-related functions are thread-safe and they're marked as such
  *    in comments, keep this in mind please.
  * 2) If method marked as Internal it means you should never use it directly.
  * 3) Use get/set functions instead of accessing fields of engine structures directly.
  *
  **/

#ifndef DE_DENGINE_H
#define DE_DENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Set this to 1 to disable asserts, can be useful for release builds to increase performance a bit. */
#define DE_DISABLE_ASSERTS 0

#define DE_EDITOR_ENABLED 1

/* Enables additional code in the math library and allows you to catch division by zero, NAN's and other
 * weird stuff. When 0 - gives absolutely zero performance impact. Very useful for debugging. */
#define DE_MATH_CHECKS 1

/* Compiler-specific defines */
#ifdef _MSC_VER
#  define _CRT_SECURE_NO_WARNINGS
/* nonstandard extension used: non-constant aggregate initializer.
 * useless warning, because project is C99, where this limitation
 * was dropped */
#  pragma warning(disable: 4204) 
#elif defined __GNUC__
#  define _POSIX_C_SOURCE 200809L
#  pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#else
#  error Compiler not supported
#endif

#define DE_UNUSED(x) ((void)(x))

#define _USE_MATH_DEFINES
#define DE_BIT(n) (1 << n)
#define DE_STRINGIZE_(x) #x
#define DE_STRINGIZE(x) DE_STRINGIZE_(x)
#define DE_STATIC_ASSERT(condition, message) typedef char static_assertion_##message[(condition)?1:-1]

#if DE_DISABLE_ASSERTS
#  define DE_ASSERT
#else 
#  define DE_ASSERT(expression) if(!(expression)) de_fatal_error("assertion failed: '" DE_STRINGIZE(expression) "' at line " DE_STRINGIZE(__LINE__) " in " __FILE__)
#endif

typedef void(*de_proc)(void);

/* Standard library */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Platform-specific */
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  include <process.h>
#  ifndef PATH_MAX
#    define PATH_MAX MAX_PATH
#  endif
#else
#  include <linux/limits.h>
#  include <X11/Xlib.h>
#  include <X11/extensions/Xrandr.h>
#  define __USE_MISC
#  include <unistd.h>
#  include <alsa/asoundlib.h>
#endif

/* OpenGL */
#include <GL/gl.h>
#ifdef _WIN32
/* Windows does not have glext.h since it support only OpenGL 1.1 by default. */
#  include "external/GL/glext.h"
#else
#  include <GL/glext.h>
#endif

/* External header-only dependencies */
#ifdef _WIN32
#  include "external/GL/wglext.h"
#  include <mmsystem.h>
#  include "external/dsound.h"
#else
#  include "GL/glx.h"
#endif

/* Forward declarations */
typedef struct de_renderer_t de_renderer_t;
typedef struct de_node_t de_node_t;
typedef struct de_surface_t de_surface_t;
typedef struct de_animation_track_t de_animation_track_t;
typedef struct de_texture_t de_texture_t;
typedef struct de_static_triangle_t de_static_triangle_t;
typedef struct de_static_geometry_t de_static_geometry_t;
typedef struct de_mesh_t de_mesh_t;
typedef struct de_body_t de_body_t;
typedef struct de_light_t de_light_t;
typedef struct de_gui_t de_gui_t;
typedef struct de_core_t de_core_t;
typedef struct de_scene_t de_scene_t;
typedef struct de_sound_device_t de_sound_device_t;
typedef struct de_sound_source_t de_sound_source_t;
typedef struct de_sound_buffer_t de_sound_buffer_t;
typedef struct de_sound_context_t de_sound_context_t;
typedef struct de_sound_decoder_t de_sound_decoder_t;
typedef struct de_resource_t de_resource_t;
typedef struct de_object_visitor_t de_object_visitor_t;
typedef struct de_video_mode_t de_video_mode_t;

/**
* Order is important here, because some parts depends on other
* Modules with minimum dependencies should be placed before others.
**/
#include "core/log.h"
#include "core/byteorder.h"
#include "core/memmgr.h"
#include "core/array.h"
#include "core/base64.h"
#include "core/thread.h"
#include "core/string.h"
#include "core/string_utils.h"
#include "core/path.h"
#include "core/config.h"
#include "core/utf32string.h"
#include "core/linked_list.h"
#include "core/time.h"
#include "core/color.h"
#include "core/color_gradient.h"
#include "input/input.h"
#include "core/event.h"
#include "resources/builtin_fonts.h"
#include "math/mathlib.h"
#include "core/pathfinder.h"
#include "core/rect.h"
#include "core/serialization.h"
#include "math/triangulator.h"	
#include "core/rectpack.h"
#include "resources/image.h"
#include "vg/vgraster.h"
#include "scene/camera.h"
#include "scene/mesh.h"
#include "scene/light.h"
#include "scene/particle_system.h"
#include "scene/node.h"
#include "scene/animation.h"
#include "scene/scene.h"
#include "physics/physics.h"
#include "renderer/surface.h"
#include "fbx/fbx.h"
#include "renderer/renderer.h"
#include "resources/resource_fdecl.h"
#include "resources/texture.h"
#include "font/font.h"
#include "core/utility.h"
#include "gui/gui.h"
#include "sound/sound.h"
#include "resources/resource.h"
#include "core/core.h" 
#include "external/miniz_tinfl.h"

#if DE_EDITOR_ENABLED
#include "editor/editor.h"
#endif

#ifdef __cplusplus
}
#endif

#endif