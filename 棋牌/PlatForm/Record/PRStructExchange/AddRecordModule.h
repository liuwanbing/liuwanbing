#pragma once
#include "basemessage.h"
#include "gameroommessage.h"
#include "PRStructExchange.h"
#include "..\include\interface.h"
#include "..\\include\struct.h"
/********************************************************************
	created:	2009/02/24
	created:	24:2:2009   17:25
	filename: 	\Record\PRStructExchange\AddRecordModule.h
	file path:	\project\Record\PRStructExchange
	file base:	AddRecordModule
	file ext:	h
	author:		yjj
	
	purpose:  添加录像插件过程的封装。	
  
  Copyright szbzw 2009 - All Rights Reserved

*********************************************************************/

//往平台添加录像模块插件
class CAddRecordModule
{
public:
	CAddRecordModule(void);
	~CAddRecordModule(void);

	//功能函数	
	/**
	* 创建录像模块 进入房间后调用
	* @param [in] pAppPath 运行程序所在的目录
	* @param [in] bool bIsRecordAvailable 是否启用录像插件
	* @return  创建成功，返回true,创建失败，返回false
	*/
	bool CreatRecorMoudle(const TCHAR* pAppPath,bool bIsRecordAvailable);      //创建录像插件
	
	/**
	* 初始化录像插件bool CGameRoomEx::StartGameClient() 中调用
	* @param [in] sourcegameinfo 游戏基本信息结构
	* @param [in] szClienDllName 游戏ico名字
	* @param [in] bMyDeskStation 自己的座位
	* @return 
	*/
	void InitRecordMoudle(GameInfoStruct* sourcegameinfo,TCHAR* szClienDllName,BYTE bMyDeskStation);      //初始化录像插件	
	
	/**
	* 保存录像文件，在一局游戏结bool CGameRoomEx::OnRoomMessage ASS_GR_GAME_END分支下本桌游戏结束调用或玩家点oncancle时调用
	* @param [in] pAppPath 可执行程序的名称
	* @param [in] pMyName 自己玩家姓名
	* @param [in] uNameId 游戏的id号
	* @return bool 保存成功，返回true,保存失败，返回false
	*/
	bool SaveRecordFile(const TCHAR* pAppPath,const TCHAR* pMyName,UINT uNameId);        //保存录像文件
	
	/**
	* 设置游戏用户信息在在一局游戏结bool CGameRoomEx::OnRoomMessage ASS_GR_GAME_BEGIN分支下或者断线回来设置场景时调用。
	* 须把本桌中所有的玩家均设置如录像系统，否则播放无法正常还原
	* @param [in] pUserItem 游戏基本信息结构
	* @param [in] pMeUserInfo 游戏ico名字
	* @param [in] bMyDeskStation 自己的座位
	* @return 
	*/
	void SetUserInfo2Record(const UserItemStruct* pUserItem,const UserItemStruct* pMeUserInfo); //给录像系统加入用户信息

	/**
	* 向录像插件中加入游戏消息 CGameRoomEx::OnSocketReadEvent中调用
	* @param [in] pNetHead 消息头
	* @param [in] pNetData 消息数据
	* @param [in] uDataSize 消息大小
	* @return 
	*/
	void AddMsgToRecord(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);        //向录像系统添加游戏消息
	
	/**
	* 判断当前的游戏状态
	* @param [in] pGameInfo 自己的玩家信息
	* @param [in] pNetHead    消息头
	* @return bool true 为需要设置玩家信息，false为不需要设置玩家信息
	*/
	bool GetUserGameState(const GameInfoStruct* pGameInfo ,NetMessageHead * pNetHead);
	//成员私有函数
private:
	/**
	* 释放录像插件，在析构函数里做	
	*/
	void ReleaseRecordMoule();    //释放录像插件
	//录像插件部分
	IRecorderForPlatform           * m_pRecordMoudlePlat;
	HINSTANCE                        m_hRecordInstance;
	bool                             m_bIsRecordAvailable;


};

