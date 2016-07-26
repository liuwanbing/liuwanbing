
// ***************************************************************
//  GameEngine   version:  1.0   ·  date: 02/11/2011
//  -------------------------------------------------------------

//	升级，子状态机，游戏客户端发牌机\
	NameID：50100440

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "StdAfx.h"

#include <afxmt.h>

class CMiniGameEngine;
class CGameMachine;

#define IDT_SEND_CARD_GAP			 220	//发牌时续张显示的时间间隔
#define IDT_SEND_MOVING_GAP			 221	//发牌运动时间的间隔

class CCardSender
{

public:

	//构造析构器
	CCardSender();

	virtual ~CCardSender();

public:

	//根据游戏当前的状态，启动游戏的状态机接口\
	  为什么需要CGameMachine指针？\
	  又因为在发牌机运转过程需要获得牌数据，一些游戏规则设置，所以要用到主状态机指针；\
	  需要控制UI、声效，用到引擎，从CGameMachine指针获取游戏引擎；\
	  同时也是主从状态机的一个关联。
	virtual bool StartUp(CGameMachine* pRootMachine,BYTE byGameStation,void* pGSData);

	//关闭状态机
	virtual bool ShutDown();

public:

	//发每张牌的每个时间间隔处理
	bool OnTimerGap();

	//发每张牌的运动时间间隔处理
	bool OnTimerMovingGap();

public:

	//获取已显示的牌
	void GetShowedCards(BYTE byCard[]);

public:

	//发牌过程重现
	bool OnRecurSending(void* pGSBase);

protected:

	//载入玩家发牌队列数据
	bool LoadSendCardQueue();

private:

	//游戏引擎对象指针，其担任两个角色，第一是UI管理，第二是通讯管理\
	  所以下面代码中有时候称呼其为“视类”，有时候称呼其为“通讯类”
	CMiniGameEngine* m_pGameEngine;

	//游戏主状态机
	CGameMachine* m_pRootMachine;

/*--------------------------------------------------------------------
	数据
  ------------------------------------------------------------------*/
public:

	//已显示的牌
	BYTE m_byShowed[MAX_HANDCARDS_NUM];

	//发牌玩家队列
	BYTE m_SendCardQueue[PLAY_COUNT + 1];

	//发牌进度，由于发牌一逐张发，需要记录当前牌已发到第几张牌
	int m_iSendingPos;

	//记录当前是发手上的第几张牌
	int m_iSendingIndex;

};