#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpGradeLogic.h"
#include "UpGradeMessage.h"


#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define ID_BEGIN_TIME				101					//开始定时器
#define ID_BET_TIMER				102					//下注计时器
#define ID_APPLYNT_TIMER			103					//申请上庄




//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

//6个筹码值
const	int	G_iChouMaValues[6] = {100,1000,10000,100000,1000000,10000000};

//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	//变量定义
public:

	BYTE                m_byGameStation ;       ///当前的游戏状态


	int					m_iXiaZhuTime;			///下注时间	
	int					m_iKaiPaiTime;			///开牌时间  
	int					m_iShowResult;			///显示结算框时间
	int					m_iFreeTime;			///空闲时间	

	__int64				m_iLimitMoney;			///<上庄需要的最少金币
	__int64				m_iMaxNote;				///当前房间最大能下的总注

	__int64				m_i64MaxZhu[BET_ARES];       //每个区域能下的最大注	

	short int			m_iZhuangList[PLAY_COUNT+1];		///庄家列表


	int					m_iNTju;				///<当前庄家进行的局数
	int					m_iNowNTstation;		///<当前庄家的位置

	__int64				m_i64MyBetCount;		//自己下注总数
	__int64				m_i64NtWinMoney;		//庄家的成绩
	__int64				m_i64MyWinMoney;		//自己输赢情况


	BYTE				m_byNowCard[2][3];		///<当前数据//庄闲的牌，0为闲，1为庄

	__int64				m_iRobotMaxBet;			//机器人最大下注

	//控件变量
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
	//用户离开
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);


	//消息函数
public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);


	void ResetGameStation(int iGameStation);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	//	afx_msg void OnClose();
protected:
	virtual void OnCancel();


private:
	//随机获取下注区域
	int		GetSelArea();
	//随机下注筹码
	int		GetSelChouMa();



};
