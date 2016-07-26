#pragma once

#include <Windows.h>

#ifdef SOCKET_EXPORTS
#define SOCKET_API __declspec(dllexport)
#else
#define SOCKET_API __declspec(dllimport)
#endif //SOCKET_EXPORTS




typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed __int64      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;


const int MAX_MESSAGE_SIZE = 100000;					///< 单条消息最大长度
/// 表明事件的类型
const UINT16 SOCKET_CONNECT			 = 0;
const UINT16 SOCKET_RECEIVE			 = 1;
const UINT16 SOCKET_CLOSE				 = 2;
const UINT16 SOCKET_CONNECT_FAILED	 = 3;
const UINT16 SOCKET_RECEIVE_LESSDATA = 4;
/// 用来为外部读取数据提供的参数结构
struct RECV_DATA_T 
{
	UINT16 wSocketID;		///< 哪个Socket的数据，ID号
	UINT16 wType;			///< 消息的类型，取值为三种：SOCKET_CONNECT, SOCKET_RECEIVE, SOCKET_CLOSE
	UINT8 *pData;			///< 数据缓冲区指针，须由外部确保该指针的有效性和足够的空间
	UINT32 uDataLen;		///< 缓冲区数据长度，作为参数传入时，指示缓冲区长度，作为参数传出时，指示数据实际长度
};

interface ISocket;
interface IServerSocket;
interface IServerSocketService;



//客户端网络服务接口
interface IClientSocketService__
{
	//接口函数 
public:
	//网络读取消息
	virtual bool OnSocketReadEvent(SOCKET iSocket, void * pNetData, UINT uDataSize)=0;
	//网络连接消息
	virtual bool OnSocketConnectEvent(SOCKET iSocket, UINT uErrorCode)=0;
	//网络关闭消息
	virtual bool OnSocketCloseEvent(SOCKET iSocket)=0;
};

//客户端网络服务接口
interface IServerSocketService
{
	//接口函数 
public:
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnSocketAccept(SOCKET iSocket) = 0;
	//网络读取消息
	virtual bool OnSocketReadEvent(SOCKET iSocket, void * pNetData, UINT uDataSize)=0;
	//网络关闭消息
	virtual bool OnSocketCloseEvent(SOCKET iSocket)=0;
};


/// XSocket提供的供初始化参数的基类
/// 
class XSocketBase
{
public:
	XSocketBase(void){}
	virtual ~XSocketBase(void){}
public:
	/// 初始化Socket，须传入一个事件句柄
	virtual INT32 InitialNew(IClientSocketService* pSocketService)=0;
	/// 初始化Socket，须传入一个事件句柄
	virtual INT32 Initial(HANDLE hEvent)=0;
	/// 收到事件句柄后，调用该函数获取读到的信息
	virtual INT32 RecvData(RECV_DATA_T &rd)=0;
};


class XServerSocketBase
{
public:
	XServerSocketBase(void){}
	virtual ~XServerSocketBase(void){}
public:
	/// 初始化Socket，须传入一个事件句柄
	virtual INT32 Initial(IServerSocketService* pSocketService)=0;
};

extern SOCKET_API bool CreateSocket(XSocketBase **pSocketBase, ISocket **pISocket);
extern SOCKET_API bool CreateServerSocket(XServerSocketBase** pSocketBase, IServerSocket **pIServerSocket);
