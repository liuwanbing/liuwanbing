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
#define UG_ERROR_KIND				    0				//错误
#define SH_OTHER					    1				//散牌
#define SH_DOUBLE					    2				//对子
#define SH_TWO_DOUBLE				    3				//两对
#define SH_THREE					    4				//三条
#define SH_SMALL_CONTINUE			    5				//最小顺子
#define SH_CONTINUE					    6				//顺子
#define SH_SAME_HUA					    7				//同花
#define SH_HU_LU					    8				//葫芦
#define SH_TIE_ZHI					    9				//4条
#define SH_SMALL_SAME_HUA_CONTINUE	    11              //最小同花顺
#define SH_SAME_HUA_CONTINUE		    10				//同花顺
#define SH_REGIUS_SAME_HUA_CONTINUE		12				//皇家同花顺


//升级逻辑类 
class CUpGradeGameLogic
{
	//变量定义
private:
	int						m_iStation[5];						//相对位置（方块，梅花，红桃，黑桃，主牌）	
	int						SevenCard[4][7];					//将牌分花色放入数组
	CPtrList				m_ptrPSList;//牌列表	

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
	inline int GetCardNum(BYTE iCard) { return (iCard & UG_VALUE_MASK) + 1;}

	//获取扑克花色
	BYTE GetCardHuaKind(BYTE iCard);

	//排列扑克
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount);

	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);

	//重新排序
	BOOL ReSortCard(BYTE iCardList[], int iCardCount);

	//辅助函数
public:
	//是否单牌
	inline BOOL IsOnlyOne(BYTE iCardList[], int iCardCount) { return iCardCount==1; };

	//是否对牌
	BOOL IsDouble(BYTE iCardList[], int iCardCount);

	//是否三条
	BOOL IsThree(BYTE iCardList[], int iCardCount);

	//得到牌列花色
	BYTE GetCardListHua(BYTE iCardList[], int iCardCount);

	//获取牌型
	BYTE GetCardShape(BYTE iCardList[], int iCardCount);

	//获取扑克相对大小
	int GetCardBulk(BYTE iCard, BOOL bExtVol);

	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);

	//是否顺子
	BOOL IsSingleContinue(BYTE iCardList[], int iCardCount);

	//是否最小顺子
	BOOL IsSmallSingleContinue(BYTE iCardList[], int iCardCount);

	//是否两对
	BOOL IsCompleDouble(BYTE iCardList[], int iCardCount);

	//是否同花
	BOOL IsSameHua(BYTE iCardList[], int iCardCount);

	//是否葫芦
	BOOL IsHuLu(BYTE iCardList[], int iCardCount);

	//是否4条
	BOOL IsTieZhi(BYTE iCardList[], int iCardCount);

	//是否同花顺
	BOOL IsSameHuaContinue(BYTE iCardList[], int iCardCount);

	//是否皇家同花顺
	BOOL IsRegiusContinue(BYTE iCardList[], int iCardCount);

	//是否是最小同花顺
	BOOL IsSmallRegiusContinue(BYTE iCardList[], int iCardCount);

	//其他函数
public:
	//获取扑克
	BYTE GetCardFromHua(int iHuaKind, int iNum);

	bool m_bCompareHuaSe;

	//服务器专用函数
public:
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount);

	//客户端函数
public:
	//自动出牌函数
	BOOL AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut);

	//是否可以出牌
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount);

	//比较手中牌大小
	int CompareCard(BYTE iFirstCardList[], int iFirstCardCount, BYTE iSecondCardList[], int iSecondCardCount);

	//内部函数
private:

	//清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);

public:
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int GetSerialBySpecifyCard(BYTE iCardList[], int iStart, int iCardCount,BYTE iCard);

    //从7张牌中分析出5张最大牌型
    int AnalysisCard(BYTE bHandCards[], int nHandCount, BYTE bPublicCards[], int nPublicCount, BYTE bResultCard[]);
	
	// 遍历出机器人手牌加上公共牌得出来的牌型
	int RobotHandCardAnalysis(BYTE byHandCards[], int nHandCount);

	///最大牌值
	int GetMaxCardValue(BYTE byHandCards[], int nHandCount,BYTE byCardType[]);

	///最大相同数字张数
	int  GetMaxSameNum(BYTE byHandCards[], int nHandCount,BYTE byCardType[] ,int &iCardNum);

	///最大相同花色张数
	int  GetMaxSameHua(BYTE byHandCards[], int nHandCount,BYTE byCardType[] ,int &iCardHua);
   
	///最大连续张数
	int  GetMaxContinueNum(BYTE byHandCards[], int nHandCount,BYTE byCardType[] ,int &iStartCardNum);
};
