#pragma once

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
#define UG_ERROR_KIND				0				//错误
#define UG_ONLY_ONE					1				//单牌
#define UG_DOUBLE					2				//对牌
#define UG_THREE					3				//三条
#define UG_DOUBLE_TRACKOR			4				//两条拖拉机
#define UG_THREE_TARCKOR			5				//三条拖拉机
#define	UG_SHOW_CARD				6				//甩牌


//扑克分析结构	（只用于服务器对比扑克）
struct CardAnalyseStruct
{
	int		iSignedCount;					//单牌数目
	int		iDoubleCount;					//对牌数目
	int		iThreeCount;					//三条数目
	int		iDoubleTractorCount;			//两对拖拉机数目
	int		iThreeTractorCount;				//三条拖拉机数目
	BYTE	iCardHuaKind;					//扑克花色

	BYTE	iBigSignedCard;					//最大单牌
	BYTE	iLessSignedCard;				//最小单牌
	BYTE	iDoubleArray[19];				//对牌数组
	BYTE	iThreeArray[13];				//三条数组
	BYTE	iDoubleTractorArray[54];		//两对拖拉机数组 （0 分割，长度，拖拉机数据）
	BYTE	iThreeTractorArray[48];			//三对拖拉机数组 （0 分割，长度，拖拉机数据）
};

//升级逻辑类 （支持 2 副和 3 副扑克）
class CUpGradeGameLogic
{
	//变量定义
private:
	bool					m_bTwoIsNT;
	BYTE					m_iNTNum;							//主牌数字
	BYTE					m_iNTHuaKind;						//主牌花色
	int						m_iStation[5];						//相对位置（方块，梅花，红桃，黑桃，主牌）
	BOOL					m_bHaveKing;						//是否有王
	//函数定义
public:
	//构造函数
	CUpGradeGameLogic(void);

	//功能函数（公共函数）
public:
	//设置2是否为常主
	void SetTwoIsNT(bool bTwoIsNT){m_bTwoIsNT=bTwoIsNT;}
	//获取2是否为主
	bool GetTwoIsNT(){return m_bTwoIsNT;}
	//设置是否有王主
	void SetHaveKing(BOOL bHaveKing = false){m_bHaveKing = bHaveKing;}
	//获取王主
	BOOL GetHaveKing(){ return m_bHaveKing ;}
	//设置主牌花色
	void SetNTHuaKind(BYTE iNTHuaKind);
	//获取主牌花色
	inline BYTE GetNTHuaKind() { return m_iNTHuaKind; }
	//设置主牌数字
	inline void SetNTCardNum(BYTE iNTNum) { m_iNTNum=iNTNum; }
	//获取主牌数字
	inline BYTE GetNTCardNum() { return m_iNTNum; }
	//获取扑克数字
	inline int GetCardNum(BYTE iCard) { return (iCard&UG_VALUE_MASK)+1; }
	//获取扑克花色
	BYTE GetCardHuaKind(BYTE iCard, BOOL bTrueHua = true);
	//排列扑克
	BOOL SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount);
	//删除扑克
	int RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount);

	//提取指定的牌
	BYTE TackOutBySpecifyCard(BYTE iCardList[], int iCardCount,BYTE bCardBuffer[],int &iResultCardCount,BYTE bCard);
	//获取指定牌个数
	BYTE GetCountBySpecifyCard(BYTE iCardList[],int iCardCount,BYTE bCard);

	//辅助函数
public:
	//是否单牌
	inline BOOL IsOnlyOne(BYTE iCardList[], int iCardCount) { return iCardCount==1; };
	//是否对牌
	BOOL IsDouble(BYTE iCardList[], int iCardCount);
	//是否三条
	BOOL IsThree(BYTE iCardList[], int iCardCount);
	//是否拖拉机逻辑
	BOOL IsTrackorRule(BYTE iFirstCard, BYTE iNextCard);
	//是否拖拉机（两条）
	BOOL IsDoubleTrackor(BYTE iCardList[], int iCardCount);
	//是否拖拉机（三条）
	BOOL IsThreeTrackor(BYTE iCardList[], int iCardCount);
	//是否甩牌
	BOOL IsShowCard(BYTE iCardList[], int iCardCount);
	//得到牌列花色
	BYTE GetCardListHua(BYTE iCardList[], int iCardCount);
    //获取牌型
	BYTE GetCardShape(BYTE iCardList[], int iCardCount);
	//获取扑克相对大小
	int GetCardBulk(BYTE iCard, BOOL bExtVol);
	//对比单牌
	BOOL CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard);

	//其他函数
public:
	//获取扑克(根据花色,数字确定牌)
	BYTE GetCardFromHua(int iHuaKind, int iNum);
	//查找分数
	int FindPoint(BYTE iCardList[], int iCardCount);
	//分析函数
	BOOL AnalyseCard(BYTE iCardList[], int iCardCount, CardAnalyseStruct & Analyse);

	//服务器专用函数
public:
	//对比函数
	int CompareCard(BYTE * iCardListArray[], int iCardCount);
	//对比甩牌
	int CompareShowCard(BYTE * iCardListArray[], int iCardCount);
	//是否可以甩牌
	BOOL CanShowCard(BYTE iOutCard[], int iOutCardCount, int iOutStation, BYTE * iCardArray[], int iCardArrayCount, BYTE iResultCard[], int & iResultCardCount);
	//混乱扑克
	BYTE RandCard(BYTE iCard[], int iCardCount,BOOL bHaveKing =true);
	//变化扑克格式
	BOOL FixCardType(BYTE iCardList[], int iCardCount, CardAnalyseStruct & BaseAnalyse, CardAnalyseStruct & OutAnalyse);

	//客户端函数
public:
	//自动出牌函数
	BOOL AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut);
	//是否可以出牌
	BOOL CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount);

	//内部函数
private:
	//清除 0 位扑克
	int RemoveNummCard(BYTE iCardList[], int iCardCount);
	//提取对牌函数
	int TackOutDouble(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard);
	//提取两条拖拉机函数
	int TackOutDoubleTrackor(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard);
	//提取三条函数
	int TackOutThree(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard);
	//提取三条拖拉机函数
	int TackOutThreeTrackor(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard);
	//提取同一花色扑克
public:
	int TackOutCardByHua(BYTE iCardList[], int iCardCount, BYTE iHuaKind, BYTE iCardBuffer[], BOOL bRemoveCard);
	//查找最长的拖拉机
	int	MaxThreeTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[]);
	//查找最长的拖拉机
	int	MaxDoubleTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[]);
	//查找用户手中托拉机数
	int FindTractorNum(BYTE iCardList[],int iCardCount);
	//查找属于某条件的牌
	BOOL TackOutByCondition(BYTE iCardList[],int iCardCount, BYTE iResultCard[],
								int & iResultCardCount,
								BYTE iConditionCard[],int iConditionCardCount);
	//查找==iCard的单牌所在iCardList中的序号(起始位置,到終點位置)
	int GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard);

};
