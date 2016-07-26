#pragma once


#ifndef CLIENTCOMSTRUCT_HEAD_FILE
#define CLIENTCOMSTRUCT_HEAD_FILE

#define EXT_CLASS _declspec(dllimport)

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

/*******************************************************************************************************/

#endif