#include "Stdafx.h"
#include "AFCLoader.h"
#include "MainFrm.h"
#include "BuildTime.h"
#include "RegKeyDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define VK_B								66						 //'B'key
#define VK_Z								90						 //'Z'key
#define VK_W								87						 //'W'key


IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(IDM_TRAY_NOTIFY_MSG,OnTrayIconNotify)
	ON_COMMAND(IDM_LOGON_SYSTEM, OnLogonSystem)
	ON_COMMAND(IDM_GET_ROOM_LIST, OnGetRoomList)
	ON_REGISTERED_MESSAGE(WM_CREATETRAYBAR, OnCreateTray)
END_MESSAGE_MAP()

CServiceManage					g_Service;					//游戏服务

//获取任务栏图标
CTrayIcon & AFCGetTrayIcon() { return ((CMainFrame *)AfxGetMainWnd())->m_TrayIcon; };

//构造函数
CMainFrame::CMainFrame() : m_TrayIcon(10)
{
	m_bMouseTrack = true;
}

//析构函数
CMainFrame::~CMainFrame()
{
}

//建立函数
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	long timeStamp=coreGetTimeStamp();

	//系统内部建立
	if (CFrameWnd::OnCreate(lpCreateStruct)==-1) return -1;
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),this,AFX_IDW_PANE_FIRST,NULL))	return -1;

	long licExpires=coreGetLicenseExpires();
	//建立任务栏图标
	HICON hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TrayIcon.SetNotifyWnd(this,IDM_TRAY_NOTIFY_MSG);
	m_TrayIcon.SetIcon(hIcon,APP_TITLE);
	SetIcon(hIcon,FALSE);
	DestroyIcon(hIcon);
	CenterWindow();
	///////注册码检测/////////
	//CString activationKey =  _bstr_t("");
	////注册码输入框，每次启动时，通过注册表读取，读取不到，弹出输入注册码的框，第一次读取，写入到注册表
	//
	//CString computerKey =  _bstr_t("");
	//CString strLicense = _bstr_t("");

	////获取注册码
	//CString strRegKey;
	//CRegKey LicenseKey;

	//strRegKey.Format(TEXT("%s"),REG_LICENSE);
	////打开注册表
	//LicenseKey.Open(HKEY_CURRENT_USER,strRegKey);
	//if (LicenseKey == NULL)
	//{
	//	//MessageBox("请输入注册码注册！");
	//	RegKeyDlg regdlg;
	//	//regdlg.Create(IDD_REGDLG);
	//	//regdlg.ShowWindow(SW_SHOW);
	//	regdlg.DoModal();
	//	
	//	//生成注册表字符		
	//	strLicense.Format(TEXT("%s"),regdlg.m_szRegCode);
	//	computerKey.Format(TEXT("%s"),regdlg.m_szRegCode);
	//	//strRegKey.Format(TEXT("%s\\%s"),REG_LICENSE,regdlg.m_szRegCode);
	//	LicenseKey.Create(HKEY_CURRENT_USER,strRegKey);
	//	LicenseKey.SetStringValue(TEXT("ComputerKey"),strLicense);
	//	
	//	//AfxMessageBox(strRegKey);
	//	
	//}
	//else
	//{
	//	
	//	TCHAR szPassBuffer[100];
	//	ULONG uBufferLength=sizeof(szPassBuffer);
	//	LONG uErrorCode=LicenseKey.QueryStringValue(TEXT("ComputerKey"),szPassBuffer,&uBufferLength);
	//	if (uErrorCode==ERROR_SUCCESS)
	//	{
	//		strRegKey.Format(TEXT("%s"),szPassBuffer);
	//		computerKey.Format(TEXT("%s"),szPassBuffer);
	//		//AfxMessageBox(strRegKey);
	//	}
	//	
	//}
	//LicenseKey.Close();
	////指定机器码，请使用原有的注册机制生成的,由文本框输入
	//CString computerID = coreGetCode();  
	//bool needsActivation = false;
	//CString errorMsg = _bstr_t(""); 
	////CString *errorMsg = _bstr_t("bzwgame");
	//LicenseValidator CheckLicense;

	//bool checked = false;
	//checked = CheckLicense.ValidateLicense(activationKey, computerKey, computerID, needsActivation, errorMsg);

	//if (!checked)
	//{
	//	AfxMessageBox("注册码错误，请联系开发商，获取正确注册码！");
	//	//MessageBox((LPCTSTR)errorMsg);
	//	DestroyWindow();
	//	return 0;
	//}


#ifdef BZ_LICENSE
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
		DestroyWindow();
		return 0;
	}
#endif // BZ_LICENSE

	//登陆系统
	g_Service.m_Info.LoadData();
	g_Service.SetMessageInterface(this);
	g_Service.Init();
	PostMessage(WM_COMMAND,IDM_LOGON_SYSTEM,0);

	CString s,scaption;
	GetWindowText(s);
	scaption.Format("%s v%d.%d.%d %s",s,VER_MAIN,VER_MIDDLE,VER_RESVERSE,VER_BUILDTIME);
	SetWindowText(scaption);	

#ifdef BZ_LICENSE
	if(timeStamp>licExpires)
	{
		DestroyWindow();
		return 0;
	}
#endif // BZ_LICENSE
	//coreInitialize(this,0);
	return 0;
}

//消息解释
BOOL CMainFrame::PreTranslateMessage(MSG * pMsg)
{
	if (this->GetSafeHwnd()==NULL)
	{
		return TRUE;
	}
	
	
	if (pMsg->message==WM_KEYDOWN)		
	{
		if((GetAsyncKeyState(VK_CONTROL) < 0)
		&& (GetAsyncKeyState(VK_B) < 0) 
		&& (GetAsyncKeyState(VK_Z) < 0) 
		&& (GetAsyncKeyState(VK_W) < 0))
		{
			CString CheckVersionHotKey = "深圳市红鸟网络科技有限公司";
			AfxMessageBox(CheckVersionHotKey);
		
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化建立参数
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs)) return FALSE;
	cs.style=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	cs.dwExStyle&=~WS_EX_CLIENTEDGE;
	cs.lpszClass=AfxRegisterWndClass(0);

	return TRUE;
}

//焦点函数
void CMainFrame::OnSetFocus(CWnd * pOldWnd)
{
	m_wndView.SetFocus();
}

//命令函数
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//任务栏图标消息
LRESULT CMainFrame::OnMouseHover(WPARAM wID, LPARAM lEvent)
{
	return 0;
}

//任务栏图标消息
LRESULT CMainFrame::OnMouseLeave(WPARAM wID, LPARAM lEvent)
{
	m_TrayIcon.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), g_Service.GetRoomName());
	m_bMouseTrack=true;
	return 0;
}

//任务栏图标消息
LRESULT CMainFrame::OnTrayIconNotify(WPARAM wID, LPARAM lEvent)
{
	switch (lEvent)
	{
	case WM_MOUSEMOVE:
		{
			if (m_bMouseTrack)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme); //结构体缓冲区大小
				tme.dwFlags = TME_LEAVE|TME_HOVER; //注册WM_MOUSEHOVER消息
				tme.dwHoverTime = 10; //WM_MOUSEHOVER消息触发间隔时间
				tme.hwndTrack = GetSafeHwnd(); //当前窗口句柄

				::TrackMouseEvent(&tme); //注册发送消息

				m_bMouseTrack = false;
			}
			return 1;
		}
	case WM_LBUTTONDBLCLK:	//双击事件
		{
			////if (g_Service.m_Info.IsLogonSystem())
			{
				ShowWindow(SW_SHOWMAXIMIZED);
				SetForegroundWindow();
			}
			return 1;
		}
	case WM_RBUTTONUP:		//右键信息
		{
			CMenu Menu;
			Menu.LoadMenu(IDR_MAINFRAME);
			CMenu * pSubMenu=Menu.GetSubMenu(0);
			CPoint MousePoint;
			::GetCursorPos(&MousePoint);	
			SetForegroundWindow();

			//控制菜单
			if (g_Service.m_Info.IsLockSystem()==false)	pSubMenu->EnableMenuItem(IDM_LOCK_SYSTEM,MF_BYCOMMAND|MF_GRAYED);
			if (g_Service.m_Info.m_uSystemState!=STS_NO_LOGON) pSubMenu->EnableMenuItem(IDM_LOGON_SYSTEM,MF_BYCOMMAND|MF_GRAYED);
			::TrackPopupMenu(pSubMenu->GetSafeHmenu(),0,MousePoint.x,MousePoint.y,0,GetSafeHwnd(),NULL);

			return 1;
		}
	}
	return 0;
}

//位置消息
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if (nType==SIZE_MINIMIZED)	ShowWindow(SW_HIDE);
}

//登陆系统
void CMainFrame::OnLogonSystem()
{
	g_Service.LogonSystem("pmb","123");//LogonDialog.m_strName,LogonDialog.m_strPass);
	/*static CLogonDialog	LogonDialog;
	if (LogonDialog.GetSafeHwnd()==NULL)	//建立登录窗口
	{
		if (LogonDialog.DoModal()==IDOK)	//登陆系统
		{
			g_Service.LogonSystem(LogonDialog.m_strName,LogonDialog.m_strPass);
		}
		else if (g_Service.m_Info.GetLogonTimes()==0)	//退出系统
		{
			PostMessage(WM_COMMAND,ID_APP_EXIT,0);
		}
	}
	else	//已经存在登录窗口
	{
		//ShowWindow(SW_HIDE);
		LogonDialog.ShowWindow(SW_SHOW);	
		LogonDialog.SetForegroundWindow();
	}
	return;*/
}

//显示信息
bool CMainFrame::ShowMessage(const TCHAR * szMessage)
{
	m_TrayIcon.ShowBalloonTip(szMessage,APP_TITLE);
	return true;
}

//获取大厅列表
void CMainFrame::OnGetRoomList()
{
	//获取注册码，通过注册表读取，若读取不到，则弹出输入注册码框，输入并写入到注册表
	//CString strRegKey;
	//CString strLicense;
	//CRegKey LicenseKey;

	//strRegKey.Format(TEXT("%s"),REG_LICENSE);
	////打开注册表
	//LicenseKey.Open(HKEY_CURRENT_USER,strRegKey);
	//if (LicenseKey == NULL)
	//{
	//	//MessageBox("请输入注册码注册！");
	//	RegKeyDlg regdlg;
	//	
	//	regdlg.DoModal();
	//	
	//	//生成注册表字符		
	//	strLicense.Format(TEXT("%s"),regdlg.m_szRegCode);

	//	if(strLicense.GetLength() != 39)
	//	{
	//		AfxMessageBox("注册码格式错误，请获取正确注册码！");
	//
	//		DestroyWindow();
	//		return;
	//	}
	//	
	//	LicenseKey.Create(HKEY_CURRENT_USER,strRegKey);
	//	LicenseKey.SetStringValue(TEXT("ComputerKey"),strLicense);
	//	
	//	//AfxMessageBox(strLicense);
	//	
	//}
	//else
	//{
	//	//LicenseKey.DeleteSubKey(LPCTSTR lpszSubKey);
	//	///////注册码检测/////////
	//	CString activationKey =  _bstr_t("");	
	//	CString computerKey =  _bstr_t("");

	//	////获取注册码
	//
	// 	TCHAR szBuffer[100];
	// 	ULONG uBufferLength=sizeof(szBuffer);
	// 	LONG uErrorCode=LicenseKey.QueryStringValue(TEXT("ComputerKey"),szBuffer,&uBufferLength);
	// 	if (uErrorCode==ERROR_SUCCESS)
	//	{
	//		computerKey.Format(TEXT("%s"),szBuffer);
	//		//AfxMessageBox(strRegKey);
	//	}
	//	
	//	//LicenseKey.Close();
	//	////指定机器码，请使用原有的注册机制生成的
	//	CString computerID = coreGetCode();  
	//	bool needsActivation = false;
	//	CString errorMsg = _bstr_t(""); 
	//	//CString *errorMsg = _bstr_t("bzwgame");
	//	LicenseValidator CheckLicense;

	//	bool checked = false;
	//	checked = CheckLicense.ValidateLicense(activationKey, computerKey, computerID, needsActivation, errorMsg);

	//	if (!checked)
	//	{
	//		//AfxMessageBox("注册码错误！！！");
	//		//MessageBox((LPCTSTR)errorMsg);
	//		//	DestroyWindow();
	//		//	return 0;
	//		LicenseKey.DeleteValue(TEXT("ComputerKey"));

	//		RegKeyDlg regdlg;
	//	
	//		regdlg.DoModal();
	//	
	//		//生成注册表字符		
	//		strLicense.Format(TEXT("%s"),regdlg.m_szRegCode);

	//		if(strLicense.GetLength() != 39)
	//		{
	//			AfxMessageBox("注册码格式错误，请获取正确注册码！");
	//
	//			DestroyWindow();
	//			return;
	//		}		
	//	
	//		LicenseKey.SetStringValue(TEXT("ComputerKey"),strLicense);			
	//	}
	//}

	//LicenseKey.Close();

	//更新信息
	g_Service.UpdateComDLLList(&m_wndView.m_ServiceControl.m_InstallList);
	g_Service.UpdateServiceInfo(&m_wndView.m_ServiceControl.m_StartList);

	//显示窗口
	//ShowWindow(SW_SHOWMINIMIZED);//SW_SHOWMINIMIZED);//SW_SHOWMAXIMIZED);
	//SetForegroundWindow();
	//AfxMessageBox("Get room list!");

	return;
}

LRESULT CMainFrame::OnCreateTray(WPARAM wp, LPARAM lp)
{
#define APP_TITLEe "GServer"
	m_TrayIcon.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),APP_TITLE,TRUE);
	return 0;
}