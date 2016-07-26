#pragma once

#ifndef GAMEUSERMANAGE_HEAD_H
#define GAMEUSERMANAGE_HEAD_H

#include "StdAfx.h"
//#include "GamePlace.h"

//游戏用户管理类
class CGameUserManage : public IFindUserInterface
{
	//变量定义
protected:
	CPtrArray								m_ActiveUserPtrArray;			//活动用户指针
	CPtrArray								m_FreeUserPtrArray;				//空闲用户指针
	CDWordArray								m_MatchUserID;					//比赛用户 ID

	//函数定义
public:
	//构造函数
	CGameUserManage(void);
	//析构函数
	virtual ~CGameUserManage(void);

	//功能函数
public:
	//清理数据
	bool CleanUserInfo();
	//获取在线人数
	INT_PTR GetOnLineUserCount() { return m_ActiveUserPtrArray.GetCount(); }
	//增加用户
	UserItemStruct * AddGameUser(UserInfoStruct * pUserInfo, UINT uSortIndex, UINT uConnection);
	//删除用户
	bool DeleteGameUser(long int dwUserID);
	//删除用户
	bool DeleteGameUser(UserItemStruct * pUserInfo);
	//增加比赛用户
	bool AddMatchUser(long int dwUserID);
	//删除比赛用户
	bool DeleteMatchUser(long int dwUserID);
	//是否比赛用户
	bool IsMatchUser(long int dwUserID);

	//接口函数
public:
	//查找用户
	virtual UserItemStruct * FindOnLineUserByIndex(int index);
	//查找用户
	virtual UserItemStruct * FindOnLineUser(long int dwUserID);
	//查找用户
	virtual UserItemStruct * FindOnLineUser(int dwAccID);
	//查找用户
	virtual INT_PTR FindOnLineUser(const TCHAR * szUserName, CPtrArray & ResultPtrArray);
	//查找用户
	virtual INT_PTR FindOnLineUser(BYTE bDeskIndex, CPtrArray & PlayPtrArray, CPtrArray & WatchPtrArray);
};

#endif