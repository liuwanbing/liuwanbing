#include "StdAfx.h"
#include "TCPServer.h"

#include <iostream>
#include <fstream>
using namespace std;

//宏定义
#define ID_SOCKET_WND			10								//SOCKET 窗口 ID
#define WM_SOCKET_MESSAGE		WM_USER+12						//SOCKET 消息

/**************************************************************************************************************/

BEGIN_MESSAGE_MAP(CTCPServerSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_MESSAGE,OnSocketNotifyMesage)
END_MESSAGE_MAP()




CTCPServerSocket::CTCPServerSocket(IServerSocketService* pSocket)
{
	m_iReadBufLen=0;
	m_pIService = pSocket;
	if (AfxGetMainWnd()!=NULL) InitSocketWnd();
	m_hSocket = INVALID_SOCKET;
	m_hClientSocket = INVALID_SOCKET;
	m_hSocket = ::WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	ifstream inf("port.txt");
	int port1, port2;

	inf>>port1>>port2;
	inf.close();


	port1 = 13000;
	port2 = 23000;
	int i = port1;
	for (; i < port2; i++)
	{
		if (Bind(i)) break;
	}
	m_iPort = i;
	if (i == port2)
	{
		AfxMessageBox("CTCPServerSocket:Bind失败");
		//#ifdef 
		//MessageBox(NULL, L"CTCPServerSocket:Bind失败", L"Error", MB_OK);
		//#endif
	}
	else
	{
		Listen();
	}
}

CTCPServerSocket::~CTCPServerSocket(void)
{
	CloseSocket();
	CloseSocketWnd();
}


//关闭 SOCKET
void CTCPServerSocket::CloseSocket()
{
	::closesocket(m_hSocket);
	::closesocket(m_hClientSocket);
	m_hSocket = INVALID_SOCKET;
	m_hClientSocket = INVALID_SOCKET;
}

//监听网络
bool CTCPServerSocket::Listen()
{
	::WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_MESSAGE, FD_ACCEPT|FD_CLOSE);
	return (::listen(m_hSocket, 5) != SOCKET_ERROR);
}
//绑定地址
bool CTCPServerSocket::Bind(UINT uPort)
{
	struct sockaddr_in SocketAddr;
	::memset(&SocketAddr,0,sizeof(SocketAddr));
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_addr.s_addr=INADDR_ANY;
	SocketAddr.sin_port=htons(uPort);
	return (::bind(m_hSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr)) != SOCKET_ERROR);
}

//发送函数
int CTCPServerSocket::SendData(int nSocketID, void * pData, int uSize, int bMainID, int bAssistantID, int bHandleCode)
{
	if ((m_hSocket!=INVALID_SOCKET)&&(uSize<=MAX_SEND_SIZE))
	{
		//定义数据
		int iErrorCode=0;
		char bSendBuffer[MAX_SEND_SIZE+NET_HEAD_SIZE];
		int uSendSize=uSize+sizeof(NetMessageHead),uSended=0;

		//打包数据
		NetMessageHead * pNetHead=(NetMessageHead *)bSendBuffer;
		pNetHead->uMessageSize=uSendSize;
		pNetHead->bMainID=bMainID;
		pNetHead->bAssistantID=bAssistantID;
		pNetHead->bHandleCode=bHandleCode;
		pNetHead->bReserve = 0;
		if (uSize>0) ::CopyMemory(bSendBuffer+sizeof(NetMessageHead),pData,uSize);

		//发送数据
		int iSendCount=0;	//发送次数计数器
		do
		{
			iErrorCode=::send(nSocketID,bSendBuffer+uSended,uSendSize-uSended,0);
			if (iErrorCode==SOCKET_ERROR) 
			{
				if (::WSAGetLastError()==WSAEWOULDBLOCK) //网络有阻塞，原来紧接着的代码是：return uSize
				{
					if(iSendCount++>100)//判断重发次数是否超过100次
						return SOCKET_ERROR;//太多次的重发了，直接返回 错误
					else
					{
						Sleep(10);	//等待 10 ms，和上面的重发100次，等于有1秒钟的时间来重发
						continue;   //重发数据
					}
				}
				else
					return SOCKET_ERROR;
			}
			uSended+=iErrorCode;
			iSendCount=0;//重要计数器置0，发下一条数据,Fred Huang 2008-05-16
		} while (uSended<uSendSize);

		return uSize;
	}
	return SOCKET_ERROR;
}

//简单命令发送函数
int CTCPServerSocket::SendData(int nSocketID, int bMainID, int bAssistantID, int bHandleCode)
{
	return SendData(nSocketID, NULL, 0, bMainID, bAssistantID, bHandleCode);
}
///发送函数
int CTCPServerSocket::SendData(int nSocketID, UINT8* pBuffer, int nLen)
{
	if ((nSocketID!=INVALID_SOCKET)&&(nLen<=MAX_SEND_SIZE))
	{

		char _d[512] = {0};
		sprintf(_d, "长度%d", nLen);
		OutputDebugStringA(_d);
		/// 因为发送过程常常处于闲置状态，所以只采用同步发送方式，而不采用异步发送机制
		//发送数据
		INT32 iSendCount=0;	///< 网络可能会有阻塞
		INT32 iErrorCode=0;
		INT32 nSendSize=nLen;
		INT32 nSended=0;
		NetMessageHead *pHead = (NetMessageHead *)pBuffer;

		do
		{
			iErrorCode=::send(nSocketID, (char *)pBuffer + nSended, nSendSize - nSended, 0);
			if (iErrorCode==SOCKET_ERROR) 
			{
				if (::WSAGetLastError()==WSAEWOULDBLOCK) //网络有阻塞，原来紧接着的代码是：return uSize,Fred Huang 2008-05-16
				{
					if(iSendCount++>100)//判断重发次数是否超过100次,Fred Huang 2008-05-16
					{
						return SOCKET_ERROR;//太多次的重发了，直接返回 错误,Fred Huang 2008-05-16
					}
					else
					{
						Sleep(10);	//等待 10 ms，和上面的重发100次，等于有1秒钟的时间来重发,Fred Huang 2008-05-16
						continue;   //重发数据,Fred Huang 2008-05-16
					}
				}
				else
					return SOCKET_ERROR;
			}
			nSended+=iErrorCode;
			iSendCount=0;//重复计数器置0，发下一条数据
		} while (nSended<nSendSize);

		return nSended;
	}
	return SOCKET_ERROR;
}

int CTCPServerSocket::SendData(UINT8 * pBuffer, int nLen)
{
	if (m_hClientSocket != INVALID_SOCKET)
		return SendData(m_hClientSocket, pBuffer, nLen);
	return 0;
}
//解释错误
string CTCPServerSocket::TranslateError(UINT uErrorCode)
{
	string strMessage;
	switch (uErrorCode)
	{
	case WSANOTINITIALISED:
		{
			strMessage="A successful WSAStartup call must occur before using this function.";
			break;
		}
	case WSAENETDOWN:
		{
			strMessage="网络系统初始化失败！";
			break;
		}
	case WSAEINTR:
		{
			strMessage="The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
			break;
		}
	case WSAEADDRINUSE:
		{
			strMessage="The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.";
			break;
		}
	case WSAEINPROGRESS:
		{
			strMessage="A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
			break;
		}
	case WSAEALREADY:
		{
			strMessage="A nonblocking connect call is in progress on the specified socket. ";
			break;
		}
	case WSAEADDRNOTAVAIL:
		{
			strMessage="The remote address is not a valid address.";
			break;
		}
	case WSAEAFNOSUPPORT:
		{
			strMessage="Addresses in the specified family cannot be used with this socket.";
			break;
		}
	case WSAECONNREFUSED:
		{
			strMessage="服务器没有启动";
			break;
		}
	case WSAEFAULT:
		{
			strMessage="The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.";
			break;
		}
	case WSAEINVAL:
		{
			strMessage="The parameter s is a listening socket.";
			break;
		}
	case WSAEISCONN:
		{
			strMessage="The socket is already connected.";
			break;
		}
	case WSAENETUNREACH:
		{
			strMessage="The network cannot be reached from this host at this time.";
			break;
		}
	case WSAENOBUFS:
		{
			strMessage="No buffer space is available. The socket cannot be connected.";
			break;
		}
	case WSAENOTSOCK:
		{
			strMessage="The descriptor is not a socket.";
			break;
		}
	case WSAETIMEDOUT:
		{
			strMessage="Attempt to connect timed out without establishing a connection.";
			break;
		}
	case WSAEWOULDBLOCK:
		{
			strMessage="The socket is marked as nonblocking and the connection cannot be completed immediately.";
			break;
		}
	case WSAEACCES:
		{
			strMessage="Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.";
			break;
		}
		//default:strMessage.Format("Unknow Socket Error: %d"),uErrorCode);
	}
	return strMessage;
}


//SOCKET 消息处理程序
LRESULT	CTCPServerSocket::OnSocketNotifyMesage(WPARAM wParam, LPARAM lParam)
{
	SOCKET _Socket = wParam;
	//if (m_hClientSocket != INVALID_SOCKET && m_hClientSocket != _Socket) return 0;
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:     // 监听中的套接字检测到有连接进入  
		{  
			SOCKET _client = ::accept(_Socket, NULL, NULL);
			if (m_hClientSocket == INVALID_SOCKET)
			{
				m_hClientSocket = _client;
				::WSAAsyncSelect(_client, m_hWnd, WM_SOCKET_MESSAGE, FD_READ|FD_WRITE|FD_CLOSE);
				if (m_pIService) m_pIService->OnSocketAccept(m_hClientSocket);
			}
			else
			{
				CloseSocket(_client);
			}
			return 0;
		}  
	case FD_WRITE:
		{
			return 0;
		}
	case FD_READ:	//读取数据
		{

			if (_Socket != m_hClientSocket)
			{
				CloseSocket(_Socket);
				return 0;
			}
			//读取网络数据
			int iLen=::recv(_Socket, (char *)(m_szBuffer+m_iReadBufLen),sizeof(m_szBuffer)-m_iReadBufLen,0);
			if (iLen==SOCKET_ERROR)
			{
				if (m_pIService) 
					m_pIService->OnSocketCloseEvent(m_hClientSocket);
				CloseClientSocket();
				return 0; 
			}

			//处理数据
			try
			{
				m_iReadBufLen+=iLen;
				BYTE szNetBuffer[MAX_SEND_SIZE+NET_HEAD_SIZE];
				do
				{
					//效验数据
					NetMessageHead * pNetHead=(NetMessageHead *)m_szBuffer;
					if ((m_iReadBufLen<sizeof(NetMessageHead))||(m_iReadBufLen<(int)pNetHead->uMessageSize))
					{
						TCHAR sz[100];
						wsprintf(sz,TEXT("%d,%d,%d"), m_iReadBufLen,sizeof(NetMessageHead),(int)pNetHead->uMessageSize);
						//AfxMessageBox(sz);
						return 0;
					}

					//拷贝数据
					UINT uMessageSize=pNetHead->uMessageSize;
					if (uMessageSize>sizeof(szNetBuffer)) throw TEXT("数据包过大");
					::CopyMemory(szNetBuffer,m_szBuffer,uMessageSize);

					//删除数据
					::MoveMemory(m_szBuffer,m_szBuffer+uMessageSize,(m_iReadBufLen-uMessageSize)*sizeof(BYTE));
					m_iReadBufLen-=uMessageSize;

					//处理数据
					//uMessageSize-=sizeof(NetMessageHead);
					//NetMessageHead * pHandleNetData=(NetMessageHead *)szNetBuffer;

					//AfxMessageBox("333");

					if ((!m_pIService) || m_pIService->OnSocketReadEvent(m_hClientSocket, szNetBuffer,uMessageSize) == false)
					{
#ifdef SUPER_VERSION
						TCHAR sz[100];
						wsprintf(sz, TEXT("网络消息处理失败, 连接即将关闭,MainID:%d bHandleCode:%d bAssistantID:%d MsgSize:%d"), pHandleNetData->bMainID, pHandleNetData->bHandleCode,
							pHandleNetData->bAssistantID, uMessageSize);
						//AFCMessageBox(sz);
#endif
						//CloseSocket(true);
						m_pIService->OnSocketCloseEvent(m_hClientSocket);
						CloseClientSocket();
						return 0;
					}
				} 
				while ((m_iReadBufLen>0) && (m_hSocket!=INVALID_SOCKET));
			}
			catch (...)	{
				if (m_pIService) m_pIService->OnSocketCloseEvent(m_hClientSocket); 
				CloseClientSocket();
			}
			return 0;
		}
	case FD_CLOSE:	//关闭消息
		{
			if (_Socket != m_hClientSocket) 
			{
				CloseSocket(_Socket);
				return 0;
			};

			if (m_pIService) 
				m_pIService->OnSocketCloseEvent(m_hClientSocket);
			CloseClientSocket();
			return 0;
		}
	}
	return 0;
}

//初始化 SOCKET 窗口
bool CTCPServerSocket::InitSocketWnd()
{
	try
	{
		if (GetSafeHwnd()==NULL)
		{	
			if (!Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),AfxGetMainWnd(),ID_SOCKET_WND,NULL))	return false;
		}
	}
	catch (...) {}
	return true;;
}
//关闭 SOCKET 窗口
bool CTCPServerSocket:: CloseSocketWnd()
{
	if (GetSafeHwnd()!=NULL) DestroyWindow();
	return true;
}

void CTCPServerSocket::CloseSocket(INT32 nSocketID)
{
	::closesocket(nSocketID);
}

void CTCPServerSocket::CloseClientSocket()
{
	::closesocket(m_hClientSocket);
	m_hClientSocket = INVALID_SOCKET;
}