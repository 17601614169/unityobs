#ifndef CQXIMTCPSOCKET_H
#define CQXIMTCPSOCKET_H

/*--------------------------------------
*    error code
-----------------------------------------*/
#define ERROR_SOCKET_SUCC                       0 //success
#define ERROR_SOCKET_NET                       -1 //network error
#define ERROR_SOCKET_INVALID_PARAMS            -2 //param error

/*----------------------------------
* define fd_set attributes
----------------------------------*/
#define CAN_READ         101
#define CAN_WRITE        102
#define CAN_CONNECT      103
#define CAN_ACCEPT       104

/*----------------------------------
* define winsock2
----------------------------------*/
#ifdef WIN32
#include <WinSock2.h>
#include <process.h>
//#pragma  comment(lib,"Ws2_32")
#include <windows.h>
#endif

/*----------------------------------
* define CTCPSOCKET_H
----------------------------------*/
#ifdef TCPSOCKET_EXPORT
#define TS_EXPORT __declspec(dllexport)
#else
#define TS_EXPORT
#endif
class TS_EXPORT CTcpSocket
{
public:
	CTcpSocket();
	~CTcpSocket();
	//get socket name
	void GetSockName(ULONG& ulIP,unsigned short& usPort);
	//set socket buffer
	void SetSockBuff(long lSockRecvBufferLen = 5*1024*1024,long lSockSendBufferLen = 5*1024*1024);
	//attach socket
	void Attach(SOCKET sock);
	//set block
	void SetBlock(bool bBlock);
public:
	//connect
	int Connect(char *szIP, unsigned short usHostPort, bool bBlock);
	int Connect(unsigned long addrIp, unsigned short usHostPort,
		    bool bBlock);
	//check socket is valid
	bool inInvalid();
	int Receive(void* lpBuff,int nBuffLen,int nFlags);
	//send data
	int Send(void* lpBuff,int nBuffLen,int nFlags);
	//Accept
	bool Accept(SOCKET &socket,SOCKADDR *lpSockAddr,int *lpSockAddrLen);
	//Accept
	bool Accept(CTcpSocket &socket,SOCKADDR *lpSockAddr,int *lpSockAddrLen);
	//listen
	bool Listen(int nConnectionBacklog = 5);
	//wait
	int Wait(long millis,int flag);
	//close
	void Close(int nHow = 0);
	//create socket
	bool Create(int af,int nType,int protocol);
	//bind
	bool Bind(unsigned short usPort, char *pszIP = NULL);

public:
	//init socket lib
	static bool initSock();
	//release socket lib
	static ULONG cleanSock();
	//connect using proxy .0(succ) ,!=0(error code)
	int  SetSockOpt(int  nLevel ,int nOptName,const char* optvalue,int nOptlen);
	//get last error code
	int  GetLastError();
protected:
	SOCKET m_sock;//socket
};
#endif
