
// ***************************************************************
//  UserData ServerObject   version:  1.0   ·  date: 2/12/2011
//  -------------------------------------------------------------

//	兴化麻将，玩家游戏数据\
	NameID：20001400

//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "comstruct.h"

#include "UpgradeMessage.h"

#include "MjCommonDef.h"
using namespace MjBase;

class CUserData
{

public:

	//初始化数据
	CUserData(void);

	//释放数据
	virtual~CUserData(void);

	//初始化
	void Init();

	//半初始化
	void HalfInit();

/*-获取信息-*/
public:

	//得到玩家定方向的骰子点数和
	BYTE GetDirectPoint();

/*-操作-*/
public:


/*-玩家游戏数据-*/
public:

	//玩家状态，主要用于判断哪些玩家同意/不同意游戏 
	enum emState
	{
		UNKNOWN = 0,		 //未知状态
		AGREEGAME = 3,       //玩家同意游戏
	};
	emState m_nState;

	//玩家定方向时打的两骰子点数值
	BYTE m_byDirectPoint[2];

	//玩家方向，东南西北分别为0,1,2,3。255为未定风向。
	BYTE m_byDirect;

	//玩家是否完成打骰子定方位动画
	bool m_bAssignedDir;

	//玩家是否完成打骰子定起牌方法动画
	bool m_bFetchMethodOK;

	//玩家前方的牌城数据
	BYTE m_byWallCards[2][MJ_MAX_WALLDUN];

	//玩家前方的牌城长度(一定是偶数)
	int m_iWallSize;

	//玩家能否抓牌
	bool m_bCanFetch;

	//玩家从牌城中的抓牌顺序，true为顺时针方向，false为逆时针方向
	bool m_bCWFetch;

	//用户的手牌
	BYTE m_byHandCards[MJ_MAX_HANDCARD];

	//用户手牌数量
	int m_iHandNums;

	//用户的花牌表
	BYTE m_byHuaCards[MJ_MAX_HUA];

	//用户的花牌数量，即花牌表长度
	int m_iHuaNums;

	//用户完成补花动画
	bool m_bBuHuaOK;

	//抓牌次数
	int m_iFetchTimes;

	//出牌锁,当玩家得到信牌后,该锁打开,一但玩家出牌成功后,上锁\
	  设计该变量的目的是避免客户端玩家通过非法手段重复出牌
	bool m_bOutCardLock;

	//已出牌表
	BYTE m_byOutedCards[MJ_MAX_OUTED];

	//已出的牌数
	int m_iOutedNums;

	//玩家的动作信息结构
	MjBase::TMjActInfo m_tagActInfo;

	//拦牌记录
	MjBase::TMjCGPNode m_lstCGP[MJ_MAX_CGP];

	//拦牌数量
	int m_iCGPNums;

	//玩家是否已听牌
	bool m_bIsTing;

	//游戏结果，指一局麻将的结果
	BYTE m_nResult;

	//玩家胡的牌
	BYTE m_byHuCard;

	//玩家胡的牌型表
	TMjActInfo::emHuType m_nHuTypes[MJ_MAX_HUTYPE];

	//玩家胡的牌型表长度
	int m_iHuTypeNums;

	//玩家的特殊牌型(如杠上开花,抢杠胡,杠冲等)
	TMjActInfo::emHuSpecialType m_nHuSpecType;

	//玩家本局的得失点数,作为结算后的附加分数,每一局开始后归0\
	  一般用来保存杠分	  
	int m_iInComePoint;	

/*-兴化麻将特有的玩家数据-*/
public:

	//玩家拥有的豹子数量
	int m_iBaoziNums;

	//每个玩家的分池值
	int m_iPoolPoint;

/*-玩家非游戏数据-*/
public:

	//玩家信息
	const UserInfoStruct* m_pUserData;

	//玩家的性别,为解决早期游戏客户端不能获取玩家性别的问题
	bool m_bBoy;

	//玩家是否已断线
	bool m_bNetCut;

	//玩家身上剩余的钱
	int m_iLeftMoney;

/*-测试数据-*/
public:

	//如果该值有效(不等于255),则抓牌取强制使用此值,\
	该值一次性使用
	BYTE m_byChFetch;
	
};