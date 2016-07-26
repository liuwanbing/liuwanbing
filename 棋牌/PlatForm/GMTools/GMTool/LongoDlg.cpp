// LongoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "LongoDlg.h"

#include "cfgEngine.h"
#include "wCore2.h"


// CLongoDlg 对话框

IMPLEMENT_DYNAMIC(CLongoDlg, CDialog)

CLongoDlg::CLongoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLongoDlg::IDD, pParent)
	, m_name(_T(""))
{

}

CLongoDlg::~CLongoDlg()
{
}

void CLongoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_UeserNameCmb);
	DDX_Control(pDX, IDC_EDIT1, m_PassWordEdt);
	DDX_CBString(pDX, IDC_COMBO1, m_name);
	DDV_MaxChars(pDX, m_name, 16);
}


BEGIN_MESSAGE_MAP(CLongoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLongoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLongoDlg 消息处理程序

BOOL CLongoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//授权检测
/*	long timeStamp=coreGetTimeStamp();

	long licExpires=coreGetLicenseExpires();
*/
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW); 
	CenterWindow();
	m_UeserNameCmb.AddString("hn781001");
	m_UeserNameCmb.AddString("hn781002");
	m_UeserNameCmb.AddString("hn781003");
	m_UeserNameCmb.SetCurSel(0);

	m_PassWordEdt.SetWindowText("888888");
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(16);
//	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetLimitText(16);

	// TODO:  在此添加额外的初始化
/*
	if(timeStamp>licExpires)
	{
		CString s,code=coreGetCode();
		s.Format("您的服务器未注册或已过期，请与服务商联系。\n\n请将以下机器码发送给服务商，获取注册码文件：\n\n%s\n\n",code);
		MessageBox(s,"提示",MB_ICONERROR);
		s="机器码已复制到您的剪贴板中，直接Ctrl+V粘贴机器码！";
		MessageBox(s,"提示",MB_ICONINFORMATION);
		OpenClipboard();
		EmptyClipboard();
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,code.GetLength()+5); 
		if (hData==NULL)  
		{ 
			CloseClipboard(); 
			return TRUE; 
		}
		LPTSTR szMemName=(LPTSTR)GlobalLock(hData); 
		lstrcpy(szMemName,code); 
		SetClipboardData(CF_TEXT,hData); 
		GlobalUnlock(hData);  
		GlobalFree(hData); 
		CloseClipboard(); 
		PostQuitMessage(0);
		DestroyWindow();
		return FALSE;
	}
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLongoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_UeserNameCmb.GetWindowText(m_UeserName);
	m_PassWordEdt.GetWindowText(m_PassWord);
	AfxGetMainWnd()->PostMessage(MSG_CMD_LONGIN, WPARAM(&m_UeserName), LPARAM(&m_PassWord));
	OnOK();
}

void CLongoDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	PostQuitMessage(0);
	CDialog::OnCancel();
	
}
