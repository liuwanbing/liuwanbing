#include "Stdafx.h"
#include "resource.h"
#include "quitask.h"
#include "MainRoomEx.h"


BEGIN_MESSAGE_MAP(CQuitAsk, CLoadFileBmp)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK,OnClicked1)
	ON_BN_CLICKED(IDCANCEL,OnClicked2)
	ON_BN_CLICKED(IDC_CHANGEID,OnClicked3)
	ON_BN_CLICKED(IDC_LOCKQUIT,OnClicked4)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


//构造函数
CQuitAsk::CQuitAsk() : CLoadFileBmp(IDD_QUIT_ASK)
{

}

//析构函数
CQuitAsk::~CQuitAsk()
{
}

BOOL CQuitAsk::OnInitDialog()
{
	CLoadFileBmp::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%stop\\\\quitask.bmp",skinfolder);
	m_bk.SetLoadInfo(path,TRUE);
	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight(),0);
	CGameImageHelper help(&m_bk);
	HRGN hRgn;
	hRgn = AFCBmpToRgn(help,m_bk.GetPixel(0,0),RGB(1,1,1));
	if(hRgn)
		SetWindowRgn(hRgn,TRUE);

	CGameImage					m_bt;
	HRGN hRgn2;
	wsprintf(path,"%stop\\quitask_ExID.png",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stop\\quitask_quit.png",skinfolder);
	m_Bt2.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_Bt2.SetWindowRgn(hRgn2,true);
	//m_Bt2.SetPlayMusic(false);
	
	wsprintf(path,"%stop\\quitask_ExID.png",skinfolder);
	m_Bt3.LoadButtonBitmap(path,false);
	m_Bt3.SetWindowRgn(hRgn2,true);

	wsprintf(path,"%stop\\quitask_quit_lock.png",skinfolder);
	m_Bt4.LoadButtonBitmap(path,false);
	m_Bt4.SetWindowRgn(hRgn2,true);

	//如果用户已锁机则“锁机退出”按钮不可用
	if (GetMainRoom()->m_PlaceUserInfo.iLockMathine)
	{
		m_Bt4.EnableWindow(false);
	}

	CMainRoomEx::m_bLockQuit = false;

	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt1.LoadButtonBitmap(path,false);
	m_Bt1.m_bIsMouseMove = false;
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_Bt1.SetWindowRgn(hRgn2,true);
	m_Bt1.MoveWindow(m_bk.GetWidth()-m_bt.GetWidth()/4,-2,m_bt.GetWidth()/4,m_bt.GetHeight(),0);

	CenterWindow(AfxGetApp()->m_pMainWnd);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	strMessage = fMsg.GetKeyVal("QuitAsk","SwitchAccount","切换帐号");
	SetDlgItemText(IDC_CHANGEID,strMessage);
	strMessage = fMsg.GetKeyVal("QuitAsk","CloseHall","关闭大厅");
	SetDlgItemText(IDOK,strMessage);
	return TRUE;  
}

void CQuitAsk::LoadSkin()
{
	// TODO:  在此添加额外的初始化
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%stop\\\\quitask.bmp",skinfolder);
	m_bk.SetLoadInfo(path,TRUE);
	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight(),0);
	//CGameImageHelper help(&m_bk);
	//HRGN hRgn;
	//hRgn = AFCBmpToRgn(help,m_bk.GetPixel(0,0),RGB(1,1,1));
	//if(hRgn)
	//	SetWindowRgn(hRgn,TRUE);

	CGameImage					m_bt;
	HRGN hRgn2;
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt2.LoadButtonBitmap(path,false);
	//hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	//if(hRgn2)
	//	m_Bt2.SetWindowRgn(hRgn2,true);
	//m_Bt2.SetPlayMusic(false);

	m_Bt3.LoadButtonBitmap(path,false);
	m_Bt3.SetWindowRgn(hRgn2,true);

	m_Bt4.LoadButtonBitmap(path,false);
	m_Bt4.SetWindowRgn(hRgn2,true);

	//如果用户已锁机则“锁机退出”按钮不可用
	//if (GetMainRoom()->m_PlaceUserInfo.iLockMathine)
	//{
	//	m_Bt4.EnableWindow(false);
	//}

	CMainRoomEx::m_bLockQuit = false;
	wsprintf(path,"%slogon\\log_exit_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt1.LoadButtonBitmap(path,false);
	//hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	//if(hRgn2)
	//	m_Bt1.SetWindowRgn(hRgn2,true);
	//m_Bt1.MoveWindow(m_bk.GetWidth()-m_bt.GetWidth()/4,0,m_bt.GetWidth()/4,m_bk.GetHeight(),0);

	//CenterWindow(AfxGetApp()->m_pMainWnd);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	strMessage = fMsg.GetKeyVal("QuitAsk","SwitchAccount","切换帐号");
	SetDlgItemText(IDC_CHANGEID,strMessage);
	strMessage = fMsg.GetKeyVal("QuitAsk","CloseHall","关闭大厅");
	SetDlgItemText(IDOK,strMessage);
}
void CQuitAsk::OnPaint()
{
	CPaintDC dc(this); 
 
	CGameImageHelper	help(&m_bk);
	help.BitBlt(dc.GetSafeHdc(),0,0);
}

void CQuitAsk::OnOK()
{
	CLoadFileBmp::OnOK();
}

void CQuitAsk::OnClicked1()
{
	
	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
	CLoadFileBmp::OnOK();
}

void CQuitAsk::OnClicked2()
{
	CLoadFileBmp::OnCancel();
}

//切换帐号 按钮
void CQuitAsk::OnClicked3()
{
	GetMainRoom()->PostMessage(WM_COMMAND,MAKELONG(IDC_BTN_MAIN5,BN_CLICKED));
	CLoadFileBmp::OnCancel();
}

void CQuitAsk::OnClicked4()
{
	CMainRoomEx* pMainRoomEx = GetMainRoom();
	if (pMainRoomEx)
	{
		if(pMainRoomEx->m_PlaceUserInfo.iLockMathine == 0)
		{
			CMainRoomEx::m_bLockQuit = true;
			pMainRoomEx->OnSwitchLockButton();
		}
	}
	//::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
	CLoadFileBmp::OnOK();
}

void CQuitAsk::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类
	CLoadFileBmp::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL,m_Bt1);	
	DDX_Control(pDX, IDOK,m_Bt2);
	DDX_Control(pDX, IDC_CHANGEID,m_Bt3);
	DDX_Control(pDX, IDC_LOCKQUIT,m_Bt4);
}


BOOL CQuitAsk::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_LBUTTONUP &&pMsg->hwnd == ((CButton*)this->GetDlgItem(IDOK))->m_hWnd)  
	{   
		BZSoundPlay(this, "music/错误提示.mp3", 0); 
		//return TRUE;
	}  
	return 	CLoadFileBmp::PreTranslateMessage(pMsg);
}
//换肤
LRESULT CQuitAsk::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
