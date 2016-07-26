#pragma once

#include "xsocketbase.h"

#include <WinSock2.h>
#include <Windows.h>
#include <string>
using namespace std;


///网络数据包结构头
struct Net__MessageHead
{
	DWORD						uMessageSize;						///数据包大小
	DWORD						bMainID;							///处理主类型
	DWORD						bAssistantID;						///辅助处理类型 ID
	DWORD						bHandleCode;						///数据包处理代码
	DWORD						bReserve;							///保留字段
};


//缓存定义
#define SED_SIZE				60000									//缓冲区大小
#define RCV_SIZE				30000									//缓冲区大小

//状态定义 
#define NO_CONNECT				0								//没有连接
#define CONNECTING				1								//正在连接
#define CONNECTED				2								//成功连接

#define CONNECTPROXY1			3
#define CONNECTPROXY2			4
#define CONNECTPROXY3			5
#define CONNECTPROXY4			6

//#define __PROXY__ ///定义则表示使用代理服务器功能


///连接成功消息 
struct MSG_S_ConnectSuccess__
{
	BYTE						bMaxVer;							///最新版本号码
	BYTE						bLessVer;							///最低版本号码
	BYTE						bReserve[2];						///保留字段
};

#define NET_HEAD_SIZE			sizeof(NetMessageHead)				///数据包头
#define MAX_SEND_SIZE			2044								///最大消息包


////客户端网络服务接口
//interface IServerSocketService
//{
//	//接口函数 
//public:
//	//////////////////////////////////////////////////////////////////////////
//	virtual bool OnSocketAccept(SOCKET iSocket) = 0;
//	//网络读取消息
//	virtual bool OnSocketReadEvent(SOCKET iSocket, void * pNetData, UINT uDataSize)=0;
//	//网络关闭消息
//	virtual bool OnSocketCloseEvent(SOCKET iSocket)=0;
//};
//


class CTCPServerSocket : public CWnd
{
DECLARE_MESSAGE_MAP()

public:
	CTCPServerSocket(IServerSocketService* pSocket);
	~CTCPServerSocket(void);



	//变量定义
private:
	int									m_iReadBufLen;					//缓存区长度
	BYTE								m_bConnectState;				//连接状态
	SOCKET								m_hSocket;						//SOCKET 句柄
	IServerSocketService				*m_pIService;					//处理接口
	SOCKET								m_hClientSocket;
	BYTE								m_szBuffer[RCV_SIZE];			//数据缓存区

	int									m_iPort;

	//HWND								m_hWnd;						//消息窗口

	//函数定义
private:

	//是否已经连接
	BYTE GetSocketState() { return m_bConnectState; }
	//监听网络
	bool Listen();
	//绑定地址
	bool Bind(UINT uPort);


	//解释错误
	string TranslateError(UINT uErrorCode);

	//SOCKET 消息处理程序
	LRESULT	OnSocketNotifyMesage(WPARAM wParam, LPARAM lParam);



	//内部函数 
private:
	//初始化 SOCKET 窗口
	bool InitSocketWnd();
	//关闭 SOCKET 窗口
	bool CloseSocketWnd();

	void CloseSocket(INT32 nSocketID);

public:
	/// 关闭指定Socket序号的socket
	void CloseSocket();
	int SendData(UINT8 * pBuffer, int nLen);
	int GetSocketPort() {return m_iPort;}
	HWND GetHWDN() {return m_hWnd;} 
	void CloseClientSocket();

	//发送函数
	int SendData(int nSocketID, void * pData, int uSize, int bMainID, int bAssistantID, int bHandleCode);
	//简单命令发送函数
	int SendData(int nSocketID, int bMainID, int bAssistantID, int bHandleCode);
	///发送函数
	int SendData(int nSocketID, UINT8* pBuffer, int nLen);

};


