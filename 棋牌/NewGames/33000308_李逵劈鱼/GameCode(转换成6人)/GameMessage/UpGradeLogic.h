#pragma once

//#include "StdAfx.h"

#include <vector>


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
#define UG_ERROR_KIND							0				//错误

#define UG_ONLY_ONE								1				//单张
#define UG_DOUBLE								2				//对牌

#define UG_VARIATION_STRAIGHT					3				//变种顺子(A2345)顺子中最小
#define UG_STRAIGHT								4               //顺子,5+张连续牌
#define UG_FLUSH								5				//同花(非连)

#define UG_STRAIGHT_FLUSH						47               //同花顺,花色相同的顺子
#define UG_VARIATION_STRAIGHT_FLUSH				46               //同花顺,花色相同的顺子

#define UG_THREE								8				//三张
#define UG_THREE_ONE							9               //3 带 1
#define UG_THREE_TWO							10               //3 带 2
#define UG_THREE_DOUBLE							11				//3 带1对

#define UG_VARIATION_DOUBLE_SEQUENCE			12				//变种双顺(AA22)最小
#define UG_DOUBLE_SEQUENCE						13				//连对,2+个连续的对子

#define UG_VARIATION_THREE_SEQUENCE				14				//变种三顺(AAA222最小)
#define UG_THREE_SEQUENCE						15				//连三张，2+个连续的三张

#define UG_VARIATION_THREE_ONE_SEQUENCE			16				//变种三顺带一
#define UG_THREE_ONE_SEQUENCE					17              //2+个连续的三带一

#define UG_VARIATION_THREE_TWO_SEQUENCE			18				//变种三顺带二
#define UG_THREE_TWO_SEQUENCE					19				//2+个连续的三带二

#define UG_VARIATION_THREE_DOUBLE_SEQUENCE		20				//变种三连张带对
#define UG_THREE_DOUBLE_SEQUENCE				21				//三连张带对

#define UG_VARIATION_THREE_SEQUENCE_DOUBLE_SEQUENCE		22		//变种蝴蝶(三顺带二顺)
#define UG_THREE_SEQUENCE_DOUBLE_SEQUENCE		23				//蝴蝶(三顺带二顺)

#define UG_FOUR_ONE								24				//四带一
#define UG_FOUR_TWO								25				//四带二张
#define UG_FOUR_ONE_DOUBLE						26				//四带一对
#define UG_FOUR_TWO_DOUBLE						27				//四带二对

#define UG_VARIATION_FOUR_SEQUENCE				28				//变种四顺
#define UG_FOUR_SEQUENCE						29				//四顺

#define UG_VARIATION_FOUR_ONE_SEQUENCE			30				//四带一顺
#define UG_FOUR_ONE_SEQUENCE					31				//四带一顺

#define UG_VARIATION_FOUR_TWO_SEQUENCE			32				//四带二顺
#define UG_FOUR_TWO_SEQUENCE					33				//四带二顺

#define UG_VARIATION_FOUR_ONE_DOUBLE_SEQUENCE	34				//四带对顺
#define UG_FOUR_ONE_DOUBLE_SEQUENCE				35				//四带对顺

#define UG_VARIATION_FOUR_TWO_DOUBLE_SEQUENCE	36				//四带二对顺
#define UG_FOUR_TWO_DOUBLE_SEQUENCE				37				//四带二对顺


#define UG_SLAVE_510K							38              //510K炸弹,花色不同
#define UG_MASTER_510K							39              //510K同花炸弹

#define UG_BOMB									45              //炸弹>=4張
#define UG_KING_BOMB							50				//王炸(最大炸弹)


#define KING_COUNT								4				//所有王的个数
//扑克分析结构	（只用于服务器对比扑克）
struct CardAnalyseStruct
{
	int		iSingleCount;	  //单牌数目
	int		iDoubleCount;	  //对牌数目
	int		iThreeCount;	  //三条数目
	int		iBombCount;		  //炸弹数
	int		i510KCount;		  //510K数目
	int		i510KMastCount;	  //主510K数目
//	BYTE	iBigsingleCard;	  //最大单牌
//	BYTE	iLesssingleCard;  //最小单牌

	BYTE	iSingleArray[54]; //单牌
	BYTE	iDoubleArray[54]; //对牌数组
	BYTE	iThreeArray[54];  //三条数组
	BYTE	iBombArray[54];	  //两对拖拉机数组 （0 分割，长度，拖拉机数据）
	BYTE	i510KArray[54];	  //三对拖拉机数组 （0 分割，长度，拖拉机数据）
};

//510K逻辑类 支持 2 副扑克）
class CUpGradeGameLogic
{
	//变量定义
private:
	BYTE			m_iNTNum;		        //主牌数字
	int				m_iStation[5];	        //相对位置（方块，梅花，红桃，黑桃，主牌）
	BYTE			m_bSortCardStyle;       //排序方式
	int				m_iCondition;		    //限制条件
	BOOL			m_bKingCanReplace;		//王是否可当
	DWORD			m_iCardShapeRestrict1;	//牌型列表
	DWORD			m_iCardShapeRestrict2;	//牌型列表

	BYTE			m_iReplaceCardArray[54];//代替牌隊列
	BYTE			m_iReplaceCardCount;	//代替牌隊列長度
	//函数定义
public:
	//构造函数		
	CUpGradeGameLogic(void);
	//析构函数
	virtual ~CUpGradeGameLogic();

	//功能函数（公共函数）
public:
	//[设置相关]
	//设置主牌数字
	inline void SetNTCardNum(BYTE iNTNum) { m_iNTNum=iNTNum;}
	//获取主牌数字
	inline BYTE GetNTCardNum() { return m_iNTNum; }
	//設置代替牌隊例
	BOOL SetReplaceCard(BYTE iCardList[],int iCardCount);
	//获取队例中可代替牌张数
	int GetReplaceCardCount(BYTE iCardList[],int iCardCount);
	//获取扑克数字
	inline int GetCardNum(BYTE iCard) { return (iCard&UG_VALUE_MASK)+1; }
	//获取扑克花色(默认为真实花色)
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua = TRUE);
	//获取扑克相对大小(默认为牌大小,非排序大小)	bBulkOrNum false:表示算主牌。true:表示不算主牌
	int GetCardBulk(BYTE iCard,BOOL bSortOrCmp = FALSE,bool bBulkOrNum = false);
	//获取扑克牌通过相对大小
	BYTE GetCardByValue(int iCardValue);
	//設置王可以當牌
	void SetKingCanReplace(BOOL bKingCanReplace=false){m_bKingCanReplace=bKingCanReplace;}
	//设置牌型
	void SetCardShapeRestrict(DWORD iCardShape1,DWORD iCardShape2 )
	{
		m_iCardShapeRestrict1 = iCardShape1;
		m_iCardShapeRestrict2 = iCardShape2;
	}
	//獵取王是否可以當牌
	BOOL GetKingCanReplace(){return m_bKingCanReplace;}
	//设置排序方式
	void SetSortCardStyle(BYTE SortCardStyle){m_bSortCardStyle = SortCardStyle;}
	//获取排序方式
	BYTE GetSortCardStyle(){return m_bSortCardStyle;}
	//获取扑克(根据花色,数字确定牌)
	BYTE GetCardFromHua(int iHuaKind, int iNum)
	{
		if (iHuaKind!=UG_ERROR_HUA) return (iHuaKind+iNum-1);
		return 0;
	}
	//[排序]
public:
	//排列扑克,按大小(保留系统序例)
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount,BOOL bSysSort = FALSE);
	//反转牌顺(从低->高)
	BOOL ReverseCard(BYTE iCardList[], BYTE bUp[], int iCardCount);
	//按牌型排序
	BOOL SortCardByStyle(BYTE iCardList[],int iCardCount);
	//按花色排序
	BOOL SortCardByKind(BYTE iCardList[],int iCardCount);
public:
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount,bool bHaveKing=false);		//bHaveKing表示是否有大小猫,false无,ture有
	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
private:
	//清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);

public://[辅助函数]
	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);
	//查找分数
	int FindPoint(BYTE iCardList[], int iCardCount);
	//是否为同一数字牌
	BOOL IsSameNumCard(BYTE iCardList[],int iCardCount);
	//是否为同一数字牌(加入代替牌逻辑)
	BOOL IsSameNumCardAndKing(BYTE iCardList[],int iCardCount);
	//是否为同一花色
	BOOL IsSameHuaKind(BYTE iCardList[],int iCardCount);
	//是否为同一花色(加入王代替逻辑)
	BOOL IsSameHuaKindAndKing(BYTE iCardList[],int iCardCount);
	//查找 >=4 炸弹的数量炸弹基数
	BYTE GetBombCount(BYTE iCardList[], int iCardCount,int iNumCount = 4);
	//获取指定大小牌个数
	BYTE GetCountBySpecifyNumCount(BYTE iCardList[],int iCardCount,int Num);
	//获取指定牌个数
	BYTE GetCountBySpecifyCard(BYTE iCardList[],int iCardCount,BYTE bCard);
	//获取指定牌张数牌大小(队例中只能够有一种牌的张数为iCount,不然传出去的将是第一个指定张数的值)
	BYTE GetBulkBySpecifyCardCount(BYTE iCardList[],int iCardCount,int iCount);
	//利用代替牌逻辑(找指定张数牌)
	BYTE GetBulkBySpecifyCardCountAndKing(BYTE iCardList[],int iCardCount,int iCount);

	//是否为某指定的顺子(变种顺子)
	BOOL IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount);
	//是否为某指定的顺子(变种顺子)(加入代替牌逻辑)
	BOOL IsVariationSequenceAndKing(BYTE iCardList[], int iCardCount, int iCount);
	//是否为某指定的顺子
	BOOL IsSequence(BYTE iCardList[], int iCardCount, int iCount);
	//是否带王顺子
	BOOL IsSequenceAndKing(BYTE iCardList[],int iCardCount,int iCount);
	//提取指定的牌
	BYTE TackOutBySpecifyCard(BYTE iCardList[], int iCardCount,BYTE bCardBuffer[],int &iResultCardCount,BYTE bCard);
	//提取某张指定数字的牌
	BOOL TackOutCardBySpecifyCardNum(BYTE iCardList[],int iCardCount,BYTE iBuffer[],int &iBufferCardCount,BYTE iCard);
	//提取所有符合条件的牌,单张,对牌,三张,4炸弹牌型
	int TackOutBySepcifyCardNumCount(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[],BYTE bCardNum);
	//提取指定花色牌
	int TackOutByCardKind(BYTE iCardList[],int iCardCount,BYTE iDoubleBuffer[],BYTE iCardKind);
	//拆出(将手中牌多的拆成少的)
	int TackOutMuchToFew(BYTE iCardList[],int iCardCount,BYTE iDoubleBuffer[],int &iBufferCardCount,BYTE iCardMuch,BYTE iCardFew);
	//查找大于iCard的单牌所在iCardList中的序号
	BYTE GetSerialByMoreThanSpecifyCard(BYTE iCardList[],int iCardCount,int iCard,BYTE iBaseCardCount, bool bExtValue=false);
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard,int temp = -1);
	//获取指定顺子中牌点最小值(iSequence 代表顺子的牌数最多为
	BYTE GetBulkBySpecifySequence(BYTE iCardList[],int iCardCount ,int iSequence = 3);
	//获取指定顺子中牌点最大值变种顺子
	BYTE GetBulkBySpecifyVariationSequence(BYTE iCardList[],int iCardCount ,int iSequence = 3);
	//查找最小 (1) or 最大 (255) 牌 ,bBulkOrNum false:表示算主牌。true:表示不算主牌
	int	GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax,bool bBulkOrNum = false);
	//得到手中最大牌
	BYTE GetMaxCard(BYTE iCardList[],int iCardCount);
	//得到手中最小牌
	BYTE GetMinCard(BYTE iCardList[],int iCardCount);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//[牌型相关]
public:
    //获取牌型
	BYTE GetCardShape(BYTE iCardList[], int iCardCount);
	//是否单牌
	inline BOOL IsOnlyOne(BYTE iCardList[], int iCardCount) { return iCardCount==1; };
	//是否对牌
	BOOL IsDouble(BYTE iCardList[], int iCardCount);
	//是否纯三条
	BOOL IsThree(BYTE iCardList[],int iCardCount);
	//3 带 1or2(带一对带二单张或带一单张
	BOOL IsThreeX(BYTE iCardList[], int iCardCount, int iX/*1or2*/);
	//3带1or 2(带一对带二单张事带一单张)加入代替牌逻辑
	BOOL IsThreeXAndKing(BYTE iCardList[],int iCardCount,int iX);
	//王炸
	BOOL IsKingBomb(BYTE iCardList[],int iCardCount);
	//4+张牌 炸弹
	BOOL IsBomb(BYTE iCardList[], int iCardCount);
	//同花炸弹
	BOOL IsBombSameHua(BYTE iCardList[],int iCardCount);
	//同花(非顺子)
	BOOL IsFlush(BYTE iCardList[],int iCardCount);
	//是否是同花顺
	BOOL IsStraightFlush(BYTE iCardList[], int iCardCount);
	//是否变种同花顺
	BOOL IsVariationStraightFlush(BYTE iCardList[], int iCardCount);
	//变种单甩
	BOOL IsVariationStraight(BYTE iCardList[],int iCardCount);
	//单甩
	BOOL IsStraight(BYTE iCardList[],int iCardCount);
	//变种对甩
	BOOL IsVariationDoubleSequence(BYTE iCardList[],int iCardCount);
	//对甩 //连对?
	BOOL IsDoubleSequence(BYTE iCardList[],int iCardCount);

	//是否变种是连续的三带X(0,1,2,3)
	BOOL IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/);

	//是否是连续的三带X(0,1,2,3)
	BOOL IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX/*0,1or2*/);
	//是否三顺带二顺(蝴蝶)
	BOOL IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount);
	//是否三顺带二顺(蝴蝶)
	BOOL IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount);
	//510K 炸弹
	BOOL IsSlave510K(BYTE iCardList[], int iCardCount);
	//510K 同花炸弹
	BOOL IsMaster510K(BYTE iCardList[], int iCardCount);
	//四带一或者四带二
	BOOL IsFourX(BYTE iCardList[],int iCardCount,int iX/*1or 2*/);//单张1,任意张2,一对子,2对4
	//是否变种四带X顺
	BOOL IsVariationFourXSequence(BYTE iCardList[],int iCardCount,int iSeqX);
	//四带一或者四带二的顺子
	BOOL IsFourXSequence(BYTE iCardList[],int iCardCount,int iSeqX);
	//[出牌相关]
public:
	//自动出牌函数
	BOOL AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut,BOOL bIsCue = FALSE);
	//是否可以出牌
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount,BOOL bFirstOut = FALSE);
	
	//[提取牌]
public:	
	//提取单个的三带1 or 2or 3(单,一对,或二单张)
	BYTE TackOutThreeX(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount,int  xValue);
	//提取2个以上连续的三带1,2
	BOOL TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue);
	//提取2个以上连续的三带1,2
	BOOL TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount);
	//获取顺子中最小位置值(xSequence表示默认单顺)
	int GetSequenceStartPostion(BYTE iCardList[],int iCardCount,int xSequence=1);
	//提取单张的顺子,连对顺子,连三顺子
	BOOL TackOutSequence(BYTE iCardList[],int iCardCount,BYTE iBaseCard[],int iBaseCount,BYTE iResultCard[],int &iResultCardCount,int xSequence,BOOL bNoComp = false);
	//提取同花顺
	BOOL TackOutStraightFlush(BYTE iCardList[],int iCardCount,BYTE iBaseCard[],int iBaseCount,BYTE iResultCard[], int &iResultCardCount);
	//提取同花顺
	BOOL TackOutStraightFlush(BYTE iCardList[],int iCardCount,BYTE iResultCard[], int &iResultCardCount);
	//提取所的炸弹
	BOOL TackOutAllBomb(BYTE iCardList[],int iCardCount,
									   BYTE iResultCard[],int &iResultCardCount,int iNumCount=4);
	//提取炸弹(张数默认为4)
	BOOL TackOutBomb(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,int iNumCount=4);
	//提取王炸
	BOOL TackOutKingBomb(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount);
	//提取510K
	BOOL TrackOut510K(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BOOL bIsMast510K = FALSE);
	//测试510K
	BOOL Test510K(BYTE iCardList[],int iCardCount);
	//拷背
	BOOL Copy510K(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount);
	//拆大桌面牌
	BOOL TackOutCardByNoSameShape(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount);
	//用大的牌牌大桌面上的牌
	BOOL TackOutMoreThanLastShape(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount,int ishape);
public:
	//查找三连牌中最小的那个三张的牌点
	int SearchMinThreeSeq(BYTE iCardList[], int iCardCount);
	//重置限制条件
	void ResetCondition();
	//查找属于某条件的牌
	BOOL TackOutByCondition(BYTE iCardList[],int iCardCount, BYTE iResultCard[],
								int & iResultCardCount,
								BYTE iConditionCard[],int iConditionCardCount);
	BOOL TackOutAddLevelCard(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount);
	
	//查找比当前出牌大的
	BOOL TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount,BYTE iBaseCard[], int iBaseCardCount,
									BYTE iResultCard[], int & iResultCardCount,bool bExtVal=false, BOOL bIsCue = FALSE);
	//提取代替单个的三带1 or 2or 3(单,一对,或二单张)
	BYTE TackOuttReplaceThreeX(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount,int  xValue);
	//提取单张的顺子,连对顺子,连三顺子
	BOOL TackOutReplaceSequence(BYTE iCardList[],int iCardCount,BYTE iBaseCard[],int iBaseCount,BYTE iResultCard[],int &iResultCardCount,int xSequence,BOOL bNoComp = false);
	//判断是不是多功能牌
	BOOL JudgeCard(BYTE iCard);
};
