#pragma once


interface IDistriManagerBase
{
public:
	//发送函数
	virtual INT32 SendData(UINT uIndex, void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID)=0;
	virtual INT32 SendBatchData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode)=0;
	virtual INT32 SendNormalData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode)=0;

	//简单命令发送函数
	virtual INT32 SendData(UINT uIndex, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID)=0;

	virtual bool CheckReConnect() = 0;
	virtual bool CheckKeepLive() = 0;
};

interface IDistriManagerNetBase
{
public:
	/// 根据用户ID获取Socket序号
	virtual int GetIndexByID(int iUserID)=0;
	/// 向客户端发送广播数据
	virtual int SendBatchDataSelf(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode) = 0;
	//发送函数
	virtual int SendData(UINT uIndex, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode, DWORD dwHandleID) = 0;
	
	virtual int KickUserSocket(DWORD dwUserID) = 0;

	virtual int GetZid() = 0;

	virtual int GetServerPort() = 0;
	// B服务器无法在Z服务器列表中找到玩家时，会反馈消息包给发起的Z服务器。这里进行容错处理。
	virtual int SendDataFail(UINT uUserID, void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, UINT bHandleCode, DWORD dwHandleID) = 0;

	//SOCKET 数据读取
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID) = 0;

};

//rongqiufen20111018
#define ID_DistriManager_WND			23								//SOCKET 窗口 ID
class CDistriManager :public IDistriManagerBase, public IClientSocketService
{
public:
	CDistriManager::CDistriManager(IDistriManagerNetBase *pNet);
	CDistriManager::~CDistriManager();

	///接口函数 
public:
	///网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	///网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);
	///网络关闭消息
	virtual bool OnSocketCloseEvent();

	INT32 Initial();

	INT32 Connect(const char * szServerIP, UINT32 uPort);

	INT32 Connect(UINT32 dwServerIP, UINT32 uPort);
	//发送函数
	INT32 SendData(UINT uUserID, void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID);
	//简单命令发送函数
	INT32 SendData(UINT uUserID, UINT bMainID, UINT bAssistantID, UINT bHandleCode, DWORD dwHandleID);

	INT32 SendNormalData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode);

	INT32 SendBatchData(void * pData, UINT uSize, UINT bMainID, UINT bAssistantID, UINT bHandleCode);

	void CloseSocket(INT32 nSocketID, bool bNotify);

	virtual bool CheckReConnect();
	virtual bool CheckKeepLive();
private:
	CTCPClientSocket		*m_pSocket;
	IDistriManagerNetBase	*m_pNet;
	int					m_bConnect;			//0表示末开始连接，1表示已经发起连接但没返回， 2表示连接成功
	int					m_nKeepAliveCount;
	CString				m_strHostIP;
};

extern IDistriManagerBase *CreateDistriManager(IDistriManagerNetBase *pNet);
extern void DestoryDistriManager(IDistriManagerBase*);

//rongqiufen20111018end