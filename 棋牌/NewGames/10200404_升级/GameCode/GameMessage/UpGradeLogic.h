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

/****************牌型*****************/
#define CS_SINGLE					10				//单牌
#define CS_DOUBLE					11				//对牌
#define CS_MULTIDOUBLE				12				//拖拉机
#define CS_MIXED					13				//甩牌
//扑克出牌类型
#define KING_COUNT								2				//所有王的个数
class CUpGradeGameLogic
{
	//变量定义
private:
//	int				m_iStation[5];	   //相对位置（方块，梅花，红桃，黑桃，主牌）
	BYTE			m_bSortCardStyle;  //排序方式
	int				m_iCondition;			//限制条件
	BOOL			m_bKingCanReplace;		//王是否可当
	DWORD           m_iCardShape;	//支持牌型
	BYTE            m_byMajorFlower;//主花色
	BYTE            m_byLevel;//级别
	//函数定义
public:
	//构造函数		
	CUpGradeGameLogic(void);
	//析构函数
	virtual ~CUpGradeGameLogic();
	//功能函数（公共函数）
public:
	//[设置相关]
	//获取扑克数字
	inline int GetCardNum(BYTE iCard) { return (iCard&UG_VALUE_MASK)+1; }
	//获取扑克花色(默认为真实花色)
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua=TRUE);
	//获取扑克相对大小(默认为牌大小,非排序大小)
	int GetCardBulk(BYTE iCard, bool bExtVal=false);
	//获取扑克牌通过相对大小
	BYTE GetCardByValue(int iCardValue);
	//設置王可以當牌
	void SetKingCanReplace(BOOL bKingCanReplace=false){m_bKingCanReplace=bKingCanReplace;}
	//獵取王是否可以當牌
	BOOL GetKingCanReplace(){return m_bKingCanReplace;}
	//设置排序方式
	void SetSortCardStyle(BYTE SortCardStyle){m_bSortCardStyle=SortCardStyle;}
	//获取排序方式
	BYTE GetSortCardStyle(){return m_bSortCardStyle;}
	//验证游戏中数据是否有效
	bool TestData(int iData,int iStyle);
	//洗盘
	BYTE RandCard(BYTE iCard[], int iCardCount,bool bHaveKing);
	//计算牌个数
	int GetCardNums(BYTE byCardList[], int iCardCount);
	//判断牌是否合法
	bool IsLegalCard(BYTE byCard);
	//花色是否合法
	bool IsLegalFlower(BYTE byFlower);
	//设置级别和花色
	void SetLevelAndFlower(BYTE byLevel,BYTE byMajorFlower);
	//[排序]
public:
	//按排大小，由达到小排序
	void SortCard(BYTE byCardArr[],int iCount);
	//扩展排序
	BOOL SortCardByEx(BYTE iCardList[], BYTE bUp[], BYTE iCardCount,BOOL bSysSort = FALSE);
	//反转牌顺(从低->高)
	BOOL ReverseCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount);
	//按牌型排序
	BOOL SortCardByStyle(BYTE iCardList[],BYTE iCardCount);
	//按花色排序
	BOOL SortCardByKind(BYTE iCardList[],BYTE iCardCount);
	//1.按王，黑，红，梅，方排序
	void SortByColor(BYTE byCardArr[],int iCount,vector<BYTE> &vJor,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond);
	//2.获得级牌
	void GetGameLevelCard(BYTE byLedCardColor,BYTE byCardLevel,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond,vector<BYTE>& vOuputVector,BYTE byDeskStation);
public:
	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
public:
	//单圈比牌
	BYTE SinglePK(TFirstData ptcsOutCard[],BYTE byRoundFirstOutCard,int& iRoundScore);	//伪实现
	//从手中删除出的牌
	void ReMoveOutCards(BYTE byCardList[],int iCardCount,vector<BYTE> &vCardsInHand);
	//获得牌型， iNums 牌张数;  tcsOutCard-输出参数
	int GetCardShape(BYTE byCardList[],int iCardCount);
	//判断是否出牌成功
	bool IsOutCardSuccess(TFirstData &tFirstPlayerCard,BYTE byCardList[],int iCardCount,int iShape,vector<BYTE> &vHandleCard);
	//根据提示获取某花色对牌
	bool GetTipDoubleCardForSameFlower(BYTE byCardList[],int iCardCount,BYTE byColor,BYTE byNewList[], int &iNewCount);
	//获得主牌
	void GetMainCard(BYTE byLevelColor,BYTE byLevelCard,vector<BYTE>& vCardsInHand,vector<BYTE>&vMainCard);
	//是否为某指定的顺子
	BOOL IsSequence(BYTE iCardList[], int iCardCount, int iCount);
	//是否为同一花色
	bool IsSameHuaKind(BYTE iCardList[], int iCardCount);
	//查找最小牌
	BYTE FindMinCard(BYTE byCardList[], int iCardCount);
	//托管出牌
	void AutoOutCard(TFirstData &tFirstPlayerCard,BYTE byCardList[],int iCardCount,int iRoundOutCardCount,T_CS_OutCard &tOutCard);
	//主牌对子数量
	int GetMajorDoubleNums(BYTE byCardList[],int iCardCount,vector<BYTE> &vMajorDoubleCard);
	//同花色牌数量
	int GetSameFlowerNums(BYTE byCardList[],int iCardCount,BYTE byFlower,vector<BYTE> &vSameFlowerCard);
	//获得主牌
	void GetMajorCard(BYTE byCardList[],int iCardCount,vector<BYTE> &vMajorCard);
	//获得副牌
	void GetOtherCard(BYTE byCardList[],int iCardCount,BYTE byResultCard[],int &iResultCount);
	//主牌拖拉机数量
	void GetSerialNums(BYTE byCardList[],int iCardCount,int iLen,vector<BYTE> &vSerialCard);
	//是否主牌
	bool IsMajorCard(BYTE byCard);
	//甩牌
	bool ThrowCard(vector<BYTE> vHandleCard[],BYTE byCardList[],int iCardCount,BYTE byDeskStation,int &iThrowSthle);
	//得到下一个出牌位置
	BYTE GetNextDeskStation(BYTE byDeskStation);
	//获得分数
	int GetPoint(BYTE byCardList[],int iCardCount);
	//计算相同牌个数
	int GetSameCardNums(BYTE byCardList[],int iCardCount,BYTE byCard);
	//两张牌比较大小
	int CompareCard(BYTE byFirstCard,BYTE bySecondCard);
	//获得牌等级(大王>小王>主级牌>副级牌>主花色牌>副牌依次分六个等级5,4,3,2,1,0)
	int GetCardLevel(BYTE byCard);
	//日志记录
	void Debuglog(const char *p, ...);
};