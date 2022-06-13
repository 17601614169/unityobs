/* Copyright Dr. Alex. Gouaillard (2015, 2020) */

#include "realtime_outputing_video_source.h"

RealtimeOutputingVideoSource::RealtimeOutputingVideoSource() {}

RealtimeOutputingVideoSource::~RealtimeOutputingVideoSource() = default;

void RealtimeOutputingVideoSource::OnFrameCaptured(const webrtc::VideoFrame & frame)
{
    rtc::AdaptedVideoTrackSource::OnFrame(frame);
}
