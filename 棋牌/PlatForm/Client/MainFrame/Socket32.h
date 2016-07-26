#pragma once


#include <WinSock2.h>
typedef int socklen_t ;
#pragma comment(lib,"ws2_32.lib")

//
//#ifdef _WIN32
//#include <WinSock2.h>
//typedef int socklen_t ;
//#else
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//
//#include <unistd.h>
//#include <errno.h>
//#include <sys/stat.h>
//#include <string.h>
//#include <netdb.h>
//#ifndef CONST
//#define CONST               const
//#endif
//
//typedef unsigned char   u_char;
//typedef unsigned short  u_short;
//typedef unsigned int    u_int;
//typedef unsigned long   u_long;
//typedef u_int           SOCKET;
//
//typedef int                 INT;
//typedef unsigned int        UINT;
//typedef unsigned int        *PUINT;
//typedef unsigned long       DWORD;
//typedef int                 BOOL;
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;
//typedef float               FLOAT;
//typedef FLOAT               *PFLOAT;
//typedef BOOL            *PBOOL;
//typedef BOOL             *LPBOOL;
//typedef BYTE            *PBYTE;
//typedef BYTE             *LPBYTE;
//typedef int             *PINT;
//typedef int              *LPINT;
//typedef WORD            *PWORD;
//typedef WORD             *LPWORD;
//typedef long             *LPLONG;
//typedef DWORD           *PDWORD;
//typedef DWORD            *LPDWORD;
//typedef void             *LPVOID;
//typedef CONST void       *LPCVOID;
//
//#ifndef _TCHAR_DEFINED
//typedef char TCHAR, *PTCHAR;
//typedef unsigned char TBYTE , *PTBYTE ;
//#define _TCHAR_DEFINED
//#endif /* !_TCHAR_DEFINED */
//
//#ifndef VOID
//#define VOID void
//typedef char CHAR;
//typedef short SHORT;
//typedef long LONG;
//#endif
//typedef CHAR *PCHAR;
//typedef CHAR *LPCH, *PCH;
//
//typedef CONST CHAR *LPCCH, *PCCH;
//typedef CHAR *NPSTR;
//typedef CHAR *LPSTR, *PSTR;
//typedef CONST CHAR *LPCSTR, *PCSTR;
//typedef LPSTR LPTCH, PTCH;
//typedef LPSTR PTSTR, LPTSTR;
//typedef LPCSTR LPCTSTR;
//
//
//#ifndef FALSE
//#define FALSE               0
//#endif
//
//#ifndef TRUE
//#define TRUE                1
//#endif
//
//#define INVALID_SOCKET  (SOCKET)(~0)
//#define SOCKET_ERROR            (-1)
//typedef struct in_addr  *LPIN_ADDR;
//
//#define closesocket(x) close(x)
//
//
//#endif

//简单的socket类 回调接口  2012.08.23 yyf
interface ISimpleSocket
{ 
	virtual  void ReadSimpleSocket(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead) = 0;//回调接口
};

//简单的socket类，用于本机 进程通信.  2012.08.23 yyf
class CSocket32
{
	SOCKET m_hSocket;
	SOCKET m_hSocketServer;

public:
	BOOL Create( UINT nSocketPort = 0, int nSocketType = SOCK_STREAM , LPCTSTR lpszSocketAddress = NULL ,bool bisServer = true);
	int SendTo( const void* lpBuf, int nBufLen, UINT nHostPort, LPCSTR lpszHostAddress = NULL, int nFlags = 0 );
	int ReceiveFrom( void* lpBuf, int nBufLen, char *rSocketAddress, UINT& rSocketPort, int nFlags = 0 );

	BOOL Listen( int nConnectionBacklog = 5 )
	{
		return !listen(m_hSocket,nConnectionBacklog);
	}
	int Send( const void* lpBuf, int nBufLen, int nFlags = 0 )
	{
		return send(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
	}
	int Receive( void* lpBuf, int nBufLen, int nFlags = 0 )
	{
		return recv(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
	}    
	void Close( )
	{
		bSimpleSocketThread = false; 
		closesocket(m_hSocketServer);
		closesocket(m_hSocket);
	}

	BOOL GetSockName( char*  rSocketAddress, UINT& rSocketPort );
	BOOL GetPeerName(  char* rPeerAddress, UINT& rPeerPort );
	BOOL Accept( CSocket32& rConnectedSocket,  LPSTR lpSockAddr = NULL,UINT *nPort = NULL);
	BOOL Connect( LPCTSTR lpszHostAddress, UINT nHostPort );

	void ReadSimpleSocket(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

	void SetSimpleSocket(ISimpleSocket * pSimpleSocket);//初始化被回调的接口指针 
	// //打开一个进程 2012.08.17 yyf
	bool OpenProcess(LPCSTR lpApplicationName);
	// //设置客户端  2012.08.17 yyf
	int SetClientSimpleSocket(void); 
	// 开始线程
	void StartThread(void);
	DWORD GetProcessIdFromName(LPCTSTR name);//如果有运行，返回进程的PID，没运行返回0 
	PROCESS_INFORMATION GetProcessInfo() const { return pi; } 
	ISimpleSocket *pSimpleSocket;
	LPCTSTR strProcessName;//进程名字
	bool    bSimpleSocketThread;//控制线程开工吧
	PROCESS_INFORMATION pi;
	//查找空闲网络端口
	void FindFreePort();

	ULONG GetFreePort();
public:
	CSocket32(void);
public:
	virtual ~CSocket32(void);
public:
	bool InitClientSimpleSocket(ISimpleSocket * pSimpleSocket);//初始化客户端.

private:
	sockaddr_in m_addr;

	void SetFreePortToBcf();
};
