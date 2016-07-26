#pragma once
#include "../ChangeFile/UpgradeMessageEx.h"

///游戏数据管理类:配置数据，游戏基本数据等，对数据进行插入修改等动作
class GameData
{
public:
	GameData(void);
	~GameData(void);
	
public:	        
	/// 麻将常规属性
	MjRuleSet				m_mjRule;					
	/// 麻将动作属性
	ActionRuleSet			m_mjAction;               
	/// 麻将算分属性
	CountFenRuleSet			m_mjFenRule;				
	/// 麻将糊牌属性
	HuPaiRuleSet			m_mjHuRule;				
	/// 麻将糊牌类型
	HuPaiTypeSet			m_mjHuType;				
	///记录游戏所有的流程事件（抓牌，出牌，吃碰杠糊，补花等，最后一个元素299记录事件总数）
	BYTE					m_byThingRecord[300];

	/// 每个事件的后接事件（最大100个事件）
	BYTE					m_byThingNext[100];

	bool					m_bShowUserId;				//是否显示所有玩家的id

	////////////基本数据定义/////////////////////////////////////////////////////////
	///上次发生的事件号
	BYTE					m_byThingLast;		
	///正在发生的事件号
	BYTE					m_byThingDoing;  
	///是否可以离开游戏
	bool					m_bCanLeaveGame;
	///待执行事件
	BYTE					m_byNextAction;
	///逃跑要扣的罚分
	int						uRunPublish;					
	///房间的倍率
	int						basepoint;	
	///要申请的事件id
	BYTE					ApplyThingID;

	///吃牌牌事件,临时数据
	tagChiPaiEx				temp_ChiPai;
	///碰牌牌事件,临时数据
	tagPengPaiEx			temp_PengPai;
	///杠牌牌事件,临时数据
	tagGangPaiEx			temp_GangPai;
	////糊牌牌事件,临时数据

	////////////事件数据对象定义///////////////////////////////////////////////////////

	///游戏开始事件
	tagBeginEx				T_Begin;
	///以东为庄事件
	tagDongNtEx				T_DongNt;
	///掷2颗色子的点数和为庄事件
	tagTwoSeziNtEx			T_TwoSeziNt;
	///掷2颗色子确定庄家和起牌点位置事件
	tagTwoSeziNtAndGetPaiEx	T_TwoSeziNtAndGetPai;
	///轮流掷2颗色子确定庄家
	tagTurnSeziNtEx			T_TurnSeziNt;
	///掷2颗色子确定起牌位置事件
	tagTwoSeziDirEx			T_TwoSeziDir;
	///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件
	tagTwoSeziDirAndGetPaiEx	T_TwoSeziDirAndGetPai;
	///掷2颗色子确定起牌敦数事件
	tagTwoSeziGetPaiEx		T_TwoSeziGetPai;
	///发牌事件
	tagSendPaiEx			T_SendPai;
	///跳牌事件
	tagTiaoPaiEx			T_TiaoPai;
	///掷色子2颗色子定精牌事件
	tagTwoSeziJingEx		T_TwoSeziJing;
	///掷色子1颗色子定金牌事件
	tagOneSeziJinEx			T_OneSeziJin;
	///所有玩家补花事件
	tagAllBuHuaEx			T_AllBuHua;
	///单个玩家补花事件
	tagOneBuHuaEx			T_OneBuHua;
	///开始出牌通知事件
	tagBeginOutPaiEx		T_BeginOutPai;
	///出牌事件
	tagOutPaiEx				T_OutPai;
	///抓牌牌事件
	tagZhuaPaiEx			T_ZhuaPai;
	///吃碰杠糊牌通知事件牌事件
	tagCPGNotifyEx			T_CPGNotify[4];
	///玩家自己的吃碰杠数据（客户端使用）
	tagCPGNotifyEx			T_Me_CPGNotify;
	///吃牌牌事件
	tagChiPaiEx				T_ChiPai;
	///碰牌牌事件
	tagPengPaiEx			T_PengPai;
	///坎牌牌事件
	tagKanPaiEx				T_KanPai;
	///扫虎事件
	tagSaoHuEx				T_SaoHu;
	///杠牌牌事件
	tagGangPaiEx			T_GangPai;
	///听牌牌事件
	tagTingPaiEx			T_TingPai;
	///糊牌牌事件
	tagHuPaiEx				T_HuPai;
	///算分事件
	tagCountFenEx			T_CountFen;
	///游戏结束处理事件
	tagEndHandleEx			T_EndHandle;
	///非正常结束处理事件
	tagUnnormalEndHandleEx   T_UnnormalEndHandle;

	BYTE       m_byThingLog[10][5];//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
	bool					m_bGangState[4];//记录杠牌状态，拥于杠开和杠后炮

public:

	///设置所有事件的后接事件
	virtual void SetThingNext();
	///设置某个事件的后接事件
	virtual void SetThingNext(BYTE id);
	//virtual 初始化数据
	virtual void InitData();
	///初始化数据
	virtual void LoadIni();

};