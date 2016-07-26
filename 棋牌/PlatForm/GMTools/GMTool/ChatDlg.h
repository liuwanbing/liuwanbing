#pragma once

//////////////////////////////////////////////////////////////////////////
///私聊弹出对话框
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////


#include "afxcmn.h"
#include "GM_MessageDefine.h"
#include "CommandMessage.h"
#include "comstruct.h"

// CChatDlg 对话框

class CChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatDlg();

	void SetUserInfo(UserInfoStruct *userinfo, int iRoomID);
	void SetUserInfo(GM_OperatedPlayerInfo_t *userinfo);
	void AddMsg(CString msg, bool issystem = false);

// 对话框数据
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持



	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
public:
	GM_Broadcast_t	m_Broadcast_t;				///私聊数据包	
	CString m_SendInfo;							///要发送的私聊内容	
	int m_iUserID;								///用户ID
	CString m_szNickName;						///用户昵称
	CString m_szUserName;						///用户登录名		
	int m_RoomID;								///房间ID	
	HBRUSH m_hBrush;							///只读EDIT的背景刷		
public:
	afx_msg void OnClose();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	CString m_ChatInof;
};
