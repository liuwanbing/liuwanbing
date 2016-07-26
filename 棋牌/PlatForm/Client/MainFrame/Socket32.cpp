
#include "stdafx.h" 
#include "Socket32.h"
#include <assert.h>
#include <Tlhelp32.h>// DWORD GetProcessIdFromName(LPCTSTR name) 2012.08.16 yyf

// 匿名管道线程 不停的读信息.
DWORD WINAPI  SimpleSocketThread(LPVOID param)
{
	CSocket32 * SimpleSocket = (CSocket32 *)param;

	BYTE BYTE_Read[10240] ;
	while(NULL != SimpleSocket && SimpleSocket->bSimpleSocketThread)
	{  
		int nRead; 
		memset(&BYTE_Read,0,10240);
		nRead = SimpleSocket->Receive((void*)BYTE_Read,10240);
		if( nRead >0 )
		{ 
			SimpleSocket->ReadSimpleSocket((LPVOID)BYTE_Read,nRead);
		} 
		else
		{ 
			OutputDebugString("yyf: SimpleSocket->Receive(..) 错误，可能断线.");
			
			Sleep(500);

		} 
	} 
	return 0;
}
/**///////////////////////////////////////////////////////////////////////
// Construction/Destruction
/**///////////////////////////////////////////////////////////////////////
//构造
CSocket32::CSocket32()
{
//#ifdef _WIN32
	WSAData wsaData;
	int err =WSAStartup(0x0202,&wsaData);
	if ( err != 0 )                           
		return;
	if ( LOBYTE( wsaData.wVersion ) != 2 ||HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		return; 
	}
//#endif
	m_hSocket = INVALID_SOCKET;

	bSimpleSocketThread = false; 
}
//析构
CSocket32::~CSocket32()
{
	Close();
}
/**//**********************************************************************/
//1.创建套接字
BOOL CSocket32::Create( UINT nSocketPort, int nSocketType ,LPCTSTR lpszSocketAddress,bool bisServer )
{
	m_hSocket = socket(AF_INET,nSocketType,IPPROTO_IP);//然后再创建
	if(    m_hSocket == INVALID_SOCKET)
		return FALSE;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nSocketPort);

	if (!lpszSocketAddress) //如果用户不指定，则系统分配
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		addr.sin_addr.s_addr = inet_addr(lpszSocketAddress);

	if(!bisServer)
	{
        return TRUE;
	}
	if (!bind(m_hSocket,(sockaddr*)&addr,sizeof(addr)))
	{
		if (0 == nSocketPort)
			m_addr = addr;
		return TRUE;//If no error occurs, bind returns zero
	}
	Close();

	return FALSE;
}

//2.发送（针对UDP）
int CSocket32::SendTo( const void* lpBuf, int nBufLen, UINT nHostPort, LPCSTR lpszHostAddress , int nFlags)
{
	sockaddr_in addr = {AF_INET,htons(nHostPort),};
	assert(lpszHostAddress);//UDP 必须指定目的地址
	addr.sin_addr.s_addr = inet_addr(lpszHostAddress);
	return sendto(m_hSocket,(char*)lpBuf,nBufLen,nFlags,(sockaddr*)&addr,sizeof(addr));
}

//3.接收（针对UDP）
int CSocket32::ReceiveFrom( void* lpBuf, int nBufLen, char *rSocketAddress, UINT& rSocketPort, int nFlags )
{
	sockaddr_in from;//必须设一个临时变量，分配内存，接收信息。
	socklen_t fromlen = sizeof(from);//然后才能计算内存长度

	int nRet = recvfrom(m_hSocket,(LPSTR)lpBuf,nBufLen,nFlags,(sockaddr*)&from,&fromlen);
	if(nRet <= 0)
		return nRet;
	if(rSocketAddress)
	{
		strcpy(rSocketAddress,inet_ntoa(from.sin_addr));//out
		rSocketPort = htons(from.sin_port);//out
	}

	return nRet;
}

//4.接受请求(针对TCP)
BOOL CSocket32::Accept( CSocket32& rConnectedSocket, LPSTR lpSockAddr ,UINT *nPort )
{
	sockaddr_in addr = {AF_INET};
	socklen_t nLen = sizeof(addr);
	rConnectedSocket.m_hSocket = accept(m_hSocket,(sockaddr*)&addr,&nLen);
	if(rConnectedSocket.m_hSocket == INVALID_SOCKET)
		return FALSE;
	if(lpSockAddr)
	{
		strcpy(lpSockAddr,inet_ntoa(addr.sin_addr));
		*nPort = htons(addr.sin_port);
	}
	return TRUE;    
}

//5.请求连接(针对TCP)
BOOL CSocket32::Connect( LPCTSTR lpszHostAddress, UINT nHostPort )
{
	sockaddr_in addr = {AF_INET,htons(nHostPort)};
	addr.sin_addr.s_addr = inet_addr(lpszHostAddress);
	if (addr.sin_addr.s_addr == INADDR_NONE)//若是有限广播地址，则用本机地址之一
	{
		hostent * lphost = gethostbyname(lpszHostAddress);
		if (lphost != NULL)
			addr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			//WSAGetLastError();
			//    WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}
	return !connect(m_hSocket,(sockaddr*)&addr,sizeof(addr));
}

//6.得到本机套接字：IP：Port
BOOL CSocket32::GetSockName( char*  rSocketAddress, UINT& rSocketPort )
{
	sockaddr_in addr;
	socklen_t nLen = sizeof(addr);

	if(SOCKET_ERROR == getsockname(m_hSocket, (sockaddr*)&addr, &nLen))
		return FALSE;
	if(rSocketAddress)
	{
		strcpy(rSocketAddress,inet_ntoa(addr.sin_addr));
		rSocketPort = htons(addr.sin_port);
	}
	return TRUE;
}

//7.得到对方套接字名字：IP：Port
BOOL CSocket32::GetPeerName(  char* rPeerAddress, UINT& rPeerPort )
{
	sockaddr_in addr;
	socklen_t nLen = sizeof(addr);

	if(SOCKET_ERROR == getpeername(m_hSocket, (sockaddr*)&addr, &nLen))
		return FALSE;
	if(rPeerAddress)
	{
		strcpy(rPeerAddress,inet_ntoa(addr.sin_addr));
		rPeerPort = htons(addr.sin_port);
	}
	return TRUE;
}
void CSocket32::ReadSimpleSocket(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead)
{ 
	if(NULL != pSimpleSocket )
	{//调用回调
		pSimpleSocket->ReadSimpleSocket(lpBuffer, dwNumberOfBytesToRead);
	}  
}


void CSocket32::SetSimpleSocket(ISimpleSocket * pSimpleSocket)//初始化被回调的接口指针  2012.08.17 yyf
{
	this->pSimpleSocket = pSimpleSocket;
}
// //打开一个进程 2012.08.17 yyf
bool CSocket32:: OpenProcess(LPCSTR lpApplicationName)
{
	strProcessName = lpApplicationName;//进程名字
	if(0 == GetProcessIdFromName(strProcessName) && NULL != pSimpleSocket && !bSimpleSocketThread)
	{//进程没开始

		bSimpleSocketThread = false; 
		STARTUPINFO sui;
		//PROCESS_INFORMATION pi;
		ZeroMemory(&sui,sizeof(STARTUPINFO));
		sui.cb=sizeof(STARTUPINFO);
		sui.dwFlags=STARTF_USESTDHANDLES; 
		sui.hStdError=GetStdHandle(STD_ERROR_HANDLE);

		if(!CreateProcess(strProcessName,NULL,NULL,NULL,
			TRUE,0,NULL,NULL,&sui,&pi))
		{
		 
			//MessageBox("创建子进程失败！");
			return false;
		}

		FindFreePort();
		SetFreePortToBcf();

		if (0 == m_addr.sin_port)
			return false;

	    //创建服务器
		if (Create(m_addr.sin_port))
		{
			if (Listen())
			{
				CString strPort;
				strPort.Format("rende123: port no: %d",m_addr.sin_port);
				OutputDebugString(strPort);

				SOCKADDR_IN addrClient;
				int len = sizeof(SOCKADDR);

				//等待客户请求到来
				m_hSocketServer = m_hSocket;
				m_hSocket = accept(m_hSocketServer, (SOCKADDR*)&addrClient, &len);//sockConn建立连接的套接字
			} 
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		//创建匿名管道线程，开始读
		bSimpleSocketThread = true; 
		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(0,0,SimpleSocketThread,this,0,&dwThreadID);
		if (hThread)
		{
			OutputDebugString("yyf: 匿名管道线程开始.");
			::CloseHandle(hThread);
		} 

		return true;
	}
	return true;
}
//保存
void CSocket32::SetFreePortToBcf()
{
	DWORD dwCfgFile = ::cfgOpenFile(CBcfFile::GetAppPath() + "bzgame.bcf");

	//bcf文件打开失败返回
	if(dwCfgFile < 0x10) 
		return;

	::cfgSetValue(dwCfgFile,_T("GameSet"),_T("UpdateSetPort"),m_addr.sin_port);
	::cfgSetKeyMemo(dwCfgFile,_T("GameSet"),_T("UpdateSetPort"),_T("设置下载更新端口"));

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);

}
//获取端口
ULONG CSocket32::GetFreePort()
{
	DWORD dwCfgFile = ::cfgOpenFile(CBcfFile::GetAppPath() + "bzgame.bcf");

	//bcf文件打开失败返回
	if(dwCfgFile < 0x10) 
		return 0;

	ULONG ulRet = ::cfgGetValue(dwCfgFile,_T("GameSet"),_T("UpdateSetPort"),6123);

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);

	return ulRet;
}

//查找空闲网络端口
void CSocket32::FindFreePort()
{
	if (Create())
	{
		int len = sizeof(m_addr);
		if(0 == getsockname(m_hSocket, (struct sockaddr *)&m_addr, &len))
		{
			closesocket(m_hSocket);
		}
	}
}
// //设置客户端  2012.08.17 yyf
int CSocket32:: SetClientSimpleSocket(void)
{
	//创建套接字
	m_hSocket = socket( AF_INET,SOCK_STREAM, 0 );
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(m_addr.sin_port);

	CString strPortNo;
	strPortNo.Format("rende123: client port no: %d", m_addr.sin_port);
	OutputDebugString(strPortNo);

	//向服务器发出连接请求
	connect( m_hSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	return 0;
}
// 开始线程
void CSocket32:: StartThread(void)
{
	bSimpleSocketThread = true;//控制线程开工吧
 
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(0,0,SimpleSocketThread,this,0,&dwThreadID);
	if (hThread)
	{ 
		::CloseHandle(hThread);
	}
	else
	{
		OutputDebugString("yyf: update.exe  线程失败.");
	}
}
//2012.08.16 yyf 返回进程的PID
DWORD CSocket32::GetProcessIdFromName(LPCTSTR name) //如果有运行，返回进程的PID，没运行返回0 //自己可以修改函数，返回运行中的进程的个数
{ 
	PROCESSENTRY32 pe; 
	DWORD id = 0; 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	pe.dwSize = sizeof(PROCESSENTRY32); 
	if( !Process32First(hSnapshot,&pe) ) 
	{
		return 0; 
	}

	while(1) 
	{ 
		pe.dwSize = sizeof(PROCESSENTRY32); 
		if( Process32Next(hSnapshot,&pe)==FALSE ) 
			break; 
		
		if(_stricmp(pe.szExeFile,name) == 0) //忽略大小写字母比较
		{ 
			id = pe.th32ProcessID; 
			break; 
		} 

	}
	CloseHandle(hSnapshot); 
	return id; 
} 
//初始化客户端.
bool CSocket32::InitClientSimpleSocket(ISimpleSocket * pSimpleSocket)
{
	SetSimpleSocket(pSimpleSocket);//初始化   
	if (0 == SetClientSimpleSocket())
	{
		StartThread();
		return true;
	}
	
	return false;
}
