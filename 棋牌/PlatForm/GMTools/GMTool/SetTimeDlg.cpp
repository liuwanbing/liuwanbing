// SetTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "SetTimeDlg.h"


// CSetTimeDlg 对话框

IMPLEMENT_DYNAMIC(CSetTimeDlg, CDialog)

CSetTimeDlg::CSetTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTimeDlg::IDD, pParent)
	, m_szReason(_T(""))
{
	
}

CSetTimeDlg::~CSetTimeDlg()
{
}

void CSetTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_DayEdit);
	DDX_Control(pDX, IDC_EDIT2, m_HourEdit);
	DDX_Control(pDX, IDC_EDIT3, m_MinuteEdit);
	DDX_Control(pDX, IDC_CHECK1, m_ForeverChk);
	DDX_Text(pDX, IDC_RICHEDIT21, m_szReason);
}


BEGIN_MESSAGE_MAP(CSetTimeDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSetTimeDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CSetTimeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetTimeDlg 消息处理程序

void CSetTimeDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	OnCancel();
}

void CSetTimeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	m_Minute = 0;
	CString s;
	m_DayEdit.GetWindowText(s);
	m_Minute += atoi(s.GetBuffer(s.GetLength())) * 24 * 60; 
	m_HourEdit.GetWindowText(s);
	m_Minute += atoi(s.GetBuffer(s.GetLength())) * 60;
	m_MinuteEdit.GetWindowText(s);
	m_Minute += atoi(s.GetBuffer(s.GetLength()));

	if (m_ForeverChk.GetCheck())
	{
		m_Minute = 0xFFFFFFFF;
	}

	OnOK();
}

void CSetTimeDlg::Init()
{
	m_DayEdit.SetWindowText("");
	m_HourEdit.SetWindowText("");
	m_MinuteEdit.SetWindowText("30");
	m_ForeverChk.SetCheck(false);
}
INT_PTR CSetTimeDlg::DoModal()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	INT_PTR r = CDialog::DoModal();
	//Init();
	return r;
}
