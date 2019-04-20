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

/* Partial implementation of C11 threads.h
 * Threads are optional in C11 and most compilers ignore them.
 *
 * WARNING: Current implementation for Windows have minumum supported version
 * Windows Vista. WinXP is not supported!
 *
 * Note: Implementation is platform-specific. */

#if defined(__GNUC__) || defined(__MINGW32__)

#include <pthread.h>

typedef pthread_cond_t de_cnd_t;
typedef pthread_t de_thrd_t;
typedef pthread_mutex_t de_mtx_t;

#else /* Win32 */

typedef struct de_mtx_t {
	intptr_t handle;
} de_mtx_t;

typedef struct de_thrd_t {
	intptr_t handle;
} de_thrd_t;

typedef struct de_cnd_t {
	intptr_t handle;
} de_cnd_t;

#endif

typedef int(*de_thrd_start_t)(void*);

/**
 * @brief Forces thread to sleep specified time
 * @param milliseconds time to sleep in milliseconds
 */
void de_sleep(int milliseconds);

/**
 * @brief Creates new suspended thread.
 */
int de_thrd_create(de_thrd_t* thr, de_thrd_start_t func, void* arg);

/**
 * @brief Runs specified thread.
 */
int de_thrd_detach(de_thrd_t* thr);

/**
 * @brief Runs specified thread and blocks callee thread until specified thread is finished.
 */
int de_thrd_join(de_thrd_t* thr);

/**
 * @brief Creates mutex
 */
void de_mtx_init(de_mtx_t* mtx);

/**
 * @brief Locks mutex.
 */
void de_mtx_lock(de_mtx_t* mtx);

/**
 * @brief Unlocks mutex.
 */
void de_mtx_unlock(de_mtx_t* mtx);

/**
 * @brief Destroys mutex.
 */
void de_mtx_destroy(de_mtx_t* mtx);

/**
 * @brief Initializes new condition variable.
 */
void de_cnd_init(de_cnd_t* cnd);

/**
 * @brief Destroys condition variable.
 */
void de_cnd_destroy(de_cnd_t* cnd);

/**
 * @brief Wake up single thread.
 */
void de_cnd_signal(de_cnd_t* cnd);

/**
 * @brief Wake up all awaiting threads.
 */
void de_cnd_broadcast(de_cnd_t* cnd);

/**
 * @brief Waits until awake signal is received.
 */
void de_cnd_wait(de_cnd_t* cnd, de_mtx_t* mtx);