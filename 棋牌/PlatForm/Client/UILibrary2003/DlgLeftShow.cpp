// DlgLeftShow.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLeftShow.h"


// CDlgLeftShow 对话框

IMPLEMENT_DYNAMIC(CDlgLeftShow, CDialog)

CDlgLeftShow::CDlgLeftShow(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SHOW_LEFT, pParent),
	  Showing(0),
	  m_time(10),
	  m_nickName(_T(""))
{

}

CDlgLeftShow::~CDlgLeftShow()
{
}
void CDlgLeftShow::GetNickName(TCHAR * nickName)
{
	if(nickName!=NULL)
	{
		m_nickName=nickName;
		//UpdateData(false);
	}
}

void CDlgLeftShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIME, m_time);
	DDX_Text(pDX, IDC_LEFT_NAME, m_nickName);
	DDV_MaxChars(pDX, m_nickName, 51);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL,m_btCancel);
}

//初始化函数
BOOL CDlgLeftShow::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR		path[MAX_PATH];
	CString		SkinFolder;

	//CString		key = Glb().m_skin;

	CString		s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile    inif(s+"bzgame.bcf");
	CString key=inif.GetKeyVal("BZW","skin","skin0");

	CBcfFile	f( s + "\\image\\" +key+"\\game\\gameskini.bcf");
	SkinFolder	= f.GetKeyVal("config", "skinfolder", "image\\client\\skin0\\");

	wsprintf(path, "%s%sgame\\SomeOneLeft.bmp", s, SkinFolder);
			
	int cx,cy;
	CGameImage	m_bt;
	m_bt.SetLoadInfo(path,true);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path, "%s%sgame\\ok_bt.bmp", s, SkinFolder);
	m_bt.SetLoadInfo(path,true);
	m_btOk.LoadButtonBitmap(path,false);
	m_btOk.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);


	wsprintf(path, "%s%sgame\\cancel_bt.bmp", s, SkinFolder);

	m_bt.SetLoadInfo(path,true);
	m_btCancel.LoadButtonBitmap(path,false);
	m_btCancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return true;
}

void CDlgLeftShow::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);
	
	int y=25;
	TCHAR szBuffer[200];
	memset(szBuffer,0,sizeof(szBuffer));
//	WCHAR wszBuffer[200];
	dc.SetTextColor(RGB(146,220,255));
	dc.SetBkMode(TRANSPARENT);
	
	//图片
	/*::lstrcpy(szBuffer,TEXT("image\\log\\gamelog%ld.png"));
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,wszBuffer,sizeof(wszBuffer));
	Graphics graphics(dc.GetSafeHdc()); // Create a GDI+ graphics object
	Image image(wszBuffer); // Construct an image
	graphics.DrawImage(&image, 125, 4, image.GetWidth(), image.GetHeight());*/

	CFont font;
	font.CreateFont(15,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(font);

	//妮称
	wsprintf(szBuffer,TEXT("%s"),m_nickName);
	CRect rect(115,80,230,100);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_CENTER|DT_LEFT);
}

LRESULT CDlgLeftShow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam,RGB(255,255,255));
		return (LRESULT)::GetStockObject(NULL_BRUSH);
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CDlgLeftShow, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgLeftShow::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLeftShow::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_CREATE()
		ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CDlgLeftShow 消息处理程序

void CDlgLeftShow::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	if(GetParent()!=NULL)
		GetParent()->PostMessageA(WM_CLOSE_GAME);
	//OnOK();
}

void CDlgLeftShow::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	if(GetParent()!=NULL)
		GetParent()->PostMessageA(WM_CLOSE_GAME);
	//OnCancel();
}

void CDlgLeftShow::BeginTimer()
{
	m_time=10;
	SetTimer(1,1000,NULL);
}
void CDlgLeftShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent==1)
	{
		m_time--;
		if(GetSafeHwnd()!=NULL)
			UpdateData(false);//更新秒数
		//时间到，关闭提示和游戏端
		if(m_time==0)
		{
			KillTimer(nIDEvent);
			if(GetParent()!=NULL)
				GetParent()->PostMessageA(WM_CLOSE_GAME);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

int CDlgLeftShow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CDlgLeftShow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	
}