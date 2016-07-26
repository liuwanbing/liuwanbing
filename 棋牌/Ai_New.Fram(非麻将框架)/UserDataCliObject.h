
// ***************************************************************
//  UserData ServerObject   version:  1.0   ·  date: 03/29/2011
//  -------------------------------------------------------------

//	德克萨斯扑克 服务端，玩家游戏数据\
	NameID：51300280

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

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

#define UD_VF_BEGIN_GAME			0x11  //开始游戏
#define UD_VF_ALTER_STAKE			0x12  //修改带入额

class  CUserDataCliObject
{

public:

	///初始化数据
	CUserDataCliObject(void);

	///释放数据
	virtual~CUserDataCliObject(void);

	/// 初始化
	void Init();

public:

	///获得玩家手上的所有牌
	void GetCards(BYTE* pCards);

public:

	//用户手上的扑克
	BYTE m_byMyCards[MAX_HANDCARDS_NUM];

	//用户手上扑克数目
	int m_iCardsNum;

	//用户当前可做的动作标记
	//BYTE m_byVerbFlag;
};