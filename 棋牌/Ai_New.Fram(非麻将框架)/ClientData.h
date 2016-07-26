
// ***************************************************************
//  GameDataObject   version:  1.0   ·  date: 12/31/2010
//  -------------------------------------------------------------

//	升级，服务端游戏数据\
	NameID：50200260

//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once
#include "UserData.h"

class CClientData
{

public:

	//初始化数据
	CClientData();

	//释放数据
	virtual~CClientData(void);

/*-常用的控件处理函数-*/
public:

	//初始化
	void Init();

	//半初始化
	void HalfInit();

	//为某个玩家手上的牌排序
	void SortCardsTo(BYTE bDeskStation);

/*----------------------------------------------------------------------------------------------------------------
	一系列游戏客户端更新的接口，每个游戏都不同
  ---------------------------------------------------------------------------------------------------------------*/

	//服务器自主配置桌子后数据更新
	void SaveSysDeskCfg(const LPCVOID lpData);

	//服务器送来分配玩家坐位,数据更新
	void SaveAssignDirectInfo(const LPCVOID lpData);

	//来自服务器定庄家的消息，更新数据
	void SaveNTInfo(const LPCVOID lpData);

	//服务器发来起牌的方法，更新数据
	void SaveFetchMothod(const LPCVOID lpData);

	//来自服务器的发手牌数据，进行更新
	void SaveHandCards(const LPCVOID lpData, BYTE nDeskStation);

	//来自服务器的跳牌数据，进行更新
	void SaveTiaoCards(const LPCVOID lpData, BYTE nDeskStation);

	//收服务器的为各玩家的补花数据，进行更新
	void SaveBuHuas(const LPCVOID lpData, BYTE nDeskStation);

	//服务器下发令牌，更新数据
	void SaveTokenInfo(const LPCVOID lpData);

	//某玩家出牌了,更新数据
	void SaveOutCardInfo(const LPCVOID lpData, BYTE nDeskStation);

	//我有动作，更新动作
	void SaveActNotify(const LPCVOID lpData, BYTE nDeskStation);

	//有玩家执行动作，更新数据
	void SaveActInfo(const LPCVOID lpData, BYTE nDeskStation);

	//有玩家暗杠，要即时更新分数，更新数据
	void SaveAnGangResult(const LPCVOID lpData);

	//收到回合结束消息动作，更新数据，这时各玩家之前保密的数据已发下来
	void SaveRoundFinish(const LPCVOID lpData);

	//收到结算消息后，更新数据
	void SaveRoundResult(const LPCVOID lpData);

/*-游戏状态现场恢复处理-*/
public:

	//恢复基本的游戏数据
	void RecurSaveBase(const LPCVOID lpData, BYTE nDeskStation);

	//恢复打牌时的游戏数据
	void RecurSavePlaying(const LPCVOID lpData, BYTE nDeskStation);

/*----------------------------------------------------------------------------------------------------------------
	数据定义
  ---------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------
	温州二人麻将 特有规则定义
------------------------------------------------------------------------------------------------------------------*/
	
	bool WzFengZiLocked(int nDeskStation, int iCaiShen);
	
	bool isWzFengZi(int iCard);

	bool WzFengZiInside(int nDeskStation, int iCaiShen, int iFengZi);

	bool isWzFengZiEx(int iCard);

/*-适用于所有游戏-*/
public:

	//用户数据
	CUserData m_UserData[PLAY_COUNT];

	//配置数据
	TDeskCfg m_tagDeskCfg;

/*-框架相关-*/
private:

/*-适用用兴化麻将的数据-*/
public:

	//起牌打骰子的点数值
	BYTE m_byFetchDice[2];

	//在这个玩家门起开始起牌
	BYTE m_byFetchUser;

	//起牌的墩数
	int m_iFetchPos;

	//得到令牌的用户，即可活动的用户
	BYTE m_byTokenUser;

	//庄家
	BYTE m_byNTUser;

};


