#pragma once

#include "StdAfx.h"
#include"..\GameMessage\UpgradeLogic.h"

//定时器 ID
#define TIME_SEND_CARD				30				//发牌定时器
#define TIME_FINISH			        31				//结束定时器
#define TIMER_SCORE					32              //叫分定时器
//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束
//玩家状态
#define LW_NO						20				//没有出过牌
#define LW_PASS						21				//已经出过牌
#define LW_NOOUT					22				//不出

//游戏桌类
class CServerGameDesk : public CGameDesk
{
public:
	CUpGradeGameLogic		m_Logic;							//游戏逻辑
private:
	//固定信息
	static BYTE				m_iThinkTime;											//游戏思考时间
	static BYTE				m_iBeginTime;											//游戏开始时间
	//运行信息
	int						bUserPrepare[PLAY_COUNT];								//玩家准备
	int						m_iAddNoOPerate[PLAY_COUNT];							//无操作的次数，达到两次托管
	BYTE					m_bPeopleHandleCard[PLAY_COUNT][CARD_NUMS];				//玩家手中扑克
	BYTE					bLastFistOutPeople;										//上一轮第一个出牌者
	BYTE                    m_bNextStation;											//当局叫分者
	BYTE					bNtPeople;												//庄家
	bool					bIsCanStart;											//是否可以开始游戏
	bool					bIsStart;												//新一轮出牌是否开始
	bool					bEndPress;												//有人出完牌，另外两个玩家是否将它的牌压住
	BYTE					bDeskCard[52];											//当局用户扑克
	int						bPeopleStation[PLAY_COUNT];								//玩家状态
	__int64					m_iPoint[PLAY_COUNT];									//玩家分数
	BYTE					bHavePointCard[12];										//当局有分牌
	BYTE					m_bPeoplePoint[PLAY_COUNT][12];							//每个人抢的有分牌
	int						m_iPeopleHandCount[PLAY_COUNT];							//玩家手中扑克数量
	BYTE					m_bLeftCard[CARD_NUMS];							        //上家出的牌
	BYTE					bOutBigPeople;											//此轮拥有最大牌的玩家
	BYTE					m_DeskBehave[PLAY_COUNT][CARD_NUMS];					//每个人桌面上的表现，用于断线重连
	int						m_iClockLabel;											//时钟标签
	bool					m_IsCut[PLAY_COUNT];									//是否断线
	bool					m_IsAuto[PLAY_COUNT];									//是否托管
	//配牌所用
	int						m_iIsMatch;										//是否配牌
	BYTE					m_bMatchCard[CARD_NUMS];						//所配的牌
	BYTE					m_bMatchPeople;									//配牌人
	vector<BYTE>            m_vFinishSort;									//出完牌的玩家排序
	//函数定义
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	//初始化游戏逻辑
	virtual bool InitDeskGameStation();
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	//框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);
	//用户断线重来
	virtual bool UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo);

	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//处理函数
public:
	//读取静态变量
	BOOL LoadIni();	//准备发牌工作
	//加载限注
	BOOL LoadExtIni();
	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);
	//超级用户验证
	void SuperUserExamine(BYTE bDeskStation);
	//是否超端用户 超端用户要发送结果给他看到
	bool IsSuperUser(BYTE byDeskStation);
	///清理所有计时器
	void KillAllTimer();
	//按钮消息服务函数
	void OnButtonMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	
	//托管
	void Auto(BYTE bDestStation);
	//取消托管
	void CancelAuto(BYTE bDestStation);
	//统计断线人数
	int GetCutNum();
	//统计托管人数
	int GetAutoNum();
	//自动出牌
	BOOL AutoOutCard(BYTE bDeskStation,bool bFlag=true);
	//游戏结束清除所有断线玩家
	void CleanUpCut();
public:
	//开始发牌
	void SendCard();
	//开始游戏
	void StartGame();
	//得到第一个出牌者
	BYTE GetFirstOutCard();
	//初始化新一轮出牌数据
	void InitRound();
	//检查是否可以开始新的一轮
	bool CheckRound();
	//得到下一个出牌玩家
	BYTE GetNextPeople(BYTE bCurrentPeople);
	//检查是否可以结束
	bool CheckFinish(int bPeopleCount[],int iCount);
	//通知客服端显示牌型得分
	void InformShowStyle();
	////最后一名摊牌
	void InformShowCard();
	//是否配牌
	bool IsMatch();
	//对玩家按得分多少进行排序
	void Ranking(BYTE rank[],int iRandCount,__int64 i64Point[],int iPointCount);
	//得到出完牌的玩家
	int GetOutputEndCard();
	//重新整理扑克
	void ResetCard(BYTE (* bHandleList)[CARD_NUMS]);
	//桌面收分
	void CollectPoint(BYTE bDeskStation);
};

/******************************************************************************************************/
