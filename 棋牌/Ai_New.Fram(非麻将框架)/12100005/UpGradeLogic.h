#pragma once

//#include "StdAfx.h"

//操作掩码
#define	UG_HUA_MASK					0xF0			//1111 0000
#define	UG_VALUE_MASK				0x0F			//0000 1111

//扑克花色
#define UG_FANG_KUAI				0x00			//方块	0000 0000
#define UG_MEI_HUA					0x10			//梅花	0001 0000
#define UG_HONG_TAO					0x20			//红桃	0010 0000
#define UG_HEI_TAO					0x30			//黑桃	0011 0000
#define UG_NT_CARD					0x40			//主牌	0100 0000
#define UG_ERROR_HUA				0xF0			//错误  1111 0000

//扑克出牌类型
#define UG_PERSON_SMALL_FIVE        0x05			//人小五
#define UG_HEAVEN_KING				0x04			//天王牌
#define UG_FIVE_SMALL				0x03			//五小牌
#define UG_HALF_PAST_TEN			0x02			//十点半
#define UG_NORMAL_CARD				0x01			//普通牌
#define UG_BUST_CARD				0x00			 //爆牌

//升级逻辑类 
class CUpGradeGameLogic
{
	//变量定义
private:
	//BYTE					m_iNTNum;							//主牌数字
	//BYTE					m_iNTHuaKind;						//主牌花色
	int						m_iStation[5];						//相对位置（方块，梅花，红桃，黑桃，主牌）

	CPtrList				m_ptrPSList;       //牌列表

	DWORD				    m_iCardShape;	  //支持牌型

	struct PSTypeNode
	{
		int mps,mcount;

		PSTypeNode()
		{
			mps = -1;
			mcount = 0;
		}
	};
	//函数定义
public:
	//构造函数
	CUpGradeGameLogic(void);

	//功能函数（公共函数）
public:

	//获取扑克数字
	inline int GetCardNum(BYTE iCard) { return (iCard&UG_VALUE_MASK)+1; }
	//获取扑克花色
	BYTE GetCardHuaKind(BYTE iCard);
	//排列扑克
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount);
	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
	//获处牌点
	//int GetCardPoint(BYTE Card[], int CardCount, bool split);

	//重新排序
	BOOL ReSortCard(BYTE iCardList[], int iCardCount);
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard);

	//辅助函数
public:

	//得到牌列花色
	BYTE GetCardListHua(BYTE iCardList[], int iCardCount);

	//获取扑克相对大小
	int GetCardBulk(BYTE iCard, BOOL bExtVol = false);
	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);

	//特殊牌型
	BOOL IsSpecial(BYTE iCardList[],int iCardCount);
	//是否黑杰克
	BOOL IsBlackJack(BYTE iCardList[], int iCardCount,bool Split);

public:
	//获取扑克
	BYTE GetCardFromHua(int iHuaKind, int iNum);
	//查找分数
	//int FindPoint(BYTE iCardList[], int iCardCount);

	//服务器专用函数
public:
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount,bool bhaveKing=true);

	//客户端函数
public:
	//自动出牌函数
	BOOL AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut);
	//是否可以出牌
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount);

	//比较手中牌大小
	int CompareCard(BYTE iFirstCardList[], int iFirstCardCount,BYTE iFirstUpCard[], BYTE iSecondCardList[], int iSecondCardCount,BYTE iSecondUpCard[]);
	//比较手中牌大小
	int CompareCard(BYTE iFirstCardList[], int iFirstCardCount, BYTE iSecondCardList[], int iSecondCardCount);

	//得到当前牌型
	BYTE GetShape(BYTE iCardList[],BYTE iCardCount);

	///设置游戏牌型
	void SetCardShape(DWORD iCardShape){m_iCardShape=iCardShape;}

	//内部函数
public:
	///清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);

	///人小五
	bool IsPersonSmallFive(BYTE  iCardList[],int iCardCount);
	///天王
	bool IsHeavenKing(BYTE  iCardList[],int iCardCount);
	///五小
	bool IsFiveSmall(BYTE  iCardList[],int iCardCount);
	/// 十点半
	bool IsHalfPastTen(BYTE  iCardList[],int iCardCount);
	/// 普通牌
	bool IsNormalCard(BYTE  iCardList[],int iCardCount);
	//爆牌
	bool IsBustCard(BYTE  iCardList[],int iCardCount);

	/// 得到牌组中最大的牌
	int GetMaxCard(BYTE  iCardList[],int iCardCount);
	//获取牌的大小
	int GetCardValue(BYTE byCardValue);
	//获取牌的点数
	int GetPoint(BYTE byCardList[],int iCardNum);

};
