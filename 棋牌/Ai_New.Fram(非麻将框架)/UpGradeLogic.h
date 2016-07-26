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
#define SH_THREE					7				//三条
#define SH_SAME_HUA_CONTINUE		6				//同花顺
#ifdef  ZJH_CONTINUE_MORE
#define SH_SAME_HUA					4				//同花
#define SH_CONTINUE					5				//顺子
#else
#define SH_SAME_HUA					5				//同花
#define SH_CONTINUE					4				//顺子
#endif
#define SH_DOUBLE					3				//对子
#define SH_SPECIAL					1				//特殊235
#define SH_OTHER					2				//单牌
#define UG_ERROR_KIND				0				//错误

//===========================================================================================
//扑克出牌类型
#define ZX_DING_HUANG				38				//丁皇		红3+大王	18
#define ZX_TIAN_PAI					37				//天牌		对红Q		17
#define ZX_DI_PAI					36				//地牌		对红2		16
#define ZX_REN_PAI					35				//人牌		对红8		15
#define ZX_HE_PAI					34				//和牌		对红4		14

#define ZX_MEI_SHI					33				//梅十		对黑10	13	//相等
#define ZX_CHANG_SANG				32				//长三		对黑6	12
#define ZX_BAN_DENG					31				//板凳		对黑4	11

#define ZX_HU_TOU					30				//虎头		对黑J	10	//相等
#define ZX_TIAO_SHI					29				//苕十		对红10	9
#define ZX_MAO_MAO					28				//猫猫		对红6	8
#define ZX_KAO_YAO					27				//膏药		对红7	7

#define ZX_DUI_HEI_JIU				26				//对黑九	6			//相等
#define ZX_DUI_HEI_BA				25				//对黑八	5
#define ZX_DUI_HEI_QI				24				//对黑七	4
#define ZX_DUI_HEI_WU				23				//对黑五	3

#define ZX_NAI_GOU					22				//奶狗		2	
#define ZX_TIAN_GANG				21				//天杠		1
#define ZX_DI_GANG					20				//地杠		0
//---------------------------------------------------------------------------
#define ZX_TIAN_GUAN_JIU			19				//天关九
#define ZX_DI_GUAN_JIU				18				//地关九
#define	ZX_DENG_LONG_JIU			17				//灯笼九
#define	ZX_HE_WU_JIU				16				//和五九
#define ZX_BAN_WU_JIU				15				//板五九	此三个牌型大小相同
#define ZX_DING_CHANG_JIU			14				//丁长九
#define ZX_MEI_SHI_JIU				13				//梅十九	
#define ZX_DING_MAO_JIU				12				//丁猫九    10	此三个牌型大小相同
#define ZX_WU_LONG_JIU				11				//乌龙九	10
#define ZX_TIAO_SHI_JIU				10				//苕十九	10
#define ZX_NINE_POINT				9				//九点
#define ZX_EIGHT_POINT				8				//八点
#define ZX_SEVEN_POINT				7				//七点
#define ZX_SIX_POINT				6				//六点
#define ZX_FIVE_POINT				5				//五点
#define ZX_FOUR_POINT				4				//四点
#define ZX_THREE_POINT				3				//三点
#define ZX_TWO_POINT				2				//二点
#define ZX_ONE_POINT				1				//一点
#define ZX_ZERO_POINT				0				//零点

//------------------------------------------------------------------------
#define RESULT_BIG					1				//比较结果大
#define RESULT_EQU					0				//比较结果相等
#define RESULT_SMA					-1				//比较结果相等
		
//===========================================================================================

//升级逻辑类 
class CUpGradeGameLogic
{
	//函数定义
public:
	//构造函数
	CUpGradeGameLogic(void);
	void Msg(const char *lpszFormat, ...)
	{
		va_list pArg;
		va_start(pArg, lpszFormat);
		char szMessage[1024] = { 0 };
#ifdef WIN32
		_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
		vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
		va_end(pArg);
		OutputDebugString(szMessage);
	}
	//功能函数（公共函数）
public:
	inline int GetCardNum(BYTE iCard){ return (iCard&UG_VALUE_MASK)+1; }				   //获取扑克数字
	BYTE GetCardHuaKind(BYTE iCard);													   //获取扑克花色
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount);						   //排列扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);//删除扑克
	BOOL ReSortCard(BYTE iCardList[], int iCardCount);										//重新排序
public:
	int GetCardBulk(BYTE iCard, BOOL bExtVol);												//获取扑克相对大小
public:
	//自动出牌函数
	BOOL AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut);
	//比较手中牌大小
	int CompareCard(BYTE iFirstCardList[], int iFirstCardCount, BYTE iSecondCardList[], int iSecondCardCount);
		int RemoveNummCard(BYTE iCardList[], int iCardCount);						//清除 0 位扑克
		BYTE RandCard(BYTE iCard[], int iCardCount,int iDeskIndex = 0);				//混乱扑克(洗牌)
public:
		bool IsRedThreeAndBigKing(BYTE byCardList[], int iCardCount);				//检测是否是丁皇
		bool IsDoubleRedQ(BYTE byCardList[], int iCardCount);						//检测是否是天牌
		bool IsDoubleRedTwo(BYTE byCardList[], int iCardCount);						//检测是否是地牌
		bool IsDoubleRedEight(BYTE byCardList[], int iCardCount);					//检测是否是人牌
		bool IsDoubleRedFour(BYTE byCardList[], int iCardCount);					//检测是否是和牌
		bool IsDoubleBlackTen(BYTE byCardList[], int iCardCount);					//检测是否是梅十
		bool IsDoubleBlackFour(BYTE byCardList[], int iCardCount);					//检测是否是板凳
		bool IsDoubleBlackSix(BYTE byCardList[], int iCardCount);					//检测是否是长三
		bool IsDoubleBlackJ(BYTE byCardList[], int iCardCount);						//检测是否是虎头
		bool IsDoubleRedTen(BYTE byCardList[], int iCardCount);						//检测是否是苕十
		bool IsDoubleRedSix(BYTE byCardList[], int iCardCount);						//检测是否是猫猫
		bool IsDoubleRedSeven(BYTE byCardList[], int iCardCount);					//检测是否是膏药
		bool IsDoubleBlackNine(BYTE byCardList[], int iCardCount);					//检测是否是对子(黑9)
		bool IsDoubleBlackEight(BYTE byCardList[], int iCardCount);					//检测是否是对子(黑8)
		bool IsDoubleBlackSeven(BYTE byCardList[], int iCardCount);					//检测是否是对子(黑7)
		bool IsDoubleBlackFive(BYTE byCardList[], int iCardCount);					//检测是否是对子(黑5)
		bool IsRedQAdnNine(BYTE byCardList[], int iCardCount);						//检测是否是奶狗
		bool IsRedQAdnEight(BYTE byCardList[], int iCardCount);						//检测是否是天杠
		bool IsRedTwoAndEight(BYTE byCardList[], int iCardCount);					//检测是否是地杠
		bool IsRedQAndSeven(BYTE byCardList[], int iCardCount);						//检测是否是天关九
		bool IsRedTwoAndSeven(BYTE byCardList[], int iCardCount);					//检测是否是地关九
		bool IsRedEightAndJ(BYTE byCardList[], int iCardCount);						//检测是否是灯笼九
		bool IsRedFourAndFive(BYTE byCardList[], int iCardCount);					//检测是否是和五九
		bool IsBlackFourAndFive(BYTE byCardList[], int iCardCount);					//检测是否是板五九
		bool IsRedThreeAndBlackSix(BYTE byCardList[], int iCardCount);				//检测是否是丁长九
		bool IsBlackTenAndNine(BYTE byCardList[], int iCardCount);					//检测是否是梅十九
		bool IsRedThreeAndRedSix(BYTE byCardList[], int iCardCount);				//检测是否是丁猫九
		bool IsBlackEightAndJ(BYTE byCardList[], int iCardCount);					//检测是否是乌龙九
		bool IsRedTenAndNine(BYTE byCardList[], int iCardCount);					//检测是否是苕十九

		bool IsThreeAndBigKingAndDoubleRedQ(BYTE byCardList[], int iCardCount);		//检是否是天皇
		bool IsThreeAndBigKingAndDoubleRedTwo(BYTE byCardList[], int iCardCount);	//检是否是鸭皇
		bool IsRedTenAndBlackTenAndJ(BYTE byCardList[], int iCardCount);			//检是否是三花十
		bool IsRedSixAndBlackSixAndBigKing(BYTE byCardList[], int iCardCount);		//检是否是三花六
//------------------------------------------------------------------------------------------------------
public:
		int  GetCardValue(BYTE byCard);												//读取牌值
		int  GetCardShape(BYTE iCardList[], int iCardCount);						//获取牌的类型
		int  CalculationCardPoint(BYTE byCardList[],int iCardNum);					//计算两张牌的点数
		int  CompareTwoCardHand(BYTE byFirstCardList[],int iFirstCardCount,BYTE bySecondCardList[],int iSecondCardCount);	//比较两份牌的大小
		BYTE TransformCardValue(BYTE byCard);																				//转换成可比的牌大小
		BYTE GetMaxCardValue(BYTE byCardList[],int iCardCount);																//从两张牌中获取最大的牌值

		int GetCmpCardValue(BYTE byCardList[],int iCardCount);						//获取牌组合信息
};