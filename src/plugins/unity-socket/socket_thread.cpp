#include "socket_thread.h"
//#pragma comment(lib, "Netapi32.lib")

#define FAILED_SEND_PACKET_TIME   3
#define ELEMENT_COUNT (1024 * 1024 * 64)
const int16_t head1 = 0x55;
const int16_t head2 = 0xAA;
const int16_t head3 = 0x5A;
const INT64 head_packet_min_size = 14;
const INT64 SOCKET_BUFFER_LEN = 1024 * 1024 * 5;
const INT64 PIXEL_SIZE_MAX = 1920 * 1080 * 4;

CTcpSocketThread *CTcpSocketThread::INSTANCE_ = new CTcpSocketThread;
CTcpSocketThread::CTcpSocketThread() {
	m_bIsSuccConnect=false;
	m_bIsThreadStop = false;
	socket_buffer = new char[SOCKET_BUFFER_LEN];
	memset(socket_buffer, 0, SOCKET_BUFFER_LEN);
	ring_buffer = Utility_CreateBuffer(64 * 1024 * 1024, 1);
	recv_packet[0].Init(PIXEL_SIZE_MAX);
	recv_packet[1].Init(PIXEL_SIZE_MAX);
	head_packet_invalid= false;
	packet_length = 0;
}

CTcpSocketThread::~CTcpSocketThread()
{
	clearPacket();
	closeSocket();
	clearRecvBuffer();
	stopThread(true);
	closeIMHandle();
}

void CTcpSocketThread::ResetSocketBuffer() {
	if (socket_buffer!=nullptr) {
		memset(socket_buffer, 0, SOCKET_BUFFER_LEN);
	}
}

void CTcpSocketThread::closeSocket()
{
	m_tcpSocket.Close();
	m_tcpSocket.cleanSock();
}

void CTcpSocketThread::clearRecvBuffer()
{
	
}

void CTcpSocketThread::closeIMHandle()
{
	

}

CTcpSocketThread* CTcpSocketThread::GetInstance() {
	return INSTANCE_;
}
void CTcpSocketThread::DeleteInstance()
{
	if ( INSTANCE_ != NULL )
	{
		delete INSTANCE_ ;
		INSTANCE_ = NULL ;
	}
}

//初始化线程
bool CTcpSocketThread::initSocketThread(SocketCallBack cb)
{
	do 
	{
		recv_callback = cb;
		m_hSThread = CreateThread(NULL,0,CTcpSocketThread::ThreadSend,this,0,&m_dwSThreadID);
		if ( m_hSThread == NULL )
		{
			return false;
		}
		m_hRThread = CreateThread(NULL,0,CTcpSocketThread::ThreadRecv,this,0,&m_dwRThreadID);
		if ( m_hRThread == NULL )
		{
			return false;
		}
		recv_thread = std::thread(&CTcpSocketThread::ProcessRecvPacket,this);
		recv_thread.detach();
	} while (false);
	return true;
}

//线程回调函数
DWORD WINAPI CTcpSocketThread::ThreadSend( void* pContext )
{
	CTcpSocketThread *pThis = static_cast<CTcpSocketThread*>(pContext);
	pThis->OnThreadSend();
	return 0;
}

DWORD WINAPI CTcpSocketThread::ThreadRecv( void* pContext )
{
	CTcpSocketThread *pThis = static_cast<CTcpSocketThread*>(pContext);
	pThis->OnThreadRecv();
	return 0;
}
//线程收发循环
void CTcpSocketThread::OnThreadSend()
{
	while(!GetThreadState())
	{
		SendMsg();//client发送队列数据发给server
	}
}

void CTcpSocketThread::OnThreadRecv()
{
	while(!GetThreadState())
	{
		RecvMsg();//server接收数据保存至client
		DispatchMsg();//派发接收队列的数据
	}
}

void CTcpSocketThread::OnThreadProc()
{
	while(!GetThreadState())
	{
		Sleep(1000);
	}
}

//停止线程
void CTcpSocketThread::stopThread(bool isThreadStop)
{
	SetThreadState(isThreadStop);
	m_bIsThreadStop = isThreadStop;
	if ( isThreadStop == true )
	{
		DWORD dwWaitResult;
		if ( m_hSThread != 0 )
		{
			dwWaitResult = WaitForSingleObject(m_hSThread,2000);//dwWaitResult 0 退出正常
			if ( dwWaitResult != WAIT_OBJECT_0 )
			{
				TerminateThread(m_hSThread,-1);
			}
			m_hSThread = 0;
		}
		if ( m_hRThread != 0 )
		{
			dwWaitResult = WaitForSingleObject(m_hRThread,2000);//dwWaitResult 0 退出正常
			if ( dwWaitResult != WAIT_OBJECT_0 )
			{
				TerminateThread(m_hRThread,-1);
			}
			m_hRThread = 0;
		}
	}
}

void CTcpSocketThread::InsertData(const char *data, int length)
{
	if (data == nullptr || length == 0)
		return;
	std::lock_guard<std::mutex> lock(mutex_send_queue);
	//if(send_queue.size()>0){
	//	STSendPacket stSendPacket = send_queue.front();
	//	send_queue.pop();
	//	stSendPacket.ReleaseData();
	//}
	STSendPacket stPacket;
	stPacket.CopyData(data,length);
	send_queue.emplace(stPacket);
}

STSendPacket CTcpSocketThread::SendPacketDequeue()
{
	std::lock_guard<std::mutex> lock(mutex_send_queue);
	//QMutexLocker lock(&mutex);
	STSendPacket stSendPacket = send_queue.front();
	send_queue.pop();
	return stSendPacket;
}

//发送队列数据
void CTcpSocketThread::SendMsg()
{
	if (!send_queue.empty())
	{
		//Lock lock(m_gCriticalSection);
		STSendPacket stSendPacket = SendPacketDequeue();
		if (stSendPacket.data!=nullptr)
		{
			bool bSendResult = false;
			int nSendPacketTime = 0;
			/*******发送成功则删除，失败则继续发送,最多只发送三次********/
			do 
			{
				bSendResult = SendData(&stSendPacket);
				if ( !bSendResult && nSendPacketTime < FAILED_SEND_PACKET_TIME )
				{
					Sleep(1);
				}
				stSendPacket.ReleaseData();
			} while (!bSendResult);

			if ( bSendResult )
			{
				
			} 		
		}
	}
}

//TCP 发送数据
bool CTcpSocketThread::SendData(STSendPacket* STSendPacket)
{
	int       nResult = 0;
	int       nWaitTime = 0;
	unsigned long     dwSocketError = 0;
	if(!CheckISSuccConnect()){
		return false;
	}
	
	int nIndex = 0 ;
	int nSendDataLen = 0 ; 
	while( nIndex < STSendPacket->length ){
		nResult = m_tcpSocket.Wait(nWaitTime,CAN_WRITE) ; 
		if ( nResult > 0 ){
			if((nResult & CAN_WRITE) == CAN_WRITE){
				nSendDataLen = m_tcpSocket.Send(
					STSendPacket->data + nIndex,
					STSendPacket->length - nIndex, 0); 
				if ( nSendDataLen >= 0 ){
					nIndex += nSendDataLen ; //更新索引值
				}
				else{
					dwSocketError = WSAGetLastError() ;
					break;
				}
			}			
		}
		else if ( nResult < 0 ){
			dwSocketError = WSAGetLastError() ;
			break;
		}
	}	
	if ( dwSocketError != 0 ){
		DisConnectSrv() ;
		return false ;
	}
	else{
		return true;
	}
	return true;
}


void CTcpSocketThread::DispatchMsg()
{
	PacketHeadValidation();
	PacketContentRead();
}

void CTcpSocketThread::RecvMsg()
{
	if (!CheckISSuccConnect()) {
		return;
	}
	int nResult = 0;
	int nWaitTime = 1;
	int recv_len = 0;
	unsigned long dwErrorNum = 0;
	if (!CheckISSuccConnect()) {
		return;
	}
	if ((nResult = m_tcpSocket.Wait(nWaitTime, CAN_READ)) >= 0) {
		if ((nResult & CAN_READ) == CAN_READ) {
			ResetSocketBuffer();
			while ((recv_len = m_tcpSocket.Receive(
					socket_buffer, SOCKET_BUFFER_LEN, 0)) >
			       0) {
				Utility_WriteBuffer(ring_buffer, socket_buffer,
						    recv_len);
				ResetSocketBuffer();
				recv_len = 0;
			}

			if (recv_len == SOCKET_ERROR || recv_len == 0) {
				dwErrorNum = WSAGetLastError();
				if (dwErrorNum != WSAEWOULDBLOCK) {
					DisConnectSrv();
				}
			}
		}
	}
}

bool CTcpSocketThread::PacketHeadValidation()
{
	if (head_packet_invalid)
		return head_packet_invalid;
	int64_t min_packet = Utility_available_read(ring_buffer);
	if (min_packet >= head_packet_min_size) {
		size_t s = sizeof(packet_head1);
		static_assert(sizeof(packet_head1) == sizeof(packet_head2),
			      "size error");
		static_assert(sizeof(packet_head2) == sizeof(packet_head3),
			      "size error");
		size_t head1_size = Utility_ReadBuffer(ring_buffer, nullptr,
						       &packet_head1,
						       sizeof(packet_head1));
		size_t head2_size = Utility_ReadBuffer(ring_buffer, nullptr,
						       &packet_head2,
						       sizeof(packet_head2));
		size_t head3_size = Utility_ReadBuffer(ring_buffer, nullptr,
						       &packet_head3,
						       sizeof(packet_head3));
		
		size_t msg_length_size =
			Utility_ReadBuffer(ring_buffer, nullptr, &packet_length,
					   sizeof(packet_length));
		head_packet_invalid = true;
	} else {
		head_packet_invalid = false;
	}
	return head_packet_invalid;
}

bool CTcpSocketThread::PacketContentRead()
{
	bool ret = false;
	if (PacketHeadValidation()) {
		if (head1 == packet_head1 && head2 == packet_head2 &&
		    head3 == packet_head3) {
			std::lock_guard<std::mutex> lock(mutex_recv_packet);
			int packet_index = 0/*current_recv == 0 ? 1 : 0*/;

			if (packet_length > PIXEL_SIZE_MAX) {
				Utility_InitBuffer(ring_buffer);
			} else {
				recv_packet[packet_index].Reset();
				size_t len = Utility_ReadBuffer(
					ring_buffer, nullptr,
					recv_packet[packet_index].ringbuffer->data,
					packet_length);
				recv_packet[packet_index].ringbuffer->write_pos = packet_length;
				current_recv = packet_index;
			}
		} else {
			Utility_InitBuffer(ring_buffer);
		}
		ret = true;
		head_packet_invalid = false;
	}
	return ret;
}

void CTcpSocketThread::ProcessRecvPacket() {
	while (!GetThreadState()) {
		std::lock_guard<std::mutex> lock(mutex_recv_packet);
		int process_packet_index = current_recv;
		INT64 can_read = Utility_available_read(
			recv_packet[process_packet_index].ringbuffer);
		if (can_read > 0 && can_read <= PIXEL_SIZE_MAX) {
			recv_callback(recv_packet[process_packet_index].ringbuffer->data,
				      recv_packet[process_packet_index].ringbuffer->write_pos);
			recv_packet[process_packet_index].Reset();
		}
	}
}

bool CTcpSocketThread::CheckISSuccConnect()
{
	if(!m_bIsSuccConnect){
		if(!ConnectSrv()){
			return false;
		}else{
			if (ring_buffer != nullptr) {
				Utility_InitBuffer(ring_buffer);
			}
		}
	}
	return true;

}

bool CTcpSocketThread::ConnectSrv()
{
    bool          bFlag = false;
    int           nReValue = 0;
    unsigned long         dwStart = 0;
    int           nRetryNum = 0;
    if(m_bIsSuccConnect)
        return true;
    m_tcpSocket.Close();
    if ( !CTcpSocket::initSock() ){
	DWORD dwStatus = WSAGetLastError();
	return false;
    }
    if ( !m_tcpSocket.Create(AF_INET,SOCK_STREAM,IPPROTO_TCP) ){
	return false;
    }
    nReValue = m_tcpSocket.Connect("127.0.0.1", 1755, false);
    DWORD e = WSAGetLastError();
    dwStart = GetTickCount();
    while(1){
	nReValue = m_tcpSocket.Wait(100, CAN_CONNECT);
	if (nReValue == -1){
	    if((dwStart + 30000) < GetTickCount()) {
		nReValue = 0;
		break;
	    }
	    if(nRetryNum < 3){
		m_tcpSocket.Close();
		Sleep(150);
		m_tcpSocket.Create(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		nReValue = m_tcpSocket.Connect("127.0.0.1",1755, false);
		++nRetryNum;
	    }else{
		    break;
	    }
	}else
	{
	    if ((nReValue & CAN_CONNECT) == CAN_CONNECT){
		m_tcpSocket.SetSockBuff();
		m_bIsSuccConnect = true;
		return true;
	    }else
	   {
		if((dwStart + 30000) < GetTickCount()){
	            nReValue = 0;
		    break;
		}
	    }
	}
    }
    return bFlag;
}

//TCP断开连接
void CTcpSocketThread::DisConnectSrv()
{
	clearPacket();
	m_bIsSuccConnect = false;
	m_tcpSocket.Close();
	Sleep(10000);
}

void CTcpSocketThread::clearPacket()
{
	
}

void CTcpSocketThread::SetThreadState(bool state)
{
	m_bIsThreadStop = state;
}

bool CTcpSocketThread::GetThreadState()
{
	return m_bIsThreadStop;
}
