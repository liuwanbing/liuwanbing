#pragma once

//////////////////////////////////////////////////////////////////////////
///操作对话框供用户填写操作持续时间，原因及相关选项包括：禁止发言，踢出房间，禁止登录
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////


#include "comstruct.h"
#include "GM_MessageDefine.h"
#include "CommandMessage.h"
#include "afxwin.h"

// COperateDlg 对话框

class COperateDlg : public CDialog
{
	DECLARE_DYNAMIC(COperateDlg)

public:
	COperateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COperateDlg();

	void Init();
	void SetUserInfo(UserInfoStruct *userinfo, int iRoomID);
	void SetUserInfo(GM_OperatedPlayerInfo_t *userinfo);
	void SetOperateType(int operatetype);

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
public:
	int			m_day;			///天数
	int			m_hour;			///时数
	int			m_min;			///分数
public:
	CString		m_Reason;		///原因
public:
	UINT		m_UserID;		///用户ID
	CString		m_UserNickName;	///用户昵称
	CString		m_UserLoginName;///用户登录名

	int			m_OperateType;	///操作类型
	int			m_RoomID;		///房间号
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedOk();
public:
	BOOL m_Forever;						///是否永久
	GM_OperateByID_t	m_GMOperate;	///操作数据包
public:
	CEdit m_dayEdit;					///天数输入框
	CEdit m_hourEdit;					///时数输入框
	CEdit m_MinEdit;					///分数输入框
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeEdit5();
};
