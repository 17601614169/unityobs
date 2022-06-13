#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <math.h>
#include <util/platform.h>
#include <graphics/image-file.h>
#include <util/dstr.h>

#include "obs-source.h"
#include <Windows.h>

#define SETTING_TYPE "type"
#define SETTING_IMAGE_PATH "image_path"

#define TEXT_TYPE obs_module_text("Type")
#define TEXT_IMAGE_PATH obs_module_text("Path")
#define TEXT_PATH_IMAGES obs_module_text("BrowsePath.Images")
#define TEXT_PATH_ALL_FILES obs_module_text("BrowsePath.AllFiles")

HANDLE save_image_handle = INVALID_HANDLE_VALUE;
const char IMAGE_MUTEX_NAME[] = "COCOS_IMAGE_SAVE";

/*面具*/
struct new_mask_face_data {
	obs_source_t *context;
	gs_effect_t *effect;

	//char *image_file;

	//gs_image_file_t image;
	gs_texture_t *target;
	int type;
};

static const char *new_mask_face_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("NewFaceMaskFilter");
}

static void new_mask_face_destroy(void *data)
{
	struct new_mask_face_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}

static void new_mask_face_update(void *data, obs_data_t *settings)
{
	struct new_mask_face_data *filter = data;

	const char *type = obs_data_get_string(settings, SETTING_TYPE);
	int nType = atoi(type);
	filter->type = nType;
}

static void *new_mask_face_create(obs_data_t *settings, obs_source_t *context)
{
	if (save_image_handle == INVALID_HANDLE_VALUE ||
	    save_image_handle == 0) {
		save_image_handle =
			OpenMutexA(SYNCHRONIZE, false, IMAGE_MUTEX_NAME);
		bool alreadyExist = !!save_image_handle;
		if (!alreadyExist)
			alreadyExist =
				CreateMutexA(NULL, false, IMAGE_MUTEX_NAME);
	}

	struct new_mask_face_data *filter =
		bzalloc(sizeof(struct new_mask_face_data));
	char *effect_path = obs_module_file("add_cocos.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);

	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		new_mask_face_destroy(filter);
		return NULL;
	}

	new_mask_face_update(filter, settings);
	return filter;
}

static void new_mask_face_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
}

static void face_render(void *data, float *facepoint, gs_effect_t *effect,
			const uint8_t *image_data,
	unsigned long start,
	unsigned long source_start)
{


	if (image_data == NULL)// || facepoint[0] == 0)
		return;
	struct new_mask_face_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;
	gs_eparam_t *param;
	//if (save_image_handle != INVALID_HANDLE_VALUE &&
	//    save_image_handle != 0) {
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(save_image_handle, 100);
		//if (dwWaitResult == WAIT_OBJECT_0) {
			if (filter->target == NULL) 
				filter->target = gs_texture_create(
					1280, 720, GS_RGBA, 1, &image_data, 0);
			 else {
				//filter->target = NULL;
				

				filter->target = gs_texture_create(
					1280, 720, GS_RGBA, 1, &image_data, 0);
			}
			unsigned long s1 = GetTickCount64();
			
			//printf("load texture delay: %lu\n", s1 - start);
			//printf("load texture from cache delay: %lu\n", start - source_start);
			param = gs_effect_get_param_by_name(filter->effect,
							    "target");
			gs_effect_set_texture(param, filter->target);
			obs_source_process_filter_end(filter->context,
						      filter->effect, 0, 0);

			UNUSED_PARAMETER(effect);
			gs_texture_destroy(filter->target);
			filter->target = NULL;
			
		//}
		ReleaseMutex(save_image_handle);
	//} else {
	//	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
	//}
}

#define IMAGE_FILTER_EXTENSIONS " (*.bmp *.jpg *.jpeg *.tga *.gif *.png)"
static obs_properties_t *new_mask_face_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_property_t *p;
	p = obs_properties_add_list(props, SETTING_TYPE, TEXT_TYPE,
				    OBS_COMBO_TYPE_LIST,
				    OBS_COMBO_FORMAT_STRING);
	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Eyes"),
				     "0");
	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Glasses"),
				     "1");
	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Nose"),
				     "2");
	obs_property_list_add_string(
		p, obs_module_text("FaceMaskType.GlassesAndNose"), "3");

	struct dstr filter_str = {0};
	dstr_copy(&filter_str, TEXT_PATH_IMAGES);
	dstr_cat(&filter_str, IMAGE_FILTER_EXTENSIONS ";;");
	dstr_cat(&filter_str, TEXT_PATH_ALL_FILES);
	dstr_cat(&filter_str, " (*.*)");
	obs_properties_add_path(props, SETTING_IMAGE_PATH, TEXT_IMAGE_PATH,
				OBS_PATH_FILE, filter_str.array, NULL);

	dstr_free(&filter_str);

	UNUSED_PARAMETER(data);
	return props;
}

static void new_mask_face_defaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, SETTING_TYPE, "0");
}

struct obs_source_info new_mask_face = {
	.id = "new_mask_face",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = new_mask_face_getname,
	.create = new_mask_face_create,
	.destroy = new_mask_face_destroy,
	.update = new_mask_face_update,
	.video_render = new_mask_face_render,
	.get_properties = new_mask_face_properties,
	.get_defaults = new_mask_face_defaults,
	.face_config = true,
	.video_face_render = face_render,
};
