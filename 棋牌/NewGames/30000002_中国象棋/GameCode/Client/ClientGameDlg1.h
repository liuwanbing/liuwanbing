#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "ClientView.h"
#include "UpGradeLogic.h"

//消息定义
#define IDM_BEGIN			WM_USER+120					//开始按钮消息
#define IDM_USER_NT			WM_USER+121					//用户反牌
#define IDM_GIVE_BACK		WM_USER+123					//留底牌按钮
#define IDM_OUT_CARD		WM_USER+124					//用户出牌
#define IDM_LOOK_LAST		WM_USER+125					//看上轮扑克
#define IDM_BACK_CARD		WM_USER+126					//底牌信息
#define IDM_HAVE_THING		WM_USER+127					//有事要走
#define IDM_SEND_THING		WM_USER+128					//发送离开请求
#define IDM_ARGEE_LEAVE		WM_USER+129					//同意离开消息

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_WAIT_ROB					101					//等待抢底牌
#define ID_GIVE_BACK				102					//等待留底牌
#define ID_OUT_CARD					103					//出牌等待时间
#define ID_SHOW_LAST				104					//查看上轮扑克
#define ID_SHOW_CARD				105					//甩牌定时器
#define ID_LOOK_BACK				106					//查看底牌

//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

//游戏框架类 
class CClientGameDlg : public CGameFrameDlg
{
	//变量定义
public:
	//固定信息
	BYTE					m_iLessCount;				//最少游戏盘数
	BYTE					m_iBeginStation;			//开始位置
	BYTE					m_iEndStation;				//结束位置
	BYTE					m_iCardCount;				//允许游戏的扑克数目
	BYTE					m_iPalyCardCount;			//游戏扑克数目

	//自己扑克数据
	BYTE					m_iMeCardCount;				//自己扑克的数目
	BYTE					m_iMeCardList[45];			//自己扑克列表

	//主牌信息
	BYTE					m_iNtCount;					//亮主的扑克数目
	BYTE					m_iNtPeopole;				//亮主的游戏者
	BYTE					m_iBackCard[8];				//底牌扑克

	//运行信息
	BYTE					m_iDeskCardCount[4];		//每人桌面扑克的数目
	BYTE					m_DeskCard[4][39];			//每人桌面的扑克

	//运行信息
	BYTE					m_iNowOutPeople;			//现在出牌者
	BYTE					m_iBaseCount;				//第一个出牌者牌数目
	BYTE					m_iFirstOutPeople;			//第一个出牌者
	BYTE					m_iBaseCard[39];			//第一个出牌者的扑克

	//上轮显示数据
	BYTE					m_iLastCardCount;			//上轮扑克的数目
	BYTE					m_bShowLast;				//是否可以显示上轮扑克
	BYTE					m_bShowIngLast;				//是否正显示上轮扑克
	BYTE					m_iLastOutCard[4][39];		//上轮的扑克

	////定时器计数
	//int						m_iTimerStation;			//定时器位置
	//int						m_iTimerCount;				//计数器剩时间

	//控件变量
public:
	CUpGradeGameLogic		m_Logic;					//升级逻辑
	CClientPlayView			m_PlayView;					//游戏视图

	//函数定义
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();

protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	virtual void OnWatchSetChange(void);
	virtual void OnGameSetting();

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

	//消息函数
public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	//NT 按钮函数
	LRESULT	OnHitNt(WPARAM wparam, LPARAM lparam);
	//按动扑克按钮函数
	LRESULT	OnHitCard(WPARAM wparam, LPARAM lparam);
	//留底牌按钮函数
	LRESULT	OnHitBackCard(WPARAM wparam, LPARAM lparam);
	//按动出牌按钮函数
	LRESULT	OnHitOutCard(WPARAM wparam, LPARAM lparam);
	//按动上轮按钮函数
	LRESULT	OnHitLastTurn(WPARAM wparam, LPARAM lparam);
	//按动看底牌按钮函数
	LRESULT	OnHitLookBack(WPARAM wparam, LPARAM lparam);
	//按动有事按钮函数
	LRESULT	OnHitHaveThing(WPARAM wparam, LPARAM lparam);
	//发送离开请求函数
	LRESULT	OnSendHaveThing(WPARAM wparam, LPARAM lparam);
	//用户请求离开
	LRESULT	OnArgeeUserLeft(WPARAM wparam, LPARAM lparam);
	//右键用牌
	LRESULT	OnRightHitCard(WPARAM wparam, LPARAM lparam);

	BOOL UpdateNtList();
	void ResetGameStation(int iGameStation);

	DECLARE_MESSAGE_MAP()
};
