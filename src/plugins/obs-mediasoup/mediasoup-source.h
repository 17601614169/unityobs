#pragma once
#include <obs-module.h>

struct mediasoup_input_source {
	uint32_t width;
	uint32_t height;
	obs_source_t *source;
};

void register_mediasoup_source();
