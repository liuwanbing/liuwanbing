#pragma once

#ifndef CMAINFRAME_H_2003
#define CMAINFRAME_H_2003

#include "TrayIcon.h"
#include "ChildView.h"
//#include "LogonDialog.h"
#include "ServiceManage.h"
//#include "..\..\..\SDK\include\common\LicenseValidator.h"

//注册表定义
#define REG_LICENSE		TEXT("BZWServer\\WServer\\LicenseKey")
#define IDM_TRAY_NOTIFY_MSG				WM_USER+10			//任务栏提示消息

//获取任务栏图标
CTrayIcon & AFCGetTrayIcon();

//窗口框架类
class CMainFrame : public CFrameWnd , public IMessageShow
{
	//变量定义
public:
	CTrayIcon					m_TrayIcon;					//任务栏图标
	CChildView					m_wndView;					//视图变量
	LRESULT OnCreateTray(WPARAM wp, LPARAM lp);
	//内核变量
	//LicenseValidator CheckLicense;
public:

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CMainFrame)
	
	//函数定义
public:
	//构造函数
	CMainFrame();
	//析构函数
	virtual ~CMainFrame();
	//初始化建立参数
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//命令函数
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo);
	//显示信息
	virtual bool ShowMessage(const TCHAR * szMessage);

	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息函数
public:
	//任务栏图标消息
	LRESULT OnTrayIconNotify(WPARAM wID, LPARAM lEvent);
	//任务栏图标消息
	LRESULT OnMouseHover(WPARAM wID, LPARAM lEvent);
	//任务栏图标消息
	LRESULT OnMouseLeave(WPARAM wID, LPARAM lEvent);
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//焦点函数
	afx_msg void OnSetFocus(CWnd * pOldWnd);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//登陆系统
	afx_msg void OnLogonSystem();
	//获取大厅列表
	afx_msg void OnGetRoomList();

	bool m_bMouseTrack;
};


extern CServiceManage					g_Service;					//游戏服务

#endif