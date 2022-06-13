#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-mediasoup", "en-US")

extern struct obs_output_info mediasoup_output_info;

bool obs_module_load()
{
	obs_register_output(&mediasoup_output_info);
	register_mediasoup_source();
	return true;
}


void obs_module_unload() {}
