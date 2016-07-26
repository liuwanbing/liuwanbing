#pragma once

//////////////////////////////////////////////////////////////////////////
///窗口之间的消息,用于主窗口在各个窗口界面传输数据
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////



#define MSG_CMD_GET_GAMEPARA				(WM_USER + 103)                              ///获得参数
#define MSG_CMD_CONNET_Z_SUCCEED			(WM_USER + 104)								 ///连接Z服务器成功	
#define MSG_CMD_CONNET_Z_FAIL				(WM_USER + 129)								 ///连接Z服务器失败	
#define MSG_CMD_LONGIN						(WM_USER + 105)                              ///登录	
#define MSG_CMD_GET_GAMEKIND				(WM_USER + 106)                              ///获得游戏类型
#define MSG_CMD_GET_GAMENAME				(WM_USER + 107)								 ///获得游戏名称
#define MSG_CMD_GET_GAMEROOM				(WM_USER + 108)								 ///获得游戏房间
#define MSG_CMD_UPDATE_GAMEROOM				(WM_USER + 109)								 ///更新游戏房间
#define MSG_CMD_ENTER_GAMEROOM				(WM_USER + 110)								 ///进入游戏房间
#define MSG_CMD_SHOW_GAMEROOM				(WM_USER + 111)                              ///显示游戏房间
#define MSG_CMD_SHOW_GAMEDESKSTATE			(WM_USER + 112)								 ///显示桌子的状态（机器人， 末开始，游戏中单独更新的）
#define MSG_CMD_UPDATE_GAMEPEO				(WM_USER + 113)								 ///更新游戏人数（单独更新）

#define MSG_CMD_OPERATE						(WM_USER + 114)								 ///对用户操作（禁言，踢出房间，禁止登录）
#define MSG_CMD_BROADCAST					(WM_USER + 115)								 ///发公告


#define MSG_CMD_GM_ACT_AWORD				(WM_USER + 116)                              ///奖励
#define MSG_CMD_GM_ACT_NO_TALK				(WM_USER + 117)                              ///禁言
#define MSG_CMD_GM_ACT_KICK_OUT				(WM_USER + 118)                              ///踢出房间
#define MSG_CMD_GM_ACT_BANDON_ID			(WM_USER + 119)                              ///禁止登录

#define MSG_CMD_CM_START_CHAT				(WM_USER + 121)                              ///发起私聊


#define MSG_CMD_GM_TALK						(WM_USER + 122)                              ///收到私聊消息 


#define MSG_CMD_GM_LOCK						(WM_USER + 123)                              ///封桌
#define MSG_CMD_GM_UNLOCK					(WM_USER + 124)                              ///解封
#define MSG_CMD_GM_ALLLOCK					(WM_USER + 125)                              ///批量封桌
#define MSG_CMD_GM_ALLUNLOCK				(WM_USER + 126)                              ///批量解封
#define MSG_CMD_GM_ALLLOCKOK				(WM_USER + 130)								 ///批量封桌
#define MSG_CMD_GM_ALLUNLOCKOK				(WM_USER + 131)                              ///批量解封

#define	MSG_CMD_GM_FINDUSER					(WM_USER + 127)                              ///搜索用户
#define	MSG_CMD_GM_GETUSER					(WM_USER + 128)								 ///取得用户				

#define MSG_CMD_BACKWLOGIN					(WM_USER + 129)                              ///后台登录W 

#define MSG_CMD_GM_PRINTLOG					(WM_USER + 132)                              ///打印Logo日志