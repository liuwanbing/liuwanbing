#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "../GameMessage/UserData.h"

///玩家数据管理类
class UserDataEx: public UserData
{
public:
	UserDataEx(void);
	~UserDataEx(void);
	
public:	
	BYTE	m_byCaiPiaoStation;//财飘位置，该值不为255是不允许吃碰杠和放炮（杭州麻将专用）
	BYTE    m_byGuanXi[PLAY_COUNT][PLAY_COUNT]; //吃碰杠关系
	bool	m_bGangKai[PLAY_COUNT];//杠开状态玩家
	BYTE    m_byLianZhuang;//连庄数
	bool    m_bTianHu;//庄家天糊状态
	bool	m_bOutJing[PLAY_COUNT];//玩家是否打出了财神牌
	bool	m_bChanPai;		//玩家是否铲牌

	bool   m_bLookBao[PLAY_COUNT];//看宝状态
	
	BYTE   m_byFirstHandPai[PLAY_COUNT][HAND_CARD_NUM];//第一手牌，出牌，吃碰杠后抓到的不算
	
	BYTE    m_byDingQue[PLAY_COUNT];//定的缺，0表万  1表条 2表筒
    
    int    m_byGFXY[PLAY_COUNT][PLAY_COUNT+1];   //刮风下雨所得
                                    //刮风（补杠） 向未胡者收1
                                    //下雨（暗杠） 向未胡者收2
                                    //下雨（直杠） 向引杠都收2

    int    m_byGengCount[PLAY_COUNT];//糊牌时根数

    bool    m_bIsHu[PLAY_COUNT];//是否胡牌

	bool	m_bIsGangPao[PLAY_COUNT];//正在杠后炮的状态
	bool    m_bQiHu[PLAY_COUNT];//玩家是否弃糊状态

	BYTE    m_NoFenGang[PLAY_COUNT][4];//记录无分杠（补杠，手中有4张牌先碰后杠无分，抓牌后能补杠但是不在当前圈杠无分）

	BYTE	m_byTempNt;//记录下局换庄玩家
	//补杠得分的牌
	BYTE    m_byBuGangPai;

	//记录每个杠牌
	GangFenRecordEx m_stGangFen[PLAY_COUNT];

public:
	
	//////////////////手牌操作/////////////////////////////////////

	///检查是否存在指定的手牌
	virtual bool IsHaveAHandPai(BYTE station,BYTE pai);
	///检查手牌中某张牌的个数
	virtual int GetAHandPaiCount(BYTE station,BYTE pai);
    ///检查某种花色牌的个数（0万 1筒 2条)
    int GetAKindPaiCount(BYTE station,BYTE kind);
    ///得到某种花色牌（0万 1筒 2条)
    int GetAKindPai(BYTE station,BYTE kind);
	///检查手牌的张数
	virtual int GetHandPaiCount(BYTE station);
	///拷贝玩家的手牌
	virtual int CopyHandPai(BYTE pai[][HAND_CARD_NUM],BYTE station,bool show);
	///描述：拷贝某个玩家的手牌
	virtual int CopyOneUserHandPai(BYTE pai[HAND_CARD_NUM] ,BYTE station);
	///手牌排序，small是否从小到大排
	virtual void SortHandPai(BYTE station,bool big);
	///添加一张牌到手牌中
	virtual void AddToHandPai(BYTE station,BYTE pai);
	///设置手牌数据
	virtual void SetHandPaiData(BYTE station,BYTE pai[]);
	///删除一张指定的手牌
	virtual void DelleteAHandPai(BYTE station,BYTE pai);
	///设置手牌牌背
	virtual void SetHandPaiBack(BYTE station,BYTE num);
	///获得最后一张手牌
	virtual BYTE GetLastHandPai(BYTE station);
	///获得选中的一张手牌
	virtual BYTE GetSelectHandPai(BYTE station);
	///是否出牌玩家
	virtual bool IsOutPaiPeople(BYTE station);
	///检测手牌中是否存在花牌
	virtual bool IsHuaPaiInHand(BYTE station);
	///将手牌中的花牌移到花牌数组中
	virtual int MoveHuaPaiFormHand(BYTE station);

	///获取手牌的牌色数
	virtual int GetPaiSeCount(BYTE station);
	///是否还有缺门的牌
	bool IsHaveQueMen(BYTE station,BYTE type);
	//获取一张data中没有的牌，用来换牌
	BYTE GetChangePai(BYTE station,BYTE data[]);

	//////////////////吃碰杠牌操作/////////////////////////////////////

	///获取玩家杠牌的个数(暗杠，明杠，补杠)
	virtual int GetUserGangNum(BYTE station,BYTE &an,BYTE &ming , BYTE &bu);
	///玩家吃牌次数
	virtual int GetUserChiNum(BYTE station);
	///玩家杠牌次数
	virtual int GetUserPengNum(BYTE station);
	///玩家杠了某个牌
	virtual bool IsUserHaveGangPai(BYTE station,BYTE pai,BYTE &type);
	///玩家是否碰了某张牌
	virtual bool IsUserHavePengPai(BYTE station,BYTE pai);
	///玩家是否吃过某张牌
	virtual bool IsUserHaveChiPai(BYTE station,BYTE pai);
	///添加一组数据到杠吃碰数组中
	virtual void AddToGCP(BYTE station,GCPStructEx *gcpData);
	///删除刚吃碰数组中指定的数据组
	virtual void DelleteAGCPData(BYTE station,BYTE type ,BYTE pai);
	///拷贝杠吃碰数组中的数据组
	virtual int CopyGCPData(BYTE station,GCPStructEx gcpData[]);
	///设置吃碰数组中的数据组
	virtual void SetGCPData(BYTE station,GCPStructEx gcpData[]);
	///获取杠吃碰的节点个数
	virtual int GetGCPCount(BYTE station);
	///描述：获取杠吃碰的中某种牌的数量
	virtual int GetOnePaiGCPCount(BYTE station,BYTE pai);
	///排序杠吃碰数组
	virtual void ShortGCP(BYTE station);
	///描述：初始化杠吃碰数组
	virtual void InitGCP(BYTE station);


	///////////////////出牌操作///////////////////////////////////////////////////////

	///添加一张牌到出牌数组中
	virtual void AddToOutPai(BYTE station,BYTE pai);
	///删除最后一张出牌
	virtual void DelleteLastOutPai(BYTE station);
	///设置出牌数据
	virtual void SetOutPaiData(BYTE station,BYTE pai[]);
	///拷贝玩家的出牌
	virtual int CopyOutPai(BYTE station,BYTE pai[]);
	///获取玩家的出牌数量
	virtual int GetOutPaiCount(BYTE station);
	///描述：获取玩家某张牌的出牌数量
	virtual int GetOneOutPaiCount(BYTE station,BYTE pai);

	//////////////////花牌操作///////////////////////////////////////////////////////

	///添加一张花牌到花牌数组中
	virtual void AddToHuaPai(BYTE station,BYTE pai);
	///设置花牌数据
	virtual void SetHuaPaiData(BYTE station,BYTE pai[]);
	///拷贝玩家的花牌
	virtual int CopyHuaPai(BYTE station,BYTE pai[]);

	///检测是否花牌
	virtual bool CheckIsHuaPai(BYTE pai);

	///////////////////门牌牌操作///////////////////////////////////////////////////////

	///设置门牌数据
	virtual void SetMenPaiData(BYTE station,BYTE pai[]);
	///拷贝玩家的门牌
	virtual int CopyMenPai(BYTE station,BYTE pai[]);

/////////////////////////////////////////////////////
	
	///获取牌数组的有效牌张数
	virtual int GetPaiNum(BYTE station,BYTE pai[],BYTE count);
	
	///初始化数据
	virtual void InitData();
	
	///描述：玩家是否存在某种牌的大杠
	bool IsUserHaveBigGangPai(BYTE station,BYTE pai);

	///描述：将抓牌索引转换成抓牌玩家的门牌索引
	bool ZhuaPaiIndex(BYTE &index,BYTE &dir);		

};

//全局对象
extern UserDataEx g_UserData;