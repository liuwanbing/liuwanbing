// CenterServerDlg.h : 头文件
//

#pragma once

#include "SysInfoList.h"
#include "GameLogonManage.h"

#include "TrayIcon.h"

// CCenterServerDlg 对话框
class CMainServerDlg : public CDialog
{
	CTrayIcon							m_TrayIcon;						//任务栏图标///
	CGameLogonModule					m_GameLogon;
// 构造
public:
    void CMainServerDlg::OutputFun(const CString & message);
	CMainServerDlg(CWnd* pParent = NULL);	// 标准构造函数
	LRESULT OnCreateTray(WPARAM wp, LPARAM lp);
// 对话框数据
	//enum { IDD = IDD_MAINSERVER_DIALOG};

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
	afx_msg void OnBnClickedOk();

	afx_msg void OnSize(UINT nType, int cx, int cy);////
	LRESULT OnTrayIconNotify(WPARAM wID, LPARAM lEvent);////
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);///

public:
	afx_msg void OnBnClickedBtnnews();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
