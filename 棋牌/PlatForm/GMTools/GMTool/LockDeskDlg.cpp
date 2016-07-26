// LockDeskDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "LockDeskDlg.h"


// CLockDeskDlg 对话框

IMPLEMENT_DYNAMIC(CLockDeskDlg, CDialog)

CLockDeskDlg::CLockDeskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLockDeskDlg::IDD, pParent)
	, m_Lower(0)
	, m_Upper(0)
	, m_Infostatic(_T(""))
{

}

CLockDeskDlg::~CLockDeskDlg()
{
}

void CLockDeskDlg::SetInfo(int itype, int imin, int imax)
{
	m_type = itype;
	m_Min = imin;
	m_Max = imax;
	m_Lower = imin;
	m_Upper = imax;
}

void CLockDeskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Lower);
	DDX_Text(pDX, IDC_EDIT2, m_Upper);
	DDX_Text(pDX, IDC_STATIC1, m_Infostatic);
}


BEGIN_MESSAGE_MAP(CLockDeskDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLockDeskDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLockDeskDlg 消息处理程序

void CLockDeskDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if (m_Lower < m_Min || m_Upper > m_Max)
	{
		AfxMessageBox("请输入正确的区间范围");
		return;
	}
	if (m_type == 1)
	{
		::PostMessage(m_hWnd, MSG_CMD_GM_ALLLOCKOK, WPARAM(&m_Lower), LPARAM(&m_Upper));
	}
	else
	{
		::PostMessage(m_hWnd, MSG_CMD_GM_ALLUNLOCKOK, WPARAM(&m_Lower), LPARAM(&m_Upper));
	}
	OnOK();
}

BOOL CLockDeskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_type == 1)
	{
		SetWindowText("批量封桌");
		m_Infostatic = "输入所需封桌的区间号:";
	}
	else 
	{
		SetWindowText("批量解封"); 
		m_Infostatic = "输入所需解封的区间号:";
	}

	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(3);	
	((CEdit*)GetDlgItem(IDC_EDIT2))->SetLimitText(3);	

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
