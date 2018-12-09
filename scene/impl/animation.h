/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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


/*=======================================================================================*/
de_animation_track_t* de_animation_track_create(de_animation_t* anim)
{
	de_animation_track_t* track;

	track = DE_NEW(de_animation_track_t);
	track->parent_animation = anim;

	return track;
}

/*=======================================================================================*/
void de_animation_track_add_keyframe(de_animation_track_t* track, const de_keyframe_t* keyframe)
{
	size_t i;

	if (keyframe->time > track->max_time)
	{
		DE_ARRAY_APPEND(track->keyframes, *keyframe);

		track->max_time = keyframe->time;
	}
	else
	{
		for (i = 0; i < track->keyframes.size; ++i)
		{
			de_keyframe_t* other_keyframe = DE_ARRAY_AT(&track->keyframes, i);

			if (keyframe->time < other_keyframe->time)
			{
				break;
			}
		}

		/*		de_array_insert(track->keyframes, keyframe, i);*/
	}
}

/*=======================================================================================*/
void de_animation_track_get_keyframe(de_animation_track_t* track, float time, de_keyframe_t* out_keyframe)
{
	size_t i;
	de_keyframe_t* left = NULL;
	de_keyframe_t* right = NULL;
	float interpolator = 0.0f;

	time = de_clamp(time, 0.0f, track->max_time);

	if (time >= track->max_time)
	{
		left = &DE_ARRAY_LAST(track->keyframes);
		right = left;

		interpolator = 0.0f;
	}
	else
	{
		int right_index = -1;

		for (i = 0; i < track->keyframes.size; ++i)
		{
			de_keyframe_t* keyframe = &DE_ARRAY_AT(track->keyframes, i);

			if (keyframe->time >= time)
			{
				right_index = i;
				break;
			}
		}

		if (right_index == 0)
		{
			left = &DE_ARRAY_FIRST(track->keyframes);
			right = left;

			interpolator = 0.0f;
		}
		else
		{
			left = &DE_ARRAY_AT(track->keyframes, right_index - 1);
			right = &DE_ARRAY_AT(track->keyframes, right_index);

			interpolator = (time - left->time) / (right->time - left->time);
		}
	}

	if (left && right)
	{
		if (interpolator == 0.0f)
		{
			*out_keyframe = *left;
		}
		else
		{
			out_keyframe->time = de_lerp(left->time, right->time, interpolator);
			de_vec3_lerp(&out_keyframe->position, &left->position, &right->position, interpolator);
			de_vec3_lerp(&out_keyframe->scale, &left->scale, &right->scale, interpolator);
			de_quat_slerp(&out_keyframe->rotation, &left->rotation, &right->rotation, interpolator);
		}
	}
}
