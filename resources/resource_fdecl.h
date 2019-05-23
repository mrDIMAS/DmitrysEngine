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

/* Forward declarations for resources. */

typedef enum de_resource_type_t {
	DE_RESOURCE_TYPE_MODEL,
	DE_RESOURCE_TYPE_TEXTURE,
	DE_RESOURCE_TYPE_SOUND_BUFFER
} de_resource_type_t;

typedef enum de_resource_flag_t {
	/**
	 * Resource is created in runtime. Will be serialized unless
	 * DE_RESOURCE_FLAG_PERSISTENT is set. Useful for dynamic resource like procedural
	 * textures.
	 */
	DE_RESOURCE_FLAG_PROCEDURAL = DE_BIT(0),

	/**
	 * Resource is will exist until core is destroyed. Such resource WON'T be serialized
	 * by object visitor! For example such resources may be useful for UI textures which will
	 * exist all the time game is running.
	 **/
	DE_RESOURCE_FLAG_PERSISTENT = DE_BIT(1),

	/**
	 * Internal. Do not use.
	 *
	 * Internal persistent engine resources will be marked with this flag.
	 */
	DE_RESOURCE_FLAG_INTERNAL = DE_BIT(2),
} de_resource_flags_t;

typedef struct de_resource_dispatch_table_t {
	void(*init)(de_resource_t* res);
	void(*deinit)(de_resource_t* res);
	bool(*visit)(de_object_visitor_t* visitor, de_resource_t* res);
	/* load implementation for external resources */
	bool(*load)(de_resource_t* res);
} de_resource_dispatch_table_t;

