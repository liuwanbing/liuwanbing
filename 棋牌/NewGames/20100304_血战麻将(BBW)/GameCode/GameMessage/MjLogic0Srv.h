#pragma once

#include "UserData.h"
//#include "..\GameMessage\ThingStruct.h"

#include "..\ChangeFile\GameDataEx.h"
#include "..\ChangeFile\UserDataEx.h"
enum
{
	RESULT_TYPE_NONE_WIN_LOSE = 0,
	RESULT_TYPE_LIUJU,
	RESULT_TYPE_ZIMO,
	RESULT_TYPE_BEI_ZIMO,
	RESULT_TYPE_HUPAI,
	RESULT_TYPE_FANGQIANG
};
/// 服务器麻将逻辑类
class  CMjLogic0Srv
{
public:

	/// 玩家是否听
	bool			m_bisTing[4];				
	///是否托管
	bool			m_bistuoguan[4];							
	/// 门牌数据
/*	MjMenPaiData    m_MenPai;                  
	/// 麻将常规属性
	MjRuleSet       m_mjRule;					
	/// 麻将动作属性
	ActionRuleSet   m_mjAction;               
	/// 麻将算分属性
	CountFenRuleSet m_mjFenRule;				
	/// 麻将糊牌属性
	HuPaiRuleSet    m_mjHuRule;				
	/// 麻将糊牌类型
	HuPaiTypeSet	m_mjHuType;				
	/// 麻将糊牌类型番数
	HuPaiFanSet		m_mjHuFan;		

	///玩家的数据：手牌，出牌，吃碰杠牌等
	UserData       m_UserData[4];		*/	
	
	/// 游戏状态
	BYTE			m_GameStatus;			
	/// 现在出牌用户
	BYTE			m_iOutCardPeople;		
	///庄家位置
	BYTE			m_byNtStation;				
	/// 正精
	BYTE			byMainJing;		
	/// 副精
	BYTE			byLessJing;	
	/// 正精数量
	BYTE			byMainJingNum[PLAY_COUNT];	
	/// 副精数量
	BYTE			byLessJingNum[PLAY_COUNT];
	/// 判断玩家是否吃碰杠糊精牌。，记录最后一次。
	BYTE			m_byCPGHjing[PLAY_COUNT];				
	///玩家能否离开
	bool            m_bUserCanLeave;
	///断线玩家数
	bool			m_bFoceLeave[PLAY_COUNT];	
	///断线玩家数
	int				m_iFoceLeavCount;							
	///每张牌的出牌次数
	BYTE            m_byOutCount[60];	
	///记录麻将的动作，从开始事件以后到结束只记录1个事件：0类型，1位置，2牌值（不记录托管，申请离开等与流程无关的事件）
	BYTE			m_byRecordAction[10][3];

	///麻将将号
	GUID			m_GuidJiangNum;
	///////////////////提牌器变量//////////////////////////////////////
	bool			m_bhaveset[4];//lym0806 super
	int				m_isetcount[4];//lym0806 super
	BYTE			m_setpai[4][20];//lym0806 super
	////////////////////////////////////////////////////////////////////
	///记录玩家的逻辑位置：0东，1北，2西，3南
	BYTE           m_byLogicStation[4];

public:
	/// 算分
	void CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen,UINT uBasePoint=1, bool bMultiHePai = false);
	/// 比较s1,s2谁先出牌 ，s1 优先则返回1，s2则返回-1，相等则返回0
	int CompareStation(int s1,int s2);
	/// 比较出牌优先级 c1， 优先则返回1 ，c2则返回-1， 相等则返回0
	int Compare();
	/// 得到下家
    int GetNextStation(int station,bool shun=false);
	///得到上一家位置
    int GetUpStation(int station);
	
public:
	/// 发牌
	virtual void DisPatchCard();
	///庄家跳牌
	void TiaoPai();
	///描述：获取手好牌
	void GetGoodPai();
	///抓一张牌
	int GetPai(int station,bool head);
	///获得剩余门牌的张数
	int GetReMainMenPaiNum();
	/// 扔色子
	void CastSeZi(BYTE *sezi0=NULL,BYTE *sezi1=NULL,BYTE *sezi2=NULL);
	/// 初始化
	virtual void Init();
	///获取下家庄家位置
	int GetNextNt(int station,bool nowin=false);
	///转换门牌数据,station起牌方位，index敦数
	void ChangeMenPai();


	///初始化数据
	CMjLogic0Srv(void);
	virtual~CMjLogic0Srv(void);//lu20090116过c++test添加虚类型


public:

	///描述：根据色子值确定正精
	void MakeMainJing(BYTE sezi0,BYTE sezi1);
	///描述：根据正精确定副精
	void MakeLessJing();
	///记录玩家的动作
	void RecordUserAction( BYTE ActionSt,BYTE BeSt,BYTE ActionType0,BYTE ActionType1,BYTE pai);

	///描述：计算精牌个数
	void CountJingPaiNum();

	///获取逻辑位置
	BYTE GetLogicStation(BYTE station);


public:


};


