///
///ClientGameDlg.h: Headers and defines for CClientGameDlg
///
///

#if !defined(_CLIENTGAMEDLG_H__INCLUDED_)
#define _CLIENTGAMEDLG_H__INCLUDED_

#include "ClientView.h"
#include "../GameMessage/UpGradeLogic.h"
#include "LoveSendClassInExe.h"
#include "irrKlang/irrKlang.h"

using namespace irrklang;

///重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

/// 定时器 ID
/// 开始定时器
#define ID_BEGIN_TIME				10000			
/// 发牌定时器ID
#define ID_SEND_CARD_TIMER          20000           
/// 开牌定时器 lym0514
#define ID_KAIPAI_TIME				10001			
/// 等待定时器 lym0514
#define ID_WAIT_TIME				10002			
/// 下注定时器 lym0514
#define ID_XIAZHU_TIME				10003		
/// 发牌间隔
#define ID_SENDCARD_INTERVAL		10004			
 /// 下注测试定时器ID
#define ID_TEST_TIMER               20001       
/// 结算显示定时器ID
#define ID_RESULT_TIMER             20002          
 /// 闪烁矩形显示定时器ID
#define ID_RECT_TIMER               20003         
/// 发牌发闲家的牌
#define ID_XIANPAI					2004	
/// 发庄家的牌
#define ID_ZHUANGPAI				2005	
/// 播放声音
#define ID_PLAY_SOUND				2006	
//下注满时的气泡信息
#define ID_FULLNOTE_TIME            2007            
/// 游戏开始
#define WM_USER_GAME_BEGIN        (WM_USER + 120)  
/// 托管
#define WM_USER_TRUSTEE           (WM_USER + 121)   
/// 有事要走
#define WM_USER_HAVETHING         (WM_USER + 122)   
/// 有事要走答复消息ID
#define WM_USER_AGREE_LEAVE       (WM_USER + 123)   
/// 用户出牌
#define WM_USER_OUT_CARD		  (WM_USER + 124)	
/// 发送有事要走消息
#define WM_USER_SEND_THING        (WM_USER + 130)	
///下注筹码消息
#define WM_USER_XIA_ZHU           (WM_USER + 140)	/// 发送玩家下注消息
/// 发送玩家上下庄消息
#define WM_USER_SHANG_ZHUANG      (WM_USER + 141)	
/// 发送测试消息
#define WM_USER_TEST              (WM_USER + 150)	
/// 发送银行消息
#define WM_USER_BANK              (WM_USER + 151)	
/// 发送超级客户端控制消息
#define WM_SUPER_CTR              (WM_USER + 152)	
#define IDM_SEND_AI_CTRL			WM_USER + 153	//发送机器人控制改变信息

/// 超五百万声音
#define ID_SOUND_MORE_500W        0x01   
/// 五百万声音
#define ID_SOUND_500W             0x02   
/// 一百万声音
#define ID_SOUND_100W             0x03   
 /// 大筹码声音
#define ID_SOUND_DCM              0x04  
 /// 筹码声音
#define ID_SOUND_CM               0x05  
/// 发牌的间隔时间
#define SEND_CARD_TIME            400   

/// 提示换庄信息
#define PROMPT_CHANGE_NT_TIME     401   

/// CClientGameDlg 客户端，游戏框架类，接收游戏消息
class CClientGameDlg :public CLoveSendClassInExe//CLoveSendClass
{
public:
	CClientGameDlg(void);
public:
	virtual ~CClientGameDlg(void);	

	CClientPlayView	  m_PlayView;					//游戏视图
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	///最近30局的信息，庄牌点，闲牌点，赢区域
	int	m_iGameInfo[MAXCOUNT][4]; 

protected:
	BYTE	m_byHighVersion;  // 游戏版本
	BYTE	m_byLowVersion;   // 游戏版本

	/// 游戏的基本信息
	GAME_BASE_INFO  m_GameBaseInfo;

	/// 玩家坐的位置
	BYTE m_abySeatID[PLAY_COUNT];
	BYTE m_byViewOrder;

	/// 各玩家手中的牌
	WORD m_wHandCard[PLAY_COUNT][USER_HAND_CARD_COUNT]; 
	/// 各玩家手中的牌张数
	BYTE m_byHandCardCount[PLAY_COUNT];               
    /// 超级客户端标志
	BOOL m_bSuperState;                              
	/// 播放声音
	bool m_PlaySound;								
	/// 托管
	bool m_bIsTrustee[PLAY_COUNT];         			   
	 /// 牌形加分
	int	 m_iAwardPoint[PLAY_COUNT];					
    /// 庄家位置
	int	 m_iNtStation;                     			 
	 /// 各家出完牌先后顺序
	BYTE m_bySequence[PLAY_COUNT];					
	/// 是否不出牌
	bool m_bGiveUp[PLAY_COUNT];            			  
	/// 当前出牌者方向
	BYTE m_byCurOrientation;						
	/// 发牌动画的总时间，控制动画。
	int m_iSendCardTime;								
	///当前播放声音的id
	int  SoundbirdID;								
	/// 当前发牌位置
	WORD m_wCurSendPos; 							  
	/// 总共发送张数
	WORD m_wSendCount;  							    
	/// 各玩家已经发了的牌张数
	BYTE m_bySentCardCount[PLAY_COUNT];					
	/// 第一个发牌玩家
	BYTE m_byFirstSendPos;							
	/// 声音通道占用情况
	int  m_bPlayListHwnd[15];							
	/// 声音通道播放位置
	int  m_bPlayListPos;								

	HWND m_hMciWnd1;	
	HWND m_hMciWnd2;
	HWND m_hMciWnd3;
	HWND m_hMciWnd4;
	HWND m_hMciWnd5;
public:
	///下注数组
	CArray<XIA_ZHU, XIA_ZHU&>  m_ArrNote; 
	///下注数组
	CArray<XIA_ZHU, XIA_ZHU&>  m_ArrPlay;
	///线程退出
	bool m_bThreadExit;                  

public:
	///下注时间	
	int  m_iXiaZhuTime;		
    ///开牌时间
	int  m_iKaiPaiTime;      
	///空闲时间
	int  m_iFreeTime;		
	///发牌时间间隔
	int  m_iSendInterval;	
	/// 发牌时间
	int  m_iTimeSendCard;	
	///是否初始化完成
	bool m_bInitFinish;		
    /// 游戏金币类型 
	CString    m_MoneyType;
	/// 赢家是否红色字体显示
	bool       m_isWinRed;
	int        m_SpeCount;

public:
	// 游戏声效引擎
	ISoundEngine				* m_pSEBkMusic;

	// 播放游戏音乐
	void PlayPublicSound(int SoundID);

	
protected:
	///版本核对
	bool CheckVersion(BYTE byHighVersion,BYTE byLowVersion,TCHAR szUpDateHttp[100]);

	/// 通知更新游戏界面
	virtual bool OnNotifyUpdateClientView(void * pNetData, UINT uDataSize);

	///// 服务器发牌
	//virtual bool OnNotifySendCard(void * pNetData, UINT uDataSize);

	///// 发送一张牌
	//void OnNotifySendACard(void);

	///// 发牌结束
	//void OnNotifySendCardFinish(void);

	/// 玩家强退结束游戏
	virtual bool OnNotifyCutEnd(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

	/// 托管消息响应函数
	virtual void OnChangeTrustee(BYTE byDeskStation, bool bIsTrustee);

public:
	/// 清除所有定时器
	void KillAllTimer(void);
	
	/// 下注按钮函数
	LRESULT	OnXiaZhu(WPARAM wparam, LPARAM lparam);

	/// 上下庄申请按钮函数
	LRESULT	OnShangZhuang(WPARAM wparam, LPARAM lparam);

	/// 测试消息函数
	LRESULT	OnTest(WPARAM wparam, LPARAM lparam);

	/// 银行按钮函数
	LRESULT	OnBank(WPARAM wparam, LPARAM lparam);
	/// 超级玩家控制消息
	LRESULT	OnSuperCtr(WPARAM wparam, LPARAM lparam);
	
	///机器人控制改变消息
	LRESULT	OnSendAICtrl(WPARAM wparam, LPARAM lparam);

// 重载
public:
	/// 座位换算到视图位置
	virtual BYTE GetViewSeat(BYTE byDeskStation);

	///设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);

	///游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize/*, CTCPClientSocket * pClientSocket*/);

	///定时器消息
	virtual bool OnGameTimer(BYTE byDeskStation, UINT uTimeID, UINT uTimeCount);

	/// 同意开始游戏 
	virtual bool OnControlHitBegin();

	/// 安全结束游戏
	virtual bool OnControlSafeFinish();

	/// 改变旁观设置
	virtual void OnWatchSetChange(void);  // 平台发送的消息没有牌数据

	/// 改变旁观设置 游戏中发送牌数据，用于更新界面
	virtual void OnWatchSetChange(S_WATCH_SET_CHANGE &sWatchSetChange);

	/// 游戏设置
	virtual void OnGameSetting();

	///重新设置游戏数据
	virtual void ResetGameStation(int iGameStation);

	/// 用于玩家换位：比如逆时针、顺时针出牌
	virtual void OnBeforeUpdateViewSeat(void);
	/// 重载播放声音
	void BZSoundPlayGame(CWnd *pWnd, char *szFilename, DWORD dwPlayEnd, int nLoop=-1);

#ifdef VIDEO
	/// 为视频模块重载此函数
	virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);

	virtual bool AFCCloseFrame() ;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnMove(int x, int y);

	/// 系统开牌消息
	void OnHandleKaiPai(void * pNetData, UINT uDataSize);
	/// 玩家下注消息
	void OnHandleXiazhu(void * pNetData, UINT uDataSize);
	/// 系统结算消息
	void OnHandleJieSuan(void * pNetData, UINT uDataSize);

	///处理下注播放声音消息
	void OnHandleXiazhuPlaySound(void * pNetData, UINT uDataSize);
	/// 处理百家乐游戏开始消息
	void OnHandleBegin(void * pNetData, UINT uDataSize);
	/// 处理超级客户端控制消息
	void OnHandleSuperCtr(void * pNetData, UINT uDataSize);

	///玩家断线重连时，通过申请消息获取游戏数据
	void OnSetGameData(void * pNetData, UINT uDataSize);

	///播放声音
	void PlayOtherSound(TCHAR* path);

	///重载视图转换函数
	BYTE ViewStation(BYTE bDeskStation);

	///重载最大化事件
	void OnBnClickedRest();
	void OnBnClickedMax();

	///开始一张牌动画的函数
	void StartMoveCard(bool zhuang,int num, int zhen);

	void StartChao500Move(int type);//启动超过500万的动画

	///绘制超过500万的动画
	void StartChao500Move(CDC*pDC,int iWidth, int iHeight);

	void GetGameCode(char * szGameCode);

	void WriteInfo(TCHAR * pch);//add by wlr 20090806

	/// 播放下注声音
	BOOL PlayNoteSound(BYTE bSoundStyle);
	/// 播放下注声音
	BOOL PlayOutCardType(TCHAR FileName[],HWND hmciwnd);

	int m_iCountTimer;
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

};
///全局指针
extern CClientGameDlg * g_pCClientGameDlg;

#endif // !defined(_CLIENTGAMEDLG_H__INCLUDED_)