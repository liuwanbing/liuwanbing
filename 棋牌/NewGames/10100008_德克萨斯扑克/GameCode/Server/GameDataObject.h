
// ***************************************************************
//  GameDataObject   version:  1.0   ·  date: 03/29/2011
//  -------------------------------------------------------------

//	德克萨斯扑克，服务端游戏数据\
	NameID：德克萨斯扑克

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "UserDataSvrObject.h"
#include "../GameMessage/UpgradeMessage.h"
#include "../GameMessage/UpGradeLogic.h"

class  CGameDataObject
{

public:

	//初始化数据
	CGameDataObject(void);

	//释放数据
	virtual~CGameDataObject(void);

	//初始化
	void Init();

	//半初始化
	void HalfInit();

/*-------------------------------------------------------------------------------------
    动作相关
  -----------------------------------------------------------------------------------*/
public:

	//设置玩家为未知是否同意底注状态
	void SetUnknowStateTo(BYTE bDeskStation);

	//设置玩家为同意底注状态
	void SetAgreeStateTo(BYTE bDeskStation);

	//设置玩家为不同意底注状态
	void SetDisAgreeStateTo(BYTE bDeskStation);
	
	//为某个玩家发牌
	bool DealCardsTo(BYTE bDeskStation,int iSendNum);

	//清空本轮的所有玩家出牌数据
	void ClearAllUserOutCard();

/*-----------------------------------------------------------------
    获取信息相关
  ---------------------------------------------------------------*/
public:

	//得到有多少个玩家同意游戏（同意底注）
	int GetAgreeNums();

	//得到有多少个玩家已发完牌
	bool IsAllPlayerSendCardFinish();

	//获取活动玩家的数量
	int GetActivePlayerNums();

	//获取下注金币
	__int64 GetBetMoney(BYTE bDeskStation = 255, bool bAll = true);

    //设置玩家的动作
    void MakeVerbs(BYTE byFlag);

	//玩家获得好牌
	void GetGoodCard(BYTE bDeskStation);

/*-----------------------------------------------------------------
    大多数游戏都会有的数据
  ---------------------------------------------------------------*/
public:

	//牌源数组，最大支持四副牌
	BYTE m_byCards[MAX_CARDS_NUM];

	//存放底牌的数组
	BYTE m_byBackCards[PLAY_COUNT][MAX_BACKCARDS_NUM];

	//底牌的数量
	int m_iBackCardNum;

	//发牌位置
	int m_iGetCardPos;

	//用户数据
	CUserDataSvrObject m_UserData[PLAY_COUNT];

	//得到令牌的用户，即可活动的用户
	BYTE m_byTokenUser;

	//庄家
	BYTE m_byNTUser;

	//当前游戏活动的圈数
	int m_nTurnNums;


/*-----------------------------------------------------------------
    德克萨斯扑克游戏特有的数据
  ---------------------------------------------------------------*/
	
	//大盲注
	BYTE m_bBigBlind;

	// 小盲注
	BYTE m_bSmallBlind;
	
	// 下注金币
	__int64 m_nBetMoney[PLAY_COUNT][8];

	// 当前下注最多的玩家
	__int64 m_nMaxBetMoney;

	// 当前下注池
	__int64 m_nBetPool;

    // 记录最后一个全下的人
    BYTE m_bAllBetStation;

	// 记录不可下注
	bool m_bNoBet[PLAY_COUNT];

	// 记录玩家在哪个池中全下的 2011-10-28 duanxiaohui
	bool m_bAllBet[PLAY_COUNT][8];

	// 记录放弃
	bool m_bGiveUp[PLAY_COUNT];

	// 每个玩家可用下注金币
	__int64 m_UserMoney[PLAY_COUNT];

	// 过牌次数
	int m_iCheckCount;

	// 下一个下注玩家
	int m_iBetUser;

	// 桌面上公共牌
	BYTE m_iCards[5];

	// 桌面公共牌数
	int m_iCardsCount;

	// 是否有玩家加注
	bool m_bRaise;
	
	// 记录加注次数
	int m_iRaiseCounts;

	// 记录全下玩家数量
	int m_iAllinUser;

	// 记录每个边池是否有全下次数
	BYTE m_byAllPools[8];
	
	//记录跟注玩家
	int m_iCallCounts;

	// 记录弃牌玩家
	int m_iFoldCount;

	// 一轮中第一个过牌玩家
	BYTE m_bFirstStation;

	// 记录第一个下注玩家
	BYTE m_bBetStation;

    // 记录最后一个加注玩家
    BYTE m_bRaiseStation;

    // 记录下注玩家数据
    int m_iBetCounts;

    // 用户当前可做的动作标记
    BYTE m_byVerbFlag;

    // 每个玩家最终牌型数据
    BYTE m_bResultCards[PLAY_COUNT][5];

	// 所有玩家弃牌结束
	bool m_bGiveUpOver;

	// 自动发牌
	bool m_bAutoSendCard;

    // 新的一轮
    bool m_bNewTurn;

	// 新的一轮
    bool m_bNewTurns;

	// 第一个发牌者
	BYTE m_iDealPeople;	

public:

	//配置数据
	TDeskCfg m_tagDeskCfg;

	//最近的成绩单
	TResult m_tagResult;

private:
	CUpGradeGameLogic m_GamgeLogic;	//游戏逻辑类

};


