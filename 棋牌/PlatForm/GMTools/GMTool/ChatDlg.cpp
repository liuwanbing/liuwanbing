// ChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "ChatDlg.h"



// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatDlg::IDD, pParent)
	, m_SendInfo(_T(""))
	, m_ChatInof(_T(""))
{
	m_iUserID = -1;
	m_hBrush = CreateSolidBrush(RGB(255,255,255));
}

CChatDlg::~CChatDlg()
{
	DeleteObject(m_hBrush);
}

void CChatDlg::SetUserInfo(UserInfoStruct *userinfo, int iRoomID)
{
	//m_UserInfo = userinfo;
	m_iUserID = userinfo->dwUserID;
	m_szNickName = userinfo->nickName;
	m_szUserName = userinfo->szName;
	m_RoomID = iRoomID;
}

void CChatDlg::SetUserInfo(GM_OperatedPlayerInfo_t *userinfo)
{
	//Init();
	m_iUserID  = userinfo->iUserID;
	m_szNickName = userinfo->szNickName;
	m_szNickName = userinfo->szUserName;
	m_RoomID = userinfo->WID;
}


void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_SendInfo);
	DDV_MaxChars(pDX, m_SendInfo, 64);
	DDX_Text(pDX, IDC_EDIT2, m_ChatInof);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CChatDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CChatDlg 消息处理程序

BOOL CChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString s;
	s.Format("与%s私聊中...", m_szNickName);
	SetWindowText(s);
	m_ChatInof = "";
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CChatDlg::OnBnClickedOk()
{
	UpdateData();
	ZeroMemory(&m_Broadcast_t, sizeof(GM_Broadcast_t));
	m_Broadcast_t.iUserID = m_iUserID;

	if (m_SendInfo != "")
		{
		strcpy(m_Broadcast_t.szMsg, m_SendInfo.GetBuffer());

		AfxGetMainWnd()->PostMessage(MSG_CMD_BROADCAST, WPARAM(&m_Broadcast_t), 0);

		m_SendInfo = "管理员: " + m_SendInfo + "\r\n";
		m_ChatInof += m_SendInfo;
		m_SendInfo = "";
	}
	else
	{
		AfxMessageBox("发送内容不能为空!");
	}
	UpdateData(false);
	//OnOK();
}

void CChatDlg::AddMsg(CString msg, bool issystem)
{
	CString s;
	if (issystem)
		s.Format("系统消息: %s\r\n", msg); 
	else
		s.Format("%d: %s\r\n", m_iUserID, msg);
	m_ChatInof += s;

	UpdateData(false);
}

void CChatDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_iUserID = -1;
	CDialog::OnClose();
}

HBRUSH CChatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	int nID = pWnd->GetDlgCtrlID()   ;   
	if(nID == IDC_EDIT2)       //这里的IDC_Edit1是CEdit控件的ID
	{   
		pDC->SetBkColor(RGB(255, 255, 255));   
		pDC->SetTextColor(RGB(0,0,0)); 
		return m_hBrush;   
	}  
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
