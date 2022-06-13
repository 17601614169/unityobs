#include <obs-module.h>

typedef struct cocos_source {
	obs_source_t *source;
	int width;
	int height;
}COCOS_SOURCE;

static const char *cocos_source_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "cocos source";
}

static void *cocos_source_create(obs_data_t *settings, obs_source_t *source)
{
	COCOS_SOURCE *cocos = bzalloc(sizeof(struct cocos_source));
	cocos->source = source;
	cocos->width = 1280;
	cocos->height = 720;
	cocos->width = obs_data_get_int(settings, "width");
	cocos->height = obs_data_get_int(settings, "height");

	return cocos;
}

static void cocos_source_destroy(void *data)
{
	COCOS_SOURCE *context = data;
	bfree(context);
}

static void cocos_source_defaults(obs_data_t *settings) {}

static obs_properties_t *cocos_source_properties(void *data)
{
	auto *src = data;
	obs_properties_t *props = obs_properties_create();
	return props;
}

uint32_t cocos_source_width(void *data)
{
	COCOS_SOURCE *context = data;
	return context->width;
}

uint32_t cocos_source_height(void *data)
{
	COCOS_SOURCE *context = data;
	return context->height;
}

void cocos_source_update(void *data, obs_data_t *settings)
{
	COCOS_SOURCE *context = data;
	uint32_t width = (uint32_t)obs_data_get_int(settings, "width");
	uint32_t height = (uint32_t)obs_data_get_int(settings, "height");
	context->width = width;
	context->height = height;
}

struct obs_source_info cocos_filter = {
	.id = "cocos_source",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_DO_NOT_DUPLICATE,
	.get_properties = cocos_source_properties,
	.get_name = cocos_source_name,
	.create = cocos_source_create,
	.destroy = cocos_source_destroy,
	.get_width = cocos_source_width,
	.get_height = cocos_source_height,
	.get_defaults = cocos_source_defaults,
	.update = cocos_source_update,
};
