#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "ClientView.h"
#include "UpGradeLogic.h"
#include "GameSet.h"
#include "LoveSendClassInExe.h"

#ifdef _USE_VIDEO
#include "BRGameVideoSDK.h"
/**
* 游戏视频结构
*/
struct MyGameVideo
{
	MyGameVideo()
		: VideoServerPort(0)
	{
	}
	MyGameVideo(const char *serverip,int port)
		: VideoServerPort(port)
	{
		if(serverip) strcpy(VideoServerIP,serverip);
	}
	char VideoServerIP[256];                 // 视频服务器IP
	int VideoServerPort;                     // 视频服务器端口
};
#endif


/// 消息定义
#define IDM_AGREE_INFO		WM_USER+118					///< 玩家同意对家设置的信息
#define IDM_DAOSHI_MONEY	WM_USER+119					///< 玩家同意扣除导师费信息
#define IDM_BEGIN			WM_USER+120					///< 开始按钮消息
#define IDM_USER_NT			WM_USER+121					///< 用户反牌
#define IDM_GIVE_BACK		WM_USER+123					///< 留底牌按钮
#define IDM_OUT_CARD		WM_USER+124					///< 用户出牌
#define IDM_LOOK_LAST		WM_USER+125					///< 看上轮扑克
#define IDM_BACK_CARD		WM_USER+126					///< 底牌信息
#define IDM_HAVE_THING		WM_USER+127					///< 有事要走
#define IDM_SEND_THING		WM_USER+128					///< 发送离开请求
#define IDM_ARGEE_LEAVE		WM_USER+129					///< 同意离开消息
#define IDM_STOP			WM_USER+130					///< 提前结束按钮
#define IDM_STOP_THING		WM_USER+131					///< 发送提前结束
#define IDM_ARGEE_STOP		WM_USER+132					///< 同意提前结束
#define IDM_ADMIT_DEFEAT	WM_USER+133					///< 认输
#define IDM_REGRET_CHESS	WM_USER+134					///< 悔棋
#define IDM_CHESS_MANUAL	WM_USER+135					///< 棋谱
#define IDM_SEND_REN_SHU	WM_USER+136					///< 发送认输
#define IDM_AGREE_REGRET	WM_USER+137					///< 同意悔棋

/// 定时器 ID
#define ID_BEGIN_TIME				100					///< 开始定时器
#define ID_WAIT_ROB					101					///< 等待抢底牌
#define ID_GIVE_BACK				102					///< 等待留底牌
#define ID_OUT_CARD					103					///< 出牌等待时间
#define ID_SHOW_LAST				104					///< 查看上轮扑克
#define ID_SHOW_CARD				105					///< 甩牌定时器
#define ID_LOOK_BACK				106					///< 查看底牌

#define ID_CANT_MONEY				199					///< 身上钱不足导师费
#define ID_DAOSHI_QUIT				200					///< 身上钱不足导师费

/// 重置函数参数
#define RS_ALL_VIEW					1					///< 重置所有参数
#define RS_GAME_CUT					2					///< 用户离开重置
#define RS_GAME_END					3					///< 游戏结束重置
#define RS_GAME_BEGIN				4					///< 游戏开始重置

/// 五子棋声音设置
#define SOUND_THREE					0x01				///< 冲三
#define SOUND_FOUR					0x02				///< 冲四
#define SOUND_WIN					0x03				///< 胜利
#define SOUND_ERROR					0x04				///< 错误
#define SOUND_GOOD_BYE				0x05				///< 再见
#define SOUND_GAME_BEGIN			0x06				///< 新游戏
#define SOUND_PUT_STONE				0x07				///< 下子
#define SOUND_UNDO					0x08				///< 悔棋
#define	SOUND_WELCOME				0x09				///< 欢迎
#define SOUND_GAME_END				0x10				///< 游戏结束
#define SOUND_WARNING				0x11				///< 警告

/// 用同样的方法重复将军次数，超过该次数则自动判其耍赖,定为5次
#define GAMER_SHUALAI_NUM           5

/// 游戏框架类 
class CClientGameDlg : public CLoveSendClassInExe//CGameFrameDlg
{
	/// 变量定义
public:
	/// 记录对方玩家耍赖结构信息
	GamerShuaLaiJiang       m_gamerShua;     
	int                     m_iGamerShuaLai;            ///< 对方用同样的方法的重复次数

	/// 固定信息
	BYTE                    m_iAfterWaitAction;			///< Colin090311开始倒计时结束后的动作 1 自动开始 2 自动离开 

	BYTE					m_iLessCount;				///< 最少游戏盘数
	BYTE					m_iBeginStation;			///< 开始位置
	BYTE					m_iEndStation;				///< 结束位置
	BYTE					m_iCardCount;				///< 允许游戏的扑克数目
	BYTE					m_iPalyCardCount;			///< 游戏扑克数目
	int						m_iRoomBasePoint;			///< 房间倍数
	int						m_iRunPublish;				///< 逃跑扣分
	int                     m_BgePeaceCount;            ///< 求和次数
	/// 自己扑克数据
	BYTE					m_iMeCardCount;				///< 自己扑克的数目
	BYTE					m_iMeCardList[45];			///< 自己扑克列表

	/// 主牌信息
	BYTE					m_iNtCount;					///< 亮主的扑克数目
	BYTE					m_iNtPeopole;				///< 亮主的游戏者
	BYTE					m_iBackCard[8];				///< 底牌扑克

	/// 运行信息
	BYTE					m_iDeskCardCount[4];		///< 每人桌面扑克的数目
	BYTE					m_DeskCard[4][39];			///< 每人桌面的扑克

	/// 运行信息
	BYTE					m_iNowOutPeople;			///< 现在出牌者
	BYTE					m_iBaseCount;				///< 第一个出牌者牌数目
	BYTE					m_iFirstOutPeople;			///< 第一个出牌者
	BYTE					m_iBaseCard[39];			///< 第一个出牌者的扑克

	/// 上轮显示数据
	BYTE					m_iLastCardCount;			///< 上轮扑克的数目
	BYTE					m_bShowLast;				///< 是否可以显示上轮扑克
	BYTE					m_bShowIngLast;				///< 是否正显示上轮扑克
	BYTE					m_iLastOutCard[4][39];		///< 上轮的扑克

	/// 定时器计数
	//int						m_iTimerStation;		///< 定时器位置
	//int						m_iTimerCount;			///< 计数器剩时间
	BYTE						m_bTimeOutCount;		///< 超时
	int							m_nJiaoZu;

	int							m_iStartI,				///< 走子起点
								m_iStartJ,
								m_iEndI,				///< 走子终点
								m_iEndJ;

	int							m_iMeBoard[10][9];		///< 本方棋局
	int							m_iMeChessFlag;			///< 本方标志
	int							m_iCurPlayFlag;			///< 当前走子者

	int							m_iJuTimeMinute,		///< 局时分
								m_iJuTimeSecond,		///< 局时秒
								m_iBuTimeMinute,		///< 步时分
								m_iBuTimeSecond,		///< 步时秒
								m_iDuMiaoMinute,		///< 读秒分
								m_iDuMiaoSecond,		///< 读秒秒
								m_iGameMoney;			///< 指导费
	bool						m_havething;
	TCHAR						m_tPlayName[PLAY_COUNT][62];///< 玩家名字
	bool                        m_bNoHaveAgreeRegret;  //是否点击了同意悔棋或不同意悔棋

	BYTE						m_daoshi;
	/// 控件变量
public:
	CUpGradeGameLogic		m_Logic;					///< 升级逻辑
	CClientPlayView			m_PlayView;					///< 游戏视图
	CGameSet				*m_GameSet;
	int						m_iVersion;
	int						m_iVersion2;

#ifdef _USE_VIDEO
	CBRGameVideoSDK         *m_pBRGameVideoSDK;             /// 视频控件
	MyGameVideo             m_GameVideoSet;                 /// 游戏视频设置
	std::map<std::string,int>   m_VideoUserInfo;             /// 3个视频用户
#endif

	/// 函数定义
public:
	/// 构造函数
	CClientGameDlg();
	/// 析构函数
	virtual ~CClientGameDlg();
    int GetGameStatus();
protected:
	/// 初始化函数
	virtual BOOL OnInitDialog();
	/// 数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	virtual void OnWatchSetChange(void);
	virtual void OnGameSetting();
	bool	AFCCloseFrame(bool bNotifyServer = true);

	virtual void FixControlStation(int iWidth, int iHeight);

	/// 重载函数
public:
	/// 设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	/// 游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize/*, CTCPClientSocket * pClientSocket*/);
	/// 重新设置界面
	virtual void ResetGameFrame();
	/// 定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);
	/// 清除所有定时器
	void KillAllTimer();
	/// 同意开始游戏 
	virtual bool OnControlHitBegin();
	/// 安全结束游戏
	virtual bool OnControlSafeFinish(){return false;};
	/// 用户离开
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	/// 用户进入
	virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);

	/// 消息函数
public:
	// 玩家同意对家设置的信息
	LRESULT	OnHitAgreeInfo(WPARAM wparam, LPARAM lparam);
	//玩家同意对家设置的信息
	LRESULT	CClientGameDlg::OnAgreeDaoShiMoney(WPARAM wparam, LPARAM lparam);
	/// 开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	/// NT 按钮函数
	LRESULT	OnHitNt(WPARAM wparam, LPARAM lparam);
	/// 按动扑克按钮函数
	LRESULT	OnHitCard(WPARAM wparam, LPARAM lparam);
	/// 留底牌按钮函数
	LRESULT	OnHitBackCard(WPARAM wparam, LPARAM lparam);
	/// 按动出牌按钮函数
	LRESULT	OnHitOutCard(WPARAM wparam, LPARAM lparam);
	/// 按动上轮按钮函数
	LRESULT	OnHitLastTurn(WPARAM wparam, LPARAM lparam);
	/// 按动看底牌按钮函数
	LRESULT	OnHitLookBack(WPARAM wparam, LPARAM lparam);
	/// 按动有事按钮函数
	LRESULT	OnHitHaveThing(WPARAM wparam, LPARAM lparam);
	/// 发送离开请求函数
	LRESULT	OnSendHaveThing(WPARAM wparam, LPARAM lparam);
	/// 用户请求离开
	LRESULT	OnArgeeUserLeft(WPARAM wparam, LPARAM lparam);
	/// 右键用牌
	LRESULT	OnRightHitCard(WPARAM wparam, LPARAM lparam);
	/// 提前结束按钮
	LRESULT	OnStop(WPARAM wparam, LPARAM lparam);
	/// 发送提前结束
	LRESULT	OnStopThing(WPARAM wparam, LPARAM lparam);
	/// 同意提前结束
	LRESULT	OnAgreeStop(WPARAM wparam, LPARAM lparam);	
	/// 认输
	LRESULT OnAdmitDefeat(WPARAM wparam, LPARAM lparam);
	/// 悔棋
	LRESULT OnRegretChess(WPARAM wparam, LPARAM lparam);
	/// 棋谱
	LRESULT OnChessManual(WPARAM wparam, LPARAM lparam);
	/// 超时处理
	LRESULT OnSuperTimer(WPARAM wparam, LPARAM lparam);
	/// 确认认输
	LRESULT OnSendAdmitDefeat(WPARAM wparam, LPARAM lparam);
	/// 同意悔棋
	LRESULT OnAgreeRegret(WPARAM wparam, LPARAM lparam);
	BOOL UpdateNtList();
	void ResetGameStation(int iGameStation);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);

#ifdef _USE_VIDEO
	afx_msg LRESULT OnGVClientEnterRoom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientLogin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientUserAtRoom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientOnlineUser(WPARAM wParam, LPARAM lParam);
	/// 刷新所有的视频面板
public:
	void RefreshAllVideoPanel(void);
#endif

//	afx_msg void OnClose();
protected:
	virtual void OnCancel();
public:
	bool IsWatcher(void);
	/// 声音设置
	BOOL SetPlaySound(BYTE bDeskStation,BYTE bSoundType,BOOL bExtVal=false);
	/// 函数定义(版本核对)
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2,TCHAR pUpDateHttp[100]);
	/// 用户下子
	BOOL UserPutStone();
	/// 位置转换(将黑白字位置换过来)
	int StationSwap(BYTE iViewStation,BYTE iMeChessFlag);
	/// 位置坐标转换
	int SwapIx(int iX);
	/// 位置坐标转换
	int SwapIy(int iY);
	/// 录影接口函数lc090227
	virtual void SetRecordView();

};
