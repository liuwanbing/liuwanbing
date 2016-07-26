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
//#define SH_BLACK_JACK               10              //黑杰克
//#define SH_THREE					7				//三条
//#define SH_SAME_HUA_CONTINUE		6				//同花顺
//#define SH_SAME_HUA					5				//同花
//#define SH_CONTINUE					4				//顺子
//#define SH_DOUBLE					3				//对子
//#define SH_SPECIAL					1				//特殊235
//#define SH_OTHER					2				//单牌
//#define UG_ERROR_KIND				0				//错误
//牛牛牌型
#define UG_NO_POINT				  0x00			//无点牌
#define UG_BULL_ONE                 0x01            //牛一
#define UG_BULL_TWO                 0x02            //牛二
#define UG_BULL_THREE               0x03            //牛三
#define UG_BULL_FOUR                0x04            //牛四
#define UG_BULL_FIVE                0x05            //牛五
#define UG_BULL_SIX                 0x06            //牛六
#define UG_BULL_SEVEN               0x07            //牛七
#define UG_BULL_EIGHT               0x08            //牛八
#define UG_BULL_NINE                0x09            //牛九
#define UG_BULL_BULL				0x0A			 //牛牛
#define UG_BULL_SILVER              0X0B            //银牛
#define UG_BULL_GOLD                0X0C            //金牛
#define UG_FIVE_SMALL               0X0D            //五小
#define UG_BULL_BOMB                0X0E            //炸弹
/*#define SH_OTHER					1				//散牌
#define SH_DOUBLE					2				//对子
#define SH_TWO_DOUBLE				3				//两对
#define SH_THREE					4				//三条
#define SH_CONTINUE					5				//顺子
#define SH_SAME_HUA					6				//同花
#define SH_HU_LU					7				//葫芦
#define SH_TIE_ZHI					8				//铁支
#define SH_SAME_HUA_CONTINUE		9				//同花顺
*/
//升级逻辑类 
class CUpGradeGameLogic
{
	//变量定义
private:
	//BYTE					m_iNTNum;							//主牌数字
	//BYTE					m_iNTHuaKind;						//主牌花色
	int						m_iStation[5];						//相对位置（方块，梅花，红桃，黑桃，主牌）
	
	//CPtrList				m_ptrPSList;//牌列表

	DWORD					m_iCardShape;	//支持牌型

	BYTE					m_iReplaceCardArray[54];		  /**<代替牌隊列*/

	BYTE					m_iReplaceCardCount;		      /**<代替牌隊例長度*/

	//struct PSTypeNode
	//{
	//	int mps,mcount;

	//	PSTypeNode()
	//	{
	//		mps = -1;
	//		mcount = 0;
	//	}
	//};
	//函数定义
public:
	//构造函数
	CUpGradeGameLogic(void);

	//功能函数（公共函数）
public:
	///設置代替牌隊例
	BOOL SetReplaceCard(BYTE iCardList[],int iCardCount);

	///获取队例中可代替牌张数
	int GetReplaceCardCount(BYTE iCardList[],int iCardCount);

	///获取指定牌个数
	BYTE GetCountBySpecifyCard(BYTE iCardList[],int iCardCount,BYTE bCard);
	
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
	//点
	int GetPoint(int Card);

	//重新排序
	BOOL ReSortCard(BYTE iCardList[], int iCardCount);
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard);

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
	int GetCardBulk(BYTE iCard, BOOL bExtVol = false);
	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);

	//是否三带一
	//BOOL IsThreeAndOne(BYTE iCardList[], int iCardCount);
	//是否顺子
	BOOL IsSingleContinue(BYTE iCardList[], int iCardCount);
//	//是否两对
//	BOOL IsCompleDouble(BYTE iCardList[], int iCardCount);
	//是否同花
	BOOL IsSameHua(BYTE iCardList[], int iCardCount);
//	//是否葫芦
//	BOOL IsHuLu(BYTE iCardList[], int iCardCount);
//	//是否铁支
//	BOOL IsTieZhi(BYTE iCardList[], int iCardCount);
	//是否同花顺
	BOOL IsSameHuaContinue(BYTE iCardList[], int iCardCount);
	//特殊牌型
	BOOL IsSpecial(BYTE iCardList[],int iCardCount);
	//是否黑杰克
	BOOL IsBlackJack(BYTE iCardList[], int iCardCount,bool Split);
	//是否双顺
	//BOOL IsDoubleContinue(BYTE iCardList[], int iCardCount);
	//是否三顺
	//BOOL IsThreeContinue(BYTE iCardList[], int iCardCount);
	//是否飞机带翅膀
	//BOOL IsPlaneAndWing(BYTE iCardList[], int iCardCount);
	//是否四带二
	//BOOL IsFourAndTwo(BYTE iCardList[], int iCardCount);
	//是否炸弹
	//BOOL IsBomb(BYTE iCardList[], int iCardCount);
	//是否火箭
	//BOOL IsRocket(BYTE iCardList[], int iCardCount);
	//是否510K
	//BOOL Is510KBomb(BYTE iCardList[], int iCardCount);
	//是否同花
	//BOOL IsSameHua(BYTE iCardList[], int iCardCount);

	//其他函数
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

	bool IsVariationShape(BYTE  iCardList[],int iCardCount,BYTE iUpCard[],int iUpCount);

	//比较手中牌大小
	int CompareCard(BYTE iFirstCardList[], int iFirstCardCount,BYTE iFirstUpCard[], BYTE iSecondCardList[], int iSecondCardCount,BYTE iSecondUpCard[]);
		//比较手中牌大小
	
	//int CompareCard(BYTE iFirstCardList[], int iFirstCardCount, BYTE iSecondCardList[], int iSecondCardCount);
	
	//获取除升起3张牌的另外两张牌 返回牌张数
	int GetRemainCard(BYTE iCardList[],int iCardCount,BYTE iUpCard[],int iUpCount,BYTE byOtherCard[]);

	//是否可以任取三个和在20,10,0内
	bool CanSumIn(BYTE iUpCard[],int iUpCount=3);

	//得到当前牌型
	int GetShape(BYTE iCardList[],int iCardCount, BYTE iUpCard[]);

	////得到当前牌型
	//int GetShape(BYTE iCardList[],int iCardCount);

	//是牛
	bool IsBull(BYTE iCardList[],int iCardCount);

	//计算牌中的点数
	int CountPoint(BYTE  iCardList[],int iCardCount);

	// 获得牛牌数组
	BOOL GetBull(BYTE iCardList[], int iCardCount, BYTE iCardResult[]);

	///设置游戏牌型
	void SetCardShape(DWORD iCardShape){m_iCardShape=iCardShape;}

	//内部函数
private:
	///清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);

	///牛牛
	bool IsBullBull(BYTE  iCardList[],int iCardCount,BYTE iUpCard[]);

	/// 金牛
	bool IsGoldBull(BYTE  iCardList[],int iCardCount=5);

	/// 银牛
	bool IsSilverBull(BYTE  iCardList[],int iCardCount=5);

	/// 炸弹牛
	bool IsBombBull(BYTE  iCardList[],int iCardCount=5);

	/// 五小
	bool IsFiveSmall(BYTE  iCardList[],int iCardCount=5);

	int IsHaveNote(BYTE  iCardList[],int iCardCount,BYTE iUpCard[],int iUpCount);

	/// 得到牌组中最大的牌
	int GetMaxCard(BYTE  iCardList[],int iCardCount=5);
	
};
