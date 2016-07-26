// PersonalInfo.cpp : 实现文件
//
#include "../stdafx.h"
#include <winuser.h>
#include <windows.h>
#include "PersonalInfo.h"
//#include "GameRoomEx.h"
//#include "gameframedlg.h"
extern EXT_CLASS TCHAR *GlbGetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);

//Avatar系统相关
#define BzGetAvatarIndex(obj) \
	GetAvatarIndex((obj).bLogoID,(obj).i64Bank,(obj).i64Money,(obj).dwPoint,0,3)


// CPersonalInfo 对话框

IMPLEMENT_DYNAMIC(CPersonalInfo, CDialog)

CPersonalInfo::CPersonalInfo(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PERSONINFO, pParent)
	//, m_szpNiCheng(NULL)
	, m_i64CaiFu(0)
	, m_lJiFeng(0)
	, m_szJiBie(_T(""))
	, m_szNiCheng(_T(""))
	, m_time(0)
	, pOrderName(NULL)
	, m_roomWnd(NULL)
{
	m_bkBrush=NULL;
}

CPersonalInfo::~CPersonalInfo()
{
	DeleteObject(m_bkBrush);
}

void CPersonalInfo::getRoomPtr(CWnd* room)
{
	m_roomWnd=room;
};

bool CPersonalInfo::UpdateUserItem(UserInfoStruct user)
{
	
	if(user.dwUserID!=0)
	{
		m_userInfoItem=user;
		m_szNiCheng=m_userInfoItem.nickName;
		m_i64CaiFu=m_userInfoItem.i64Money;
		if(pOrderName!=NULL)
			m_szJiBie=pOrderName(m_userInfoItem.dwPoint);
		else m_szJiBie="";
		m_lJiFeng=m_userInfoItem.dwPoint-GetNormalOrderBasePoint(m_userInfoItem.dwPoint);
		//加载人物头像

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
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CPersonalInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR		path[MAX_PATH];
	CString		SkinFolder;
	CString		key = Glb().m_skin;

	/// 读取BCF，取得倍数显示金币的倍数的幂次
	CString szAppPath = CBcfFile::GetAppPath();

	CString		s = "..\\";// CINIFile::GetAppPath ();/////本地路径
	CBcfFile	f( s + "skin.bcf");
	SkinFolder	= f.GetKeyVal(key, "skinfolder", "image\\client\\skin1\\");


	ZeroMemory(path,sizeof(path));
	wsprintf(path, "%s%sgame\\userInfo.bmp", s, SkinFolder);
	
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

	ZeroMemory(path,sizeof(path));
	wsprintf(path, "%s%sgame\\Chat_bt.bmp", s, SkinFolder);
	//::lstrcpy(path,"image\\game\\Chat_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btChat.LoadButtonBitmap(path,false);
	m_btChat.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	ZeroMemory(path,sizeof(path));
	wsprintf(path, "%s%sgame\\Invite_bt.bmp", s, SkinFolder);
//	::lstrcpy(path,"image\\game\\Invite_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btInvite.LoadButtonBitmap(path,false);
	m_btInvite.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	ZeroMemory(path,sizeof(path));
	wsprintf(path, "%s%sgame\\watchgame_bt.bmp", s, SkinFolder);
	//::lstrcpy(path,"image\\game\\watchgame_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btWatch.LoadButtonBitmap(path,false);
	m_btWatch.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	ZeroMemory(path,sizeof(path));
	wsprintf(path, "%s%sgame\\gift_bt.bmp", s, SkinFolder);
	//::lstrcpy(path,"image\\game\\watchgame_bt.bmp");
	m_bt.SetLoadInfo(path,true);
	m_btGift.LoadButtonBitmap(path,false);
	m_btGift.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	memset(&m_userInfoItem,0,sizeof(m_userInfoItem));
	m_szNiCheng=m_userInfoItem.nickName;
	m_i64CaiFu=m_userInfoItem.i64Money;
	
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


	m_bkBrush=CreateSolidBrush(m_bkimage.GetPixel(0,0));


	return FALSE;  // return TRUE unless you set the focus to a control
 }

void CPersonalInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAIFU, m_i64CaiFu);
	DDX_Text(pDX, IDC_JIFENG, m_lJiFeng);
	DDX_Text(pDX, IDC_JIBIE, m_szJiBie);
	DDX_Text(pDX, IDC_NICHENG, m_szNiCheng);

	DDX_Control(pDX, IDC_CHAT, m_btChat);
	DDX_Control(pDX, IDC_INVITE_PLAY, m_btInvite);
	DDX_Control(pDX, IDC_WATCH_GAME, m_btWatch);
	DDX_Control(pDX, IDC_GIFT, m_btGift);
}

void CPersonalInfo::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	int y=22;
	TCHAR szBuffer[200];
	WCHAR wszBuffer[200];
	memset(szBuffer,0,sizeof(szBuffer));
	memset(wszBuffer,0,sizeof(wszBuffer));

	dc.SetTextColor(RGB(146,220,255));
	dc.SetBkMode(TRANSPARENT);
	int iCountTotal=m_userInfoItem.uCutCount+m_userInfoItem.uLostCount+m_userInfoItem.uMidCount+m_userInfoItem.uWinCount;
	int temprun=(iCountTotal==0)?0:m_userInfoItem.uCutCount*100/iCountTotal,
		tempwin=(iCountTotal==0)?0:m_userInfoItem.uWinCount*100/iCountTotal;

	//人物头象
	//////////////////////////////////////////////////////////////////////////
	//Modify By Fred Huang,2008-04-23

	//{{add by zxd 2009-03-26，找上一级目录来读头像图片，用szAppPathParent代替原来的CINIFile::GetAppPath()
	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);
	//add by zxd}}

	wsprintf(szBuffer,TEXT("%s/image\\log\\%s\\gamelog%ld.png"),szAppPathParent,m_userInfoItem.bBoy?"Boy":"Girl",BzGetAvatarIndex(m_userInfoItem));
	if(m_userInfoItem.bLogoID>=0xFF && m_userInfoItem.bLogoID<0x200)
	{
		wsprintf(szBuffer,"..\\CustomFace\\%d.png",m_userInfoItem.dwUserID);
		CFileFind ff;
		BOOL bFoundFile=ff.FindFile(szBuffer);
		ff.Close();
		if(!bFoundFile)
			wsprintf(szBuffer,"%s/image\\log\\%s\\gamelog%d.png",szAppPathParent,m_userInfoItem.bBoy?"Boy":"Girl",BzGetAvatarIndex(m_userInfoItem));
	}
#ifndef BZ_ZLIB
	if (!CBcfFile::IsFileExist(szBuffer))
	{
		wsprintf(szBuffer,"%s/image\\log\\%s\\gamelog%d.png",szAppPathParent,m_userInfoItem.bBoy?"Boy":"Girl",0/*m_userInfoItem.bLogoID%12*/);
	}
#endif
	
//////////////////////////////////////////////////////////////////////////
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,wszBuffer,sizeof(wszBuffer));
	Graphics graphics(dc.GetSafeHdc()); // Create a GDI+ graphics object
#ifndef BZ_ZLIB
	Image image(wszBuffer); // Construct an image
#else
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	pStream=unZipRes.LoadFileResFromZip(szBuffer);

	if(pStream==NULL)
	{
		wsprintf(szBuffer,"%s/image\\log\\%s\\gamelog%d.png",szAppPathParent,m_userInfoItem.bBoy?"Boy":"Girl",0/*m_userInfoItem.bLogoID%12*/);
		pStream=unZipRes.LoadFileResFromZip(szBuffer);
	}

	Image image(pStream);
	if(pStream!=NULL)
	{
		pStream->Release();
		pStream=NULL;
	}
#endif
	
	graphics.DrawImage(&image, 125, 6, 51,73);// image.GetWidth(), image.GetHeight());

	CFont font;
	font.CreateFont(12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(font);

	//妮称
	wsprintf(szBuffer,TEXT("%s"),m_szNiCheng);
	CRect rect(47,29,185,255);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

		//爱聊修改，Fred Huang,2008-06-13

	//爱聊号：（ID号）
	//wsprintf(szBuffer,"%d",m_userInfoItem.dwUserID);
	//rect.left=60;
	//rect.top+=y;
	//dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	
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
	default:
		lstrcpy(szBuffer,"未知状态");
		break;
	}
	rect.left=69;
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//财富级别
	wsprintf(szBuffer,TEXT("%s"),GetMoneyOrderName(m_i64CaiFu));
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//积分
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.dwPoint-GetNormalOrderBasePoint(m_userInfoItem.dwPoint));
	rect.left=47;
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//金币
	//wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.dwMoney);
	GlbGetNumString(szBuffer, m_userInfoItem.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//网速
	wsprintf(szBuffer,TEXT("%d"),90);
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	
	
	//逃跑率
	wsprintf(szBuffer,TEXT("%2d%%"),temprun);
	rect.left+=20;
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);

	//输
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.uLostCount);
	rect.left-=20;
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//嬴
	wsprintf(szBuffer,TEXT("%d"),m_userInfoItem.uWinCount);
	rect.left+=60;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//胜率
	wsprintf(szBuffer,TEXT("%2d%%"),tempwin);
	rect.left-=60;
	rect.top+=y;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);	

	//魅力
	wsprintf(szBuffer,"%d",m_userInfoItem.dwFascination);
	rect.left+=70;
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_LEFT|DT_TOP);

	font.DeleteObject();

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
	ON_BN_CLICKED(IDC_GIFT, &CPersonalInfo::OnBnClickedGift)
END_MESSAGE_MAP()


// CPersonalInfo 消息处理程序

void CPersonalInfo::OnBnClickedChat()
{
	 //TODO: 在此添加控件通知处理程序代码
	if(m_roomWnd!=NULL)
	{
		//UserItemStruct * pUserItem=((CGameRoomEx*)m_roomWnd)->m_UserManage.FindOnLineUser(m_userInfoItem.dwUserID);
		//((CGameRoomEx*)m_roomWnd)->SendShortMessage(pUserItem);
		m_roomWnd->PostMessage(WM_CHATING_WITH,(WPARAM)m_userInfoItem.dwUserID,(LPARAM)m_userInfoItem.dwUserID);
	}
}
void CPersonalInfo::OnBnClickedCall()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置此桌子和位子号的用户为当前用户
	if(m_roomWnd!=NULL)
	{
		//((CGameRoomEx*)m_roomWnd)->SetCurrentUser(m_userInfoItem.dwUserID);
		//((CGameRoomEx*)m_roomWnd)->OnInvitePlay();
		m_roomWnd->PostMessage(WM_INVITE_PLAY,(WPARAM)m_userInfoItem.dwUserID,(LPARAM)m_userInfoItem.dwUserID);
	}
}

void CPersonalInfo::OnBnClickedWatchGame()
{
	if(m_roomWnd!=NULL)
	{
		//((CGameRoomEx*)m_roomWnd)->OnLeftHitDesk(m_userInfoItem.bDeskNO,m_userInfoItem.bDeskStation);
		m_roomWnd->PostMessage(WM_WATCH_GAME,(WPARAM)m_userInfoItem.bDeskNO,(LPARAM)m_userInfoItem.bDeskStation);
	}
}

void CPersonalInfo::OnBnClickedGift()
{
	if(m_roomWnd!=NULL)
	{
		//((CGameRoomEx*)m_roomWnd)->OnLeftHitDesk(m_userInfoItem.bDeskNO,m_userInfoItem.bDeskStation);
		m_roomWnd->PostMessage(WM_USER+157,(WPARAM)m_userInfoItem.dwUserID,1);
	}

}