// ***************************************************************
//  GameAlgorithm   version:  1.0   ·  date: 03/29/2011
//  -------------------------------------------------------------

//		德克萨斯扑克，游戏主要算法\
	    NameID：51300280

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

//#include "StdAfx.h"

//操作掩码
#define	CARD_MASK_HUA				0xF0
#define	CARD_MASK_VAL				0x0F

//扑克花色
#define CARD_HUA_FANG				0x00			//方块	0000 0000
#define CARD_HUA_MEI				0x10			//梅花	0001 0000
#define CARD_HUA_TAO				0x20			//红桃	0010 0000
#define CARD_HUA_KUI				0x30			//黑葵	0011 0000
#define CARD_HUA_JOKE				0x40			//王牌	0100 0000
#define CARD_HUA_ERR				0xF0			//错误  1111 0000

#define CARD_VAL(card)	(((card) & CARD_MASK_VAL) + 1)	//取得一张牌的点数
#define CARD_HUA(card)	((card) & CARD_MASK_HUA)		//取得一张牌的花色

class CGameAlgorithm
{

public:

	//构造/析构
	CGameAlgorithm(void);

	virtual ~CGameAlgorithm(void);

/*--------------------------------------------------------------------
	公共函数，适合大多数牌类游戏使用
  ------------------------------------------------------------------*/
public:

	//获取扑克数字
	static BYTE GetCardNum(BYTE byCard);

	//获取扑克花色
	static BYTE GetCardHuaKind(BYTE byCard);

	//获得牌值
	static USHORT GetCardBulk(BYTE byCard,LPCVOID pData = NULL);

	//单牌比较
	static bool CompareCard(BYTE byCard1,BYTE byCard2,LPCVOID pData = NULL);

	//混乱扑克
	static BYTE RandCard(BYTE byCard[],int nNum,int nDeskIndex);

	//在已排好序的牌上再加上一张牌，添加后保持排序
	static void InsertSorted(BYTE byCard[],int iCount,BYTE byNewCard,LPCVOID pData = NULL);

	//排列扑克
	static void SortCard(BYTE byCard[],BYTE bUp[],int iCardCount,LPCVOID pData = NULL);

	//清除0位扑克
	static int RemoveNummCard(BYTE iCardList[], int iCardCount);

	//删除扑克
	static int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);

	//枚举牌型
	static void EnumCardShapes(BYTE byCard[],int iCount,LPCVOID pData = NULL);

	//输入需要弹起来的牌值，获得弹起来的牌的数据，用于SetCard的参数中
	static void GetUpCardList(BYTE byUp[],BYTE byCard[],int iCardNum,BYTE byUpSrc[],int iUpSrcNum);


/*--------------------------------------------------------------------
	升级游戏特有函数
  ------------------------------------------------------------------*/
public:


};

#define GAlg	CGameAlgorithm