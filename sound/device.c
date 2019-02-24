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
#include "sound/device_dsound.c"
#else
#include "sound/device_alsa.c"
#endif

static int de_sound_device_mixer_thread(void* ptr) {
	size_t i, k;
	de_sound_device_t* dev = (de_sound_device_t*)ptr;
	de_log("Sound thread started!");
	while (dev->mixer_status == DE_MIXER_STATUS_ACTIVE) {
		de_sound_context_lock(dev->ctx);
		/* gather all active sounds */
		DE_ARRAY_CLEAR(dev->active_sources);
		for (k = 0; k < dev->ctx->sounds.size; ++k) {
			de_sound_source_t* src = dev->ctx->sounds.data[k];
			if (de_sound_source_can_produce_samples(src)) {
				DE_ARRAY_APPEND(dev->active_sources, src);
			}
		}
		/* mix them */
		for (i = 0; i < dev->out_samples_count;) {
			float left = 0, right = 0;
			for(k = 0; k < dev->active_sources.size; ++k) {
				float sleft, sright;
				de_sound_source_sample(dev->active_sources.data[k], &sleft, &sright);				
				left += sleft;
				right += sright;
			}
			/* clamp to [-1;1] to prevent clicks */
			if (left > 1.0f) {
				left = 1.0f;
			} else if (left < -1.0f) {
				left = -1.0f;
			}
			if (right > 1.0f) {
				right = 1.0f;
			} else if (right < -1.0f) {
				right = -1.0f ;
			}	
			dev->out_buffer[i++] = (short)(left * (float)INT16_MAX);
			dev->out_buffer[i++] = (short)(right * (float)INT16_MAX);
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
	dev->sample_rate = 44100;	
	dev->buffer_len_bytes = dev->sample_rate / 5;
	dev->out_samples_count = dev->buffer_len_bytes / sizeof(int16_t);
	if ((dev->out_samples_count % 2) != 0) {
		de_fatal_error("sample count must be even!");
	}
	de_log("sound device feed rate: %d Hz\noutput buffer len: %f ms", dev->sample_rate / dev->out_samples_count, 1000.0f * (dev->out_samples_count / (float)dev->sample_rate));
	dev->out_buffer = de_malloc(dev->buffer_len_bytes);
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