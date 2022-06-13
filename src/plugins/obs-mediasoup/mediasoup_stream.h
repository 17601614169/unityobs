#ifndef _MEDIASOUPSTREAM_H_
#define _MEDIASOUPSTREAM_H_

#if WIN32
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Msdmo.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "amstrmid.lib")
#endif

#pragma comment(lib, "webrtc.lib")

// lib obs includes
#include "obs.h"

// obs-webrtc includes
#include "WebsocketClient.h"
#include "realtime_outputing_video_source.h"
#include "audio_device_module_wrapper.h"
#include "mediasoup_interface.h"


// webrtc includes
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/rtc_error.h"
#include "api/scoped_refptr.h"
#include "api/set_remote_description_observer_interface.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "rtc_base/critical_section.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/thread.h"
#include "rtc_base/timestamp_aligner.h"

// mediasoup client includes
#include "mediasoupclient.hpp"
#include "nlohmann/json.hpp"
#include "base-define.h"
// std lib
#include <initializer_list>
#include <regex>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

class RealtimeOutputingAudioSource;
class RealtimeIncomingVideoSource;
class RealtimeIncomingAudioSource;
typedef std::unordered_map<std::string, mediasoupclient::Producer *> TDMapProducer;
struct DeviceInfo {
	std::string flag;
	std::string name;
	std::string version;

	DeviceInfo(const std::string &flag, const std::string &name,
		   const std::string &version)
		: flag(flag), name(name), version(version)
	{
	}
	nlohmann::json toJson()
	{
		nlohmann::json data = {
			{"flag", flag}, {"name", name}, {"version", version}};

		return data;
	}
};

class MediasoupStreamInterface
	: public WebsocketClient::Listener,
	  public mediasoupclient::SendTransport::Listener,
	  public mediasoupclient::Producer::Listener,
	  public mediasoupclient::Consumer::Listener,
	  public mediasoupclient::DataProducer::Listener,
	  public mediasoupclient::DataConsumer::Listener {
};

class MediasoupStream : public MediasoupStreamInterface {
public:
	/*struct TimerKiller {
		// returns false if killed:
		template<class R, class P>
		bool WaitFor(std::chrono::duration<R, P> const &time) const
		{
			std::unique_lock<std::mutex> lock(m);
			return !cv.wait_for(lock, time,
					    [&] { return terminate; });
		}
		void Kill()
		{
			std::unique_lock<std::mutex> lock(m);
			terminate =
				true; // Should be modified inside mutex lock.
			cv.notify_all(); // It is safe, and *sometimes* optimal, to do this outside the lock.
		}

	private:
		mutable std::condition_variable cv;
		mutable std::mutex m;
		bool terminate = false;
	};*/
	struct ConsumerNode
	{
		mediasoupclient::Consumer *consumer;
		MediasoupIncommintSourceInterface *incomming_source;
	};
	typedef std::unordered_map<std::string, ConsumerNode*> ConsumerNodeList;

	struct DataConsumerNode {
		mediasoupclient::DataConsumer *consumer;
		MediasoupIncommintSourceInterface *incomming_source;
	};
	typedef std::unordered_map<std::string, DataConsumerNode*> DataConsumerNodeList;

public:
	MediasoupStream(obs_output_t *output);
	~MediasoupStream();

	bool close(bool wait);
	bool start();
	bool stop();
	void onAudioFrame(audio_data *frame);
	void onVideoFrame(video_data *frame);
	void setCodec(const std::string &new_codec)
	{
		this->video_codec = new_codec;
	}

	//
	// WebsocketClient::Listener implementation.
	//
	void onConnected() override;
	void onDisconnected() override;
	void onConnectedFail(int code) override;
	void onRequest(int id, const std::string &method,
		       const nlohmann::json &body) override;
	void onNotification(const std::string &method,
			    const nlohmann::json &body) override;
	
	// NOTE LUDO: #80 add getStats
	// WebRTC stats
	void getStats();
	const char *get_stats_list() { return stats_list.c_str(); }
	// Bitrate & dropped frames
	uint64_t getBitrate() { return total_bytes_sent; }
	int getDroppedFrames() { return pli_received; }
	// Synchronously get stats
	rtc::scoped_refptr<const webrtc::RTCStatsReport> NewGetStats();

	template<typename T> rtc::scoped_refptr<T> make_scoped_refptr(T *t)
	{
		return rtc::scoped_refptr<T>(t);
	}

	void Pause(const char* kind);
	void Resume(const char* kind);
	void SignalProtocol(const char *method,const char* key,const char* value);
	/* Virtual methods inherited from SendTransport::Listener. */
public:
	std::future<void>
	OnConnect(mediasoupclient::Transport *transport,
		  const nlohmann::json &dtlsParameters) override;
	void
	OnConnectionStateChange(mediasoupclient::Transport *transport,
				const std::string &connectionState) override;
	std::future<std::string>
	OnProduce(mediasoupclient::SendTransport *transport,
		  const std::string &kind, nlohmann::json rtpParameters,
		  const nlohmann::json &appData) override;
	/*void
	OnProduce(mediasoupclient::SendTransport *transport,
		  mediasoupclient::Producer::Listener *producerListener,
		  const mediasoupclient::SendHandler::SendResult &sendResult,
		  webrtc::MediaStreamTrackInterface *track,
		  const nlohmann::json &appData) override;*/

	std::future<std::string>
	OnProduceData(mediasoupclient::SendTransport *transport,
		      const nlohmann::json &sctpStreamParameters,
		      const std::string &label, const std::string &protocol,
		      const nlohmann::json &appData) override;

	/* Virtual methods inherited from Producer::Listener. */
public:
	void OnTransportClose(mediasoupclient::Producer *producer) override;

	/* Virtual methods inherited from Consumer::Listener. */
public:
	void OnTransportClose(mediasoupclient::Consumer *consumer) override;

	/* Virtual methods inherited from DataConsumer::Listener */
public:
	void OnMessage(mediasoupclient::DataConsumer *dataConsumer,
		       const webrtc::DataBuffer &buffer) override;
	void OnConnecting(mediasoupclient::DataConsumer *dataConsumer) override
	{
	}
	void OnClosing(mediasoupclient::DataConsumer *dataConsumer) override {}
	void OnClose(mediasoupclient::DataConsumer *dataConsumer) override {}
	void OnOpen(mediasoupclient::DataConsumer *dataConsumer) override {}
	void
	OnTransportClose(mediasoupclient::DataConsumer *dataConsumer) override;

	/* Virtual methods inherited from DataProducer::Listener */
public:
	void OnOpen(mediasoupclient::DataProducer *dataProducer) override;
	void OnClose(mediasoupclient::DataProducer *dataProducer) override;
	void OnBufferedAmountChange(mediasoupclient::DataProducer *dataProducer,
				    uint64_t size) override;
	void
	OnTransportClose(mediasoupclient::DataProducer *dataProducer) override;

private:
	std::future<void>
	OnConnectSendTransport(const nlohmann::json &dtlsParameters);
	std::future<void>
	OnConnectRecvTransport(const nlohmann::json &dtlsParameters);

	void CreateSendTransport();
	void CreateRecvTransport();
	void CreateDataProducer();
	void CreateDataConsumer();
	void Join();
	void AfterJoin();

private:
	void Accept(int id, const nlohmann::json &data);
	void Reject(int id, int errorCode, const std::string &errorReson);

	std::string onNewConsumer(const nlohmann::json &body);
	std::string onNewDataConsumer(const nlohmann::json &body);
	std::string OnPeerClosed(const nlohmann::json &body);
	std::string OnNewPeer(const nlohmann::json &body);
 public:
	void DeleteSources(const std::string& sourceList);
	void DeletePeer(const std::string& sourceList);
private:
	DeviceInfo deviceInfo;
	mediasoupclient::Device* device;
	mediasoupclient::PeerConnection::Options option;

	mediasoupclient::SendTransport *sendTransport{nullptr};
	mediasoupclient::RecvTransport *recvTransport{nullptr};
	mediasoupclient::DataProducer *dataProducer{nullptr};
	mediasoupclient::DataConsumer *dataConsumer{nullptr};

	//struct TimerKiller timerKiller;

	TDMapProducer mapProducers;

	ConsumerNodeList mapConsumers;

	DataConsumerNodeList mapDataConsumers;

protected:
	// Connection properties
	//Type type;
	int audio_bitrate;
	int video_bitrate;
	std::string url;
	std::string room;
	std::string username;
	std::string password;
	std::string protocol;
	std::string audio_codec;
	std::string video_codec;
	int channel_count;

	void resetStats();

	// NOTE LUDO: #80 add getStats
	std::string stats_list;
	uint16_t frame_id;
	uint64_t audio_bytes_sent;
	uint64_t video_bytes_sent;
	uint64_t total_bytes_sent;
	int pli_received;
	// Used to compute fps
	// NOTE ALEX: Should be initialized in constructor.
	std::chrono::system_clock::time_point previous_time =
		std::chrono::system_clock::time_point(
			std::chrono::duration<int>(0));
	uint32_t previous_frames_sent = 0;

	std::thread thread_closeAsync;

	rtc::CriticalSection crit_;

	// Audio Wrapper
	rtc::scoped_refptr<AudioDeviceModuleWrapper> adm;

	// Video Capturer
	rtc::scoped_refptr<RealtimeOutputingVideoSource> videoCapturer;
	rtc::TimestampAligner timestamp_aligner_;

	// PeerConnection
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;

	// SetRemoteDescription observer
	rtc::scoped_refptr<webrtc::SetRemoteDescriptionObserverInterface>
		srd_observer;

	// Media stream
	//rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;

	// Webrtc source
	typedef std::unique_ptr<RealtimeIncomingVideoSource>
		RealtimeIncomingVideoSourcePointer;
	typedef std::vector<RealtimeIncomingVideoSourcePointer>
		RealtimeIncomingVideoSourceVector;
	RealtimeIncomingVideoSourceVector incomming_videos;

	typedef std::unique_ptr<RealtimeIncomingAudioSource>
		RealtimeIncomingAudioSourcePointer;
	typedef std::vector<RealtimeIncomingAudioSourcePointer>
		RealtimeIncomingAudioSourceVector;
	RealtimeIncomingAudioSourceVector incomming_audios;

	// Webrtc Source that wraps an OBS capturer
	rtc::scoped_refptr<RealtimeOutputingAudioSource> audio_source;

	//std::unique_ptr<webrtc::WinObsVideoEncoderFactory> obs_encoder_factory;

	// Tracks
	rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track;
	rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;

	// WebRTC threads
	std::unique_ptr<rtc::Thread> network;
	std::unique_ptr<rtc::Thread> worker;
	std::unique_ptr<rtc::Thread> signaling;

	// Websocket client
	WebsocketClient *client;

	// OBS stream output
	obs_output_t *output;

	std::map<std::string,std::string> m_mapPeerId2SourceName;
	std::map<std::string,int> m_mapPeerId2Phone;
};

#endif
