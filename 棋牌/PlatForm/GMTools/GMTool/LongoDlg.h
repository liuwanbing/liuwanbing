#pragma once

//////////////////////////////////////////////////////////////////////////
///登录对话框，负责用户输入帐号密码以及登录Z服务器
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "afxwin.h"
#include "CommandMessage.h"


// CLongoDlg 对话框

class CLongoDlg : public CDialog
{
	DECLARE_DYNAMIC(CLongoDlg)

public:
	CLongoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLongoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox	m_UeserNameCmb;      ///用户名输入框
	CEdit		m_PassWordEdt;		 ///密码输入框
	CString		m_UeserName;		 ///用户名	
	CString		m_PassWord;			 ///密码	
	
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
protected:
	virtual void OnCancel();
public:
	CString m_name;
};
