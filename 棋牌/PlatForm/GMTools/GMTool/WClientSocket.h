#pragma once

//////////////////////////////////////////////////////////////////////////
///W服务器客户端类，负责存储，接收，发送与W服务器相关的数据
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

static CString LOGIN_ERR_STRING[16] = {"未知错误", "登陆成功", "用户不存在", "用户密码错误", "用户帐号禁用", "登陆 IP 禁止",
"不是指定地址", "会员游戏房间", "正在其他房间", "帐号正在使用", "人数已经满", "部分用户列表", "全部用户列表",
"暂停登陆服务", "游戏房间", "时间到期"};


class CWClientSocket :
	public IClientSocketService
{
public:
	CWClientSocket();
	CWClientSocket(GM_MSG_O_LogonResult_t* plogonInfo, ComRoomInfo* pRoomInfo);
public:
	~CWClientSocket(void);


public:
	//实现IClientSocketService的三个接口
public:
	//网络读取消息
	bool OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	//网络连接消息
	bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket);
	//网络关闭消息
	bool OnSocketCloseEvent();

public:
	///连接后回调函数
	bool OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket); 
	///获取游戏列表的回调函数
	bool OnGetUserList(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket); 
	///用户动作回调函数
	bool OnUserAction(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket); 
	///房间消息回调函数
	bool OnRoomMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///封桌
	bool OnLockMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);

public:
	///连接到服务器
	bool ConnectToServer();
	///关闭连接
	bool CloseSocket();
	///登录
	bool Longo();
	///发送操作
	bool SendOpera(GM_OperateByID_t* opera);
	///封桌
	bool LockDesk(int UserID, int iDeskIndex, int state);
	///批量封桌
	bool LockDesk(int UserID, int lower, int upper, int state);
	///打印日志
	bool SendPrintLogCmd(GM_MSG_PrintLog_t* pCmd);

private:
	///获取用户序号
	UINT GetUserSortIndex(UserInfoStruct * pUserItem);
	///
	UINT GetUserConnection(long int dwUserID);

private:
	CTCPClientSocket			m_ClientSocket;

public:
	HWND						m_hwnd;				///接收消息窗口
	GM_MSG_O_LogonResult_t*		m_pLogonInfo;		///登录信息
	ComRoomInfo*				m_pRoomInfo;		///房间信息	

	CGameUserManage				m_UserManage;		///用户管理表
	CDeskStation				m_DeskStation;		///桌子状态	
};
