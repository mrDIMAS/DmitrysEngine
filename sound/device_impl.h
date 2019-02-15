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
* @brief Internal. Prepares platform-specific sound device.
*/
bool de_sound_device_setup(de_sound_device_t* dev);

/**
* @brief Internal. Closes platform-specific sound device.
*/
void de_sound_device_shutdown(de_sound_device_t* dev);

/**
* @brief Sends data to output device.
*
* Note: This function is blocking.
*/
void de_sound_device_send_data(de_sound_device_t* dev);

#ifdef _WIN32
#include "sound/device_impl_dsound.h"
#else
#include "sound/device_impl_alsa.h"
#endif

short waveSIN(int x, float frequency) {
	return (short)(32767.0f * (float)sin(frequency * 2.0f * M_PI * ((float)x / SW_OUTPUT_DEVICE_SAMPLE_RATE)));
}

static int de_sound_device_mixer_thread(void* ptr) {
	size_t i;
	de_sound_device_t* dev = (de_sound_device_t*)ptr;
	de_log("Sound thread started!");
	for (i = 0; i < dev->buffer_len; i += 2) {
		short sample = waveSIN(i, 220);
		dev->out_buffer[i] = sample;
		dev->out_buffer[i + 1] = sample;
	}
	while (dev->mixer_status == DE_MIXER_STATUS_ACTIVE) {
		de_mtx_lock(&dev->mtx);
		
		de_mtx_unlock(&dev->mtx);
		/* send_data is locking so mutex is already unlocked here */
		de_sound_device_send_data(dev);
	}
	dev->mixer_status = DE_MIXER_STATUS_STOPPED;
	de_cnd_signal(&dev->cnd);
	de_log("Sound thread stopped!");
	return 0;
}

bool de_sound_device_init(de_core_t* core, de_sound_device_t* dev) {
	de_thrd_t mixer_thread;

	de_zero(dev, sizeof(*dev));
	dev->core = core;
	dev->buffer_len = SW_OUTPUT_DEVICE_SAMPLE_RATE ;
	dev->out_buffer = de_calloc(dev->buffer_len, sizeof(*dev->out_buffer));
	dev->mixer_status = DE_MIXER_STATUS_ACTIVE;
	de_mtx_init(&dev->mtx);
	de_cnd_init(&dev->cnd);

	de_sound_device_setup(dev);

	de_thrd_create(&mixer_thread, de_sound_device_mixer_thread, dev);
	de_thrd_detach(&mixer_thread);

	return true;
}

void de_sound_device_free(de_sound_device_t* dev) {
	dev->mixer_status = DE_MIXER_STATUS_NEED_STOP;
	de_mtx_lock(&dev->mtx);
	while (dev->mixer_status != DE_MIXER_STATUS_STOPPED) {
		de_cnd_wait(&dev->cnd, &dev->mtx);
	}
	de_free(dev->out_buffer);
	de_sound_device_shutdown(dev);
	de_mtx_unlock(&dev->mtx);
	de_mtx_destroy(&dev->mtx);
	de_cnd_destroy(&dev->cnd);
}

void de_sound_device_update(de_sound_device_t* dev) {
	de_sound_source_t* src;
	DE_LINKED_LIST_FOR_EACH(dev->sounds, src) {

	}
}