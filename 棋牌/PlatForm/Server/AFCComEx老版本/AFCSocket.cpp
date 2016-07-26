#include "StdAfx.h"
#include "AFCSocket.h"
#include "AFCException.h"
#include "AFCFunction.h"

/**************************************************************************************************************/

//控制标识
#define SOCKET_SND				1								//SOCKET 发送
#define SOCKET_REV				2								//SOCKET 接收
#define SOCKET_CLO				3								//SOCKET 退出

//超时时间
#define	TIME_OUT				3000							

#define ID_SOCKET_WND			10								//SOCKET 窗口 ID
#define WM_SOCKET_MESSAGE		WM_USER+12						//SOCKET 消息

/**************************************************************************************************************/

///static int MaxConnectCount=3;

//生成空闲 SOCKET
CTCPSocket * CTCPSocketManage::TCPSocketNew()
{
	////if(m_SocketPtrArray.GetActiveCount() > MaxConnectCount)return NULL;
	if (m_bRun)
	{
		CSignedLockObject LockObject(m_SocketPtrArray.GetArrayLock(),true);
		return m_SocketPtrArray.NewItem();
	}
	return NULL;
}


//释放活动 SOCKET
bool CTCPSocketManage::TCPSocketDelete(CTCPSocket * pSocket)
{
	if (pSocket!=NULL)
	{
		CSignedLockObject LockObject(m_SocketPtrArray.GetArrayLock(),true);
		pSocket->CloseSocket();
		return m_SocketPtrArray.FreeItem(pSocket);
	}
	return false;
}



//SOCKET 连接应答线程
unsigned __stdcall CTCPSocketManage::ThreadAccept(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct	* pStartData=(ThreadStartStruct *)pThreadData;			//线程启动数据指针
	CTCPSocketManage	* pSocketManage=pStartData->pSocketManage;				//管理指针
	HANDLE				hCompletionPort=pStartData->hCompletionPort;			//完成端口

	sockaddr_in			SocketAddr;
	int					iAddrBuferLen=sizeof(SocketAddr);
	SOCKET				hSocket=INVALID_SOCKET;
	CTCPSocket			* pNewTCPSocket=NULL;
	HANDLE				hAcceptCompletePort=NULL;
	SOCKET				hListenScoket=pStartData->pSocketManage->m_ListenSocket.GetSafeSocket();

	//连接发送数据
	MSG_S_ConnectSuccess ConncetSuccess;
	ConncetSuccess.bMaxVer=pSocketManage->m_bMaxVer;
	ConncetSuccess.bLessVer=pSocketManage->m_bLessVer;

	//线程数据读取完成
	SetEvent(pStartData->hThreadEvent);

	while (1)
	{
		try
		{

			//接收连接
			hSocket=::WSAAccept(hListenScoket,(sockaddr *)&SocketAddr,&iAddrBuferLen,NULL,NULL);
			if (hSocket==INVALID_SOCKET) 
			{
				int iErrorCode=WSAGetLastError();
				_endthreadex(0);
			}

			//处理连接
			pNewTCPSocket=pSocketManage->TCPSocketNew();
			if ((pNewTCPSocket!=NULL)&&(pNewTCPSocket->Attach(hSocket,SocketAddr,GetHandleID())))
			{
				hAcceptCompletePort=::CreateIoCompletionPort((HANDLE)hSocket,hCompletionPort,(ULONG_PTR)pNewTCPSocket,0);
				if ((hAcceptCompletePort==NULL)||(!pNewTCPSocket->RecvData())) throw TEXT("Accept False");

				//发送连接成功消息

				MSG_S_ConnectSuccess Message;
				Message.bLessVer=MAKELONG(3,4);
				Message.bReserve[0]=0;
				Message.bMaxVer=MAKELONG(3,4);
				Message.bReserve[1]=0;
				//此处把hSocket进行加密，由客户端解密并在包头的bReserve字段中发过来校验。其中SECRET_KEY为密钥由客户自己管理在
				Message.i64CheckCode = (int)hSocket * 23 + pSocketManage->m_iSecretKey;
				pNewTCPSocket->SendData(&Message,sizeof(Message),MDM_CONNECT,ASS_CONNECT_SUCCESS, 0, pNewTCPSocket->GetHandleID());

			}
			else
			{
				//没有空闲的SOCKET了，判断为房间满
				NetMessageHead netHead;
				ZeroMemory(&netHead,sizeof(NetMessageHead));
				netHead.bMainID=100;//MDM_GR_LOGON
				netHead.bAssistantID=3;//ASS_GR_LOGON_ERROR
				netHead.bHandleCode=10;//ERR_GR_PEOPLE_FULL
				netHead.uMessageSize=sizeof(NetMessageHead);
				netHead.bReserve=0;
				
				sendto(hSocket,(char *)&netHead,sizeof(NetMessageHead),0,(sockaddr*)&SocketAddr,sizeof(SocketAddr));
				throw TEXT("Accept False");
			}
		}
		catch (...)
		{TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
			::closesocket(hSocket);
			if (pNewTCPSocket!=NULL) pSocketManage->TCPSocketDelete(pNewTCPSocket);
		}
	}

	return 0;
}
























//构造函数
CUDPSocket::CUDPSocket(void)
{
	m_hSocket=INVALID_SOCKET;
}

//析构函数
CUDPSocket::~CUDPSocket(void)
{
	if (m_hSocket!=INVALID_SOCKET) ::closesocket(m_hSocket);
}

//发送函数
int	CUDPSocket::SendData(void * pData, UINT uSize, TCHAR szSendIP[])
{
	return uSize;
}

//接收函数
int CUDPSocket::RecvData()
{
	return 0;
}

/**************************************************************************************************************/

//构造函数
CListenSocket::CListenSocket(void)
{
	m_hSocket=INVALID_SOCKET;
}

//析构函数
CListenSocket::~CListenSocket(void)
{
	CloseSocket();
}

//建立 SOCKET
bool CListenSocket::Create()
{
	m_hSocket=::WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	return (m_hSocket!=INVALID_SOCKET);
}

//绑定地址
bool CListenSocket::Bind(UINT uPort)
{
	struct sockaddr_in SocketAddr;
	::memset(&SocketAddr,0,sizeof(SocketAddr));
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_addr.s_addr=INADDR_ANY;
	SocketAddr.sin_port=htons(uPort);
	return (::bind(m_hSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr))!=SOCKET_ERROR);
}

//监听网络
bool CListenSocket::Listen()
{
	return (::listen(m_hSocket,200)!=SOCKET_ERROR);
}

//关闭 SOCKET
bool CListenSocket::CloseSocket()
{
	closesocket(m_hSocket);
	m_hSocket=INVALID_SOCKET;
	return true;
}

/**************************************************************************************************************/

BEGIN_MESSAGE_MAP(CTCPClientSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_MESSAGE,OnSocketNotifyMesage)
END_MESSAGE_MAP()

//构造函数
CTCPClientSocket::CTCPClientSocket(IClientSocketService * pIService)
{
	m_iReadBufLen=0;
	m_pIService=pIService;
	m_bConnectState=NO_CONNECT;
	m_hSocket=INVALID_SOCKET;
	if (AfxGetMainWnd()!=NULL) InitSocketWnd();
}

//析构函数
CTCPClientSocket::~CTCPClientSocket()
{
	CloseSocket(false);
	CloseSocketWnd();
}

//初始化 SOCKET
bool CTCPClientSocket::InitSocketWnd()
{
	try
	{
		if (GetSafeHwnd()==NULL)
		{	
			if (!Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),AfxGetMainWnd(),ID_SOCKET_WND,NULL))	return false;
		}
	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return true;
}

//关闭 SOCKET 窗口
bool CTCPClientSocket::CloseSocketWnd()
{
	if (GetSafeHwnd()!=NULL) DestroyWindow();
	return true;
}

//SOCKET 消息处理程序
LRESULT	CTCPClientSocket::OnSocketNotifyMesage(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:	//连接信息
		{
			UINT uErrorCode=WSAGETSELECTERROR(lParam);
			if (uErrorCode==0) m_bConnectState=CONNECTED;
			else CloseSocket(false);
			if (m_pIService!=NULL) m_pIService->OnSocketConnectEvent(uErrorCode,this);
			return 0;
		}
	case FD_READ:	//读取数据
		{
			//读取网络数据
			int iLen=::recv(m_hSocket,(char *)(m_szBuffer+m_iReadBufLen),sizeof(m_szBuffer)-m_iReadBufLen,0);
			if (iLen==SOCKET_ERROR)
			{ 
				CloseSocket(true);
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
					if ((m_iReadBufLen<sizeof(NetMessageHead))||(m_iReadBufLen<(int)pNetHead->uMessageSize)) return 0;

					//拷贝数据
					UINT uMessageSize=pNetHead->uMessageSize;
					if (uMessageSize>sizeof(szNetBuffer)) throw TEXT("数据包过大");
					::CopyMemory(szNetBuffer,m_szBuffer,uMessageSize);

					//删除数据
					::MoveMemory(m_szBuffer,m_szBuffer+uMessageSize,(m_iReadBufLen-uMessageSize)*sizeof(BYTE));
					m_iReadBufLen-=uMessageSize;

					//处理数据
					uMessageSize-=sizeof(NetMessageHead);
					NetMessageHead * pHandleNetData=(NetMessageHead *)szNetBuffer;
					if (m_pIService->OnSocketReadEvent(pHandleNetData,uMessageSize?pHandleNetData+1:NULL,uMessageSize,this)==false)
					{
						CloseSocket(true);
						return 0;
					}
				} while ((m_iReadBufLen>0)&&(m_hSocket!=INVALID_SOCKET));
			}
			catch (...)	{ CloseSocket(true);TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__); }
			return 0;
		}
	case FD_CLOSE:	//关闭消息
		{
			CloseSocket();
			return 0;
		}
	}
	return 0;
}

//连接服务器
bool CTCPClientSocket::Connect(const TCHAR * szServerIP, UINT uPort)
{
	long dwServerIP=inet_addr(szServerIP);
	if (dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=::gethostbyname(szServerIP);
		if (lpHost==NULL) return false;
		dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	return Connect(dwServerIP,uPort);
}

//连接服务器
bool CTCPClientSocket::Connect(long int dwServerIP, UINT uPort)
{
	//效验数据
	if (dwServerIP==INADDR_NONE) return false;

	//初始化数据
	InitSocketWnd();
	m_hSocket=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_hSocket==INVALID_SOCKET) return false;
	if (::WSAAsyncSelect(m_hSocket,GetSafeHwnd(),WM_SOCKET_MESSAGE,FD_READ|FD_CONNECT|FD_CLOSE)==SOCKET_ERROR) return false;

	//解释服务器地址
	sockaddr_in SocketAddr;
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_port=htons(uPort);
	SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

	//连接操作
	if ((::connect(m_hSocket,(sockaddr *)&SocketAddr,sizeof(SocketAddr))==SOCKET_ERROR)
		&&(::WSAGetLastError()!=WSAEWOULDBLOCK))
	{
		CloseSocket(false);
		return false;
	}

	//设置数据
	m_bConnectState=CONNECTING;
	return true;
}

//关闭 SOCKET
bool CTCPClientSocket::CloseSocket(bool bNotify)
{
	bool bClose=(m_hSocket!=INVALID_SOCKET);
	m_bConnectState=NO_CONNECT;
	if (m_hSocket!=INVALID_SOCKET)
	{
		::WSAAsyncSelect(m_hSocket,this->GetSafeHwnd(),WM_SOCKET_MESSAGE,0);
		::closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
		m_iReadBufLen=0;
	}
	if ((bNotify==true)&&(bClose==true)&&(m_pIService!=NULL)) m_pIService->OnSocketCloseEvent();
	return bClose;
}

//发送函数
int CTCPClientSocket::SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if ((m_hSocket!=INVALID_SOCKET)&&(uSize<=MAX_SEND_SIZE))
	{
		//定义数据
		int iErrorCode=0;
		char bSendBuffer[MAX_SEND_SIZE+NET_HEAD_SIZE];
		UINT uSendSize=uSize+sizeof(NetMessageHead),uSended=0;

		//打包数据
		NetMessageHead * pNetHead=(NetMessageHead *)bSendBuffer;
		pNetHead->uMessageSize=uSendSize;
		pNetHead->bMainID=bMainID;
		pNetHead->bAssistantID=bAssistantID;
		pNetHead->bHandleCode=bHandleCode;
		pNetHead->bReserve = m_iCheckCode;

		if (uSize>0) ::CopyMemory(bSendBuffer+sizeof(NetMessageHead),pData,uSize);

		//发送数据
		do
		{
			iErrorCode=::send(m_hSocket,bSendBuffer+uSended,uSendSize-uSended,0);
			if (iErrorCode==SOCKET_ERROR) 
			{
				if (::WSAGetLastError()==WSAEWOULDBLOCK) return uSize;
				else return SOCKET_ERROR;
			}
			uSended+=iErrorCode;
		} while (uSended<uSendSize);

		return uSize;
	}
	return SOCKET_ERROR;
}

//简单命令发送函数
int CTCPClientSocket::SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return SendData(NULL,0,bMainID,bAssistantID,bHandleCode);
}

//设置校验码，密文与密钥
void CTCPClientSocket::SetCheckCode(__int64 iCheckCode, int isecretkey)
{
	m_iCheckCode = (iCheckCode - isecretkey) / 23;
}

/**************************************************************************************************************/

//构造函数
CTCPSocket::CTCPSocket(void)
{
	m_bSending=false;
	m_uIndex=0;
	m_dwHandleID=0;
	m_lBeginTime=0;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=INVALID_SOCKET;
	m_pManage=NULL;
	memset(&m_SocketAddr,0,sizeof(m_SocketAddr));
	memset(&m_SendOverData,0,sizeof(m_SendOverData));
	memset(&m_RecvOverData,0,sizeof(m_RecvOverData));
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;
}

//析够函数
CTCPSocket::~CTCPSocket(void)
{
	CloseSocket();
}

//合并 SOCKET
bool CTCPSocket::Attach(SOCKET hSocket, sockaddr_in & Addr, DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	CSignedLockObject SendLock(&m_csSendLock,true);

	//效验数据 
	if (hSocket==INVALID_SOCKET) return false;

	//记录数据
	m_bSending=false;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=hSocket;
	m_SocketAddr=Addr;
	m_dwHandleID=dwHandleID;
	m_lBeginTime=(long int)time(NULL);
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;

	return true;
}

//关闭 SOCKET
bool CTCPSocket::CloseSocket()
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	CSignedLockObject SendLock(&m_csSendLock,true);

	bool bClose=(m_hSocket!=INVALID_SOCKET);
	::closesocket(m_hSocket);
	m_bSending=false;
	m_dwHandleID=0;
	m_lBeginTime=0;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=INVALID_SOCKET;
	memset(&m_SocketAddr,0,sizeof(m_SocketAddr));
	memset(&m_SendOverData,0,sizeof(m_SendOverData));
	memset(&m_RecvOverData,0,sizeof(m_RecvOverData));
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;

	return bClose;
}

//发送数据函数
int CTCPSocket::SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject SendLock(&m_csSendLock,true);

	//效验数据
	if (dwHandleID==0) dwHandleID=m_dwHandleID;
	if ((m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)||(dwHandleID!=m_dwHandleID)) return SOCKET_ERROR;
	if (sizeof(NetMessageHead)>(SED_SIZE-m_dwSendBuffLen)) return SOCKET_ERROR;

	//发送数据
	NetMessageHead * pNetHead=(NetMessageHead *)(m_szSendBuf+m_dwSendBuffLen);
	pNetHead->bReserve=0;
	pNetHead->bMainID=bMainID;
	pNetHead->bAssistantID=bAssistantID;
	pNetHead->bHandleCode=bHandleCode;
	pNetHead->uMessageSize=sizeof(NetMessageHead);
	m_dwSendBuffLen+=sizeof(NetMessageHead);
	return CheckSendData()?sizeof(NetMessageHead):0;
}

//发送数据函数
int CTCPSocket::SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if (uBufLen<=MAX_SEND_SIZE)
	{
		//锁定数据
		UINT uSendSize=sizeof(NetMessageHead)+uBufLen;
		CSignedLockObject SendLock(&m_csSendLock,true);

		//效验数据
		if (dwHandleID==0) dwHandleID=m_dwHandleID;
		if ((m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)||(dwHandleID!=m_dwHandleID))
		{
			////wushuqun 2009.5.22
			//::OutputDebugString("函数 SendData: (m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)||(dwHandleID!=m_dwHandleID) 发送数据失败了");
			return SOCKET_ERROR;
		}
		if (uSendSize>(SED_SIZE-m_dwSendBuffLen))
		{
			//CString str;
			//str.Format("uSendSize :%d,缓冲区大小 %d K,已使用 %d K",
			//	uSendSize,SED_SIZE / 1024 ,m_dwSendBuffLen / 1024);
			////wushuqun 2009.5.22
			//::OutputDebugString("函数 SendData: uSendSize>(SED_SIZE-m_dwSendBuffLen 发送数据失败了");
			//::OutputDebugString(str);
			return SOCKET_ERROR;
		}

		//发送数据
		NetMessageHead * pNetHead=(NetMessageHead *)(m_szSendBuf+m_dwSendBuffLen);
		pNetHead->bMainID=bMainID;
		pNetHead->bAssistantID=bAssistantID;
		pNetHead->bHandleCode=bHandleCode;
		pNetHead->uMessageSize=uSendSize;
		pNetHead->bReserve=0;
		if (pData!=NULL) CopyMemory(pNetHead+1,pData,uBufLen);
		m_dwSendBuffLen+=uSendSize;
		return CheckSendData()?uBufLen:0;
	}
	////wushuqun 2009.5.22
	//::OutputDebugString("函数 SendData: uBufLen > MAX_SEND_SIZE发送数据失败了");
	return 0;
}

//检测发送数据函数
bool CTCPSocket::CheckSendData()
{
	//锁定数据
	CSignedLockObject SendLock(&m_csSendLock,true);

	if ((m_bSending==false)&&(m_dwSendBuffLen>0))
	{
		DWORD dwThancferred=0;
		m_bSending=true;
		m_SendOverData.dwHandleID=m_dwHandleID;
		m_SendOverData.WSABuffer.buf=m_szSendBuf;
		m_SendOverData.WSABuffer.len=m_dwSendBuffLen;
		if ((::WSASend(m_hSocket,&m_SendOverData.WSABuffer,1,&dwThancferred,0,(LPWSAOVERLAPPED)&m_SendOverData,NULL)==SOCKET_ERROR)
			&&(::WSAGetLastError()!=WSA_IO_PENDING))
		{
			////wushuqun 2009.5.22
			//::OutputDebugString("函数 CheckSendData: 发送数据失败了");

			m_bSending=false;
			return false;
		}
	}
	return true;
}

//发送完成函数
bool CTCPSocket::OnSendCompleted(DWORD dwThancferred, DWORD dwHandleID)
{
	CSignedLockObject SendLock(&m_csSendLock,true);
	if ((dwHandleID==0)||(m_dwHandleID!=dwHandleID)) return true;

	//处理数据
	m_bSending=false;
	if ((dwThancferred>0)&&(m_dwSendBuffLen>=dwThancferred))
	{
		m_dwSendBuffLen-=dwThancferred;
		::MoveMemory(m_szSendBuf,&m_szSendBuf[dwThancferred],m_dwSendBuffLen*sizeof(m_szSendBuf[0]));
		return CheckSendData();
	}

	return false;
}

//接收完成函数
bool CTCPSocket::OnRecvCompleted(DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);

	//效验数据
	if ((m_dwHandleID==0)||(m_dwHandleID!=dwHandleID))	return false;

	try
	{
		//接收数据
		int iRecvCode=::recv(m_hSocket,m_szRecvBuf+m_dwRecvBuffLen,RCV_SIZE-m_dwRecvBuffLen,0);
			TRACE(TEXT("RECEIVE DATA SIZE:%d\n"),iRecvCode);
		if (iRecvCode>0)
		{
			//处理数据
			m_dwRecvBuffLen+=iRecvCode;
			NetMessageHead * pNetHead=(NetMessageHead *)m_szRecvBuf;
			if(pNetHead->bMainID==200)
			{
				m_pManage->CloseSocket(this,m_dwHandleID);
				return false;
			}
			while ((m_dwRecvBuffLen>=sizeof(NetMessageHead))&&(m_dwRecvBuffLen>=pNetHead->uMessageSize))
			{
				//效验数据
				if (pNetHead->uMessageSize<sizeof(NetMessageHead)) throw ("非法数据包");

				///校验码
				if (pNetHead->bReserve != (int)m_hSocket) throw("非法数据包");

				//打包数据
				UINT uMessageSize=pNetHead->uMessageSize;
				UINT uHandleSize=uMessageSize-sizeof(NetMessageHead);
				if (uMessageSize>MAX_SEND_SIZE) throw TEXT("数据包太大");
				if (m_pManage->OnNetMessage(this,pNetHead,uHandleSize?pNetHead+1:NULL,uHandleSize,m_uIndex,m_dwHandleID)==false)
					throw TEXT("数据处理失败");

				//删除缓存数据
				::MoveMemory(m_szRecvBuf,m_szRecvBuf+uMessageSize,m_dwRecvBuffLen-uMessageSize);
				m_dwRecvBuffLen-=uMessageSize;
			}
			return RecvData();
		}
	}
	catch (...) 
	{
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		DWORD dwErr=::GetLastError();
	}
	m_pManage->CloseSocket(this,m_dwHandleID);

	return false;
}

//投递接收 IO
bool CTCPSocket::RecvData()
{
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	DWORD dwThancferred=0,dwFlags=0;
	m_RecvOverData.dwHandleID=m_dwHandleID;
	m_RecvOverData.WSABuffer.buf=m_szRecvBuf+m_dwRecvBuffLen;
	m_RecvOverData.WSABuffer.len=0;
	if ((::WSARecv(m_hSocket,&m_RecvOverData.WSABuffer,1,&dwThancferred,&dwFlags,(LPWSAOVERLAPPED)&m_RecvOverData,NULL))
		&&(::WSAGetLastError()!=WSA_IO_PENDING))
	{
		m_pManage->CloseSocket(this,m_dwHandleID);
		return false;
	}
	return true;
}

//判断是否连接
bool CTCPSocket::IsConnectID(DWORD dwHandleID)
{
	if ((dwHandleID!=m_dwHandleID)||(m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)) return false;
	return true;
}

/**************************************************************************************************************/

//构造函数
CTCPSocketManage::CTCPSocketManage()
{
	m_bRun=false;
	m_bInit=false;
	m_bCheckConnect=true;
	m_bMaxVer=0;
	m_bLessVer=0;
	m_uListenPort=0;
	m_uRSThreadCount=0;
	m_uBatchCerrenPos=0;
	m_uBatchLineCount=0;
	m_uCheckTime=15000;			//最开始是15000，后来有人改成5000经常掉线 fuckfuckfuck
	m_pKeep=NULL;
	m_hEventThread=NULL;
	m_hCompletionPortRS=NULL;
	m_hCompletionPortBatch=NULL;
	m_hThreadKeep=NULL;
	m_hThreadAccept=NULL;
	ZeroMemory(m_SendLine,sizeof(m_SendLine));
///	 m_CurConnectCount=0;
	return;
}

//析构函数
CTCPSocketManage::~CTCPSocketManage()
{
	SafeDeleteArray(m_pKeep);
	return;
}

//初始化
bool CTCPSocketManage::Init(UINT uMaxCount, UINT uListenPort, BYTE bMaxVer, BYTE bLessVer, int iSecretKey, IServerSocketService * pService)
{
	//效验参数
	if ((this==NULL)||(m_bInit==true)||(m_bRun==true)) throw new CAFCException(TEXT("CTCPSocketManage::Init 状态效验失败"),0x409);
	if ((uMaxCount==0)||(uListenPort==0)) throw new CAFCException(TEXT("CTCPSocketManage::Init 输入参数效验失败"),0x40A);

	//设置参数
	m_bInit=true;
	m_pService=pService;
	m_bMaxVer=bMaxVer;
	m_bLessVer=bLessVer;
	m_uListenPort=uListenPort;
	m_iSecretKey = iSecretKey;
	::ZeroMemory(m_SendLine,sizeof(m_SendLine));

	//申请内存
	m_pKeep=new BYTE [uMaxCount];
	if (m_pKeep==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Init 申请内存失败"),0x40B);
	::ZeroMemory(m_pKeep,sizeof(BYTE)*uMaxCount);

	//建立 SOCKET 数组
	if (m_SocketPtrArray.Init(uMaxCount,uMaxCount)==false) throw new CAFCException(TEXT("CTCPSocket 初始化失败"),0x40C);
	for (UINT i=0;i<uMaxCount;i++)
	{
		m_SocketPtrArray.GetArrayItem(i)->SetIndex(i);
		m_SocketPtrArray.GetArrayItem(i)->SetSocketManagePtr(this);
	}
	return true;
}

//取消初始化
bool CTCPSocketManage::UnInit()
{
	if (this==NULL) return false;

	//设置数据 
	m_bInit=false;
	m_pService=NULL;
	m_uBatchCerrenPos=0;
	m_uBatchLineCount=0;
	m_uListenPort=0;
	ZeroMemory(m_SendLine,sizeof(m_SendLine));

	//清理内存
	m_SocketPtrArray.UnInit();
	SafeDeleteArray(m_pKeep);

	return true;
}

//开始服务
bool CTCPSocketManage::Start(UINT uAcceptThreadCount, UINT uSocketThradCount)
{
	if ((this==NULL)||(m_bInit==false)||(m_bRun==true)) throw new CAFCException(TEXT("CTCPSocketManage::Start 没有初始化参数"),0x40D);

	//获取系统信息
	SYSTEM_INFO SystemInfo;
	::GetSystemInfo(&SystemInfo);
	if (uSocketThradCount==0) uSocketThradCount=SystemInfo.dwNumberOfProcessors*2;

	//建立事件
	m_hEventThread=::CreateEvent(NULL,TRUE,false,NULL);
	if (m_hEventThread==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start m_hEventThread 建立失败"),0x40E);

	//建立完成端口
	m_hCompletionPortRS=::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPortRS==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start m_hCompletionPortRS 建立失败"),0x40F);

	//建立批量发送完成端口
	m_hCompletionPortBatch=::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPortBatch==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start m_hCompletionPortBatch 建立失败"),0x410);

	//建立变量
	UINT				uThreadID=0;
	HANDLE				hThreadHandle=NULL;
	ThreadStartStruct	ThreadStartData;
	ThreadStartData.pSocketManage=this;

	//建立读写线程
	ThreadStartData.hThreadEvent=m_hEventThread;
	ThreadStartData.hCompletionPort=m_hCompletionPortRS;
	for (UINT i=0;i<uSocketThradCount;i++)
	{
		hThreadHandle=(HANDLE)::_beginthreadex(NULL,0,ThreadRSSocket,&ThreadStartData,0,&uThreadID);
		if (hThreadHandle==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start ThreadStartData 线程建立失败"),0x411);
		m_uRSThreadCount++;
		::WaitForSingleObject(m_hEventThread,INFINITE);
		::ResetEvent(m_hEventThread);
		::CloseHandle(hThreadHandle);
	}

	//建立监听SOCKET
	if (!m_ListenSocket.Create())  throw new CAFCException(TEXT("CTCPSocketManage::Start 网络建立失败"),0x412);
	if (!m_ListenSocket.Bind(m_uListenPort))  throw new CAFCException(TEXT("CTCPSocketManage::Start 网络绑定失败"),0x413);
	if (!m_ListenSocket.Listen()) throw new CAFCException(TEXT("CTCPSocketManage::Start 网络监听失败"),0x414);

	//建立应答线程
	ThreadStartData.hCompletionPort=m_hCompletionPortRS;
	ThreadStartData.hThreadEvent=m_hEventThread;
	m_hThreadAccept=(HANDLE)::_beginthreadex(NULL,0,ThreadAccept,&ThreadStartData,0,&uThreadID);
	if (m_hThreadAccept==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start ThreadAccept 线程建立失败"),0x415);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//建立维护线程
	ThreadStartData.hCompletionPort=NULL;
	ThreadStartData.hThreadEvent=m_hEventThread;
	m_hThreadKeep=(HANDLE)::_beginthreadex(NULL,0,ThreadKeepActive,&ThreadStartData,0,&uThreadID);
	if (m_hThreadKeep==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start ThreadKeepActive 线程建立失败"),0x416);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//建立批量发送线程
	ThreadStartData.hCompletionPort=m_hCompletionPortBatch;
	ThreadStartData.hThreadEvent=m_hEventThread;
	hThreadHandle=(HANDLE)::_beginthreadex(NULL,0,ThreadBatchSend,&ThreadStartData,0,&uThreadID);
	if (hThreadHandle==NULL) throw new CAFCException(TEXT("CTCPSocketManage::Start ThreadBatchSend 线程建立失败"),0x417);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//关闭事件
	m_bRun=true;
	return true;
}

//停止服务
bool CTCPSocketManage::Stop()
{
	if (this==NULL) return false;

	//关闭 SOCKET
	m_bRun=false;
	m_ListenSocket.CloseSocket();

	//退出应答线程
	if (m_hThreadAccept!=NULL) 
	{
		DWORD dwCode=::WaitForSingleObject(m_hThreadAccept,TIME_OUT);
		if (dwCode==WAIT_TIMEOUT) ::TerminateThread(m_hThreadAccept,1);
		::CloseHandle(m_hThreadAccept);
		m_hThreadAccept=NULL;
	}

	//关闭完成端口
	if (m_hCompletionPortRS!=NULL)
	{
		for (UINT i=0;i<m_uRSThreadCount;i++)
		{
			::PostQueuedCompletionStatus(m_hCompletionPortRS,0,NULL,NULL);
			::WaitForSingleObject(m_hEventThread,TIME_OUT);
			::ResetEvent(m_hEventThread);
		}
		m_uRSThreadCount=0;
		::CloseHandle(m_hCompletionPortRS);
		m_hCompletionPortRS=NULL;
	}

	//关闭维护线程
	if (m_hThreadKeep!=NULL)
	{
		if (::WaitForSingleObject(m_hEventThread,TIME_OUT)==WAIT_TIMEOUT) ::TerminateThread(m_hThreadKeep,0);
		::CloseHandle(m_hThreadKeep);
		m_hThreadKeep=NULL;
	}

	//关闭批量发送线程
	if (m_hCompletionPortBatch!=NULL)
	{
		::PostQueuedCompletionStatus(m_hCompletionPortBatch,0,NULL,NULL);
		::WaitForSingleObject(m_hEventThread,TIME_OUT);
		::ResetEvent(m_hEventThread);
		::CloseHandle(m_hCompletionPortBatch);
		m_hCompletionPortBatch=NULL;
	}

	//关闭 SOCKET
	for (UINT i=0;i<m_SocketPtrArray.GetPermitCount();i++) 
	{
		m_SocketPtrArray.GetArrayItem(i)->CloseSocket();
	}

	//关闭事件
	if (m_hEventThread!=NULL)
	{
		::CloseHandle(m_hEventThread);
		m_hEventThread=NULL;
	}

	return true;
}

//发送数据函数
int CTCPSocketManage::SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if ((uIndex<m_SocketPtrArray.GetPermitCount())&&(m_bRun==true))
	{
		return m_SocketPtrArray.GetArrayItem(uIndex)->SendData(bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	return 0;
}

//发送数据函数
int CTCPSocketManage::SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	////wushuqun 2009.5.22
	//::OutputDebugString("函数 CTCPSocketManage::SendData: 发送数据-1");

	if ((uIndex<m_SocketPtrArray.GetPermitCount())&&(m_bRun==true))
	{
		return m_SocketPtrArray.GetArrayItem(uIndex)->SendData(pData,uBufLen,bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	////wushuqun 2009.5.22
	//::OutputDebugString("函数 CTCPSocketManage::SendData: 发送数据-2");
	return 0;
}

//发送数据函数
int CTCPSocketManage::SendData(CTCPSocket * pSocket, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if ((pSocket!=NULL)&&(m_bRun==true)) 
	{
		return pSocket->SendData(pData,uBufLen,bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	return 0;
}

//批量发送函数
int CTCPSocketManage::SendDataBatch(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	//效验数据
	if ((m_bRun==false)||(uSize>(sizeof(m_SendLine[0].szSendData)-sizeof(NetMessageHead)))) return 0;

	CSignedLockObject BatchLock(&m_csBacthSend,true);
	if (m_uBatchLineCount>=uBatchLineSize) return 0;

	//写入缓冲
	UINT m_uWritePos=(m_uBatchCerrenPos+m_uBatchLineCount)%uBatchLineSize;
	m_SendLine[m_uWritePos].NetHead.bMainID=bMainID;
	m_SendLine[m_uWritePos].NetHead.bAssistantID=bAssistantID;
	m_SendLine[m_uWritePos].NetHead.bHandleCode=bHandleCode;
	m_SendLine[m_uWritePos].NetHead.uMessageSize=uSize+sizeof(NetMessageHead);
	m_SendLine[m_uWritePos].NetHead.bReserve=0;
	if (uSize>0) CopyMemory(m_SendLine[m_uWritePos].szSendData,pData,uSize);

	//激发发送线程
	m_uBatchLineCount++;
	PostQueuedCompletionStatus(m_hCompletionPortBatch,uSize+sizeof(NetMessageHead),NULL,NULL);
	
	return uSize;
}

//关闭连接
bool CTCPSocketManage::CloseSocket(UINT uIndex, DWORD dwHandleID)
{ 
	if (uIndex<m_SocketPtrArray.GetPermitCount()) 
	{
		return CloseSocket(m_SocketPtrArray.GetArrayItem(uIndex),dwHandleID);
	}
	return false;
}

//关闭连接
bool CTCPSocketManage::CloseSocket(CTCPSocket * pSocket, DWORD dwHandleID) 
{ 
	if (pSocket!=NULL)
	{
		CSignedLockObject RecvLock(pSocket->GetRecvLock(),true);
		CSignedLockObject SendLock(pSocket->GetSendLock(),true);

		//效验数据
		if (dwHandleID==0) dwHandleID=pSocket->m_dwHandleID;
		if ((pSocket->m_dwHandleID==0)||(dwHandleID!=pSocket->m_dwHandleID)) return false;

		//设置数据
		ULONG dwAccessIP=pSocket->GetConnectData();
		long int lConnectTime=(long int)time(NULL)-pSocket->GetBeginTime();
		*(m_pKeep+pSocket->m_uIndex)=0;

		//关闭处理
		if ((pSocket->CloseSocket()==true)&&(m_pService!=NULL))	m_pService->OnSocketCloseEvent(dwAccessIP,pSocket->m_uIndex,lConnectTime);
		
		//清理信息
		TCPSocketDelete(pSocket);

		return true;
	}
	return false;
}

//判断是否连接
bool CTCPSocketManage::IsConnectID(UINT uIndex, DWORD dwHandleID)
{
	if (uIndex>=m_SocketPtrArray.GetPermitCount()) return false;
	return m_SocketPtrArray.GetArrayItem(uIndex)->IsConnectID(dwHandleID);
}



//消息处理函数
bool CTCPSocketManage::OnNetMessage(CTCPSocket * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	*(m_pKeep+uIndex)=0;
	//效验网络测试数据
	if ((pNetHead->bMainID==MDM_CONNECT)&&(pNetHead->bAssistantID==ASS_NET_TEST)) 
	{
		//*(m_pKeep+uIndex)=0;  以前这句是放这里的,但是应该是收到任何消息都应该把心跳清0
		return true;
	}
	if (m_pService!=NULL) return m_pService->OnSocketReadEvent(pSocket,pNetHead,pData,uSize,uIndex,dwHandleID);
	return false;
}


//SOCKET 数据接收线程
unsigned __stdcall CTCPSocketManage::ThreadRSSocket(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct	* pStartData=(ThreadStartStruct *)pThreadData;			//线程启动数据指针
	CTCPSocketManage	* pSocketManage=pStartData->pSocketManage;				//管理指针
	HANDLE				hCompletionPort=pStartData->hCompletionPort;			//完成端口
	HANDLE				hEvent=pStartData->hThreadEvent;						//事件

	//线程数据读取完成
	::SetEvent(hEvent);

	//重叠数据
	DWORD							dwThancferred=0;							//接收数量
	CTCPSocket						* pTCPSocket=NULL;							//完成键
	CTCPSocket::OverLappedStruct	* pOverData=NULL;							//重叠数据

	while (1)
	{
		try
		{
			//等待完成端口
			dwThancferred=0;
			pTCPSocket=NULL;
			pOverData=NULL;
			::GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPSocket,(LPOVERLAPPED *)&pOverData,INFINITE);
			if ((pTCPSocket==NULL)||(pOverData==NULL))
			{
				if ((pTCPSocket==NULL)&&(pOverData==NULL)) ::SetEvent(hEvent);
				_endthreadex(0);
			}

			//处理完成端口
			if ((dwThancferred==0)&&(pOverData->uOperationType==SOCKET_SND))
			{
				//SOCKET 关闭
				pSocketManage->CloseSocket(pTCPSocket,pOverData->dwHandleID);
				continue;
			}

			switch (pOverData->uOperationType)
			{
			case SOCKET_REV:	//SOCKET 数据读取
				{
					pTCPSocket->OnRecvCompleted(pOverData->dwHandleID);
					break;
				}
			case SOCKET_SND:	//SOCKET 数据发送
				{
					pTCPSocket->OnSendCompleted(dwThancferred,pOverData->dwHandleID);
					break;
				}
			}
		}
		catch (...) 
		{
			TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		}
	}

	return 0;
}

//SOCKET 批量发送数据线程
unsigned __stdcall CTCPSocketManage::ThreadBatchSend(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct		* pStartData=(ThreadStartStruct *)pThreadData;					//线程启动数据指针
	CTCPSocketManage		* pSocketManage=pStartData->pSocketManage;						//管理指针
	UINT					uMaxCount=pSocketManage->m_SocketPtrArray.GetPermitCount();		//最大数目
	HANDLE					hCompletionPort=pStartData->hCompletionPort;					//完成端口
	HANDLE					hEvent=pStartData->hThreadEvent;								//事件
	
	//启动完成
	SetEvent(pStartData->hThreadEvent);


	//临时变量
	NetMessageHead			* pNetHead=NULL;												//数据包头
	CTCPSocket				* pSendSocket=pSocketManage->m_SocketPtrArray.GetArrayItem(0);	//发送 SOCKET
	UINT					* puSendCount=&pSocketManage->m_uBatchLineCount;				//当前数量

	DWORD					dwThancferred=0;												//接收数量
	ULONG					dwCompleteKey=0L;												//重叠 IO 临时数据
	LPOVERLAPPED			OverData;														//重叠 IO 临时数据

	while (1)
	{
		try
		{
			//提取发送数据
			dwThancferred=0;
			GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,&dwCompleteKey,(LPOVERLAPPED *)&OverData,INFINITE);
			if (dwThancferred==0)
			{
				SetEvent(hEvent);
				_endthreadex(0);
			}

			//发送数据
			BatchSendLineStruct * pBatchData=&pSocketManage->m_SendLine[pSocketManage->m_uBatchCerrenPos];
			for (UINT i=0;i<uMaxCount;i++) 
			{
				if ((pSendSocket+i)->IsConnect())
				{
					(pSendSocket+i)->SendData(pBatchData->szSendData,pBatchData->NetHead.uMessageSize-sizeof(NetMessageHead),
						pBatchData->NetHead.bMainID,pBatchData->NetHead.bAssistantID,pBatchData->NetHead.bHandleCode,0);
				}
			}

			//处理数据
			{
				CSignedLockObject BatchLock(&pSocketManage->m_csBacthSend,true);
				pSocketManage->m_uBatchCerrenPos=(pSocketManage->m_uBatchCerrenPos+1)%uBatchLineSize;
				pSocketManage->m_uBatchLineCount--;
			}
		}
		catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	}

	return 0;
}

//SOCKET 维护线程
unsigned __stdcall CTCPSocketManage::ThreadKeepActive(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct		* pStartData=(ThreadStartStruct *)pThreadData;		//启动指针
	CTCPSocketManage		* pSocketManage=pStartData->pSocketManage;			//管理指针
	HANDLE					m_hThreadEvent=pStartData->hThreadEvent;			//线程事件
	
	//启动完成
	::SetEvent(m_hThreadEvent);

	//数据定义
	UINT uCheckCount=0;
	::Sleep(pSocketManage->m_uCheckTime);

	while (1)
	{
		//发送测试数据
		pSocketManage->SendDataBatch(NULL,0,MDM_CONNECT,ASS_NET_TEST,0);

		//检查退出标志
		uCheckCount=0;
		while (uCheckCount<pSocketManage->m_uCheckTime)
		{
			if (!pSocketManage->m_bRun) 
			{
				SetEvent(m_hThreadEvent);
				_endthreadex(0);
			}
			::Sleep(1000);
			uCheckCount+=1000;
		}

		//效验连接数据
		if ((pSocketManage->m_bCheckConnect)&&(pSocketManage->m_bRun)&&(pSocketManage->m_SocketPtrArray.GetArrayItem(0)!=NULL))
		{
			for (int i=(pSocketManage->m_SocketPtrArray.GetPermitCount()-1);i>=0;i--)
			{
				if (pSocketManage->m_SocketPtrArray.GetArrayItem(i)->IsConnect())
				{
					if ((*(pSocketManage->m_pKeep+i))>=3) //如果超出3次计数，则断开该客户连接
						pSocketManage->CloseSocket(i,0);
					else 
						(*(pSocketManage->m_pKeep+i))++;//小于3次读数，则给读数器+1
				}
			}
		}
	}

	return 0;
}

//获取时间函数
unsigned __int64 CTCPSocketManage::AFCGetTimer()
{
	_asm _emit 0x0F;
	_asm _emit 0x31;
}

//获取处理 ID 号码
DWORD CTCPSocketManage::GetHandleID()
{
	static long int dwBaseID=1L;
	if (dwBaseID>3000000L) dwBaseID=1L;
	return ::InterlockedIncrement(&dwBaseID);
}

//获取本机 IP 地址
bool CTCPSocketManage::GetLoaclIPAddr(TCHAR szAddrBuffer[], UINT uStation)
{
	if (szAddrBuffer==NULL) return false;

	TCHAR szHostName[51];
	::gethostname(szHostName,sizeof(szHostName));
	HOSTENT * pHostInfo=::gethostbyname(szHostName);
	if (pHostInfo!=NULL)
	{
		LPIN_ADDR * pAddrList=(LPIN_ADDR *)(pHostInfo->h_addr_list); 
		LPIN_ADDR In_Addr=*pAddrList;

		UINT uCount=0;
		szAddrBuffer[0]=0;
		while (In_Addr!=NULL)
		{
			if (uCount==uStation)
			{
				lstrcpy(szAddrBuffer,inet_ntoa(*In_Addr));
				return true;
			}
			uCount++;
			pAddrList++;
			In_Addr=*pAddrList;
		}
	}
	return false;
}

//数值变 IP 字符
TCHAR * CTCPSocketManage::ULongToIP(ULONG dwIP, TCHAR * szIP)
{
	wsprintf(szIP,TEXT("%d.%d.%d.%d"),FOURTH_IPADDRESS(dwIP),THIRD_IPADDRESS(dwIP),SECOND_IPADDRESS(dwIP),FIRST_IPADDRESS(dwIP));
	return szIP;
}

//获取指定index的连接信息
string CTCPSocketManage::GetServerInfo(UINT uIndex)
{
	string s = "";
	for (UINT i=0;i<m_SocketPtrArray.GetPermitCount();i++) 
	{
		if(m_SocketPtrArray.GetArrayItem(i)->m_uIndex == uIndex)
		{
			//memcpy(pSocket, &(m_SocketPtrArray.GetArrayItem(i)->m_SocketAddr), sizeof(sockaddr_in));
			s = inet_ntoa(m_SocketPtrArray.GetArrayItem(i)->m_SocketAddr.sin_addr);
			return s;
		}
	}
	return s;
}

/**************************************************************************************************************/
//////////////////////////////////////////////////////////////
// 以下为专为Z服务器准备的Socket类的实现

/**************************************************************************************************************/
//生成空闲 SOCKET
CTCPSocketForZ * CTCPSocketManageForZ::TCPSocketNew()
{
	////if(m_SocketPtrArray.GetActiveCount() > MaxConnectCount)return NULL;
	if (m_bRun)
	{
		CSignedLockObject LockObject(m_SocketPtrArray.GetArrayLock(),true);
		return m_SocketPtrArray.NewItem();
	}
	return NULL;
}


//释放活动 SOCKET
bool CTCPSocketManageForZ::TCPSocketDelete(CTCPSocketForZ * pSocket)
{
	if (pSocket!=NULL)
	{
		CSignedLockObject LockObject(m_SocketPtrArray.GetArrayLock(),true);
		pSocket->CloseSocket();
		return m_SocketPtrArray.FreeItem(pSocket);
	}
	return false;
}



//SOCKET 连接应答线程
unsigned __stdcall CTCPSocketManageForZ::ThreadAccept(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct	* pStartData=(ThreadStartStruct *)pThreadData;			//线程启动数据指针
	CTCPSocketManageForZ	* pSocketManage=pStartData->pSocketManage;				//管理指针
	HANDLE				hCompletionPort=pStartData->hCompletionPort;			//完成端口

	sockaddr_in			SocketAddr;
	int					iAddrBuferLen=sizeof(SocketAddr);
	SOCKET				hSocket=INVALID_SOCKET;
	CTCPSocketForZ			* pNewTCPSocket=NULL;
	HANDLE				hAcceptCompletePort=NULL;
	SOCKET				hListenScoket=pStartData->pSocketManage->m_ListenSocket.GetSafeSocket();

	//连接发送数据
	MSG_S_ConnectSuccess ConncetSuccess;
	ConncetSuccess.bMaxVer=pSocketManage->m_bMaxVer;
	ConncetSuccess.bLessVer=pSocketManage->m_bLessVer;

	//线程数据读取完成
	SetEvent(pStartData->hThreadEvent);

	while (1)
	{
		try
		{

			//接收连接
			hSocket=::WSAAccept(hListenScoket,(sockaddr *)&SocketAddr,&iAddrBuferLen,NULL,NULL);
			if (hSocket==INVALID_SOCKET) 
			{
				int iErrorCode=WSAGetLastError();
				_endthreadex(0);
			}

			//处理连接
			pNewTCPSocket=pSocketManage->TCPSocketNew();
			if ((pNewTCPSocket!=NULL)&&(pNewTCPSocket->Attach(hSocket,SocketAddr,GetHandleID())))
			{
				hAcceptCompletePort=::CreateIoCompletionPort((HANDLE)hSocket,hCompletionPort,(ULONG_PTR)pNewTCPSocket,0);
				if ((hAcceptCompletePort==NULL)||(!pNewTCPSocket->RecvData())) throw TEXT("Accept False");

				//发送连接成功消息

				MSG_S_ConnectSuccess Message;
				Message.bLessVer=MAKELONG(3,4);
				Message.bReserve[0]=0;
				Message.bMaxVer=MAKELONG(3,4);
				Message.bReserve[1]=0;
				//此处把hSocket进行加密，由客户端解密并在包头的bReserve字段中发过来校验。其中SECRET_KEY为密钥由客户自己管理在
				Message.i64CheckCode = (int)hSocket * 23 + pSocketManage->m_iSecretKey;
				pNewTCPSocket->SendData(&Message,sizeof(Message),MDM_CONNECT,ASS_CONNECT_SUCCESS,0,pNewTCPSocket->GetHandleID());

			}
			else
			{
				//没有空闲的SOCKET了，判断为房间满
				NetMessageHead netHead;
				ZeroMemory(&netHead,sizeof(NetMessageHead));
				netHead.bMainID=100;//MDM_GR_LOGON
				netHead.bAssistantID=3;//ASS_GR_LOGON_ERROR
				netHead.bHandleCode=10;//ERR_GR_PEOPLE_FULL
				netHead.uMessageSize=sizeof(NetMessageHead);
				netHead.bReserve=0;

				sendto(hSocket,(char *)&netHead,sizeof(NetMessageHead),0,(sockaddr*)&SocketAddr,sizeof(SocketAddr));
				throw TEXT("Accept False");
			}
		}
		catch (...)
		{TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		::closesocket(hSocket);
		if (pNewTCPSocket!=NULL) pSocketManage->TCPSocketDelete(pNewTCPSocket);
		}
	}

	return 0;
}

//构造函数
CTCPSocketForZ::CTCPSocketForZ(void)
{
	m_bSending=false;
	m_uIndex=0;
	m_dwHandleID=0;
	m_lBeginTime=0;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=INVALID_SOCKET;
	m_pManage=NULL;
	memset(&m_SocketAddr,0,sizeof(m_SocketAddr));
	memset(&m_SendOverData,0,sizeof(m_SendOverData));
	memset(&m_RecvOverData,0,sizeof(m_RecvOverData));
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;
}

//析够函数
CTCPSocketForZ::~CTCPSocketForZ(void)
{
	CloseSocket();
}

//合并 SOCKET
bool CTCPSocketForZ::Attach(SOCKET hSocket, sockaddr_in & Addr, DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	CSignedLockObject SendLock(&m_csSendLock,true);

	//效验数据 
	if (hSocket==INVALID_SOCKET) return false;

	//记录数据
	m_bSending=false;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=hSocket;
	m_SocketAddr=Addr;
	m_dwHandleID=dwHandleID;
	m_lBeginTime=(long int)time(NULL);
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;

	return true;
}

//关闭 SOCKET
bool CTCPSocketForZ::CloseSocket()
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	CSignedLockObject SendLock(&m_csSendLock,true);

	bool bClose=(m_hSocket!=INVALID_SOCKET);
	::closesocket(m_hSocket);
	m_bSending=false;
	m_dwHandleID=0;
	m_lBeginTime=0;
	m_dwSendBuffLen=0;
	m_dwRecvBuffLen=0;
	m_hSocket=INVALID_SOCKET;
	memset(&m_SocketAddr,0,sizeof(m_SocketAddr));
	memset(&m_SendOverData,0,sizeof(m_SendOverData));
	memset(&m_RecvOverData,0,sizeof(m_RecvOverData));
	m_SendOverData.uOperationType=SOCKET_SND;
	m_RecvOverData.uOperationType=SOCKET_REV;

	return bClose;
}

//发送数据函数
int CTCPSocketForZ::SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject SendLock(&m_csSendLock,true);

	//效验数据
	if (dwHandleID==0) dwHandleID=m_dwHandleID;
	if ((m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)||(dwHandleID!=m_dwHandleID)) return SOCKET_ERROR;
	if (sizeof(NetMessageHead)>(SED_SIZE_Z-m_dwSendBuffLen)) return SOCKET_ERROR;

	//发送数据
	NetMessageHead * pNetHead=(NetMessageHead *)(m_szSendBuf+m_dwSendBuffLen);
	pNetHead->bReserve=0;
	pNetHead->bMainID=bMainID;
	pNetHead->bAssistantID=bAssistantID;
	pNetHead->bHandleCode=bHandleCode;
	pNetHead->uMessageSize=sizeof(NetMessageHead);
	m_dwSendBuffLen+=sizeof(NetMessageHead);
	return CheckSendData()?sizeof(NetMessageHead):0;
}

//发送数据函数
int CTCPSocketForZ::SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if (uBufLen<=MAX_SEND_SIZE)
	{
		//锁定数据
		UINT uSendSize=sizeof(NetMessageHead)+uBufLen;
		CSignedLockObject SendLock(&m_csSendLock,true);

		//效验数据
		if (dwHandleID==0) dwHandleID=m_dwHandleID;
		if ((m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)||(dwHandleID!=m_dwHandleID)) return SOCKET_ERROR;
		if (uSendSize>(SED_SIZE_Z-m_dwSendBuffLen)) return SOCKET_ERROR;

		//发送数据
		NetMessageHead * pNetHead=(NetMessageHead *)(m_szSendBuf+m_dwSendBuffLen);
		pNetHead->bMainID=bMainID;
		pNetHead->bAssistantID=bAssistantID;
		pNetHead->bHandleCode=bHandleCode;
		pNetHead->uMessageSize=uSendSize;
		pNetHead->bReserve=0;
		if (pData!=NULL) CopyMemory(pNetHead+1,pData,uBufLen);
		m_dwSendBuffLen+=uSendSize;
		return CheckSendData()?uBufLen:0;
	}
	return 0;
}

//检测发送数据函数
bool CTCPSocketForZ::CheckSendData()
{
	//锁定数据
	CSignedLockObject SendLock(&m_csSendLock,true);

	if ((m_bSending==false)&&(m_dwSendBuffLen>0))
	{
		DWORD dwThancferred=0;
		m_bSending=true;
		m_SendOverData.dwHandleID=m_dwHandleID;
		m_SendOverData.WSABuffer.buf=m_szSendBuf;
		m_SendOverData.WSABuffer.len=m_dwSendBuffLen;
		if ((::WSASend(m_hSocket,&m_SendOverData.WSABuffer,1,&dwThancferred,0,(LPWSAOVERLAPPED)&m_SendOverData,NULL)==SOCKET_ERROR)
			&&(::WSAGetLastError()!=WSA_IO_PENDING))
		{
			m_bSending=false;
			return false;
		}
	}
	else
	{
		return true;
	}
	return true;
}

//发送完成函数
bool CTCPSocketForZ::OnSendCompleted(DWORD dwThancferred, DWORD dwHandleID)
{
	CSignedLockObject SendLock(&m_csSendLock,true);
	if ((dwHandleID==0)||(m_dwHandleID!=dwHandleID)) return true;

	//处理数据
	m_bSending=false;
	if ((dwThancferred>0)&&(m_dwSendBuffLen>=dwThancferred))
	{
		m_dwSendBuffLen-=dwThancferred;
		::MoveMemory(m_szSendBuf,&m_szSendBuf[dwThancferred],m_dwSendBuffLen*sizeof(m_szSendBuf[0]));
		return CheckSendData();
	}

	return false;
}

//接收完成函数
bool CTCPSocketForZ::OnRecvCompleted(DWORD dwHandleID)
{
	//锁定数据
	CSignedLockObject RecvLock(&m_csRecvLock,true);

	//效验数据
	if ((m_dwHandleID==0)||(m_dwHandleID!=dwHandleID))	return false;

	try
	{
		//接收数据
		int iRecvCode=::recv(m_hSocket,m_szRecvBuf+m_dwRecvBuffLen,RCV_SIZE_Z-m_dwRecvBuffLen,0);
		TRACE(TEXT("RECEIVE DATA SIZE:%d\n"),iRecvCode);
		if (iRecvCode>0)
		{
			//处理数据
			m_dwRecvBuffLen+=iRecvCode;
			NetMessageHead * pNetHead=(NetMessageHead *)m_szRecvBuf;
			if(pNetHead->bMainID==200)
			{
				m_pManage->CloseSocket(this,m_dwHandleID);
				return false;
			}
			while ((m_dwRecvBuffLen>=sizeof(NetMessageHead))&&(m_dwRecvBuffLen>=pNetHead->uMessageSize))
			{
				//效验数据
				if (pNetHead->uMessageSize<sizeof(NetMessageHead)) throw ("非法数据包");
				///校验码
				if (pNetHead->bReserve != (int)m_hSocket) throw("非法数据包");

				//打包数据
				UINT uMessageSize=pNetHead->uMessageSize;
				UINT uHandleSize=uMessageSize-sizeof(NetMessageHead);
				if (uMessageSize>MAX_SEND_SIZE) throw TEXT("数据包太大");
				if (m_pManage->OnNetMessage(this,pNetHead,uHandleSize?pNetHead+1:NULL,uHandleSize,m_uIndex,m_dwHandleID)==false)
					throw TEXT("数据处理失败");

				//删除缓存数据
				::MoveMemory(m_szRecvBuf,m_szRecvBuf+uMessageSize,m_dwRecvBuffLen-uMessageSize);
				m_dwRecvBuffLen-=uMessageSize;
			}
			return RecvData();
		}
	}
	catch (...) 
	{
		TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		DWORD dwErr=::GetLastError();
	}
	m_pManage->CloseSocket(this,m_dwHandleID);

	return false;
}

//投递接收 IO
bool CTCPSocketForZ::RecvData()
{
	CSignedLockObject RecvLock(&m_csRecvLock,true);
	DWORD dwThancferred=0,dwFlags=0;
	m_RecvOverData.dwHandleID=m_dwHandleID;
	m_RecvOverData.WSABuffer.buf=m_szRecvBuf+m_dwRecvBuffLen;
	m_RecvOverData.WSABuffer.len=0;
	if ((::WSARecv(m_hSocket,&m_RecvOverData.WSABuffer,1,&dwThancferred,&dwFlags,(LPWSAOVERLAPPED)&m_RecvOverData,NULL))
		&&(::WSAGetLastError()!=WSA_IO_PENDING))
	{
		m_pManage->CloseSocket(this,m_dwHandleID);
		return false;
	}
	return true;
}

//判断是否连接
bool CTCPSocketForZ::IsConnectID(DWORD dwHandleID)
{
	if ((dwHandleID!=m_dwHandleID)||(m_dwHandleID==0)||(m_hSocket==INVALID_SOCKET)) return false;
	return true;
}

/**************************************************************************************************************/

//构造函数
CTCPSocketManageForZ::CTCPSocketManageForZ()
{
	m_bRun=false;
	m_bInit=false;
	m_bCheckConnect=true;
	m_bMaxVer=0;
	m_bLessVer=0;
	m_uListenPort=0;
	m_uRSThreadCount=0;
	m_uBatchCerrenPos=0;
	m_uBatchLineCount=0;
	m_uCheckTime=15000;			//最开始是15000，后来有人改成5000经常掉线 fuckfuckfuck
	m_pKeep=NULL;
	m_hEventThread=NULL;
	m_hCompletionPortRS=NULL;
	m_hCompletionPortBatch=NULL;
	m_hThreadKeep=NULL;
	m_hThreadAccept=NULL;
	ZeroMemory(m_SendLine,sizeof(m_SendLine));
	///	 m_CurConnectCount=0;
	return;
}

//析构函数
CTCPSocketManageForZ::~CTCPSocketManageForZ()
{
	SafeDeleteArray(m_pKeep);
	return;
}

//初始化
bool CTCPSocketManageForZ::Init(UINT uMaxCount, UINT uListenPort, BYTE bMaxVer, BYTE bLessVer, int iSecretKey, IServerSocketServiceForZ * pService)
{
	//效验参数
	if ((this==NULL)||(m_bInit==true)||(m_bRun==true)) throw new CAFCException(TEXT("CTCPSocketManageForZ::Init 状态效验失败"),0x409);
	if ((uMaxCount==0)||(uListenPort==0)) throw new CAFCException(TEXT("CTCPSocketManageForZ::Init 输入参数效验失败"),0x40A);

	//设置参数
	m_bInit=true;
	m_pService=pService;
	m_bMaxVer=bMaxVer;
	m_bLessVer=bLessVer;
	m_uListenPort=uListenPort;
	m_iSecretKey = iSecretKey;
	::ZeroMemory(m_SendLine,sizeof(m_SendLine));

	//申请内存
	m_pKeep=new BYTE [uMaxCount];
	if (m_pKeep==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Init 申请内存失败"),0x40B);
	::ZeroMemory(m_pKeep,sizeof(BYTE)*uMaxCount);

	//建立 SOCKET 数组
	if (m_SocketPtrArray.Init(uMaxCount,uMaxCount)==false) throw new CAFCException(TEXT("CTCPSocketForZ 初始化失败"),0x40C);
	for (UINT i=0;i<uMaxCount;i++)
	{
		m_SocketPtrArray.GetArrayItem(i)->SetIndex(i);
		m_SocketPtrArray.GetArrayItem(i)->SetSocketManagePtr(this);
	}
	return true;
}

//取消初始化
bool CTCPSocketManageForZ::UnInit()
{
	if (this==NULL) return false;

	//设置数据 
	m_bInit=false;
	m_pService=NULL;
	m_uBatchCerrenPos=0;
	m_uBatchLineCount=0;
	m_uListenPort=0;
	ZeroMemory(m_SendLine,sizeof(m_SendLine));

	//清理内存
	m_SocketPtrArray.UnInit();
	SafeDeleteArray(m_pKeep);

	return true;
}

//开始服务
bool CTCPSocketManageForZ::Start(UINT uAcceptThreadCount, UINT uSocketThradCount)
{
	if ((this==NULL)||(m_bInit==false)||(m_bRun==true)) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start 没有初始化参数"),0x40D);

	//获取系统信息
	SYSTEM_INFO SystemInfo;
	::GetSystemInfo(&SystemInfo);
	if (uSocketThradCount==0) uSocketThradCount=SystemInfo.dwNumberOfProcessors*2;

	//建立事件
	m_hEventThread=::CreateEvent(NULL,TRUE,false,NULL);
	if (m_hEventThread==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start m_hEventThread 建立失败"),0x40E);

	//建立完成端口
	m_hCompletionPortRS=::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPortRS==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start m_hCompletionPortRS 建立失败"),0x40F);

	//建立批量发送完成端口
	m_hCompletionPortBatch=::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPortBatch==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start m_hCompletionPortBatch 建立失败"),0x410);

	//建立变量
	UINT				uThreadID=0;
	HANDLE				hThreadHandle=NULL;
	ThreadStartStruct	ThreadStartData;
	ThreadStartData.pSocketManage=this;

	//建立读写线程
	ThreadStartData.hThreadEvent=m_hEventThread;
	ThreadStartData.hCompletionPort=m_hCompletionPortRS;
	for (UINT i=0;i<uSocketThradCount;i++)
	{
		hThreadHandle=(HANDLE)::_beginthreadex(NULL,0,ThreadRSSocket,&ThreadStartData,0,&uThreadID);
		if (hThreadHandle==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start ThreadStartData 线程建立失败"),0x411);
		m_uRSThreadCount++;
		::WaitForSingleObject(m_hEventThread,INFINITE);
		::ResetEvent(m_hEventThread);
		::CloseHandle(hThreadHandle);
	}

	//建立监听SOCKET
	if (!m_ListenSocket.Create())  throw new CAFCException(TEXT("CTCPSocketManageForZ::Start 网络建立失败"),0x412);
	if (!m_ListenSocket.Bind(m_uListenPort))  throw new CAFCException(TEXT("CTCPSocketManageForZ::Start 网络绑定失败"),0x413);
	if (!m_ListenSocket.Listen()) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start 网络监听失败"),0x414);

	//建立应答线程
	ThreadStartData.hCompletionPort=m_hCompletionPortRS;
	ThreadStartData.hThreadEvent=m_hEventThread;
	m_hThreadAccept=(HANDLE)::_beginthreadex(NULL,0,ThreadAccept,&ThreadStartData,0,&uThreadID);
	if (m_hThreadAccept==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start ThreadAccept 线程建立失败"),0x415);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//建立维护线程
	ThreadStartData.hCompletionPort=NULL;
	ThreadStartData.hThreadEvent=m_hEventThread;
	m_hThreadKeep=(HANDLE)::_beginthreadex(NULL,0,ThreadKeepActive,&ThreadStartData,0,&uThreadID);
	if (m_hThreadKeep==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start ThreadKeepActive 线程建立失败"),0x416);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//建立批量发送线程
	ThreadStartData.hCompletionPort=m_hCompletionPortBatch;
	ThreadStartData.hThreadEvent=m_hEventThread;
	hThreadHandle=(HANDLE)::_beginthreadex(NULL,0,ThreadBatchSend,&ThreadStartData,0,&uThreadID);
	if (hThreadHandle==NULL) throw new CAFCException(TEXT("CTCPSocketManageForZ::Start ThreadBatchSend 线程建立失败"),0x417);
	::WaitForSingleObject(m_hEventThread,INFINITE);
	::ResetEvent(m_hEventThread);

	//关闭事件
	m_bRun=true;
	return true;
}

//停止服务
bool CTCPSocketManageForZ::Stop()
{
	if (this==NULL) return false;

	//关闭 SOCKET
	m_bRun=false;
	m_ListenSocket.CloseSocket();

	//退出应答线程
	if (m_hThreadAccept!=NULL) 
	{
		DWORD dwCode=::WaitForSingleObject(m_hThreadAccept,TIME_OUT);
		if (dwCode==WAIT_TIMEOUT) ::TerminateThread(m_hThreadAccept,1);
		::CloseHandle(m_hThreadAccept);
		m_hThreadAccept=NULL;
	}

	//关闭完成端口
	if (m_hCompletionPortRS!=NULL)
	{
		for (UINT i=0;i<m_uRSThreadCount;i++)
		{
			::PostQueuedCompletionStatus(m_hCompletionPortRS,0,NULL,NULL);
			::WaitForSingleObject(m_hEventThread,TIME_OUT);
			::ResetEvent(m_hEventThread);
		}
		m_uRSThreadCount=0;
		::CloseHandle(m_hCompletionPortRS);
		m_hCompletionPortRS=NULL;
	}

	//关闭维护线程
	if (m_hThreadKeep!=NULL)
	{
		if (::WaitForSingleObject(m_hEventThread,TIME_OUT)==WAIT_TIMEOUT) ::TerminateThread(m_hThreadKeep,0);
		::CloseHandle(m_hThreadKeep);
		m_hThreadKeep=NULL;
	}

	//关闭批量发送线程
	if (m_hCompletionPortBatch!=NULL)
	{
		::PostQueuedCompletionStatus(m_hCompletionPortBatch,0,NULL,NULL);
		::WaitForSingleObject(m_hEventThread,TIME_OUT);
		::ResetEvent(m_hEventThread);
		::CloseHandle(m_hCompletionPortBatch);
		m_hCompletionPortBatch=NULL;
	}

	//关闭 SOCKET
	for (UINT i=0;i<m_SocketPtrArray.GetPermitCount();i++) 
	{
		m_SocketPtrArray.GetArrayItem(i)->CloseSocket();
	}

	//关闭事件
	if (m_hEventThread!=NULL)
	{
		::CloseHandle(m_hEventThread);
		m_hEventThread=NULL;
	}

	return true;
}

//发送数据函数
int CTCPSocketManageForZ::SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if ((uIndex<m_SocketPtrArray.GetPermitCount())&&(m_bRun==true))
	{
		return m_SocketPtrArray.GetArrayItem(uIndex)->SendData(bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	return 0;
}

//发送数据函数
int CTCPSocketManageForZ::SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if ((uIndex<m_SocketPtrArray.GetPermitCount())&&(m_bRun==true))
	{
		return m_SocketPtrArray.GetArrayItem(uIndex)->SendData(pData,uBufLen,bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	return 0;
}

//发送数据函数
int CTCPSocketManageForZ::SendData(CTCPSocketForZ * pSocket, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID)
{
	if ((pSocket!=NULL)&&(m_bRun==true)) 
	{
		return pSocket->SendData(pData,uBufLen,bMainID,bAssistantID,bHandleCode,dwHandleID);
	}
	return 0;
}

//批量发送函数
int CTCPSocketManageForZ::SendDataBatch(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	//效验数据
	if ((m_bRun==false)||(uSize>(sizeof(m_SendLine[0].szSendData)-sizeof(NetMessageHead)))) return 0;

	CSignedLockObject BatchLock(&m_csBacthSend,true);
	if (m_uBatchLineCount>=uBatchLineSize) return 0;

	//写入缓冲
	UINT m_uWritePos=(m_uBatchCerrenPos+m_uBatchLineCount)%uBatchLineSize;
	m_SendLine[m_uWritePos].NetHead.bMainID=bMainID;
	m_SendLine[m_uWritePos].NetHead.bAssistantID=bAssistantID;
	m_SendLine[m_uWritePos].NetHead.bHandleCode=bHandleCode;
	m_SendLine[m_uWritePos].NetHead.uMessageSize=uSize+sizeof(NetMessageHead);
	m_SendLine[m_uWritePos].NetHead.bReserve=0;
	if (uSize>0) CopyMemory(m_SendLine[m_uWritePos].szSendData,pData,uSize);

	//激发发送线程
	m_uBatchLineCount++;
	PostQueuedCompletionStatus(m_hCompletionPortBatch,uSize+sizeof(NetMessageHead),NULL,NULL);

	return uSize;
}

//关闭连接
bool CTCPSocketManageForZ::CloseSocket(UINT uIndex, DWORD dwHandleID)
{ 
	if (uIndex<m_SocketPtrArray.GetPermitCount()) 
	{
		return CloseSocket(m_SocketPtrArray.GetArrayItem(uIndex),dwHandleID);
	}
	return false;
}

//关闭连接
bool CTCPSocketManageForZ::CloseSocket(CTCPSocketForZ * pSocket, DWORD dwHandleID) 
{ 
	if (pSocket!=NULL)
	{
		CSignedLockObject RecvLock(pSocket->GetRecvLock(),true);
		CSignedLockObject SendLock(pSocket->GetSendLock(),true);

		//效验数据
		if (dwHandleID==0) dwHandleID=pSocket->m_dwHandleID;
		if ((pSocket->m_dwHandleID==0)||(dwHandleID!=pSocket->m_dwHandleID)) return false;

		//设置数据
		ULONG dwAccessIP=pSocket->GetConnectData();
		long int lConnectTime=(long int)time(NULL)-pSocket->GetBeginTime();
		*(m_pKeep+pSocket->m_uIndex)=0;

		//关闭处理
		if ((pSocket->CloseSocket()==true)&&(m_pService!=NULL))	m_pService->OnSocketCloseEvent(dwAccessIP,pSocket->m_uIndex,lConnectTime);

		//清理信息
		TCPSocketDelete(pSocket);

		return true;
	}
	return false;
}

//判断是否连接
bool CTCPSocketManageForZ::IsConnectID(UINT uIndex, DWORD dwHandleID)
{
	if (uIndex>=m_SocketPtrArray.GetPermitCount()) return false;
	return m_SocketPtrArray.GetArrayItem(uIndex)->IsConnectID(dwHandleID);
}



//消息处理函数
bool CTCPSocketManageForZ::OnNetMessage(CTCPSocketForZ * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID)
{
	*(m_pKeep+uIndex)=0;
	//效验网络测试数据
	if ((pNetHead->bMainID==MDM_CONNECT)&&(pNetHead->bAssistantID==ASS_NET_TEST)) 
	{
		//*(m_pKeep+uIndex)=0; 以前这句是放这里的,但是应该是收到任何消息都应该把心跳清0
		return true;
	}
	if (m_pService!=NULL) return m_pService->OnSocketReadEvent(pSocket,pNetHead,pData,uSize,uIndex,dwHandleID);
	return false;
}


//SOCKET 数据接收线程
unsigned __stdcall CTCPSocketManageForZ::ThreadRSSocket(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct	* pStartData=(ThreadStartStruct *)pThreadData;			//线程启动数据指针
	CTCPSocketManageForZ	* pSocketManage=pStartData->pSocketManage;				//管理指针
	HANDLE				hCompletionPort=pStartData->hCompletionPort;			//完成端口
	HANDLE				hEvent=pStartData->hThreadEvent;						//事件

	//线程数据读取完成
	::SetEvent(hEvent);

	//重叠数据
	DWORD							dwThancferred=0;							//接收数量
	CTCPSocketForZ						* pTCPSocket=NULL;							//完成键
	CTCPSocketForZ::OverLappedStruct	* pOverData=NULL;							//重叠数据

	while (1)
	{
		try
		{
			//等待完成端口
			dwThancferred=0;
			pTCPSocket=NULL;
			pOverData=NULL;
			::GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPSocket,(LPOVERLAPPED *)&pOverData,INFINITE);
			if ((pTCPSocket==NULL)||(pOverData==NULL))
			{
				if ((pTCPSocket==NULL)&&(pOverData==NULL)) ::SetEvent(hEvent);
				_endthreadex(0);
			}

			//处理完成端口
			if ((dwThancferred==0)&&(pOverData->uOperationType==SOCKET_SND))
			{
				//SOCKET 关闭
				pSocketManage->CloseSocket(pTCPSocket,pOverData->dwHandleID);
				continue;
			}

			switch (pOverData->uOperationType)
			{
			case SOCKET_REV:	//SOCKET 数据读取
				{
					pTCPSocket->OnRecvCompleted(pOverData->dwHandleID);
					break;
				}
			case SOCKET_SND:	//SOCKET 数据发送
				{
					pTCPSocket->OnSendCompleted(dwThancferred,pOverData->dwHandleID);
					break;
				}
			}
		}
		catch (...) 
		{
			TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);
		}
	}

	return 0;
}

//SOCKET 批量发送数据线程
unsigned __stdcall CTCPSocketManageForZ::ThreadBatchSend(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct		* pStartData=(ThreadStartStruct *)pThreadData;					//线程启动数据指针
	CTCPSocketManageForZ		* pSocketManage=pStartData->pSocketManage;						//管理指针
	UINT					uMaxCount=pSocketManage->m_SocketPtrArray.GetPermitCount();		//最大数目
	HANDLE					hCompletionPort=pStartData->hCompletionPort;					//完成端口
	HANDLE					hEvent=pStartData->hThreadEvent;								//事件

	//启动完成
	SetEvent(pStartData->hThreadEvent);


	//临时变量
	NetMessageHead			* pNetHead=NULL;												//数据包头
	CTCPSocketForZ				* pSendSocket=pSocketManage->m_SocketPtrArray.GetArrayItem(0);	//发送 SOCKET
	UINT					* puSendCount=&pSocketManage->m_uBatchLineCount;				//当前数量

	DWORD					dwThancferred=0;												//接收数量
	ULONG					dwCompleteKey=0L;												//重叠 IO 临时数据
	LPOVERLAPPED			OverData;														//重叠 IO 临时数据

	while (1)
	{
		try
		{
			//提取发送数据
			dwThancferred=0;
			GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,&dwCompleteKey,(LPOVERLAPPED *)&OverData,INFINITE);
			if (dwThancferred==0)
			{
				SetEvent(hEvent);
				_endthreadex(0);
			}

			//发送数据
			BatchSendLineStruct * pBatchData=&pSocketManage->m_SendLine[pSocketManage->m_uBatchCerrenPos];
			for (UINT i=0;i<uMaxCount;i++) 
			{
				if ((pSendSocket+i)->IsConnect())
				{
					(pSendSocket+i)->SendData(pBatchData->szSendData,pBatchData->NetHead.uMessageSize-sizeof(NetMessageHead),
						pBatchData->NetHead.bMainID,pBatchData->NetHead.bAssistantID,pBatchData->NetHead.bHandleCode,0);
				}
			}

			//处理数据
			{
				CSignedLockObject BatchLock(&pSocketManage->m_csBacthSend,true);
				pSocketManage->m_uBatchCerrenPos=(pSocketManage->m_uBatchCerrenPos+1)%uBatchLineSize;
				pSocketManage->m_uBatchLineCount--;
			}
		}
		catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	}

	return 0;
}

//SOCKET 维护线程
unsigned __stdcall CTCPSocketManageForZ::ThreadKeepActive(LPVOID pThreadData)
{
	//数据定义
	ThreadStartStruct		* pStartData=(ThreadStartStruct *)pThreadData;		//启动指针
	CTCPSocketManageForZ		* pSocketManage=pStartData->pSocketManage;			//管理指针
	HANDLE					m_hThreadEvent=pStartData->hThreadEvent;			//线程事件

	//启动完成
	::SetEvent(m_hThreadEvent);

	//数据定义
	UINT uCheckCount=0;
	::Sleep(pSocketManage->m_uCheckTime);

	while (1)
	{
		//发送测试数据
		pSocketManage->SendDataBatch(NULL,0,MDM_CONNECT,ASS_NET_TEST,0);

		//检查退出标志
		uCheckCount=0;
		while (uCheckCount<pSocketManage->m_uCheckTime)
		{
			if (!pSocketManage->m_bRun) 
			{
				SetEvent(m_hThreadEvent);
				_endthreadex(0);
			}
			::Sleep(1000);
			uCheckCount+=1000;
		}

		//效验连接数据
		if ((pSocketManage->m_bCheckConnect)&&(pSocketManage->m_bRun)&&(pSocketManage->m_SocketPtrArray.GetArrayItem(0)!=NULL))
		{
			for (int i=(pSocketManage->m_SocketPtrArray.GetPermitCount()-1);i>=0;i--)
			{
				if (pSocketManage->m_SocketPtrArray.GetArrayItem(i)->IsConnect())
				{
					if ((*(pSocketManage->m_pKeep+i))>=3) //如果超出3次计数，则断开该客户连接
						pSocketManage->CloseSocket(i,0);
					else 
						(*(pSocketManage->m_pKeep+i))++;//小于3次读数，则给读数器+1
				}
			}
		}
	}

	return 0;
}

//获取时间函数
unsigned __int64 CTCPSocketManageForZ::AFCGetTimer()
{
	_asm _emit 0x0F;
	_asm _emit 0x31;
}

//获取处理 ID 号码
DWORD CTCPSocketManageForZ::GetHandleID()
{
	static long int dwBaseID=1L;
	if (dwBaseID>3000000L) dwBaseID=1L;
	return ::InterlockedIncrement(&dwBaseID);
}

//获取本机 IP 地址
bool CTCPSocketManageForZ::GetLoaclIPAddr(TCHAR szAddrBuffer[], UINT uStation)
{
	if (szAddrBuffer==NULL) return false;

	TCHAR szHostName[51];
	::gethostname(szHostName,sizeof(szHostName));
	HOSTENT * pHostInfo=::gethostbyname(szHostName);
	if (pHostInfo!=NULL)
	{
		LPIN_ADDR * pAddrList=(LPIN_ADDR *)(pHostInfo->h_addr_list); 
		LPIN_ADDR In_Addr=*pAddrList;

		UINT uCount=0;
		szAddrBuffer[0]=0;
		while (In_Addr!=NULL)
		{
			if (uCount==uStation)
			{
				lstrcpy(szAddrBuffer,inet_ntoa(*In_Addr));
				return true;
			}
			uCount++;
			pAddrList++;
			In_Addr=*pAddrList;
		}
	}
	return false;
}

//数值变 IP 字符
TCHAR * CTCPSocketManageForZ::ULongToIP(ULONG dwIP, TCHAR * szIP)
{
	wsprintf(szIP,TEXT("%d.%d.%d.%d"),FOURTH_IPADDRESS(dwIP),THIRD_IPADDRESS(dwIP),SECOND_IPADDRESS(dwIP),FIRST_IPADDRESS(dwIP));
	return szIP;
}
