#ifndef AFCSOCLET_H_FILE_2003_07
#define AFCSOCLET_H_FILE_2003_07

#include <string>
using namespace std;

#include <AfxSock.h>
#include "AFCLock.h"
#include "AFCPtrArray.h"
#include "AFCInterface.h"
#include "../common/BaseMessage.h"



/**************************************************************************************************************/

// UDP SOCKET 网络部分

/**************************************************************************************************************/

//UDP 网络 SOCKET 类
class AFX_EXT_CLASS CUDPSocket
{
	//变量定义
protected:
	SOCKET						m_hSocket;							//SOCKET 句柄

	//函数定义
public:
	//构造函数
	CUDPSocket(void);
	//析构函数
	virtual ~CUDPSocket(void);
	//发送函数
	inline int	SendData(void * pData, UINT uSize, TCHAR szSendIP[]);
	//接收函数
	inline int RecvData();
};

/**************************************************************************************************************/

// TCP SOCKET 网络部分 

/**************************************************************************************************************/

//类说明
class CTCPSocket;
class CTCPSocketManage;

//缓存定义
#define SED_SIZE				/*4*1024*/ 50*1024 //hansen 2014-12-27				//缓冲区大小
#define RCV_SIZE				/*4*1024*/ 50*1024 //hansne 2014-12-27				//缓冲区大小
//缓存定义用于Z服务器，是W的10分之1
#define SED_SIZE_Z				50000							//缓冲区大小
#define RCV_SIZE_Z				50000							//缓冲区大小

//常量定义
const UINT uBatchLineSize=180;

/**************************************************************************************************************/

#define NO_CONNECT				0								//没有连接
#define CONNECTING				1								//正在连接
#define CONNECTED				2								//成功连接

//客户端 SOKET
class AFX_EXT_CLASS CTCPClientSocket : public CWnd
{
	DECLARE_MESSAGE_MAP()

	//变量定义
private:
	int							m_iReadBufLen;					//缓存区长度
	BYTE						m_bConnectState;				//连接状态
	SOCKET						m_hSocket;						//SOCKET 句柄
	IClientSocketService		* m_pIService;					//处理接口
	BYTE						m_szBuffer[RCV_SIZE];			//数据缓存区

	int									m_iCheckCode;

	//函数定义
public:
	//构造函数
	explicit CTCPClientSocket(IClientSocketService * pIService);
	//析构函数
	virtual ~CTCPClientSocket();
	//初始化 SOCKET 窗口
	inline bool InitSocketWnd();
	//是否已经连接
	inline BYTE GetSocketState() { return m_bConnectState; }
	//关闭 SOCKET
	inline bool CloseSocket(bool bNotify=true);
	//连接服务器
	inline bool Connect(const TCHAR * szServerIP, UINT uPort);
	//连接服务器
	inline bool Connect(long int dwServerIP, UINT uPort);
	//发送函数
	inline int SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//简单命令发送函数
	inline int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	//设置校验码，密文与密钥
	void SetCheckCode(__int64 iCheckCode, int isecretkey);
	//内部函数
private:
	//关闭 SOCKET 窗口
	inline bool CloseSocketWnd();
	//SOCKET 消息处理程序
	inline LRESULT	OnSocketNotifyMesage(WPARAM wParam, LPARAM lParam);

};

//服务器 SOCKET
class AFX_EXT_CLASS CListenSocket
{
	//变量定义
protected:
	SOCKET						m_hSocket;							//SOCKET 句柄

	//函数定义
public:
	//构造函数
	CListenSocket(void);
	//析构函数
	virtual ~CListenSocket(void);

	//功能函数
public:
	//建立 SOCKET
	inline bool Create();
	//监听网络
	inline bool Listen();
	//绑定地址
	inline bool Bind(UINT uPort);
	//关闭 SOCKET
	inline bool CloseSocket();
	//获取 SOCKET 句柄
	inline SOCKET GetSafeSocket() { return m_hSocket; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Modified by zxd 20090804
// 为了支持Z服务器最大人数突破1W，须改小缓冲区，但同时不能影响W服务器，
// 因此新建一套平行的Socket类供Z使用，保留原有类供W使用
// 以下是原有的类供W使用

//TCP SOCKET 类
class AFX_EXT_CLASS CTCPSocket
{
	friend class CTCPSocketManage;

	//结构定义
	struct OverLappedStruct		//SOCKET 重叠 IO 结构
	{
		//变量定义
		OVERLAPPED				OverLapped;							//重叠结构
		WSABUF					WSABuffer;							//数据缓冲
		UINT					uOperationType;						//操作类型
		DWORD					dwHandleID;							//处理 ID 号码
	};

	//变量定义
protected:
	UINT						m_uIndex;							//SOCKET 索引
	DWORD						m_dwHandleID;						//处理标识
	char						m_szSendBuf[SED_SIZE];				//发送数据缓冲区
	char						m_szRecvBuf[RCV_SIZE];				//数据接收缓冲区
	long int					m_lBeginTime;						//连接时间
	SOCKET						m_hSocket;							//SOCKET 句柄
	CTCPSocketManage			* m_pManage;						//SOCKET 管理类指针
	struct sockaddr_in			m_SocketAddr;						//地址信息

	//内部数据
private:
	bool						m_bSending;							//是否发送中
	DWORD						m_dwSendBuffLen;					//发送缓冲区长度
	DWORD						m_dwRecvBuffLen;					//接收缓冲区长度
	CAFCSignedLock				m_csSendLock;						//读取同步锁
	CAFCSignedLock				m_csRecvLock;						//接受同步锁
	OverLappedStruct			m_SendOverData;						//发送数据重叠结构
	OverLappedStruct			m_RecvOverData;						//接收数据重叠结构

	//函数定义
public:
	//构造函数
	CTCPSocket(void);
	//析够函数
	virtual ~CTCPSocket(void);

	//功能函数
public:
	//获取处理标志
	inline DWORD GetHandleID() { return m_dwHandleID; }
	//合并 SOCKET
	inline bool Attach(SOCKET hSocket, sockaddr_in & Addr, DWORD dwHandleID);
	//发送数据函数
    inline int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
	inline int SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);

	//功能函数
public:
	//判断是否连接
	inline bool IsConnect() { return (m_hSocket!=INVALID_SOCKET); }
	//获取连接时间
	inline long int GetBeginTime() { return m_lBeginTime; };
	//获取连接信息
	inline ULONG GetConnectData() { return m_SocketAddr.sin_addr.S_un.S_addr; }
	//获取接收锁
	 inline  CAFCSignedLock * GetRecvLock() { return &m_csRecvLock; }
	//获取发送锁锁
	 inline  CAFCSignedLock * GetSendLock() { return &m_csSendLock; }
	//判断是否连接
	inline bool IsConnectID(DWORD dwHandleID);

	//辅助函数
protected:
	//设置 ID 索引号码
	inline void SetIndex(UINT uIndex) { m_uIndex=uIndex; };
	//设置管理类指针
	inline void SetSocketManagePtr(CTCPSocketManage * pManage) { m_pManage=pManage; };
	//发送完成函数
	inline bool OnSendCompleted(DWORD dwThancferred, DWORD dwHandleID);
	//接收完成函数
	inline bool OnRecvCompleted(DWORD dwHandleID);
	//检测发送数据函数
	inline bool CheckSendData();
	//投递接收 IO
	inline bool RecvData();
	//关闭 SOCKET
	inline bool CloseSocket();
};

/**************************************************************************************************************/

//TCP 网络 SOCKET 管理类
class AFX_EXT_CLASS CTCPSocketManage
{
	///int m_CurConnectCount;
	//结构定义
	struct BatchSendLineStruct	//批量发送线程队列结构
	{
		//变量定义
		NetMessageHead			NetHead;						//数据包头
		char					szSendData[MAX_SEND_SIZE];		//发送缓存
	};

	struct ThreadStartStruct	//线程启动参数结构
	{
		//变量定义
		HANDLE					hCompletionPort;				//完成端口
		HANDLE					hThreadEvent;					//线程事件
		CTCPSocketManage		* pSocketManage;				//SOCKET 管理指针
	};

	//内核变量
protected:
	bool						m_bInit;						//初始化标志
	volatile bool				m_bRun;							//工作标志
	CListenSocket				m_ListenSocket;					//监听 SOCKET
	BYTE						* m_pKeep;						//保持连接数据
	IServerSocketService		* m_pService;					//服务接口
	CAFCPtrArray<CTCPSocket>	m_SocketPtrArray;				//TCP SOCKET 数组

	//设置参数
protected:
	bool						m_bCheckConnect;				//是否检测断线
	BYTE						m_bMaxVer;						//最新版本
	BYTE						m_bLessVer;						//最低版本
	UINT						m_uCheckTime;					//检测断线间隔
	UINT						m_uListenPort;					//监听断口

public:
	UINT						m_iSecretKey;                   //加密校验码的密钥

	//批发数据
protected:
	UINT						m_uBatchCerrenPos;				//批量发送点
	UINT						m_uBatchLineCount;				//发送队列数目
	CAFCSignedLock				m_csBacthSend;					//批发同步锁
	BatchSendLineStruct			m_SendLine[uBatchLineSize];		//批量发送队列

	//完成端口变量
private:
	UINT						m_uRSThreadCount;				//读写线程数目
	HANDLE						m_hEventThread;					//批量发送事件
	HANDLE						m_hCompletionPortBatch;			//批量完成端口
	HANDLE						m_hCompletionPortRS;			//读写完成端口
	HANDLE						m_hThreadKeep;					//SOCKET 维护线程
	HANDLE						m_hThreadAccept;				//应答线程

	//函数定义
public:
	//构造函数
	CTCPSocketManage();
	CTCPSocketManage(CTCPSocketManage&);
	CTCPSocketManage& operator=(CTCPSocketManage&); 
	//析构函数
	virtual ~CTCPSocketManage();

	//服务函数
public:
	//初始化
	virtual bool Init(UINT uMaxCount, UINT uListenPort, BYTE bMaxVer, BYTE bLessVer, int iSecretKey, IServerSocketService * pService);
	//取消初始化
	virtual bool UnInit();
	//开始服务
	virtual bool Start(UINT uAcceptThreadCount, UINT uSocketThradCount);
	//停止服务
	virtual bool Stop();
	//消息处理函数
	virtual bool OnNetMessage(CTCPSocket * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);

	//功能函数
public:
	//发送数据函数
    inline int SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
    inline int SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
    inline int SendData(CTCPSocket * pSocket, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//批量发送函数
	inline int SendDataBatch(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//关闭连接
	inline bool CloseSocket(UINT uIndex, DWORD dwHandleID);
	//关闭连接
	inline bool CloseSocket(CTCPSocket * pSocket, DWORD dwHandleID);
	//判断是否连接
	inline bool IsConnectID(UINT uIndex, DWORD dwHandleID);
	//数值变 IP 字符
	static TCHAR * ULongToIP(ULONG dwIP, TCHAR * szIP);
	//获取本机 IP 地址
	inline static bool GetLoaclIPAddr(TCHAR szAddrBuffer[], UINT uStation);
	//获取指定index的连接信息
	string GetServerInfo(UINT uIndex);

	//内核函数
private:
	//生成空闲 SOCKET
	inline CTCPSocket * TCPSocketNew();
	//释放活动 SOCKET
	inline bool TCPSocketDelete(CTCPSocket * pSocket);
	//SOCKET 连接应答线程
	inline static unsigned __stdcall ThreadAccept(LPVOID pThreadData);
	//SOCKET 数据接收线程
	inline static unsigned __stdcall ThreadRSSocket(LPVOID pThreadData);
	//SOCKET 批量发送数据线程
	inline static unsigned __stdcall ThreadBatchSend(LPVOID pThreadData);
	//SOCKET 维护线程
	inline static unsigned __stdcall ThreadKeepActive(LPVOID pThreadData);
	//获取时间函数
	inline static unsigned __int64 AFCGetTimer();
	//获取处理 ID 号码
	inline static DWORD GetHandleID();
};

// Modified by zxd 20090804
// 为了支持Z服务器最大人数突破1W，须改小缓冲区，但同时不能影响W服务器，
// 因此新建一套平行的Socket类供Z使用，保留原有类供W使用
/////////////////////////////////////////////////////////////////////////////////////////////
// 以上是原有的类供W使用
//---------------------------------------------------------------------------------------------------
// 以下是新建的类供Z使用

/////////////////////////////////////////////////////////////////////////////////////////////
// Modified by zxd 20090804
// 为了支持Z服务器最大人数突破1W，须改小缓冲区，但同时不能影响W服务器，
// 因此新建一套平行的Socket类供Z使用，保留原有类供W使用

//TCP SOCKET 类
class AFX_EXT_CLASS CTCPSocketForZ
{
	friend class CTCPSocketManageForZ;

	//结构定义
	struct OverLappedStruct		//SOCKET 重叠 IO 结构
	{
		//变量定义
		OVERLAPPED				OverLapped;							//重叠结构
		WSABUF					WSABuffer;							//数据缓冲
		UINT					uOperationType;						//操作类型
		DWORD					dwHandleID;							//处理 ID 号码
	};

	//变量定义
protected:
	UINT						m_uIndex;							//SOCKET 索引
	DWORD						m_dwHandleID;						//处理标识
	char						m_szSendBuf[SED_SIZE_Z];			//发送数据缓冲区
	char						m_szRecvBuf[RCV_SIZE_Z];			//数据接收缓冲区
	long int					m_lBeginTime;						//连接时间
	SOCKET						m_hSocket;							//SOCKET 句柄
	CTCPSocketManageForZ		* m_pManage;						//SOCKET 管理类指针
	struct sockaddr_in			m_SocketAddr;						//地址信息

	//内部数据
private:
	bool						m_bSending;							//是否发送中
	DWORD						m_dwSendBuffLen;					//发送缓冲区长度
	DWORD						m_dwRecvBuffLen;					//接收缓冲区长度
	CAFCSignedLock				m_csSendLock;						//读取同步锁
	CAFCSignedLock				m_csRecvLock;						//接受同步锁
	OverLappedStruct			m_SendOverData;						//发送数据重叠结构
	OverLappedStruct			m_RecvOverData;						//接收数据重叠结构

	//函数定义
public:
	//构造函数
	CTCPSocketForZ(void);
	//析够函数
	virtual ~CTCPSocketForZ(void);

	//功能函数
public:
	//获取处理标志
	inline DWORD GetHandleID() { return m_dwHandleID; }
	//合并 SOCKET
	inline bool Attach(SOCKET hSocket, sockaddr_in & Addr, DWORD dwHandleID);
	//发送数据函数
	inline int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
	inline int SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);

	//功能函数
public:
	//判断是否连接
	inline bool IsConnect() { return (m_hSocket!=INVALID_SOCKET); }
	//获取连接时间
	inline long int GetBeginTime() { return m_lBeginTime; };
	//获取连接信息
	inline ULONG GetConnectData() { return m_SocketAddr.sin_addr.S_un.S_addr; }
	//获取接收锁
	inline  CAFCSignedLock * GetRecvLock() { return &m_csRecvLock; }
	//获取发送锁锁
	inline  CAFCSignedLock * GetSendLock() { return &m_csSendLock; }
	//判断是否连接
	inline bool IsConnectID(DWORD dwHandleID);

	//辅助函数
protected:
	//设置 ID 索引号码
	inline void SetIndex(UINT uIndex) { m_uIndex=uIndex; };
	//设置管理类指针
	inline void SetSocketManagePtr(CTCPSocketManageForZ * pManage) { m_pManage=pManage; };
	//发送完成函数
	inline bool OnSendCompleted(DWORD dwThancferred, DWORD dwHandleID);
	//接收完成函数
	inline bool OnRecvCompleted(DWORD dwHandleID);
	//检测发送数据函数
	inline bool CheckSendData();
	//投递接收 IO
	inline bool RecvData();
	//关闭 SOCKET
	inline bool CloseSocket();
};

/**************************************************************************************************************/

//TCP 网络 SOCKET 管理类
class AFX_EXT_CLASS CTCPSocketManageForZ
{
	///int m_CurConnectCount;
	//结构定义
	struct BatchSendLineStruct	//批量发送线程队列结构
	{
		//变量定义
		NetMessageHead			NetHead;						//数据包头
		char					szSendData[MAX_SEND_SIZE];		//发送缓存
	};

	struct ThreadStartStruct	//线程启动参数结构
	{
		//变量定义
		HANDLE					hCompletionPort;				//完成端口
		HANDLE					hThreadEvent;					//线程事件
		CTCPSocketManageForZ	* pSocketManage;				//SOCKET 管理指针
	};

	//内核变量
protected:
	bool						m_bInit;						//初始化标志
	volatile bool				m_bRun;							//工作标志
	CListenSocket				m_ListenSocket;					//监听 SOCKET
	BYTE						* m_pKeep;						//保持连接数据
	IServerSocketServiceForZ	* m_pService;					//服务接口
	CAFCPtrArray<CTCPSocketForZ>	m_SocketPtrArray;				//TCP SOCKET 数组

	//设置参数
protected:
	bool						m_bCheckConnect;				//是否检测断线
	BYTE						m_bMaxVer;						//最新版本
	BYTE						m_bLessVer;						//最低版本
	UINT						m_uCheckTime;					//检测断线间隔
	UINT						m_uListenPort;					//监听断口
public:
	UINT						m_iSecretKey;					//加密校验码的密钥

	//批发数据
protected:
	UINT						m_uBatchCerrenPos;				//批量发送点
	UINT						m_uBatchLineCount;				//发送队列数目
	CAFCSignedLock				m_csBacthSend;					//批发同步锁
	BatchSendLineStruct			m_SendLine[uBatchLineSize];		//批量发送队列

	//完成端口变量
private:
	UINT						m_uRSThreadCount;				//读写线程数目
	HANDLE						m_hEventThread;					//批量发送事件
	HANDLE						m_hCompletionPortBatch;			//批量完成端口
	HANDLE						m_hCompletionPortRS;			//读写完成端口
	HANDLE						m_hThreadKeep;					//SOCKET 维护线程
	HANDLE						m_hThreadAccept;				//应答线程

	//函数定义
public:
	//构造函数
	CTCPSocketManageForZ();
	CTCPSocketManageForZ(CTCPSocketManageForZ&);
	CTCPSocketManageForZ& operator=(CTCPSocketManageForZ&); 
	//析构函数
	virtual ~CTCPSocketManageForZ();

	//服务函数
public:
	//初始化
	virtual bool Init(UINT uMaxCount, UINT uListenPort, BYTE bMaxVer, BYTE bLessVer, int iSecretKey, IServerSocketServiceForZ * pService);
	//取消初始化
	virtual bool UnInit();
	//开始服务
	virtual bool Start(UINT uAcceptThreadCount, UINT uSocketThradCount);
	//停止服务
	virtual bool Stop();
	//消息处理函数
	virtual bool OnNetMessage(CTCPSocketForZ * pSocket, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uIndex, DWORD dwHandleID);

	//功能函数
public:
	//发送数据函数
	inline int SendData(UINT uIndex, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
	inline int SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//发送数据函数
	inline int SendData(CTCPSocketForZ * pSocket, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID);
	//批量发送函数
	inline int SendDataBatch(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//关闭连接
	inline bool CloseSocket(UINT uIndex, DWORD dwHandleID);
	//关闭连接
	inline bool CloseSocket(CTCPSocketForZ * pSocket, DWORD dwHandleID);
	//判断是否连接
	inline bool IsConnectID(UINT uIndex, DWORD dwHandleID);
	//数值变 IP 字符
	static TCHAR * ULongToIP(ULONG dwIP, TCHAR * szIP);
	//获取本机 IP 地址
	inline static bool GetLoaclIPAddr(TCHAR szAddrBuffer[], UINT uStation);

	//内核函数
private:
	//生成空闲 SOCKET
	inline CTCPSocketForZ * TCPSocketNew();
	//释放活动 SOCKET
	inline bool TCPSocketDelete(CTCPSocketForZ * pSocket);
	//SOCKET 连接应答线程
	inline static unsigned __stdcall ThreadAccept(LPVOID pThreadData);
	//SOCKET 数据接收线程
	inline static unsigned __stdcall ThreadRSSocket(LPVOID pThreadData);
	//SOCKET 批量发送数据线程
	inline static unsigned __stdcall ThreadBatchSend(LPVOID pThreadData);
	//SOCKET 维护线程
	inline static unsigned __stdcall ThreadKeepActive(LPVOID pThreadData);
	//获取时间函数
	inline static unsigned __int64 AFCGetTimer();
	//获取处理 ID 号码
	inline static DWORD GetHandleID();
};

// Modified by zxd 20090804
// 为了支持Z服务器最大人数突破1W，须改小缓冲区，但同时不能影响W服务器，
// 因此新建一套平行的Socket类供Z使用，保留原有类供W使用
// 以上是新建的类供Z使用
/////////////////////////////////////////////////////////////////////////////////////////////


/**************************************************************************************************************/

#endif