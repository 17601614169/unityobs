project(obs-mediasoup VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)

if(WIN32)
	set(obs-mediasoup_PLATFORM_DEPS
		ws2_32
		winmm
		Iphlpapi)

endif()


include_directories(D:/webrtc-check3/src)
include_directories(D:/webrtc-check3/src/third_party)
include_directories(D:/webrtc-check3/src/third_party/abseil-cpp)
include_directories(D:/webrtc-check3/src/third_party/libyuv/include)
include_directories(D:/webrtc-check3/src/third_party/boringssl/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libmediasoupclient/deps/libsdptransform/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libmediasoupclient/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libmediasoupclient/deps/cpr/include)
include_directories(${CMAKE_SOURCE_DIR}/plugins/websocket-client)
include_directories(${CMAKE_SOURCE_DIR}/UI)
include_directories(${CMAKE_SOURCE_DIR}/UI/obs-frontend-api)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_definitions(-DWEBRTC_WIN)
add_definitions(-DNOMINMAX)
add_definitions(-DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE)

set(obs-mediasoup_HEADERS
	mediasoup_stream.h
	mediasoup-source.h
	mediasoup-output.h
	audio_device_module_wrapper.h
	realtime_incoming_audio_source.h
	realtime_incoming_video_source.h
	realtime_outputing_audio_source.h
	realtime_outputing_video_source.h)
set(obs-mediasoup_SOURCES
	mediasoup_stream.cpp
	mediasoup-source.c
	mediasoup-output.cpp
	obs-mediasoup-main.c
	audio_device_module_wrapper.cpp
	realtime_incoming_audio_source.cpp
	realtime_incoming_video_source.cpp
	realtime_outputing_audio_source.cpp
	realtime_outputing_video_source.cpp)

add_library(obs-mediasoup MODULE
	${obs-mediasoup_HEADERS}
	${obs-mediasoup_SOURCES})

set(WEBRTC_LIBS_DEBUG ${CMAKE_SOURCE_DIR}/lib/Debug/webrtc.lib ${CMAKE_SOURCE_DIR}/lib/Debug/sdptransform.lib ${CMAKE_SOURCE_DIR}/lib/Debug/mediasoupclient.lib)
set(WEBRTC_LIBS_RELEASE ${CMAKE_SOURCE_DIR}/lib/Release/webrtc.lib ${CMAKE_SOURCE_DIR}/lib/Release/sdptransform.lib ${CMAKE_SOURCE_DIR}/lib/Release/mediasoupclient.lib)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
	
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
	
ELSE()
	message("Some other build type.")
ENDIF()
target_link_libraries(obs-mediasoup
	libobs
	obs-frontend-api
	websocketclient
	gdiplus
	"$<$<CONFIG:Debug>:${WEBRTC_LIBS_DEBUG}>"
	"$<$<CONFIG:Release>:${WEBRTC_LIBS_RELEASE}>"
	${obs-mediasoup_PLATFORM_DEPS})

set_target_properties(obs-mediasoup PROPERTIES FOLDER "plugins")
install_obs_plugin_with_data(obs-mediasoup data)
