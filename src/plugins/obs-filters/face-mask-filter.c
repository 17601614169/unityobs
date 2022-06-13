//#include <obs-module.h>
//#include <obs-source.h>
//#include <obs.h>
//#include <math.h>
//#include <util/platform.h>
//#include <graphics/image-file.h>
//#include <util/dstr.h>
//
//#include "obs-source.h"
//#include <Windows.h>
//
//#define SETTING_TYPE                   "type"
//#define SETTING_IMAGE_PATH             "image_path"
//
//#define TEXT_TYPE                      obs_module_text("Type")
//#define TEXT_IMAGE_PATH                obs_module_text("Path")
//#define TEXT_PATH_IMAGES               obs_module_text("BrowsePath.Images")
//#define TEXT_PATH_ALL_FILES            obs_module_text("BrowsePath.AllFiles")
//
//HANDLE save_image_handle = INVALID_HANDLE_VALUE;
//const char IMAGE_MUTEX_NAME[]= "COCOS_IMAGE_SAVE";
//
///*面具*/
//struct face_mask_filter_data {
//	obs_source_t *context;
//	gs_effect_t *effect;
//
//	char *image_file;
//	gs_eparam_t *smatrix00_param;
//	gs_eparam_t *smatrix01_param;
//	gs_eparam_t *smatrix02_param;
//	gs_eparam_t *smatrix03_param;
//	gs_eparam_t *smatrix10_param;
//	gs_eparam_t *smatrix11_param;
//	gs_eparam_t *smatrix12_param;
//	gs_eparam_t *smatrix13_param;
//	gs_eparam_t *smatrix20_param;
//	gs_eparam_t *smatrix21_param;
//	gs_eparam_t *smatrix22_param;
//	gs_eparam_t *smatrix23_param;
//	gs_eparam_t *smatrix30_param;
//	gs_eparam_t *smatrix31_param;
//	gs_eparam_t *smatrix32_param;
//	gs_eparam_t *smatrix33_param;
//	gs_eparam_t *xcenter_param;
//	gs_eparam_t *ycenter_param;
//	gs_eparam_t *stickerw_param;
//	gs_eparam_t *stickerh_param;
//
//	gs_image_file_t image;
//	gs_texture_t *target;
//	int type;
//};
//
//static const char *face_mask_filter_getname(void *unused)
//{
//	UNUSED_PARAMETER(unused);
//	return obs_module_text("FaceMaskFilter");
//}
//
//static void face_mask_filter_destroy(void *data)
//{
//	struct face_mask_filter_data *filter = data;
//
//	if (filter->image_file)
//		bfree(filter->image_file);
//
//	if (filter->effect) {
//		obs_enter_graphics();
//		gs_effect_destroy(filter->effect);
//		obs_leave_graphics();
//	}
//
//	bfree(data);
//}
//
//static void face_mask_filter_image_unload(struct face_mask_filter_data *filter)
//{
//	obs_enter_graphics();
//	gs_image_file_free(&filter->image);
//	obs_leave_graphics();
//}
//
//static void face_mask_filter_image_load(struct face_mask_filter_data *filter)
//{
//	if (save_image_handle != INVALID_HANDLE_VALUE &&
//	    save_image_handle != 0) {
//		DWORD dwWaitResult;
//		dwWaitResult = WaitForSingleObject(save_image_handle, 10000);
//		if (dwWaitResult == WAIT_OBJECT_0) {
//			face_mask_filter_image_unload(filter);
//
//			char *path = filter->image_file;
//			if (path && *path) {
//				gs_image_file_init(&filter->image, "./tmpcolor9.png");
//				obs_enter_graphics();
//				gs_image_file_init_texture(&filter->image);
//				obs_leave_graphics();
//				filter->target = filter->image.texture;
//			}
//			ReleaseMutex(save_image_handle);
//		} else {
//			ReleaseMutex(save_image_handle);
//		}
//	}
//}
//
//static void face_mask_filter_update(void *data, obs_data_t *settings)
//{
//	struct face_mask_filter_data *filter = data;
//
//	const char *path = obs_data_get_string(settings, SETTING_IMAGE_PATH);
//	if (filter->image_file)
//		bfree(filter->image_file);
//	filter->image_file = bstrdup(path);
//	
//	face_mask_filter_image_load(filter);
//
//	const char *type = obs_data_get_string(settings, SETTING_TYPE);
//	int nType = atoi(type);
//	filter->type = nType;
//}
//
//static void *face_mask_filter_create(obs_data_t *settings, obs_source_t *context)
//{
//	if (save_image_handle == INVALID_HANDLE_VALUE ||
//	    save_image_handle == 0) {
//		save_image_handle = OpenMutexA(SYNCHRONIZE, false, IMAGE_MUTEX_NAME);
//		bool alreadyExist = !!save_image_handle;
//		if (!alreadyExist)
//			alreadyExist = CreateMutexA(NULL, false, IMAGE_MUTEX_NAME);
//	}
//
//	struct face_mask_filter_data *filter =
//		bzalloc(sizeof(struct face_mask_filter_data));
//	char *effect_path = obs_module_file("face_mask_filter.effect");
//
//	filter->context = context;
//
//	obs_enter_graphics();
//
//	filter->effect = gs_effect_create_from_file(effect_path, NULL);
//
//	if (filter->effect) {
//		filter->smatrix00_param = gs_effect_get_param_by_name(filter->effect, "smatrix00");
//		filter->smatrix01_param = gs_effect_get_param_by_name(filter->effect, "smatrix01");
//		filter->smatrix02_param = gs_effect_get_param_by_name(filter->effect, "smatrix02");
//		filter->smatrix03_param = gs_effect_get_param_by_name(filter->effect, "smatrix03");
//		filter->smatrix10_param = gs_effect_get_param_by_name(filter->effect, "smatrix10");
//		filter->smatrix11_param = gs_effect_get_param_by_name(filter->effect, "smatrix11");
//		filter->smatrix12_param = gs_effect_get_param_by_name(filter->effect, "smatrix12");
//		filter->smatrix13_param = gs_effect_get_param_by_name(filter->effect, "smatrix13");
//		filter->smatrix20_param = gs_effect_get_param_by_name(filter->effect, "smatrix20");
//		filter->smatrix21_param = gs_effect_get_param_by_name(filter->effect, "smatrix21");
//		filter->smatrix22_param = gs_effect_get_param_by_name(filter->effect, "smatrix22");
//		filter->smatrix23_param = gs_effect_get_param_by_name(filter->effect, "smatrix23");
//		filter->smatrix30_param = gs_effect_get_param_by_name(filter->effect, "smatrix30");
//		filter->smatrix31_param = gs_effect_get_param_by_name(filter->effect, "smatrix31");
//		filter->smatrix32_param = gs_effect_get_param_by_name(filter->effect, "smatrix32");
//		filter->smatrix33_param = gs_effect_get_param_by_name(filter->effect, "smatrix33");
//		filter->xcenter_param = gs_effect_get_param_by_name(filter->effect, "xcenter");
//		filter->ycenter_param = gs_effect_get_param_by_name(filter->effect, "ycenter");
//		filter->stickerw_param = gs_effect_get_param_by_name(filter->effect, "stickerw");
//		filter->stickerh_param = gs_effect_get_param_by_name(filter->effect, "stickerh");
//	}
//
//	obs_leave_graphics();
//
//	bfree(effect_path);
//
//	if (!filter->effect) {
//		face_mask_filter_destroy(filter);
//		return NULL;
//	}
//
//	face_mask_filter_update(filter, settings);
//	return filter;
//}
//
//static void face_mask_filter_render(void *data, gs_effect_t *effect)
//{
//	UNUSED_PARAMETER(effect);
//}
//
//static double Rrect_update(float *result) {
//	double a = 0;
//	float cx = 0;
//	float cy = 0;
//	float w = 0;
//	int croped_w = 0;
//	float max_x = 0,min_x = 1,max_y = 0,min_y = 1;
//        for (int i =0;i<192;i++){
//
//		if(result[2*i] > max_x){
//		max_x = result[2*i];
//		}
//		if(result[2*i] < min_x){
//		min_x = result[2*i];
//		}
//
//		if(result[2*i+1] > max_y){
//		max_y = result[2*i+1];
//		}
//		if(result[2*i+1] < min_y){
//		min_y = result[2*i+1];
//		}
//        }
//        float x_diff = max_x-min_x;
//        float y_diff = max_y-min_y;
//
//        float w_max = max(x_diff,y_diff);
//
//        // center nose
//        float center_x = (max_x+min_x)/2;
//        float center_y = (max_y+min_y)/2;
//
//        // calc center point by last bounding box center adding center shift by current detection
//        cx = 240;// (float) center_shift_global_frame.get(0,0)[0];
//        cy = 320;//(float) center_shift_global_frame.get(1,0)[0]*-1;
//
//        w = max(w_max*croped_w*1.07f,156);
//        w = min(w,250);
//        w = croped_w;
//        // eye vector
//        float left_eye_x = result[62 * 2];
//        float left_eye_y = (result[62 * 2 + 1]+result[66 * 2 + 1])/2;
//        float right_eye_x = result[70 * 2];
//        float right_eye_y = (result[70 * 2 + 1] + result[74 * 2 + 1])/2;
//
//        float v_x = -1*(right_eye_x - left_eye_x);
//        float v_y = -1*(right_eye_y - left_eye_y);
//        float k = v_y / v_x;
//        a = atan2(v_y, v_x) / M_PI;
//	return a;
//}
//
//static void jzrotate(float rm[], int rmOffset,float a, float x, float y, float z) {
//        rm[rmOffset + 3] = 0;
//        rm[rmOffset + 7] = 0;
//        rm[rmOffset + 11]= 0;
//        rm[rmOffset + 12]= 0;
//        rm[rmOffset + 13]= 0;
//        rm[rmOffset + 14]= 0;
//        rm[rmOffset + 15]= 1;
//        a *= (float) M_PI;
//        float s = (float) sin(a);
//        float c = (float) cos(a);
//        if (1.0f == x && 0.0f == y && 0.0f == z) {
//            rm[rmOffset + 5] = c;   rm[rmOffset + 10]= c;
//            rm[rmOffset + 6] = s;   rm[rmOffset + 9] = -s;
//            rm[rmOffset + 1] = 0;   rm[rmOffset + 2] = 0;
//            rm[rmOffset + 4] = 0;   rm[rmOffset + 8] = 0;
//            rm[rmOffset + 0] = 1;
//        } else if (0.0f == x && 1.0f == y && 0.0f == z) {
//            rm[rmOffset + 0] = c;   rm[rmOffset + 10]= c;
//            rm[rmOffset + 8] = s;   rm[rmOffset + 2] = -s;
//            rm[rmOffset + 1] = 0;   rm[rmOffset + 4] = 0;
//            rm[rmOffset + 6] = 0;   rm[rmOffset + 9] = 0;
//            rm[rmOffset + 5] = 1;
//        } else if (0.0f == x && 0.0f == y && 1.0f == z) {
//            rm[rmOffset + 0] = c;   rm[rmOffset + 5] = c;
//            rm[rmOffset + 1] = s;   rm[rmOffset + 4] = -s;
//            rm[rmOffset + 2] = 0;   rm[rmOffset + 6] = 0;
//            rm[rmOffset + 8] = 0;   rm[rmOffset + 9] = 0;
//            rm[rmOffset + 10]= 1;
//        } else {
//            float len = sqrtf(x * x + y * y + z * z);
//            if (1.0f != len) {
//                float recipLen = 1.0f / len;
//                x *= recipLen;
//                y *= recipLen;
//                z *= recipLen;
//            }
//            float nc = 1.0f - c;
//            float xy = x * y;
//            float yz = y * z;
//            float zx = z * x;
//            float xs = x * s;
//            float ys = y * s;
//            float zs = z * s;
//            rm[rmOffset +  0] = x*x*nc +  c;
//            rm[rmOffset +  4] =  xy*nc - zs;
//            rm[rmOffset +  8] =  zx*nc + ys;
//            rm[rmOffset +  1] =  xy*nc + zs;
//            rm[rmOffset +  5] = y*y*nc +  c;
//            rm[rmOffset +  9] =  yz*nc - xs;
//            rm[rmOffset +  2] =  zx*nc - ys;
//            rm[rmOffset +  6] =  yz*nc + xs;
//            rm[rmOffset + 10] = z*z*nc +  c;
//        }
//}
//
//static float ModelMatrix[4][4] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
//static double atheta = 0;
//
//static float distance(float x1,float x2,float y1,float y2) {
//	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
//}
//
//static void face_render(void *data, float *facepoint, gs_effect_t *effect)
//{
//	struct face_mask_filter_data *filter = data;
//
//	atheta = Rrect_update(facepoint);
//	jzrotate(ModelMatrix,0,atheta+180,0,0,1);
//
//	obs_enter_graphics();
//	gs_image_file_free(&filter->image);
//	obs_leave_graphics();
//
//	face_mask_filter_image_load(filter);
//
//	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
//					     OBS_ALLOW_DIRECT_RENDERING))
//		return;
//
//	float width = facepoint[190];
//	float height = facepoint[191];
//
//	float xcenter = 0;
//	float ycenter = 0;
//	int sticker_width = distance(facepoint[0 * 2] ,facepoint[32 * 2] ,facepoint[0 * 2 + 1],facepoint[32 * 2 + 1]);
//	int sticker_height = distance(facepoint[35 * 2],facepoint[16 * 2],facepoint[35 * 2+1] , facepoint[16 * 2+1]);
//	gs_eparam_t *param;
//	float smatrix00 = ModelMatrix[0][0];
//	float smatrix01 = ModelMatrix[0][1];
//	float smatrix02 = ModelMatrix[0][2];
//	float smatrix03 = ModelMatrix[0][3];
//	float smatrix10 = ModelMatrix[1][0];
//	float smatrix11 = ModelMatrix[1][1];
//	float smatrix12 = ModelMatrix[1][2];
//	float smatrix13 = ModelMatrix[1][3];
//	float smatrix20 = ModelMatrix[2][0];
//	float smatrix21 = ModelMatrix[2][1];
//	float smatrix22 = ModelMatrix[2][2];
//	float smatrix23 = ModelMatrix[2][3];
//	float smatrix30 = ModelMatrix[3][0];
//	float smatrix31 = ModelMatrix[3][1];
//	float smatrix32 = ModelMatrix[3][2];
//	float smatrix33 = ModelMatrix[3][3];
//	gs_effect_set_float(filter->smatrix00_param, smatrix00);
//	gs_effect_set_float(filter->smatrix01_param, smatrix01);
//	gs_effect_set_float(filter->smatrix02_param, smatrix02);
//	gs_effect_set_float(filter->smatrix03_param, smatrix03);
//	gs_effect_set_float(filter->smatrix10_param, smatrix10);
//	gs_effect_set_float(filter->smatrix11_param, smatrix11);
//	gs_effect_set_float(filter->smatrix12_param, smatrix12);
//	gs_effect_set_float(filter->smatrix13_param, smatrix13);
//	gs_effect_set_float(filter->smatrix20_param, smatrix20);
//	gs_effect_set_float(filter->smatrix21_param, smatrix21);
//	gs_effect_set_float(filter->smatrix22_param, smatrix22);
//	gs_effect_set_float(filter->smatrix23_param, smatrix23);
//	gs_effect_set_float(filter->smatrix30_param, smatrix30);
//	gs_effect_set_float(filter->smatrix31_param, smatrix31);
//	gs_effect_set_float(filter->smatrix32_param, smatrix32);
//	gs_effect_set_float(filter->smatrix33_param, smatrix33);
//	param = gs_effect_get_param_by_name(filter->effect, "target");
//	gs_effect_set_texture(param, filter->target);
//
//	if (filter->type == 0) {
//		xcenter = facepoint[51 * 2];
//		ycenter = facepoint[51 * 2 + 1];
//	} else if (filter->type == 1) {
//		xcenter = facepoint[51 * 2];
//		ycenter = facepoint[51 * 2 + 1];
//	} else if (filter->type == 2) {
//		sticker_height = distance(facepoint[51 * 2],facepoint[85* 2],facepoint[51 * 2+1] ,facepoint[85 * 2+1])+50;
//		xcenter = facepoint[57 * 2];
//		ycenter = facepoint[57 * 2 + 1];
//	} else if (filter->type == 3) {
//		xcenter = facepoint[52 * 2];
//		ycenter = (facepoint[52 * 2 + 1]+facepoint[53 * 2 + 1])/2;
//	}
//
//	gs_effect_set_float(filter->xcenter_param, xcenter);
//	gs_effect_set_float(filter->ycenter_param, ycenter);
//	gs_effect_set_float(filter->stickerw_param, sticker_width);
//	gs_effect_set_float(filter->stickerh_param, sticker_height);
//	param = gs_effect_get_param_by_name(filter->effect, "s_width");
//	gs_effect_set_float(param, width);
//	param = gs_effect_get_param_by_name(filter->effect, "s_height");
//	gs_effect_set_float(param, height);
//
//	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
//	UNUSED_PARAMETER(effect);
//}
//
//#define IMAGE_FILTER_EXTENSIONS " (*.bmp *.jpg *.jpeg *.tga *.gif *.png)"
//static obs_properties_t *face_mask_filter_properties(void *data)
//{
//	obs_properties_t *props = obs_properties_create();
//
//	obs_property_t *p;
//	p = obs_properties_add_list(props, SETTING_TYPE, TEXT_TYPE, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
//	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Eyes"), "0");
//	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Glasses"), "1");
//	obs_property_list_add_string(p, obs_module_text("FaceMaskType.Nose"), "2");
//	obs_property_list_add_string(p, obs_module_text("FaceMaskType.GlassesAndNose"), "3");
//
//	struct dstr filter_str = {0};
//	dstr_copy(&filter_str, TEXT_PATH_IMAGES);
//	dstr_cat(&filter_str, IMAGE_FILTER_EXTENSIONS ";;");
//	dstr_cat(&filter_str, TEXT_PATH_ALL_FILES);
//	dstr_cat(&filter_str, " (*.*)");
//	obs_properties_add_path(props, SETTING_IMAGE_PATH, TEXT_IMAGE_PATH, OBS_PATH_FILE, filter_str.array, NULL);
//
//	dstr_free(&filter_str);
//
//	UNUSED_PARAMETER(data);
//	return props;
//}
//
//static void face_mask_filter_defaults(obs_data_t *settings)
//{
//	obs_data_set_default_string(settings, SETTING_TYPE, "0");
//}
//
//struct obs_source_info face_mask_filter = {
//	.id = "face_mask_filter",
//	.type = OBS_SOURCE_TYPE_FILTER,
//	.output_flags = OBS_SOURCE_VIDEO,
//	.get_name = face_mask_filter_getname,
//	.create = face_mask_filter_create,
//	.destroy = face_mask_filter_destroy,
//	.update = face_mask_filter_update,
//	.video_render = face_mask_filter_render,
//	.get_properties = face_mask_filter_properties,
//	.get_defaults = face_mask_filter_defaults,
//	.face_config = true,
//	.video_face_render = face_render,
//};
