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

	//[排序]
public:
	//按排大小，由达到小排序
	void SortCard(BYTE byCardArr[],int iCount,BYTE byLedCardColor,BYTE byCardLevel);
		
		//1.按王，黑，红，梅，方排序
	void SortByColor(BYTE byCardArr[],int iCount,vector<BYTE> &vJor,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond);
		//2.获得级牌
	void GetGameLevelCard(BYTE byLedCardColor,BYTE byCardLevel,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond,vector<BYTE>& vOuputVector);


public:
	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);
private:
	//清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);

public:
	BOOL IsThereLevelCard(BYTE byLevelCard,BYTE* pbyCards,int iCardNum);
	//检查选出的牌是否合法 // 【这个方法没什么用】
	BOOL IsLegal(vector<BYTE> &vOutCards);
	//单圈比牌
	BYTE SinglePK(T_CS_OutCard *ptcsOutCard,BYTE byRoundFirstOutCard,BYTE byGameLevel,BYTE byGameLevelColor,int& iRoundScore);	//伪实现
	//判断甩牌是不是最大
	bool CheckMixedCardLegal(T_CS_OutCard *ptcsOutCard);	//伪实现
	//从手中删除出出去的牌
	void RemoveOutCards(BYTE *OriCards,int* iOriNums,BYTE *DelpCards,int iDelNums);
	//从手中删除出的牌
	void ReMoveOutCards(T_CS_OutCard* ptOutCards,vector<BYTE> &vCardsInHand);
	//获得出牌的牌型， iNums 牌张数;  tcsOutCard-输出参数
	void GetOutCardShape(BYTE *pCards,int iNums,T_CS_OutCard &tcsOutCard,BYTE byDeskStation,BYTE byGameLevelCard,bool isFirst);
	//判断是否出牌成功
	bool IsOutCardSuccess(T_CS_OutCard &tFirstPlayerCard, T_CS_OutCard &tCurrentOutCard,vector<BYTE> pvPlayerCards[],WCHAR* RetMsg,BYTE byGameLevel,BYTE byGameLevelColor);
	//判断是否甩牌成功
	bool IsOutMixedCardsSuccess(T_CS_OutCard &tCurrentOutCard,vector<BYTE> pvPlayerCards[],BYTE byMinCards[],int* iCardCount);
	//获取某花色单盘
	bool GetSingleCard(BYTE byColor,vector<BYTE>& vHandCards,BYTE* pbyOutCards,int *piOutCount);
	//获取某花色对牌
	bool GetDoubleCard(BYTE byColor,vector<BYTE>& vHandCards,BYTE* pbyOutCards,int *piOutCount);
	//获取某花色拖拉机
	bool GetSerialDouble(BYTE byColor,vector<BYTE>& vHandCards,int iPair,BYTE byGameLevelCard,BYTE* pbyOutCards,int *piOutPair);
	//是否是主牌
	bool IsMainCard(BYTE byLevelColor , BYTE byLevelCard,BYTE byCompareCard);
	//获得主牌
	void GetMainCard(BYTE byLevelColor,BYTE byLevelCard,vector<BYTE>& vCardsInHand,vector<BYTE>&vMainCard);
};