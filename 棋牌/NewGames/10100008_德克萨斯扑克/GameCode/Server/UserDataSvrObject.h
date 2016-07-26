
// ***************************************************************
//  UserData ServerObject   version:  1.0   ·  date: 03/29/2011
//  -------------------------------------------------------------

//	德克萨斯扑克服务端，玩家游戏数据\
	NameID：51300280

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "../GameMessage/UpgradeMessage.h"

#define UD_VF_CALL					0x01  //可以跟注
#define UD_VF_ADD					0x02  //可以加注
#define UD_VF_BET					0x04  //可以下注
#define UD_VF_FOLD					0x08  //可以弃牌
#define UD_VF_CHECK					0x10  //可以过牌
#define UD_VF_ALLIN					0x20  //可以全下
#define UD_VF_CHECKMANDATE			0x40  //可以过牌托管
#define UD_VF_CALLMANDATE			0x80  //可以跟注托管
#define UD_VF_CALLANYMANDATE		0x100 //可以跟任何注托管
#define UD_VF_PASSABANDONMANDATE	0x200 //可以过牌/弃牌托管

class  CUserDataSvrObject
{

public:

	//初始化数据
	CUserDataSvrObject(void);

	//释放数据
	virtual~CUserDataSvrObject(void);

	//初始化
	void Init();

	//半初始化
	void HalfInit();

public:

	//获得玩家手上的所有牌
	void GetHandCards(BYTE* pCards);

	//获得玩家刚出的牌
	void GetOutCards(BYTE* pCards);

	////设置玩家的动作
	//void MakeVerbs(BYTE byFlag);

public:

	//玩家状态，主要用于判断哪些玩家同意/不同意游戏 
	enum emState
	{
		UNKNOWN = 0,		 //未知状态
		AGREE_DESKCFG = 1,	 //玩家同意游戏
		DISAGREE_DESKCFG = 2,//玩家不同意游戏
	};
	emState m_nState;

	//用户手上的扑克
	BYTE m_byMyCards[MAX_BACKCARDS_NUM];

	//用户手上扑克数目
	int m_iCardsNum;

	//当前用户打出的扑克
	BYTE m_byOutCards[MAX_HANDCARDS_NUM];

	//当前用户打出的扑克数量
	int m_iOutCardsNum;

public:

	//玩家是否已断线
	bool m_bNetCut;

	//玩家是否发玩牌
	bool m_bSendCardFinish;

	// 获取当前桌数玩家是否2人
	int m_iNnt[PLAY_COUNT];

	////用户当前可做的动作标记
	//BYTE m_byVerbFlag;

	// 记录是否已全部确认代入
	bool m_bIn;
};