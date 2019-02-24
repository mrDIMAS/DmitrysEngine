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

struct de_sound_context_t {
	de_mtx_t mtx;
	de_core_t* core;
	de_sound_device_t dev;
	de_listener_t listener;
	DE_ARRAY_DECLARE(de_sound_source_t*, sounds);
	DE_ARRAY_DECLARE(de_sound_buffer_t*, buffers);
};

/**
 * @brief Internal. Creates new sound context.
 */
de_sound_context_t* de_sound_context_create(de_core_t* core);

/**
 * @brief Internal. Destroys sound context.
 */
void de_sound_context_free(de_sound_context_t* ctx);

/**
 * @brief Applies properties of every sounds source. This function is blocking, which
 * means that mixer thread will be paused until every sound source isn't updated.
 * You should call this function at least 10 times per second to get decent
 * results. See sound.h to more detailed explanation.
 */
void de_sound_context_update(de_sound_context_t* ctx);

/**
 * @brief Internal. Locks context, use with caution.
 */
void de_sound_context_lock(de_sound_context_t* ctx);

/**
 * @brief Internal. Unlocks context, use with caution.
 */
void de_sound_context_unlock(de_sound_context_t* ctx);

/**
 * @brief Returns pointer to current listener.
 */
de_listener_t* de_sound_context_get_listener(de_sound_context_t* ctx);