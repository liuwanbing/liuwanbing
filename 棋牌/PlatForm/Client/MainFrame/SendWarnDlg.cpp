// SendWarnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendWarnDlg.h"
#include "GameRoomEx.h"

// CSendWarnDlg dialog

IMPLEMENT_DYNAMIC(CSendWarnDlg, CDialog)

CSendWarnDlg::CSendWarnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendWarnDlg::IDD, pParent)
{

}

CSendWarnDlg::~CSendWarnDlg()
{
}

void CSendWarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSendWarnDlg, CDialog)
END_MESSAGE_MAP()


// CSendWarnDlg message handlers

BOOL CSendWarnDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_NAME,m_name);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSendWarnDlg::OnOK()
{
	GetDlgItemText(IDC_WARNNING_MSG,m_warnmsg);
	if(m_warnmsg.GetLength()<=0)
	{
		return ;
	}
	if(m_warnmsg.GetLength()>=201)
	{
		SetDlgItemTextA(IDC_WARNNING_MSG,"您的发言超过了１００个字，请精简一下再发！");
		return;
	}
	CDialog::OnOK();
}
