#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>

struct thin_chin_data {
	obs_source_t *context;
	gs_effect_t *effect;
	gs_eparam_t *strongly_param;

	gs_eparam_t *lefteyeX_param;
	gs_eparam_t *lefteyeY_param;
	gs_eparam_t *righteyeX_param;
	gs_eparam_t *righteyeY_param;
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
	float strongly;
};

static const char *thin_chin_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("ThinChin");
}

static void thin_chin_update(void *data, obs_data_t *settings)
{
	struct thin_chin_data *filter = data;
	double sharpness = obs_data_get_double(settings, "strongly");
	filter->strongly = (float)sharpness;
}

static void thin_chin_destroy(void *data)
{
	struct thin_chin_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}
static void *thin_chin_create(obs_data_t *settings, obs_source_t *context)
{
	struct thin_chin_data *filter =
		bzalloc(sizeof(struct thin_chin_data));
	char *effect_path = obs_module_file("thin_chin.effect");
	filter->context = context;
	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();
	bfree(effect_path);
	if (!filter->effect) {
		thin_chin_destroy(filter);
		return NULL;
	}
	filter->strongly_param = gs_effect_get_param_by_name(filter->effect, "strongly");
	filter->lefteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeX");
	filter->lefteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeY");
	filter->righteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeX");
	filter->righteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeY");
	filter->point12X_param =gs_effect_get_param_by_name(filter->effect, "gl_point12X");
	filter->point12Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point12Y");
	filter->point20X_param =gs_effect_get_param_by_name(filter->effect, "gl_point20X");
	filter->point20Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point20Y");
	filter->point16X_param =gs_effect_get_param_by_name(filter->effect, "gl_point16X");
	filter->point16Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point16Y");
	filter->point85X_param =gs_effect_get_param_by_name(filter->effect, "gl_point85X");
	filter->point85Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point85Y");
	filter->width_param =gs_effect_get_param_by_name(filter->effect, "gl_width");
	filter->height_param =gs_effect_get_param_by_name(filter->effect, "gl_height");
	thin_chin_update(filter, settings);
	return filter;
}

static void thin_chin_render(void *data, gs_effect_t *effect)
{
	/*struct eyes_reshape_filter_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;*/

	/*gs_effect_set_float(filter->strongly_param, filter->strongly);*/

	/*obs_source_process_filter_end(filter->context, filter->effect, 0, 0);*/

	//UNUSED_PARAMETER(effect);
}

static void thin_chin_face_render(void *data, float *facepoint,
				  gs_effect_t *effect)
{
	struct thin_chin_data *filter = data;
	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;
	float width = facepoint[356];
	float height = facepoint[389];
	float lefteyeX = facepoint[159*2];
	float lefteyeY = (facepoint[159 * 2 + 1] + facepoint[454 * 2 + 1]) / 2;
	float righteyeX = facepoint[386*2];
	float righteyeY = (facepoint[386*2 + 1] + facepoint[374*2 + 1])/2;
	float point12X = facepoint[169*2];
	float point12Y = facepoint[169*2+1];
	float point20X = facepoint[394*2];
	float point20Y = facepoint[394*2+1];
	float point16X = facepoint[175*2];
	float point16Y = facepoint[175*2+1];
	float point85X = facepoint[17*2];
	float point85Y = facepoint[17*2+1];
	
	gs_effect_set_float(filter->width_param, width);
	gs_effect_set_float(filter->height_param, height);
	gs_effect_set_float(filter->strongly_param, filter->strongly);
	gs_effect_set_float(filter->lefteyeX_param, lefteyeX);
	gs_effect_set_float(filter->lefteyeY_param, lefteyeY);
	gs_effect_set_float(filter->righteyeX_param, righteyeX);
	gs_effect_set_float(filter->righteyeY_param, righteyeY);
	gs_effect_set_float(filter->point12X_param, point12X);
	gs_effect_set_float(filter->point12Y_param, point12Y);
	gs_effect_set_float(filter->point20X_param, point20X);
	gs_effect_set_float(filter->point20Y_param, point20Y);
	gs_effect_set_float(filter->point16X_param, point16X);
	gs_effect_set_float(filter->point16Y_param, point16Y);
	gs_effect_set_float(filter->point85X_param, point85X);
	gs_effect_set_float(filter->point85Y_param, point85Y);

	//gs_effect_set_val(filter->facepoint_param, &ff,sizeof(float)*4);
	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	UNUSED_PARAMETER(effect);
}
static obs_properties_t *thin_chin_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "strongly",
					obs_module_text("Strongly"), 0.0, 1.0,
					0.01);

	UNUSED_PARAMETER(data);
	return props;
}

static void thin_chin_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "strongly", 0.08);
}

struct obs_source_info thin_chin_filter = {
	.id = "thin_chin",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = thin_chin_getname,
	.create = thin_chin_create,
	.destroy = thin_chin_destroy,
	.update = thin_chin_update,
	.video_render = thin_chin_render,
	.get_properties = thin_chin_properties,
	.get_defaults = thin_chin_defaults,
	.face_config = true,
	.video_face_render = thin_chin_face_render,
};
