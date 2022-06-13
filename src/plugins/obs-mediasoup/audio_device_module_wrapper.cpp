#include "audio_device_module_wrapper.h"
#include "api/task_queue/default_task_queue_factory.h"
//#include "service/bee_entrance.h"

bool AudioDeviceModuleWrapper::input_param_set_ = false;
int32_t AudioDeviceModuleWrapper::channels_ = 0;
int32_t AudioDeviceModuleWrapper::sample_rate_ = 0;
int32_t AudioDeviceModuleWrapper::sample_size_ = 0;
bool AudioDeviceModuleWrapper::enabled_ = false;

AudioDeviceModuleWrapper::AudioDeviceModuleWrapper(
	const AudioLayer audioLayer,
	webrtc::TaskQueueFactory *task_queue_factory)
	: rtc::RefCountedObject<webrtc::AudioDeviceModuleImpl>(
		  audioLayer, task_queue_factory)
{
	enabled_ = true;
}

AudioDeviceModuleWrapper::~AudioDeviceModuleWrapper() {}

rtc::scoped_refptr<AudioDeviceModuleWrapper>
AudioDeviceModuleWrapper::Create(const AudioLayer audio_layer)
{
	auto factory = webrtc::CreateDefaultTaskQueueFactory();
	rtc::scoped_refptr<AudioDeviceModuleWrapper> audio_device(
		new rtc::RefCountedObject<AudioDeviceModuleWrapper>(
			audio_layer, factory.get()));

	// Ensure that the current platform is supported.
	if (audio_device->CheckPlatform() == -1) {
		return nullptr;
	}

	// Create the platform-dependent implementation.
	if (audio_device->CreatePlatformSpecificObjects() == -1) {
		return nullptr;
	}

	// Ensure that the generic audio buffer can communicate with the platform
	// specific parts.
	if (audio_device->AttachAudioBuffer() == -1) {
		return nullptr;
	}

	// audio_device->Init();
	return audio_device;
}

int32_t
AudioDeviceModuleWrapper::RegisterAudioCallback(AudioTransport *audioCallback)
{
	audio_transport_ = audioCallback; //Custom record transport.
	return AudioDeviceModuleImpl::RegisterAudioCallback(audioCallback);
}

int32_t AudioDeviceModuleWrapper::InitRecording()
{
	rec_is_initialized_ = true;
	return 0;
}

bool AudioDeviceModuleWrapper::RecordingIsInitialized() const
{
	return rec_is_initialized_;
}

int32_t AudioDeviceModuleWrapper::StartRecording()
{
	if (!rec_is_initialized_) {
		return -1;
	}
	recording_ = true;
	return 0;
}

int32_t AudioDeviceModuleWrapper::StopRecording()
{
	recording_ = false;
	return 0;
}

bool AudioDeviceModuleWrapper::Recording() const
{
	return recording_;
}

int32_t
AudioDeviceModuleWrapper::StereoRecordingIsAvailable(bool *available) const
{
	*available = false;
	return 0;
}

int32_t AudioDeviceModuleWrapper::SetStereoRecording(bool enable)
{
	if (!enable) {
		return 0;
	}
	return -1;
}

int AudioDeviceModuleWrapper::SetInputParam(int32_t channels,
					    int32_t sample_rate,
					    int32_t sample_size)
{
	int ret = 0;
	do {
		if (!enabled_) {
			ret = -1;
			break;
		}

		if (!input_param_set_) {
			channels_ = channels;
			sample_rate_ = sample_rate;
			sample_size_ = sample_size;
			input_param_set_ = true;
		} else if (channels != channels_ ||
			   sample_rate != sample_rate_ ||
			   sample_size != sample_size_) {
			ret = -2;
		}
	} while (0);
	return ret;
}

void AudioDeviceModuleWrapper::OnPCMData(uint8_t *data,
					 size_t samples_per_channel)
{
	crit_sect_.Enter();
	if (!audio_transport_) {
		return;
	}
	crit_sect_.Leave();

	//This info is set on the stream before starting capture
	size_t channels = channels_;
	size_t sample_rate = sample_rate_;
	size_t sample_size = sample_size_;
	//Get chunk for 10ms
	size_t chunk = (sample_rate / 100);

	size_t i = 0;
	uint32_t level;

	//Check if we had pending
	if (pending_length_) {
		//Copy the missing ones
		i = chunk - pending_length_;
		//Copy
		memcpy(pending_ + pending_length_ * sample_size * channels,
		       data, i * sample_size * channels);

		//Add sent
		audio_transport_->RecordedDataIsAvailable(
			pending_, chunk, sample_size * channels, channels,
			sample_rate, 0, 0, 0, 0, level);

		//No pending
		pending_length_ = 0;
	}

	//Send all full chunks possible
	while (i + chunk < samples_per_channel) {
		//Send them
		audio_transport_->RecordedDataIsAvailable(
			data + i * sample_size * channels, chunk,
			sample_size * channels, channels, sample_rate, 0, 0, 0,
			0, level);
		//Inc sent
		i += chunk;
	}

	//If there are missing ones
	if (i != samples_per_channel) {
		//Calculate pending
		pending_length_ = samples_per_channel - i;
		//Copy to pending buffer
		memcpy(pending_, data + i * sample_size * channels,
		       pending_length_ * sample_size * channels);
	}
}
