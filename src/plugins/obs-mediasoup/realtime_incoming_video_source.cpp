/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "realtime_incoming_video_source.h"
#include <obs.h>
#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
#include "media/base/media_channel.h"
#include "media/base/video_common.h"

#include "obs-frontend-api.h"
#include "mediasoup-source.h"
#include "base-define.h"
#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define error(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)

RealtimeIncomingVideoSource::RealtimeIncomingVideoSource(
	webrtc::VideoTrackInterface *track_to_render,const std::string & source_name,const std::string &peerId,int phone)
	: video_track_(track_to_render),
	  source(nullptr),
	  m_nPhone(phone),
	  m_strPeerId(peerId)
{
	m_nFrameWidth = 0;
	m_nFrameHeight = 0;
	debug("ObsWebrtcVideoObserver addr:%p, track:%p", this,
	      track_to_render);
	m_strSourceName = source_name;
	video_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

void RealtimeIncomingVideoSource::NewComsumerSource(const std::string& sourceName) {
	#if 0
	STNewConsumer stComsumer;
	stComsumer.phone = m_nPhone;
	
	int minlen = strlen(sourceName.c_str()) >= _countof(stComsumer.name) ? _countof(stComsumer.name) : strlen(sourceName.c_str());
	memcpy(stComsumer.name,sourceName.c_str(),minlen);

	int minPeer = strlen(sourceName.c_str()) >= _countof(stComsumer.peer) ? _countof(stComsumer.peer) : strlen(m_strPeerId.c_str());
	memcpy(stComsumer.peer,m_strPeerId.c_str(),minPeer);
	
	int len = sizeof(STNewConsumer);
	int bufferlen = sizeof(int) + len*sizeof(char);

	char *buffer = new char[bufferlen];
	memcpy(buffer,&len,sizeof(len));
	memcpy(buffer+sizeof(int),&stComsumer,sizeof(stComsumer));
	HWND hId = (HWND)obs_frontend_get_main_window_handle();
	::PostMessageA(hId,MAINWINDOW_NEW_CONSUMER,NULL,(LPARAM)buffer);
	#endif
	
}

std::string RealtimeIncomingVideoSource::GetSourceName() {
	return m_strSourceName;
}

RealtimeIncomingVideoSource::~RealtimeIncomingVideoSource()
{
	video_track_->RemoveSink(this);
	if (source != nullptr) {
		obs_source_release(source);
		source=nullptr;
	}
}

void RealtimeIncomingVideoSource::OnFrame(const webrtc::VideoFrame &video_frame)
{
	std::unique_lock<std::mutex> lock(mutex);

	debug("OnFrame frame:%d, height:%d, width:%d", video_frame.id(),
	      video_frame.height(), video_frame.width());
	
	obs_source_frame frame;
	memset(&frame, 0, sizeof(obs_source_frame));

	rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
		video_frame.video_frame_buffer());

	rtc::scoped_refptr<webrtc::I420BufferInterface> i420_buffer =
		buffer->ToI420();
	
	frame.timestamp = video_frame.render_time_ms();

	//blog(LOG_DEBUG, "frame ts: %llu", videoFrame.renderTimeMs);

	frame.width = video_frame.width();

	frame.height = video_frame.height();

	frame.format = VIDEO_FORMAT_I420;

	frame.data[0] = (uint8_t *)i420_buffer->DataY();

	frame.data[1] = (uint8_t *)i420_buffer->DataU();

	frame.data[2] = (uint8_t *)i420_buffer->DataV();

	frame.linesize[0] = i420_buffer->StrideY();

	frame.linesize[1] = i420_buffer->StrideU();

	frame.linesize[2] = i420_buffer->StrideV();

	if (!frame.color_matrix[0]) {

		enum video_colorspace cs = VIDEO_CS_601;

		video_range_type range = VIDEO_RANGE_PARTIAL;

		video_format_get_parameters(

			cs, range,

			frame.color_matrix,

			frame.color_range_min,

			frame.color_range_max);
	}
	if (this->source == nullptr) {
		webrtc::VideoTrackSourceInterface::Stats stat;
		video_track_->GetSource()->GetStats(&stat);
		source = obs_get_source_by_name(m_strSourceName.c_str());
		while (source!=nullptr) {
			int i = 2;
			char name[128] = {0};
			sprintf(name, "%s%d",m_strSourceName.c_str(),i);
			source = obs_get_source_by_name(name);
			if (source == nullptr) {
				m_strSourceName = std::string(name);
			} else {
				obs_source_release(source);
			}
			i++;
		}
		if (!source) {
			obs_data_t *settings = obs_data_create();
			obs_data_set_int(settings, "width", frame.width);
			obs_data_set_int(settings, "height", frame.height);
			source = obs_source_create("mediasoup_source", m_strSourceName.c_str(), settings, nullptr);
			obs_source_t* output_source = obs_frontend_get_current_scene();
			obs_scene_t *scene = obs_scene_from_source(output_source);
			scene_item = obs_scene_add(scene, source);
			obs_data_release(settings);
			obs_scene_release(scene);
			NewComsumerSource(m_strSourceName+",");
		}
		m_nFrameWidth = frame.width;
		m_nFrameHeight = frame.height;
	}
#if 1
	if (m_nFrameWidth != frame.width || m_nFrameHeight != frame.height) {
		/*obs_data_t* settings =  obs_data_create();
		obs_data_set_int(settings, "width", frame.width);
		obs_data_set_int(settings, "height", frame.height);
		obs_source_update(this->source,settings);
		obs_data_release(settings);
		m_nFrameWidth = frame.width;
		m_nFrameHeight = frame.height;*/
		//blog(LOG_INFO,"first frame width:%d,height:%d--- change width:%d,height:%d",m_nFrameWidth,m_nFrameHeight,frame.width,frame.height);
	}
#endif
	if (this->source != nullptr)
		obs_source_output_video(this->source, &frame);	

}

void RealtimeIncomingVideoSource::OnClose()
{
	
}
