#ifndef __AUDIO_DEVICE_MODULE_WRAPPER_H__
#define __AUDIO_DEVICE_MODULE_WRAPPER_H__

#include <stdio.h>
//#include "bee/base/bee_define.h"
#include "modules/audio_device/audio_device_impl.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/critical_section.h"
#include "rtc_base/checks.h"

using webrtc::AudioDeviceBuffer;
using webrtc::AudioDeviceGeneric;
using webrtc::AudioDeviceModule;
using webrtc::AudioTransport;
using webrtc::kAdmMaxDeviceNameSize;
using webrtc::kAdmMaxGuidSize;
using webrtc::kAdmMaxFileNameSize;

class AudioDeviceModuleWrapper
	: public rtc::RefCountedObject<webrtc::AudioDeviceModuleImpl> {
public:
	AudioDeviceModuleWrapper(const AudioLayer audioLayer,
				 webrtc::TaskQueueFactory *task_queue_factory);
	virtual ~AudioDeviceModuleWrapper() override;
	// Creates an ADM.
	static rtc::scoped_refptr<AudioDeviceModuleWrapper>
	Create(const AudioLayer audio_layer = kPlatformDefaultAudio);

	// Full-duplex transportation of PCM audio
	virtual int32_t RegisterAudioCallback(AudioTransport *audioCallback);

	// Device enumeration
	virtual int16_t RecordingDevices() { return 0; }
	virtual int32_t RecordingDeviceName(uint16_t index,
					    char name[kAdmMaxDeviceNameSize],
					    char guid[kAdmMaxGuidSize])
	{
		return 0;
	}

	// Device selection
	virtual int32_t SetRecordingDevice(uint16_t index) { return 0; }
	virtual int32_t SetRecordingDevice(WindowsDeviceType device)
	{
		return 0;
	}

	// Audio transport initialization
	virtual int32_t RecordingIsAvailable(bool *available) { return 0; }
	virtual int32_t InitRecording();
	virtual bool RecordingIsInitialized() const;

	// Audio transport control
	virtual int32_t StartRecording();
	virtual int32_t StopRecording();
	virtual bool Recording() const;

	// Audio mixer initialization
	virtual int32_t InitMicrophone() { return 0; }
	virtual bool MicrophoneIsInitialized() const { return 0; }

	// Microphone volume controls
	virtual int32_t MicrophoneVolumeIsAvailable(bool *available)
	{
		return 0;
	}
	virtual int32_t SetMicrophoneVolume(uint32_t volume) { return 0; }
	virtual int32_t MicrophoneVolume(uint32_t *volume) const { return 0; }
	virtual int32_t MaxMicrophoneVolume(uint32_t *maxVolume) const
	{
		return 0;
	}
	virtual int32_t MinMicrophoneVolume(uint32_t *minVolume) const
	{
		return 0;
	}

	// Microphone mute control
	virtual int32_t MicrophoneMuteIsAvailable(bool *available) { return 0; }
	virtual int32_t SetMicrophoneMute(bool enable) { return 0; }
	virtual int32_t MicrophoneMute(bool *enabled) const { return 0; }

	// Stereo support
	virtual int32_t StereoRecordingIsAvailable(bool *available) const;
	virtual int32_t SetStereoRecording(bool enable);
	virtual int32_t StereoRecording(bool *enabled) const { return 0; }

	// Only supported on Android.
	virtual bool BuiltInAECIsAvailable() const { return false; }
	virtual bool BuiltInAGCIsAvailable() const { return false; }
	virtual bool BuiltInNSIsAvailable() const { return false; }

	// Enables the built-in audio effects. Only supported on Android.
	virtual int32_t EnableBuiltInAEC(bool enable) { return 0; }
	virtual int32_t EnableBuiltInAGC(bool enable) { return 0; }
	virtual int32_t EnableBuiltInNS(bool enable) { return 0; }

	static bool IsEnabled() { return enabled_; }
	static int SetInputParam(int32_t channels, int32_t sample_rate,
				 int32_t sample_size);
	void OnPCMData(uint8_t *data, size_t samples_per_channel);

public:
	bool init_ = false;
	bool recording_ = false;
	bool rec_is_initialized_ = false;
	rtc::CriticalSection crit_sect_;
	AudioTransport *audio_transport_ = NULL;
	uint8_t pending_[640 * 2 * 2];
	size_t pending_length_ = 0;
	static bool input_param_set_;
	static int32_t channels_;
	static int32_t sample_rate_;
	static int32_t sample_size_;
	static bool enabled_;
};

#endif // __AUDIO_DEVICE_MODULE_WRAPPER_H__
