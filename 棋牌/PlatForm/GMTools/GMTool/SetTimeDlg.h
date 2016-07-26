#pragma once
#include "afxwin.h"


// CSetTimeDlg 对话框

class CSetTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetTimeDlg)

public:
	CSetTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetTimeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_DayEdit;
public:
	CEdit m_HourEdit;
public:
	CEdit m_MinuteEdit;
public:
	CButton m_ForeverChk;
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedOk();
public:
	int m_Minute;

	void Init();
public:
	virtual INT_PTR DoModal();
public:
	CString m_szReason;
};
