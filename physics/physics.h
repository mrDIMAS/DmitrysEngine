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

#define DE_MAX_CONTACTS (8)
#define DE_AIR_FRICTION (0.01f)
#define DE_DEFAULT_GRAVITY (de_vec3_t) { 0.0f, -9.81f, 0.0f }

/**
* @class de_contact_t
* @brief Physical contact
*/
typedef struct de_contact_t {
	de_body_t* body;                 /**< Handle of body with which contact is appeared */
	de_vec3_t position;             /**< Position of contact */
	de_vec3_t normal;               /**< Normal vector in contact point */
	de_static_triangle_t* triangle; /**< Pointer to triangle of static geometry */
	de_static_geometry_t* geometry;
} de_contact_t;

#include "physics/octree.h"
#include "physics/shape.h"
#include "physics/body.h"
#include "physics/collision.h"
#include "physics/gjk_epa.h"