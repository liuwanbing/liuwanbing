#pragma once
#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"
#include "UpGradeLogic.h"

/*--------------------------------------------------------------------*/
//定时器ID
#define TIMER_SELECTGAMEMODE		101		//模式选择
#define TIMER_NOTE					102		//下注计时器
#define TIMER_GETREADY				103		//准备计时器
#define TIMER_BETTING				104		//下注定时器
/*--------------------------------------------------------------------*/


//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
public:
	CUpGradeGameLogic		m_Logic;					//升级逻辑
	//函数定义
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();

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
	//同意开始游戏 
	virtual bool OnControlHitBegin();
	virtual bool SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond=false);
	//消息函数
public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation);
	//杀定时器
	void KillAllTimer();

private:
	//选择游戏模式定时器
	void OnTimerSelectGameMode(int iTimerID);

	//准备定时器
	void OnTimerGetReady(int iTimerID);

	//下注定时器
	void OnTimerBetting(int iTimerID);

	void OnMessageBetting(void *pData,int iLen);

	DECLARE_MESSAGE_MAP()
};
