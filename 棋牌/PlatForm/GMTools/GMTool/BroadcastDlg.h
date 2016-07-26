#pragma once

//////////////////////////////////////////////////////////////////////////
///公告弹出对话框
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "GM_MessageDefine.h"
#include "CommandMessage.h"

// CBroadcastDlg 对话框

class CBroadcastDlg : public CDialog
{
	DECLARE_DYNAMIC(CBroadcastDlg)

public:
	CBroadcastDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBroadcastDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	GM_Broadcast_t	m_Broadcast_t;			///公告数据	
	CString m_Info;						    ///公告内容						

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedOk();
};
