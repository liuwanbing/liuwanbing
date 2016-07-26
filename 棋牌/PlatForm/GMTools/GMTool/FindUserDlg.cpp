// FindUserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "FindUserDlg.h"


// CFindUserDlg 对话框

IMPLEMENT_DYNAMIC(CFindUserDlg, CDialog)

CFindUserDlg::CFindUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindUserDlg::IDD, pParent)
	, m_UserNickName(_T(""))
{

}

CFindUserDlg::~CFindUserDlg()
{
}

void CFindUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_UserNickName);
}


BEGIN_MESSAGE_MAP(CFindUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFindUserDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFindUserDlg 消息处理程序

void CFindUserDlg::OnBnClickedOk()
{

	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	ZeroMemory(&m_GMGetPlay, sizeof(GM_GetOperationByPlayer_t));


	if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == true)
	{
		m_GMGetPlay.bType = 0;
		m_GMGetPlay.playerInfo.UserID = atoi(m_UserNickName);
		for (int i = 0; i < m_UserNickName.GetLength(); i++)
		{
			if (m_UserNickName[i] > '9' || m_UserNickName[i] < '0') 
				m_GMGetPlay.playerInfo.UserID = 0;
		}
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck() == true)
	{
		m_GMGetPlay.bType = 2;
		memcpy(m_GMGetPlay.playerInfo.szName, m_UserNickName.GetBuffer(), m_UserNickName.GetLength());
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck() == true)
	{
		m_GMGetPlay.bType = 1;
		memcpy(m_GMGetPlay.playerInfo.szName, m_UserNickName.GetBuffer(), m_UserNickName.GetLength());
	}
	
	//memcpy(m_GMGetPlay.playerInfo.szName,  m_UserNickName.GetBuffer(), m_UserNickName.GetLength());
	AfxGetMainWnd()->PostMessage(MSG_CMD_GM_FINDUSER, WPARAM(&m_GMGetPlay), 0);
	m_UserNickName = "";
	UpdateData(FALSE);
	OnOK();
}

BOOL CFindUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO1))-> SetCheck(TRUE); 
	// TODO:  在此添加额外的初始化
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(32);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
