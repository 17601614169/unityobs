#ifndef _MEDIASOUP_AUDIO_RENDER_H_
#define _MEDIASOUP_AUDIO_RENDER_H_


#include "api/media_stream_interface.h"
#include "mediasoup_interface.h"

class RealtimeIncomingAudioSource
	: private webrtc::AudioTrackSinkInterface,
	  public MediasoupIncommintSourceInterface {
public:
	static rtc::scoped_refptr<RealtimeIncomingAudioSource>
	create(rtc::scoped_refptr<webrtc::AudioTrackInterface> &&, std::string &&);

public:
	RealtimeIncomingAudioSource(
		webrtc::AudioTrackInterface *track_to_render);
	~RealtimeIncomingAudioSource() override;

	void OnClose() override;

private:
	// webrtc::AudioTrackSinkInterface API
	void OnData(const void *audio_data, int bits_per_sample,
		    int sample_rate, size_t number_of_channels,
		    size_t number_of_frames) override;


private:
	rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track_;
};

#endif
