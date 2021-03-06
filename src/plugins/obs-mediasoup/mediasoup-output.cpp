// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include <stdio.h>
#include <obs-module.h>
#include <obs-avc.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <util/threading.h>
#include <inttypes.h>
#include <modules/audio_processing/include/audio_processing.h>

#define warn(format, ...)  blog(LOG_WARNING, format, ##__VA_ARGS__)
#define info(format, ...)  blog(LOG_INFO,    format, ##__VA_ARGS__)
#define debug(format, ...) blog(LOG_DEBUG,   format, ##__VA_ARGS__)

#define OPT_DROP_THRESHOLD "drop_threshold_ms"
#define OPT_PFRAME_DROP_THRESHOLD "pframe_drop_threshold_ms"
#define OPT_MAX_SHUTDOWN_TIME_SEC "max_shutdown_time_sec"
#define OPT_BIND_IP "bind_ip"
#define OPT_NEWSOCKETLOOP_ENABLED "new_socket_loop_enabled"
#define OPT_LOWLATENCY_ENABLED "low_latency_mode_enabled"

#include "mediasoup_stream.h"

extern "C" const char *mediasoup_stream_getname(void *unused)
{
	info("mediasoup_stream_getname");
	UNUSED_PARAMETER(unused);
	return obs_module_text("MEDIASOUPStream");
}

extern "C" void mediasoup_stream_destroy(void *data)
{
	info("mediasoup_stream_destroy");
	
	//Get stream
	MediasoupStream *stream = (MediasoupStream*)data;
	//Stop it
	stream->stop();
	//Remove ref and let it self destroy
	delete stream;
	stream = nullptr;
}

extern "C" void *mediasoup_stream_create(obs_data_t *settings, obs_output_t *output)
{
	info("mediasoup_stream_create");
	//Create new stream
	MediasoupStream *stream = new MediasoupStream(output);
	//Don't allow it to be deleted
	//stream->AddRef();
	//Return it
	return (void *)stream;	
}

extern "C" void mediasoup_stream_stop(void *data, uint64_t ts)
{
	info("mediasoup_stream_stop");
	UNUSED_PARAMETER(ts);
	//Get stream
	MediasoupStream *stream = (MediasoupStream*)data;
	//Stop it
	stream->stop();
	//Remove ref and let it self destroy
	//stream->Release();
}

extern "C" bool mediasoup_stream_start(void *data)
{
	info("mediasoup_stream_start");
	//Get stream
	MediasoupStream *stream = (MediasoupStream*)data;
	//Don't allow it to be deleted
	//stream->AddRef();
	//Start it
	return stream->start();
}

extern "C" void mediasoup_receive_video(void *data, struct video_data *frame)
{
	//Get stream
	MediasoupStream *stream = (MediasoupStream*)data;
	//Process audio
	stream->onVideoFrame(frame);
}
extern "C" void mediasoup_receive_audio(void *data, struct audio_data *frame)
{
	//Get stream
	MediasoupStream *stream = (MediasoupStream*)data;
	//Process audio
	stream->onAudioFrame(frame);
}

extern "C" void mediasoup_stream_defaults(obs_data_t *defaults)
{
	info("mediasoup_stream_defaults");
	obs_data_set_default_int(defaults, OPT_DROP_THRESHOLD, 700);
	obs_data_set_default_int(defaults, OPT_PFRAME_DROP_THRESHOLD, 900);
	obs_data_set_default_int(defaults, OPT_MAX_SHUTDOWN_TIME_SEC, 30);
	obs_data_set_default_string(defaults, OPT_BIND_IP, "default");
	obs_data_set_default_bool(defaults, OPT_NEWSOCKETLOOP_ENABLED, false);
	obs_data_set_default_bool(defaults, OPT_LOWLATENCY_ENABLED, false);
}

extern "C" obs_properties_t *mediasoup_stream_properties(void *data)
{
	info("mediasoup_stream_properties");
	UNUSED_PARAMETER(data);

	obs_properties_t *props = obs_properties_create();

	obs_properties_add_int(props, OPT_DROP_THRESHOLD,
			obs_module_text("MEDIASOUPStream.DropThreshold"),
			200, 10000, 100);

	obs_properties_add_bool(props, OPT_NEWSOCKETLOOP_ENABLED,
			obs_module_text("MEDIASOUPStream.NewSocketLoop"));
	obs_properties_add_bool(props, OPT_LOWLATENCY_ENABLED,
			obs_module_text("MEDIASOUPStream.LowLatencyMode"));

	return props;
}

// NOTE LUDO: #80 add getStats
extern "C" void mediasoup_stream_get_stats(void *data)
{
  // Get stream
	//MediasoupStream* stream = (MediasoupStream*) data;
	//stream->getStats();
}

extern "C" const char *mediasoup_stream_get_stats_list(void *data)
{
  // Get stream
	//MediasoupStream* stream = (MediasoupStream*) data;
	//return stream->get_stats_list();
	return "";
}

extern "C" uint64_t mediasoup_stream_total_bytes_sent(void *data)
{
	//Get stream
	MediasoupStream *stream = (MediasoupStream*) data;
	return stream->getBitrate();
	return 0;
}

extern "C" int mediasoup_stream_dropped_frames(void *data)
{
	//Get stream
	MediasoupStream *stream = (MediasoupStream*) data;
	return stream->getDroppedFrames();
	return 0;
}

extern "C" float mediasoup_stream_congestion(void *data)
{
	UNUSED_PARAMETER(data);
	return 0.0f;
}

extern "C" void mediasoup_stream_pause(void *data, const char *kind)
{
	MediasoupStream *stream = (MediasoupStream*) data;
	stream->Pause(kind);
}

extern "C" void mediasoup_stream_resume(void *data, const char *kind)
{
	MediasoupStream *stream = (MediasoupStream*) data;
	stream->Resume(kind);
}

extern "C" void mediasoup_signal_protocol(void *data, const char *method,const char* key,const char* value)
{
	MediasoupStream *stream = (MediasoupStream*) data;
	stream->SignalProtocol(method,key,value);
}

extern "C" {
#ifdef _WIN32
	struct obs_output_info mediasoup_output_info = {
		"mediasoup_output", //id
		OBS_OUTPUT_AV | OBS_OUTPUT_SERVICE|OBS_OUTPUT_CAN_PAUSE, //flags
		mediasoup_stream_getname, //get_name
		mediasoup_stream_create, //create
		mediasoup_stream_destroy, //destroy
		mediasoup_stream_start, //start
		mediasoup_stream_stop, //stop
		mediasoup_receive_video, //raw_video
		mediasoup_receive_audio, //raw_audio
		nullptr, //encoded_packet
		nullptr, //update
		mediasoup_stream_defaults, //get_defaults
		mediasoup_stream_properties, //get_properties
		nullptr, //unused1 (formerly pause)
		// NOTE LUDO: #80 add getStats
		mediasoup_stream_get_stats,
		mediasoup_stream_get_stats_list,
		mediasoup_stream_total_bytes_sent, //get_total_bytes
		mediasoup_stream_dropped_frames, //get_dropped_frames
		nullptr, //type_data
		nullptr, //free_type_data
		mediasoup_stream_congestion, //get_congestion
		nullptr, //get_connect_time_ms
		"vp8", //encoded_video_codecs
		"opus", //encoded_audio_codecs
		nullptr, //raw_audio2,
		mediasoup_stream_pause,
		mediasoup_stream_resume,
		mediasoup_signal_protocol
	};
#else
	struct obs_output_info mediasoup_output_info = {
		.id                   = "mediasoup_output",
		.flags                = OBS_OUTPUT_AV | OBS_OUTPUT_SERVICE,
		.get_name             = mediasoup_stream_getname,
		.create               = mediasoup_stream_create,
		.destroy              = mediasoup_stream_destroy,
		.start                = mediasoup_stream_start,
		.stop                 = mediasoup_stream_stop,
		.raw_video            = mediasoup_receive_video,
		.raw_audio            = mediasoup_receive_audio, //for single-track
		.encoded_packet       = nullptr,
		.update               = nullptr,
		.get_defaults         = mediasoup_stream_defaults,
		.get_properties       = mediasoup_stream_properties,
		.unused1              = nullptr,
    // NOTE LUDO: #80 add getStats
    .get_stats            = mediasoup_stream_get_stats,
    .get_stats_list       = mediasoup_stream_get_stats_list,
		.get_total_bytes      = mediasoup_stream_total_bytes_sent,
		.get_dropped_frames   = mediasoup_stream_dropped_frames,
		.type_data            = nullptr,
		.free_type_data       = nullptr,
		.get_congestion       = mediasoup_stream_congestion,
		.get_connect_time_ms  = nullptr,
		.encoded_video_codecs = "vp8",
		.encoded_audio_codecs = "opus",
		.raw_audio2           = nullptr,
		.pause		      = mediasoup_stream_pause,
		.resume		      = mediasoup_stream_resume
		// .raw_audio2           = mediasoup_receive_multitrack_audio, //for multi-track
	};
#endif
}
