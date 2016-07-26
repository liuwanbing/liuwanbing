#pragma once

#include "../../../../sdk/include/ServerCommon/GameDesk.h"
#include "../GameMessage/UpGradeLogic.h"

// 服务器开启太多的定时器，会影响性能，故此作出优化，只开一个定时器，所有使用到的定时器都由这个定时器来触发，在触发中加以判断

// 定时器所用结构
struct TimerStruct 
{
	UINT nId;			//定时器标志号
	int nTickCount;		//已经经过了次
	int nTriggerCount;	// 经过多少次后触发事件
	TimerStruct(UINT nTimerID, int uElapse) : nId(nTimerID),nTickCount(0), nTriggerCount(uElapse)
	{
	}
	TimerStruct(): nId(0),nTickCount(0), nTriggerCount(0){}
};


//定时器 ID
#define TIME_MY_TIMER				20				//公用定时器，只有这个定时器是实际定时器，其余都是从这个定时器经过判断来的
#define MY_ELAPSE					250				// 定时器间隔
#define TIMER_COUNT					11				// 用到的定时器数目

// 下列为游戏中使用到的定时器，由上面的TIME_MY_TIMER计数进行统一处理

#define TIME_SEND_CARD				30				//发牌定时器
#define TIMER_SEND_END              31				//发牌结束
#define TIMER_WAIT_NOTE             32				//等待下注 
#define TIME_SEND_ONE_CARD			33				//发一次牌
#define TIME_SEPARATE			    34				//分牌时间
#define TIME_GAME_FINISH			35				//游戏结束定时器
#define TIME_CUT_AI                 36              //托管定时器
#define	TIME_BOBO_RESET_TIME		37				//定时簸簸还余下的时间

#define DEFAULT_WAIT_FREQUENCE      1000            //一秒中检测一次

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束
#define GF_EARLY_END				12				//提前结束
#define  TIMER_CUT_AI_LONG                  1000


// 用来方便地加锁和解锁的类
class BZWLockForCriticalSection
{
	CRITICAL_SECTION *m_pSection;
	BZWLockForCriticalSection();
public:
	BZWLockForCriticalSection(CRITICAL_SECTION *pSection)
	{
		m_pSection = pSection;
		EnterCriticalSection(m_pSection);
	}
	~BZWLockForCriticalSection()
	{
		LeaveCriticalSection(m_pSection);
	}
};


//游戏桌类
class CServerGameDesk : public CGameDesk
{
private:
	int						   m_iTimerCount;      //记录玩家时间
	__int64					   m_i64BoBase;					//簸底
	__int64					   m_i64SmallBobo;				//最小簸簸
	//------------------------------与托管相关的功能------------------
	bool m_arybCutUser[PLAY_COUNT];     //玩家是否掉线
protected:
	enum 
	{
		FINISH_TYPE_UNFINISH = 0,     //没有结束		
		FINISH_TYPE_FISH = 1,         //其他结束      
		FINISH_TYPE_REACH_LIMITE_SELF = 2,  //达到上限，结束
		FINISH_TYPE_REACH_LIMITE_NEXT = 3   //下一家跟注就会超过限制
	};
	TimerStruct		m_arTimer[TIMER_COUNT];
	CRITICAL_SECTION m_csForIPCMessage;
	//运行状态变量
	BYTE					m_iNtPeople;                         //赢家
	int						m_iUpGradePeople;					//庄家位置
	BYTE					m_iHaveThingPeople;					//有事要走玩家
	BYTE					m_iAllCardCount;					//游戏所用的牌数
	BYTE					m_iSendCardTime;					//游戏思考时间
	BYTE					m_iSendCardRes;						//簸簸余下的时间
	BYTE					m_iThinkTime;						//游戏思考时间
	BYTE					m_iBeginTime;						//游戏开始时间
	BYTE					m_bySeparateCardTime;				//分牌时间
	//状态信息
	int						m_iUserCardShap[PLAY_COUNT][3];				//玩家的信型信息0:左边,1:右边 ，保存数据为 aabbcc;aa:牌型,bb:点,cc:牌中最大值，2:保存序号	
	BYTE					m_bySendCardCount[PLAY_COUNT];				///已经发牌的张数
	BYTE					m_iUserCardCount[PLAY_COUNT];				//用户手上扑克数目
	BYTE					m_iUserCard[PLAY_COUNT][MAX_CARD_COUNT];	//用户手上的扑克
	BYTE					m_byCardShap[PLAY_COUNT][2];				//牌型 0:保存左边牌型 1:保存右边牌型
	bool					m_bSeparatCardFale[PLAY_COUNT];				//玩家是否已经分牌了
	//运行信息
	int						m_iOutCardPeople;							//现在出牌用户
	int						m_iFirstOutPeople;							//先出牌的用户
	bool					m_bSetAllFlag;						//敲标志位
	bool					m_bFollowFlag;						//跟标志位
	//发牌数据
	BYTE					m_iTotalCard[32];					//总的牌
	BYTE					m_iSendCardPos;						//发牌位置
	CUpGradeGameLogic		m_Logic;     						//游戏逻辑
	int						m_iLastOutPeople;
	int						m_iUserStation[PLAY_COUNT];			//记录用户状态
	__int64					m_iPerJuTotalNote[PLAY_COUNT];      //用户下总注	
	__int64					m_i64WinMoney[PLAY_COUNT];			//所赢得的金币(总收入)
	__int64					m_i64NetIncome[PLAY_COUNT];			//净收入
	__int64					m_iTotalNote;					    //总注
	__int64                 m_iFrontNote;                       //前家押注
	__int64					m_i64SaveFront;						//保存前面下的注
	__int64					m_i64Bobo[PLAY_COUNT];				//每个玩家的簸簸数
	__int64					m_iNowNote[PLAY_COUNT];			    //当前押注数
	__int64					m_i64NoteMoney[PLAY_COUNT][3];		//玩家每次所下的金币值

	BYTE					m_byGameStation;					//游戏状态()
	BYTE					m_byNoteTime[PLAY_COUNT];			//下注的次数
	BYTE					m_byNoteFlag[PLAY_COUNT];			//第一轮下注
	BYTE					m_byOperationState[PLAY_COUNT];		//玩家的操作记录		
	int						m_iAlreadySendCard;					//已经发牌的张数
	BYTE					m_iSendPos;							//新一轮发牌开始位置
	///玩家准备状态
	bool                    m_bUserReady[PLAY_COUNT] ;				//用户同意标志位
	bool					m_bAgree[PLAY_COUNT];					//开始游戏标志
	bool					m_bAlreadSetBobo[PLAY_COUNT];			//完成簸簸的设置

	bool					m_bSpecialCardGiveUp[PLAY_COUNT];		//特珠牌点放弃(三花十或三花六)
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();
//重载函数
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//用户断线
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	virtual bool InitDeskGameStation();
//重载纯虚函数
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//处理函数
public:
	void InitAIData();
	virtual bool OnBeginUserNetCutAI(BYTE bDeskStation);  //重载触发托管函数
	void ExcutNetCutAI();  //执行托管操作
	//超端判断
	void SuperUserIDJudge();
	// 杀死定时器标志
	void KillMyTimer(UINT nTimeID);
	int SetMyTimer(UINT nTimeID, int uElapse);
	bool OnMyTimer(UINT uTimerID);
	//读取静态变量
	BOOL LoadIni();	//准备发牌工作
	//加载限注
	BOOL LoadExtIni();
	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);
	//用户设置游戏
	BOOL UserSetGame(UserArgeeGame * pGameSet);
	//发送扑克给用户
	BOOL SendCard();
	//发牌结束
	BOOL SendCardFinish();
	//游戏开始
	BOOL BeginPlayUpGrade();
	//处理用户结果
	BOOL UserActiveResult(BYTE bDeskStation, BYTE iVerbType,__int64 iNoteVal=0,BYTE bComparedStation=255);
	//发牌位置
	BOOL CanSendCard(BYTE bDeskStation);
	//统计游戏人数
	BYTE CountPlayer();
	//換牌
	BOOL ChangeBackCard(BYTE bDeskStation,BYTE bSourceCard,BYTE bResultCard);
	//隊例中是否有某牌是否替換成功
	BOOL ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard);
	//逃跑扣分
	int GetRunPublish();
	//得到下一个出牌玩家的位置
	BYTE GetNextNoteStation(BYTE bCurDeskStation);
	//----控制输赢--------
	BOOL ChangeCard(BYTE bDestStation,BYTE bWinStation);						//换牌	
	bool SetUserCard();			// 读取配置文件，用于调试发牌	Add by zxd 20090803
	//平台奖池功能具体实现需要游戏传递
	//修改奖池数据1(平台->游戏)
	//@param bAIWinAndLostAutoCtrl 机器人控制输赢开关
	//@return 是否成功
	virtual bool SetRoomPond(bool	bAIWinAndLostAutoCtrl);
	//修改奖池数据2(平台->游戏)
	//@param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
	//@param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
	//@return 是否成功
	virtual bool SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[]);
	//修改奖池数据3(平台->游戏)
	//@param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
	//@return 是否成功
	virtual bool SetRoomPondEx(__int64	iReSetAIHaveWinMoney);
	//控制胜负
	//判断
	virtual BOOL Judge();
	//胜
	virtual BOOL JudgeWiner();
	//输
	virtual BOOL JudgeLoser();

	bool Ass_Set_Bobo_Data(void * pData, UINT uSize);						//处理接收到的簸簸数
	void SendOneCardFinsh();												//发一张牌完成
	void SeparateCardFinsh();												//分牌时间到(超时了)
	bool Ass_Cs_Separat_Card(void * pData, UINT uSize);
	void FinshCountScore();													//结束算分
	int	 CmpTwoValue(int iFirstList[],int iSecondList[]);					//两组牌型的大小
	bool CheckGameFinish();													//检测游戏是否结束了

	void SpecialCardGive(int iDesk);										//已经发四张牌之后,出现三花十或三花六之后点放弃
};

/******************************************************************************************************/
