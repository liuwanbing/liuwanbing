#pragma once
#include "../../../../sdk/include/ServerCommon/GameDesk.h"
#include "../GameMessage/UpGradeLogic.h"
#include "GameDataObject.h"


//定时器 ID
#define IDT_USER_CUT				1L				//断线定时器 ID

//*****************************************************************************************
//CServerGameDesk为提高性能采用特殊的计时方式MyTimer，相关的ID和配置如下：

#define IDT_MYTIMER					0x21			//公用定时器，只有这个定时器是实际定时器，其余都是从这个定时器经过判断来的

#define MY_ELAPSE					500				//定时器间隔
#define TIMER_COUNT					25				//用到的定时器数目
#define TIMER_ALLTIMERFINSH         0x24            //总的定时器

#define IDMYT_BASE					30				//MyTimer基ID，不用作计时
#define IDMYT_EVENT_PROTECT			31				//事件流保护定时器

#define IDEV_SYS_CFG_DESK			35				//系统设定桌面配置定时器/事件
#define IDEV_USER_CFG_DESK			36				//玩家设定桌面配置定时器/事件
#define IDEV_RAND_CARD				37				//洗牌定时器/事件

#define IDEV_SENDACARD				38				//下发牌定时器/事件
#define IDEV_SEND_3_CARD			39				//发   3 张牌定时器/事件
#define IDEV_SEND_4_5_CARD			40				//发第 4, 5 张牌定时器/事件
#define IDEV_BURN_CARD				42				//烧牌定时器/事件

#define IDEV_BET_START				43				//首次下注定时器/事件
#define IDEV_BET_BLINDS_AUTO		44				//大小盲注同时下注定时器/事件(系统强制)
#define IDEV_BET_NEXT				45				//下位玩家下注定时器/事件

#define IDEV_BIGBLIND               46              //下大肓注定时器
#define IDEV_SMALL_BLIND	        47              //下小肓注定时器

#define IDEV_ROUND_FINISH			48				//一回合完成/事件
#define IDEV_RESULT					49				//游戏结算定时器/事件
#define IDEV_NEXT_ROUND_BEGIN		50				//下一回合开始的定时器/事件		
#define IDEV_COMPARE_CARD           51              //比牌
#define IDEV_RANDOM					52				//随机一个庄与大小/事件

#define IDEV_AUTO_OPERATE           53              //自动操作定时器, 如果客户端玩家未操作，则服务器代理操作

#define ONE_SECOND                  1000            // 1 秒时间

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束

//游戏桌类

//游戏桌类
class CServerGameDesk : public CGameDesk
{

public:
	//构造函数
	CServerGameDesk(); 

	//析构函数
	virtual ~CServerGameDesk();

/*-----------------------------------------------------------------------------------------------------------
    一系列平台提供的需要重载来驱动游戏服务的函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
  ----------------------------------------------------------------------------------------------------------*/
public:

	//初始游戏
	virtual bool InitDeskGameStation();

	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);

	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation,BYTE bCloseFlag);

	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);

	//根据游戏判断是否在游戏中
	virtual BOOL IsPlayingByGameStation();

	//框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//玩家坐下
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);

	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation,CGameUserInfo* pUserInfo);

	//用来截取玩家断线回来信息
	virtual bool UserReCome(BYTE bDeskStation,CGameUserInfo* pNewUserInfo);

	//用来截取玩家断线信息
	virtual bool UserNetCut(BYTE bDeskStation,CGameUserInfo* pLostUserInfo);

	//用来改变用户断线条件的函数
	virtual bool CanNetCut(BYTE bDeskStation);

  /*-重载纯虚函数-*/

	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);

	//定时器消息
	virtual bool OnTimer(UINT uTimerID);


/*-----------------------------------------------------------------------------------------------------------
        声明一系列桌子特有机制函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
    函括配置机制、计时机制、动作流机制、游戏状态的管理
  ----------------------------------------------------------------------------------------------------------*/
public:

	//加载公共桌子配置
	BOOL LoadDeskCfg();

	//根据房间ID加载桌子配置
	BOOL LoadDeskCfg(int iRoomID);

	//配制牌型
	BOOL LoadChangCard(void);

	//初始分组表
	void InitGroupTable(void);

	//添加定时器标志
	int SetMyTimer(UINT nTimeID,int uElapse);

	//清除定时器标志
	void KillMyTimer(UINT nTimeID);

	//定时器某标志的响应函数
	bool OnMyTimer(UINT uTimerID);

	//动作流控制
	bool ActionRedirect(UINT uiActID,int iDelay);

	//开打动作流保护程序
	bool OpenEventProtection(UINT uiActID,int iActLife);

	//事件进行时处理
	bool EventGoing();

	//释放事件资源
	bool CloseEventProtection();

	//获得游戏的基本的状态
	void GetGSBase(TGSBase* pGSBase,BYTE byDeskstation);

	//获得游戏的基本的状态
	void CopyGSBase(TGSBase* pTarget,const TGSBase* pSource);


/*-----------------------------------------------------------------------------------------------------------
    声明游戏事件响应函数，功能函数，不同游戏差异较大
  ----------------------------------------------------------------------------------------------------------*/
public:

  /*-主动事件响应，桌子在游戏中自发驱动---*/

	//系统设定桌面的事件处理（主要设定时间、游戏规则等）
	bool OnEventSysCfgDesk();

	//玩家设定桌面的事件处理（主要设定底注）
	bool OnEventUserCfgDesk();

	//洗牌事件
	bool OnEventRandCard();

	//随机一个庄
	bool OnEventRandom();

	//为每个玩家发牌的事件
	bool OnEventSendACard();

	//为发3张公牌的事件
	bool OnEventSend3Card();

	//为发1张公共底牌的事件
	bool OnEventSend1Card();

	//首次下注事件
	bool OnEventBetStart();

	//大小盲注强制同时下注事件
	bool OnEventBetBlindsAuto();

	//下一位玩家下注事件
	bool OnEventBetNext();

	//开始下一回合游戏事件处理
	bool OnEventNextRoundBegin();

    //自动操作游戏事件处理
    bool OnEventAutoOperate();

    // 比牌
    bool OnCompareCard();
  /*-被动事件响应，通过客户端驱动---*/

	//处理玩家同意游戏
	bool OnUserAgreeGame(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//处理玩家设置底注
	bool OnUserCfgDesk(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//处理玩家设置底注超时
	bool OnUserCfgDeskTimeout(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//处理玩家同意底注
	bool OnUserAgreeDeskCfg(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

    //玩家代入金币设置
    bool OnUserSubMoney(BYTE bDeskStation, void* pData);

	//玩家下注信息
	bool OnUserBet(BYTE bDeskStation, void* pData , bool bUserforceExit = false);

	//处理玩家发牌完成
	bool OnUserSendCardFinish(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//处理玩家对下一回合准备的信号
	bool OnUserNextRoundReq(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//处理玩家本身金币不足于最少带入金额
	bool OnUserMoneyLack(BYTE bDeskStation);

	////处理玩家线束本轮游戏的请求
	bool OnTestFinishRoundRequest(BYTE bDeskStation,
		NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);


  /*-游戏事件保护，当某事件在生命期内没有收到任何手动销毁的消息，会自然毁灭，在毁灭前应该进行代管处理，以确保游戏流程不受影响--------*/

	//代管玩家发牌完成
	bool OnAgentSendCardsFinish();

	//出牌事件流代理
	bool OnAgentOutCard();

	//下一局开始事件流代理
	bool OnAgentNextRoundBegin();


  /*-游戏状态恢复响应，用于客户端在任何情况下启动游戏，得到最新最正确的数据-----*/
	//处理要得到游戏等待开始的请求
	bool OnGetGSWaitAgree(BYTE bDeskStation,
		UINT uSocketID,bool bWatchUser,TGSBase* pGSBase);

	//处理要得到游戏中状态的请求
	bool OnGetGSPlaying(BYTE bDeskStation,
		UINT uSocketID,bool bWatchUser,TGSBase* pGSBase);

	//处理要得到游戏回合结束的请求
	bool OnGetGSWaitNextRound(BYTE bDeskStation,
		UINT uSocketID,bool bWatchUser,TGSBase* pGSBase);


  /*-功能函数--*/

	//逆时针顺序得到下家位置
	BYTE GetNextUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//逆时针顺序得到上家位置
	BYTE GetLastUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//获取游戏玩家数量
	int GetPlayerCount(bool bFold = false);

    // 计算每轮胜利玩家
    void CalculateWin(BYTE nSectionCount, CByteArray &arrayResult);

	// 判断玩家是否属于配好牌玩家
	bool CanGetGoodCard(BYTE byStation);
/*-----------------------------------------------------------------------------------------------------------
    定义桌面的数据，以下这些数据适用于大多数据游戏开发时使用
  ----------------------------------------------------------------------------------------------------------*/
protected:

	//游戏数据
	CGameDataObject m_GameData;

	//游戏逻辑
	CUpGradeGameLogic m_Logic;

	//服务器开启太多的定时器，会影响性能，故此作出优化，只开一个定时器，\
	  所有使用到的定时器都由这个定时器来触发，在触发中加以判断

	struct TMyTimerNode
	{
		//定时器标志号
		UINT uiID;

		//已经经过了次
		int  nTickCount; 

		//经过多少次后触发事件
		int  nTriggerCount; 

		TMyTimerNode(UINT nTimerID, int uElapse):\
			uiID(nTimerID),nTickCount(0),nTriggerCount(uElapse){}

		TMyTimerNode():\
			uiID(0),nTickCount(0), nTriggerCount(0){}
	};

	TMyTimerNode m_arTimer[TIMER_COUNT];

	//当前事件流保护状态的上下文
	struct TEvProtectDescr
	{
		//当前被保护的事件
		UINT uiEvID;

		//事件生命期
		int  iLiftTime;

		//已逝去的时间
		int  iPassTime;

		TEvProtectDescr():\
			uiEvID(0),iLiftTime(0),iPassTime(0){}

	};

	TEvProtectDescr m_EventProtecter;


/*-----------------------------------------------------------------------------------------------------------
    定义桌面的数据，以下这些数据适用于大多数据游戏开发时使用
  ----------------------------------------------------------------------------------------------------------*/
private:
	bool m_bGameFinished;
protected:
	int						m_iUserStation[PLAY_COUNT];				//记录用户状态
	
	bool					m_bFaiPaiFinish;						// 发牌完成
};
