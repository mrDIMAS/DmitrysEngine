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

de_animation_t* de_animation_create(de_scene_t* s) {
	de_animation_t* animation;

	animation = DE_NEW(de_animation_t);

	animation->scene = s;
	animation->weight = 1.0f;
	animation->speed = 1.0f;
	animation->flags = (de_animation_flags_t)(DE_ANIMATION_FLAG_ENABLED | DE_ANIMATION_FLAG_LOOPED);

	DE_LINKED_LIST_APPEND(s->animations, animation);

	return animation;
}

de_animation_track_t* de_animation_track_create(de_animation_t* anim) {
	de_animation_track_t* track;

	track = DE_NEW(de_animation_track_t);
	track->parent_animation = anim;

	return track;
}

bool de_animation_visit(de_object_visitor_t* visitor, de_animation_t* anim) {
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Resource", &anim->resource, de_resource_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Scene", &anim->scene, de_scene_visit);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Tracks", anim->tracks, de_animation_track_visit);
	result &= de_object_visitor_visit_uint32(visitor, "Flags", &anim->flags);
	result &= de_object_visitor_visit_float(visitor, "Speed", &anim->speed);
	result &= de_object_visitor_visit_float(visitor, "Length", &anim->length);
	result &= de_object_visitor_visit_float(visitor, "TimePosition", &anim->time_position);
	result &= de_object_visitor_visit_float(visitor, "Weight", &anim->weight);
	result &= de_object_visitor_visit_float(visitor, "FadeStep", &anim->fade_step);		
	return result;
}

static bool de_keyframe_visit(de_object_visitor_t* visitor, de_keyframe_t* key) {
	bool result = true;
	result &= de_object_visitor_visit_vec3(visitor, "Position", &key->position);
	result &= de_object_visitor_visit_vec3(visitor, "Scale", &key->scale);
	result &= de_object_visitor_visit_quat(visitor, "Rotation", &key->rotation);
	result &= de_object_visitor_visit_float(visitor, "Time", &key->time);	
	return result;
}

bool de_animation_track_visit(de_object_visitor_t* visitor, de_animation_track_t* track) {
	bool result = true;
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Animation", &track->parent_animation, de_animation_visit);
	if (track->parent_animation && !track->parent_animation->resource) {
		/* visit keyframes only if this animation was created during runtime, not from external resource */
		result &= DE_OBJECT_VISITOR_VISIT_ARRAY(visitor, "Keyframes", track->keyframes, (de_visit_callback_t)de_keyframe_visit);	
	}	
	result &= de_object_visitor_visit_bool(visitor, "Enabled", &track->enabled);
	result &= de_object_visitor_visit_float(visitor, "MaxTime", &track->max_time);
	result &= DE_OBJECT_VISITOR_VISIT_POINTER(visitor, "Node", &track->node, de_node_visit);	
	return result;
}

void de_animation_resolve(de_animation_t* anim) {
	/* no need to resolve resource dependencies if there is no resource */
	/* TODO: support more resource types */
	if (anim->resource && anim->resource->type == DE_RESOURCE_TYPE_MODEL) { 
		de_model_t* model = de_resource_to_model(anim->resource);
		for (size_t i = 0; i < anim->tracks.size; ++i) {
			de_animation_track_t* track = anim->tracks.data[i];
			DE_LINKED_LIST_FOR_EACH_T(de_animation_t*, ref_anim, model->scene->animations) {
				for (size_t k = 0; k < ref_anim->tracks.size; ++k) {
					de_animation_track_t* ref_track = ref_anim->tracks.data[k];
					if (ref_track->node && track->node && track->node->original == ref_track->node) {
						/* copy keyframes from ref track */
						DE_ARRAY_COPY(ref_track->keyframes, track->keyframes);
					} else {
						de_log("unable to resolve track resource dependencies");
					}
				}
			}
		}
	}
}

void de_animation_track_free(de_animation_track_t* track) {
	DE_ARRAY_FREE(track->keyframes);

	de_free(track);
}

void de_animation_track_set_node(de_animation_track_t* track, de_node_t* node) {	
	track->node = node;
}

de_animation_track_t* de_animation_track_copy(de_animation_track_t* track, de_animation_t* dest_anim) {
	de_animation_track_t* copy = DE_NEW(de_animation_track_t);
	copy->parent_animation = dest_anim;
	for (size_t i = 0; i < track->keyframes.size; ++i) {
		DE_ARRAY_APPEND(copy->keyframes, track->keyframes.data[i]);
	}
	copy->enabled = track->enabled;
	copy->max_time = track->max_time;	
	/* Track copy will point on same node for further remapping. */
	de_animation_track_set_node(copy, track->node);	
	return copy;
}

void de_animation_track_add_keyframe(de_animation_track_t* track, const de_keyframe_t* keyframe) {
	size_t i;

	if (keyframe->time > track->max_time) {
		DE_ARRAY_APPEND(track->keyframes, *keyframe);

		track->max_time = keyframe->time;
	} else {
		for (i = 0; i < track->keyframes.size; ++i) {
			de_keyframe_t* other_keyframe = DE_ARRAY_AT(&track->keyframes, i);

			if (keyframe->time < other_keyframe->time) {
				break;
			}
		}

		DE_ARRAY_INSERT(track->keyframes, i, *keyframe);
	}
}

void de_animation_free(de_animation_t* anim) {
	size_t i;

	DE_LINKED_LIST_REMOVE(anim->scene->animations, anim);

	for (i = 0; i < anim->tracks.size; ++i) {
		de_animation_track_free(anim->tracks.data[i]);
	}
	DE_ARRAY_FREE(anim->tracks);

	if (anim->resource) {
		de_resource_release(anim->resource);
	}

	de_free(anim);
}

de_animation_t* de_animation_copy(de_animation_t* anim, de_scene_t* dest_scene) {
	de_animation_t* copy = DE_NEW(de_animation_t);
	copy->scene = dest_scene;
	for (size_t i = 0; i < anim->tracks.size; ++i) {
		DE_ARRAY_APPEND(copy->tracks, de_animation_track_copy(anim->tracks.data[i], copy));
	}
	copy->resource = anim->resource;
	if (copy->resource) {
		de_resource_add_ref(copy->resource);
	}
	copy->flags = anim->flags;
	copy->speed = anim->speed;
	copy->length = anim->length;
	copy->time_position = anim->time_position;
	copy->weight = anim->weight;
	copy->fade_step = anim->fade_step;
	DE_LINKED_LIST_APPEND(dest_scene->animations, copy);
	return copy;
}

void de_animation_track_get_keyframe(de_animation_track_t* track, float time, de_keyframe_t* out_keyframe) {
	size_t i;
	de_keyframe_t* left = NULL;
	de_keyframe_t* right = NULL;
	float interpolator = 0.0f;

	time = de_clamp(time, 0.0f, track->max_time);

	if (time >= track->max_time) {
		left = &DE_ARRAY_LAST(track->keyframes);
		right = left;

		interpolator = 0.0f;
	} else {
		int right_index = -1;

		for (i = 0; i < track->keyframes.size; ++i) {
			de_keyframe_t* keyframe = &DE_ARRAY_AT(track->keyframes, i);

			if (keyframe->time >= time) {
				right_index = i;
				break;
			}
		}

		if (right_index == 0) {
			left = &DE_ARRAY_FIRST(track->keyframes);
			right = left;

			interpolator = 0.0f;
		} else {
			left = &DE_ARRAY_AT(track->keyframes, right_index - 1);
			right = &DE_ARRAY_AT(track->keyframes, right_index);

			interpolator = (time - left->time) / (right->time - left->time);
		}
	}

	if (left && right) {
		if (interpolator == 0.0f) {
			*out_keyframe = *left;
		} else {
			out_keyframe->time = de_lerp(left->time, right->time, interpolator);
			de_vec3_lerp(&out_keyframe->position, &left->position, &right->position, interpolator);
			de_vec3_lerp(&out_keyframe->scale, &left->scale, &right->scale, interpolator);
			de_quat_slerp(&out_keyframe->rotation, &left->rotation, &right->rotation, interpolator);
		}
	}
}

void de_animation_add_track(de_animation_t* anim, de_animation_track_t* track) {
	DE_ARRAY_APPEND(anim->tracks, track);
}

void de_animation_update(de_animation_t* anim, float dt) {
	size_t i;
	float nextTimePos = anim->time_position + dt * anim->speed;

	for (i = 0; i < anim->tracks.size; ++i) {
		de_animation_track_t* track;
		de_keyframe_t keyframe;
		de_node_t* node;

		track = anim->tracks.data[i];
				
		node = track->node;

		if (!node) {
			continue;
		}

		de_animation_track_get_keyframe(track, anim->time_position, &keyframe);

		/* Accumulate position */
		de_vec3_add(&node->position, &node->position, &keyframe.position);

		/* Accumulate rotation */
		de_quat_mul(&node->rotation, &node->rotation, &keyframe.rotation);

		/* Accumulate scale */
		node->scale.x *= keyframe.scale.x;
		node->scale.y *= keyframe.scale.y;
		node->scale.z *= keyframe.scale.z;
	}

	de_animation_set_time_position(anim, nextTimePos);

	/* Handle fading - part of animation blending */
	if (anim->fade_step != 0.0f) {
		anim->weight += anim->fade_step * dt;
		if (anim->fade_step < 0 && anim->weight <= 0) {
			anim->weight = 0;
			de_animation_reset_flags(anim, DE_ANIMATION_FLAG_ENABLED);
			anim->fade_step = 0;
		} else if (anim->fade_step > 0 && anim->weight >= 1) {
			anim->weight = 1;
			anim->fade_step = 0;
		}
	}
}

void de_animation_set_time_position(de_animation_t* anim, float time) {
	if (anim->flags & DE_ANIMATION_FLAG_LOOPED) {
		anim->time_position = de_fwrap(time, 0.0f, anim->length);
	} else {
		anim->time_position = de_clamp(time, 0.0f, anim->length);
	}
}

bool de_animation_is_flags_set(de_animation_t* anim, uint32_t flags) {
	return (anim->flags & flags) == flags;
}

void de_animation_set_flags(de_animation_t* anim, uint32_t flags) {
	anim->flags |= flags;
}

void de_animation_reset_flags(de_animation_t* anim, uint32_t flags) {
	anim->flags &= ~flags;
}

void de_animation_clamp_length(de_animation_t* anim) {
	size_t i;

	for (i = 0; i < anim->tracks.size; ++i) {
		de_animation_track_t* track = anim->tracks.data[i];

		if (track->max_time > anim->length) {
			anim->length = track->max_time;
		}
	}
}

de_animation_t* de_animation_extract(de_animation_t* anim, float from, float to) {
	size_t i;
    de_animation_t* new_anim;

	from = (float)fabs(from);
	to = (float)fabs(to);

	if (from > to) {
		float temp = from;
		from = to;
		to = temp;
	}

	new_anim = de_animation_create(anim->scene);

	for (i = 0; i < anim->tracks.size; ++i) {
		/* TODO */
	}

	return new_anim;
}