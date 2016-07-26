#pragma once
//#include "defines.h"
//#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "upgrademessage.h"
#include "./UpGradeLogic.h"
#include <list>

#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_THINK_TIME				103					//思考定时器
#define ID_SCARD_TIME				104					//发牌定时器
#define ID_RESULT_TIME				105					//结束定时器


//游戏框架类 
class CClientGameDlg : public CLoveSendClass
{
private:
	CUpGradeGameLogic		m_Logic;							///逻辑

	IniData					m_sIData;							///服务端INI文件数据

	BData					m_sBData;							///游戏基本数据

	BYTE					m_bGameStation;						///游戏状态

	BYTE					m_byLgStation;						///玩家逻辑位置

	bool					m_bWatchMode;						///是否为旁观者

	bool					m_bWatch;							///是否充许旁观

	bool					m_bSound;							///是否播放音效

	int						m_iUCCount[PLAY_COUNT];				///玩家手牌数量

	BYTE					m_byUCard[PLAY_COUNT][USER_HAND_CARD];	///玩家扑克
				
	BYTE                    m_byDownBCard[PLAY_COUNT][USER_HAND_CARD];			///下墩牌

	int						m_iSCCount;							///发牌张数
	
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();


	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnShowFinishDlg(WPARAM wParam, LPARAM lParam);
public:
	/**
	*初始化游戏数据
	*
	*return bool 初始化游戏数据是否成功
	*/
	bool InitGameData();

	/**
	* 重新设置游戏中客户端的数据
	*
	*return bool true:重新设置成功，false:重新设置失败
	*/
	bool  ResetGameData();

	/**
	* 提示
	*/
	void OnHitCue(); 

	/**
	* 摊牌
	*/
	void OnHitTanPai();

	/**
	* 打印日志
	*
	*/
	void DebugPrintf(const char *p, ...);

public:

	//--------------------------------重载函数-----------------------------------------
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);

	//清除所有定时器
	void KillAllTimer();
	
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);

	//同意开始游戏 
	virtual bool OnControlHitBegin();

	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
};
