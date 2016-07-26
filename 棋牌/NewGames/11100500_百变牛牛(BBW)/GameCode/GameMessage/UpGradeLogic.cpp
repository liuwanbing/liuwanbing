#include"stdafx.h"
#include "UpGradeLogic.h"
#include "rand/GenRand.h"
// 构造、析构
CPokerGameLogic::CPokerGameLogic()
{
	m_bCompareKind = true ; 
}

CPokerGameLogic::~CPokerGameLogic()
{

}

/********************************************************************************
* 函数名: static char Card2Char(char shape,char number)
* 功能;   将给定第几副、花色、牌点的牌转换为字符 公式：
*         (((byPkSet<<8)&0xFF00)|((byCardShape<<4)&0x00F0)|(byCardNumber&0x000F))
* 参数: BYTE byPkSet [in] 为第几副牌
*       BYTE byShape [in] 给定的花色 
*	     BYTE byNumber[in] 给定的牌点
* 返回: 转换后的结果 
********************************************************************************/
BYTE CPokerGameLogic::CardToWord(BYTE byPkSet, BYTE byShape, BYTE byNumber)
{
	// 第几副牌
	if (!IsAvailableCardSet(byPkSet))
	{
		return POKER_CARD_NONE;
	}

	// 检查花色
	if (!IsAvailableCardShape(byShape))
	{
		return POKER_CARD_NONE;
	}

	// 检查牌点
	if (!IsAvailableCardNumber(byNumber))
	{
		return POKER_CARD_NONE;
	}

	// 组合成牌
	BYTE wCard = MAKE_A_CARD(byPkSet, byShape, byNumber);
	return wCard;
}

/********************************************************************************
* 函数名: static bool WordToCard(WORD wInCard, BYTE &byPkSet, BYTE &byShape, BYTE &byNumber)
* 功能;   将给定的牌转换为牌的第几副、花色和牌点
* 参数: WORD wInCard  [in]  输入需要转换的牌
*       BYTE &byPkSet [out] 为第几副牌
*       BYTE &byShape [out] 给定的花色 
*	     BYTE &byNumber[out] 给定的牌点 
* 返回: bool 转换成功true，失败false 
********************************************************************************/
bool CPokerGameLogic::WordToCard(BYTE wInCard, BYTE &byPkSet, BYTE &byShape, BYTE &byNumber)
{
	// 获取第几副牌
	//byPkSet = GetCardSet(wInCard);

	// 获取花色
	byShape = GetCardShape(wInCard);

	// 获取牌点
	byNumber = GetCardNumber(wInCard);

	// 牌副数、牌点检查
	if ( _ECN_NUM_NONE == byNumber)
	{
		return false;
	}
	return true;
}

/********************************************************************************
* 函数名: static BYTE GetCardSet(WORD wCard)
* 功能;   获取牌为第几副牌中的牌
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 第几副牌
********************************************************************************/
BYTE CPokerGameLogic::GetCardSet(BYTE wCard)
{
	BYTE bySet = (BYTE)(wCard&0xFF);     // 牌副
	if (IsAvailableCardSet(bySet))
	{
		return bySet;
	}
	return EPSN_SET_NUM_NONE;
}

/********************************************************************************
* 函数名: static BYTE GetCardShape(WORD wCard)
* 功能;   获取牌的花色
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌的花色
********************************************************************************/
BYTE CPokerGameLogic::GetCardShape(BYTE wCard)
{
	BYTE byCardShape = 0x00;  // 牌花色

	if((wCard>>4)&0x0F == 0)
	{
		byCardShape = _ECS_SHAPE_DIAMONDS;
	}
	else if((wCard>>4)&0x0F == 1)
	{
		byCardShape = _ECS_SHAPE_CLUBS;
	}
	else if((wCard>>4)&0x0F == 2)
	{
		byCardShape = _ECS_SHAPE_HEARTS;
	}
	else if((wCard>>4)&0x0F == 3)
	{
		byCardShape = _ECS_SHAPE_SPADE;
	}
	else
	{
		byCardShape = _ECS_SHAPE_NONE;
	}

	if (IsAvailableCardShape(byCardShape))
	{
		return byCardShape;
	}
	return _ECS_SHAPE_NONE;
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CPokerGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	if ((iCard==0x4E)||(iCard==0x4F))
	{
		return bExtVol?(iCard-14):(iCard-64);	//大小鬼64+14-62=16			只返回大小猫的值
	}

	int iCardNum=(iCard&0x0F);
	int iHuaKind=(iCard&0xF0);

	if (iCardNum == 0x0D)	// A为1点,牛牛中特有	
	{
		if(bExtVol)		
			return ((iHuaKind>>4)+(1*4));
		else
			return 1;
	}
	return ((bExtVol)?((iHuaKind>>4)+(iCardNum*4)):(iCardNum));
}

/********************************************************************************
* 函数名: static BYTE GetCardNumber(WORD wCard)
* 功能;   返回牌点
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌点
********************************************************************************/
BYTE CPokerGameLogic::GetCardNumber(BYTE wCard)
{
	BYTE byCardNumber = 0x00;  // 牌点
	if((wCard & 0x0F) == 0x0D)		//A
	{
		byCardNumber = _ECN_NUM_A;
	}
	else if((wCard & 0x0F) == 0x0E)		//小王
	{
		byCardNumber = _ECN_NUM_Joker;
	}
	else if((wCard & 0x0F) == 0x0F)		//大王
	{
		byCardNumber = _ECN_NUM_JOKER;
	}
	else if((wCard&0x0F)==0x0A || (wCard&0x0F)==0x0B || (wCard&0x0F)==0x0C)		//大王
	{
		byCardNumber = _ECN_NUM_10;
	}
	else
	{
		byCardNumber = (wCard&0x0F) + 1;
	}
	
	if (IsAvailableCardNumber(byCardNumber))
	{
		return byCardNumber;
	}
	return _ECN_NUM_NONE;
}


/********************************************************************************
* 函数名: static BYTE GetCardValue(WORD wCard)
* 功能;   返回牌值(花色<<4|牌点)
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌值
********************************************************************************/
// BYTE CPokerGameLogic::GetCardValue(BYTE wCard)
// {
// 	// 检查牌值
// 	if (IsAvailableCardValue(GetCardNumber(wCard)))
// 	{
// 		return byCardValue;
// 	}
// 	return 0;
// }

/********************************************************************************
* 函数名:static bool IsAvailableCard(WORD wCard)
* 功能;   检验牌是否为有效
* 参数: WORD wCard [in]  牌(第几副、花色与牌点组合)
* 返回: bool 合法的牌，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCard(BYTE wCard)
{
	if (IsAvailableCardValue(wCard&0xFF))
	{
		return true;
	}
	return false;
}

/********************************************************************************
* 函数名:static bool IsAvailableCardValue(BYTE byCardValue)
* 功能;   检验牌值是否为有效
* 参数: BYTE byCardValue [in]  牌值(花色与牌点组合)
* 返回: bool 合法的牌值，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCardValue(BYTE byCardValue)
{
	// 检查牌花色
	if (!IsAvailableCardShape((byCardValue>>4)&0x0F))
	{
		return false;
	}
	// 检查牌值
	if (!IsAvailableCardNumber(byCardValue&0x0F))
	{
		return false;
	}
	return true;
}

/********************************************************************************
* 函数名:static bool IsAvailableCardSet(BYTE byCardSet)
* 功能;   检验牌副是否为有效
* 参数: BYTE byCardSet [in]  第几副
* 返回: bool 合法的牌，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCardSet(BYTE byCardSet)
{
	// 检查牌副是否合法
	if ((EPSN_SET_NUM_NONE == byCardSet) || (byCardSet>POKER_SET_NUM))
	{
		return false;
	}
	return true;
}

/********************************************************************************
* 函数名: static bool IsAvailableCardShape(BYTE byCardShape)
* 功能;   检验牌花色是否为有效
* 参数: BYTE byCardShape [in]  牌花色
* 返回: bool 合法的牌花色，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCardShape(BYTE byCardShape)
{
	// 检查牌的花色
	if (byCardShape < POKER_COUNT_SHAPES)
	{
		return true;
	}
	return false;
}

/********************************************************************************
* 函数名:static bool IsAvailableCardNumber(BYTE byCardNumber)
* 功能;   检验牌点是否为有效
* 参数: BYTE byCardNumber [in]  牌点
* 返回: bool 合法的牌点，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCardNumber(BYTE byCardNumber)
{
	if ((byCardNumber < POKER_COUNT_CARDNUM) && (0x00 != byCardNumber))
	{
		return true;
	}
	return false;
}


/********************************************************************************
* 函数名: static int RemoveCard(WORD wRemoveCard[], int iRemoveCount, WORD wCardList[], int iCardCount)
* 功能;   删除扑克
* 参数: WORD wRemoveCard[]  [in]    需要删除的扑克
*       int iRemoveCount  [in]      删除牌数组大小
*       WORD wCardList[]  [in/out]  从该数值中删除扑克
*       int iCardCount    [in]      删除牌原数组的大小  
* 返回: int 删除牌的数量
********************************************************************************/
int CPokerGameLogic::RemoveCard(BYTE wRemoveCard[], int iRemoveCount, BYTE wCardList[], int iCardCount)
{
	//检验数据
	if ((iRemoveCount>iCardCount)) return 0;

	//把要删除的牌置零
	int iDeleteCount=0;
	for (int i=0; i<iRemoveCount; i++)
	{
		for (int j=0; j<iCardCount; j++)
		{
			if (wRemoveCard[i]==wCardList[j])
			{
				iDeleteCount++;
				wCardList[j] = 0;
				break;
			}
		}
	}
	RemoveNummCard(wCardList,iCardCount);
	if (iDeleteCount!=iRemoveCount) return 0;

	return iDeleteCount;
	//return 0;
}

//清除 0 位扑克
int CPokerGameLogic::RemoveNummCard(BYTE wCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0; i<iCardCount; i++)
	{
		if (wCardList[i]!=0) wCardList[i-iRemoveCount]=wCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

/********************************************************************************
* 函数名: static int CardCompare(WORD wCardFirst, WORD wCardSecond)
* 功能;   两张牌进行比较
* 参数: WORD wCardFirst  [in]   第一张牌
*       WORD wCardSecond [in]   第二张牌 
* 返回: int 返回E_COMPARE_RESULT 第一张牌<第二张 ECR_LESS
********************************************************************************/
int CPokerGameLogic::CardCompare(BYTE wCardFirst, BYTE wCardSecond)
{
	// 检查牌是否合法
	if (!IsAvailableCard(wCardFirst) || !IsAvailableCard(wCardSecond))
	{
		return ECR_OTHER;   // 无法比较
	}

	// 去除牌副数进行比较
	BYTE byValueFirst = GetCardNumber(wCardFirst);
	BYTE byValueSecond = GetCardNumber(wCardSecond);
	if (byValueFirst == byValueSecond)
	{
		return ECR_EQUAL;
	}
	else if (byValueFirst < byValueSecond)
	{
		return ECR_LESS;
	}
	else
	{
		return ECR_MORE;
	}
}

/********************************************************************************
* 函数名: static HRESULT RandCard(WORD wCardList[], int iCardCount)
* 功能;   洗牌
* 参数: WORD wCardList[]  [out]     洗好的牌
*       int iCardCount    [in]      总共张数
*       bool bHaveKing    [in]      是否要王
* 返回: HRESULT 0 成功，其他失败
********************************************************************************/
BYTE CPokerGameLogic::RandCard(BYTE iCard[], int iCardCount,bool bhaveKing)
{
	static const BYTE m_CardArray[54]={
		//2     3     4     5     6     7     8     9     10   J      Q    K     A
		0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
		0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
		0x4E, 0x4F};
		//小鬼，大鬼

		BYTE iSend=0,iStation=0,iCardList[162];
		static int temp = 0;
		if (temp > 9999999999)
			temp = 0;
		srand((unsigned)GetCurrentTime()+(temp++));

		if ( bhaveKing)
		{
			for (int i=0;i<iCardCount;i+=54)
				::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
		}
		else
		{
			for (int i=0;i<iCardCount;i+=52)
				::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
		}

		do
		{
			iStation=rand()%(iCardCount-iSend);
			iCard[iSend]=iCardList[iStation];
			iSend++;
			iCardList[iStation]=iCardList[iCardCount-iSend];
		} while (iSend<iCardCount);

		return iCardCount;
}
/********************************************************************************
* 函数名: static HRESULT FindCardIndex(int &iIndex, WORD wFindCard, WORD wCardList[], int iCardCount);
* 功能;   查找某在牌在队列中的索引值
* 参数: int  &iIndex      [out]  查找到的索引值 (未查找到为-1)
*       WORD wFindCard    [in]   需要查找的牌
*       WORD wCardList[]  [in]   牌队列
*       int iCardCount    [in]   总共张数
* 返回: HRESULT 0 成功，其他失败
********************************************************************************/
HRESULT CPokerGameLogic::FindCardIndex(int &iIndex, BYTE wFindCard, BYTE wCardList[], int iCardCount)
{
	iIndex = -1;
	if (!IsAvailableCard(wFindCard))
	{
		return RET_SUCCESS;	
	}

	int i=0;
	for (i=0; i<iCardCount; ++i)
	{
		if (wFindCard == wCardList[i])
		{
			iIndex = i;
			break;  // 找到后退出吧
		}
	}
	return RET_SUCCESS;	
}

/********************************************************************************
* 函数名: BYTE GetCardLevel(WORD wCard)
* 功能;   返回牌的权值
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌的权值
********************************************************************************/
BYTE CPokerGameLogic::GetCardLevel(BYTE wCard)
{
	// 每游戏，牌的权值能不一样，根据需求修改此函数
	if (!IsAvailableCard(wCard))
	{
		return POKER_CARD_NONE;
	}

	// K(13) A(14) 2(16) 小王（18） 大王(20)
	BYTE byCardNumber = GetCardNumber(wCard);

	{
		return byCardNumber;
	}
}

/********************************************************************************
* 函数名: bool SortByLevel(WORD wCardList[], int iCardCount,bool bAscending=false)
* 功能;   权值排序
* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
*       int iCardCount    [in]      牌数组大小
*       bool bAscending   [in]      true升序，false降序
* 返回: bool 排列是否成功 true 成功  false 失败
********************************************************************************/
bool CPokerGameLogic::SortByLevel(BYTE wCardList[], int iCardCount,bool bAscending)
{
	// 一张牌不用排序
	if (iCardCount <= SINGLE_CARD_COUNT)
	{
		return true;
	}

	// 牌是否合法
	int i=0;
	for (i=0; i<iCardCount; ++i)
	{
		if (!IsAvailableCard(wCardList[i]))
		{
			return false;
		}
	}

	// 使用冒泡排序
	int j=0;
	for (i=0; i<iCardCount; ++i)
	{
		for (j=(iCardCount -1); j>i; --j)
		{
			// 比较权值
			BYTE byFirst = GetCardLevel(wCardList[j-1]);
			BYTE bySecond = GetCardLevel(wCardList[j]);
			if (byFirst == bySecond)
			{
				// 比较牌点
				byFirst = GetCardNumber(wCardList[j-1]);
				bySecond = GetCardNumber(wCardList[j]);
				if (byFirst == bySecond)
				{
					// 比较花色
					byFirst = GetCardShape(wCardList[j-1]);
					if (_ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
					{
						byFirst = POKER_COUNT_SHAPES;
					}
					bySecond = GetCardShape(wCardList[j]);
					if (_ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
					{
						bySecond = POKER_COUNT_SHAPES;
					}
					if (byFirst == bySecond)
					{
						continue;
					}
				}
			}

			// 交换位置
			if (((byFirst > bySecond) && (bAscending))    // 第一张大于第二张 要求升序
				||((byFirst < bySecond) && (!bAscending)))// 第一张小于第二张  要求降序
			{
				BYTE wTemp = wCardList[j-1];
				wCardList[j-1] = wCardList[j];
				wCardList[j] = wTemp;
			}
		}
	}
	return true;
}
/********************************************************************************
* 函数名: bool SortByNumber(WORD wCardList[], int iCardCount,bool bAscending=false)
* 功能;   牌点排序
* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
*       int iCardCount    [in]      牌数组大小
*       bool bAscending   [in]      true升序，false降序
* 返回: bool 排列是否成功 true 成功  false 失败
********************************************************************************/
bool CPokerGameLogic::SortByNumber(BYTE wCardList[], int iCardCount,bool bAscending)
{
	// 一张牌不用排序
	if (iCardCount <= SINGLE_CARD_COUNT)
	{
		return true;
	}

	// 牌是否合法
	int i=0;
	for (i=0; i<iCardCount; ++i)
	{
		if (!IsAvailableCard(wCardList[i]))
		{
			return false;
		}
	}

	// 使用冒泡排序
	int j=0;
	for (i=0; i<iCardCount; ++i)
	{
		for (j=(iCardCount -1); j>i; --j)
		{
			// 比较牌点
			BYTE byFirst = GetCardNumber(wCardList[j-1]);
			BYTE bySecond = GetCardNumber(wCardList[j]);
			if (byFirst == bySecond)
			{
				// 比较权值
				byFirst = GetCardLevel(wCardList[j-1]);
				bySecond = GetCardLevel(wCardList[j]);
				if (byFirst == bySecond)
				{
					// 比较花色
					byFirst = GetCardShape(wCardList[j-1]);
					if (_ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
					{
						byFirst = POKER_COUNT_SHAPES;
					}
					bySecond = GetCardShape(wCardList[j]);
					if (_ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
					{
						bySecond = POKER_COUNT_SHAPES;
					}
					if (byFirst == bySecond)
					{
						continue;
					}
				}
			}

			// 交换位置
			if (((byFirst > bySecond) && (bAscending))    // 第一张大于第二张 要求升序
				||((byFirst < bySecond) && (!bAscending)))// 第一张小于第二张  要求降序
			{
				BYTE wTemp = wCardList[j-1];
				wCardList[j-1] = wCardList[j];
				wCardList[j] = wTemp;
			}
		}
	}
	return true;
}
/********************************************************************************
* 函数名: bool SortByShape(WORD wCardList[], int iCardCount,bool bAscending=false)
* 功能;   牌的花色排序
* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
*       int iCardCount    [in]      牌数组大小
*       bool bAscending   [in]      true升序，false降序
* 返回: bool 排列是否成功 true 成功  false 失败
********************************************************************************/
bool CPokerGameLogic::SortByShape(BYTE wCardList[], int iCardCount,bool bAscending)
{
	// 一张牌不用排序
	if (iCardCount <= SINGLE_CARD_COUNT)
	{
		return true;
	}

	// 牌是否合法
	int i=0;
	for (i=0; i<iCardCount; ++i)
	{
		if (!IsAvailableCard(wCardList[i]))
		{
			return false;
		}
	}

	// 使用冒泡排序
	int j=0;
	for (i=0; i<iCardCount; ++i)
	{
		for (j=(iCardCount -1); j>i; --j)
		{
			// 比较花色
			BYTE byFirst = GetCardShape(wCardList[j-1]);
			if (_ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
			{
				byFirst = POKER_COUNT_SHAPES;
			}
			BYTE bySecond = GetCardShape(wCardList[j]);
			if (_ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
			{
				bySecond = POKER_COUNT_SHAPES;
			}
			if (byFirst == bySecond)
			{
				// 比较权值
				byFirst = GetCardLevel(wCardList[j-1]);
				bySecond = GetCardLevel(wCardList[j]);
				if (byFirst == bySecond)
				{
					// 比较牌点
					byFirst = GetCardNumber(wCardList[j-1]);
					bySecond = GetCardNumber(wCardList[j]);
					if (byFirst == bySecond)
					{
						continue;
					}
				}
			}

			// 交换位置
			if (((byFirst > bySecond) && (bAscending))    // 第一张大于第二张 要求升序
				||((byFirst < bySecond) && (!bAscending)))// 第一张小于第二张  要求降序
			{
				BYTE wTemp = wCardList[j-1];
				wCardList[j-1] = wCardList[j];
				wCardList[j] = wTemp;
			}
		}
	}

	return true;
}
/********************************************************************************
* 函数名: bool SortByCardCount(WORD wCardList[], int iCardCount,bool bAscending=false)
* 功能;   牌点的张数排序
* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
*       int iCardCount    [in]      牌数组大小
*       bool bAscending   [in]      true升序，false降序
* 返回: bool 排列是否成功 true 成功  false 失败
********************************************************************************/
bool CPokerGameLogic::SortByCardCount(BYTE wCardList[], int iCardCount,bool bAscending)
{
	// 一张牌不用排序
	if (iCardCount <= SINGLE_CARD_COUNT)
	{
		return true;
	}

	// 计算牌点张牌的张数
	BYTE abyCount[POKER_COUNT_CARDNUM] = {0};
	int i=0;
	for (i=0; i<iCardCount; ++i)
	{
		if (!IsAvailableCard(wCardList[i]))
		{
			return false;
		}
		abyCount[GetCardNumber(wCardList[i])] ++;
	}

	// 使用冒泡排序
	int j=0;
	for (i=0; i<iCardCount; ++i)
	{
		for (j=(iCardCount -1); j>i; --j)
		{
			BYTE byFirst = abyCount[GetCardNumber(wCardList[j-1])];
			BYTE bySecond = abyCount[GetCardNumber(wCardList[j])];
			if (byFirst == bySecond)
			{
				// 比较权值
				byFirst = GetCardLevel(wCardList[j-1]);
				bySecond = GetCardLevel(wCardList[j]);
				if (byFirst == bySecond)
				{
					// 比较牌点
					byFirst = GetCardNumber(wCardList[j-1]);
					bySecond = GetCardNumber(wCardList[j]);
					if (byFirst == bySecond)
					{
						// 比较花色
						byFirst = GetCardShape(wCardList[j-1]);
						if (_ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
						{
							byFirst = POKER_COUNT_SHAPES;
						}
						bySecond = GetCardShape(wCardList[j]);
						if (_ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
						{
							bySecond = POKER_COUNT_SHAPES;
						}
						if (byFirst == bySecond)
						{
							continue;
						}
					}
				}
			}

			// 交换位置
			if (((byFirst > bySecond) && (bAscending))    // 第一张大于第二张 要求升序
				||((byFirst < bySecond) && (!bAscending)))// 第一张小于第二张  要求降序
			{
				BYTE wTemp = wCardList[j-1];
				wCardList[j-1] = wCardList[j];
				wCardList[j] = wTemp;
			}
		}
	}
	return true;
}
/********************************************************************************
* 函数名: bool SortCards(WORD wCardList[], int iCardCount, int iMod, bool bAscending)
* 功能;   排列扑克
* 参数: WORD wCardList[]  [in/out]  排序输入牌、排好序后的输出
*       int iCardCount    [in]      牌数组大小
*       int iMod          [in]      排序模式   0根据权值 1牌点大小、2牌数量、3牌的花色
*       bool bAscending   [in]      true升序，false降序
* 返回: bool 排列是否成功 true 成功  false 失败
********************************************************************************/
bool CPokerGameLogic::SortCards(BYTE wCardList[], int iCardCount, int iMod, bool bAscending)
{
	if ((iMod < ECST_LEVEL) || (iMod > ECST_SHAPE))
	{
		return false;
	}
	// 一张牌不用排序
	if (iCardCount <= SINGLE_CARD_COUNT)
	{
		return true;
	}

	switch (iMod)
	{
	case ECST_LEVEL:            // 权值排序
		{
			return SortByLevel(wCardList, iCardCount, bAscending);
		}
	case ECST_NUMBER:          // 牌点排序
		{
			return SortByNumber(wCardList, iCardCount, bAscending);
		}
	case ECST_CARDCOUN:        // 牌点张数排序
		{
			return SortByCardCount(wCardList, iCardCount, bAscending);
		}
	case ECST_SHAPE:           // 花色排序
		{
			return SortByShape(wCardList, iCardCount, bAscending);
		}
	default:
		break;
	}
	return false;
}
int CPokerGameLogic::GetPoint(BYTE wCard)
{
	if (wCard == 0x00)
		return 0;
	switch (GetCardNumber(wCard))
	{
	case _ECN_NUM_10:
		return 10;
	case _ECN_NUM_J:
		return 10;
	case _ECN_NUM_Q:
		return 10;
	case _ECN_NUM_K:
		return 10;
	case _ECN_NUM_A:
		return 1;
	case _ECN_NUM_Joker:
		return 0x0E;
	case _ECN_NUM_JOKER:
		return 0x0F;
	default:
		return GetCardNumber(wCard);
	}
}
// int CPokerGameLogic::GetCardPoint(BYTE wCardList[],int iCardCount)
// {
// 
// 	return 0;
// }

///得到当前牌型
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  返回牌型
int CPokerGameLogic::GetShape(BYTE wCardList[],int iCardCount)
{
	
// 	if (CanSumIn(wCardList, iCardCount, 3)>0 && GetCardNumber(GetMaxCard(wCardList,iCardCount)) == ECN_NUM_Joker)
// 		return EBCT_TYPE_BULL_joker;
// 	if (CanSumIn(wCardList, iCardCount, 3)>0 && GetCardNumber(GetMaxCard(wCardList,iCardCount)) == ECN_NUM_JOKER)
// 		return EBCT_TYPE_BULL_JOKER;
	if (2 == CountKing(wCardList,iCardCount)) //含有两个王
		return EBCT_TYPE_BULL_JOKER;
	if (1 == CountKing(wCardList,iCardCount))//王代牛（含有一个王）
	{
		BYTE Temp[5] = {0};
		switch(GetMaxPoint(wCardList,iCardCount,Temp))//含有一张王的牌中找出其中最大点的两张牌
		{
		case 0:
			if (GetCardNumber(GetMaxCard(wCardList,iCardCount)) == _ECN_NUM_Joker)
				return EBCT_TYPE_BULL_joker;
			if (GetCardNumber(GetMaxCard(wCardList,iCardCount)) == _ECN_NUM_JOKER)
				return EBCT_TYPE_BULL_JOKER;
		case 1:
			return EBCT_TYPE_BULL_ONE;
		case 2:
			return EBCT_TYPE_BULL_TWO;
		case 3:
			return EBCT_TYPE_BULL_THREE;
		case 4:
			return EBCT_TYPE_BULL_FOUR;
		case 5:
			return EBCT_TYPE_BULL_FIVE;
		case 6:
			return EBCT_TYPE_BULL_SIX;
		case 7:
			return EBCT_TYPE_BULL_SEVEN;
		case 8:
			return EBCT_TYPE_BULL_EIGHT;
		case 9:
			return EBCT_TYPE_BULL_NINE;
		}
	}
	if(IsBullBull(wCardList,iCardCount)) 
		return EBCT_TYPE_BULL_BULL;
	switch(IsHaveNote(wCardList,iCardCount))
	{
	case 0:
		return EBCT_TYPE_NO_POINT;
	case 1:
		return EBCT_TYPE_BULL_ONE;
	case 2:
		return EBCT_TYPE_BULL_TWO;
	case 3:
		return EBCT_TYPE_BULL_THREE;
	case 4:
		return EBCT_TYPE_BULL_FOUR;
	case 5:
		return EBCT_TYPE_BULL_FIVE;
	case 6:
		return EBCT_TYPE_BULL_SIX;
	case 7:
		return EBCT_TYPE_BULL_SEVEN;
	case 8:
		return EBCT_TYPE_BULL_EIGHT;
	case 9:
		return EBCT_TYPE_BULL_NINE;
	case 10:
		return EBCT_TYPE_BULL_BULL;
	}
	
	return EBCT_TYPE_NO_POINT;
}

///比较两手中牌的大小
///@param: WORD wFirstCard[] [in] 第一手牌集
///@param: int  iFirstCount  [in] 第一手牌张数
///@param: WORD wSecondCard[] [in] 第二手牌集
///@param: int  iSecondCount  [in] 第二手牌张数
///@return: int [out]  是否大过
int CPokerGameLogic::CompareCard(BYTE wFirstCard[], int iFirstCount, BYTE wSecondCard[], int iSecondCount)
{
	BYTE wFirCard[5],wSecCard[5];

	::CopyMemory(wFirCard,wFirstCard,sizeof(BYTE)*iFirstCount);
	::CopyMemory(wSecCard,wSecondCard,sizeof(BYTE)*iSecondCount);


	SortByNumber(wFirCard,iFirstCount);
	SortByNumber(wSecCard,iSecondCount);	

 	BYTE iFirstCardShape = GetShape(wFirCard,iFirstCount);
 	BYTE iSecondCardShape = GetShape(wSecCard,iSecondCount);

	BYTE MaxFir = GetMaxCard(wFirCard, iFirstCount);
	BYTE MaxSec = GetMaxCard(wSecCard, iSecondCount);

 	if(iFirstCardShape != iSecondCardShape)  //比较牌型
 	{		
 		return (iFirstCardShape - iSecondCardShape > 0) ? 1 : -1;
 	}
 	else if(MaxFir != MaxSec)						//比较牌值大小
 	{	
		if (GetCardBulk(MaxFir) >=_ECN_NUM_Joker && GetCardBulk(MaxSec)>=_ECN_NUM_Joker)
		{
			return (GetCardBulk(MaxFir) - GetCardBulk(MaxSec) > 0) ? 1 : -1;
		}
		if (GetCardBulk(MaxFir) >=_ECN_NUM_Joker || GetCardBulk(MaxSec)>=_ECN_NUM_Joker)
		{
			return GetCardBulk(MaxFir) >= _ECN_NUM_Joker ? -1 : 1;
		}	
		if (GetCardBulk(MaxFir) != GetCardBulk(MaxSec)) //都是牛牛就比较最大牌
		{
			return (GetCardBulk(MaxFir) - GetCardBulk(MaxSec) > 0) ? 1 : -1;
		}
		else
		{
			if(m_bCompareKind)
			{
				BYTE byFirsCardColor = (MaxFir&0xF0);
				BYTE bySecCardColor = (MaxSec&0xF0);
				return (byFirsCardColor - bySecCardColor > 0) ? 1 : -1 ; 
			}
			else
			{
				return -1 ; 
			}
		}
	}	
	return 0;
}
///计算牌中点数
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  返回牌点数
int CPokerGameLogic::CountPoint(BYTE  wCardList[],int iCardCount)
{
	int point = 0;
	for (int i=0;i<iCardCount;i++)
	{
		int temp = GetCardNumber(wCardList[i]);
		point += temp;
	}
	return point;
}
///统计选出指定张数牌是否可以组成20,10,0如果返回为非0值,表示余下点数和,返回0表示不成立
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  返回牌点数

int CPokerGameLogic::CanSumIn(BYTE wCardList[],int iCardCount,int iSelectNum)
{
	int total = CountPoint(wCardList,iCardCount);
	for (int i=0; i<3; i++)
	{
		if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker
			|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
		{
			continue;
		}
		for (int j=i+1; j<4; j++)
		{
			if (GetCardNumber(wCardList[j]) == _ECN_NUM_Joker
				|| GetCardNumber(wCardList[j]) == _ECN_NUM_JOKER)
			{
				continue;
			}
			for (int k = j+1; k<iCardCount; k++)
			{
				if (GetCardNumber(wCardList[k]) == _ECN_NUM_Joker
					|| GetCardNumber(wCardList[k]) == _ECN_NUM_JOKER)
				{
					continue;
				}

				int temp = GetCardNumber(wCardList[i])+GetCardNumber(wCardList[j])+GetCardNumber(wCardList[k]);
				if (temp==30||temp==10||temp==20)
				{
					return total-temp;
				}
			}
		}
	}
	return -1;
}

///自动找牛
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@param: WORD wBullCard[] [out] 找到的牛牌
///@return: int [out]  返回牌点数
bool CPokerGameLogic::GetBull(BYTE wCardList[],int iCardCount,BYTE wBullCard[])
{
	int total = CountPoint(wCardList,iCardCount);
	for (int i=0; i<3; i++)
	{
		if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker
			|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
		{
			continue;
		}
		for (int j=i+1; j<4; j++)
		{
			if (GetCardNumber(wCardList[j]) == _ECN_NUM_Joker
				|| GetCardNumber(wCardList[j]) == _ECN_NUM_JOKER)
			{
				continue;
			}
			for (int k = j+1; k<iCardCount; k++)
			{
				if (GetCardNumber(wCardList[k]) == _ECN_NUM_Joker
					|| GetCardNumber(wCardList[k]) == _ECN_NUM_JOKER)
				{
					continue;
				}
				int temp = GetCardNumber(wCardList[i])+GetCardNumber(wCardList[j])+GetCardNumber(wCardList[k]);
				if (temp==30||temp==10||temp==20)
				{
					wBullCard[0]=wCardList[i];
					wBullCard[1]=wCardList[j];
					wBullCard[2]=wCardList[k];
					return true;
				}
			}
		}
	}
	if (2 == CountKing(wCardList,iCardCount)) //双王一定是牛牛
	{
		int index = 0;
		int kingCount = 0;
		for (int k=0; k<5; k++)
		{
			if (GetCardNumber(wCardList[k]) == _ECN_NUM_Joker
				&& kingCount == 0)
			{
				kingCount++;
				wBullCard[index++] = wCardList[k];
			}
			if (2 == index && 0 == kingCount)
			{
				continue;
			}
			else
			{
				if (GetCardNumber(wCardList[k]) != _ECN_NUM_Joker
					&& GetCardNumber(wCardList[k]) != _ECN_NUM_JOKER)
				{
					wBullCard[index++] = wCardList[k];
				}
			}
			if (index == 3 && kingCount == 1)
			{
				return true;
			}
		}
	}
	if (1 == CountKing(wCardList,iCardCount))//王代牛
	{
		int iMaxPoint = 0;
		int iTempPoint= 0;
		int index = 0;
		for (int i=0; i<4; i++)
		{
			if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker
				|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
			{
				continue;
			}
			for (int j=i+1; j<5; j++)
			{
				if (GetCardNumber(wCardList[j]) == _ECN_NUM_Joker
					|| GetCardNumber(wCardList[j]) == _ECN_NUM_JOKER)
				{
					continue;
				}
				iTempPoint = GetCardNumber(wCardList[j]) + GetCardNumber(wCardList[i]);
				if (iTempPoint%10 == 0)
				{
					iMaxPoint = iTempPoint%10;
					index = 0;
					for (int k=0; k<5; k++)
					{
						if (k!=i && k!=j)
						{
							wBullCard[index++] = wCardList[k];
						}
						if (index == 3)
						{
							return true;
						}						
					}
				}
				else
					if (iTempPoint%10 > iMaxPoint)
					{
						iMaxPoint = iTempPoint%10;
						index = 0;
						for (int k=0; k<5; k++)
						{
							if (k!=i && k!=j)
							{
								wBullCard[index++] = wCardList[k];
							}
							if (index == 3)
							{
								break;
							}
						}
					}
			}
		}
		if (index == 3)
		{
			return true;
		}
	}
	return false;
}


///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: bool [out]  返回是否牛
bool CPokerGameLogic::IsBullBull(BYTE  wCardList[],int iCardCount)
{
	int total=CountPoint(wCardList,iCardCount);
	if(CanSumIn(wCardList,iCardCount,3)==-1)
		return false;
	if((total > 0) && (total%10 == 0) && (CanSumIn(wCardList,iCardCount,3)!=-1))
		return true;
	return false;
}

///得到牌集中最大的两张牌相加组合，以便计算王代牛
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@param: WORD wBullCard[] [out] 返回牌集
///@return: int [out]  无意义
int CPokerGameLogic::GetMaxPoint(BYTE  wCardList[],int iCardCount,BYTE wBullCard[])
{
	int iMaxPoint = 0;
	int iTempPoint= 0;

	bool bFlag = false;
	int iOne = 0xff;
	int iTwo = 0xff;
	int iThree = 0xff;
	for(int i= 0x00;i < 0x03;i ++)
	{
		if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
		{
			continue;
		}
		for(int j = i + 0x01;j < 0x04;j ++)
		{
			if (GetCardNumber(wCardList[j]) == _ECN_NUM_Joker|| GetCardNumber(wCardList[j]) == _ECN_NUM_JOKER)
			{
				continue;
			}
			for(int k = j + 1;k < 0x05; k ++)
			{
				if (GetCardNumber(wCardList[k]) == _ECN_NUM_Joker|| GetCardNumber(wCardList[k]) == _ECN_NUM_JOKER)
				{
					continue;
				}
				int sum = wCardList[i] + wCardList[j] + wCardList[k];
				if(sum % 10 == 0x00)
				{
					iOne = i;
					iTwo = j;
					iThree = k;
					bFlag = true;
					break;
				}
			}
			if(bFlag)
			{
				break;
			}
		}
		if(bFlag)
		{
			break;
		}
	}
	if(bFlag)
	{
		int j = 0x00;
		for(int i = 0x00;i < 0x05;i ++)
		{
			if(i != iOne || i != iTwo || i != iThree)
			{
				wBullCard[j ++] = wCardList[i];
			}
		}
		return 0x00;
	}

	for (int i=0; i<4; i++)
	{
		if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker
			|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
		{
			continue;
		}
		for (int j=i+1; j<5; j++)
		{
			if (GetCardNumber(wCardList[j]) == _ECN_NUM_Joker
				|| GetCardNumber(wCardList[j]) == _ECN_NUM_JOKER)
			{
				continue;
			}
			iTempPoint = GetCardNumber(wCardList[j]) + GetCardNumber(wCardList[i]);
			if (iTempPoint%10 == 0)
			{
				iMaxPoint = iTempPoint%10;
				wBullCard[0] = wCardList[i];
                wBullCard[1] = wCardList[j];
				return 0x00;
			}
			else if (iTempPoint%10 > iMaxPoint)
			{
				iMaxPoint = iTempPoint%10;
				wBullCard[0] = wCardList[i];
                wBullCard[1] = wCardList[j];
			}
		}
	}
	return iMaxPoint;
}
///是否为有点
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  返回点数
int CPokerGameLogic::IsHaveNote(BYTE  wCardList[],int iCardCount)
{
	int Note = CanSumIn(wCardList, iCardCount, 3);
	if((Note!=-1) && (Note%10!=0)) //有牛
	{
		return (Note % 10);
	}
	else if((Note!=-1) && (Note%10==0)) //牛牛
	{
		return 10;
	}
	else if(Note == -1)  //无牛
	{
		return 0;
	}
	return 0;	
}
///得到手牌中最大的牌(含花色)
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  最大牌
BYTE CPokerGameLogic::GetMaxCard(BYTE  wCardList[],int iCardCount)
{
	int temp = 0;
	BYTE card = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (temp == 0)
		{
			temp = GetCardBulk(wCardList[i]);
			card = wCardList[i];
		}
		else
		{
			if (temp < GetCardBulk(wCardList[i])) 
			{
				temp = GetCardBulk(wCardList[i]);
				card = wCardList[i];
			}
			else if (temp == GetCardBulk(wCardList[i]))
			{
				if(GetCardShape(card) < GetCardShape(wCardList[i]))
				{
					temp = GetCardBulk(wCardList[i]);
					card = wCardList[i];
				}
			}
		}
	}
	return card;
}
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: bool [out]  是否牛
bool CPokerGameLogic::IsBull(BYTE  wCardList[],int iCardCount)
{
	int temp=0;
	for (int i=0; i<iCardCount; i++)
	{
		temp += GetCardNumber(wCardList[i]);
	}
	return temp % 10 == 0;
}
///@param: WORD wCardList[] [in] 牌集
///@param: int  iCardCount  [in] 牌张数
///@return: int [out]  王个数
int  CPokerGameLogic::CountKing(BYTE  wCardList[],int iCardCount)
{
	int temp=0;
	for (int i=0; i<iCardCount; i++)
	{
		if (GetCardNumber(wCardList[i]) == _ECN_NUM_Joker
			|| GetCardNumber(wCardList[i]) == _ECN_NUM_JOKER)
		{
			temp++;
		}
	}
	return temp;
}
//===================================================================================
//获取牌点
//===================================================================================
BYTE CPokerGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	BYTE cbPipCount=0;

	//获取牌点
	BYTE cbCardValue=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		cbCardValue=GetCardNumber(cbCardData[i]);
		if(cbCardValue==0x0D)					//如果是A算1点
		{
			cbCardValue=0x01;
			cbPipCount+=cbCardValue;
		}
		else
		{
			cbPipCount+=(++cbCardValue);
		}
	}
	return (cbPipCount%10);
}

