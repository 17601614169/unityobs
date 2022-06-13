// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include "mediasoup_stream.h"
#include "realtime_outputing_audio_source.h"
#include "realtime_incoming_video_source.h"
#include "realtime_incoming_audio_source.h"

#include "media-io/video-io.h"

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video/i420_buffer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "pc/rtc_stats_collector.h"
#include "rtc_base/checks.h"
#include "rtc_base/critical_section.h"
#include "obs-frontend-api.h"
#include "base-define.h"
#include <libyuv.h>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <memory>
#include <mutex>
#include <thread>
#include <algorithm>
#include <locale>
#include <iostream>
#include "base-define.h"
using json = nlohmann::json;

#ifndef min
#define min
#endif
#ifndef max
#define max
#endif
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <gdiplusbitmap.h>
using namespace Gdiplus;
HANDLE upload_cover_handle = INVALID_HANDLE_VALUE;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
#if 0
static void DumpCover(uint8_t* data,int len,int width,int height){
	if (upload_cover_handle == INVALID_HANDLE_VALUE || upload_cover_handle == 0 ) {
		upload_cover_handle = OpenMutexA(SYNCHRONIZE, false, UPLOAD_MUTEX_NAME.c_str());
		bool alreadyExist = !!upload_cover_handle;
		if (!alreadyExist)
			alreadyExist = CreateMutexA(nullptr, false, UPLOAD_MUTEX_NAME.c_str());
	}
	if (upload_cover_handle != INVALID_HANDLE_VALUE && upload_cover_handle != 0 ) {
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(upload_cover_handle,10000);
		if (dwWaitResult == WAIT_OBJECT_0) {
			Gdiplus::Bitmap bitmap(width, height, width * 3, PixelFormat24bppRGB, data);
			//bitmap.RotateFlip(Gdiplus::Rotate180FlipY);
			CLSID pngClsid;
			if (GetEncoderClsid(L"image/png", &pngClsid) != -1) {
				WCHAR *filename = L"cover.png";
				bitmap.Save(filename, &pngClsid);
			}
			free(data);
			ReleaseMutex(upload_cover_handle);
		} else {
			ReleaseMutex(upload_cover_handle);
		}
	}
}
#endif 

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define error(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)

class StatsCallback : public webrtc::RTCStatsCollectorCallback {
public:
	rtc::scoped_refptr<const webrtc::RTCStatsReport> report()
	{
		return report_;
	}
	bool called() const { return called_; }

protected:
	void OnStatsDelivered(
		const rtc::scoped_refptr<const webrtc::RTCStatsReport> &report)
		override
	{
		report_ = report;
		called_ = true;
	}

private:
	bool called_ = false;
	rtc::scoped_refptr<const webrtc::RTCStatsReport> report_;
};

class CustomLogger : public rtc::LogSink {
public:
	void OnLogMessage(const std::string &message) override
	{
		//info("%s", message.c_str());
	}
};

CustomLogger logger;

MediasoupStream::MediasoupStream(obs_output_t *output)
	: deviceInfo("obs", "obs-device", "1.0"), device(nullptr)
{
	rtc::LogMessage::RemoveLogToStream(&logger);
	rtc::LogMessage::AddLogToStream(&logger,
					rtc::LoggingSeverity::LS_ERROR);

	auto logLevel = mediasoupclient::Logger::LogLevel::LOG_TRACE;
	mediasoupclient::Logger::SetLogLevel(logLevel);
	mediasoupclient::Logger::SetDefaultHandler();

	// Initilize mediasoupclient.
	mediasoupclient::Initialize();

	resetStats();

	audio_bitrate = 128;
	video_bitrate = 2500;

	// Store output
	this->output = output;
	this->client = nullptr;	

	// Network thread
	network = rtc::Thread::CreateWithSocketServer();
	network->SetName("network", nullptr);
	network->Start();

	// Worker thread
	worker = rtc::Thread::Create();
	worker->SetName("worker", nullptr);
	worker->Start();

	// Signaling thread
	signaling = rtc::Thread::Create();
	signaling->SetName("signaling", nullptr);
	signaling->Start();

	// Create audio device module
	// NOTE ALEX: check if we still need this
	//adm = AudioDeviceModuleWrapper::Create();
	adm = worker->Invoke<rtc::scoped_refptr<AudioDeviceModuleWrapper>>(
		RTC_FROM_HERE, [=]() {
			auto audio_device = AudioDeviceModuleWrapper::Create();
			audio_device->Init();
			return audio_device;
		});

	audio_t *audio = obs_get_audio();
	size_t num_channels = audio_output_get_channels(audio);
	uint32_t sample_rate = audio_output_get_sample_rate(audio);
	//size_t sample_size = audio_output_get_block_size(audio
	size_t sample_size = 2;
	AudioDeviceModuleWrapper::SetInputParam(num_channels, sample_rate,
						sample_size);

	factory = webrtc::CreatePeerConnectionFactory(
		network.get(), worker.get(), signaling.get(), adm,
		webrtc::CreateBuiltinAudioEncoderFactory(),
		webrtc::CreateBuiltinAudioDecoderFactory(),
		webrtc::CreateBuiltinVideoEncoderFactory(),
		//std::make_unique<webrtc::WinObsVideoEncoderFactory>(),
		webrtc::CreateBuiltinVideoDecoderFactory(), nullptr, nullptr);

	this->option.factory = factory;

	// Create video capture module
	videoCapturer =
		new rtc::RefCountedObject<RealtimeOutputingVideoSource>();
}

MediasoupStream::~MediasoupStream()
{
	rtc::LogMessage::RemoveLogToStream(&logger);

	// Shutdown websocket connection and close Peer Connection
	close(false);

	worker->Invoke<void>(RTC_FROM_HERE, [this]() {
		this->adm.release();
		this->adm = nullptr;
	});
	// Free factories
	//adm = nullptr;
	factory.release();
	factory = nullptr;
	videoCapturer.release();
	videoCapturer = nullptr;

	// Stop all threads
	if (!network->IsCurrent())
		network->Stop();
	if (!worker->IsCurrent())
		worker->Stop();
	if (!signaling->IsCurrent())
		signaling->Stop();
	/**
	network.release();
	worker.release();
	signaling.release();
	*/
	network.reset();
	worker.reset();
	signaling.reset();

	if (thread_closeAsync.joinable())
		thread_closeAsync.detach();
}

void MediasoupStream::resetStats()
{
	stats_list = "";
	frame_id = 0;
	pli_received = 0;
	audio_bytes_sent = 0;
	video_bytes_sent = 0;
	total_bytes_sent = 0;
	previous_frames_sent = 0;
}

bool MediasoupStream::start()
{
	info("MediasoupStream::start");
	//this->type = type;

	resetStats();

	// Access service if started, or fail

	obs_service_t *service = obs_output_get_service(output);
	if (!service) {
		obs_output_set_last_error(
			output,
			"An unexpected error occurred during stream startup.");
		obs_output_signal_stop(output, OBS_OUTPUT_CONNECT_FAILED);
		return false;
	}

	// Extract setting from service

	url = obs_service_get_url(service) ? obs_service_get_url(service) : "";
	room = obs_service_get_room(service) ? obs_service_get_room(service)
					     : "";
	username = obs_service_get_username(service)
			   ? obs_service_get_username(service)
			   : "";
	password = obs_service_get_password(service)
			   ? obs_service_get_password(service)
			   : "";
	video_codec = obs_service_get_codec(service)
			      ? obs_service_get_codec(service)
			      : "";
	protocol = obs_service_get_protocol(service)
			   ? obs_service_get_protocol(service)
			   : "";
	// Some extra log
	info("Video codec: %s",
	     video_codec.empty() ? "Automatic" : video_codec.c_str());
	info("Protocol:    %s",
	     protocol.empty() ? "Automatic" : protocol.c_str());

	// Stream setting sanity check

	bool isServiceValid = true;
	if (url.empty()) {
		warn("Invalid url");
		isServiceValid = false;
	}

	if (room.empty()) {
		warn("Missing room ID");
		isServiceValid = false;
	}

	if (!isServiceValid) {
		obs_output_set_last_error(
			output,
			"Your service settings are not complete. Open the settings => stream window and complete them.");
		obs_output_signal_stop(output, OBS_OUTPUT_CONNECT_FAILED);
		return false;
	}

	// Set up encoders.
	// NOTE ALEX: should not be done for webrtc.

	obs_output_t *context = output;

	obs_encoder_t *aencoder = obs_output_get_audio_encoder(context, 0);
	obs_data_t *asettings = obs_encoder_get_settings(aencoder);
	audio_bitrate = (int)obs_data_get_int(asettings, "bitrate");
	obs_data_release(asettings);

	obs_encoder_t *vencoder = obs_output_get_video_encoder(context);
	obs_data_t *vsettings = obs_encoder_get_settings(vencoder);
	video_bitrate = (int)obs_data_get_int(vsettings, "bitrate");
	obs_data_release(vsettings);

	struct obs_audio_info audio_info;
	if (!obs_get_audio_info(&audio_info)) {
		warn("Failed to load audio settings.  Defaulting to opus.");
		audio_codec = "opus";
	} else {
		// NOTE ALEX: if input # channel > output we should down mix
		//            if input is > 2 but < 6 we might have a porblem with multiopus.
		channel_count = (int)(audio_info.speakers);
		audio_codec = channel_count <= 2 ? "opus" : "multiopus";
	}

	// Shutdown websocket connection and close Peer Connection (just in case)
	if (close(false)) {
		blog(LOG_INFO,"MediasoupStream start close return true");
		obs_output_signal_stop(output, OBS_OUTPUT_WEBSOCKET_SHUTDOWN);
	}
		

	cricket::AudioOptions options;
	options.echo_cancellation.emplace(false); // default: true
	options.auto_gain_control.emplace(false); // default: true
	options.noise_suppression.emplace(false); // default: true
	options.highpass_filter.emplace(false);   // default: true
	options.stereo_swapping.emplace(false);
	options.typing_detection.emplace(false); // default: true
	options.experimental_agc.emplace(false);
	// m79 options.extended_filter_aec.emplace(false);
	// m79 options.delay_agnostic_aec.emplace(false);
	options.experimental_ns.emplace(false);
	options.residual_echo_detector.emplace(false); // default: true
	// options.tx_agc_limiter.emplace(false);

	//stream = factory->CreateLocalMediaStream("obs");

	audio_source = RealtimeOutputingAudioSource::Create(&options);
	audio_track = factory->CreateAudioTrack("audio", audio_source);
	//stream->AddTrack(audio_track);

	video_track = factory->CreateVideoTrack("video", videoCapturer);
	//stream->AddTrack(video_track);

	client = createWebsocketClient();
	if (!client) {
		warn("Error creating Websocket client");
		// Close Peer Connection
		close(false);
		// Disconnect, this will call stop on main thread
		obs_output_set_last_error(
			output,
			"There was a problem creating the websocket connection.  Are you behind a firewall?");
		obs_output_signal_stop(output, OBS_OUTPUT_CONNECT_FAILED);
		return false;
	}
	std::string userid = obs_service_get_userid(service)
				     ? obs_service_get_userid(service)
				     : "";
	// Extra logging
	info("CONNECTING TO %s, userid:%s", url.c_str(), userid.c_str());

	// Connect to the signalling server
	if (!client->Connect(url, room, userid, this)) {
		warn("Error connecting to server");
		// Shutdown websocket connection and close Peer Connection
		close(false);
		// Disconnect, this will call stop on main thread
		obs_output_set_last_error(
			output, "There was a problem connecting to your room.");
		obs_output_signal_stop(output, OBS_OUTPUT_CONNECT_FAILED);
		return false;
	}
	return true;
}

void MediasoupStream::onConnected()
{
	info("MediasoupStream::onConnected");

	nlohmann::json body;
	auto data =
		this->client->Request("getRouterRtpCapabilities", body).get();

	// Load the device.
	this->device = new mediasoupclient::Device();
	this->device->Load(data, &this->option);

	CreateSendTransport();

	CreateRecvTransport();

	Join();

	AfterJoin();
}

bool MediasoupStream::close(bool wait)
{
	std::string strIds;
	this->mapProducers.clear();
	ConsumerNodeList::iterator iter = this->mapConsumers.begin();
	while (iter != this->mapConsumers.end()) {
		if (iter->second->incomming_source) {
			RealtimeIncomingVideoSource *source =
				dynamic_cast<RealtimeIncomingVideoSource*>(iter->second->incomming_source);
			if (source != nullptr) {
				std::string name=source->GetSourceName();
				if (!name.empty()) {
					if (strIds.empty())
						strIds=name;
					else
						strIds+=","+name;
				}
			}
			delete iter->second->incomming_source;
			iter->second->incomming_source=nullptr;
		}
		iter = this->mapConsumers.erase(iter);
	}
	
	this->mapConsumers.clear();
	if (!strIds.empty()) {
		strIds +='\0';
		DeleteSources(strIds);
	}
	if (this->recvTransport) {
		this->recvTransport->Close();
		delete this->recvTransport;
		this->recvTransport = nullptr;
	}

	if (this->sendTransport) {
		this->sendTransport->Close();
		delete this->sendTransport;
		this->sendTransport = nullptr;
	}

	if (this->dataProducer) {
		this->dataProducer->Close();
		delete this->dataProducer;
		this->dataProducer = nullptr;
	}

	if (this->dataConsumer) {
		this->dataConsumer->Close();
		delete this->dataConsumer;
		this->dataConsumer = nullptr;
	}

	if (this->device) {
		delete this->device;
		this->device = nullptr;
	}

	audio_track.release();
	video_track.release();
	audio_source.release();

	if (thread_closeAsync.joinable())
		thread_closeAsync.detach();

	// Shutdown websocket connection
	if (client) {
		client->Disconnect(wait);
		delete (client);
		client = nullptr;
		return false;
	} else {
		return false;
	}
}

void MediasoupStream::DeleteSources(const std::string& sourceList) {
	int len = sourceList.size();
	int bufferlen = sizeof(int) + len*sizeof(char);
	char *buffer = new char[bufferlen];
	memcpy(buffer,&len,sizeof(len));
	memcpy(buffer+sizeof(int),sourceList.c_str(),sizeof(char)*len);
	HWND hId = (HWND)obs_frontend_get_main_window_handle();
#if 0
	::PostMessageA(hId,MEDIASOUP_SOURCE_DELETE,NULL,(LPARAM)buffer);
#endif
}

void MediasoupStream::DeletePeer(const std::string& sourceList) {
	int len = sourceList.size();
	int bufferlen = sizeof(int) + len*sizeof(char);
	char *buffer = new char[bufferlen];
	memcpy(buffer,&len,sizeof(len));
	memcpy(buffer+sizeof(int),sourceList.c_str(),sizeof(char)*len);
	HWND hId = (HWND)obs_frontend_get_main_window_handle();
#if 0
	::PostMessageA(hId,MEDIASOUP_DELETE_PEER,NULL,(LPARAM)buffer);
#endif
}

bool MediasoupStream::stop()
{
	blog(LOG_INFO,"%s","MediasoupStream::stop");
	//this->timerKiller.Kill();
	// Shutdown websocket connection and close Peer Connection
	close(true);
	// Disconnect, this will call stop on main thread
	obs_output_end_data_capture(output);

	return true;
}

void MediasoupStream::onDisconnected()
{
	info("MediasoupStream::onDisconnected");
	// are we done retrying?
	if (thread_closeAsync.joinable())
		thread_closeAsync.join();

	// Shutdown websocket connection and close Peer Connection asynchronously
	thread_closeAsync = std::thread([&]() {
		close(false);
		// Disconnect, this will call stop on main thread
		obs_output_signal_stop(output, OBS_OUTPUT_DISCONNECTED);
		/*HWND hId = (HWND)obs_frontend_get_main_window_handle();
		int code = OBS_OUTPUT_DISCONNECTED;
		char *buffer = new char[sizeof(int)];
		memcpy(buffer,&code,sizeof(int));
		::PostMessageA(hId,MEDIASOUP_OUTPUT_DISCONNECT,NULL,(LPARAM)buffer);*/
	});
}

void MediasoupStream::onConnectedFail(int code)
{
	info("MediasoupStream::onConnectedFail");
	// are we done retrying?
	if (thread_closeAsync.joinable())
		thread_closeAsync.join();

	// Shutdown websocket connection and close Peer Connection asynchronously
	thread_closeAsync = std::thread([&]() {
		close(false);
		// Disconnect, this will call stop on main thread
		obs_output_signal_stop(output, OBS_OUTPUT_DISCONNECTED);
		/*HWND hId = (HWND)obs_frontend_get_main_window_handle();
		int code = OBS_OUTPUT_DISCONNECTED;
		char *buffer = new char[sizeof(int)];
		memcpy(buffer,&code,sizeof(int));
		::PostMessageA(hId,MEDIASOUP_OUTPUT_DISCONNECT,NULL,(LPARAM)buffer);*/
	});
}

void MediasoupStream::onRequest(int id, const std::string &method,
				const nlohmann::json &body)
{
	std::string errMsg;
	nlohmann::json data;
	if (method.compare("newConsumer") == 0) {
		errMsg = onNewConsumer(body);
	} else if (method.compare("newDataConsumer") == 0) {
		errMsg = onNewDataConsumer(body);
	}
	else {
		errMsg = "UnAnalysis method: " + method;
		info(errMsg.c_str());
	}

	if (errMsg.empty())
		Accept(id, data);
	else
		Reject(id, -1, errMsg);
}

void MediasoupStream::onNotification(const std::string &method,
				     const nlohmann::json &body)
{
	std::string errMsg;
	if (method.compare("peerClosed") == 0) {
		nlohmann::json stats;
		errMsg = OnPeerClosed(body);
	} else if (method.compare("newPeer") == 0) {
		OnNewPeer(body);
	}
}

void MediasoupStream::Pause(const char* kind)
{
	if (kind && *kind) {
		TDMapProducer::iterator iter = mapProducers.begin();
		while (iter!=mapProducers.end()) {
			if (iter->second != nullptr) {
				if (stricmp("both", kind) == 0) {
					json body = {{"producerId",
						      iter->second->GetId()}};
					auto data =this->client->Request("pauseProducer",body).get();
				} else if (stricmp(kind, iter->second->GetKind()
								 .c_str()) ==
					   0) {
					json body = {{"producerId", iter->second->GetId()}};
					auto data = this->client->Request("pauseProducer", body).get();
				}
			}
			iter++;
		}
	}
}

void MediasoupStream::Resume(const char* kind)
{
	if (kind && *kind) {
		TDMapProducer::iterator iter = mapProducers.begin();
		while (iter!=mapProducers.end()) {
			if (iter->second != nullptr) {
				if (stricmp("both", kind) == 0) {
					json body = {{"producerId",
						      iter->second->GetId()}};
					auto data =this->client->Request("resumeProducer",body).get();
				}
				else if (stricmp(kind, iter->second->GetKind().c_str()) == 0) {
					json body = {{"producerId", iter->second->GetId()}};
					auto data = this->client->Request("resumeProducer", body).get();
				}
			}
			iter++;
		}
	}
}

void MediasoupStream::SignalProtocol(const char* method,const char *key,const char *value) {
	if (method && *method && key && *key && value && *value) {
		if (this->client != nullptr) {
			json body = {{key, value}};
			this->client->SendMsgNoResponse(method,body);
		}
	}
}

std::string MediasoupStream::onNewConsumer(const nlohmann::json &body)
{
	std::string errorMsg = "";
	if (this->recvTransport == nullptr) {

		errorMsg = "[ERROR] 'recvTransport' is null.";

		error(errorMsg.c_str());

		return errorMsg;
	}
	if (body.find("id") == body.end()) {

		errorMsg = "[ERROR] 'id' missing in response.";

		error(errorMsg.c_str());

		return errorMsg;
	} else if (body.find("producerId") == body.end()) {

		errorMsg = "[ERROR] 'producerId' missing in response.";

		error(errorMsg.c_str());

		return errorMsg;
	} else if (body.find("kind") == body.end()) {

		errorMsg = "[ERROR] 'kind' missing in response.";

		error(errorMsg.c_str());

		return errorMsg;
	} else if (body.find("rtpParameters") == body.end()) {

		errorMsg = "[ERROR] 'rtpParameters' missing in response.";

		error(errorMsg.c_str());

		return errorMsg;
	} else if (body.find("appData") == body.end()) {

		errorMsg = "[ERROR] 'appData' missing in response.";

		error(errorMsg.c_str());

		return errorMsg;
	}

	std::cout << "[INFO] recvTransport create comsume..." << std::endl;
	std::string text = body.dump();
	auto id = body["id"].get<std::string>();
	auto producerId = body["producerId"].get<std::string>();
	auto kind = body["kind"].get<std::string>();
	auto peerId = body["peerId"].get<std::string>();
	std::string source_name=kind+peerId;
	auto rtpParameters = body["rtpParameters"];
	ConsumerNodeList::iterator iter = this->mapConsumers.find(source_name);
	if (iter != this->mapConsumers.end()) {
		if (kind == "video") {
			RealtimeIncomingVideoSource* vs = dynamic_cast<RealtimeIncomingVideoSource*>(iter->second->incomming_source);
			if (vs != nullptr) {
				delete vs;
				vs = nullptr;
			}
		}
		iter = this->mapConsumers.erase(iter);
	}
	ConsumerNode *consumers = new ConsumerNode();
	consumers->consumer = this->recvTransport->Consume(
		this, id, producerId, kind, &rtpParameters, body["appData"]);
	if (kind == "audio") {
		auto *audio_track = static_cast<webrtc::AudioTrackInterface *>(consumers->consumer->GetTrack());
		consumers->incomming_source = new RealtimeIncomingAudioSource(audio_track);
	}
	if (kind == "video") {
		int phone = 0;
		if (m_mapPeerId2Phone.find(peerId) != m_mapPeerId2Phone.end()) {
			phone = m_mapPeerId2Phone[peerId];
		}
		auto *video_track = static_cast<webrtc::VideoTrackInterface *>(consumers->consumer->GetTrack());
		consumers->incomming_source = new RealtimeIncomingVideoSource(video_track,source_name,peerId,phone);
		//NewComsumerSource(source_name+",",phone);
		m_mapPeerId2SourceName[peerId]=source_name;
	}
	this->mapConsumers[source_name] = consumers;
	return errorMsg;
}

std::string MediasoupStream::onNewDataConsumer(const nlohmann::json &body)
{
	std::string errorMsg = "";
	return errorMsg;
}

std::string MediasoupStream::OnPeerClosed(const nlohmann::json& body) {
	std::string errorMsg = "";
	if (body.find("peerId")==body.end()) {
		errorMsg = "peerClosed peerId not found!";
		return errorMsg;
	}
	auto peerId = body["peerId"];
	if (!peerId.empty()) {
		std::map<std::string,std::string>::iterator iter=
			m_mapPeerId2SourceName.find(peerId);
		if (iter != m_mapPeerId2SourceName.end()) {
			std::string name = iter->second;
			DeletePeer(name+",");
		}
	}
	return errorMsg;
}

std::string MediasoupStream::OnNewPeer(const nlohmann::json& body) {
	std::string errorMsg = "";
	if (body.find("id")==body.end()) {
		errorMsg = "newPeer id not found!";
		return errorMsg;
	}
	if (body.find("device")==body.end()) {
		errorMsg = "newPeer device not found!";
		return errorMsg;
	}
	std::string peerId = body["id"].get<std::string>();
	auto device = body["device"];
	if (device.find("flag")==device.end()){
		errorMsg = "device  flag not found!";
		return errorMsg;
	}
	auto flag = device["flag"];
	std::string strFlag = flag.get<std::string>();
	std::transform(strFlag.begin(), strFlag.end(), strFlag.begin(), ::tolower);
	int phone = 0;
	if (strFlag.find("ios") != std::string::npos) {
		phone = 1;
	} else if (strFlag.find("android") != std::string::npos) {
		phone = 2;
	}
	m_mapPeerId2Phone[peerId]=phone;
	return std::string("");
}



void MediasoupStream::onAudioFrame(audio_data *frame)
{
	if (!frame || !audio_source)
		return;

	if (!adm)
		return;

	adm->OnPCMData((unsigned char *)frame->data[0], frame->frames);
}

void MediasoupStream::onVideoFrame(video_data *frame)
{
	if (!frame)
		return;
	if (!videoCapturer)
		return;

	if (std::chrono::system_clock::time_point(
		    std::chrono::duration<int>(0)) == previous_time)
		// First frame sent: Initialize previous_time
		previous_time = std::chrono::system_clock::now();

	// Calculate size
	int outputWidth = obs_output_get_width(output);
	int outputHeight = obs_output_get_height(output);
	auto videoType = webrtc::VideoType::kNV12;
	uint32_t size = outputWidth * outputHeight * 3 / 2;

	int stride_y = outputWidth;
	int stride_uv = (outputWidth + 1) / 2;
	int target_width = abs(outputWidth);
	int target_height = abs(outputHeight);

	// Convert frame
	rtc::scoped_refptr<webrtc::I420Buffer> buffer =
		webrtc::I420Buffer::Create(target_width, target_height,
					   stride_y, stride_uv, stride_uv);

	libyuv::RotationMode rotation_mode = libyuv::kRotate0;

	const int conversionResult = libyuv::ConvertToI420(
		frame->data[0], size, buffer.get()->MutableDataY(),
		buffer.get()->StrideY(), buffer.get()->MutableDataU(),
		buffer.get()->StrideU(), buffer.get()->MutableDataV(),
		buffer.get()->StrideV(), 0, 0, outputWidth, outputHeight,
		target_width, target_height, rotation_mode,
		ConvertVideoType(videoType));
	static DWORD tickCount = 0;
	if (tickCount == 0) {
		tickCount = ::GetTickCount64();
	}
	DWORD tmpTick = ::GetTickCount64();
	int tickInterval = tmpTick - tickCount;
#if 0
	if (tickInterval>10000) {
		tickCount = tmpTick;
		uint8_t *p = (uint8_t*)malloc(outputWidth * outputHeight * 3);

		if (p != nullptr) {
			libyuv::I420ToRGB24(
				buffer.get()->MutableDataY(), buffer.get()->StrideY(),
				buffer.get()->MutableDataU(), buffer.get()->StrideU(),
				buffer.get()->MutableDataV(), buffer.get()->StrideV(),
				p, target_width * 3, target_width, target_height);
			
			std::thread upload_bitmap(DumpCover
				,p,outputWidth * outputHeight * 3,
				outputWidth,outputHeight);
			upload_bitmap.detach();
		}

	}
#endif
	// not using the result yet, silence compiler
	(void)conversionResult;

	const int64_t obs_timestamp_us =
		(int64_t)frame->timestamp / rtc::kNumNanosecsPerMicrosec;

	// Align timestamps from OBS capturer with rtc::TimeMicros timebase
	const int64_t aligned_timestamp_us =
		timestamp_aligner_.TranslateTimestamp(obs_timestamp_us,
						      rtc::TimeMicros());

	// Create a webrtc::VideoFrame to pass to the capturer
	webrtc::VideoFrame video_frame =
		webrtc::VideoFrame::Builder()
			.set_video_frame_buffer(buffer)
			.set_rotation(webrtc::kVideoRotation_0)
			.set_timestamp_us(aligned_timestamp_us)
			.set_id(++frame_id)
			.build();

	// Send frame to video capturer
	videoCapturer->OnFrameCaptured(video_frame);
}

// NOTE LUDO: #80 add getStats
void MediasoupStream::getStats()
{
	/*rtc::scoped_refptr<const webrtc::RTCStatsReport> report = NewGetStats();
	if (nullptr == report) {
		return;
	}
	stats_list = "";

	std::vector<const webrtc::RTCOutboundRTPStreamStats *> send_stream_stats =
		report->GetStatsOfType<webrtc::RTCOutboundRTPStreamStats>();
	for (const auto &stat : send_stream_stats) {
		if (stat->kind.ValueToString() == "audio") {
			audio_bytes_sent =
				std::stoll(stat->bytes_sent.ValueToJson());
		}
		if (stat->kind.ValueToString() == "video") {
			video_bytes_sent =
				std::stoll(stat->bytes_sent.ValueToJson());
			pli_received = std::stoi(stat->pli_count.ValueToJson());
		}
	}
	total_bytes_sent = audio_bytes_sent + video_bytes_sent;

	// RTCDataChannelStats
	std::vector<const webrtc::RTCDataChannelStats *> data_channel_stats =
		report->GetStatsOfType<webrtc::RTCDataChannelStats>();
	for (const auto &stat : data_channel_stats) {
		stats_list += "data_messages_sent:" +
			      stat->messages_sent.ValueToJson() + "\n";
		stats_list += "data_bytes_sent:" +
			      stat->bytes_sent.ValueToJson() + "\n";
		stats_list += "data_messages_received:" +
			      stat->messages_received.ValueToJson() + "\n";
		stats_list += "data_bytes_received:" +
			      stat->bytes_received.ValueToJson() + "\n";
	}

	// RTCMediaStreamTrackStats
	// double audio_track_jitter_buffer_delay = 0.0;
	// double video_track_jitter_buffer_delay = 0.0;
	// uint64_t audio_track_jitter_buffer_emitted_count = 0;
	// uint64_t video_track_jitter_buffer_emitted_count = 0;
	std::vector<const webrtc::RTCMediaStreamTrackStats *>
		media_stream_track_stats = report->GetStatsOfType<
			webrtc::RTCMediaStreamTrackStats>();
	for (const auto &stat : media_stream_track_stats) {
		if (stat->kind.ValueToString() == "audio") {
			if (stat->audio_level.is_defined())
				stats_list += "track_audio_level:" +
					      stat->audio_level.ValueToJson() +
					      "\n";
			if (stat->total_audio_energy.is_defined())
				stats_list +=
					"track_total_audio_energy:" +
					stat->total_audio_energy.ValueToJson() +
					"\n";
			// stats_list += "track_echo_return_loss:"   + stat->echo_return_loss.ValueToJson() + "\n";
			// stats_list += "track_echo_return_loss_enhancement:" + stat->echo_return_loss_enhancement.ValueToJson() + "\n";
			// stats_list += "track_total_samples_received:" + stat->total_samples_received.ValueToJson() + "\n";
			if (stat->total_samples_duration.is_defined())
				stats_list += "track_total_samples_duration:" +
					      stat->total_samples_duration
						      .ValueToJson() +
					      "\n";
			// stats_list += "track_concealed_samples:" + stat->concealed_samples.ValueToJson() + "\n";
			// stats_list += "track_concealment_events:" + stat->concealment_events.ValueToJson() + "\n";
		}
		if (stat->kind.ValueToString() == "video") {
			// stats_list += "track_jitter_buffer_delay:"    + stat->jitter_buffer_delay.ValueToJson() + "\n";
			// stats_list += "track_jitter_buffer_emitted_count:" + stat->jitter_buffer_emitted_count.ValueToJson() + "\n";
			stats_list += "track_frame_width:" +
				      stat->frame_width.ValueToJson() + "\n";
			stats_list += "track_frame_height:" +
				      stat->frame_height.ValueToJson() + "\n";
			stats_list += "track_frames_sent:" +
				      stat->frames_sent.ValueToJson() + "\n";
			stats_list += "track_huge_frames_sent:" +
				      stat->huge_frames_sent.ValueToJson() +
				      "\n";

			// FPS = frames_sent / (time_now - start_time)
			std::chrono::system_clock::time_point time_now =
				std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds =
				time_now - previous_time;
			previous_time = time_now;
			uint32_t frames_sent =
				std::stol(stat->frames_sent.ValueToJson());
			stats_list += "track_fps:" +
				      std::to_string((frames_sent -
						      previous_frames_sent) /
						     elapsed_seconds.count()) +
				      "\n";
			previous_frames_sent = frames_sent;
		}
	}

	// RTCOutboundRTPStreamStats
	std::vector<const webrtc::RTCOutboundRTPStreamStats *>
		outbound_stream_stats = report->GetStatsOfType<
			webrtc::RTCOutboundRTPStreamStats>();
	for (const auto &stat : outbound_stream_stats) {
		if (stat->kind.ValueToString() == "audio") {
			stats_list += "outbound_audio_packets_sent:" +
				      stat->packets_sent.ValueToJson() + "\n";
			stats_list += "outbound_audio_bytes_sent:" +
				      stat->bytes_sent.ValueToJson() + "\n";
			// stats_list += "outbound_audio_target_bitrate:" + stat->target_bitrate.ValueToJson() + "\n";
			// stats_list += "outbound_audio_frames_encoded:" + stat->frames_encoded.ValueToJson() + "\n";
		}
		if (stat->kind.ValueToString() == "video") {
			stats_list += "outbound_video_packets_sent:" +
				      stat->packets_sent.ValueToJson() + "\n";
			stats_list += "outbound_video_bytes_sent:" +
				      stat->bytes_sent.ValueToJson() + "\n";
			// stats_list += "outbound_video_target_bitrate:" + stat->target_bitrate.ValueToJson() + "\n";
			// stats_list += "outbound_video_frames_encoded:" + stat->frames_encoded.ValueToJson() + "\n";
			stats_list += "outbound_video_fir_count:" +
				      stat->fir_count.ValueToJson() + "\n";
			stats_list += "outbound_video_pli_count:" +
				      stat->pli_count.ValueToJson() + "\n";
			stats_list += "outbound_video_nack_count:" +
				      stat->nack_count.ValueToJson() + "\n";
			// stats_list += "outbound_video_sli_count:"      + stat->sli_count.ValueToJson() + "\n";
			if (stat->qp_sum.is_defined())
				stats_list += "outbound_video_qp_sum:" +
					      stat->qp_sum.ValueToJson() + "\n";
		}
	}

	// RTCTransportStats
	std::vector<const webrtc::RTCTransportStats *> transport_stats =
		report->GetStatsOfType<webrtc::RTCTransportStats>();
	for (const auto &stat : transport_stats) {
		stats_list += "transport_bytes_sent:" +
			      stat->bytes_sent.ValueToJson() + "\n";
		stats_list += "transport_bytes_received:" +
			      stat->bytes_received.ValueToJson() + "\n";
	}
	*/
}
/*
rtc::scoped_refptr<const webrtc::RTCStatsReport> MediasoupStream::NewGetStats()
{
	rtc::CritScope lock(&crit_);

	if (nullptr == pc) {
		return nullptr;
	}

	rtc::scoped_refptr<StatsCallback> stats_callback =
		new rtc::RefCountedObject<StatsCallback>();

	pc->GetStats(stats_callback);

	while (!stats_callback->called())
		std::this_thread::sleep_for(std::chrono::microseconds(1));

	return stats_callback->report();
}
*/
// ============================= mediasoup client ===========================
void MediasoupStream::OnTransportClose(mediasoupclient::Producer *producer)
{
	std::cout << "[INFO] MediasoupOutputClient::OnTransportClose()"
		  << std::endl;
	if (producer == nullptr)
		return;

	std::string producerId = producer->GetId();
	auto producerIt = this->mapProducers.find(producerId);
	if (producerIt == this->mapProducers.end())
		return;

	if (producerIt->second != nullptr) {
		delete producerIt->second;
		producerIt->second = nullptr;
	}
	this->mapProducers.erase(producerId);
}

void MediasoupStream::OnTransportClose(
	mediasoupclient::DataProducer * /*dataProducer*/)
{
	std::cout << "[INFO] MediasoupOutputClient::OnTransportClose()"
		  << std::endl;
}

void MediasoupStream::OnTransportClose(mediasoupclient::Consumer *consumer)
{
	if (consumer == nullptr)
		return;

	std::string consumerId = consumer->GetId();
	auto consumerNodeIt = this->mapConsumers.find(consumerId);
	if (consumerNodeIt == this->mapConsumers.end())
		return;

	if (consumerNodeIt->second != nullptr) {

		if (consumerNodeIt->second->incomming_source != nullptr) {
			consumerNodeIt->second->incomming_source->OnClose();
			delete consumerNodeIt->second->incomming_source;
			consumerNodeIt->second->incomming_source = nullptr;
		}

		if (consumerNodeIt->second->consumer != nullptr) {
			delete consumerNodeIt->second->consumer;
			consumerNodeIt->second->consumer = nullptr;
		}

		delete consumerNodeIt->second;
		consumerNodeIt->second = nullptr;
	}

	this->mapConsumers.erase(consumerId);
}


void MediasoupStream::OnTransportClose(
	mediasoupclient::DataConsumer *dataConsumer)
{
	if (dataConsumer == nullptr)
		return;

	std::string consumerId = dataConsumer->GetId();
	auto consumerNodeIt =
		this->mapDataConsumers.find(consumerId);
	if (consumerNodeIt == this->mapDataConsumers.end())
		return;

	if (consumerNodeIt->second != nullptr) {

		if (consumerNodeIt->second->incomming_source != nullptr) {
			consumerNodeIt->second->incomming_source->OnClose();
			delete consumerNodeIt->second->incomming_source;
			consumerNodeIt->second->incomming_source = nullptr;
		}

		if (consumerNodeIt->second->consumer != nullptr) {
			delete consumerNodeIt->second->consumer;
			consumerNodeIt->second->consumer = nullptr;
		}

		delete consumerNodeIt->second;
		consumerNodeIt->second = nullptr;
	}

	this->mapDataConsumers.erase(consumerId);
}

/* Transport::Listener::OnConnect
 *
 * Fired for the first Transport::Consume() or Transport::Produce().
 * Update the already created remote transport with the local DTLS parameters.
 */
std::future<void>
MediasoupStream::OnConnect(mediasoupclient::Transport *transport,
			   const json &dtlsParameters)
{
	std::cout << "[INFO] MediasoupOutputClient::OnConnect()" << std::endl;
	// std::cout << "[INFO] dtlsParameters: " << dtlsParameters.dump(4) << std::endl;

	if (transport->GetId() == this->sendTransport->GetId()) {
		return this->OnConnectSendTransport(dtlsParameters);
	} else if (transport->GetId() == this->recvTransport->GetId()) {
		return this->OnConnectRecvTransport(dtlsParameters);
	} else {
		std::promise<void> promise;

		promise.set_exception(std::make_exception_ptr(
			"Unknown transport requested to connect"));

		return promise.get_future();
	}
}

std::future<void>
MediasoupStream::OnConnectSendTransport(const json &dtlsParameters)
{
	std::promise<void> promise;

	debug("OnConnectSendTransport transportId:%s",
	      this->sendTransport->GetId().c_str());

	json body = {{"transportId", this->sendTransport->GetId()},
		     {"dtlsParameters", dtlsParameters}};

	auto data = this->client->Request("connectWebRtcTransport", body).get();

	debug("OnConnectSendTransport data:%s", data.dump().c_str());

	promise.set_value();
	return promise.get_future();
}

std::future<void>
MediasoupStream::OnConnectRecvTransport(const json &dtlsParameters)
{
	std::promise<void> promise;

	debug("OnConnectRecvTransport transportId:%s",
	      this->recvTransport->GetId().c_str());

	json body = {{"transportId", this->recvTransport->GetId()},
		     {"dtlsParameters", dtlsParameters}};

	auto data = this->client->Request("connectWebRtcTransport", body).get();

	debug("OnConnectRecvTransport data:%s", data.dump().c_str());

	promise.set_value();
	return promise.get_future();
}

/*
 * Transport::Listener::OnConnectionStateChange.
 */
void MediasoupStream::OnConnectionStateChange(
	mediasoupclient::Transport * transport,
	const std::string &connectionState)
{
	std::cout
		<< "[INFO] MediasoupOutputClient::OnConnectionStateChange() [connectionState:"
		<< connectionState << "]" << std::endl;

	if (connectionState == "failed") {
		// Close must be carried out on a separate thread in order to avoid deadlock
		auto thread = std::thread([=]() {
			obs_output_set_last_error(output,
						  "Connection failure\n\n");
			// Disconnect, this will call stop on main thread
			blog(LOG_INFO,"OnConnectionStateChange fail");
			obs_output_signal_stop(output, OBS_OUTPUT_MEDIASOUP_CONNECTSTATE_FAIL);
		});
		//Detach
		thread.detach();
	}
	if (connectionState == "connected") {
		if (this->sendTransport != nullptr &&
		    transport->GetId() == this->sendTransport->GetId()) {
			//this->CreateDataProducer();
		}
	}
}

/* Producer::Listener::OnProduce
 *
 * Fired when a producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
/*
void MediasoupStream::OnProduce(
	mediasoupclient::SendTransport *transport,
	mediasoupclient::Producer::Listener *producerListener,
	const mediasoupclient::SendHandler::SendResult &sendResult,
	webrtc::MediaStreamTrackInterface *track, const nlohmann::json &appData)
{
	json body = {{"transportId", transport->GetId()},
		     {"kind", track->kind()},
		     {"rtpParameters", sendResult.rtpParameters},
		     {"appData", appData}};

	this->client->request(
		"produce", body,
		[transport, producerListener, sendResult, track,
		 appData, this](const nlohmann::json &data) {
			auto it = data.find("id");
			if (it == data.end() || !it->is_string()) {
				std::cout << "'id' missing in response"
					  << std::endl;
				return;
			}

			std::string producerId = it->get<std::string>();
			mediasoupclient::Producer *p =
				transport->CreateProducer(producerListener,
							  producerId,
						  sendResult, track, appData);
			if (p) {
				this->mapProducers[p->GetId()] = p;
			}
		});
}
*/

std::future<std::string> MediasoupStream::OnProduce(
	mediasoupclient::SendTransport *transport, const std::string &kind,
	nlohmann::json rtpParameters, const nlohmann::json &appData)
{

	std::promise<std::string> promise;

	json body = {{"transportId", transport->GetId()},
		     {"kind", kind},
		     {"rtpParameters", rtpParameters},
		     {"appData", appData}};

	auto data = this->client->Request("produce", body).get();

	auto it = data.find("id");
	if (it == data.end() || !it->is_string()) {
		std::cout << "'id' missing in response" << std::endl;
		promise.set_exception(
			std::make_exception_ptr("'id' missing in response"));
	} else {
		promise.set_value(it->get<std::string>());
	}
	return promise.get_future();
}

/* Producer::Listener::OnProduceData
 *
 * Fired when a data producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
std::future<std::string> MediasoupStream::OnProduceData(
	mediasoupclient::SendTransport * transport,
	const json &sctpStreamParameters, const std::string &label,
	const std::string &protocol, const json & appData)
{
	debug("[INFO] MediasoupOutputClient::OnProduceData()");
	
	std::promise<std::string> promise;

	/* clang-format off */
	json body =
	{
		{"transportId", transport->GetId()},
		{ "label"                , label },
		{ "protocol"             , protocol },
		{ "sctpStreamParameters" , sctpStreamParameters },
		{ "appData"		 , appData }
	};

	auto data = this->client->Request("produceData", body).get();

	auto it = data.find("id");
	if (it == data.end() || !it->is_string()) {
		promise.set_exception(std::make_exception_ptr(
			"'id' missing in response"));
	} else {
		auto dataProducerId = (*it).get<std::string>();
		promise.set_value(dataProducerId);
	}
	
	return promise.get_future();
}

/** Virtual methods inherited from DataConsumer::Listener */
void MediasoupStream::OnMessage(mediasoupclient::DataConsumer *dataConsumer,
				const webrtc::DataBuffer &buffer)
{
	std::cout << "[INFO] MediasoupOutputClient::OnMessage()" << std::endl;
	if (dataConsumer->GetLabel() == "chat") {
		std::string s = std::string(buffer.data.data<char>(),
					    buffer.data.size());
		std::cout << "[INFO] received chat data: " + s << std::endl;
	}
}

/** Virtual methods inherited from DataProducer::Listener */
void MediasoupStream::OnOpen(mediasoupclient::DataProducer * /*dataProducer*/)
{
	std::cout << "[INFO] MediasoupOutputClient::OnOpen()" << std::endl;
}
void MediasoupStream::OnClose(mediasoupclient::DataProducer * /*dataProducer*/)
{
	std::cout << "[INFO] MediasoupOutputClient::OnClose()" << std::endl;
}
void MediasoupStream::OnBufferedAmountChange(
	mediasoupclient::DataProducer * /*dataProducer*/, uint64_t /*size*/)
{
	std::cout << "[INFO] MediasoupOutputClient::OnBufferedAmountChange()"
		  << std::endl;
}

/*
*  Local obs mediasoup client logic
*/
void MediasoupStream::CreateSendTransport()
{
	std::cout << "[INFO] creating mediasoup send WebRtcTransport..."
		  << std::endl;

	json sctpCapabilities = this->device->GetSctpCapabilities();
	/*
	* {
						forceTcp         : this._forceTcp,
						producing        : true,
						consuming        : false,
						sctpCapabilities : this._useDataChannel
							? this._mediasoupDevice.sctpCapabilities
							: undefined
					}
	*/
	json body = {{"forceTcp", false},
		     {"producing", true},
		     {"consuming", false},
		     {"sctpCapabilities", sctpCapabilities}};

	auto data = this->client->Request("createWebRtcTransport", body).get();

	if (data.find("id") == data.end()) {
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;

		return;
	} else if (data.find("iceParameters") == data.end()) {
		std::cerr << "[ERROR] 'iceParametersd' missing in response"
			  << std::endl;

		return;
	} else if (data.find("iceCandidates") == data.end()) {
		std::cerr << "[ERROR] 'iceCandidates' missing in response"
			  << std::endl;

		return;
	} else if (data.find("dtlsParameters") == data.end()) {
		std::cerr << "[ERROR] 'dtlsParameters' missing in response"
			  << std::endl;

		return;
	} else if (data.find("sctpParameters") == data.end()) {
		std::cerr << "[ERROR] 'sctpParameters' missing in response"
			  << std::endl;

		return;
	}

	std::cout << "[INFO] creating SendTransport..." << std::endl;

	auto sendTransportId = data["id"].get<std::string>();

	this->sendTransport = this->device->CreateSendTransport(
		this, sendTransportId, data["iceParameters"],
		data["iceCandidates"], data["dtlsParameters"],
		data["sctpParameters"], &this->option);
}

void MediasoupStream::CreateRecvTransport()
{
	std::cout << "[INFO] creating mediasoup recv WebRtcTransport..."
		  << std::endl;

	json sctpCapabilities = this->device->GetSctpCapabilities();

	json body = {{"forceTcp", false},
		     {"producing", false},
		     {"consuming", true},
		     {"sctpCapabilities", sctpCapabilities}};
	auto data = this->client->Request("createWebRtcTransport", body).get();

	if (data.find("id") == data.end()) {
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;

		return;
	} else if (data.find("iceParameters") == data.end()) {
		std::cerr << "[ERROR] 'iceParameters' missing in response"
			  << std::endl;

		return;
	} else if (data.find("iceCandidates") == data.end()) {
		std::cerr << "[ERROR] 'iceCandidates' missing in response"
			  << std::endl;

		return;
	} else if (data.find("dtlsParameters") == data.end()) {
		std::cerr << "[ERROR] 'dtlsParameters' missing in response"
			  << std::endl;

		return;
	} else if (data.find("sctpParameters") == data.end()) {
		std::cerr << "[ERROR] 'sctpParameters' missing in response"
			  << std::endl;

		return;
	}

	auto recvTransportId = data["id"].get<std::string>();

	std::cout << "[INFO] creating RecvTransport..." << std::endl;

	auto sctpParameters = data["sctpParameters"];

	this->recvTransport = this->device->CreateRecvTransport(
		this, recvTransportId, data["iceParameters"],
		data["iceCandidates"], data["dtlsParameters"], sctpParameters,
		&this->option);

	this->CreateDataConsumer();
}

void MediasoupStream::CreateDataProducer() {
	if (this->dataProducer != nullptr) {
		error("dataProducer[%s] already created.",
		      this->dataProducer->GetId().c_str());
		return;
	}

	/*json appData = {
		{"info", "my-chat-DataProducer"}
	};
	this->dataProducer = this->sendTransport->ProduceData(this);*/
}

void MediasoupStream::CreateDataConsumer()
{
	/*const std::string &dataProducerId = this->dataProducer->GetId();

	json body = {{"dataProducerId", dataProducerId}};

	
	// create server data consumer
	auto r =
		cpr::PostAsync(
			cpr::Url{this->baseUrl + "/MediasoupOutputClients/" + this->id +
				 "/transports/" + this->recvTransport->GetId() +
				 "/consume/data"},
			cpr::Body{body.dump()},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::VerifySsl{verifySsl})
			.get();
	if (r.status_code != 200) {
		std::cerr
			<< "[ERROR] server unable to consume mediasoup recv WebRtcTransport"
			<< " [status code:" << r.status_code << ", body:\""
			<< r.text << "\"]" << std::endl;
		return;
	}
	
	auto response = json::parse(r.text);
	if (response.find("id") == response.end()) {
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;
		return;
	}
	std::string dataConsumerId = response["id"].get<std::string>();
	// Create client consumer.
	this->dataConsumer = this->recvTransport->ConsumeData(
		this, dataConsumerId, dataProducerId, "chat", "",
		nlohmann::json());	*/
}

void MediasoupStream::Join()
{
	json body = {{"displayName", "test"},
		     {"device", deviceInfo.toJson()},
		     {"rtpCapabilities", this->device->GetRtpCapabilities()},
		     {"sctpCapabilities", this->device->GetSctpCapabilities()}};

	auto data = this->client->Request("join", body).get();
}

void MediasoupStream::AfterJoin()
{
	///////////////////////// Create Audio Producer //////////////////////////

	if (this->device->CanProduce("audio")) {
		json codecOptions = {{"opusStereo", true}, {"opusDtx", true}};

		auto producer = this->sendTransport->Produce(this, this->audio_track, nullptr,
					     &codecOptions);
		this->mapProducers[producer->GetId()] = producer;
	} else {
		std::cerr << "[WARN] cannot produce audio" << std::endl;
	}

	///////////////////////// Create Video Producer //////////////////////////

	if (this->device->CanProduce("video")) {
		std::vector<webrtc::RtpEncodingParameters> encodings;
		encodings.emplace_back(webrtc::RtpEncodingParameters());
		encodings.emplace_back(webrtc::RtpEncodingParameters());
		encodings.emplace_back(webrtc::RtpEncodingParameters());

		auto producer = this->sendTransport->Produce(this, this->video_track,
					     &encodings, nullptr);
		this->mapProducers[producer->GetId()] = producer;

	} else {
		std::cerr << "[WARN] cannot produce video" << std::endl;
	}

	///////////////////////// Create Data Producer //////////////////////////

	/*this->dataProducer = sendTransport->ProduceData(this);

	uint32_t intervalSeconds = 10;
	std::thread([this, intervalSeconds]() {
		bool run = true;
		while (run) {
			std::chrono::system_clock::time_point p =
				std::chrono::system_clock::now();
			std::time_t t = std::chrono::
				system_clock::to_time_t(p);
			std::string s = std::ctime(&t);
			auto dataBuffer = webrtc::DataBuffer(s);
			std::cout
				<< "[INFO] sending chat data: "
				<< s << std::endl;
			this->dataProducer->Send(dataBuffer);
			run = timerKiller.WaitFor(
				std::chrono::seconds(
					intervalSeconds));
		}
	}).detach();*/

	// Set audio conversion info
	audio_convert_info conversion;
	conversion.format = AUDIO_FORMAT_16BIT;
	conversion.samples_per_sec = 48000;
	conversion.speakers = (speaker_layout)channel_count;
	obs_output_set_audio_conversion(output, &conversion);

	info("Begin data capture...");
	obs_output_begin_data_capture(output, 0);
}

void MediasoupStream::Accept(int id, const nlohmann::json &data)
{
	json body = {
		{"response", true}, {"id", id}, {"ok", true}, {"data", data}};
	client->Response(body);
}

void MediasoupStream::Reject(int id, int errorCode,
			     const std::string &errorReson)
{
	json body = {{"response", true},
		     {"id", id},
		     {"ok", false},
		     {"errorCode", errorCode},
		     {"errorReson", errorReson}};
	client->Response(body);
}
