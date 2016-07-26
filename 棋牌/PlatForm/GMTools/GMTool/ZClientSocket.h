#pragma once

//////////////////////////////////////////////////////////////////////////
///Z服务器客户端类，负责存储，接收，发送与Z服务器相关的数据
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "../../../SDK/include/socket/TCPClientSocket.h"
#include "CenterServerhead.h"
#include "CommandMessage.h"
#include "comstruct.h"
#include "gameplacemessage.h"
#include "iptypes.h"
#include "iphlpapi.h"
#include "sha1.h"
#include "MD5.h"
#include "GM_MessageDefine.h"

#include <vector>
#include <map>
#include <algorithm>
using namespace std;

class CZClientSocket :
	public IClientSocketService
{
public:
	CZClientSocket(void);
	~CZClientSocket(void);

public:
	//实现IClientSocketService的三个接口
public:
	///网络读取消息
	bool OnSocketReadEvent(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///网络连接消息
	bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket* pClientSocket);
	///网络关闭消息
	bool OnSocketCloseEvent();


public:
	///连接到服务器
	bool ConnectToServer();

	///服务器返回的消息响应
private:
	///连接后回调函数
	bool OnConnect(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///登录后回调函数
	bool OnLongo(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///获取游戏列表的回调函数
	bool OnGetGameList(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///收到GM消息的回调函数
	bool OnGMMessage(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);
	///收到用户私聊的信息
	bool OnTalkMsg(NetMessageHead* pNetHead, void* pNetData, UINT uDataSize, CTCPClientSocket* pClientSocket);

	///向服务器发送指令
public:
	///登录
	bool Longo();
	///获取游戏列表
	bool GetGameList();
	///获取游戏房间
	bool GetGameRoom(UINT uKindID, UINT uNameID);
	///发送操作
	bool SendOperate(GM_OperateByID_t *Operate);
	///发送搜索用户
	bool SendFindUser(GM_GetOperationByPlayer_t *play);
	///发送私聊和公告
	bool SendTalk(GM_Broadcast_t *talk);

private:
	///获取机器码
	CString innerGetMac();
	CString coreGetCode();

private:
	CTCPClientSocket	m_ClientSocket;

public:
	CenterServerMsg				m_ServerInfo;					///Z服务器信息
	CString						m_UerseName, m_PassWord;		///用户名与密码
	GM_MSG_O_LogonResult_t		m_LogonInfo;					///登录信息
	MSG_GP_S_LogonByNameStruct	m_LogonByName;					///登录信息
	GM_MSG_S_LogonByName_t      m_GMLogin;						///登录数据包

	GM_Broadcast_t				m_GMTalkMsg;					///公告私聊数据包
	GM_OperatedPlayerInfo_t		m_GMFindUser;					///查找用户数据包

	
	map<int, GM_AwardType_t*> m_AwardType;						///奖励方案表
	map<int, GM_GetPropInfo_t*> m_PropList;						///道具表
	
	bool m_GetAwardTypeing;										///是否获取奖励方案中
	bool m_GetAwardListing;										///是否获取道具列表中
	
	vector<ComKindInfo*> m_KindList;							///游戏类型表
	vector<ComNameInfo*> m_NameList;							///游戏名字表
	vector<ComRoomInfo*> m_RoomList;							///游戏房间表
};
