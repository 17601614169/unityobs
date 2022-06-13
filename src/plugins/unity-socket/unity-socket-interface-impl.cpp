#include "unity-socket-interface.h"
#include "socket_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
bool InitUnitySocket(SocketCallBack cb, const char *ip, int port)
{
	bool ret = false;
	CTcpSocketThread::GetInstance()->initSocketThread(cb);
	return ret;
}

void UnInit()
{

}

void SendFaceVec(const char *data,int length)
{
	CTcpSocketThread::GetInstance()->InsertData(data,length);
}
#ifdef __cplusplus
}
#endif
