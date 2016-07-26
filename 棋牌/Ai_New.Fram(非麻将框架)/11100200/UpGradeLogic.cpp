#include"stdafx.h"
#include "UpGradeLogic.h"

// 构造、析构
CPokerGameLogic::CPokerGameLogic()
{

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
WORD CPokerGameLogic::CardToWord(BYTE byPkSet, BYTE byShape, BYTE byNumber)
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
	WORD wCard = MAKE_A_CARD(byPkSet, byShape, byNumber);
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
bool CPokerGameLogic::WordToCard(WORD wInCard, BYTE &byPkSet, BYTE &byShape, BYTE &byNumber)
{
	// 获取第几副牌
	byPkSet = GetCardSet(wInCard);

	// 获取花色
	byShape = GetCardShape(wInCard);

	// 获取牌点
	byNumber = GetCardNumber(wInCard);

	// 牌副数、牌点检查
	if ((EPSN_SET_NUM_NONE == byPkSet) || (ECN_NUM_NONE == byNumber))
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
BYTE CPokerGameLogic::GetCardSet(WORD wCard)
{
	BYTE bySet = (BYTE)(wCard>>8&0x00FF);     // 牌副
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
BYTE CPokerGameLogic::GetCardShape(WORD wCard)
{
	BYTE byCardShape = (BYTE)(wCard>>4&0x000F);  // 牌花色
	if (IsAvailableCardShape(byCardShape))
	{
		return byCardShape;
	}
	return ECS_SHAPE_NONE;
}

/********************************************************************************
* 函数名: static BYTE GetCardNumber(WORD wCard)
* 功能;   返回牌点
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌点
********************************************************************************/
BYTE CPokerGameLogic::GetCardNumber(WORD wCard)
{
	BYTE byCardNumber = (BYTE)(wCard&0x000F);  // 牌点
	if (IsAvailableCardNumber(byCardNumber))
	{
		return byCardNumber;
	}
	return ECN_NUM_NONE;
}

/********************************************************************************
* 函数名: static BYTE GetCardNumber(WORD wCard)
* 功能;   返回牌值(花色<<4|牌点)
* 参数: WORD wCard  [in]  输入牌
* 返回: BYTE 牌值
********************************************************************************/
BYTE CPokerGameLogic::GetCardValue(WORD wCard)
{
	BYTE byCardValue = (BYTE)(wCard&0x00FF);
	// 检查牌值
	if (IsAvailableCardValue(byCardValue))
	{
		return byCardValue;
	}
	return 0;
}

/********************************************************************************
* 函数名:static bool IsAvailableCard(WORD wCard)
* 功能;   检验牌是否为有效
* 参数: WORD wCard [in]  牌(第几副、花色与牌点组合)
* 返回: bool 合法的牌，返回true，否则返回false
********************************************************************************/
bool CPokerGameLogic::IsAvailableCard(WORD wCard)
{
	// 检查牌为第几副牌
	BYTE bySet = (BYTE)(wCard>>8&0xFF);
	if (!IsAvailableCardSet(bySet))
	{
		return false;
	}

	// 检查牌值
	BYTE byCardValue = (BYTE)(wCard&0x00FF);
	if (IsAvailableCardValue(byCardValue))
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
	BYTE byCardShape = (byCardValue>>4)&0x0F;
	if (!IsAvailableCardShape(byCardShape))
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
int CPokerGameLogic::RemoveCard(WORD wRemoveCard[], int iRemoveCount, WORD wCardList[], int iCardCount)
{
	return 0;
}

/********************************************************************************
* 函数名: static int CardCompare(WORD wCardFirst, WORD wCardSecond)
* 功能;   两张牌进行比较
* 参数: WORD wCardFirst  [in]   第一张牌
*       WORD wCardSecond [in]   第二张牌 
* 返回: int 返回E_COMPARE_RESULT 第一张牌<第二张 ECR_LESS
********************************************************************************/
int CPokerGameLogic::CardCompare(WORD wCardFirst, WORD wCardSecond)
{
	// 检查牌是否合法
	if (!IsAvailableCard(wCardFirst) || !IsAvailableCard(wCardSecond))
	{
		return ECR_OTHER;   // 无法比较
	}

	// 去除牌副数进行比较
	BYTE byValueFirst = GetCardValue(wCardFirst);
	BYTE byValueSecond = GetCardValue(wCardSecond);
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
* 返回: HRESULT 0 成功，其他失败
********************************************************************************/
HRESULT CPokerGameLogic::RandCard(WORD wCardList[], int iCardCount)
{
	// 参数检查
	if ((iCardCount<SINGLE_CARD_COUNT) || (iCardCount > (POKER_CARD_NUM*POKER_SET_NUM)))
	{
		return RET_FAILED;
	}

	// 清除
	memset(wCardList, 0,iCardCount*sizeof(WORD));
	BYTE i = 0;
	BYTE j = 0;
	BYTE h = 0;
	int iUseCount = 0;

	//CINIFile  f("E:/T.ini");
	//BYTE byShape = (BYTE)f.GetKeyVal("game", "shape", 0);
	//BYTE byNumber = (BYTE)f.GetKeyVal("game", "number", 0);

	// 组装牌数组
	for (i=0; i<POKER_SET_NUM; ++i)
	{
		BYTE byCardSet = i+1;
		for (j=ECS_SHAPE_SPADE; j>ECS_SHAPE_NONE; --j)
		{
			for (h=ECN_NUM_K; h>ECN_NUM_NONE; --h)
			{
				//wCardList[iUseCount] = CardToWord(byCardSet, byShape, byNumber); 
				wCardList[iUseCount] = CardToWord(byCardSet, j, h);
				iUseCount ++;
				if (iUseCount == iCardCount) // 牌组装完成
				{
					break;
				}
			}

			if (iUseCount == iCardCount) // 牌组装完成
			{
				break;
			}
		}

		if (iUseCount == iCardCount) // 牌组装完成
		{
			break;
		}

		// 增加大、小王
		//wCardList[iUseCount] = CardToWord(byCardSet, byShape, byNumber); 
		//wCardList[iUseCount] = CardToWord(byCardSet, ECS_SHAPE_NONE, ECN_NUM_Joker);   // 小王
		//iUseCount ++;
		if (iUseCount == iCardCount) // 牌组装完成
		{
			break;
		}

		//wCardList[iUseCount] = CardToWord(byCardSet, byShape, byNumber); 
		//wCardList[iUseCount] = CardToWord(byCardSet, ECS_SHAPE_NONE, ECN_NUM_JOKER);   // 大王
		//iUseCount ++;
		if (iUseCount == iCardCount) // 牌组装完成
		{
			break;
		}
	}

	// 随机洗牌
	int iRandStart = 0;
	srand((unsigned)GetTickCount());
	do
	{
		int iPos = rand()%(iUseCount-iRandStart) + iRandStart;
		if (iPos != iRandStart)
		{
			WORD wCardTemp = wCardList[iRandStart];
			wCardList[iRandStart]= wCardList[iPos];
			wCardList[iPos] = wCardTemp;
		}		
		iRandStart++;
	}while (iRandStart<iUseCount);

	return RET_SUCCESS;	
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
HRESULT CPokerGameLogic::FindCardIndex(int &iIndex, WORD wFindCard, WORD wCardList[], int iCardCount)
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
BYTE CPokerGameLogic::GetCardLevel(WORD wCard)
{
	// 每游戏，牌的权值能不一样，根据需求修改此函数
	if (!IsAvailableCard(wCard))
	{
		return POKER_CARD_NONE;
	}

	// K(13) A(14) 2(16) 小王（18） 大王(20)
	BYTE byCardNumber = GetCardNumber(wCard);
	if (ECN_NUM_A == byCardNumber)
	{
		return 0x0E;
	}
	else if (ECN_NUM_2 == byCardNumber)
	{
		return 0x10;
	}
	else if (ECN_NUM_Joker == byCardNumber)
	{
		return 0x12;
	}
	else if (ECN_NUM_JOKER == byCardNumber)
	{
		return 0x14;
	}
	else
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
bool CPokerGameLogic::SortByLevel(WORD wCardList[], int iCardCount,bool bAscending)
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
					if (ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
					{
						byFirst = POKER_COUNT_SHAPES;
					}
					bySecond = GetCardShape(wCardList[j]);
					if (ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
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
				WORD wTemp = wCardList[j-1];
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
bool CPokerGameLogic::SortByNumber(WORD wCardList[], int iCardCount,bool bAscending)
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
					if (ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
					{
						byFirst = POKER_COUNT_SHAPES;
					}
					bySecond = GetCardShape(wCardList[j]);
					if (ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
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
				WORD wTemp = wCardList[j-1];
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
bool CPokerGameLogic::SortByShape(WORD wCardList[], int iCardCount,bool bAscending)
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
			if (ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
			{
				byFirst = POKER_COUNT_SHAPES;
			}
			BYTE bySecond = GetCardShape(wCardList[j]);
			if (ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
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
				WORD wTemp = wCardList[j-1];
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
bool CPokerGameLogic::SortByCardCount(WORD wCardList[], int iCardCount,bool bAscending)
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
						if (ECS_SHAPE_NONE == byFirst)  // 王的花色升一级
						{
							byFirst = POKER_COUNT_SHAPES;
						}
						bySecond = GetCardShape(wCardList[j]);
						if (ECS_SHAPE_NONE == bySecond)  // 王的花色升一级
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
				WORD wTemp = wCardList[j-1];
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
bool CPokerGameLogic::SortCards(WORD wCardList[], int iCardCount, int iMod, bool bAscending)
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
