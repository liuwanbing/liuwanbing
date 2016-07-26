#pragma once
#include "Resource.h"

// CSuperSettingDlg 对话框

class CSuperSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSuperSettingDlg)

public:
	CSuperSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSuperSettingDlg();

// 对话框数据
	enum { IDD = IDD_SUPER_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
};
