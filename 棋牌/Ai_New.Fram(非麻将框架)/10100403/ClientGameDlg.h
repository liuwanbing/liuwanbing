#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"
#include"UpGradeLogic.h"


//定时器 ID
#define ID_BEGIN_TIME		100										//开始定时器
#define ID_SCORE_TIME       101 									//叫分定时器


//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	/////////////////////////
public:

	CUpGradeGameLogic					m_Logic;										//客户端逻辑
	//固定信息
	BYTE								m_iVersion;
	BYTE								m_iVersion2;
	BYTE                                m_iMyDeskStation;                               //自己位置
	BYTE								m_iThinkTime;									//游戏思考时间
	BYTE								m_iBeginTime;									//游戏开始时间
	//运行信息
	int								    m_bUserPrepare[PLAY_COUNT];								//玩家准备
	BYTE								m_byGameStation;                                //游戏运行信息
	BYTE					            m_bNtPeople;									//庄家
	BYTE								m_DeskCard[PLAY_COUNT][CARD_NUMS];			    //每人桌面的真实扑克
	BYTE								m_bOwnCard[CARD_NUMS];							//自己的扑克
	BYTE								m_iDeskCardCount[PLAY_COUNT];					//每人桌面扑克的数目
	BYTE								m_bLeftCard[CARD_NUMS];							//上家出的牌
	BYTE								m_bCurrentCall;									//当前出牌玩家
	bool								m_bIsOutCard;									//时间到后是否自己一定要出牌
	
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
	//删除制定的牌
	void RemoveCard(BYTE bCardList[],BYTE bTarCard,int iCount);
	//重排牌数组
	void RestartSort(BYTE bCardList[],int iCount);
public:
	//处理同意游戏消息
	void OnAgreeGame(void *buffer);
	//处理发牌
	void OnHandleSendCardBegin(void *buffer);
	//处理游戏开始
	void OnHandleGameBegin(void *buffer);
	//处理服务器发过来的叫牌消息
	void OnHandleCall(void *buffer);
	//显示得分牌
	void OnHandleShowStyle(void *buffer);
	//处理服务器发过来的游戏正常结束消息
	void OnHandleNormalFinish(void *buffer);
	//处理服务器发过来的游戏托管消息
	void OnHandleAuto(void *buffer);

	//机器人判断是否出牌
	void RobotOutCard();
};
