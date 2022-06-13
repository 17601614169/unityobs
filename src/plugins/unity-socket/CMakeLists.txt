project(unity-socket)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(UnityHeaders 
	unity-socket-interface.h
	ring_buffer.h
	socket_thread.h)
	
set(UnitySources 
	unity-socket-interface-impl.cpp
	ring_buffer.cpp
	socket_thread.cpp)
add_definitions(-DUnitySocketExport)
source_group("Header Files" FILES ${UnityHeaders})
source_group("Source Files" FILES ${UnitySources})

add_library(unity-socket SHARED ${UnityHeaders} ${UnitySources})

target_link_libraries(unity-socket Ws2_32.lib Netapi32.lib ${CMAKE_SOURCE_DIR}/lib/$<CONFIG>/tcpsocket.lib)
set_target_properties(unity-socket PROPERTIES FOLDER "plugins")
install_obs_plugin(unity-socket)
install_obs_core(unity-socket EXPORT unity-socket)




