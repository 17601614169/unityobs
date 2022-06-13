#pragma once
#include <stack>
#include <string>
#include "tcp_socket.h"
#include "ring_buffer.h"
#include "unity-socket-interface.h"
#include <mutex>
#include <queue>
#include <thread>
struct STSendPacket {
	char * data;
	int length;
	STSendPacket() {
		data = nullptr;
		length = 0;
	}
	void CopyData(const char *data_src, int length_src)
	{
		data = new char[length_src];
		memset(data,0,length_src);
		memcpy(data, data_src, length_src);
		length = length_src;
	}
	void ReleaseData(){
		if(data!=nullptr){
			delete [] data;
			data = nullptr;
		}
	}
};

struct STRecvPacket {
	RingBuffer *ringbuffer;
	int length = 0;
	STRecvPacket() {
		ringbuffer = nullptr;
		length = 0;
	}
	void Init(int src_len) {
		if (ringbuffer == nullptr) {
			ringbuffer = Utility_CreateBuffer(src_len, 1);
		}
	}
	void Unit() {
		if (ringbuffer != nullptr) {
			Utility_FreeBuffer(ringbuffer);
			ringbuffer = nullptr;
		}
	}
	void Reset(){
		if (ringbuffer != nullptr) {
			Utility_InitBuffer(ringbuffer);
		}
	}
};

class CTcpSocketThread 
{
public:
	CTcpSocketThread(const CTcpSocketThread &) = delete;
	CTcpSocketThread &operator=(const CTcpSocketThread &) = delete;

	static CTcpSocketThread* INSTANCE_;
	static CTcpSocketThread *GetInstance();
	static void DeleteInstance() ;
	~CTcpSocketThread();
public:
	static DWORD WINAPI ThreadSend( void* pContext );
	static DWORD WINAPI ThreadRecv( void* pContext );
	void ProcessRecvPacket();

public:
	void OnThreadSend();
	void OnThreadRecv();
	void OnThreadProc();
	bool initSocketThread(SocketCallBack cb);
	bool SendData(STSendPacket *STSendPacket);

	bool PacketHeadValidation();
	bool PacketContentRead();
	void ResetSocketBuffer();

public:
	void SendMsg();
	void RecvMsg();
	void DispatchMsg();
public:
	bool CheckISSuccConnect();
	bool ConnectSrv();
	void DisConnectSrv();
public:
	void stopThread(bool isThreadStop);
	void clearPacket();
	void closeIMHandle();
	void closeSocket();
	void clearRecvBuffer();
	void SetThreadState(bool state);
	bool GetThreadState();
public:
	STSendPacket SendPacketDequeue();

	void InsertData(const char *data, int length);

private:
	CTcpSocketThread();
	CTcpSocket m_tcpSocket;
	DWORD m_dwSThreadID;
	HANDLE m_hSThread;
	DWORD m_dwRThreadID;
	HANDLE m_hRThread;
	bool m_bIsSuccConnect;
	bool m_bIsThreadStop;
	
	std::mutex mutex_send_queue;
	std::queue<STSendPacket> send_queue;

	std::thread recv_thread;
	std::mutex mutex_recv_packet;
	STRecvPacket recv_packet[2];
	int current_recv=0;
	SocketCallBack recv_callback;

	char *socket_buffer=nullptr;
	RingBuffer *ring_buffer;
	bool head_packet_invalid;

	int16_t packet_head1 = 0x00;
	int16_t packet_head2 = 0x00;
	int16_t packet_head3 = 0x00;
	int64_t packet_length=0;
};
