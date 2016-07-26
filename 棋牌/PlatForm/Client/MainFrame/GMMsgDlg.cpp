#include "StdAfx.h"
#include "GMMsgDlg.h"

CGMMsgDlg::CGMMsgDlg(CString strInfo, int sec):
ConfirmDlg(strInfo)
{
	m_sec = sec;
}

CGMMsgDlg::~CGMMsgDlg(void)
{
}

BOOL CGMMsgDlg::OnInitDialog()
{
	ConfirmDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	if (m_sec != 0)
		SetTimer(10000, m_sec * 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BEGIN_MESSAGE_MAP(CGMMsgDlg, ConfirmDlg)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CGMMsgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 10000) OnOK();
	ConfirmDlg::OnTimer(nIDEvent);
}

void CGMMsgDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	ConfirmDlg::OnOK();
}
