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

/**
 * ASCII + Binary FBX Loader
 *
 * -------------------
 * | IMPORTANT STUFF |
 * vvvvvvvvvvvvvvvvvvv
 *
 * What is NOT supported:
 * - Animated geometric transform (scale, translation, rotation), basically there is only
 *   translation, rotation, scale is supported as animated properties. Poke me if you need
 *   support of other properties to be animated.
 * - Cameras
 * - Area and Volume lights
 * - Some of vertex components mappings. Engine will shoot error if will see unsupported
 *   mapping. This stuff is still in progress.
 * - Embedded media
 * - Maybe some other stuff, about which I don't know.
 *
 * Important conventions:
 *
 * To make life easier, here is some convetions for textures that you can use in
 * development.
 *
 * - For normal maps you should add suffix "_normal" to your normal map, so engine
 *   can correctly assign it to surface. So if you using diffuse texture called "foo.tga"
 *   you should create a normap map texture called "foo_normal.tga", and engine will
 *   automatically link them together.
 * - For height maps suffix is "_height"
 *
 **/

 /* Supported versions */
#define DE_FBX_VERSION_MIN 7000
#define DE_FBX_VERSION_MAX 7400

typedef enum de_fbx_mapping_t {
	DE_FBX_MAPPING_UNKNOWN,
	DE_FBX_MAPPING_BY_POLYGON,
	DE_FBX_MAPPING_BY_POLYGON_VERTEX,
	DE_FBX_MAPPING_BY_VERTEX,
	DE_FBX_MAPPING_BY_EDGE,
	DE_FBX_MAPPING_ALL_SAME
} de_fbx_mapping_t;

typedef enum de_fbx_reference_t {
	DE_FBX_REFERENCE_UNKNOWN,
	DE_FBX_REFERENCE_DIRECT,
	DE_FBX_REFERENCE_INDEX_TO_DIRECT
} de_fbx_reference_t;

#include "fbx/fbx_node.h"
#include "fbx/fbx_ascii.h"
#include "fbx/fbx_binary.h"

/**
* @brief
* @param file
* @return
*/
de_node_h de_fbx_load_to_scene(de_scene_t* scene, const char* file);

bool de_fbx_is_binary(const char* filename);