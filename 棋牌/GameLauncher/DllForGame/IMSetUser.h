#pragma once


// CIMSetUser 对话框

class CIMSetUser : public CDialog
{
	DECLARE_DYNAMIC(CIMSetUser)

public:
	CIMSetUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIMSetUser();

	int		dwUser;
	CString szName;
	CString imWords;
// 对话框数据
	enum { IDD = IDD_IM_SETUSER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctlWords;
public:
	CStatic ctlTip;
protected:
	//	virtual void PreInitDialog();
public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
};
