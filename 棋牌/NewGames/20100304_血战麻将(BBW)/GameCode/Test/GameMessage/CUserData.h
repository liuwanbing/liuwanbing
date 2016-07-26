#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "CLogicBase.h"

///玩家数据管理类
class CUserData
{
public:
	CUserData(void);
	~CUserData(void);
	
public:	
	/// 门牌数据
	MjMenPaiData    m_MenPai;      

	///手牌不超过20张，0为结束标志
	BYTE m_byArHandPai[4][HAND_CARD_NUM];
	///出牌不超过160张，0为结束标志
	BYTE m_byArOutPai[4][OUT_CARD_NUM];
	///花牌不超过20张，0为结束标志
	BYTE m_byArHuaPai[4][HUA_CARD_NUM];
	///门牌不超过40张
	BYTE m_iArMenPai[4][MEN_CARD_NUM];
	///转换后的门牌数据
	BYTE m_byMenPaiData[4][MEN_CARD_NUM/2][2];

	///关系数据
	BYTE		m_byGuanXi[4][4];
	///吃碰杠
	GCPStructEx	m_UserGCPData[4][5];
	///是否可用的牌
	bool		m_bEnableCard[4][HAND_CARD_NUM]; 

	///庄家位置
	BYTE		m_byNtStation;	
	///当前玩家的逻辑位置（客户端使用）
	BYTE        m_byStation;
	///最后打出的牌
	BYTE        m_byLastOutPai;
	///最后抓到的牌
	BYTE		m_byLastZhuaPai[4];
	///当前玩家最后抓拍
	BYTE        m_byMeZhuaPai;
	///当前出牌位置
	BYTE		m_byNowOutStation;
	///当前抓牌方向，true 顺时针，false 逆时针
	bool		m_bZhuaPaiDir;
	///上次出牌位置
	BYTE		m_byLastOutStation;
	///当前其他玩家打出的牌
	BYTE		m_byOtherOutPai;
	///停牌或其他操作后能打出的牌
	BYTE       m_byCanOutCard[4][HAND_CARD_NUM];
	///游戏已经进行的局数（强退，安全退出，解散等重新开始计算）
	BYTE		byPlayCount;
	///圈风
	BYTE		m_byQuanFeng;
	///门风
	BYTE		m_byMenFeng[4];

	//玩家是否返回了色子结束消息
	bool		m_bSiziFinish[4];
	//是否处理等待色子动画结束状态
	bool		m_bWaitSezi;
	///玩家离开的数量
	BYTE		m_byFoceLeavCount;

	///当前选中排索引（客户端使用）
	BYTE        m_bySelectIndex;
	///精牌数据
	JingStruct  m_StrJing;

	///控件变量
	bool                m_bIsBoy[4];
	///发牌数据
	SuperStruct			m_SetData[4];
	///设置下一张摸牌（测试使用）
	BYTE				m_bySetMoPai[4];

	BYTE				m_byApplyMaxAction;//玩家当前申请的最大事件
	BYTE				m_byLianZhuang;//连庄数

	//bitset< MAX_HEPAI_TYPES > re;
	//re.reset();
	//四个玩家的状态，每个玩家32位，可以代表32个状态。前10 框架使用，后面的后备使用
	bitset< 128 >m_bitUserState;

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
	///删除几张指定的手牌
	virtual void DelleteSomeHandPai(BYTE station,BYTE pai[],BYTE len=1);
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
	///通过索引获取一张手牌
	virtual BYTE GetHandPaiFromIndex(BYTE station,int index);
	///通过索引设置一张手牌
	virtual bool SetHandPaiFromIndex(BYTE station,int index,BYTE pai);
	///索引是否有牌
	virtual bool IsHaveHandPaiFromIndex(BYTE station,int index);
	///描述：是否存在某种花色的牌
	virtual bool IsHaveHuaSe(BYTE station,BYTE huase);
	///游戏开始时发牌
	virtual void SendCard();
	///描述：抓一张牌
	virtual BYTE GetPai(BYTE &index,int station,bool head,BYTE type=0);
	///描述：获取牌墙中的一张牌
	virtual BYTE GetPaiFromMenPai(BYTE &index,bool head);
	///描述：将抓牌索引转换成抓牌玩家的门牌索引
	virtual bool ZhuaPaiIndex(BYTE &index,BYTE &dir);

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
	///拷贝杠吃碰数组中的数据组
	virtual void CopyAllGCPData(GCPStructEx gcpData[][5]);
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
	///拷贝所以玩家的出牌
	virtual void CopyAllOutPai(BYTE pai[][OUT_CARD_NUM]);
	///获取玩家的出牌数量
	virtual int GetOutPaiCount(BYTE station);
	///描述：获取玩家某张牌的出牌数量
	virtual int GetOneOutPaiCount(BYTE station,BYTE pai);
	///获取玩家的最后一张出牌
	virtual BYTE GetLastOutPai(BYTE station);
	//////////////////花牌操作///////////////////////////////////////////////////////

	///添加一张花牌到花牌数组中
	virtual void AddToHuaPai(BYTE station,BYTE pai);
	///设置花牌数据
	virtual void SetHuaPaiData(BYTE station,BYTE pai[]);
	///拷贝玩家的花牌
	virtual int CopyHuaPai(BYTE station,BYTE pai[]);
	///拷贝所有玩家的花牌
	virtual void CopyAllHuaPai(BYTE pai[][HUA_CARD_NUM]);
	///检测是否花牌
	virtual bool CheckIsHuaPai(BYTE pai);

	///////////////////门牌牌操作///////////////////////////////////////////////////////

	///设置门牌数据
	virtual void SetMenPaiData(BYTE station,BYTE pai[]);
	///拷贝玩家的门牌
	virtual int CopyMenPai(BYTE station,BYTE pai[]);
	///拷贝所有玩家的门牌
	virtual void CopyAllMenPai(BYTE pai[][MEN_CARD_NUM]);
	///描述：门牌转换
	virtual void ChangeMenPai();

	///打乱牌数据：每局发牌前调用
	virtual void RandCard();
	///设置发牌中的门牌,send 已经发的次数
	virtual void SetMenPaiFaPai(BYTE num);

	///////////////////财神操作//////////////////////////////////////////////
	/////检测pai是否财神
	//virtual bool CheckIsCaiShen(BYTE pai);
	/////清空财神结构
	//virtual void InitCaiShen();


	///////////////////色子操作////////////////////////////////////////
	//玩家是否已经播放完了色子：station 玩家位置，返回某个玩家是否结束
	virtual bool IsOneSeziFinish(BYTE station);
	//玩家是否已经播放完了色子：返回是否所有玩家结束色子
	virtual bool IsAllSeziFinish();
	//设置等待色子结束
	virtual void SetWaiteSezi(bool wait);

	///////////////////玩家状态操作/////////////////////////////////////////////
	//设置/取消玩家的状态
	virtual void SetUserState(BYTE station,BYTE type,bool state);
	//取消所有玩家的某种状态
	virtual void ReSetUserState(BYTE type);
	//获取玩家的状态
	virtual bool IsUserState(BYTE station,BYTE type);
	//是否所有玩家都都处于某种状态
	virtual bool IsAllUserState(BYTE type);

	///////////////////玩家动作操作/////////////////////////////////////////////
	//设置/取消玩家的状态
	//virtual void SetUserMaxAction(BYTE station,BYTE type,bool state);


	/////////////////超级客户端设置牌操作///////////////////////
	///换所有手牌
	virtual void ChangeAllHandPai(BYTE station,BYTE pai[],BYTE count);
	///换指定手牌
	virtual void ChangeSomeHandPai(BYTE station,BYTE pai[],BYTE handpai[],BYTE count);
	///设置自己的下一张牌
	virtual void SetMeNextPai(BYTE station,BYTE pai);
	///设置牌墙的下一张牌
	virtual void SetGameNextPai(BYTE pai);
	///超级客户端抓牌（将随机抓的牌换成想要抓的牌）
	virtual void SuperZhuaPai(BYTE &pai);

/////////////////////////////////////////////////////

	///获取牌数组的有效牌张数
	virtual int GetPaiNum(BYTE station,BYTE pai[],BYTE count);
	///初始化数据
	virtual void InitData();
	
};
