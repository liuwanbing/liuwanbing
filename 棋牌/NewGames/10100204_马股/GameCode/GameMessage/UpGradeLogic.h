/*	UpGradeLogic.h
*	马股游戏的规则： 
*	1.牌： 牌张数， 牌色 ，
*	2.玩家数，
*	3.庄家和闲家的确定规则
*	4.游戏模式
*	5.各种思考时间
*	6.赔率，下注倍数，底注
*	7.牌型
*/

#pragma once

#include <vector>
using namespace std;

extern struct T_CS_OutCard;

//操作掩码
#define	UG_HUA_MASK		0xF0	//1111 0000
#define	UG_VALUE_MASK	0x0F	//0000 1111

//扑克花色
#define CT_SPADE		0x00	//黑桃
#define CT_HREART		0x10	//红心
#define	CT_CLUB			0x20	//梅花
#define CT_DIAMONS		0x30	//方块


//牌
// const BYTE Cards[36] =	
// {
// 	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
// 	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
// 	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
// 	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39
// };


//最小带入额
const int MinMoney = 200;

//模式选择思考时间
const short SelectGameModeTime = 15;
//告知玩家游戏模式时间
const short TellGameModeTime = 15;
//强庄思考时间
const short BetBankerTime = 15;
//投注时间
const short BetTime = 15;

//底注
const short MinBet = 100;
const short DoubleBet = 2;	//2倍
const short TripleBet = 3;	//3倍
const short FiveTimesBet = 5;	//5倍
const short TenTimesBet = 10;	//10倍


//一桌上最大玩家数
const short MaxPlayerInTable = 4;

//赔率
const short Pay_NOTEN = 1;			//无凑  no ten
const short Pay_ONETEN = 1;		//有凑  one ten
const short Pay_DOUBLETEN = 2;		//马股	double ten
const short Pay_DTENANDDF = 4;	//铜锤 double ten AND double five
const short Pay_DTSMALLASC = 5;		//小顺马，small double ten
const short Pay_DTBIGASC = 6;		//大顺马, big double ten
const short Pay_ALLSUMTEN = 8;		//十马， all sum ten
const short Pay_BOOM = 10;			//四条， boom

//牌型
const enum EnCardShape {
	CS_INVALID = 1,			//无效
	CS_NOTEN ,			//无凑
	CS_ONETEN,			//有凑
	CS_OT_ONE,			//有凑一点
	CS_OT_TWO,			//有凑二点
	CS_OT_THREE,		//有凑三点
	CS_OT_FOUR,			//有凑四点
	CS_OT_FIVE,			//有凑五点
	CS_OT_SIX,			//有凑六点
	CS_OT_SEVEN,		//有凑七点
	CS_OT_EIGHT,		//有凑八点
	CS_OT_NIGHT,		//有凑九点
	CS_DOUBLETEN,		//马股
	CS_DTENANDDF,		//铜锤
	CS_SMALLHORSE,		//小顺马
	CS_BIGHORSE,		//大顺马
	CS_TENHORSE,			//十马
	CS_BOOM			//四条

};

//马股规则
class CUpGradeGameLogic
{
public:
	CUpGradeGameLogic();
	~CUpGradeGameLogic();	


public:

	//获取扑克数字
	inline int GetCardNum(BYTE iCard) { return (iCard&UG_VALUE_MASK)+1; }
	//获取扑克花色(默认为真实花色)
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua=TRUE);

	//洗牌
	void RandCard();

	//发牌
	void DistributeCards(int iDeskStation,BYTE* pCards,int iCardCount);

	//验证游戏中数据是否有效
	bool TestData(int iData,int iStyle);


	//////////////////////////////////////////////////////////////////////////
	//牌型判断
	
	//无凑
	bool NoTen(T_CS_OutCard *pCards);

	//有凑
	int IsOneTen(T_CS_OutCard *pCards);
	//马股
	bool IsDoubleTen(const T_CS_OutCard *pCards);
	//铜锤
	bool IsDoubleTenDoubleFive(const T_CS_OutCard *pCards);
	//小顺马
	bool IsSmallHourse(const T_CS_OutCard *pCards);
	//大顺马
	bool IsBigHourse(const T_CS_OutCard *pCards);
	//十马
	bool isAllSumTen(const T_CS_OutCard *pCards);
	//四条
	bool isFourSame(const T_CS_OutCard *pCards);
private:
	vector<BYTE> m_Cards;

};





