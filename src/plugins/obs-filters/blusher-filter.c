#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>

struct blusher_data {
	obs_source_t *context;
	gs_effect_t *effect;
	gs_eparam_t *strongly_param;
	gs_eparam_t *lefteyeX_param;
	gs_eparam_t *lefteyeY_param;
	gs_eparam_t *righteyeX_param;
	gs_eparam_t *righteyeY_param;
	gs_eparam_t *point2X_param;
	gs_eparam_t *point2Y_param;
	gs_eparam_t *point53X_param;
	gs_eparam_t *point53Y_param;
	gs_eparam_t *point30X_param;
	gs_eparam_t *point30Y_param;
	gs_eparam_t *width_param;
	gs_eparam_t *height_param;


	float strongly;
};

static const char *blusher_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("Blusher");
}

static void blusher_update(void *data, obs_data_t *settings)
{
	struct blusher_data *filter = data;
	double sharpness = obs_data_get_double(settings, "strongly");
	filter->strongly = (float)sharpness;
}

static void blusher_destroy(void *data)
{
	struct blusher_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}
static void *blusher_create(obs_data_t *settings, obs_source_t *context)
{
	struct blusher_data *filter =
		bzalloc(sizeof(struct blusher_data));
	char *effect_path = obs_module_file("blusher.effect");
	filter->context = context;
	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();
	bfree(effect_path);
	if (!filter->effect) {
		blusher_destroy(filter);
		return NULL;
	}
	
	filter->strongly_param = gs_effect_get_param_by_name(filter->effect, "strongly");
	filter->lefteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeX");
	filter->lefteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_lefteyeY");
	filter->righteyeX_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeX");
	filter->righteyeY_param =gs_effect_get_param_by_name(filter->effect, "gl_righteyeY");

	filter->point2X_param =gs_effect_get_param_by_name(filter->effect, "gl_point2X");
	filter->point2Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point2Y");
	filter->point53X_param =gs_effect_get_param_by_name(filter->effect, "gl_point53X");
	filter->point53Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point53Y");
	filter->point30X_param =gs_effect_get_param_by_name(filter->effect, "gl_point30X");
	filter->point30Y_param =gs_effect_get_param_by_name(filter->effect, "gl_point30Y");
	filter->width_param =gs_effect_get_param_by_name(filter->effect, "gl_width");
	filter->height_param =gs_effect_get_param_by_name(filter->effect, "gl_height");


	blusher_update(filter, settings);
	return filter;
}

static void blusher_render(void *data, gs_effect_t *effect)
{

}

static void blusher_face_render(void *data, float *facepoint,
				  gs_effect_t *effect)
{
	struct blusher_data *filter = data;
	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;
	float width = facepoint[356];
	float height = facepoint[389];
	float lefteyeX = facepoint[159*2];
	float lefteyeY = (facepoint[159 * 2 + 1] + facepoint[145 * 2 + 1]) / 2;
	float righteyeX = facepoint[386*2];
	float righteyeY = (facepoint[386*2 + 1] + facepoint[374*2 + 1])/2;
	float point2X = facepoint[101*2];
	float point2Y = facepoint[101*2+1];
	float point53X = facepoint[5*2];
	float point53Y = facepoint[5*2+1];
	float point30X = facepoint[330*2];
	float point30Y = facepoint[330*2+1];
	
	gs_effect_set_float(filter->strongly_param, filter->strongly);
	gs_effect_set_float(filter->lefteyeX_param, lefteyeX);
	gs_effect_set_float(filter->lefteyeY_param, lefteyeY);
	gs_effect_set_float(filter->righteyeX_param, righteyeX);
	gs_effect_set_float(filter->righteyeY_param, righteyeY);
	gs_effect_set_float(filter->point2X_param, point2X);
	gs_effect_set_float(filter->point2Y_param, point2Y);
	gs_effect_set_float(filter->point53X_param, point53X);
	gs_effect_set_float(filter->point53Y_param, point53Y);
	gs_effect_set_float(filter->point30X_param, point30X);
	gs_effect_set_float(filter->point30Y_param, point30Y);
	gs_effect_set_float(filter->width_param, width);
	gs_effect_set_float(filter->height_param, height);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	UNUSED_PARAMETER(effect);
}
static obs_properties_t *blusher_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "strongly",
					obs_module_text("Strongly"), 0.0, 0.5,
					0.01);

	UNUSED_PARAMETER(data);
	return props;
}

static void blusher_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "strongly", 0.08);
}

struct obs_source_info blusher_filter = {
	.id = "blusher",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = blusher_getname,
	.create = blusher_create,
	.destroy = blusher_destroy,
	.update = blusher_update,
	.video_render = blusher_render,
	.get_properties = blusher_properties,
	.get_defaults = blusher_defaults,
	.face_config = true,
	.video_face_render = blusher_face_render,
};
