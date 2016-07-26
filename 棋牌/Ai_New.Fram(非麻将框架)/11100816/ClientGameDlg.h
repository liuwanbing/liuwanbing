#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpGradeLogic.h"
#include "GameMessage.h"
#include "UpgradeMessage.h"

#define MAST_PEOPLE			-1							//是否为管理员

//消息定义
#define IDM_BEGIN			WM_USER+120					//开始按钮消息
#define IDM_HAVE_THING		WM_USER+127					//有事要走
#define IDM_SEND_THING		WM_USER+128					//发送离开请求
#define IDM_ARGEE_LEAVE		WM_USER+129					//同意离开消息
#define IDM_STOP			WM_USER+130					//提前结束按钮
#define IDM_STOP_THING		WM_USER+131					//发送提前结束
#define IDM_ARGEE_STOP		WM_USER+132					//同意提前结束
#define IDM_PASS			WM_USER+133					//不出按钮消息
#define IDM_AUTO			WM_USER+134					//托管消息
#define	IDM_ACTIVE_RESULT	WM_USER+136					//用户处理结果
#define IDM_SHOW_NOTE		WM_USER+137					//用户选择下注,加注
#define IDM_SELECT_NOTE_NUM	WM_USER+138					//用户下注
#define	IDM_SOUND			WM_USER+139					//用户设置声音
#define IDM_BGSOUND			WM_USER+140					//背景音乐

//定时器 ID
#define ID_BEGIN_TIME		100							//开始定时器
#define TIME_XIAZHU         101							//下注定时器
#define TIME_EMPTY          102							//空闲时间定时器
#define TIME_KAIPAI         103						    //开牌时间
#define TIME_WAIT           104                         //等待定时器
#define TIME_XZ_TIME        105                         //申请下庄定时器
#define TIME_SZ_TIME        106                          //上庄定时器

//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置
#define GAME_RAND_TIME				5				    //系统时间

//声音类型设置
#define SOUND_WARNING				0x10				//警告
#define SOUND_SEND_CARD				0x20				//发牌声音
#define SOUND_INVALIDATE			0x30				//提示失败
#define SOUND_START_DEF				0x40				//四个人开始游戏后
#define SOUND_GAME_OVER				0x50				//游戏结束
#define SOUND_WIN                   0x60                //胜利
#define SOUND_LOST                  0x70                //失败
#define SOUND_NOTE                  0x80                //下注筹码声
#define SOUND_START                 0x90                //游戏开始
#define SOUND_KAIPAI                0xA0                //开牌


//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	/////////////////////////
public:
	//bool					m_bIsFinish;				//游戏是否已经结束
	//本局庄家
	BYTE                    m_zhunagStation;            //庄家
	int                     m_iGameBeen;                //当前庄家游戏局数
	__int64                 m_iMyZhu;                   //本人下注
	int                     m_iZhuangMaxCount;          //本局庄家做庄最大次数
	int                     m_iMaxDownMoney;            //每局最高下注	
	int                     m_iTempMoney;               //百亿
public:
	
	BYTE					m_iBeginTime;				//游戏开始时间
	BYTE					m_iThinkTime;				//暂时未用
	BYTE                    m_iXiaZhuTime ;             ///下注时间
	BYTE                    m_iKaipaiTime ;             ///开牌时间

	int						m_iRoomBasePoint;			//房间倍数
	int						m_iRunPublish;				//逃跑扣分
	int						m_iLimitNote;				//最大限额注
	int						m_iBaseNote;				//底注

	__int64					m_iMyXiaZhuCount;			//本人下注总数	

	int						m_iNtMaxCount;				//机器人最大坐庄次数
	int						m_iNtMinCount;				//机器人最少坐庄次数
	int						m_iMyNtCount;				//我当庄当了多少局

	//控件变量
public:
	GameLogic               m_Logic;				    //游戏逻辑

	int						m_iVersion;
	int						m_iVersion2;

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

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT nIDEvent);

public:
	//函数声明
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

	//重置游戏
	void ResetGameStation(int iGameStation);

public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);

	// 游戏开始
	void OnGameBegin(GameBeginStruct * pGameBegin);

	// 玩家申请上庄、下庄消息
	void OnUserApplyZhuang(EZP_APPLY_ZHUANG * pApplyZhuang);

	// 玩家下注信息
	void OnUserXiaZhuInfo(EZP_XIAZHU_INFO * userXiazhu);

	// 开始游戏(游戏进行中...)
	void OnPlayGame(EZP_PLAY_GAME * pPlayGame);

public:
	//随机下注
	void OnSuiJiXiaZhu(int  &iNotetype);	
	///获取随机数
	int  OnGetRandNum(int iDeskStation,int iMaxNum,int iMinNum ) ; 
		

};
