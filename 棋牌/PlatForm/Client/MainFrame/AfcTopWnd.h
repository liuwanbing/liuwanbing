#ifndef CAFCWNDTOP_HEAD_FILE
#define CAFCWNDTOP_HEAD_FILE

#include "SkinMgr.h"
#include "QuitAsk.h"
#include <vector>
#include "gif\WndAnimate.h"
using namespace std;

#define WM_LOADSKIN					WM_USER+2
#define IDM_SKIN					WM_USER+3
#define ID_REST_MAX					WM_USER+4
//#define IDM_OPEN_BANK				WM_USER+4

class CGamePlaceDlg;

class  CTopWndClass:public CWnd
{
	bool							bInitialed;

	//信息变量
protected:
	CNormalBitmapButtonEx					m_Bt1;					//
	CNormalBitmapButtonEx					m_Bt2;					//
	CNormalBitmapButtonEx					m_Bt3;					//
	CNormalBitmapButtonEx					m_Bt4;					//
	CNormalBitmapButtonEx					m_Bt5;					//
	CNormalBitmapButtonEx					m_Bt6;					
	CNormalBitmapButtonEx					m_Bt7;
	CNormalBitmapButtonEx					m_Bt8;					//
	CNormalBitmapButtonEx					m_Bt9;					
	CNormalBitmapButtonEx					m_Bt10;

	CNormalBitmapButtonEx					m_Bt11;

	CGamePlaceDlg*							m_pPlasceDlg;


	//wushuqun 2009.7.15
	CGameImage  m_imageBt1,m_imageBt2,m_imageBt3,m_imageBt4,m_imageBt5,m_imageBt6,m_imageBt7,
		        m_imageBt8,m_imageBt9,m_imageBt10_1, m_imageBt10_2,m_imageBt11, m_imageRight;

	CNormalBitmapButtonEx					m_BtClose;					//换肤
	CNormalBitmapButtonEx					m_BtMin;					//最大化

	CNormalBitmapButtonEx					m_BtSkin;					//换肤

	CGameImage								m_ltlogo;					//左边游戏logo
	CIEClass								m_WebView;					//网页视图
	CGameImage								m_top,m_bottom,m_left,m_right;		//顶部边框
	CGameImage								m_tl,m_tr,m_bl,m_br;		//顶部边角
	

	CGameImage								m_topc,m_topl,m_topr;	//顶部背景
	int										m_btnx,m_btny;				//顶部按钮开始位置
	int										m_topskx,m_topsky;			//顶部换肤按钮位置
	int										m_topminx,m_topminy;		//顶部最小化按钮位置
	int										m_topclx,m_topcly;			//顶部关闭按钮位置
	int										m_toprestx,m_topresty;		//顶部最大化，还原按钮位置
	int										m_topImgRightXoffset;		//顶部一排按钮右边的边界处那张图的X偏移
	
	//各按钮之间的空间
	int										m_iRightBlank;				//大厅顶部按钮右侧留白部分宽度
	int										m_iSpaceWidth;				//按钮空隙
	int										m_iAdX,m_iAdY,m_iAdWidth,m_iAdHeight;	//广告条的x,y
	int										m_iAdBtnX,m_iAdBtnY;			//第一个按钮起始位置
	int										m_ilogX,m_ilogY;				//logo位置
	int										m_iLockX, m_iLockY;			///< 锁机按钮位置 zxd20091218
	BOOL									m_bLockAccount;				//表示当前帐号是否锁定机器 zxj 2009-11-13
	TCHAR									m_szLockPath[MAX_PATH];		//锁定机器图片目录 zxj 2009-11-13
	TCHAR									m_szUnlockPath[MAX_PATH];	//解除锁定图片目录 zxj 2009-11-13
	BOOL									m_bLogon;					//是否成功登录了Z服务器 zxj 2009-11-13
	//函数定义
public:
	CToolTipCtrl							m_ToolTip;						//大厅按钮提示
	//构造函数
	CTopWndClass();
	//析构函数
	virtual ~CTopWndClass();
	CNormalBitmapButtonEx					m_BtMax;					//最大化	
	CNormalBitmapButtonEx					m_BtRestore;				//还原
	CWndAnimate                             m_toplogo;
	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//建立函数
	bool CreateWnd(CWnd * pParentWnd, UINT uWndID);
	//显示版权信息
	//bool ShowCopyRight(bool bShow=true);
	//内部函数
public:
	//调整大小
	bool FixControlStation();

	//锁定机器命令主ID zxj 2009-11-12
	void SetLockType(int nLockType);

	//锁定机器命令主ID zxj 2009-11-12
	void SetLockWndState(BOOL bType);
private:
	//绘画框架
	void DrawFrame(CDC * pDC);
	//消息函数 
public:
	//重画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
	LRESULT OnExchangeSkin(WPARAM wparam, LPARAM lparam);
	
public:
	void LoadSkin(void);

private:
	//绘制框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);

protected:
	void OnHitMin();
	void OnHitMax();
	void OnHitRestore();
	void OnHitSkin();
	void OnHitClose();
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	//调整窗口位置
	//void AutoSize(int cx,int cy);
	//功能按钮
	void OnBnClickedBt1();

	void OnBnClickedBt2();

	void OnBnClickedBt3();

	void OnBnClickedBt4();

	void OnBnClickedBt5();

	void OnBnClickedBt6();

	void OnBnClickedBt7();

	void OnBnClickedBt8();

	void OnBnClickedBt9();

	void OnBnClickedBt10();
	//得到通知已经收到flash地址
	void SetFlashAd();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/////////////////////////////////
	///Kylin 增加银行，呼叫网管只在游戏房间显示功能
	bool m_bHide;
	/////////////////////////////////

private:
	//加载的皮肤
	CSkinMgr m_skinmgr;

public:
	CQuitAsk m_quitAskdlg;
};

#endif