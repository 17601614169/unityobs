#include "realtime_incoming_audio_source.h"
#include <obs.h>

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define error(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)

RealtimeIncomingAudioSource::RealtimeIncomingAudioSource(
	webrtc::AudioTrackInterface *track_to_render)
	: audio_track_(track_to_render)
{
	debug("before AddSink sink:%p", this);
	audio_track_->AddSink(this);
}

RealtimeIncomingAudioSource::~RealtimeIncomingAudioSource()
{
	audio_track_->RemoveSink(this);
}

void RealtimeIncomingAudioSource::OnData(const void *audio_data,
					 int bits_per_sample,
				    int sample_rate,
	    size_t number_of_channels, size_t number_of_frames)
{
	debug("OnData audio_data:%p, bits_per_sample:%d, sample_rate:%d number_of_channels:%d number_of_frames:%d",
	      audio_data, bits_per_sample, sample_rate, number_of_channels,
	      number_of_frames);
}

void RealtimeIncomingAudioSource::OnClose() {}
