#pragma once

//////////////////////////////////////////////////////////////////////////
///查找用户对话框
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "GM_MessageDefine.h"
#include "CommandMessage.h"
// CFindUserDlg 对话框

class CFindUserDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindUserDlg)

public:
	CFindUserDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindUserDlg();


	
// 对话框数据
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	GM_GetOperationByPlayer_t m_GMGetPlay;			///查找用户数据包
	CString m_UserNickName;							///输入的用户ID，昵称或者登录名

	DECLARE_MESSAGE_MAP()
public:
	
public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
};
