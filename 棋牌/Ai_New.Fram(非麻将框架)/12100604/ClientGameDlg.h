#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpGradeLogic.h"
#include "UpGradeMessage.h"


#define MAST_PEOPLE			-1							//是否为管理员

//定时器 ID
#define ID_BEGIN_TIME				101					//开始定时器
#define ID_ROB_NT_TIMER				102					//抢庄定时器
#define ID_NOTE_TIMER               103	                //下注计时器
#define ID_OPEN_CARD_TIMER			104					//摆牛计时器


//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	//变量定义
public:
	//固定信息
	//=====================时间表信息===================
	BYTE					m_iThinkTime;				//最少游戏盘数
	BYTE					m_iBeginTime;				//游戏开始时间
	BYTE					m_iCallScoreTime;			//游戏开始时间
	BYTE					m_iXiaZhuTime;				//下注时间
	int						m_iRoomBasePoint;			//房间倍数
	int						m_iRunPublish;				//逃跑扣分
	int						m_iVersion;
	int						m_iVersion2;

	BYTE					m_iCardCount;				//允许游戏的扑克数目
	BYTE					m_iPalyCardCount;			//游戏扑克数目


	__int64						m_iThisTurnLimit;			//本轮限注

	//运行信息
	BYTE					m_iDeskCardCount[PLAY_COUNT];		//每人桌面扑克的数目
	BYTE					m_UserCard[PLAY_COUNT][SH_USER_CARD];			//用户扑克
	BYTE                    m_iUpBullCard[PLAY_COUNT][3];//升起的牛牌

	BYTE					m_bUpCard[2];				//上面的牌
	BYTE                    m_bUnderCard[3];			//下面的牌

	//运行信息
	BYTE						m_bTimeOutCount;			//超时
	BYTE						m_bShowAnimalStation;		//动画显示位置
	int					    	m_iNowOutPeople;			//现在出牌者
	int						    m_iFirstOutPeople;			//第一个出牌者
	__int64					    	m_iNowBigNote;				//当前最大押注数
	__int64						    m_iThisGameNote[PLAY_COUNT];			//本局游戏下注数(明注)
	__int64						    m_iTotalGameNote[PLAY_COUNT];			//本局游戏下注数(实际)
	int						    m_iUpGradePeople;					//庄家位置
	int                         m_iNote;                    //当前有效下注数即前位玩家有效下注数;
	BOOL						m_bBgSound;					//背景音乐
	BOOL                        m_bMustGiveup;
//	BYTE					m_iGameNoteSetType;				//是否启用牛牛中所有的牌型的最大值 
	int						m_iGameTimes;					/**<游戏局数*/					
	bool					m_bLeftOrGoOn;					/**<游戏结束后是离开还是继续游戏 true:继续游戏 false:离开*/
	BYTE					m_byGameTimes;					/**<多少局游戏后离开游戏*/
	BYTE					m_byLeaveThinkTime;				/**<离开思考时间*/
	int						m_iProNote[4];					/**<四个筹码值 下注概率 和 为 100%*/
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



};
