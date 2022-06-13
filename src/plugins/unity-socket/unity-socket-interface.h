#ifndef UNITY_SOCKET_INTERFACE_H
#define UNITY_SOCKET_INTERFACE_H

#ifdef UnitySocketExport
#define USExport __declspec(dllexport)
#else
#define USExport	
#endif
#include <functional>
using SocketCallBack = std::function<void(char*,int length)>;
#ifdef __cplusplus
extern "C"{
#endif
bool USExport InitUnitySocket(SocketCallBack cb, const char *ip, int port);
void USExport SendFaceVec(const char *data, int length);
void USExport UnInit();
#ifdef __cplusplus
}
#endif
#endif
