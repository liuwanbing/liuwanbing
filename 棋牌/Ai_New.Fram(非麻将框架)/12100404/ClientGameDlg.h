#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"

#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_WAIT_NEXT				101					//下一局定时器
#define ID_OUT_CARD					102					//叫牌定时器
#define ID_NOTE                     105                 //等待闲家下注




//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	//控件变量
public:

	int						m_iVersion;
	int						m_iVersion2;
public: 
	BYTE                m_byGameStation ;										//当前的游戏状态
	int                 m_iMyDeskStation ;										//玩家自己的游戏位子
	BYTE				m_bNtPeople;											//当局庄家

	BYTE				m_bScorePeople;											//当前叫分者
	BYTE				m_bGameBeginWaitTime;									//游戏开始等待时间
	BYTE				m_bGameBetTime;											//游戏下注时间
	BYTE				m_bCallCardTime;										//游戏叫牌时间
	BYTE				m_iUserAlreadySend[PLAY_COUNT][13];						//用户已经发的扑克
	__int64				mi64UserMaxBet;											//用户最大下注数
	__int64				m_i64UserCanBet[3];										//用户下注筹码的值
	__int64				m_i64CurrentNote;										//当前选择的下注
	__int64				m_i64UserBet[PLAY_COUNT];								//用户的下注

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
	virtual bool SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUseMillSecond = false) ; 
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

private:
	//加载配置文件
	void LoadGameIni();
private:


public:
	///初始化部分数据
	void	ResetGameData();

	//函数声明
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

	//重置游戏
	void ResetGameStation(int iGameStation);

public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);

public:
	
	//处理服务器同意游戏消息
	void OnHandleAgreeGame(void *buffer);

	//处理服务器发过来的确定庄家消息
	void OnHandleNt(void *buffer);

	//处理服务器发过来的下注消息
	void OnHandleBet(void *buffer);

	//处理服务器发过来的发牌消息
	void OnHandleSendCard(void *buffer);

	//处理服务器发过来的开始游戏消息
	void OnHandlePlay(void *buffer);

	//处理服务器发过来的游戏安全结束消息
	void OnHandleSafeFinish(void *buffer);

	//处理服务器发过来的下注消息
	void OnHandleBetResult(void *buffer);

	//处理服务器发过来的叫牌消息
	void OnHandleCall(void *buffer);

	//处理服务器发过来的游戏正常结束消息
	void OnHandleNormalFinish(void *buffer);

	//处理服务器发过来的超端用户信息
	void HandleSuperMessage(void *buffer);

};
