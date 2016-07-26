#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"
#include "fish/GameScreenWnd.h"

#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define TIME_FISH_ROUND			101			//鱼儿角度转换计时器
#define TIME_BULLTE_RUN			102			//子弹和鱼的运行计时器
#define TIME_FISH_MOVE			103			//鱼儿游动序列计时器 delete
#define TIME_NET_OPEN			104			//子弹撒网计时器
#define TIME_HIT_FISH			105			//击中鱼儿

#define TIME_ADD_FIRE			106			//加炮
#define TIME_ADD_SCORE			107			//上分
#define TIME_SHOT				108			//发射炮弹





//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{

	//控件变量
public:

	int						m_iVersion;
	int						m_iVersion2;
public: 

	BYTE                m_byGameStation ;         ///当前的游戏状态
	int                 m_iMyDeskStation ;       ///玩家自己的游戏位子
	bool				m_bIsPlaying;				//游戏是否已经开始

private:
	//游戏场景类
	GameScreenWnd		m_GameScreen;
	//游戏数据
	GameStation_Base	m_GameData;

	__int64		m_i64MinAddScore;		//最小上分
	__int64		m_i64MaxAddScore;		//最大上分
	int			m_iMinFire;				//最小炮火倍数
	int			m_iMaxFire;				//最大炮火倍数
	int			m_iAutoAddScoreSet;		//自动上分设置

	int			m_iMinShotTime;			//最快发射间隔
	int			m_iMaxShotTime;			//最慢发射间隔

	int			m_iTimeContrl;			//发射子弹间隔控制


	__int64		m_i64RandAddScore;
	int			m_iRandFire;

	//函数定义
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();
 
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	
	void	LoadRobotSet();
private:
	//切换座位
	int		SwitchChairID(int m_Currchairid);

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


};
