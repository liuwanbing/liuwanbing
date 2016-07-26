#pragma once


//////////////////////////////////////////////////////////////////////////
///后台W服务器客户端类，负责存储，接收，发送与W服务器相关的数据
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////


#include "../../../SDK/include/socket/TCPClientSocket.h"
#include "comstruct.h"
#include "gameplacemessage.h"
#include "basemessage.h"
#include "GameUserManage.h"
#include "gameroommessage.h"
#include "comstruct.h"
#include "CommandMessage.h"
#include "GM_MessageDefine.h"

class CBackWSocket :
	public IClientSocketService
{
public:
	CBackWSocket(void);
public:
	~CBackWSocket(void);

public:
	///网络读取消息回调函数
	bool OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///网络连接消息回调函数
	bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket);
	///网络关闭消息回调函数
	bool OnSocketCloseEvent();

public:
	///网络连接回调函数
	bool OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket); 
public:
	///连接到服务器
	bool ConnectToServer();
	///断开连接
	bool closesocket();
	///登录
	bool Longo();
	///发送对用户的操作
	bool SendOpera(GM_OperateByID_t* opera);
private:
	CTCPClientSocket			m_ClientSocket;

public:
	HWND						m_hwnd;			///接受消息的窗口
	GM_MSG_O_LogonResult_t*		m_pLogonInfo;   ///登录数据
	ComRoomInfo*				m_pRoomInfo;	///房间信息
};
