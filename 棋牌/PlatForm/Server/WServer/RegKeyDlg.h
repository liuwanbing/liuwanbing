#pragma once
#include "afxwin.h"


// RegKeyDlg 对话框

class RegKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(RegKeyDlg)

public:
	RegKeyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RegKeyDlg();

// 对话框数据
	enum { IDD = IDD_REGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_RegCode;
	TCHAR m_szRegCode[40];
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();
};
