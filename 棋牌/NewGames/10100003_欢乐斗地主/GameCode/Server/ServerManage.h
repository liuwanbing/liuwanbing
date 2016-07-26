#pragma once

#include "GameDesk.h"
//#include "../GameMessage/UpGradeLogic.h"
#include "GameTaskLogic.h"
#include <afxmt.h>
//定时器 ID
#define IDT_USER_CUT				1L										//断线定时器 ID

#define TIME_SEND_CARD				30				//发牌定时器
#define TIME_SEND_ALL_CARD			31				//发所有牌
#define TIME_SEND_CARD_ANI      	32				//等待反牌
#define TIME_SEND_CARD_FINISH       33              //发牌结束
#define TIME_ROB_NT					34				//抢地主
#define TIME_ADD_DOUBLE				35				//加棒
#define TIME_OUT_CARD				36				//出牌
#define TIME_WAIT_NEWTURN			37				//新一轮时间设置
#define TIME_GAME_FINISH			38				//游戏结束定时器
#define TIME_JIAO_FEN               39              //叫分定时器
#define TIME_SHOW_CARD              40              //亮牌


#define TIME_START_GAME				41				//比赛开始时间设置
//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SAFE						11				//游戏安全结束
#define GF_NO_CALL_SCORE			13				//无人叫分
#define GF_TERMINATE				14				//意外停止游戏
#define GF_AHEAD_END				15				//提前结束

//*****************************************************************************************
//游戏桌类

class CServerGameDesk : public CGameDesk
{
	//静态变量
private:
	static 	bool  m_bHaveKing;      			    //是否有王(DEF=1,0)
	static 	BOOL  m_bKingCanReplace;				//王可否代替牌(DEF=1,0)
	static 	bool  m_bRobnt;         				//是否可以抢地主
	static 	bool  m_bAdddouble;    				//是否可以加棒
	static 	bool  m_bShowcard;     				//是否可以加倍
	static 	UINT  m_iPlayCard;     				//所有扑克副数(1,DEF=2,3
	static 	UINT  m_iPlayCount;    				//使用扑克数(52,54,104,DEF=108)
	static 	UINT  m_iSendCount;    				//发牌数(48,51,DEF=100,108)
	static 	UINT  m_iBackCount;    				//底牌数(3,6,DEF=8,12)
	static 	UINT  m_iUserCount;    				//玩家手中牌数(12,13,DEF=25,27)

	static 	DWORD m_iCardShape;	  				//牌型
	//=============扩展
	static 	BYTE m_iThinkTime;               	//游戏思考时间
	static 	BYTE m_iBeginTime;               	//游戏开始时间
	static  BYTE m_iSendCardTime ;           	//发牌时间
	static  BYTE m_iCallScoreTime;           	//叫分时间
	static  BYTE m_iRobNTTime ;              	//抢地主时间
	static  BYTE m_iAddDoubleTime;           	//加棒时间
	static  BOOL m_bTurnRule;					//游戏顺序


	CMutex  m_Mutex;

protected:

	CGameTaskLogic          m_Logic ;                             ///游戏中的逻辑（包含任务逻辑）
	GameMutipleStruct       m_GameMutiple ;                      ///游戏中的倍数

	int                     m_iBaseMult;					   //游戏倍数
	int                     m_iAddDoubleLimit;                  ///加倍限制
	int                     m_iGameMaxLimit ;                  ///房间最大输赢 
	int                     m_iLimitPoint ;                   //游戏最大倍数

	int  	                m_iLimitPlayGame;					   //至少打完多少局
	BYTE					m_iBeenPlayGame;					 	//已经游戏的局数
	BYTE					m_iLeaveArgee;							//离开同意标志
	//状态信息
	BYTE					m_iUserCardCount[PLAY_COUNT];			//用户手上扑克数目
	BYTE					m_iUserCard[PLAY_COUNT][45];			//用户手上的扑克
	BYTE					m_iBackCard[12];						//底牌列表
	BYTE					m_iBaseOutCount;						//出牌的数目
	BYTE					m_iDeskCardCount[PLAY_COUNT];			//桌面扑克的数目
	BYTE					m_iDeskCard[PLAY_COUNT][45];			//桌面的扑克
	//发牌数据
	BYTE					m_iSendCardPos;							//发牌位置
	
	BYTE					m_iAIStation[PLAY_COUNT];				//四家机器托管状态
	BYTE					m_iHaveThingPeople;						//有事要走玩家
	BYTE					m_iGameFlag;							//游戏状态小分解
	
	bool					m_bIsLastCard;						//是否有上轮数据
	BYTE					m_iLastCardCount[PLAY_COUNT];		//上轮扑克的数目
	BYTE					m_iLastOutCard[PLAY_COUNT][45];		//上轮的扑克
	BYTE					m_byteHitPass;						//记录不出
	bool                    m_byPass[PLAY_COUNT] ;             //本轮不出
	bool                    m_byLastTurnPass[PLAY_COUNT]  ;      //上轮不出

	BYTE                    m_bySendFinishCount ;               //发完牌的人

	BYTE					m_bThrowoutCard;						//明牌
	BYTE					m_bFirstCallScore;						//第一个叫地主者
	BYTE					m_bCurrentCallScore;					//当前叫分者

	int						m_iUpGradePeople;						//庄家位置
	int						m_iNtFirstCount;						//地主出的第一手牌数
	//运行信息
	int						m_iOutCardPeople;						//现在出牌用户
	int						m_iFirstOutPeople;						//先出牌的用户
	int						m_iNowBigPeople;						//当前桌面上最大出牌者
	int						m_iRecvMsg;								//收到消息计数
	

	int						m_iCallScore[PLAY_COUNT];
	int						m_iAwardPoint[PLAY_COUNT];				//牌形加分
	bool					m_bAuto[PLAY_COUNT];					//托管设置
	//比赛场使用变量
	int						m_iWinPoint[PLAY_COUNT];				//胜者
	int						m_iDealPeople;							//反牌玩家
	bool					m_bHaveSendBack;						//该轮是否发过底牌
	bool					m_bCanleave[PLAY_COUNT];				//能否离开

	BYTE					m_iPrepareNT;							//预备庄家
	BYTE                    m_iFirstRobNt ;                        //第一个叫地主的人
	BYTE                    m_iFirstShow ;                         ///第一个明牌的人
	BYTE                    m_iCurrentRobPeople ;                  ///当前叫地主的人

	BYTE                    m_iRobStation[PLAY_COUNT] ;            ///玩家抢地主状态(0-未操作 255-不叫地主 其他-叫地主)
	BYTE                    m_iAddStation[PLAY_COUNT] ;            ///玩家加倍状态                     

	//斷線玩家
	BYTE					m_iFirstCutPeople;						//第一個掉線玩家
	int                     m_icountleave;                           //点离开人数统计

	///玩家准备状态
	bool                    m_bUserReady[PLAY_COUNT] ; 


	vector <long>			m_vlSuperID;	//保存超端玩家的容器

	bool					m_bUserNetCut[PLAY_COUNT];	//标识玩家是否断线

	int						m_iDeskID;	//桌子ID(用于写日志)



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
    ///拦截窗口消息
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用来改变用户断线条件的函数
	virtual bool CanNetCut(BYTE bDeskStation);//JAdd-20090116
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
	//处理函数
public:
	//用户设置游戏
	BOOL UserSetGame(UserArgeeGame * pGameSet);
	//服务端自动开始不由服务端控制
	BOOL StartGame();
	//游戏准备工作
	BOOL GamePrepare();
	//发牌过程中
	BOOL SendCardMsg(BYTE bDeskStation,BYTE bCard);
	//初始化一系列工作(重新开始游戏才进行此种初始化)
	BOOL InitThisGame();
	//读取静态变量
	static BOOL LoadIni();
	//加载限注
	BOOL LoadExtIni();
	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);
	//发送扑克给用户
	BOOL SendCard();
	//一次将所的牌全部发送
	BOOL	SendAllCard();
	//發送結束
	BOOL	SendCardFinish();
	//发送叫分
	BOOL SendCallScore(BYTE bDeskStation);
	//用户上交叫分
	BOOL UserCallScore(BYTE bDeskStation, int iVal);
	//叫分结束
	BOOL CallScoreFinish();
	//发送抢地主消息
	BOOL	SendRobNT(BYTE bDeskStation);
	//玩家抢地主消息
	BOOL	UserRobNT(BYTE bDeskStation, int iVal);
	//抢地主结束
	BOOL	RobNTFinish();
	//发送底牌
	BOOL	SendBackCard();
	//发送加棒消息
	BOOL	SendAddDouble();
	//玩家加棒
	BOOL	UserAddDouble(BYTE bDeskStation, int iVal);
	//加棒结果
	BOOL	AddDoubleResult();
	//加棒结束
	BOOL	AddDoubleFinish();
	//亮牌
	BOOL	SendShowCard();
	//明牌
	BOOL	UserShowCard(BYTE bDeskStation,int iValue);
	//明牌结束
	BOOL	ShowCardFinish();
	//游戏开始
	BOOL	BeginPlay();
	//用户出牌
	BOOL	UserOutCard(BYTE bDeskStation, BYTE iOutCard[],  int iCardCount);
	//新一轮开始
	BOOL NewPlayTurn(BYTE bDeskStation);
	//用户请求离开
	BOOL UserHaveThing(BYTE bDeskStation, char * szMessage);
	//同意用户离开
	BOOL ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee);
	//用户托管
	bool UserSetAuto(BYTE bDeskStation,bool bAutoCard);
	//填充日志表
	//	bool FindAllInfo(BYTE iCardList[],int iCardCount,GamePlayInfo  &iWriteInfo);
	//是否為新一輪
	BOOL IsNewTurn();
	//殺所有計時器
	void KillAllTimer();
	//加分判断
	BOOL IsAwardPoin(BYTE iOutCard[],int iCardCount,BYTE bDeskStation);
	//统计倍数(人头)
	int GetHumanBasePoint();
	//桌面倍数
	int GetDeskBasePoint();
	//房间倍数
	int GetRoomMul();
	//逃跑扣分
	int GetRunPublish();
	//逃跑扣分
	int GetRunPublish(BYTE bDeskStation);
	//非强退玩家得分
	int GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation);
	//计算各家得分
	int GetTruePoint(BYTE bDeskStation);
	//设置下一个庄家
	BOOL SetNextBanker(BYTE bGameFinishState = 0);
	//获取下一个庄家
	BYTE GetNextBanker();
	//机器人托管
	BOOL UseAI(BYTE bDeskStation );
	//玩家超时出牌(全部由服务端接管)
	BOOL UserAutoOutCard(BYTE bDeskStation);
	//发送取消机器人托管
	virtual BOOL SetAIMachine(BYTE bDeskStation,BOOL bState = FALSE);
	//玩家断线处理
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	//获取下一个玩家位置
	BYTE GetNextDeskStation(BYTE bDeskStation);
	//获取一个抢地主位置
	BYTE GetRobNtDeskStation(BYTE bDeskStation);
	//是否为未出过牌
	BYTE GetNoOutCard();

	//检查是否超端玩家
	bool	IsSuperUser(BYTE byDeskStation);
	//验证是否超端
	void	SpuerExamine(BYTE byDeskStation);
////道俱
//bool UserProp(BYTE bsendDeskStation,BYTE brecvDeskStation,int iValue);
};

/******************************************************************************************************/
