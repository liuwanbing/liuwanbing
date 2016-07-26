#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "../GameMessage/CUserData.h"

///玩家数据管理类
class CUserDataEx: public CUserData
{
public:
	CUserDataEx(void);
	~CUserDataEx(void);
	
public:	
	
	//庄家押注情况：255未曾进行押注，0不押注，1压一半，2压全额
	BYTE	m_byYaZhu;
	//闲家同意押注情况
	bool	m_bYaZhu[PLAY_COUNT];
	//玩家弃糊的番数
	BYTE	m_byQiHuFan[PLAY_COUNT];
	
	//四个方向门牌的数量
	BYTE		m_byMenPaiCount[4];

public:
	
	//////////////////手牌操作/////////////////////////////////////

	///检查是否存在指定的手牌
	virtual bool IsHaveAHandPai(BYTE station,BYTE pai);
	///检查手牌中某张牌的个数
	virtual int GetAHandPaiCount(BYTE station,BYTE pai);
	///检查手牌的张数
	virtual int GetHandPaiCount(BYTE station);
	///拷贝玩家的手牌
	virtual int CopyAllHandPai(BYTE pai[][HAND_CARD_NUM],BYTE station,bool show);
	///描述：拷贝某个玩家的手牌
	virtual int CopyOneUserHandPai(BYTE pai[] ,BYTE station);
	///描述：拷贝某个玩家的部分手牌
	virtual int CopyOneUserHandPai(BYTE pai[] ,BYTE station, BYTE size);
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
	///描述：门牌转换
	virtual void ChangeMenPai();

/////////////////////////////////////////////////////
	
	///获取牌数组的有效牌张数
	virtual int GetPaiNum(BYTE station,BYTE pai[],BYTE count);
	
	///初始化数据
	virtual void InitData();
	
		
	//设置弃糊状态
	void SetQiHu(BYTE station,BYTE fan);
	//清除弃糊状态
	void ReSetQiHu(BYTE station);
	//检测是否弃糊状态(弃糊状态不能点炮糊牌)
	bool CheckQiHu(BYTE station,BYTE fan);

	//设置各个方向门牌的数量
	void	SetMenPaiCount(BYTE count[]);

};
