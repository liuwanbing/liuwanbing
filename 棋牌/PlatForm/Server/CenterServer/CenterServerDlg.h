// CenterServerDlg.h : 头文件
//

#pragma once

#include "SysInfoList.h"
#include "afxcmn.h"

#include "afxwin.h"

#include "CenterServerManage.h"
#include "TrayIcon.h"


// CCenterServerDlg 对话框
class CCenterServerDlg : public CDialog
{
	void CheckBanBen();
	CTrayIcon							m_TrayIcon;						//任务栏图标///
	CCenterServerModule					m_CenterServerModule;
	void SetStatusBar();
// 构造
public:
    void CCenterServerDlg::OutputFun(CString message);
	CCenterServerDlg(CWnd* pParent = NULL);	// 标准构造函数
	LRESULT OnCreateTray(WPARAM wp, LPARAM lp);
// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CSysInfoList m_listSysRunInfo;
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	CStatic m_strStatusBar;
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);////
	LRESULT OnTrayIconNotify(WPARAM wID, LPARAM lEvent);////
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);///
public:
	//CDlgSetup setup;
};
