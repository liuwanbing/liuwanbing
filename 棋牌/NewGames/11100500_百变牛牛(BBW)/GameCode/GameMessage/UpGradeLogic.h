/******************************************************************************************
*类型说明：牌类游戏逻辑管理类                                                             *
*功能说明：游戏逻辑处理、游戏算法处理。每种不同的游戏都有不同的游戏逻辑，开发者可将       *
*游戏逻辑整合在该类中实现。                                                               *
*创建时间：2009.02.06                                                                     *
*修改时间：                                                                               *
*作者：   qy                                                                              *
*修改者：                                                                                 *
******************************************************************************************/

#pragma once
#include "UI_ForLogic/IUserInterface.h"

// 牌的副数
const BYTE  POKER_SET_NUM  = 1;         /// 扑克牌的副数
const BYTE  POKER_CARD_NUM = 54;        /// 扑克牌的张数
const BYTE  POKER_COUNT_SHAPES = 5;     /// 0(大小王) 1(黑) 2(红) 3(梅) 4(方) 五种花色
const BYTE  POKER_COUNT_CARDNUM=16;     /// 0 A 2 3 ... J Q K 小王 大王——共16种牌(0表示牌背)
const BYTE  POKER_CARD_NONE = 0x00;     /// 无牌
const int   SINGLE_CARD_COUNT = 1;      /// 单张

const HRESULT RET_SUCCESS = 0;          /// 成功
const HRESULT RET_FAILED = -1;          /// 失败
#define SUCCESSED(hRet) (RET_SUCCESS == (HRESULT)hRet)

// 产生一张牌
#define MAKE_A_CARD(byPkSet,byCardShape,byCardNumber)  ((byPkSet&0x0F)|((byCardShape<<4)&0xF0)|(byCardNumber&0x0F))

/// 牌的副数
typedef enum ePokerSetNumber
{
	EPSN_SET_NUM_NONE = 0,		/// 无牌 
	EPSN_SET_NUM_1,			    /// 第一副
	EPSN_SET_NUM_2              /// 第二副
}E_POKER_SET_NUMBER;

// 牌的花色
typedef enum _eCardShape
{
	_ECS_SHAPE_NONE = 0,      /// 大小王或无花色
	_ECS_SHAPE_DIAMONDS=1,    /// 方块
	_ECS_SHAPE_CLUBS=2,       /// 梅花
	_ECS_SHAPE_HEARTS=3,      /// 红桃
	_ECS_SHAPE_SPADE=4        /// 黑桃
}_E_CARD_SHAPE;

 //牌的点数
typedef enum _eCardNumber
{
	_ECN_NUM_NONE = 0,		// 牌背 
	_ECN_NUM_A,			    // A	
	_ECN_NUM_2,
	_ECN_NUM_3,
	_ECN_NUM_4,
	_ECN_NUM_5,
	_ECN_NUM_6,
	_ECN_NUM_7,
	_ECN_NUM_8,
	_ECN_NUM_9,
	_ECN_NUM_10,
	_ECN_NUM_J,
	_ECN_NUM_Q,
	_ECN_NUM_K,
	_ECN_NUM_Joker,	       // 小王
	_ECN_NUM_JOKER	       // 大王
}_E_CARD_NUMBER;

/// 基本牌型
typedef enum eBaseCardsType
{ 
	EBCT_TYPE_NONE = 0,				    /// 无牌型
	//牛牛牌型
	EBCT_TYPE_NO_POINT	= 1000,							///无点牌
	EBCT_TYPE_BULL_ONE,                            ///牛一
	EBCT_TYPE_BULL_TWO,                             ///牛二
	EBCT_TYPE_BULL_THREE,                           ///牛三
	EBCT_TYPE_BULL_FOUR,                            ///牛四
	EBCT_TYPE_BULL_FIVE,                            ///牛五
	EBCT_TYPE_BULL_SIX,                             ///牛六
	EBCT_TYPE_BULL_SEVEN,                           ///牛七
	EBCT_TYPE_BULL_EIGHT,                           ///牛八
	EBCT_TYPE_BULL_NINE,                            ///牛九
    EBCT_TYPE_BULL_joker,                           ///小王牛牛
	EBCT_TYPE_BULL_JOKER,                           /// 大王牛牛
	EBCT_TYPE_BULL_BULL							///牛牛
	// ……可以扩展其他牌型
}E_BASE_CARDS_TYPE;

/// 比较结果
typedef enum eCompareResult
{
	ECR_OTHER = -2,         /// 无法比较
	ECR_LESS  = -1,         /// 小于
	ECR_EQUAL = 0,          /// 等于
	ECR_MORE = 1            /// 大于
}E_COMPARE_RESULT;

/// 比较结果
typedef enum eCardSortType
{
	ECST_LEVEL   = 0,       /// 权值排序
	ECST_NUMBER  = 1,       /// 牌点排序
	ECST_CARDCOUN= 2,       /// 相同牌点的张数排序
	ECST_SHAPE   = 3        /// 花色排序
}E_CARD_SORT_TYPE;

/// 牌类游戏逻辑
class CPokerGameLogic
{
public:
	CPokerGameLogic();
	virtual ~CPokerGameLogic();

private:
	bool  m_bCompareKind ; 

public:																									

	/********************************************************************************
	* 函数名: static char Card2Char(char shape,char number)
	* 功能;   将给定第几副、花色、牌点的牌转换为字符 公式：
	*         (((byPkSet<<8)&0xFF00)|((byCardShape<<4)&0x00F0)|(byCardNumber&0x000F))
	* @param: BYTE byPkSet [in] 为第几副牌
	* @param: BYTE byShape [in] 给定的花色 
	* @param: BYTE byNumber[in] 给定的牌点
	* @param: 转换后的结果 
	********************************************************************************/
	static BYTE CardToWord(BYTE byPkSet, BYTE byShape, BYTE byNumber);

	/********************************************************************************
	* 函数名: static bool WordToCard(WORD wInCard, BYTE &byPkSet, BYTE &byShape, BYTE &byNumber)
	* 功能;   将给定的牌转换为牌的第几副、花色和牌点
	* @param: WORD wInCard  [in]  输入需要转换的牌
	* @param: BYTE &byPkSet [out] 为第几副牌
	* @param: BYTE &byShape [out] 给定的花色 
	* @param: BYTE &byNumber[out] 给定的牌点 
	* 返回: bool 转换成功true，失败false 
	********************************************************************************/
	static bool WordToCard(BYTE wInCard, BYTE &byPkSet, BYTE &byShape, BYTE &byNumber);

	/********************************************************************************
	* 函数名: static BYTE GetCardSet(WORD wCard)
	* 功能;   获取牌为第几副牌中的牌
	* 参数: WORD wCard  [in]  输入牌
	* 返回: BYTE 第几副牌
	********************************************************************************/
	static BYTE GetCardSet(BYTE wCard);

	/********************************************************************************
	* 函数名: static BYTE GetCardShape(WORD wCard)
	* 功能;   获取牌的花色
	* 参数: WORD wCard  [in]  输入牌
	* 返回: BYTE 牌的花色
	********************************************************************************/
	static BYTE GetCardShape(BYTE wCard);

	/********************************************************************************
	* 函数名: static BYTE GetCardNumber(WORD wCard)
	* 功能;   返回牌点
	* 参数: WORD wCard  [in]  输入牌
	* 返回: BYTE 牌点
	********************************************************************************/
	static BYTE GetCardNumber(BYTE wCard);


	/********************************************************************************
	* 函数名: static BYTE GetCardNumber(WORD wCard)
	* 功能;   返回牌值(花色<<4|牌点)
	* 参数: WORD wCard  [in]  输入牌
	* 返回: BYTE 牌值
	********************************************************************************/
	//static BYTE GetCardValue(BYTE wCard);

	/********************************************************************************
	* 函数名:static bool IsAvailableCard(WORD wCard)
	* 功能;   检验牌是否为有效
	* 参数: WORD wCard [in]  牌(第几副、花色与牌点组合)
	* 返回: bool 合法的牌，返回true，否则返回false
	********************************************************************************/
	static bool IsAvailableCard(BYTE wCard);

	/********************************************************************************
	* 函数名:static bool IsAvailableCardValue(BYTE byCardValue)
	* 功能;   检验牌值是否为有效
	* 参数: BYTE byCardValue [in]  牌值(花色与牌点组合)
	* 返回: bool 合法的牌值，返回true，否则返回false
	********************************************************************************/
	static bool IsAvailableCardValue(BYTE byCardValue);

	/********************************************************************************
	* 函数名:static bool IsAvailableCardSet(BYTE byCardSet)
	* 功能;   检验牌副是否为有效
	* 参数: BYTE byCardSet [in]  第几副
	* 返回: bool 合法的牌，返回true，否则返回false
	********************************************************************************/
	static bool IsAvailableCardSet(BYTE byCardSet);

	/********************************************************************************
	* 函数名: static bool IsAvailableCardShape(BYTE byCardShape)
	* 功能;   检验牌花色是否为有效
	* 参数: BYTE byCardShape [in]  牌花色
	* 返回: bool 合法的牌花色，返回true，否则返回false
	********************************************************************************/
	static bool IsAvailableCardShape(BYTE byCardShape);

	/********************************************************************************
	* 函数名: static bool IsAvailableCardNumber(BYTE byCardNumber)
	* 功能;   检验牌点是否为有效
	* 参数: BYTE byCardNumber [in]  牌点
	* 返回: bool 合法的牌点，返回true，否则返回false
	********************************************************************************/
	static bool IsAvailableCardNumber(BYTE byCardNumber);	
	
	/********************************************************************************
	* 函数名: int RemoveCard(WORD wRemoveCard[], int iRemoveCount, WORD wCardList[], int iCardCount)
	* 功能;   删除扑克
	* 参数: WORD wRemoveCard[]  [in]    需要删除的扑克
	*       int iRemoveCount  [in]      删除牌数组大小
	*       BYTE wCardList[]  [in/out]  从该数值中删除扑克
	*       int iCardCount    [in]      删除牌原数组的大小  
	* 返回: int 删除牌的数量
	********************************************************************************/
	int RemoveCard(BYTE wRemoveCard[], int iRemoveCount, BYTE wCardList[], int iCardCount);

	/********************************************************************************
	* 函数名: static int CardCompare(WORD wCardFirst, WORD wCardSecond)
	* 功能;   两张牌进行比较
	* 参数: WORD wCardFirst  [in]   第一张牌
	*       WORD wCardSecond [in]   第二张牌 
	* 返回: int 返回E_COMPARE_RESULT 第一张牌<第二张 ECR_LESS
	********************************************************************************/
	static int CardCompare(BYTE wCardFirst, BYTE wCardSecond);

	/********************************************************************************
	* 函数名: static HRESULT RandCard(WORD wCardList[], int iCardCount)
	* 功能;   洗牌
	* 参数: WORD wCardList[]  [out]     洗好的牌
	*       int iCardCount    [in]      总共张数
	* 返回: HRESULT 0 成功，其他失败
	********************************************************************************/
	static BYTE	RandCard(BYTE iCard[], int iCardCount,bool bhaveKing=true);

	/********************************************************************************
	* 函数名: static HRESULT FindCardIndex(int &iIndex, WORD wFindCard, WORD wCardList[], int iCardCount);
	* 功能;   查找某在牌在队列中的索引值
	* 参数: int  &iIndex      [out]  查找到的索引值 (未查找到为-1)
	*       WORD wFindCard    [in]   需要查找的牌
	*       WORD wCardList[]  [in]   牌队列
	*       int iCardCount    [in]   总共张数
	* 返回: HRESULT 0 成功，其他失败
	********************************************************************************/
	static HRESULT FindCardIndex(int &iIndex, BYTE wFindCard, BYTE wCardList[], int iCardCount);

	


public:
	/********************************************************************************
	* 函数名: BYTE GetCardLevel(WORD wCard)
	* 功能;   返回牌的权值
	* 参数: WORD wCard  [in]  输入牌
	* 返回: BYTE 牌的权值
	********************************************************************************/
	 BYTE GetCardLevel(BYTE wCard);

	/********************************************************************************
	* 函数名: bool SortByLevel(WORD wCardList[], int iCardCount,bool bAscending=false)
	* 功能;   权值排序
	* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
	*       int iCardCount    [in]      牌数组大小
	*       bool bAscending   [in]      true升序，false降序
	* 返回: bool 排列是否成功 true 成功  false 失败
	********************************************************************************/
	bool SortByLevel(BYTE wCardList[], int iCardCount,bool bAscending=false);

	/********************************************************************************
	* 函数名: bool SortByNumber(WORD wCardList[], int iCardCount,bool bAscending=false)
	* 功能;   牌点排序
	* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
	*       int iCardCount    [in]      牌数组大小
	*       bool bAscending   [in]      true升序，false降序
	* 返回: bool 排列是否成功 true 成功  false 失败
	********************************************************************************/
	bool SortByNumber(BYTE wCardList[], int iCardCount,bool bAscending=false);

	/********************************************************************************
	* 函数名: bool SortByShape(WORD wCardList[], int iCardCount,bool bAscending=false)
	* 功能;   牌的花色排序
	* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
	*       int iCardCount    [in]      牌数组大小
	*       bool bAscending   [in]      true升序，false降序
	* 返回: bool 排列是否成功 true 成功  false 失败
	********************************************************************************/
	bool SortByShape(BYTE wCardList[], int iCardCount,bool bAscending=false);

	/********************************************************************************
	* 函数名: bool SortByCardCount(WORD wCardList[], int iCardCount,bool bAscending=false)
	* 功能;   牌点的张数排序
	* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
	*       int iCardCount    [in]      牌数组大小
	*       bool bAscending   [in]      true升序，false降序
	* 返回: bool 排列是否成功 true 成功  false 失败
	********************************************************************************/
	bool SortByCardCount(BYTE wCardList[], int iCardCount,bool bAscending=false);

	/********************************************************************************
	* 函数名: bool SortCards(WORD wCardList[], int iCardCount, int iMod, bool bAscending)
	* 功能;   排列扑克
	* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
	*       int iCardCount    [in]      牌数组大小
	*       int iMod          [in]      排序模式 0牌值大小、1牌数量、2牌的花色
	*       bool bAscending   [in]      true升序，false降序
	* 返回: bool 排列是否成功 true 成功  false 失败
	********************************************************************************/
	bool SortCards(BYTE wCardList[], int iCardCount, int iMod, bool bAscending=false);

	///清除 0 位扑克
	int RemoveNummCard(BYTE wCardList[], int iCardCount);
	///得到牌点值牛牛中的点
	int GetPoint(BYTE wCard);
// 	得到牌值
// 	int GetCardPoint(BYTE wCardList[],int iCardCount);

	///得到牌型需三张牌（选出牛）
	//int GetShape(WORD wCardList[],int iCardCount,WORD wUpCard[]);

	///得到牌型，无需三张牌
	int GetShape(BYTE wCardList[],int iCardCount);

	///计算牌中点数
	int CountPoint(BYTE  wCardList[],int iCardCount);

	///统计选出指定张数牌是否可以组成20,10,0如果返回为非0值,表示余下点数和,返回0表示不成立

	int CanSumIn(BYTE wCardList[],int iCardCount,int iSelectNum);

	///是牛牛牌型
	bool IsBullBull(BYTE  wCardList[],int iCardCount);

	///是否为有点
	int IsHaveNote(BYTE  wCardList[],int iCardCount);
	///得到手牌中最大的牌(含花色)
	BYTE GetMaxCard(BYTE  wCardList[],int iCardCount);
    ///是牛
	bool IsBull(BYTE  wCardList[],int iCardCount);
	///自动找牛
	bool GetBull(BYTE wCardList[],int iCardCount,BYTE wBullCard[]);
	///比较两手中牌的大小
	 int CompareCard(BYTE wFirstCard[], int iFirstCount, BYTE wSecondCard[], int iSecondCount);

	 ///得到牌集中非十的最大点，以便计算王代牛
	int GetMaxPoint(BYTE  wCardList[],int iCardCount,BYTE wBullCard[]);

	///计算王个数
	int  CountKing(BYTE  wCardList[],int iCardCount);

	///获取是否比较花色值
	inline void  GetCompareValue(bool  bRet) 
	{
		m_bCompareKind = bRet ; 
	}

	//获取牌的点数
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);

	int GetCardBulk(BYTE iCard, BOOL bExtVol = false);
};