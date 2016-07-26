// OperateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "OperateDlg.h"


// COperateDlg 对话框

IMPLEMENT_DYNAMIC(COperateDlg, CDialog)

COperateDlg::COperateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperateDlg::IDD, pParent)
	, m_day(0)
	, m_hour(0)
	, m_min(30)
	, m_Reason(_T(""))
	, m_UserID(0)
	, m_UserNickName(_T(""))
	, m_UserLoginName(_T(""))
	, m_Forever(FALSE)
{
	
}

void COperateDlg::SetUserInfo(UserInfoStruct *userinfo, int iRoomID)
{	
	m_UserID = userinfo->dwUserID;
	m_UserNickName = userinfo->nickName;
	m_UserLoginName = userinfo->szName;
	m_RoomID = iRoomID;
}

void COperateDlg::SetUserInfo(GM_OperatedPlayerInfo_t *userinfo)
{
	m_UserID = userinfo->iUserID;
	m_UserNickName = userinfo->szNickName;
	m_UserLoginName = userinfo->szUserName;
	m_RoomID = userinfo->WID;
}



void COperateDlg::SetOperateType(int operatetype)
{
	m_OperateType = operatetype;
}

COperateDlg::~COperateDlg()
{
}

void COperateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_day);
	DDX_Text(pDX, IDC_EDIT2, m_hour);
	DDX_Text(pDX, IDC_EDIT3, m_min);
	DDX_Text(pDX, IDC_EDIT7, m_Reason);
	DDX_Text(pDX, IDC_EDIT4, m_UserID);
	DDX_Text(pDX, IDC_EDIT5, m_UserNickName);
	DDX_Text(pDX, IDC_EDIT6, m_UserLoginName);
	DDX_Check(pDX, IDC_CHECK1, m_Forever);
	DDX_Control(pDX, IDC_EDIT1, m_dayEdit);
	DDX_Control(pDX, IDC_EDIT2, m_hourEdit);
	DDX_Control(pDX, IDC_EDIT3, m_MinEdit);
	DDV_MaxChars(pDX, m_Reason, 64);
}


BEGIN_MESSAGE_MAP(COperateDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COperateDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT5, &COperateDlg::OnEnChangeEdit5)
END_MESSAGE_MAP()


// COperateDlg 消息处理程序

BOOL COperateDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::Create(lpszTemplateName, pParentWnd);
}

void COperateDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();
	if (m_Reason == "")
	{
		AfxMessageBox("操作原因不能为空!");
		return;
	}
	int x = m_day * 24 * 60 + m_hour * 60 + m_min;
	if (x < 0 || x > 0x00FFFFFF)
	{
		AfxMessageBox("时间超出限制!");
		return;
	}
	if (m_Forever) x = 0x00FFFFFF;

	ZeroMemory(&m_GMOperate, sizeof(m_GMOperate));
	m_GMOperate.iUserID = m_UserID;
	m_GMOperate.iWorkMinutes = x;
	memcpy(m_GMOperate.szReason, m_Reason.GetBuffer(), m_Reason.GetLength());
	memcpy(m_GMOperate.szNickName, m_UserNickName.GetBuffer(), m_UserNickName.GetLength());
	m_GMOperate.iOperation = m_OperateType;

	AfxGetMainWnd()->PostMessage(MSG_CMD_OPERATE, WPARAM(&m_GMOperate), LPARAM(&m_RoomID));

	CDialog::OnOK();
}

void COperateDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

//BOOL COperateDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	// TODO:  在此添加额外的初始化
//
//	::SetWindowLong(m_dayEdit.m_hWnd, GWL_STYLE, ::GetWindowLong(m_dayEdit.m_hWnd, GWL_STYLE) & ~ES_NUMBER); 
//	::SetWindowLong(m_hourEdit.m_hWnd, GWL_STYLE, ::GetWindowLong(m_hourEdit.m_hWnd, GWL_STYLE) & ~ES_NUMBER); 
//	::SetWindowLong(m_MinEdit.m_hWnd, GWL_STYLE, ::GetWindowLong(m_MinEdit.m_hWnd, GWL_STYLE) & ~ES_NUMBER); 
//	UpdateData();
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 异常: OCX 属性页应返回 FALSE
//}

BOOL COperateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	((CEdit*)GetDlgItem(IDC_EDIT7))->SetLimitText(64);
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(4);
	((CEdit*)GetDlgItem(IDC_EDIT2))->SetLimitText(4);
	((CEdit*)GetDlgItem(IDC_EDIT3))->SetLimitText(4);
	static CString caption[5] = {"", "奖励玩家", "禁言玩家", "踢出玩家", "封号玩家"};
	CString s;
	s.Format("%s %d", caption[m_OperateType], m_UserID);
	SetWindowText(s);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void COperateDlg::OnEnChangeEdit5()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
