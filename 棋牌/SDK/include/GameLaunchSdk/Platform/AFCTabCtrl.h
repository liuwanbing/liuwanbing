#pragma once

#ifndef CAFCTABCTRL_HEAD_FILE
#define CAFCTABCTRL_HEAD_FILE

#include "PublicDefine.h"
#include "AFCResource.h"
#include "AFCWindow.h"

//属性页风格
#define TS_UP						0							//上方
#define TS_DOWN						1							//下方
#define TS_LEFT						2							//左方
#define TS_RIGHT					3							//右方
#define WM_SHOWGAMEWINDOW			WM_USER+1					//发送给房间, 决定是否显示游戏
#define WM_LOADSKIN					WM_USER+2
#define TIMER_AD					1							//广告移动计时器
//最大属性页
#define MAX_PAGE					20							//最大属性页

#define IDM_SEND_ROOMMSG			WM_USER+5					//发送房间聊天消息
#define IDM_HIDE_RIGHT				WM_USER+10					//隐藏右边
#define IDM_SHOW_LEFT				WM_USER+15					//隐藏或显示左边
#define IDM_OPEN_BANK				WM_USER+19					//打开银行
#define IDM_OPEN_IE					WM_USER+20					//打开IE
#define IDM_MESSAGE_APPEAL          WM_USER+21                  //投诉消息
#define RIGHT_WND_WIDTH				230							//右侧窗口宽度

//页面类定义
class CNewGoGamePower//CNewGoGamePower
{
	friend class CNewGameClass;

	//信息变量
private:
	bool							m_bAutoDelete;				//自动删除
	CRect							m_rcTitleRect;				//标签位置

	//控件变量
private:
	HICON							m_hIcon;					//图标句柄
public:
	CWnd							* m_pWnd;					//窗口指针
	CWnd							*m_pRightWnd;				//右边窗口指针
	CString							m_strTitle;					//窗口文字

	CGameImage						m_n,m_a;

	COLORREF						m_cn,m_ca;

	//函数定义
private:
	//构造函数
	CNewGoGamePower();
	//析构函数
	~CNewGoGamePower();
	//取得宽度
	UINT GetAreaWidth(CDC * pDC);
	//绘画函数
	void Draw(CDC * pDC, UINT nViewStyle, bool bActive);
	//删除页面
	bool DeletePage();

	void ShowGameWindow(int nShow);
	
	void SetSkin(TCHAR  pathn[], TCHAR  patha[], COLORREF cn, COLORREF ca);
};

//属性页类
class EXT_CLASS CNewGameClass/*CNewGameClass*/ : public CWnd
{
	friend class CNewGoGamePower;
	
	bool m_bInit;
	//信息变量
protected:
	bool							m_bCopyRight;				//是否版权
	UINT							m_uActivePage;				//激活页面
	UINT							m_uViewStyte;				//显示属性
	INT_PTR							m_uIndex[MAX_PAGE];			//页面索引
	CPtrArray						m_PageArray;				//页面数组

	CGameImage						m_PicBottom;				//框架图片
	//版权处理
	CHyperLink						m_copyright;
	CRect							m_rect;
	HCURSOR							m_cursor;
	TCHAR							m_szURL[300];
	//skin
//	CGameImage						m_l,m_r,m_c,m_t;
	CGameImage						m_top;						//顶上高度
	int								m_x,m_y;
	TCHAR							m_pathn[MAX_PATH],m_patha[MAX_PATH];
	COLORREF						m_cn,m_ca,m_adcolor;
	CRect							m_adRect,m_addrawrect;//广告区域
	CSize							m_adsize;
	CString							m_ad;

	CGameImage						m_tabc,m_tabl,m_tabr;		//属性页背景
	CGameImage						m_tabn;						//属性页标签
	//函数定义
public:
	//构造函数
	CNewGameClass();
	//析构函数
	virtual ~CNewGameClass();

	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//建立函数
	bool CreateTabWnd(CWnd * pParentWnd, UINT uWndID);
	//加入窗口
	UINT AddTabPage(CWnd * pWnd,TCHAR * szTitle, HICON hIcon=NULL, bool bAutoDelete=true, bool bActive=true);
	//加入窗口
	UINT AddTabPage(CWnd * pWnd, CWnd *pRightWnd,TCHAR * szTitle, HICON hIcon=NULL, bool bAutoDelete=true, bool bActive=true);
	//删除页面
	bool RemoveTabPage(UINT nExtIndex);
	//显示版权信息
	bool ShowCopyRight(bool bShow=true);
	//激活窗口
	bool SetActivePageByExtIndex(UINT uExtIndex);
	
	void ShowAllWindow(int bShow);
	
	void SetViewStyle(UINT uStyle);
	//内部函数
private:
	//激活窗口
	bool SetActivePage(UINT uIndex);
	//调整大小
	bool FixTabPageSize();
	//绘画框架
	void DrawTabFrame(CDC * pDC);
	//获取页面
	CNewGoGamePower * GetPageItem(UINT nExtIndex);
	
	//消息函数 
public:
	//重画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//鼠标按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//得到焦点
	afx_msg void OnSetFocus(CWnd * pOldWnd);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//游戏TAB，鼠标双击消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
public:
	void LoadSkin(void);
public:
	void SetAd(TCHAR ad[]);
	//调整窗口位置
	void AutoSize(int cx,int cy);
	//大厅窗口大小
	void SetMainRoomWnd(CRect rect);
	//房间消息
	LRESULT OnSendRoomMsg(WPARAM wparam, LPARAM lparam);
	//房间消息
	LRESULT OnHideRight(WPARAM wparam, LPARAM lparam);
	//打开银行
	LRESULT OpenBank(WPARAM wparam, LPARAM lparam);
	//打开银行
	LRESULT OpenIE(WPARAM wparam, LPARAM lparam);
    //投诉
	LRESULT OAppeal(WPARAM wparam, LPARAM lparam);
};

#endif