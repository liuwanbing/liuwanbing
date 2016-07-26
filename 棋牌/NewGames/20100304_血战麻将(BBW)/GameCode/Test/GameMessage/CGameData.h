#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "CLogicBase.h"
#include "cstring.h"

///游戏数据管理类:配置数据，游戏基本数据等，对数据进行插入修改等动作
class CGameData
{
public:
	CGameData(void);
	~CGameData(void);
	
public:	        
	/// 麻将常规属性
	MjRuleSet				m_mjRule;			
	/// 麻将动作属性
	ActionRuleSet			m_mjAction;          				
	/// 麻将糊牌类型
	HuPaiTypeSet			m_mjHuType;		
	/// 每个事件的后接事件（最大100个事件）
	BYTE					m_byThingNext[100];
	//是否显示所有玩家的id
	bool					m_bShowUserId;
	////////////基本数据定义/////////////////////////////////////////////////////////	
	///正在发生的事件号
	BYTE					m_byThingDoing;  
	///游戏当前状态：出牌前状态，出牌状态，拦牌状态，抢杠状态，游戏结束状态，其他状态
	BYTE					m_byGameState; 

	///逃跑要扣的罚分
	int						uRunPublish;				
	///房间的倍率
	int						basepoint;
	///等待执行的事件id
	BYTE					ApplyThingID;
	//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
	BYTE					m_byThingLog[10][5];
	
	
	///吃牌牌事件,临时数据
	tagChiPaiEx				temp_ChiPai;
	///碰牌牌事件,临时数据
	tagPengPaiEx			temp_PengPai;
	///杠牌牌事件,临时数据
	tagGangPaiEx			temp_GangPai;
	////糊牌牌事件,临时数据


	//////////// 事件数据对象定义 ///////////////////////////////////////////////////////

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
	///掷色子2颗色子定精牌事件
	tagTwoSeziJingEx		T_TwoSeziJing;
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
	tagUnnormalEndHandleEx  T_UnnormalEndHandle;
	
	BYTE					m_byApplyMaxAction;//玩家当前申请的最大事件
	///待执行事件
	BYTE					m_byNextAction;


	///麻将时间设置
	MJ_SettingEx    m_MjSet;
	///麻将时间等待：时间到了服务器代为处理
	MJ_WaitTimeEx   m_MjWait;
	
public:
	
	///设置所有事件的后接事件
	virtual void SetThingNext();
	///设置某个事件的后接事件
	virtual void SetThingNext(BYTE id);
	//virtual 初始化数据
	virtual void InitData();
	///初始化数据
	virtual void LoadIni();
	//加载牌数据
	virtual BYTE LoadPaiData(MjMenPaiData & menpai);
	
	
//////////////////玩家操作////////////////////////////////////////

	///收到玩家的动作请求
	virtual void AcceptAction(BYTE station,BYTE type);
	///获取最大的请求
	virtual BYTE GetMaxRequet(BYTE station);
	///指定请求是否全部回应
	bool IsNotifyAllReply(BYTE station,BYTE request);
	/// 设置超时数据
	virtual void SetOutTime(BYTE station,BYTE time,BYTE thing,bool effect);
	/// 恢复超时数据
	virtual void ReSetOutTime();
	
///////////////客户端吃碰杠操作//////////////////////////////////////////////
	/// 获得玩家的吃牌数量
	virtual int GetUserChiCount(BYTE station,BYTE handpai[]);
	/// 获得玩家的吃牌数量
	virtual int GetUserGangCount(BYTE station,BYTE handpai[]);


	int GetCanGangCount(BYTE station);//获得能杠牌的数量
	int GetCanChiCount(BYTE station);//获得能吃牌的数量


	///清理临时数据
	void ClearTempData();

};

