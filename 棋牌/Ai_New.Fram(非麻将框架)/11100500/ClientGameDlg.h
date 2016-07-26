#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"

#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID100以上

#define		TIME_BET			102			//下注计时器
#define		TIME_APPLY			103			//申请上庄计时器
#define TIME_LEAVE			105			//申请下庄


//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{

	//控件变量
public:

	int						m_iVersion;
	int						m_iVersion2;
private://机器人中可配置的变量
	BYTE                m_iRobotMaxNtLimit;									//机器人最大上庄人数
	__int64             m_iRobotBetSumLimit;								//机器人下注限制
	int					m_iBetChouMaPro[6];				//下注筹码的概率
public: 
	BYTE                m_byGameStation ;									///当前的游戏状态
	int					m_iXiaZhuTime;										///下注时间				
	int					m_iKaiPaiTime;										///开牌时间       
	int					m_iFreeTime;										///空闲时间	
	__int64				m_i64ChoumaValue[6];					///六个筹码值DWJ
	__int64				m_i64MaxNote;					/// 每个玩家最多下注数DWJ
	__int64             m_i64ApplyNtLimite ;								//庄家上庄条件
	__int64				m_iMyBetSum;				//自己的下注总数
	BYTE				m_byWaitNtList[PLAY_COUNT+1];						//上庄列表
	BYTE				m_byNtStation;										//庄家位置
	
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
	//开始游戏
	void		OnHandleBet(void * buffer,int nLen);
	void		OnHandleFree(void * buffer,int nLen);			//接收到等待消息
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

public:
	//机器人上庄
	void	RobortApplyNt();
	//机器人下注
	void	RobortBet();

	//初始化游戏数据
	void InitGameData();

	//加载配置文件
	void LoadGameIni();

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
	//判断是否申请上庄
	void JudgeApplyNt(bool bFlag);


};
