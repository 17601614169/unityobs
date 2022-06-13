#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>
//瘦脸

struct face_lift_filter_data {
	obs_source_t *context;
	gs_effect_t *effect;
	gs_eparam_t *strongly_eye_param;
	gs_eparam_t *strongly_face_param;
	gs_eparam_t *strongly_thin_param;
	gs_eparam_t *lefteyeX_param;
	gs_eparam_t *lefteyeY_param;
	gs_eparam_t *righteyeX_param;
	gs_eparam_t *righteyeY_param;
	gs_eparam_t *point0X_param;
	gs_eparam_t *point0Y_param;
	gs_eparam_t *point51X_param;
	gs_eparam_t *point51Y_param;
	gs_eparam_t *point32X_param;
	gs_eparam_t *point32Y_param;
	gs_eparam_t *point4X_param;
	gs_eparam_t *point4Y_param;
	gs_eparam_t *point54X_param;
	gs_eparam_t *point54Y_param;
	gs_eparam_t *point28X_param;
	gs_eparam_t *point28Y_param;
	gs_eparam_t *point10X_param;
	gs_eparam_t *point10Y_param;
	gs_eparam_t *point22X_param;
	gs_eparam_t *point22Y_param;
	gs_eparam_t *point57X_param;
	gs_eparam_t *point57Y_param;
	gs_eparam_t *point12X_param;
	gs_eparam_t *point12Y_param;
	gs_eparam_t *point20X_param;
	gs_eparam_t *point20Y_param;
	gs_eparam_t *point16X_param;
	gs_eparam_t *point16Y_param;
	gs_eparam_t *point85X_param;
	gs_eparam_t *point85Y_param;
	gs_eparam_t *width_param;
	gs_eparam_t *height_param;
	float eye_strongly;
	float face_strongly;
	float thin_strongly;
};

static const char *face_lift_filter_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("FaceLiftFilter");
}

static void face_lift_filter_update(void *data, obs_data_t *settings)
{

	struct face_lift_filter_data *filter = data;
	double sharpness_eye = obs_data_get_double(settings, "eye_strongly");
	double sharpness_face = obs_data_get_double(settings, "face_strongly");
	double sharpness_thin = obs_data_get_double(settings, "thin_strongly");
	filter->eye_strongly = (float)sharpness_eye;
	filter->face_strongly = (float)sharpness_face;
	filter->thin_strongly = (float)sharpness_thin;
}

static void face_lift_filter_destroy(void *data)
{
	struct face_lift_filter_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}
static void *face_lift_filter_create(obs_data_t *settings,
				     obs_source_t *context)
{
	struct face_lift_filter_data *filter =
		bzalloc(sizeof(struct face_lift_filter_data));
	char *effect_path = obs_module_file("face_lift.effect");
	filter->context = context;
	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();
	bfree(effect_path);
	if (!filter->effect) {
		face_lift_filter_destroy(filter);
		return NULL;
	}
	filter->lefteyeX_param =
		gs_effect_get_param_by_name(filter->effect, "gl_lefteyeX");
	filter->lefteyeY_param =
		gs_effect_get_param_by_name(filter->effect, "gl_lefteyeY");
	filter->righteyeX_param =
		gs_effect_get_param_by_name(filter->effect, "gl_righteyeX");
	filter->righteyeY_param =
		gs_effect_get_param_by_name(filter->effect, "gl_righteyeY");
	filter->strongly_eye_param =
		gs_effect_get_param_by_name(filter->effect, "eye_strongly");
	filter->strongly_face_param =
		gs_effect_get_param_by_name(filter->effect, "face_strongly");
	filter->strongly_thin_param =
		gs_effect_get_param_by_name(filter->effect, "thin_strongly");
	filter->point0X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point0X");
	filter->point0Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point0Y");
	filter->point51X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point51X");
	filter->point51Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point51Y");
	filter->point32X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point32X");
	filter->point32Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point32Y");
	filter->point4X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point4X");
	filter->point4Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point4Y");
	filter->point28X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point28X");
	filter->point28Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point28Y");
	filter->point54X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point54X");
	filter->point54Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point54Y");
	filter->point10X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point10X");
	filter->point10Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point10Y");
	filter->point22X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point22X");
	filter->point22Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point22Y");
	filter->point57X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point57X");
	filter->point57Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point57Y");
	filter->point12X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point12X");
	filter->point12Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point12Y");
	filter->point20X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point20X");
	filter->point20Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point20Y");
	filter->point16X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point16X");
	filter->point16Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point16Y");
	filter->point85X_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point85X");
	filter->point85Y_param =
		gs_effect_get_param_by_name(filter->effect, "gl_point85Y");
	filter->width_param =
		gs_effect_get_param_by_name(filter->effect, "gl_width");
	filter->height_param =
		gs_effect_get_param_by_name(filter->effect, "gl_height");
	face_lift_filter_update(filter, settings);
	return filter;
}

static void face_lift_filter_render(void *data, gs_effect_t *effect) {}

static void face_lift_filter_face_render(void *data, float *facepoint,
					 gs_effect_t *effect)
{
	struct face_lift_filter_data *filter = data;
	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;
	float width = facepoint[356];
	float height = facepoint[389];
	float point12X = facepoint[169 * 2];
	float point12Y = facepoint[169 * 2 + 1];
	float point20X = facepoint[394 * 2];
	float point20Y = facepoint[394 * 2 + 1];
	float point16X = facepoint[175 * 2];
	float point16Y = facepoint[175 * 2 + 1];
	float point85X = facepoint[85 * 2];
	float point85Y = facepoint[85 * 2 + 1];
	float point0X = facepoint[177 * 2];
	float point0Y = facepoint[177 * 2 + 1];
	float point51X = facepoint[168 * 2];
	float point51Y = facepoint[168 * 2 + 1];
	float point32X = facepoint[401 * 2];
	float point32Y = facepoint[401 * 2 + 1];
	float point4X = facepoint[215 * 2];
	float point4Y = facepoint[215 * 2 + 1];
	float point28X = facepoint[435 * 2];
	float point28Y = facepoint[435 * 2 + 1];
	float point54X = facepoint[0 * 2];
	float point54Y = facepoint[0 * 2 + 1];
	float point10X = facepoint[169 * 2];
	float point10Y = facepoint[169 * 2 + 1];
	float point22X = facepoint[394 * 2];
	float point22Y = facepoint[394 * 2 + 1];
	float point57X = facepoint[2 * 2];
	float point57Y = facepoint[2 * 2 + 1];

	float lefteyeX = facepoint[159 * 2];
	float lefteyeY = (facepoint[159 * 2 + 1] + facepoint[145 * 2 + 1]) / 2;
	float righteyeX = facepoint[386 * 2];
	float righteyeY = (facepoint[386 * 2 + 1] + facepoint[374 * 2 + 1]) / 2;

	gs_effect_set_float(filter->width_param, width);
	gs_effect_set_float(filter->height_param, height);
	gs_effect_set_float(filter->lefteyeX_param, lefteyeX);
	gs_effect_set_float(filter->lefteyeY_param, lefteyeY);
	gs_effect_set_float(filter->righteyeX_param, righteyeX);
	gs_effect_set_float(filter->righteyeY_param, righteyeY);
	gs_effect_set_float(filter->strongly_eye_param, filter->eye_strongly);
	gs_effect_set_float(filter->strongly_face_param, filter->face_strongly);
	gs_effect_set_float(filter->strongly_thin_param, filter->thin_strongly);
	gs_effect_set_float(filter->point12X_param, point12X);
	gs_effect_set_float(filter->point12Y_param, point12Y);
	gs_effect_set_float(filter->point20X_param, point20X);
	gs_effect_set_float(filter->point20Y_param, point20Y);
	gs_effect_set_float(filter->point16X_param, point16X);
	gs_effect_set_float(filter->point16Y_param, point16Y);
	gs_effect_set_float(filter->point85X_param, point85X);
	gs_effect_set_float(filter->point85Y_param, point85Y);
	gs_effect_set_float(filter->point0X_param, point0X);
	gs_effect_set_float(filter->point0Y_param, point0Y);
	gs_effect_set_float(filter->point51X_param, point51X);
	gs_effect_set_float(filter->point51Y_param, point51Y);
	gs_effect_set_float(filter->point32X_param, point32X);
	gs_effect_set_float(filter->point32Y_param, point32Y);
	gs_effect_set_float(filter->point4X_param, point4X);
	gs_effect_set_float(filter->point4Y_param, point4Y);
	gs_effect_set_float(filter->point28X_param, point28X);
	gs_effect_set_float(filter->point28Y_param, point28Y);
	gs_effect_set_float(filter->point54X_param, point54X);
	gs_effect_set_float(filter->point54Y_param, point54Y);
	gs_effect_set_float(filter->point10X_param, point10X);
	gs_effect_set_float(filter->point10Y_param, point10Y);
	gs_effect_set_float(filter->point22X_param, point22X);
	gs_effect_set_float(filter->point22Y_param, point22Y);
	gs_effect_set_float(filter->point57X_param, point57X);
	gs_effect_set_float(filter->point57Y_param, point57Y);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	UNUSED_PARAMETER(effect);
}
static obs_properties_t *face_lift_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "eye_strongly",
					obs_module_text("Eye_Strongly"), 0.0,
					1.0, 0.01);
	obs_properties_add_float_slider(props, "face_strongly",
					obs_module_text("Face_Strongly"), 0.0,
					0.6, 0.01);
	obs_properties_add_float_slider(props, "thin_strongly",
					obs_module_text("Thin_Strongly"), 0.0,
					1.0, 0.01);
	UNUSED_PARAMETER(data);
	return props;
}

static void face_lift_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "eye_strongly", 0.08);
	obs_data_set_default_double(settings, "face_strongly", 0.08);
	obs_data_set_default_double(settings, "thin_strongly", 0.08);
}

struct obs_source_info face_lift_filter = {
	.id = "face_lift_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = face_lift_filter_getname,
	.create = face_lift_filter_create,
	.destroy = face_lift_filter_destroy,
	.update = face_lift_filter_update,
	.video_render = face_lift_filter_render,
	.get_properties = face_lift_filter_properties,
	.get_defaults = face_lift_filter_defaults,
	.face_config = true,
	.video_face_render = face_lift_filter_face_render,
};
