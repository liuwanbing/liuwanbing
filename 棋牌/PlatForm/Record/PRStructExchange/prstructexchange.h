#pragma once

#include "../include/struct.h"
#include "comstruct.h"
#include "../../Client/UILibrary2003/ClientComStruct.h"
//#include "GameUserManage.h"

#define MAX_PLAYER_COUNT   180   //最多有八个玩家

class CPRStructExchange
{
public:
	CPRStructExchange(void);
	~CPRStructExchange(void);
	//*************************公用函数*************************/
	/**
	* 录像插件数据结构转换为大厅
	* 
	*/	
	//客户端和服务器公用函数
	/**
	* 转换user结构 从平台结构转换为录像结构
	*/
	bool ChangeUsrInfoPlat2Re(RecordUserInfoStruct* DestUser,const UserInfoStruct* sourceUser);
	//**************************服务器专用函数************************/

	bool ChangeUserIterPlayToReSrv(RecordUserItemStruct* destitem,UserInfoStruct* sourceUser,BYTE bMyDeskStation);
    
	bool ChangeGameRoomPlatToReSrv(RecordGameInfoStruct* destgameinfo,ManageInfoStruct* sourcmange,BYTE bMyDeskStation);
	//**************************客户端专用函数************************/
	bool ChaneUserItemPlatToReClient(RecordUserItemStruct* destitem,const UserItemStruct* souceitem);
	bool ChangeGameRoomPlatToReClient(RecordGameInfoStruct* destgameinfo,GameInfoStruct* sourcegameinfo,TCHAR* szClienDllName,BYTE bMyDeskStation);
	
	//***************************播放器用函数*************************************/
	//
	bool ChaneUserItemReToPlatClient(UserItemStruct* destitem,RecordUserItemStruct* souceitem);
	bool ChangeGameRoomReToPlatClient(GameInfoStruct* destgameinfo,RecordGameInfoStruct* sourcegameinfo);
	bool ChangeUsrInfoRe2Plat(UserInfoStruct* DestUser,RecordUserInfoStruct* sourceUser);
	
	//void CleanAllUser() {m_UserManage.CleanUserInfo();}
	//CGameUserManage* GetUserManage() {return &m_UserManage;}

private:
	UINT GetUserSortIndex(UserInfoStruct * pUserItem,BYTE bMyDeskStation);




	//CGameUserManage		m_UserManage;
	

};
