
// ***************************************************************
//  GameEngine   version:  1.0   ·  date: 02/11/2011
//  -------------------------------------------------------------

//	德克萨斯扑克，主状态机，游戏客户端状态机\
	NameID：51300280

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "StdAfx.h"
#include "UserDataCliObject.h"
#include "../GameMessage/UpGradeLogic.h"
#include "GameLanchSdk\bzwUiInclude\bbaseparameter.h"
#include "SoundsDef.h"
#include "CardSender.h"
#include<stack>
using namespace std;


#define IDT_TIMEOUT_NO_USE			 100    //只用于摆设的定时器
#define IDT_TIMEOUT_CFG_DESK		 101	//设置底注的定时器ID
#define IDT_TIMEOUT_WAIT_AGREE		 102	//等待其它同意底注定时器ID1
#define IDT_TIMEOUT_VERB			 103    //等待下注定时器ID
#define IDT_TIMEOUT_NO_PLAYER		 104	//没有玩家等待游戏的退出
#define IDT_TIMEOUT_WAIT_CONFIG_NOTE 105	//等待设置底注超时
#define IDT_TIMEOUT_NOT_ENOUGH_MONEY 107	//玩家钱不足提示超时
#define IDT_TIMEOUT_SHOW_BASE_CARD	 109	//等待显示底牌
#define IDT_TIMEOUT_NEXT_ROUND_BEGIN 110	//等待下一回合开始

#define IDT_COLLECT_JETONS			 120	//收集筹码动画的时间
#define IDT_WAIT_DISPATCH_JETONS	 121	//等待颁发筹码
#define IDT_DISPATCH_JETONS			 122	//颁发筹码动画的时间

#define IDT_TIME_SOUND               123    //定时播放定时器声音

#define IDSM_CARD_SENDER			 1		//发牌子状态机ID (IDSM = ID of Sub Machine)

/************************************************************************************
*									德克萨斯扑克									*
*************************************************************************************/
#define IDT_TIMEOUT_USER_STAKE		120		// 注额定时器状态


/************************************************************************************
*																					*
*************************************************************************************/

class CMiniGameEngine;

class CGameMachine
{

public:

	//构造析构器
	CGameMachine();

	virtual ~CGameMachine();

public:

	//根据游戏当前的状态，启动游戏的状态机接口
	virtual bool StartUp(CMiniGameEngine* p,BYTE byGameStation,void* pGSData);

	//关闭
	virtual bool ShutDown();

	//游戏音效配置加载
	virtual bool LoadSoundCfg();

	//根据ID获取子状态机
	virtual void* GetSubMachine(UINT uiMacID);

/*--------------------------------------------------------------------
	所有状态机状态变换接口
  ------------------------------------------------------------------*/
public:

	//开始游戏的接口
	virtual bool OnHandleGameBegin(void* pData,UINT uiSize);

	//为玩家提供选择注额的接口
	virtual bool OnHandleCfgDesk(void* pData,UINT uiSize);

	//系统提供选择注额后，来自服务器的注额信息的处理收口
	virtual bool OnHandleGetSysDeskCfg(void* pData,UINT uiSize);

	//玩家提供选择注额后，来自服务器的注额信息的处理收口
	virtual bool OnHandleGetDeskCfg(void* pData,UINT uiSize);

	//玩家提供代入金额后处理收口
	virtual bool OnHandleInMoney(void* pData,UINT uiSize);

	//玩家选择底注写成的处理接口
	virtual bool OnHandleCfgDeskFinish(BYTE byIndex);

	//等待庄家选择注额的超时处理
	virtual bool OnHandleWaitCfgDeskTimeout();

/***************************************************************************************************/
	// 开始按钮
	virtual bool OnHandleBeginGame(BBaseParameter* pParam);

	// 修改带入金额按钮处理接口
	virtual bool OnHandleModifyMoney();
	
	// 玩家下注处按钮理接口
	virtual bool OnHandleBet();

	// 玩家跟注按钮处理接口
	virtual bool OnHandleCall();

	// 玩家加注按钮处理接口
	virtual bool OnHandleRaise();
	
	// 玩家过牌按钮处理接口
	virtual bool OnHandleCheck();

	// 玩家弃牌按钮处理接口
	virtual bool OnHandleFolo();

	// 玩家全下按钮处理接口
	virtual bool OnHandleAllin();
	
	// 玩家金额按钮处理接口
	virtual bool OnHandleButtonMoney(int nID);

	// 玩家滑动加注轴处理接口
	virtual bool OnHandleSlidingAxle();

	// 玩家加注钮处理接口
	virtual bool OnHandleAddMinus();

	// 玩家减注钮处理接口
	virtual bool OnHandleSubMinus();

	// 玩家跟注托管处理接口
	virtual bool OnHandleCallMandate(bool bFlag);

	// 玩家过牌托管处理接口
	virtual bool OnHandleCheckMandate(bool bFlag);

	// 玩家跟任何注托管处理接口
	virtual bool OnHandleCallAnyMandate(bool bFlag);

	// 玩家过牌或弃牌托管处理接口
	virtual bool OnHandlePassAbandonMandate(bool bFlag);

	// 初始化数据
	virtual bool Init();
/***************************************************************************************************/

	//服务器下发牌处理接口
	virtual bool OnHandleSendACards(void* pData,UINT uiSize);

	//服务器下发3张公共底牌处理接口
	virtual bool OnHandleSend3Cards(void* pData,UINT uiSize);

	//服务器下发1张底牌处理接口
	virtual bool OnHandleSend1Cards(void* pData,UINT uiSize);

	//服务器下发牌处理接口
	virtual bool OnHandleBetPoolUp(void* pData,UINT uiSize);

	//服务器下注更新处理接口
	virtual bool OnHandleBetInfo(void* pData,UINT uiSize);

	//服务器报告所有玩家发牌完成的消息处理接口
	virtual bool OnHandleSendCardFinish();

	//服务器下发令牌处理接口
	virtual bool OnHandleGetToken(void* pData,UINT uiSize);

	//收到玩家下注的消息接口
	virtual bool OnHandleGetBetInfo(void* pData,UINT uiSize);

	//玩家升起手牌处理接口
	virtual bool OnHandleHitHandCards(BYTE byCard[],int iUpCount);

	//玩家把牌全放下处理接口
	virtual bool OnHandleUnUpCard();

	//玩家提示处理接口
	virtual bool OnHandleCue();

	//比牌处理接口
	virtual bool OnHandleCompareCard(void* pData,UINT uiSize);

	//结算处理接口
	virtual bool OnHandleRoundResult(void* pData,UINT uiSize);

	//结算颁发筹码接口
	virtual bool OnHandleDispatchJetons();

	//下一回合准备动作处理接口
	virtual bool OnHandleNextRoundReq(BBaseParameter* pParam);

	//新回合开始处理接口
	virtual bool OnHandleNewRoundBegin(void* pData,UINT uiSize);

	//没有玩家同意底注，即没有玩家的时候处理接口
	virtual bool OnHandleNoPlayer(void* pData,UINT uiSize);

	//玩家是否同意游戏的处理接口
	virtual bool OnHandleAgree(void* pData,UINT uiSize);

	//玩家手上的钱不足够时的处理接口
	virtual bool OnHandleNotEnoughMoney(void* pData,UINT uiSize);

	//系统自动清理片处理接口
	virtual bool OnUserSortOut();
	
	//玩家设置音效处理接口
	virtual bool OnHandleCfgSound(BBaseParameter* pParam);

	/*--关于定时的处理---------------------------------------------*/

    /*--关于超时的处理---------------------------------------------*/

	//选择注额的超时接口
	virtual bool OnTimeoutCfgDesk(BYTE bDeskStation);

	//选择注额的超时接口
	virtual bool OnTimeoutWaitCfgNote();

	//等待底注同意的超时接口
	virtual bool OnTimeoutAgreeNote(BYTE bDeskStation);

	//等待下一回合开始的时间用光处理接口
	virtual bool OnTimeoutNextRoundBegin(BYTE bDeskStation);

	//玩家没有钱超时处理接口
	virtual bool OnTimeoutNotEnoughMoney(BYTE bDeskStation);

	/*关于状态重现的处理------------------------------------------*/

	//游戏桌基本状态重现
	virtual bool OnRecurBase(BYTE byGameStation,TGSBase* pGSBase);

	//游戏等待同意状态重现
	virtual bool OnRecurWaitAgree(void* pData);

	//游戏进行中的状态重现
	virtual bool OnRecurPlayingGame(void* pData);

	//等待下一轮游戏的状态重现
	virtual bool OnRecurWaitNextRound(void* pData);


	//逆时针顺序得到下家位置
	BYTE GetNextUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//逆时针顺序得到上家位置
	BYTE GetLastUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//清空记录上一个操作玩家栈列表
	void ClearAllStack();
/*--------------------------------------------------------------------
	状态机特有数据
  ------------------------------------------------------------------*/
public:

	//游戏引擎对象指针，其担任两个角色，第一是UI管理，第二是通讯管理\
	  所以下面代码中有时候称呼其为“视类”，有时候称呼其为“通讯类”
	CMiniGameEngine* m_pGameEngine;

	//自己的逻辑位置
	BYTE m_byMeStation;

	//游戏逻辑
	CUpGradeGameLogic m_Logic;

	//地方语言类别
	DZPKSound::LanguageType m_ltLangType;

	//游戏背景音乐声效开关
	bool m_bBgMusic;

	//游戏音效开关
	bool m_bGameSound;

	//是否处理托管状态
	bool m_bTuoGuan;

	//是否已通知服务器玩家准备游戏
	bool m_bAgreeGame;

	//得到令牌的玩家
	BYTE m_byTokenUser;

	//托管标志 0: 自动跟注(只限一轮)  1: 自动跟任何注(每轮都跟注)  2: 自动过牌  3: 自动弃牌(上家有人过牌就过牌，没人过牌就弃牌)
	bool m_bTrusteeship[4];

	//记录每个玩家操作类型
	emType m_emLastOperation[PLAY_COUNT];

	//记录上一个操作玩家
	stack<BYTE> m_stackLastStation;

/*--------------------------------------------------------------------
	子状态机
  ------------------------------------------------------------------*/
private:

	//发牌机指针，发牌机需要动态创建，发牌后请释放，并赋空值
	CCardSender* m_pCardSender;

/*--------------------------------------------------------------------
	游戏数据
  ------------------------------------------------------------------*/
public:

	//注额信息
	TDeskCfg m_tagDeskCfg;

	// 代入金额信息
	TSubMoney m_tagSubMoney;

	// 结算信息
	TResult m_tagResult;

	//每位玩家手的数据
	CUserDataCliObject m_UserData[PLAY_COUNT];

	//庄家位置
	BYTE m_byNTUser;

	//小盲注位置
	BYTE m_bySmallBlind;

	//大盲注位置
	BYTE m_byBigBlind;

	//底牌数据
	BYTE m_byBackCard[MAX_BACKCARDS_NUM];

	//底牌数量
	int m_iBackCardNums;

	//当前下注金币
	int m_nCurrentMoney; 

	// 跟注金币
	int m_nCallMoney;

	//记录下注玩家
	int m_iBetCounts;

	//游戏中的所有动作时间
	int m_iVerbTime;

	//每个用户可用金币
	int m_nMoney[PLAY_COUNT];

	//每个玩家下注金币
	int m_nBetMoney[PLAY_COUNT];

	// 弃牌玩家
	bool m_bGiveUp[PLAY_COUNT];

	// 全下玩家
	bool m_bAllBet[PLAY_COUNT];
	
	// 下注边池
	TBetPool m_tagBetPool;
};