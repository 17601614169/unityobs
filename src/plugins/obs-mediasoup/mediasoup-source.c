#include "mediasoup-source.h"



static const char *mediasoup_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Mediasoup Source";
}

static void *mediasoup_source_create(obs_data_t *settings, obs_source_t *source)
{
	struct mediasoup_input_source *s =
		bzalloc(sizeof(struct mediasoup_input_source));
	s->source = source;
	s->width = 1280;
	s->height =720;
	s->width = obs_data_get_int(settings, "width");
	s->height= obs_data_get_int(settings, "height");
	
	
	return s;
}

static void mediasoup_source_destroy(void *data)
{
	struct mediasoup_input_source *context = data;
	bfree(context);
}

static void mediasoup_source_defaults(obs_data_t *settings) {}

static obs_properties_t *mediasoup_source_properties(void *data)
{
	auto *src = data;
	obs_properties_t *props = obs_properties_create();
	return props;
}

uint32_t mediasoup_source_width(void *data)
{
	struct mediasoup_input_source *context = data;
	return context->width;
}

uint32_t mediasoup_source_height(void *data)
{
	struct mediasoup_input_source *context = data;
	return context->height;
}

void mediasoup_source_update(void* data, obs_data_t* settings) {
	struct mediasoup_input_source *context = data;
	uint32_t width = (uint32_t)obs_data_get_int(settings, "width");
	uint32_t height = (uint32_t)obs_data_get_int(settings, "height");
	context->width = width;
	context->height = height;
}

void register_mediasoup_source()
{
	struct obs_source_info si = {0};
	si.id = "mediasoup_source";
	si.type = OBS_SOURCE_TYPE_INPUT;
	si.output_flags = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_DO_NOT_DUPLICATE;
	si.get_properties = mediasoup_source_properties;

	si.get_name = mediasoup_source_get_name;
	si.create = mediasoup_source_create;
	si.destroy = mediasoup_source_destroy;
	si.get_width = mediasoup_source_width;
	si.get_height = mediasoup_source_height;
	si.get_defaults = mediasoup_source_defaults;
	si.update = mediasoup_source_update;
	obs_register_source(&si);
}

