#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"
#include "UpGradeLogic.h"



//定时器 ID
#define ID_BEGIN_TIME		100							//开始定时器

#define TIMER_GETREADY		101							//准备
#define TIMER_CALLLEVELCARD 102							//叫主
#define TIMER_BURYBACKCARD  103							//埋牌
#define TIMER_OUTCARD		104							//出牌

//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	/////////////////////////
public:
	CUpGradeGameLogic   m_cLogic ;             ///客户端逻辑
private:
	BYTE				m_byCards[33];				//牌数据
	vector<BYTE>		m_vbyCards;					//牌数据
	BYTE				m_byBackCard[8];			//底牌
	int					m_CardCounts;				//25张要显示的牌
	
	int					m_timeToGetReady;			//准备时间
	int					m_timeToCallLevelCard;		//叫主时间

	BYTE				m_byGameLevel[2];		//级牌
	BYTE				m_byGameLevelColor;		//级牌花色
	vector<BYTE>		m_vbyBigJor;			//大王
	vector<BYTE>		m_vbySmallJor;			//小王
	vector<BYTE>		m_vbyLevelCard;			//级牌

	T_CS_OutCard		m_tFirstPlayerCard;		//一圈中第一个出牌的玩家出的牌
	T_CS_OutCard		m_tCurrentOutCard;		//当前出的牌
	bool				m_isFirstOutCard;		//是否第一个出牌
	
	//函数定义
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();
    int GetGameStatus();

protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//重新设置界面
	virtual void ResetGameFrame();
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);
	//清除所有定时器
	void KillAllTimer();
	//同意开始游戏 
	virtual bool OnControlHitBegin();
	//安全结束游戏
	virtual bool OnControlSafeFinish(){return false;};
	//能否离开桌子
	virtual BOOL CanLeftDesk();
	//桌号换算到视图位置
	virtual BYTE ViewStation(BYTE bDeskStation);
	//消息处理函数
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//退出函数
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT nIDEvent);

public:
	//函数声明
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

	//重置游戏
	void ResetGameStation(int iGameStation);
	
	void ResetGameStatus();

public:

// 	游戏开始
// 		void OnGameBegin(GameBeginStruct * pGameBegin);
	//游戏状态处理
	void GameStationProc(TGameStation& tGameStation);

public:
	//随机下注
	void OnSuiJiXiaZhu();	
		
	//生成一个随机数
	int GenerateRand(int iBase);
	//接受牌数据，并显示
	void ReceiveAndShowCards(TSendCards* ptSendCards);
	//时间到，自动出牌
	void OutTimeAutoOutCard();
	//跟庄出牌
	void FindCardsCompatibleWithFirst(T_CS_OutCard &tFirstOutCard,BYTE *pOutCards,int* piCount);
	//显示现在的等级
	void ShowCurrentGameLevel(TGameLevel *pTGameLevel);
	//发送准备,或退出游戏消息消息	 , isReadyOrQuit - false： 退出游戏； true:准备好
	void ReadyOrQuit(bool bReadyOrQuit);
};

