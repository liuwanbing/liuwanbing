#ifndef COMMON_H
#define COMMON_H

#pragma once

#pragma pack(push,1)


#define  SafeDeleteArray(pData)	{ try { delete [] pData; } catch (...) { } pData=NULL; } 


#define SOCKET_PACKAGE			2046
#define KIND_LEN				32
#define SERVER_LEN				32
#define MAX_CHAT_LEN			512

#define IPCFILENAME				"AA4F9A47-086F-40a8-856F-83BD017870F6"


//启动结果
#define	SM_CREATE_ERROR				0									//启动错误
#define	SM_CREATE_SUCCESS			1									//启动成功
#define	SM_ALREADY_EXIST			2									//已经存在


//////////////////////////////////////////////////////////////////////////
//数据包命令信息
struct CMD_Command
{
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//////////////////////////////////////////////////////////////////////////

//内核主命令码
#define IPC_MAIN_IPC_KERNEL					0							//内核命令

//内核子命令码
#define IPC_SUB_IPC_CLIENT_CONNECT			1							//连接通知
#define IPC_SUB_IPC_SERVER_ACCEPT			2							//应答通知
#define IPC_SUB_IPC_CLIENT_CLOSE			3							//关闭通知

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////

//IPC 数据定义
#define IPC_VER							0x0001								//IPC 版本
#define IPC_IDENTIFIER					0x0001								//标识号码
#define IPC_PACKAGE						4096								//最大 IPC 包
#define IPC_BUFFER						(sizeof(IPC_Head)+IPC_PACKAGE)		//缓冲长度

//IPC 数据包头
struct IPC_Head
{
	WORD								wVersion;							//IPC 版本
	WORD								wDataSize;							//数据大小
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//IPC 缓冲结构
struct IPC_Buffer
{
	IPC_Head							Head;								//数据包头
	BYTE								cbBuffer[IPC_PACKAGE];				//数据缓冲
};

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//IPC 网络事件

#define IPC_MAIN_SOCKET					1								//网络消息

#define		IPC_SUB_SOCKET_SEND				1								//网络发送
#define		IPC_SUB_SOCKET_RECV				2								//网络接收
#define		IPC_SUB_IPC_HEART_BEAT			3								//心跳通知

//IPC 网络包结构
struct IPC_SocketPackage
{
	CMD_Command							Command;						//命令信息
	BYTE								cbBuffer[SOCKET_PACKAGE];		//数据缓冲
};

//////////////////////////////////////////////////////////////////////////
//IPC 配置信息

#define IPC_MAIN_CONFIG					2								//配置信息

#define		IPC_SUB_SERVER_INFO				1								//房间信息
#define		IPC_SUB_COLUMN_INFO				2								//列表信息

//游戏信息
struct IPC_GF_ServerInfo
{
	DWORD								dwUserID;						//用户 I D
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
	WORD								wKindID;						//类型标识
	WORD								wServerID;						//房间标识
	WORD								wGameGenre;						//游戏类型
	WORD								wChairCount;					//椅子数目
	BYTE								cbHideUserInfo;					//隐藏信息
	DWORD								dwVideoAddr;					//视频地址
	TCHAR								szKindName[KIND_LEN];			//类型名字
	TCHAR								szServerName[SERVER_LEN];		//房间名称
};

//////////////////////////////////////////////////////////////////////////
//IPC 用户信息

#define IPC_MAIN_USER					3								//用户信息

#define		IPC_SUB_USER_COME				1								//用户信息
#define		IPC_SUB_USER_STATUS				2								//用户状态
#define		IPC_SUB_USER_SCORE				3								//用户积分
#define		IPC_SUB_GAME_START				4								//游戏开始
#define		IPC_SUB_GAME_FINISH				5								//游戏结束
#define		IPC_SUB_ASK_USER_LIST			6								//请求同桌用户清单
#define		IPC_SUB_REPLY_USER_LIST			7								//回复同桌用户清单

//用户状态
struct IPC_UserStatus
{
	DWORD								dwUserID;						//用户 I D
	WORD								wNetDelay;						//网络延时
	BYTE								cbUserStatus;					//用户状态
};

//用户分数
struct IPC_UserScore
{
	DWORD								dwUserID;						//用户 I D
//	tagUserScore						UserScore;						//用户积分
};

//////////////////////////////////////////////////////////////////////////
//IPC 控制信息

#define IPC_MAIN_CONCTROL				4								//控制信息

#define		IPC_SUB_START_FINISH			1								//启动完成
#define		IPC_SUB_CLOSE_FRAME				2								//关闭框架
#define		IPC_SUB_JOIN_IN_GAME			3								//加入游戏
#define		IPC_SUB_SHOW_MSG_IN_GAME		4								//显示游戏中消息
#define		IPC_SUB_SHOW_DUDU				5								//显示嘟嘟消息

//////////////////////////////////////////////////////////////////////////
//IPC 道具消息	2008/11/3
#define IPC_MAIN_PROP					5								//道具主消息
#define		IPC_SUB_INIT_PROP				1								//初始化道具
#define		IPC_SUB_INIT_PROP_FINISH		2
#define		IPC_SUB_USE_PROP				3								//使用道具	
#define		IPC_SUB_BUY_PROP				4								//购买道具
#define		IPC_SUB_PRESENT_PROP			5								//给别人赠送道具
#define		IPC_SUB_ACCEPT_PROP				6								//接受别人赠送的道具
#define		IPC_SUB_BROADCAST_BIG			7								//大喇叭消息
#define		IPC_SUB_BROADCAST_SMALL			8								//小喇叭消息
#define		IPC_SUB_USE_MOVIE_PROP			9								//使用动画道具	
#define		IPC_SUB_PRESENT_PROP_FAIL		10								//给别人赠送道具失败

//加入游戏
struct IPC_JoinInGame
{
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
};


//共享内存定义
struct tagShareMemory
{
	WORD								wDataSize;						//数据大小
	HWND								hWndGameFrame;					//框架句柄
	HWND								hWndGamePlaza;					//广场句柄
	HWND								hWndGameServer;					//房间句柄
};

// 用于向EXE发送显示游戏房间消息的结构
struct MsgInGameStruct 
{
	TCHAR szMsg[1000];
	bool  bIsFontValid;
	TCHAR szFontName[32];
	bool  bIsTitleValid;
	TCHAR szTitle[64];
	BYTE bNewOrSys;
	UINT uSize;
};
// 用于向EXE发送嘟嘟消息的结构
struct MsgDuduStruct 
{
	TCHAR szSendName[64];
	TCHAR szTargetName[64];
	TCHAR szCharString[256];
	COLORREF crTextColor;
	bool  bShowTime;
	UINT uSize;
	bool bIsFontValid;
	TCHAR szFontName[64];
	int iDuduType;
};
#pragma pack(pop)

#endif //COMMON_H
