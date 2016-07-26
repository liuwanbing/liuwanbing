// PersonalInfo.cpp : 实现文件
//
#include <winuser.h>
#include <windows.h>
#include "stdafx.h"
#include "PersonalInfo.h"
#include "GameRoomEx.h"


// CPersonalInfo 对话框

IMPLEMENT_DYNAMIC(CPersonalInfo, CDialog)

CPersonalInfo::CPersonalInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CPersonalInfo::IDD, pParent)
	//, m_szpNiCheng(NULL)
	, m_lCaiFu(0)
	, m_lJiFeng(0)
	, m_szJiBie(_T(""))
	, m_szNiCheng(_T(""))
	, m_time(0)
	, pOrderName(NULL)
	, m_roomWnd(NULL)
{

}

CPersonalInfo::~CPersonalInfo()
{
}

void CPersonalInfo::getRoomPtr(CWnd* room)
{
	m_roomWnd=room;
};

bool CPersonalInfo::UpdateUserItem(UserInfoStruct user)
{	
	if(user.dwUserID>0)
	{
		m_userInfoItem=user;
		m_szNiCheng=m_userInfoItem.nickName;
		m_lCaiFu=m_userInfoItem.dwMoney;
		if(pOrderName!=NULL)
			m_szJiBie=pOrderName(m_userInfoItem.dwPoint);
		else m_szJiBie="";
		m_lJiFeng=m_userInfoItem.dwPoint-GetNormalOrderBasePoint(m_userInfoItem.dwPoint);
		return true;
	}else return false;
}

LRESULT CPersonalInfo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CTLCOLORSCROLLBAR :
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		COLORREF col=::GetBkColor((HDC)wParam);
		SetBkMode((HDC)wParam,TRANSPARENT);
		SetTextColor((HDC)wParam, RGB(255,0,0)) ;
		SetBkColor((HDC)wParam,m_bkimage.GetPixel(0,0));
		return (LRESULT)CreateSolidBrush(m_bkimage.GetPixel(0,0));
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CPersonalInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR path[MAX_PATH];
	::lstrcpy(path,"image\\client\\game\\userInfo.bmp");
	
	int cx,cy;
	CGameImage	m_bt;
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	::lstrcpy(path,"image\\client\\game\\Chat_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btChat.LoadButtonBitmap(path,false);
	m_btChat.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	::lstrcpy(path,"image\\client\\game\\Invite_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btInvite.LoadButtonBitmap(path,false);
	m_btInvite.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	::lstrcpy(path,"image\\client\\game\\watchgame_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btWatch.LoadButtonBitmap(path,false);
	m_btWatch.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	memset(&m_userInfoItem,0,sizeof(m_userInfoItem));
	m_szNiCheng=m_userInfoItem.nickName;
	m_lCaiFu=m_userInfoItem.dwMoney;

	
	if(pOrderName!=NULL)
		m_szJiBie=pOrderName(m_userInfoItem.dwPoint);
	m_lJiFeng=m_userInfoItem.dwPoint-GetNormalOrderBasePoint(m_userInfoItem.dwPoint);
	
	
	m_time=0;
	;;SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
 	//-------------------------窗体透明----------------------
	HINSTANCE hInst = LoadLibrary("User32.DLL");
	if(hInst != NULL) 
	{ 
	typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),RGB(255,0,255),0,2); 
		FreeLibrary(hInst); 
	} 
	SetTimer(WINDOW_APP,15,NULL);
	return FALSE;  // return TRUE unless you set the focus to a control
 }

void CPersonalInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAIFU, m_lCaiFu);
	DDX_Text(pDX, IDC_JIFENG, m_lJiFeng);
	DDX_Text(pDX, IDC_JIBIE, m_szJiBie);
	DDX_Text(pDX, IDC_NICHENG, m_szNiCheng);

	DDX_Control(pDX, IDC_CHAT, m_btChat);
	DDX_Control(pDX, IDC_INVITE_PLAY, m_btInvite);
	DDX_Control(pDX, IDC_WATCH_GAME, m_btWatch);
}

void CPersonalInfo::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	int y=25;
	TCHAR szBuffer[200];
	WCHAR wszBuffer[200];
	dc.SetTextColor(RGB(146,220,255));
	dc.SetBkMode(TRANSPARENT);
	int iCountTotal=m_userInfoItem.uCutCount+m_userInfoItem.uLostCount+m_userInfoItem.uMidCount+m_userInfoItem.uWinCount;
	float temprun=(iCountTotal==0)?0:m_userInfoItem.uCutCount*100/iCountTotal,
		tempwin=(iCountTotal==0)?0:m_userInfoItem.uWinCount*100/iCountTotal;

	//人物头象
	wsprintf(szBuffer,TEXT("%simage\\log\\gamelog%ld.png"),CINIFile::GetAppPath(),m_userInfoItem.bLogoID);
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,wszBuffer,sizeof(wszBuffer));
	Graphics graphics(dc.GetSafeHdc()); // Create a GDI+ graphics object
	Image image(wszBuffer); // Construct an image
	graphics.DrawImage(&image, 125, 4, image.GetWidth(), image.GetHeight());

	CFont font;
	font.CreateFont(12,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(font);

	//妮称
	wsprintf(szBuffer,TEXT("%s"),m_szNiCheng);
	CRect rect(47,29,185,255);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	
	
	//游戏状态
	switch(m_userInfoItem.bUserState)
	{	
	case USER_NO_STATE:
		lstrcpy(szBuffer,"未知状态");
		break;
	case USER_LOOK_STATE:
		lstrcpy(szBuffer,"未坐下");
		break;
	case USER_SITTING:
		lstrcpy(szBuffer,"在游戏桌");
		break;
	case USER_ARGEE:
		lstrcpy(szBuffer,"同意状态");
		break;
	case USER_WATCH_GAME:
		lstrcpy(szBuffer,"旁观游戏");
		break;
	case USER_CUT_GAME:
		lstrcpy(szBuffer,"断线状态");
		break;
	case USER_PLAY_GAME:
		lstrcpy(szBuffer,"游戏中");
		break;
	}
	rect.left=69;
	rect.top+=y-3;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//财富级别
	wsprintf(szBuffer,TEXT("%s"),GetMoneyOrderName(m_lCaiFu));
	rect.top+=(y-2);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//积分
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.dwPoint-GetNormalOrderBasePoint(m_userInfoItem.dwPoint));
	rect.left=47;
	rect.top+=(y-3);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//银子
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.dwMoney);
	rect.top+=(y-2);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//网速
	wsprintf(szBuffer,TEXT("%d"),90);
	rect.top+=(y-2);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	
	
	//逃跑率
	wsprintf(szBuffer,TEXT("%.2f%%"),temprun);
	rect.left+=9;
	rect.top+=(y-3);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);

	//输
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.uLostCount);
	rect.left-=20;
	rect.top+=(y-3);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//嬴
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.uWinCount);
	rect.left+=62;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	
	
	//平
	/*sprintf(szBuffer,TEXT("%s"),GetMoneyOrderName(m_lCaiFu));
	rect.top+=(y-2);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	*/

	//胜率
	wsprintf(szBuffer,TEXT("%.2f%%"),tempwin);
	rect.left-=53;
	rect.top+=(y-2);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//魅力
	wsprintf(szBuffer,"魅力：%d",m_userInfoItem.dwFascination);
	rect.left+=53;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);
		
}
void CPersonalInfo::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==WINDOW_APP)
	{
		KillTimer(WINDOW_DISAPP);
		if(m_time==20) 
		{
			KillTimer(WINDOW_APP);
		}
		else 
		{
			m_time++;
			//;;SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
			//-------------------------窗体透明----------------------
			HINSTANCE hInst = LoadLibrary("User32.DLL");
			if(hInst != NULL) 
			{ 
				typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
				MYFUNC fun = NULL;
				//取得SetLayeredWindowAttributes函数指针 
				fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
				if(fun)fun(this->GetSafeHwnd(),RGB(255,0,255),12*m_time,2); 
				FreeLibrary(hInst); 
			} 
		}
	}else if(nIDEvent==WINDOW_DISAPP)
	{
		KillTimer(WINDOW_APP);
		if(m_time==0)
		{
			KillTimer(WINDOW_DISAPP);
			DestroyWindow();
		}else 
		{
			m_time--;
			//;;SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
			//-------------------------窗体透明----------------------
			HINSTANCE hInst = LoadLibrary("User32.DLL");
			if(hInst != NULL) 
			{ 
				typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
				MYFUNC fun = NULL;
				//取得SetLayeredWindowAttributes函数指针 
				fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
				if(fun)fun(this->GetSafeHwnd(),RGB(255,0,255),12*m_time,2); 
				FreeLibrary(hInst); 
			} 
		}
	}
	CWnd::OnTimer(nIDEvent);
}
void CPersonalInfo::close()
{
	KillTimer(WINDOW_APP);
	SetTimer(WINDOW_DISAPP,10,NULL);
}

BEGIN_MESSAGE_MAP(CPersonalInfo, CDialog)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHAT, &CPersonalInfo::OnBnClickedChat)
	ON_BN_CLICKED(IDC_INVITE_PLAY, &CPersonalInfo::OnBnClickedCall)
	ON_BN_CLICKED(IDC_WATCH_GAME, &CPersonalInfo::OnBnClickedWatchGame)
END_MESSAGE_MAP()


// CPersonalInfo 消息处理程序

void CPersonalInfo::OnBnClickedChat()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_roomWnd!=NULL)
	{
		UserItemStruct * pUserItem=((CGameRoomEx*)m_roomWnd)->m_UserManage.FindOnLineUser(m_userInfoItem.dwUserID);
		((CGameRoomEx*)m_roomWnd)->SendShortMessage(pUserItem);
	}
}
void CPersonalInfo::OnBnClickedCall()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置此桌子和位子号的用户为当前用户
	if(m_roomWnd!=NULL)
	{
		((CGameRoomEx*)m_roomWnd)->SetCurrentUser(m_userInfoItem.dwUserID);
		((CGameRoomEx*)m_roomWnd)->OnInvitePlay();
	}

}

void CPersonalInfo::OnBnClickedWatchGame()
{
	if(m_roomWnd!=NULL)
		((CGameRoomEx*)m_roomWnd)->OnLeftHitDesk(m_userInfoItem.bDeskNO,m_userInfoItem.bDeskStation);
}
