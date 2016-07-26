#pragma once
#ifndef GAMEPLACEDLG_HEAD_H
#define GAMEPLACEDLG_HEAD_H

#include "StdAfx.h"
#include "Resource.h"
#include "MainRoomEx.h"
#include "GameRoomEx.h"
//#include "LongonDialog.h"
#include "PlaceResource.h"

#include "DUITopWnd.h"

#include "AFCActwnd.h"						//大厅通知
#include "AfcTopWnd.h"						//顶部按钮
#include "chatting.h"
#include  "afcgamelistwnd.h"				//游戏例表窗口
#include "SkinMgr.h"

#define LEFT_WND_WIDTH				/*296*/225		//左边窗口大小
#define LEFT_WND_TOP				120		//顶部
#define QUIT_GAMEROOM               WM_USER + 101  ///< 金葫芦2代，修改密码后用户被踢消息

#define IDT_GP_SHOW_WINDOW			1		//ShowWindow的延时
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*******************************************************************************************************/

//关于对话框
class CAboutDlg : public CDialog
{
	//函数定义
public:
	//构造函数
	CAboutDlg();
	virtual BOOL OnInitDialog();
	void OnFeedBack();
	void OnServer();
	afx_msg void OnPaint();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);   
	CNormalBitmapButtonEx				m_Bt1;

	CGameImage	m_bk;

	DECLARE_MESSAGE_MAP()

private:
	CSkinMgr m_skinmgr;

	void Init();

};

/*******************************************************************************************************/

//主对话框
class CGamePlaceDlg : public CDialog
{
	CRgn							m_Rgn;								///< 绘制区域，不重绘子窗口部分
	POINT							m_SavedScreen;
public:
	bool							m_bMax;								//是否最大化
	bool							m_bShowMax;
	//变量定义
public:
//	CChattingWnd			m_ChattingWnd;								//房间顶部条
	CTopWndClass			m_topWnd;									//房间顶部条AFC版
	CTopDuWndClass			m_topDuWnd;									//房间顶部条DirectUI版
	CString					m_strTopWndRander;							//房间顶部条的渲染方式\

	CActWnd					m_ActInformWnd;								//大厅通知
//	CBottomWndClass			m_BottomWnd;								//房间顶部条
	CGameListWnd			m_GameListWnd;								//游戏例表窗口
	CNewGameClass			m_RoomTabWnd;								//房间 TAB 控件
	CMainRoomEx				* m_pMainRoom;								//大厅房间
	//////////////////////////////////////////////////////////////////////////
	//以下是和音乐播放有关的控制，Fred Huang 2008-07-24
	int								m_bgHallSoundID;
//	int								m_bgRoomSoundID;
	int								m_bgCurrentID;

	CNormalBitmapButtonEx	m_btSndPlay;
	CNormalBitmapButtonEx	m_btSndPause;
	CNormalBitmapButtonEx   m_btVolDown;
	CNormalBitmapButtonEx	m_btVolUp;
#ifndef DEV_NOFREE
	CNormalBitmapButtonEx   m_btTechSupport; //技术支持	add by lxl 2011-1-23
#endif

//	CToolTipCtrl            m_toolTip;

	int						m_sndBtnCx,m_sndBtnCy;

	int sndPlayX,sndPlayY;
	int sndPauseX,sndPauseY;
	int sndVolUpX,sndVolUpY;
	int sndVolDownX,sndVolDownY;
	int techSupportX, techSupportY;

	void OnSndPlay();
	void OnSndPause();
	void OnSndVolDown();
	void OnSndVolUp();
	void OnTechSupport();
	
	bool m_bSndPauseHall;
	int	 m_sndVolHall,m_sndVolRoom;
	//////////////////////////////////////////////////////////////////////////

	CGameImage						m_lc,m_rc,m_tc,m_bc;	//框架图片	
	CGameImage						m_lt,m_rt,m_lb,m_rb,m_rbext;	//边角	
public:
	//加载图片
	void LoadSkin(void);
	//HICON hIcon; 
	//函数定义
public:
	//构造函数
	CGamePlaceDlg();
	~CGamePlaceDlg();
	//初始化函数
	virtual BOOL OnInitDialog();
	//重画函数
	afx_msg void OnPaint();
	afx_msg void OnMove(int x,int y);
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL ShowWindow(int nCmdShow);

	// add xqm 2011-3-22
	//设置硬件加速
	void SetAcceleration(int iLevel);

	//保存玩家原来的硬件加速设置
	int		m_iLevel;
	DECLARE_MESSAGE_MAP()

	//消息映射
public:
	//热建函数
	LRESULT OnHotKey(WPARAM wparam, LPARAM lparam);	
	////建立通知函数
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//系统菜单
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//位置变化 
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//得到焦点
	afx_msg void OnSetFocus(CWnd * pOldWnd);

    // PengJiLin, 2011-6-23, 激活
    afx_msg void OnActivate(UINT uState, CWnd* pOldWnd, BOOL bMinisted);

	/// 
	/// 定时器响应消息
	/// @param[in]		UINT_PTR nIDEvent		定时器ID
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	CAboutDlg dlgAbout;
private:
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	///< 退出房间GameRoom时弹出对话框
	///< wParam 退出房间类型
	///< return LRESULT
	LRESULT OnQuitGameRoom(WPARAM wParam, LPARAM lParam);
public:
	void SetDisplays(void);
	//设置显示模式
	LRESULT  SetLeftHideOrShow(WPARAM wparam,LPARAM lparam);
	//设置显示模式
	LRESULT  SetRightHideOrShow(WPARAM wparam,LPARAM lparam);
	//打开银行
	LRESULT OpenBank(WPARAM wparam,LPARAM lparam);
	//打开好友 2013-02-03 wangzhitu
	LRESULT OpenIM(WPARAM wparam,LPARAM lparam);
	//打开银行
	LRESULT OpenIE(WPARAM wparam,LPARAM lparam);

	//音效设置
	LRESULT SetSound(WPARAM wparam,LPARAM lparam);

	///呼叫GM rongqiufen 2010-8-10
	LRESULT CallGM(WPARAM wparam,LPARAM lparam);
	LRESULT OnSetLogo(WPARAM wparam,LPARAM lparam);
    //投诉
	LRESULT OpenAppeal(WPARAM wparam,LPARAM lparam);
	LRESULT OnLockAccount(WPARAM wParam, LPARAM lParam);	//锁定机器 zxj 2009-11-12
	
	//版本核对
	BOOL CheckVersion();
	bool SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight);
	//移动窗口
	bool SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight);
	//播放音效
	void PlayBgSound(CWnd *pWnd,char *szFilename,DWORD dwPlayEnd,int nLoop=1);
	//停止音效
	void StopSound();
public:
	afx_msg void OnNcPaint();

	//wushuqun 2009.7.13
	//修改游戏房间中退出游戏时闪屏问题
	afx_msg BOOL OnEraseBkgnd(CDC* pDC)
	{
		return true;
		//
		return CWnd::OnEraseBkgnd(pDC);
	}
private:
	void EnCode_Key(CString &strdest, char *strsource, char key);

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
private:
	CSkinMgr m_skinmgr;
};
#endif
/*******************************************************************************************************/
