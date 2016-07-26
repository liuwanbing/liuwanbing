#pragma once

#include "GameDesk.h"
#include "../GameMessage/UpGradeLogic.h"

#include <afxmt.h>
//定时器 ID
#define TIMER_SENDBACKCARD			32				//发送底牌
#define TIMER_ROUNDBOSSOUTCARD		33				//单圈出牌最大的玩家出牌定时器
#define TIMER_GAMEOVER				34				//游戏结束
#define TIMER_SENDOTHERCARD			35				//发送其他玩家牌数据给超端
#define TIMER_SENDCARD   			36				//发牌定时器
#define TIMER_THINK   			    37				//思考定时器
#define TIMER_BACK   			    38				//埋底牌定时器


//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SAFE						11				//游戏安全结束
#define GF_TERMINATE				12				//意外停止游戏


#define SingleDeckCard				54				//一副牌张数
#define NumsOfDeckCard				2				//几副牌
const BYTE sByFaceDeskStation[PLAY_COUNT]={2,3,0,1};//对家位置
//每一轮牌数据
struct TTurnCardData
{
	int iCardNums[PLAY_COUNT];		//牌张数
	BYTE byCards[PLAY_COUNT][25];		//牌数据
	TTurnCardData()
	{
		memset(this,0,sizeof(TTurnCardData));
	}
};
//*****************************************************************************************
//游戏桌类

class CServerGameDesk : public CGameDesk
{
	//静态变量
private:
	
	CMutex  m_Mutex;

	vector<BYTE>			m_vbyAllCards;		//所有牌
	CUpGradeGameLogic		m_Logic;			//


protected:
	bool					m_bStartFlag;//启动游戏标志
	bool					m_bQiangNt;//是否可以抢庄
	//玩家准备状态
	bool                    m_bUserReady[PLAY_COUNT];		//玩家准备状态
	vector <long>			m_vlSuperID;					//保存超端玩家的容器
	bool					m_bUserNetCut[PLAY_COUNT];		//标识玩家是否断线
	//游戏信息
	BYTE					m_byGroupOneevel;				//第一组玩家的当前级数 -- 庄家
	BYTE					m_byGoupTwolevel;				//第二组玩家的当前级数 -- 闲家
	bool					m_bIsCalled;					//是否已经有人叫主
	BYTE					m_byRoundOutCardCount;			//每圈出牌人数
	BYTE					m_byFirstOutCardDeskStation;	//每圈首家的座位号
	TFirstData				m_tRoundCard[PLAY_COUNT];		//每圈每个玩家的出牌数据
	vector<BYTE>			m_vPlayerCards[PLAY_COUNT];		//当前每个玩家手中的牌数据
	T_CS_OutCard			m_tRoundFirstCard;				//单圈中第一个玩家出的牌
	TTurnCardData           m_tTurnCardData[30];			//每一轮牌数据
	int						m_iTurnNums;					//记录轮数
	TCallLevelCard			m_tCurrentCall;					//当前叫主
	BYTE					m_byBackCard[8];				//底牌
	int						m_iNoMoreCardCount;				//没有牌的人数

	BYTE					m_byNextOutCardPlayer;			//下个出牌的玩家座位号
	int						m_iRoundScore;					//单圈得分
	int						m_iTotalScore;					//总分
	int                     m_iBackCardScore;				//底牌分数
	__int64					m_i64Score[PLAY_COUNT];			//结算金币					
	//游戏配置信息
	int						m_iTimeToReady;					//准备时间
	int						m_iTimeToCallNT;				//叫主时间
	int						m_iTimeToThink;					//思考时间
	int						m_iSendCardSpeed;				//发牌速度
	//超端
	vector<long>			m_vSuperUsers;						//超端用户
	bool					m_bySuperDeskStation[PLAY_COUNT];	//超端用户座位号

	//函数定义
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	//判断是否正在游戏
	virtual bool IsPlayGameEx(BYTE bDeskStation);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	///用户断线离开
	bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
    ///拦截窗口消息
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用来改变用户断线条件的函数
	virtual bool CanNetCut(BYTE bDeskStation);//JAdd-20090116
	//读取静态变量
	BOOL LoadIni();	
	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//配置INI文件
	virtual bool InitDeskGameStation();
	//处理准备消息
	bool OnHandleAgree(BYTE byDeskStation,void *pBuffer,int iSize);
	//处理埋底牌消息
	bool OnHandleBackCard(BYTE byDeskStation,void *pBuffer,int iSize);
	//处理出牌消息
	bool OnHandleOutCard(BYTE byDeskStation,void *pBuffer,int iSize);
	//发牌完成
	void SendCardFinish();
	//处理查牌
	bool OnHandleLookCard(BYTE byDeskStation,void *pBuffer,int iSize);
	
	//发牌
	void SendCards();
	//发送洗牌消息
	void SendWashCardMsg();
	//重置游戏级别
	void ResetGameLevel();
	//发送游戏级别消息
	void SendGameLevelMsg();
	// for debug
	void ForDebugging(TSendCards& tSendCards);
	//发送玩家是否准备消息
	void SendPlayerReadyMsg(BYTE byDeskStation=255);
	//叫主
	void CallGameLevelCard(TCallLevelCard *ptCallLevelCard,BYTE byDeskStation);
	//向玩家发送亮牌数据
	void SendCallGameLevelCard(TCallLevelCard *ptCallLevelCard);
	//发送其他玩家的牌给超端
	void SendAllCardsToSuper();
	//记录每轮玩家的牌数据
	void RecordTurnCardData(BYTE byDeskStation,BYTE bCardList[],int iCardCount);
	//修改奖池数据1(平台->游戏)
	/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
	/// @return 是否成功
	virtual bool SetRoomPond(bool	bAIWinAndLostAutoCtrl);

	//修改奖池数据2(平台->游戏)
	/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
	/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[]);

	//修改奖池数据3(平台->游戏)
	/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iReSetAIHaveWinMoney);

	//控制胜负
	/// 判断
	virtual BOOL Judge();
	/// 胜
	virtual BOOL JudgeWiner();
	/// 输
	virtual BOOL JudgeLoser();
	//GRM函数
	//更新奖池数据(游戏->平台)
	/// @return 机器人赢钱数
	virtual void UpDataRoomPond(__int64 iAIHaveWinMoney);
	//日志记录
	void Debuglog(const char *p, ...);
};

/******************************************************************************************************/
