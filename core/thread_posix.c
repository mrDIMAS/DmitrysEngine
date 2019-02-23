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

int de_thrd_create(de_thrd_t* thr, de_thrd_start_t func, void* arg) {	
    pthread_create(thr, NULL, (void*(*)(void*)) func, arg);
    return true;
}

int de_thrd_detach(de_thrd_t* thr) {    
    return pthread_detach(*thr) == 0;
}

int de_thrd_join(de_thrd_t* thr) {
    void* code;
    return pthread_join(*thr, &code);
}

void de_mtx_init(de_mtx_t* mtx) {
    pthread_mutex_init(mtx, NULL);
}

void de_mtx_lock(de_mtx_t* mtx) {	
	pthread_mutex_lock(mtx);
}

void de_mtx_unlock(de_mtx_t* mtx) {
	pthread_mutex_unlock(mtx);
}

void de_mtx_destroy(de_mtx_t* mtx) {
    pthread_mutex_destroy(mtx);
}

void de_cnd_init(de_cnd_t* cnd) {
    pthread_cond_init(cnd, NULL);
}

void de_cnd_destroy(de_cnd_t* cnd) {	
    pthread_cond_destroy(cnd);
}

void de_cnd_signal(de_cnd_t* cnd) {
    pthread_cond_signal(cnd);
}

void de_cnd_broadcast(de_cnd_t* cnd) {
    pthread_cond_broadcast(cnd);
}

void de_cnd_wait(de_cnd_t* cnd, de_mtx_t* mtx) {
    pthread_cond_wait(cnd, mtx);
}