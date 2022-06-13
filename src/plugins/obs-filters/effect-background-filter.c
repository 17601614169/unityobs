#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#include <graphics/image-file.h>
#include <util/dstr.h>

#define SETTING_IMAGE_PATH             "image_path"

#define TEXT_IMAGE_PATH                obs_module_text("Path")
#define TEXT_PATH_IMAGES               obs_module_text("BrowsePath.Images")
#define TEXT_PATH_ALL_FILES            obs_module_text("BrowsePath.AllFiles")

/*面具*/
struct background_mask_filter_data {
	obs_source_t *context;
	gs_effect_t *effect;

	char *image_file;
	gs_eparam_t *target_param;

	gs_image_file_t image;
	gs_texture_t *target;
};

static const char *background_mask_filter_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("BackgroundMaskFilter");
}

static void background_mask_filter_destroy(void *data)
{
	struct background_mask_filter_data *filter = data;

	if (filter->image_file)
		bfree(filter->image_file);

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}

static void background_mask_filter_image_unload(struct background_mask_filter_data *filter)
{
	obs_enter_graphics();
	gs_image_file_free(&filter->image);
	obs_leave_graphics();
}

static void background_mask_filter_image_load(struct background_mask_filter_data *filter)
{
	background_mask_filter_image_unload(filter);

	char *path = filter->image_file;
	if (path && *path) {
		gs_image_file_init(&filter->image, path);
		obs_enter_graphics();
		gs_image_file_init_texture(&filter->image);
		obs_leave_graphics();
		filter->target = filter->image.texture;
	}
}

static void background_mask_filter_update(void *data, obs_data_t *settings)
{
	struct background_mask_filter_data *filter = data;

	const char *path = obs_data_get_string(settings, SETTING_IMAGE_PATH);
	if (filter->image_file)
		bfree(filter->image_file);
	filter->image_file = bstrdup(path);
	
	background_mask_filter_image_load(filter);
}

static void *background_mask_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct background_mask_filter_data *filter =
		bzalloc(sizeof(struct background_mask_filter_data));
	char *effect_path = obs_module_file("face_mask_filter.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);

	if (filter->effect) {
		filter->target_param = gs_effect_get_param_by_name(filter->effect, "target");
	}

	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		background_mask_filter_destroy(filter);
		return NULL;
	}

	background_mask_filter_update(filter, settings);
	return filter;
}

static void background_mask_filter_render(void *data, gs_effect_t *effect)
{
	struct background_mask_filter_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
		return;

	gs_effect_set_texture(filter->target_param, filter->target);
	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);

	UNUSED_PARAMETER(effect);
}
//
//static void background_render(void *data, float *facepoint, gs_effect_t *effect)
//{
//	struct background_mask_filter_data *filter = data;
//
//	if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
//		return;
//
//	gs_effect_set_texture(filter->target_param, filter->target);
//
//	int sticker_width = abs(facepoint[0 * 2] - facepoint[32 * 2]);
//
//	int sticker_height = abs(facepoint[35 * 2+1] - facepoint[54 * 2+1])*3.0;
//	
//	if (facepoint!=NULL) {
//		/*if (filter->type == 0) {
//			obs_source_process_settexture(filter->target, (int)facepoint[54 * 2],
//				      (int)(facepoint[38 * 2 + 1]+(int)facepoint[50 * 2 + 1])/2, sticker_width,
//				      sticker_height);
//		} else if (filter->type == 1) {
//			obs_source_process_settexture(filter->target, (int)facepoint[54 * 2],
//				      (int)(facepoint[64 * 2 + 1]+(int)facepoint[68 * 2 + 1])/2, sticker_width,
//				      sticker_height);
//		} else if (filter->type == 2) {
//			int sticker_height = abs(facepoint[51 * 2+1] - facepoint[85 * 2+1])*1.5;
//			obs_source_process_settexture(filter->target, (int)facepoint[54 * 2],
//				      (int)facepoint[79 * 2 + 1], sticker_width,
//				      sticker_height);
//		} else if (filter->type == 3) {
//			obs_source_process_settexture(filter->target, (int)facepoint[54 * 2],
//				      (int)facepoint[52 * 2 + 1], sticker_width,
//				      sticker_height);
//		}*/
//
//		obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
//	}	
//
//	UNUSED_PARAMETER(effect);
//}

#define IMAGE_FILTER_EXTENSIONS " (*.bmp *.jpg *.jpeg *.tga *.gif *.png)"
static obs_properties_t *background_mask_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	struct dstr filter_str = {0};
	dstr_copy(&filter_str, TEXT_PATH_IMAGES);
	dstr_cat(&filter_str, IMAGE_FILTER_EXTENSIONS ";;");
	dstr_cat(&filter_str, TEXT_PATH_ALL_FILES);
	dstr_cat(&filter_str, " (*.*)");
	obs_properties_add_path(props, SETTING_IMAGE_PATH, TEXT_IMAGE_PATH, OBS_PATH_FILE, filter_str.array, NULL);

	dstr_free(&filter_str);

	UNUSED_PARAMETER(data);
	return props;
}

static void background_mask_filter_defaults(obs_data_t *settings)
{
}

struct obs_source_info background_mask_filter = {
	.id = "background_mask_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = background_mask_filter_getname,
	.create = background_mask_filter_create,
	.destroy = background_mask_filter_destroy,
	.update = background_mask_filter_update,
	.video_render = background_mask_filter_render,
	.get_properties = background_mask_filter_properties,
	.get_defaults = background_mask_filter_defaults,
};
