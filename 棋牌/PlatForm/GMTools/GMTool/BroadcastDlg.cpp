// BroadcastDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "BroadcastDlg.h"



// CBroadcastDlg 对话框

IMPLEMENT_DYNAMIC(CBroadcastDlg, CDialog)

CBroadcastDlg::CBroadcastDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBroadcastDlg::IDD, pParent)
	, m_Info(_T(""))
{

}

CBroadcastDlg::~CBroadcastDlg()
{
}

void CBroadcastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Info);
	DDV_MaxChars(pDX, m_Info, 255);
}


BEGIN_MESSAGE_MAP(CBroadcastDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CBroadcastDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CBroadcastDlg 消息处理程序

BOOL CBroadcastDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CBroadcastDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData();
	if (m_Info != "")
	{
		m_Broadcast_t.iUserID = 0;
		strcpy(m_Broadcast_t.szMsg, m_Info.GetBuffer());
		AfxGetMainWnd()->PostMessage(MSG_CMD_BROADCAST, WPARAM(&m_Broadcast_t), 0);
		m_Info = "";
		UpdateData(false);
		CDialog::OnOK();
	}
	else 
	{
		AfxMessageBox("公告内容不能为空！");
	}
}

void CBroadcastDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
