#include "StdAfx.h"
//#include "adwnd.h"
#include "Resource.h"
#include "chatting.h"
#include "LogonDialog.h"

//#include "afxcmn.h"
//#include "afcfunction.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CChattingWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//构造函数
CChattingWnd::CChattingWnd(void) :CDialog(CChattingWnd::IDD)
{
}

//析构函数
CChattingWnd::~CChattingWnd(void)
{
}
BOOL CChattingWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	
	SetWindowText(fMsg.GetKeyVal("MainRoom","MyFriendList","我的好友列表!"));
	;;SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	//-------------------------窗体透明----------------------
	HINSTANCE hInst = LoadLibrary("User32.DLL");
	if(hInst != NULL) 
	{ 
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes"); 
		if(fun)
		{
			fun(this->GetSafeHwnd(),0,225,2); 
		}
		FreeLibrary(hInst); 
	}

	CString s=CBcfFile::GetAppPath();/////本地路径
	CINIFile skinf( s + Glb().m_skin);

	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=skinf.GetKeyVal(key,"skinfolder","image\\game\\");
	skinfolder=s+"\\"+skinfolder;
	wsprintf(path,"%sgame\\chatting.bmp",skinfolder);

	m_bk.SetLoadInfo(path,false);

	CGameImageHelper help(&m_bk);
	HRGN hRgn=AFCBmpToRgn(help,RGB(255,0,255),RGB(1,1,1));     //过滤掉RGB(255,0,255)的颜色区域
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);        //设置为没有过滤掉的区域
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}

	CBcfFile f( s + "bzgame.bcf");
	key="BZW";
	CString temp;
	temp=f.GetKeyVal(key,"chattingurl","e:\\a.txt");

	m_URL.Format(temp.GetBuffer());
	m_iWidth = f.GetKeyVal(key,"chattingw",100);
	m_iHeight = f.GetKeyVal(key,"chattingh",400);

	m_iWidth = (m_iWidth > help.GetWidth())?m_iWidth:help.GetWidth();
	m_iHeight = (m_iWidth > help.GetHeight())?m_iHeight:help.GetHeight();	
	CRect ClientRect;
	GetClientRect(&ClientRect);
	this->MapWindowPoints(this->GetParent(),&ClientRect);
	this->MoveWindow(ClientRect.left,ClientRect.top,m_iWidth,m_iHeight);	

	m_WebView.Create("",WS_VISIBLE|WS_CHILD ,CRect(0,0,m_iWidth,m_iHeight),this, IDC_CHATTING);

	TCHAR url[MAX_PATH];
	wsprintf(url,"%s?name=%s&pass=%s",temp,CLogonDialog::m_szName,CLogonDialog::m_szMD5Pass);
	//if(m_WebView.get_LocationURL() != m_URL)
	//{// 改变网址
		if(m_WebView.GetSafeHwnd())
			m_WebView.Navigate(url,NULL,NULL,NULL,NULL);
	//}
//	SetTimer(1,100,NULL);
	return true;
}

//重画函数
void CChattingWnd::OnPaint() 
{
	CPaintDC dc(this); 
	CGameImageHelper	help(&m_bk);
	help.BitBlt(dc.GetSafeHdc(),0,0);
}

void CChattingWnd::OnLButtonDown(UINT nFags,CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	return ;
}


void CChattingWnd::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_AD2, m_WebView);
}

void CChattingWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 1:
		UpdateIE();
		break;
	}
	return ;
}


void CChattingWnd::UpdateIE()
{
	if(m_WebView.GetSafeHwnd())
		m_WebView.Navigate(m_URL,NULL,NULL,NULL,NULL);
	return ;
}
void CChattingWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);
	CDialog::OnClose();
}
