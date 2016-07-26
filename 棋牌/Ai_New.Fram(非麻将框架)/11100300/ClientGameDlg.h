#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"

#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define TIME_AGREE			101			//同意计时器
#define TIME_BET			102			//下注计时器
#define	TIME_APPLY			103			//申请上庄计时器
#define TIME_BALLOT			104			//藏签计时器
#define TIME_LEAVE			105			//申请下庄


//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{

	//控件变量
public:

	int						m_iVersion;
	int						m_iVersion2;
public: 

	BOOL				m_RobotCanBet;											//机器人能否下注
	BYTE                m_byGameStation ;										///当前的游戏状态
	int                 m_iMyDeskStation ;										///玩家自己的游戏位子

	BYTE				m_byFreeTime;											//空闲时间
	BYTE				m_byBetTime;											//下注时间
	BYTE				m_byNoBetTime;											//多少秒不能下注
	BYTE				m_byOpenTime;											//开奖时间
	BYTE				m_byShowResultTime;										//结算显示时间

	BYTE				m_byRemaindTime;										//剩余时间
	BYTE				m_iRobotMaxNtLimit;										//机器人最大上庄人数限制
	BYTE				m_byWaitNtList[PLAY_COUNT];								//上庄列表
	
	BYTE				m_byMaxNtPeople;										//最大上庄人数
	BYTE				m_byNtStation;											//庄家位置
	__int64             m_i64ApplyNtLimite ;									//机器人上庄条件
	__int64				m_iRobotBetSumLimit;									//机器人下注限制
	
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
	virtual bool OnControlSafeFinish(){OutputDebugString("Robot::OnControlSafeFinish");return TRUE;};
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
	//初始化游戏数据
	void InitGameData();

	//加载配置文件
	void LoadGameIni();

	//机器人申请上庄
	void	RobortApplyNt();
	//机器人下庄
	void	RobortApplyLeave();
	//机器人下注
	void	RobortBet();
private:
	//获取指定玩家的剩余金币
	__int64	GetUserRemaindMoney(BYTE byDestStation);
	//获取玩家剩余的金币
	__int64	GetMyRemaindMoney();
	//是否在上庄列表中
	bool	IsInWaitNtList();
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
	
    ///设置筹码列表
	void  SetChoumaValueList(void) ;

public:
	//处理下注结果
	void HandleUserBetResult(void *buffer);

	//申请上庄结果处理
	void HandleApplyResult(void *buffer);

	//空闲消息
	void	HandleGameFree(void * pBuffer);

	//开牌
	void HandleKaiPai(void *buffer);

	//判断是否申请上庄
	void JudgeApplyNt(BYTE ,BOOL);

	//收到开始下注消息
	void	HandleBeginBet(void * pBuffer);

	//处理申请下庄结果消息
	void HandleLeaveNtResult(void *buffer);
};
