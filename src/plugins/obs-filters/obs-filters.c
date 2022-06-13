#include <obs-module.h>
#include "obs-filters-config.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-filters", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "OBS core filters";
}

extern struct obs_source_info mask_filter;
extern struct obs_source_info crop_filter;
extern struct obs_source_info gain_filter;
extern struct obs_source_info color_filter;
extern struct obs_source_info scale_filter;
extern struct obs_source_info scroll_filter;
extern struct obs_source_info gpu_delay_filter;
extern struct obs_source_info color_key_filter;
extern struct obs_source_info color_grade_filter;
extern struct obs_source_info sharpness_filter;
extern struct obs_source_info chroma_key_filter;
extern struct obs_source_info async_delay_filter;
#if NOISEREDUCTION_ENABLED
extern struct obs_source_info noise_suppress_filter;
extern struct obs_source_info noise_suppress_filter_v2;
#endif
extern struct obs_source_info invert_polarity_filter;
extern struct obs_source_info new_mask_face;
extern struct obs_source_info noise_gate_filter;
extern struct obs_source_info compressor_filter;
extern struct obs_source_info limiter_filter;
extern struct obs_source_info expander_filter;
extern struct obs_source_info luma_key_filter;
//extern struct obs_source_info face_mask_filter;
extern struct obs_source_info face_lift_filter;
extern struct obs_source_info blusher_filter;
extern struct obs_source_info eyes_reshape_filter;
extern struct obs_source_info moulting_filter;
extern struct obs_source_info ruddy_filter;
extern struct obs_source_info skin_white_filter;
extern struct obs_source_info thin_chin_filter;
extern struct obs_source_info cocos_filter;
extern struct obs_source_info old_face_mask_filter;

bool obs_module_load(void)
{
	obs_register_source(&mask_filter);
	obs_register_source(&crop_filter);
	obs_register_source(&gain_filter);
	obs_register_source(&color_filter);
	obs_register_source(&scale_filter);
	obs_register_source(&scroll_filter);
	obs_register_source(&gpu_delay_filter);
	obs_register_source(&color_key_filter);
	obs_register_source(&color_grade_filter);
	obs_register_source(&sharpness_filter);
	obs_register_source(&chroma_key_filter);
	obs_register_source(&async_delay_filter);
#if NOISEREDUCTION_ENABLED
	obs_register_source(&noise_suppress_filter);
	obs_register_source(&noise_suppress_filter_v2);
#endif
	obs_register_source(&invert_polarity_filter);
	obs_register_source(&noise_gate_filter);
	obs_register_source(&new_mask_face);
	obs_register_source(&compressor_filter);
	obs_register_source(&limiter_filter);
	obs_register_source(&expander_filter);
	obs_register_source(&luma_key_filter);
	//obs_register_source(&face_mask_filter);
	obs_register_source(&face_lift_filter);
	obs_register_source(&blusher_filter);
	obs_register_source(&eyes_reshape_filter);
	obs_register_source(&moulting_filter);
	obs_register_source(&ruddy_filter);
	obs_register_source(&skin_white_filter);
	obs_register_source(&thin_chin_filter);
	obs_register_source(&cocos_filter);
	obs_register_source(&old_face_mask_filter);
	return true;
}