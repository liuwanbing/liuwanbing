#pragma once

#ifndef CLIENTCOMSTRUCT_HEAD_FILE
#define CLIENTCOMSTRUCT_HEAD_FILE

//#include "PublicDefine.h"

/*******************************************************************************************************/

//启动游戏客户端//GetFrameInterface
#define GETFRAMEINTERFACE(GAME_DLG_CLASS) \
	extern "C" __declspec(dllexport) IFrameInterface * BeginInitGoGame() \
{ \
	return new GAME_DLG_CLASS();\
}

/*******************************************************************************************************/

//获取图标函数//GetGameIcon
#define GETGAMEICON(NAME_ID) extern "C" __declspec(dllexport) HICON BeginInitGameIcon(UINT uNameID, bool bGameList) \
{ \
	CString msg;\
	msg.Format("%d.ico",NAME_ID);\
	if (bGameList) return LoadIcon(GetModuleHandle(msg),MAKEINTRESOURCE(IDI_CLIENT_LIST)); \
	return LoadIcon(GetModuleHandle(msg),MAKEINTRESOURCE(IDI_CLIENT_ROOM)); \
}

/*******************************************************************************************************/

//获取登记等级函数//GetOrderFunction
#define GETORDERFUNCTION(ORDER_FUNCTION) extern "C" __declspec(dllexport) GetOrderName * SetGameInitClass(UINT uNameID) \
{ \
	return GetGameOrderName; \
}

/*******************************************************************************************************/

//获取客户端信息//GetGameClientInfo
#define GETCLIENTINFO extern "C" __declspec(dllexport) bool InitShareMemory(ClientDLLInfoStruct * pClientInfo, UINT uVer) \
{ \
	if (uVer==DEV_LIB_VER) \
	{ \
		pClientInfo->uNameID=NAME_ID; \
		pClientInfo->uPlayCount=PLAY_COUNT; \
		pClientInfo->uProgramVer=GAME_MAX_VER; \
		pClientInfo->uChangeVer=GAME_CHANGE_VER; \
		pClientInfo->uSupperType=SUPPER_TYPE; \
		lstrcpy(pClientInfo->szGameName,GAMENAME); \
		CString msg;\
		msg.Format("%d.ico",NAME_ID);\
		lstrcpy(pClientInfo->szClientDLLName,msg); \
		return true; \
	} \
	return false; \
}

/*******************************************************************************************************/

//获取等级名字函数声明
typedef TCHAR * (GetOrderName)(__int64 dwPoint);

//接口声明
interface IFindUserInterface;
interface ISocketSendInterface;

/*******************************************************************************************************/

//用户信息结构
struct UserItemStruct
{
	bool							bMatchUser;							//是否比赛
	UINT							uSortIndex;							//排列索引
	UINT							uConnection;						//玩家关系
	long int						dwLogonTime;						//登陆时间
	UserInfoStruct					GameUserInfo;						//用户信息
};

//游戏共享信息结构
struct GameInfoStruct
{
	IFindUserInterface				* pIFindUser;						//查找接口
	//共享信息
	bool							bDeskOnwer;							//是否台主
	BYTE							bGameStation;						//游戏状态
	UserItemStruct					* pMeUserInfo;						//玩家信息
	long int						dwGamePower;						//用户权限
	long int						dwMasterPower;						//管理权限
	long int						dwRoomRule;							//设置规则
	GetOrderName					* pOrderName;						//等级函数

	//设置参数
	bool							bEnableSound;						//允许声音
	bool							bEnableWatch;						//允许旁观
	bool							bShowUserInfo;						//显示进出
	bool							bEnableBackSound;					//背景音乐
	//接口变量
	ISocketSendInterface			* pISocketSend;						//发送接口

	//大厅数据
	TCHAR							szProcessName[31];					//进程名字
	UINT							uDeskPeople;						//游戏人数
	UINT							uRoomID;							//房间表示
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	UINT							uComType;							//游戏类型
	UINT							uNameID;							//名字表示
	TCHAR							szGameName[61];						//游戏名字
	UINT							uLessPoint;							//最少经验值

	int							uVirtualUser;						//20081211 , Fred Huang
	int							uVirtualGameTime;					//20081211 , Fred Huang
	bool                            bIsInRecord;                        //是否在录像中 yjj  090223  
};

//获取游戏组件结构
struct ClientDLLInfoStruct
{
	UINT							uNameID;							//名字 ID
	UINT							uChangeVer;							//修改版本
	UINT							uProgramVer;						//程序版本
	UINT							uPlayCount;							//游戏人数
	UINT							uSupperType;						//支持类型
	TCHAR							szGameName[61];						//游戏名字
	TCHAR							szClientDLLName[61];				//文件名字
};

/*******************************************************************************************************/

#endif