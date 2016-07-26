#include "Stdafx.h"
#include "MainFrm.h"
#include "LogonDialog.h"
#include "AccessDialog.h"

BEGIN_MESSAGE_MAP(CLogonDialog, CDialog)
	ON_BN_CLICKED(IDC_SETTING, OnBnClickedSetting)
END_MESSAGE_MAP()

//构造函数
CLogonDialog::CLogonDialog(CWnd * pParent)	: CDialog(IDD_LOGON, pParent)
{
}

//析构函数
CLogonDialog::~CLogonDialog()
{
}

//初始化函数
BOOL CLogonDialog::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化控件
	GetDlgItem(IDC_NAME)->SetWindowText(g_Service.m_Info.m_szName);
	if (g_Service.m_Info.m_uLogonTimes>0) GetDlgItem(IDC_SETTING)->EnableWindow(FALSE);

	return TRUE;
}

//确定函数
void CLogonDialog::OnOK()
{
	//效验是否注册
	if (g_Service.m_Info.m_szServerGUID[0]==0)
	{
		AFCGetTrayIcon().ShowBalloonTip(TEXT("服务器未被授权使用，请按系统 [设置] 按钮，设置授权序列号码"),APP_TITLE);
		return;
	}
	if (g_Service.m_Info.m_szLogonAddr[0]==0)
	{
		AFCGetTrayIcon().ShowBalloonTip(TEXT("没有设置登陆服务器地址，请按系统 [设置] 按钮，设置登陆服务器地址"),APP_TITLE);
		return;
	}

	//获取数据
	GetDlgItem(IDC_NAME)->GetWindowText(m_strName);
	GetDlgItem(IDC_PASS)->GetWindowText(m_strPass);

	//效验数据
	if ((m_strName.GetLength()==0)||(m_strPass.GetLength()==0))
	{
		AFCGetTrayIcon().ShowBalloonTip(TEXT("请输入登陆名字和密码，不能包含非法字符或者系统保留字符"),APP_TITLE);
		GetDlgItem(IDC_NAME)->SetFocus();
		return;
	}

	__super::OnOK();
}

//授权设置
void CLogonDialog::OnBnClickedSetting()
{
	CAccessDialog Access;
	Access.SetServerInfo(g_Service.m_Info.m_szServerGUID,g_Service.m_Info.m_szLogonAddr);
	if (Access.DoModal()!=IDOK) return;
	Access.GetServerInfo(g_Service.m_Info.m_szServerGUID,g_Service.m_Info.m_szLogonAddr);

	return;
}

