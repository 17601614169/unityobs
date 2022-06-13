#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <math.h>
#include <util/platform.h>
#include <graphics/image-file.h>
#include <util/dstr.h>
#include<graphics/matrix4.h>
#include<graphics/vec2.h>
#include "obs-source.h"

#define SETTING_ERDUO_DIRECTORY		"erduo_directory"
#define SETTING_HUZI_DIRECTORY          "huzi_directory"

#define TEXT_IMAGE_PATH                obs_module_text("Path")

struct old_face_mask_filter_data {
	obs_source_t *context;
	gs_effect_t *effect;

	char *image_directory_erduo;
	char *image_directory_huzi;
	
	//struct vec2 ErDuo_center_left;
	//struct vec2 HuZi_center_left;
	//struct vec2 ErDuo_sticker_left;	
	//struct vec2 HuZi_sticker_left;

	gs_texture_t *target_ErDuo_left;
	gs_texture_t *target_HuZi_left;
	
	DARRAY(gs_image_file_t) image_array_erduo;
	DARRAY(gs_image_file_t) image_array_huzi;
	gs_image_file_t image_erduo;
	gs_image_file_t image_huzi;
	int index_erduo;
	int index_huzi;

	struct vec2 first_point_erduo;
	struct vec2 second_point_erduo;
	struct vec2 first_point_huzi;
	struct vec2 second_point_huzi;

	float stic_wid_cof_erduo;
	float stic_hei_cof_erduo;
	float stic_wid_cof_huzi;
	float stic_hei_cof_huzi;
};

static const char *old_face_mask_filter_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("FaceMaskFilter");
}

static void old_face_mask_filter_destroy(void *data)
{
	struct old_face_mask_filter_data *filter = data;

	if (filter->image_directory_erduo)
		bfree(filter->image_directory_erduo);

	if (filter->image_directory_huzi)
		bfree(filter->image_directory_huzi);

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);

		int count = filter->image_array_erduo.num;
		for (int i = 0; i < count; i++) {
			gs_image_file_free(&filter->image_array_erduo.array[i]);
		}
		da_free(filter->image_array_erduo);

		count = filter->image_array_huzi.num;
		for (int i = 0; i < count; i++) {
			gs_image_file_free(&filter->image_array_huzi.array[i]);
		}
		da_free(filter->image_array_huzi);

		obs_leave_graphics();
	}

	bfree(data);
}

static void old_face_mask_filter_unload(struct old_face_mask_filter_data *filter)
{	
	obs_enter_graphics();

	int count = filter->image_array_erduo.num;
	for (int i = 0; i < count; i++) {
		gs_image_file_free(&filter->image_array_erduo.array[i]);
	}
	da_free(filter->image_array_erduo);

	count = filter->image_array_huzi.num;
	for (int i = 0; i < count; i++) {
		gs_image_file_free(&filter->image_array_huzi.array[i]);
	}
	da_free(filter->image_array_huzi);

	obs_leave_graphics();
}

static void old_face_mask_filter_image_load(struct old_face_mask_filter_data *filter)
{
	old_face_mask_filter_unload(filter);
	
	obs_enter_graphics();
	char *dir_erduo = filter->image_directory_erduo;
	if (dir_erduo && *dir_erduo) {
		char json_path_erduo[256];
		sprintf(json_path_erduo, "%s/config.json", dir_erduo);
		bool result = os_file_exists(json_path_erduo);
		obs_data_t *data = obs_data_create_from_json_file(json_path_erduo);
		if (!data) {
			blog(LOG_INFO, "create json failed");
			return;
		}

		obs_data_t *config_obj = obs_data_get_obj(data, "config");
		obs_data_t *first_point_obj = obs_data_get_obj(config_obj, "point_first");
		obs_data_t *second_point_obj = obs_data_get_obj(config_obj, "point_second");
		int first_index = 0, end_index = 0;
		if (config_obj) {
			const char *name = obs_data_get_string(config_obj, "name");
			const char *extension = obs_data_get_string(config_obj, "image_extension");
			first_index = obs_data_get_int(config_obj, "image_first_index");
			end_index = obs_data_get_int(config_obj, "image_end_index");
			if (first_point_obj) {
				filter->first_point_erduo.x = obs_data_get_double(first_point_obj, "x");
				filter->first_point_erduo.y = obs_data_get_double(first_point_obj, "y");
			}
			if (second_point_obj) {
				int x = 0, y = 0;
				filter->second_point_erduo.x = obs_data_get_double(second_point_obj, "x");
				filter->second_point_erduo.y = obs_data_get_double(second_point_obj, "y");
			}
			filter->stic_wid_cof_erduo = obs_data_get_double(config_obj, "width_coefficient");
			filter->stic_hei_cof_erduo = obs_data_get_double(config_obj, "height_coefficient");
		}

		da_init(filter->image_array_erduo);
		for (int i = first_index; i <= end_index; i++) {
			char image_path[256];
			sprintf(image_path, "%s/%d.png", dir_erduo, i);
			if (os_file_exists(image_path)) {
				gs_image_file_init(&filter->image_erduo, image_path);
				gs_image_file_init_texture(&filter->image_erduo);

				darray_push_back(sizeof(gs_image_file_t), &filter->image_array_erduo, &filter->image_erduo);
			} else {
				break;
			}
		}
		obs_data_release(second_point_obj);
		obs_data_release(first_point_obj);
		obs_data_release(config_obj);
		obs_data_release(data);
	}

	char *dir_huzi = filter->image_directory_huzi;
	if (dir_huzi && *dir_huzi) {
		char json_path_huzi[256];
		sprintf(json_path_huzi, "%s/config.json", dir_huzi);
		bool result = os_file_exists(json_path_huzi);
		obs_data_t *data = obs_data_create_from_json_file(json_path_huzi);
		if (!data) {
			blog(LOG_INFO, "create json failed");
			return;
		}

		obs_data_t *config_obj = obs_data_get_obj(data, "config");
		obs_data_t *first_point_obj = obs_data_get_obj(config_obj, "point_first");
		obs_data_t *second_point_obj = obs_data_get_obj(config_obj, "point_second");
		int first_index = 0, end_index = 0;
		if (config_obj) {
			const char *name = obs_data_get_string(config_obj, "name");
			const char *extension = obs_data_get_string(config_obj, "image_extension");
			first_index = obs_data_get_int(config_obj, "image_first_index");
			end_index = obs_data_get_int(config_obj, "image_end_index");
			if (first_point_obj) {
				filter->first_point_huzi.x = obs_data_get_double(first_point_obj, "x");
				filter->first_point_huzi.y = obs_data_get_double(first_point_obj, "y");

			}if (second_point_obj) {
				filter->second_point_huzi.x = obs_data_get_double(second_point_obj, "x");
				filter->second_point_huzi.y = obs_data_get_double(second_point_obj, "y");
			}
			filter->stic_wid_cof_huzi = obs_data_get_double(config_obj, "width_coefficient");
			filter->stic_hei_cof_huzi = obs_data_get_double(config_obj, "height_coefficient");
		}

		da_init(filter->image_array_huzi);
		for (int i = first_index; i <= end_index; i++) {
			char image_path[256];
			sprintf(image_path, "%s/%d.png", dir_huzi, i);
			if (os_file_exists(image_path)) {
				gs_image_file_init(&filter->image_huzi, image_path);
				gs_image_file_init_texture(&filter->image_huzi);

				darray_push_back(sizeof(gs_image_file_t), &filter->image_array_huzi, &filter->image_huzi);
			} else {
				break;
			}
		}
		obs_data_release(second_point_obj);
		obs_data_release(first_point_obj);
		obs_data_release(config_obj);
		obs_data_release(data);
	}
	obs_leave_graphics();
}

static void old_face_mask_filter_update(void *data, obs_data_t *settings)
{
	struct old_face_mask_filter_data *filter = data;

	const char *dir_erduo = obs_data_get_string(settings, SETTING_ERDUO_DIRECTORY);
	if (filter->image_directory_erduo)
		bfree(filter->image_directory_erduo);
	filter->image_directory_erduo = bstrdup(dir_erduo);

	const char *dir_huzi = obs_data_get_string(settings, SETTING_HUZI_DIRECTORY);
	if (filter->image_directory_huzi)
		bfree(filter->image_directory_huzi);
	filter->image_directory_huzi = bstrdup(dir_huzi);
	
	old_face_mask_filter_image_load(filter);
}

static void *old_face_mask_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct old_face_mask_filter_data *filter =
		bzalloc(sizeof(struct old_face_mask_filter_data));
	char *effect_path = obs_module_file("old_face_mask_filter.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);


	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		old_face_mask_filter_destroy(filter);
		return NULL;
	}

	old_face_mask_filter_update(filter, settings);
	filter->index_erduo = 0;
	filter->index_huzi = 0;
	return filter;
}

static void old_face_mask_filter_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
}

static float distance(float x1,float x2,float y1,float y2) {
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

static void set_vec2_param(struct old_face_mask_filter_data *filter, const char *name, float x, float y) {
	struct vec2 my_vec2;
	gs_eparam_t *param;
	param = gs_effect_get_param_by_name(filter->effect, name);
	vec2_set(&my_vec2, x, y);
	gs_effect_set_vec2(param, &my_vec2);
}


void projection_(struct vec4 *x) {
	x->y*=x->y/x->z*1280;
	x->x*=x->x/x->z*1280;
}

void projection(struct matrix4 *p3) {
	projection_(&(p3->x));
	projection_(&(p3->y));
	projection_(&(p3->z));
	projection_(&(p3->t));
}

static void face_render(void *data, float *facepoint, gs_effect_t *effect)
{
	struct old_face_mask_filter_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING)) 
		return;
	
	gs_eparam_t *param;
	float width = facepoint[356];
	float height = facepoint[389];

	float pitch = 0.001, yaw = 0.001, roll = -facepoint[323];
	//if (roll < 0) {
	//	roll = (3.1415/2-roll)*-1;
	//}


	param = gs_effect_get_param_by_name(filter->effect, "s_width");
	gs_effect_set_float(param, width);
	param = gs_effect_get_param_by_name(filter->effect, "s_height");
	gs_effect_set_float(param, height);

	struct matrix4 my_rmat;
	struct axisang aa1;
	axisang_set(&aa1, -pitch, -yaw, roll, 1);
	matrix4_from_axisang(&my_rmat, &aa1);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);

	int count = filter->image_array_erduo.num;
	if (count > 0) {
		filter->index_erduo %= count;
		filter->target_ErDuo_left = filter->image_array_erduo.array[filter->index_erduo].texture;
		filter->index_erduo++;

		if (!obs_source_process_filter_begin(filter->context, GS_RGBA,OBS_ALLOW_DIRECT_RENDERING)) 
			return;

		param = gs_effect_get_param_by_name(filter->effect, "target_ErDuo_left");
		gs_effect_set_texture(param, filter->target_ErDuo_left);

		float xcenter = facepoint[(int)filter->first_point_erduo.x];
		float ycenter = facepoint[(int)filter->first_point_erduo.y];
		if (filter->second_point_erduo.x != 0 || filter->second_point_erduo.y != 0) {
			xcenter = abs((xcenter - facepoint[(int)filter->second_point_erduo.x]) * 2 - facepoint[(int)filter->second_point_erduo.x]);
			ycenter = abs((ycenter - facepoint[(int)filter->second_point_erduo.y]) * 2 - facepoint[(int)filter->second_point_erduo.y]);
		}
		float sticker_width = distance(facepoint[46 * 2] ,facepoint[276 * 2] ,facepoint[46 * 2 + 1],facepoint[276 * 2 + 1]) * (float)filter->stic_wid_cof_erduo;
		float sticker_height = distance(facepoint[9 * 2],facepoint[199 * 2],facepoint[9 * 2+1] , facepoint[199 * 2+1])*(float)filter->stic_hei_cof_erduo;
		
		//中心
		int img_width = filter->image_array_erduo.array->cx;
		int img_height = filter->image_array_erduo.array->cy;
	
		double sticker_h = (sticker_width)*1.0 / img_width * img_height;
		struct matrix4 p1 = {{-1,1,0,1}, {1,1,0,1}, {1,-1,0,1}, {-1,-1,0,1}},p3;
		p1.x.x = - sticker_width / 2;
		p1.x.y = - sticker_h / 2 ;

		p1.y.x =  sticker_width / 2;
		p1.y.y = - sticker_h / 2;

		p1.z.x =  sticker_width / 2;
		p1.z.y =  sticker_h / 2;

		p1.t.x = - sticker_width / 2;
		p1.t.y =  sticker_h / 2;

		matrix4_mul(&p3,&p1,&my_rmat);
		//
		float f1=width/4;
		//
		//projection(&p3);

		//p3.x.y*=p3.x.y/p3.x.z*1280;
		//p3.x.x*=p3.x.x/p3.x.z*1280;

		//p3.y.y*=f1/(f1-p3.y.z);
		//p3.y.x*=f1/(f1-p3.y.z);

		//p3.z.y*=f1/(f1-p3.z.z);
		//p3.z.x*=f1/(f1-p3.z.z);

		//p3.t.y*=f1/(f1-p3.t.z);
		//p3.t.x*=f1/(f1-p3.t.z);
		struct vec3 u1_p[] = {
			{p3.x.x+ xcenter, p3.x.y+ ycenter, 0},
			{p3.y.x+ xcenter, p3.y.y+ ycenter, 0},
			{p3.t.x+ xcenter, p3.t.y+ ycenter, 0},
			{p3.z.x+ xcenter, p3.z.y+ ycenter, 0}
		};
		struct matrix4 pp1 = {
			{(p3.x.x + xcenter), (p3.x.y + ycenter), 0,0},
			{p3.y.x + xcenter, p3.y.y + ycenter, 0,0},
			{p3.t.x + xcenter, p3.t.y + ycenter, 0,0},
			{p3.z.x + xcenter, p3.z.y + ycenter, 0,0}
		};

		param = gs_effect_get_param_by_name(filter->effect, "pp1");
		gs_effect_set_matrix4(param, &pp1);
		param = gs_effect_get_param_by_name(filter->effect, "u1_p0");
		gs_effect_set_vec3(param, &u1_p[1]);
		param = gs_effect_get_param_by_name(filter->effect, "u1_p1");
		gs_effect_set_vec3(param, &u1_p[0]);
		param = gs_effect_get_param_by_name(filter->effect, "u1_p2");
		gs_effect_set_vec3(param, &u1_p[3]);
		param = gs_effect_get_param_by_name(filter->effect, "u1_p3");
		gs_effect_set_vec3(param, &u1_p[2]);

		obs_source_process_filter_tech_end(filter->context, filter->effect, 0,0, "Draw2");
	}

	count = filter->image_array_huzi.num;
	if (count > 0) {
		filter->index_huzi %= count;
		filter->target_HuZi_left = filter->image_array_huzi.array[filter->index_huzi].texture;
		filter->index_huzi++;
		
		if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING)) 
			return;

		param = gs_effect_get_param_by_name(filter->effect, "target_HuZi_left");
		gs_effect_set_texture(param, filter->target_HuZi_left);
		
		float xcenter = facepoint[(int)filter->first_point_huzi.x];
		float ycenter = facepoint[(int)filter->first_point_huzi.y];
		if (filter->second_point_huzi.x != 0 || filter->second_point_huzi.y != 0) {
			xcenter = abs((xcenter - facepoint[(int)filter->second_point_huzi.x]) * 2 - facepoint[(int)filter->second_point_huzi.x]);
			ycenter = abs((ycenter - facepoint[(int)filter->second_point_huzi.y]) * 2 - facepoint[(int)filter->second_point_huzi.y]);
		}

		float sticker_width = distance(facepoint[46 * 2] ,facepoint[276 * 2] ,facepoint[46 * 2 + 1],facepoint[276 * 2 + 1]) * (float)filter->stic_wid_cof_huzi;
		float sticker_height = distance(facepoint[9 * 2],facepoint[199 * 2],facepoint[9 * 2+1] , facepoint[199 * 2+1])*(float)filter->stic_hei_cof_huzi;

		//获取贴纸的宽高  保证贴纸的宽高比
		int img_width = filter->image_array_huzi.array->cx;
		int img_height = filter->image_array_huzi.array->cy;
		//贴纸宽高
		double sticker_h = (sticker_width)*1.0 / img_width * img_height;
		struct matrix4 p = {{-1,1,0,1}, {1,1,0,1}, {1,-1,0,1}, {-1,-1,0,1}},p2;
		p.x.x = - sticker_width / 2;
		p.x.y = - sticker_h / 2;

		p.y.x =  sticker_width / 2;
		p.y.y = - sticker_h / 2;

		p.z.x =  sticker_width / 2;
		p.z.y =  sticker_h / 2;

		p.t.x = - sticker_width / 2;
		p.t.y =  sticker_h / 2;

		matrix4_mul(&p2,&p,&my_rmat);

		float f=width/4;
		//projection(&p2);
	
		//p2.x.y*=f/(f-p2.x.z);
		//p2.x.x*=f/(f-p2.x.z);

		//p2.y.y*=f/(f-p2.y.z);
		//p2.y.x*=f/(f-p2.y.z);

		//p2.z.y*=f/(f-p2.z.z);
		//p2.z.x*=f/(f-p2.z.z);

		//p2.t.y*=f/(f-p2.t.z);
		//p2.t.x*=f/(f-p2.t.z);

		struct vec3 u_p[] = {
			{p2.x.x+ xcenter, p2.x.y+ ycenter, 0},
			{p2.y.x+ xcenter, p2.y.y+ ycenter, 0},
			{p2.t.x+ xcenter, p2.t.y+ ycenter, 0} ,
			{p2.z.x+ xcenter, p2.z.y+ ycenter, 0}
		};

		struct matrix4 pp = {
			{(p2.x.x + xcenter), (p2.x.y + ycenter), 0,0},
			{p2.y.x + xcenter, p2.y.y + ycenter, 0,0},
			{p2.t.x + xcenter, p2.t.y + ycenter, 0,0},
			{p2.z.x + xcenter, p2.z.y + ycenter, 0,0}
		};

		param = gs_effect_get_param_by_name(filter->effect, "pp");
		gs_effect_set_matrix4(param, &pp);
		param = gs_effect_get_param_by_name(filter->effect, "u_p0");
		gs_effect_set_vec3(param, &u_p[1]);
		param = gs_effect_get_param_by_name(filter->effect, "u_p1");
		gs_effect_set_vec3(param, &u_p[0]);
		param = gs_effect_get_param_by_name(filter->effect, "u_p2");
		gs_effect_set_vec3(param, &u_p[3]);
		param = gs_effect_get_param_by_name(filter->effect, "u_p3");
		gs_effect_set_vec3(param, &u_p[2]);

		obs_source_process_filter_tech_end(filter->context, filter->effect, 0, 0, "Draw1");
	}
	
	UNUSED_PARAMETER(effect);
}

static obs_properties_t *old_face_mask_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_path(props, SETTING_ERDUO_DIRECTORY, TEXT_IMAGE_PATH, OBS_PATH_DIRECTORY, NULL, NULL);
	obs_properties_add_path(props, SETTING_HUZI_DIRECTORY, TEXT_IMAGE_PATH, OBS_PATH_DIRECTORY, NULL, NULL);

	UNUSED_PARAMETER(data);
	return props;
}

static void old_face_mask_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, SETTING_ERDUO_DIRECTORY, "");
	obs_data_set_default_string(settings, SETTING_HUZI_DIRECTORY, "");
}

struct obs_source_info old_face_mask_filter = {
	.id = "old_face_mask_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = old_face_mask_filter_getname,
	.create = old_face_mask_filter_create,
	.destroy = old_face_mask_filter_destroy,
	.update = old_face_mask_filter_update,
	.video_render = old_face_mask_filter_render,
	.get_properties = old_face_mask_filter_properties,
	.get_defaults = old_face_mask_filter_defaults,
	.face_config = true,
	.video_face_render = face_render,
};
