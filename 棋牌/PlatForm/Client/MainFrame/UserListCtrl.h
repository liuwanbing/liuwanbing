#pragma once
#ifndef UESRLISTCTRL_HEAD_H
#define USERLISTCTRL_HEAD_H

#include "StdAfx.h"
#include "Afxcmn.h"
#include "Resource.h"
#include "SkinMgr.h"

///
///CUserListCtrl 用户列表类
///
///@Modify yc.yan
///@version 1.0
///@date
///
class CUserListCtrl  : public CHappyGoDlg
{
	//变量定义
private:
	static UINT					m_uItemKind[15];					//子项类型
	int							m_uComType;
	GameInfoStruct				*pGameInfo;
	CWnd						*msgWnd;							//20081211 , Fred Huang
	UINT						m_uVirtualUserCount;
	//函数定义
public:
	//构造函数
	CUserListCtrl();  
	//析构函数
	virtual ~CUserListCtrl();
	//初始化
	virtual void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType,GameInfoStruct *GameInfo);
	//是否允许查看 IP
	void EnableIPView(bool bLookIP);

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	///
	///增加用户
	///
	///@param pUserItem 用户信息
	///
	///@return int 插入的位置，-1插入失败
	///
	virtual bool AddGameUser(UserItemStruct * pUserItem);
	//删除用户
	virtual bool DeleteGameUser(UserItemStruct * pUserItem);
	//更新状态
	virtual bool UpdateGameUser(UserItemStruct * pUserItem);
	//换皮肤
	virtual void LoadSkin(void);

public:
	//换肤 add by rende on 2012-8-31
	void ReLoadSkin();

public:
	/// 填充虚拟玩家
	///	@param uVirtualUserCount虚拟玩家人数
	///	@return void
	void fillVirtualUser(int uVirtualUserCount);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void SetMSGReceiveWnd(CWnd * pWnd);//20081211 , Fred Huang
public:
	/// 查找空闲的虚拟玩家
	///@param void
	///@return UserItemStruct 虚拟玩家信息
	UserItemStruct * GetIdelVirtualUser(void);
	/// 查找指定虚拟玩家
	///@param long dwUserID 需要查找的用户ID
	///@return UserItemStruct 玩家信息
	UserItemStruct * GetUserFromID(long dwUserID);
	/// 获取用户插入的位置
	int GetInsertPos(UserItemStruct * pUserItem);
	/// 加载列设置
	void LoadColumnConfig();
	/// 将字符串解析出 顺序号，列名，列宽
	void DecodeString(CString str,CString &strColumn,int &width);
	/// 设置每一行的值
	void PrepareDate(UserItemStruct * pUserItem);
	/// 获取玩家ID信息，是否散户，特别账号，指派账号
	/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
	/// @return 0到8是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
	int GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd);

private:
	CSkinMgr m_skinmgr;

public:
	void ChangeSkin();
};
#endif