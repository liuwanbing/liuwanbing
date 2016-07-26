#pragma once

#include "StdAfx.h"

#include "CMD_Fish.h"
#include "Include\Macro.h"
#include "Include\Define.h"
#include "UpgradeMessage.h"

//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
private:
	int exchange_ratio_userscore_;
	int exchange_ratio_fishscore_;
	int exchange_count_;

	int min_bullet_multiple_;
	int max_bullet_multiple_;

	BulletKind current_bullet_kind_;
	int current_bullet_mulriple_;
	bool allow_fire_;
	bool m_tag_fire;//是否启动了开火间歇计时器

	SCORE exchange_fish_score_;
	SCORE fish_score_;//鱼币

	float last_fire_angle_;
	int exchange_times_;

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

	//计时器
	bool SetGameTimerEx(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT nIDEvent);

public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);

	//游戏事件
public:
	//游戏消息
	bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	// 消息处理
protected:
	bool OnSubGameConfig(void* data, WORD data_size);
	bool OnSubExchangeFishScore(void* data, WORD data_size);
	bool OnSubUserFire(void* data, WORD data_size);
	bool OnSubCatchFish(void* data, WORD data_size);
	bool OnSubBulletIonTimeout(void* data, WORD data_size);
	bool OnSubCatSweepFishResult(void* data, WORD data_size);
	bool OnSubSwitchScene(void* data, WORD data_size);

private:
	void ExchangeFishScore();
	void Fire();
	float GetAngle(WORD chair_id);


public:
	//生成随机数
	int MyRand();
};
