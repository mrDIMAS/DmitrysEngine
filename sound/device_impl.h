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

static int de_sound_device_mixer_thread(void* ptr) {
	size_t i;
	de_sound_source_t* src;
	de_sound_device_t* dev = (de_sound_device_t*)ptr;
	de_log("Sound thread started!");
	while (dev->mixer_status == DE_MIXER_STATUS_ACTIVE) {
		de_sound_context_lock(dev->ctx);
		for (i = 0; i < dev->buffer_len/2;) {
			float samples[2] = { 0 };
			DE_LINKED_LIST_FOR_EACH(dev->ctx->sounds, src) {
				if (de_sound_source_can_produce_samples(src)) {
					de_sound_source_sample(src, samples);
				}
			}
			if (samples[0] > 1.0f) {
				samples[0] = 1.0f;
			} else if (samples[0] < -1.0f) {
				samples[0] = -1.0f;
			}
			if (samples[1] > 1.0f) {
				samples[1] = 1.0f;
			} else if (samples[1] < -1.0f) {
				samples[1] = -1.0f;
			}
			dev->out_buffer[i++] = (short)(samples[0] * SHRT_MAX);
			dev->out_buffer[i++] = (short)(samples[1] * SHRT_MAX);
		}
		de_sound_context_unlock(dev->ctx);
		/* send_data is locking so mutex is already unlocked here */
		de_sound_device_send_data(dev);
	}
	dev->mixer_status = DE_MIXER_STATUS_STOPPED;
	de_cnd_signal(&dev->cnd);
	de_log("Sound thread stopped!");
	return 0;
}

bool de_sound_device_init(de_sound_context_t* ctx, de_sound_device_t* dev) {
	de_thrd_t mixer_thread;

	de_zero(dev, sizeof(*dev));
	dev->ctx = ctx;
	dev->buffer_len = DE_SOUND_DEVICE_SAMPLE_RATE / sizeof(short);
	dev->out_buffer = de_calloc(dev->buffer_len, sizeof(*dev->out_buffer));
	dev->mixer_status = DE_MIXER_STATUS_ACTIVE;

	de_cnd_init(&dev->cnd);

	de_sound_device_setup(dev);

	de_thrd_create(&mixer_thread, de_sound_device_mixer_thread, dev);
	de_thrd_detach(&mixer_thread);

	return true;
}

void de_sound_device_free(de_sound_device_t* dev) {
	dev->mixer_status = DE_MIXER_STATUS_NEED_STOP;
	de_sound_context_lock(dev->ctx);
	while (dev->mixer_status != DE_MIXER_STATUS_STOPPED) {
		de_cnd_wait(&dev->cnd, &dev->ctx->mtx);
	}
	de_free(dev->out_buffer);
	de_sound_device_shutdown(dev);
	de_sound_context_unlock(dev->ctx);
	de_cnd_destroy(&dev->cnd);
}