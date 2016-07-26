#include "StdAfx.h"
#include "AFCLoader.h"
#include "AccessDialog.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CAccessDialog, CDialog)
END_MESSAGE_MAP()

//构造函数
CAccessDialog::CAccessDialog() : CDialog(IDD_ACCESS_SET)
{
	memset(m_szServerIP,0,sizeof(m_szServerIP));
	memset(m_szServerGUID,0,sizeof(m_szServerGUID));
}

//析构函数
CAccessDialog::~CAccessDialog()
{
}

//初始化函数
BOOL CAccessDialog::OnInitDialog()
{
	__super::OnInitDialog();
	
	//设置控件
	((CEdit *)GetDlgItem(IDC_ACCESS_NUM))->LimitText(36);
	((CEdit *)GetDlgItem(IDC_LOGIN_ADDR))->LimitText(50);
	GetDlgItem(IDC_LOGIN_ADDR)->SetWindowText(m_szServerIP);
	GetDlgItem(IDC_ACCESS_NUM)->SetWindowText(m_szServerGUID);

	return TRUE;
}

//确定函数
void CAccessDialog::OnOK()
{
	//获取数据
	GetDlgItem(IDC_LOGIN_ADDR)->GetWindowText(m_szServerIP,sizeof(m_szServerIP));
	GetDlgItem(IDC_ACCESS_NUM)->GetWindowText(m_szServerGUID,sizeof(m_szServerGUID));

	//效验数据
	if ((lstrlen(m_szServerGUID)==0)||(lstrlen(m_szServerIP)==0))///if ((lstrlen(m_szServerGUID)!=36)||(lstrlen(m_szServerIP)==0))
	{
		AFCGetTrayIcon().ShowBalloonTip(TEXT("请完整的输入授权序列号和登陆服务器地址，按 [取消] 按钮取消设置"),APP_TITLE);
		GetDlgItem(IDC_ACCESS_NUM)->SetFocus();
		return;
	}

	__super::OnOK();
}

//设置信息
bool CAccessDialog::SetServerInfo(TCHAR * szServerGUID, TCHAR * szServerIP)
{
	//初始化数据
	memset(m_szServerIP,0,sizeof(m_szServerIP));
	memset(m_szServerGUID,0,sizeof(m_szServerGUID));

	//设置数据
	if ((szServerIP!=NULL)&&(lstrlen(szServerIP)<=50)) lstrcpy(m_szServerIP,szServerIP);
	if ((szServerGUID!=NULL)&&(lstrlen(szServerGUID)<=36)) lstrcpy(m_szServerGUID,szServerGUID);

	//显示数据
	if (GetSafeHwnd()!=NULL)
	{
		GetDlgItem(IDC_LOGIN_ADDR)->SetWindowText(m_szServerIP);
		GetDlgItem(IDC_ACCESS_NUM)->SetWindowText(m_szServerGUID);
	}

	return true;
}

//获取信息
bool CAccessDialog::GetServerInfo(TCHAR * szServerGUID, TCHAR * szServerIP)
{
	//写入数据
	if ((szServerIP!=NULL)&&(lstrlen(szServerIP)<=50)) lstrcpy(szServerIP,m_szServerIP);
	if ((szServerGUID!=NULL)&&(lstrlen(szServerGUID)<=36)) lstrcpy(szServerGUID,m_szServerGUID);

	return true;
}
