// ClientGameDlg.h: Headers and defines for CClientGameDlg
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CLIENTGAMEDLG_H__INCLUDED_)
#define _CLIENTGAMEDLG_H__INCLUDED_

#include "StdAfx.h"
#include "upgrademessage.h"
#include "UpGradeLogic.h"

//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

// 定时器 ID
#define ID_BEGIN_TIME				10000			// 开始定时器
#define ID_SEND_CARD_TIMER          20000           // 发牌定时器ID
#define ID_KAIPAI_TIME				10001			// 开牌定时器 lym0514

#define ID_WAIT_TIME				100				// 等待定时器 lym0514
#define ID_XIAZHU_TIME				101				// 下注定时器 lym0514
#define ID_XIAZHU_TOTAL_TIME		102				// 下注总时间
#define ID_XIAZHU_WAIT				5				// 下注等待

#define ID_TEST_TIMER               20001           // 下注测试定时器ID

#define ID_RESULT_TIMER             20002           // 结算显示定时器ID

#define ID_RECT_TIMER               20003           // 闪烁矩形显示定时器ID

#define ID_XIANPAI					2004	//发牌发闲家的牌
#define ID_ZHUANGPAI				2005		//发庄家的牌

#define WM_USER_GAME_BEGIN        (WM_USER + 120)   // 游戏开始
#define WM_USER_TRUSTEE           (WM_USER + 121)   // 托管
#define WM_USER_HAVETHING         (WM_USER + 122)   // 有事要走
#define WM_USER_AGREE_LEAVE       (WM_USER + 123)   // 有事要走答复消息ID

#define WM_USER_OUT_CARD		  (WM_USER + 124)	// 用户出牌

#define WM_USER_SEND_THING        (WM_USER + 130)	// 发送有事要走消息
////下注筹码消息
#define WM_USER_XIA_ZHU           (WM_USER + 140)	// 发送玩家下注消息
#define WM_USER_SHANG_ZHUANG      (WM_USER + 141)	// 发送玩家上下庄消息
#define WM_USER_TEST              (WM_USER + 150)	// 发送测试消息

struct XiaZhuStruct
{
	int Count;		// 下注次数
	int Money;		// 下注钱
	int StopTime;	// 下注停止时间
	int MoneyType;	// 下注筹码类型
	int type;		//下注区域0-7
	int RunCount;	// 执行次数
		
	// [@param Count] 下注次数
	// [@param Money] 下注钱
	// [@param StopTime] 停止时间
	// [@param MoneyType] 筹码类型
	// [@param type] 下注区域
	// [@param RunCount] 执行次数
	int set(int Count, int Money, int StopTime, int MoneyType, int type, int RunCount=0)
	{
		this->Count = Count;
		this->Money = Money;
		this->StopTime = StopTime;
		this->MoneyType = MoneyType;
		this->type = type;
		this->RunCount = RunCount;

		return 0;
	}

	XiaZhuStruct()
	{
		Count = 0;
		Money = 0;
		StopTime = 0;
		RunCount = 0;
	}
};



class CClientGameDlg :public CLoveSendClass
{
public:
	CClientGameDlg(void);
	virtual ~CClientGameDlg(void);
	virtual BOOL OnInitDialog();

	XiaZhuStruct  m_XiaZhuStruct;

	DECLARE_MESSAGE_MAP()

	// 游戏的基本信息
	//GAME_BASE_INFO  m_GameBaseInfo;

	// 玩家坐的位置
	//BYTE m_abySeatID[PLAY_COUNT];
	//BYTE m_byViewOrder;

	// 数据统一保存逻辑位置
	//WORD m_wHandCard[PLAY_COUNT][USER_HAND_CARD_COUNT];   // 各玩家手中的牌
	//BYTE m_byHandCardCount[PLAY_COUNT];                   // 各玩家手中的牌张数

	//bool m_bIsTrustee[PLAY_COUNT];              // 托管	
	//int	 m_iAwardPoint[PLAY_COUNT];				// 牌形加分
	//BYTE m_bySequence[PLAY_COUNT];				// 各家出完牌先后顺序
	//bool m_bGiveUp[PLAY_COUNT];                 // 是否不出牌
	//BYTE m_byCurOrientation;				    // 当前出牌者方向

	//int m_iSendCardTime;						// 发牌动画的总时间，控制动画。
	
	//int  SoundbirdID;							//当前播放声音的id



	//WORD m_wCurSendPos;							// 当前发牌位置
	//WORD m_wSendCount;      // 总共发送张数
	//BYTE m_bySentCardCount[PLAY_COUNT];  // 各玩家已经发了的牌张数
	//BYTE m_byFirstSendPos;  // 第一个发牌玩家

	////////////////////////////////////////////////////////////////////
	BYTE m_byHighVersion;						// 游戏版本
	BYTE m_byLowVersion; 						// 游戏版本

	bool m_bCanXiaZhu;							// 能否下注；

	__int64  m_iMaxDownMoney;					// 每局最高下局
	int m_iZhuang;								// 记录上局开的是庄还是闲
	int m_iNowNtStation;						// 当前庄家的位置	
	__int64 m_iMeZhu;								// 当前本局下的注
	int m_iZnum;								// 庄家局数
	int m_iXnum;								// 开闲局数
	int m_iZhuangBaShu;							// 庄家进行了几把
	__int64  m_iShangZhuangLimit;				// 上庄需要的金币数

	__int64 m_iAIMoney[8];						//机器人在每个区域下的注
	int m_iMaxZhuang;							// 庄家进行几把就下庄
	int m_iBaseNum;								// 机器人最大只能下自身携带金币的几分之一
	int  m_nPowerOfGold;//放大倍数
	///当前玩家下的总注
	__int64		m_iMeZongZhu;	
	///当前房间最大能下的总注
	__int64		m_iMaxNote;	
	
	// 上下庄开关
	bool m_bSZSwitch;
	bool m_bXZSwitch;
	// 房间上庄条件是1亿，可设制机器人上庄金币在1亿以上两亿以下这个位置的数值里随机上庄
	__int64 m_iMaxApplyNtMoney; 
	__int64 m_iMinApplyNtMoney; 

	// 上庄输了多少钱，他即下庄，赢了多少钱他也可以下庄
	__int64 m_iWinDownMoney; 
	__int64 m_iLostDownMoney; 

	// 机器人可以设制自动下注，下注金额也要可以让管理员设制一个范围，下注也是按这个范围内下注，包括下注哪种面值的筹码
	int  m_iCanNoteValue; 

	int   m_byZTWangPct;						//庄天王下注概率(精确到千分之一)
	int   m_byZDZiPct;							//庄对子下注概率(精确到千分之一)
	int   m_byXTWangPct;						//闲天王下注概率(精确到千分之一)
	int   m_byXDZiPct;							//闲对子下注概率(精确到千分之一)
	int   m_byHePct;							//和下注概率	(精确到千分之一)
	int   m_byTDHePct;							//同点和下注概率(精确到千分之一)

	int m_probability[6][CHIP_NUM];//各筹码使用的概率

	//可配置项
	int                     m_iRobortSitZMax;            //机器人最大坐庄次数
	int                     m_iRobortSitZMin;            //机器人最小坐庄次数	
	//int                     m_iRobotNotePercent;       //当前机器人下注的百分比限制

	int                     m_iRobortSitCount;           //当前机器人上庄的把数
	BOOL                    m_bPlayerInList;             //是否有玩家在庄家列表中

	bool                    m_bUse500WJeton; 
    ///是否启用毫秒进行下注
	bool                    m_bUseMilliSecond ;
    ///启用的毫秒单位
	int                     m_iMilliSecond ; 

protected:
	//版本核对
	bool CheckVersion(BYTE byHighVersion,BYTE byLowVersion,TCHAR szUpDateHttp[100]);

	// 通知更新游戏界面
	//virtual bool OnNotifyUpdateClientView(void * pNetData, UINT uDataSize){ return true; }
	// 服务器发牌
	//virtual bool OnNotifySendCard(void * pNetData, UINT uDataSize){ return true; }
	// 发送一张牌
	//void OnNotifySendACard(void);
	// 发牌结束
	//void OnNotifySendCardFinish(void);
	// 玩家强退结束游戏
	//virtual bool OnNotifyCutEnd(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize){ return true; }
	// 托管消息响应函数
	//virtual void OnChangeTrustee(BYTE byDeskStation, bool bIsTrustee){}

public:
	// 清除所有定时器
	void KillAllTimer(void);	
	// 开始按钮函数
	//LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);

	// 托管按钮
	//LRESULT	OnBtnTrustee(WPARAM wparam, LPARAM lparam);
	// 有事离开按钮
	//LRESULT	OnBtnHaveThing(WPARAM wparam, LPARAM lparam);
	// 发送离开请求函数
	//LRESULT	OnSendHaveThing(WPARAM wparam, LPARAM lparam);
	// 同意玩家离开
	//LRESULT	OnAgreeUserLeave(WPARAM wparam, LPARAM lparam);

	// 下注按钮函数
	LRESULT	OnXiaZhu(WPARAM wparam, LPARAM lparam);
	// 上下庄申请按钮函数
	//LRESULT	OnShangZhuang(WPARAM wparam, LPARAM lparam);
	// 测试消息函数
	LRESULT	OnTest(WPARAM wparam, LPARAM lparam);

// 重载
public:
	// 座位换算到视图位置
	//virtual BYTE GetViewSeat(BYTE byDeskStation){ return true; }
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	///重写设置定时器
	virtual bool SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond = false) ;
	//定时器消息
	virtual bool OnGameTimer(BYTE byDeskStation, UINT uTimeID, UINT uTimeCount);
	// 同意开始游戏 
	virtual bool OnControlHitBegin(){ return true; }
	// 安全结束游戏
	virtual bool OnControlSafeFinish();
	// 改变旁观设置
	//virtual void OnWatchSetChange(void){};
	// 改变旁观设置 游戏中发送牌数据，用于更新界面
	//virtual void OnWatchSetChange(S_WATCH_SET_CHANGE &sWatchSetChange){};
	// 游戏设置
	//virtual void OnGameSetting(){};
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation);
	// 用于玩家换位：比如逆时针、顺时针出牌
	//virtual void OnBeforeUpdateViewSeat(void){};


protected:
	virtual void OnCancel();

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
	// 系统开牌消息lym0512
	void OnHandleKaiPai(void * pNetData, UINT uDataSize);
	// 玩家下注消息
	void OnHandleXiazhu(void * pNetData, UINT uDataSize);
	// 处理百家乐游戏开始消息lym0512
	void OnHandleBegin(void * pNetData, UINT uDataSize);
	// 处理百家乐上庄消息
	void OnHandleApplyNt(void * pNetData, UINT uDataSize);
	//void PlayOtherSound(TCHAR* path);
	//重载视图转换函数
	//BYTE ViewStation(BYTE bDeskStation);
	//重载最大化事件
	//void OnBnClickedRest();
	//void OnBnClickedMax();
	//开始一张牌动画的函数
	//void StartMoveCard(bool zhuang,int num, int zhen);
	//void StartChao500Move(int type);//启动超过500万的动画
	//绘制超过500万的动画
	//void StartChao500Move(CDC*pDC,int iWidth, int iHeight);
	//void GetGameCode(char * szGameCode);
	void WriteInfo(TCHAR * pch);//add by wlr 20090806
	///////////////////////////////////////////////////
	//随机下注
	void OnSuiJiXiaZhu();

	//获取随机下注区域
	int GetRandType();
	int  Getchip(int line_num);			///运用权重概率性获取一个位置
	//加载配置文件
	void LoadIniConfig();

	///检测玩家是否可以上庄
	inline  bool CheckApply(__int64 iMyMoney)
	{
		/*if (m_bSZSwitch)
		{
			return (iMyMoney >= m_iMinApplyNtMoney && iMyMoney <= m_iMaxApplyNtMoney) ; 
		}*/
		return true;
	}
	///检测庄家是否需要下庄
	inline  bool  CheckDownNt(__int64  iWinMoney)
	{
		//return (iWinMoney >= m_iWinDownMoney || iWinMoney <= (-m_iLostDownMoney)) ; 
		return true;
	}	 


	///日志记录接口
	void  SrvTestlog(TCHAR *pMessage);

};
#endif // !defined(_CLIENTGAMEDLG_H__INCLUDED_)