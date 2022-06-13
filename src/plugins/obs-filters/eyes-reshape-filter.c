#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>
//大眼
struct eyes_reshape_filter_data {
	obs_source_t *context;
	gs_effect_t *effect;
	gs_eparam_t *strongly_param;
	gs_eparam_t *lefteyeX_param;
	gs_eparam_t *lefteyeY_param;
	gs_eparam_t *righteyeX_param;
	gs_eparam_t *righteyeY_param;
	gs_eparam_t *width_param;
	gs_eparam_t *height_param;
	float strongly;
};

static const char *eyes_reshape_filter_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("EyesReshapeFilter");
}

static void eyes_reshape_filter_update(void *data, obs_data_t *settings)
{
	struct eyes_reshape_filter_data *filter = data;
	double sharpness = obs_data_get_double(settings, "strongly");
	filter->strongly = (float)sharpness;
}

static void eyes_reshape_filter_destroy(void *data)
{
	struct eyes_reshape_filter_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}
static void *eyes_reshape_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct eyes_reshape_filter_data *filter =
		bzalloc(sizeof(struct eyes_reshape_filter_data));
	char *effect_path = obs_module_file("bigeye.effect");
	filter->context = context;
	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();
	bfree(effect_path);
	if (!filter->effect) {
		eyes_reshape_filter_destroy(filter);
		return NULL;
	}
	filter->strongly_param = gs_effect_get_param_by_name(filter->effect, "strongly");
	filter->lefteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeX");
	filter->lefteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeY");
	filter->righteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeX");
	filter->righteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeY");
	filter->width_param =gs_effect_get_param_by_name(filter->effect, "gl_width");
	filter->height_param =gs_effect_get_param_by_name(filter->effect, "gl_height");
	filter->width_param =gs_effect_get_param_by_name(filter->effect, "gl_width");
	filter->height_param =gs_effect_get_param_by_name(filter->effect, "gl_height");
	eyes_reshape_filter_update(filter, settings);
	return filter;
}

static void eyes_reshape_filter_render(void *data, gs_effect_t *effect)
{
	/*struct eyes_reshape_filter_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;*/

	/*gs_effect_set_float(filter->strongly_param, filter->strongly);*/

	/*obs_source_process_filter_end(filter->context, filter->effect, 0, 0);*/

	//UNUSED_PARAMETER(effect);
}
//struct vec192 {
//	union {
//		float ptr[4];
//		__m128 m;
//
//	};
//};
static void eyes_reshape_filter_face_render(void *data, float *facepoint,
				  gs_effect_t *effect)
{
	struct eyes_reshape_filter_data *filter = data;
	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;
	//facepoint[3] = 0.5;
	/*struct vec192 ff;*/
	/*ff.ptr[0] = 0.1;
	ff.ptr[1] = 0.2;*/
	float width = facepoint[356];
	float height = facepoint[389];
	//float lefteyeX = facepoint[62*2];
	float lefteyeX = facepoint[159*2];
	//float lefteyeY = (facepoint[62 * 2 + 1] + facepoint[66 * 2 + 1]) / 2;
	float lefteyeY = (facepoint[159 * 2 + 1] + facepoint[145 * 2 + 1]) / 2;
	float righteyeX = facepoint[386*2];
	float righteyeY = (facepoint[386 * 2 + 1] + facepoint[374*2 + 1])/2;

	gs_effect_set_float(filter->width_param, width);
	gs_effect_set_float(filter->height_param, height);
	gs_effect_set_float(filter->strongly_param, filter->strongly);
	gs_effect_set_float(filter->lefteyeX_param, lefteyeX);
	gs_effect_set_float(filter->lefteyeY_param, lefteyeY);
	gs_effect_set_float(filter->righteyeX_param, righteyeX);
	gs_effect_set_float(filter->righteyeY_param, righteyeY);
	//gs_effect_set_float(filter->width_param, width);
	//gs_effect_set_float(filter->height_param, height);
	//gs_effect_set_val(filter->facepoint_param, &ff,sizeof(float)*4);
	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	UNUSED_PARAMETER(effect);
}
static obs_properties_t *eyes_reshape_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "strongly",
					obs_module_text("Strongly"), 0.0, 1.0,
					0.01);

	UNUSED_PARAMETER(data);
	return props;
}

static void eyes_reshape_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "strongly", 0.08);
}

struct obs_source_info eyes_reshape_filter = {
	.id = "eyes_reshape_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = eyes_reshape_filter_getname,
	.create = eyes_reshape_filter_create,
	.destroy = eyes_reshape_filter_destroy,
	.update = eyes_reshape_filter_update,
	.video_render = eyes_reshape_filter_render,
	.get_properties = eyes_reshape_filter_properties,
	.get_defaults = eyes_reshape_filter_defaults,
	.face_config = true,
	.video_face_render = eyes_reshape_filter_face_render,
};
