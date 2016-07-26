// TCPDlg.h : 头文件
//

#pragma once
#include "../../../SDK/include/socket/TCPClientSocket.h"
#include "BClientSocket.h"
#include "ZClientSocket.h"
#include "WClientSocket.h"
#include "LongoDlg.h"
#include "GameManageDlg.h"



// CTCPDlg 对话框
class CTCPDlg : public CDialog
{
// 构造
public:
	CTCPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CBClientSocket		m_bSocket;				///B服务器客户端
	CZClientSocket		m_zSocket;				///Z服务器客户端

	CLongoDlg			m_LongoDlg;				///登录对话框
	CGameManageDlg		 m_GameManageDlg;		///大厅

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
public:
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(UINT lpszTemplateName, CWnd* pParentWnd = NULL);
public:
	virtual BOOL DestroyWindow();
};
