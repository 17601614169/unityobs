#ifndef _OBS_WEBRTC_VIDEO_OBSERVER_H_
#define _OBS_WEBRTC_VIDEO_OBSERVER_H_

#include <mutex>

#include "rtc_base/thread.h"
#include "api/media_stream_interface.h"
#include "api/video/video_sink_interface.h"
#include "mediasoup_interface.h"

struct obs_scene;
struct obs_source;
struct obs_scene_item;
typedef struct obs_source obs_source_t;
typedef struct obs_scene_item obs_sceneitem_t;


class RealtimeIncomingVideoSource
	: public rtc::VideoSinkInterface<webrtc::VideoFrame>,
	  public MediasoupIncommintSourceInterface {
public:
	RealtimeIncomingVideoSource(
		webrtc::VideoTrackInterface *track_to_render,const std::string & source_name,
		const std::string &peerId,int phone);		
	~RealtimeIncomingVideoSource(); 

	void OnClose() override;
	std::string GetSourceName() ;
	void NewComsumerSource(const std::string & sourceName) ;
	void DeleteSources(const std::string& sourceList);
protected:
	// VideoSinkInterface implementation
	void OnFrame(const webrtc::VideoFrame &video_frame) override;

private:
	obs_scene *scene=NULL;
	obs_source_t *scene_source=NULL;
	obs_source_t *source=NULL;
	obs_sceneitem_t *scene_item=NULL;
	rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_;
	std::mutex mutex;
	std::string m_strSourceName;
	std::string m_strPeerId;
	int m_nPhone;
	int m_nFrameWidth;
	int m_nFrameHeight;
};




#endif
