#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>

struct ruddy_filter_data {
	obs_source_t *context;
	gs_effect_t *effect;

	gs_eparam_t *strongly_param;

	float strongly;
};

static const char *ruddy_filter_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("RuddyFilter");
}

static void ruddy_filter_update(void *data, obs_data_t *settings)
{
	struct ruddy_filter_data *filter = data;
	double sharpness = obs_data_get_double(settings, "strongly");
	filter->strongly = (float)sharpness;
}

static void ruddy_filter_destroy(void *data)
{
	struct ruddy_filter_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}

static void *ruddy_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct ruddy_filter_data *filter =
		bzalloc(sizeof(struct ruddy_filter_data));
	char *effect_path = obs_module_file("ruddy_filter.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);

	if (filter->effect) {
		filter->strongly_param = gs_effect_get_param_by_name(
			filter->effect, "strongly");
	}



	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		ruddy_filter_destroy(filter);
		return NULL;
	}

	ruddy_filter_update(filter, settings);
	return filter;
}

static void ruddy_filter_render(void *data, gs_effect_t *effect)
{
	struct ruddy_filter_data *filter = data;
	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;

	gs_effect_set_float(filter->strongly_param, filter->strongly);

	//obs_source_process_settexture(filter->target);
	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	obs_source_process_filter_tech_end(filter->context, filter->effect, 0,
					   0, "Draw");
	UNUSED_PARAMETER(effect);
}

static obs_properties_t *ruddy_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "strongly",
					obs_module_text("Strongly"), 1.0, 1.5,
					0.01);

	UNUSED_PARAMETER(data);
	return props;
}

static void ruddy_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "strongly", 1.25);
}

struct obs_source_info ruddy_filter = {
	.id = "ruddy_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = ruddy_filter_getname,
	.create = ruddy_filter_create,
	.destroy = ruddy_filter_destroy,
	.update = ruddy_filter_update,
	.video_render = ruddy_filter_render,
	.get_properties = ruddy_filter_properties,
	.get_defaults = ruddy_filter_defaults,
};
