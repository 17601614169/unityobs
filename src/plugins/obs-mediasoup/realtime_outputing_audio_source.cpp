#include "realtime_outputing_audio_source.h"
#include <obs.h>

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define error(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)

rtc::scoped_refptr<RealtimeOutputingAudioSource>
RealtimeOutputingAudioSource::Create(cricket::AudioOptions *options)
{
 	audio_t *audio = obs_get_audio();

 	if (nullptr == audio) {
 		blog(LOG_ERROR, "Could not retrieve OBS audio source.");
 		return nullptr; }

  	rtc::scoped_refptr<RealtimeOutputingAudioSource> source(
 		new rtc::RefCountedObject<RealtimeOutputingAudioSource>());
 	source->Initialize(audio, options);
 	return source;
}

void RealtimeOutputingAudioSource::AddSink(webrtc::AudioTrackSinkInterface *sink) {
	rtc::CritScope lock(&_critSect);
	if (nullptr != sink_) {
 		blog(LOG_WARNING, "Replacing audio sink..."); }

  	sink_ = sink;
}

void RealtimeOutputingAudioSource::RemoveSink(webrtc::AudioTrackSinkInterface *sink) {
	rtc::CritScope lock(&_critSect);
 	if (sink_ != sink) {
 		blog(LOG_WARNING, "Attempting to remove unassigned sink...");
 		return; }

  	sink_ = nullptr;
}

void RealtimeOutputingAudioSource::OnAudioData(audio_data *frame)
{
	rtc::CritScope lock(&_critSect);
 	webrtc::AudioTrackSinkInterface *sink = this->sink_;
 	if (nullptr == sink) {
 		return;
 	}

        // NOTE ALEX: should double check frame before -> it
  	uint8_t *data         = frame->data[0];
 	size_t   num_channels = audio_output_get_channels(audio_);
 	//uint32_t sample_rate  = 48000
	uint32_t sample_rate = audio_output_get_sample_rate(audio_);
 	size_t   chunk        = (sample_rate / 100);
 	//size_t   sample_size  = 2;
	size_t sample_size = audio_output_get_block_size(audio_);
 	size_t   i            = 0;
 	uint8_t *position;

  	if (pending_remainder) {
 		// Copy missing chunks
 		i = chunk - pending_remainder;
 		memcpy(pending + pending_remainder * sample_size * num_channels,
                       data,
 		       i * sample_size * num_channels
                      );
		info("audio.data=%p sample_rate=%d num_channels=%d number_of_frames=%d",
		     pending, sample_rate, num_channels, chunk);
		
  		// Send
 		sink->OnData(pending, 16, sample_rate, num_channels, chunk);		
  		// No pending chunks
 		pending_remainder = 0;
 	}

  	while (i + chunk < frame->frames) {
 		position = data + i * sample_size * num_channels;
		info("audio.data=%p sample_rate=%d num_channels=%d number_of_frames=%d",
		     position, sample_rate, num_channels, chunk);
 		sink->OnData(position, 16, sample_rate, num_channels, chunk);
 		i += chunk;
 	}

  	if (i != frame->frames) {
 		pending_remainder = frame->frames - i;
 		memcpy(pending,
                       data + i * sample_size * num_channels,
 		       pending_remainder * sample_size * num_channels
                      );
 	}
}

RealtimeOutputingAudioSource::RealtimeOutputingAudioSource() { sink_ = nullptr; }

RealtimeOutputingAudioSource::~RealtimeOutputingAudioSource() { free(pending); }

void RealtimeOutputingAudioSource::Initialize(audio_t *audio, cricket::AudioOptions *options)
{
	rtc::CritScope lock(&_critSect);
 	// TODO: Null-check audio
 	audio_ = audio;
 	options_ = *options;

  	size_t num_channels = audio_output_get_channels(audio_);
 	size_t pending_len = num_channels * 2 * 640;
 	pending = (uint8_t *)malloc(pending_len);
 	pending_remainder = 0;
 }
