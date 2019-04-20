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

int de_thrd_create(de_thrd_t* thr, de_thrd_start_t func, void* arg)
{
	thr->handle = (intptr_t)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)func, arg, 0, 0);
	return thr->handle != 0;
}

int de_thrd_detach(de_thrd_t* thr)
{
	CloseHandle((HANDLE)thr->handle);
	return true;
}

int de_thrd_join(de_thrd_t* thr)
{
	WaitForSingleObject((HANDLE)thr->handle, INFINITE);
	return true;
}

void de_mtx_init(de_mtx_t* mtx)
{
	CRITICAL_SECTION* ptr = de_malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(ptr);
	mtx->handle = (intptr_t)ptr;
}

void de_mtx_lock(de_mtx_t* mtx)
{
	EnterCriticalSection((CRITICAL_SECTION*)mtx->handle);
}

void de_mtx_unlock(de_mtx_t* mtx)
{
	LeaveCriticalSection((CRITICAL_SECTION*)mtx->handle);
}

void de_mtx_destroy(de_mtx_t* mtx)
{
	DeleteCriticalSection((CRITICAL_SECTION*)mtx->handle);
	de_free((void*)mtx->handle);
}

void de_cnd_init(de_cnd_t* cnd)
{
	CONDITION_VARIABLE* ptr = de_malloc(sizeof(CONDITION_VARIABLE));
	InitializeConditionVariable(ptr);
	cnd->handle = (intptr_t)ptr;
}

void de_cnd_destroy(de_cnd_t* cnd)
{
/* no WinAPI call needed */
	de_free((CONDITION_VARIABLE*)cnd->handle);
}

void de_cnd_signal(de_cnd_t* cnd)
{
	WakeConditionVariable((CONDITION_VARIABLE*)cnd->handle);
}

void de_cnd_broadcast(de_cnd_t* cnd)
{
	WakeAllConditionVariable((CONDITION_VARIABLE*)cnd->handle);
}

void de_cnd_wait(de_cnd_t* cnd, de_mtx_t* mtx)
{
	SleepConditionVariableCS((CONDITION_VARIABLE*)cnd->handle, (CRITICAL_SECTION*)mtx->handle, INFINITE);
}
