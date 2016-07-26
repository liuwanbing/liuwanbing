// GetHelpMoneyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetHelpMoneyDlg.h"


// ----------------------------------------------------------------------------
//
// PengJiLin, 2011-4-14, 领取金币 对话框
//
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CGetHelpMoneyDlg, CGameFaceGo)

CGetHelpMoneyDlg::CGetHelpMoneyDlg(CWnd* pParent /*=NULL*/)
	: CGameFaceGo(IDD_GETHELPMONEY)
{
    m_pParentWnd = pParent;
}

CGetHelpMoneyDlg::~CGetHelpMoneyDlg()
{
}

void CGetHelpMoneyDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_CLOSE, m_BtClose);
    DDX_Control(pDX, IDC_WEB_VIEW, m_WebView);
}


BEGIN_MESSAGE_MAP(CGetHelpMoneyDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CGetHelpMoneyDlg::OnBnClickedClose)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


LRESULT CGetHelpMoneyDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	Invalidate();
	return LRESULT(0);
}
BOOL CGetHelpMoneyDlg::OnInitDialog()
{
    CGameFaceGo::OnInitDialog();

	Init();

    return TRUE;
}
void CGetHelpMoneyDlg::Init()
{
	CString s=CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	/*// 2012.12.17 yyf 屏蔽
	wsprintf(path,"%sdialog\\gethelpmoney_bk.bmp",skinfolder);
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CGameImageHelper	ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bk.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}*/
	
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CGetHelpMoneyDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/
	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtClose.LoadButtonBitmap(path,false);

	CRect ClientRect;
	GetClientRect(&ClientRect);

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper	help(&m_bk);
	MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());*/
	
	MoveWindow(ClientRect.left,ClientRect.top,cx,cy);// 2012.12.17 yyf 

	//m_WebView.MoveWindow(ClientRect.left+5, ClientRect.top+25, help.GetWidth()-10, help.GetHeight()-30);
	m_WebView.MoveWindow(ClientRect.left+5, ClientRect.top+25, cx-10, cy-30);//2012.10.17 yyf

	//m_BtClose.MoveWindow(help.GetWidth()-36,0,m_bk.GetWidth()/4,m_bk.GetHeight(), FALSE);
	m_BtClose.MoveWindow(cx-36,0,cx/4,cy, FALSE);//2012.10.17 yyf

	m_bkBrush1 = CreateSolidBrush(RGB(0,102,179));
	m_bkBrush2 = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(16,185));//m_bk
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}

void CGetHelpMoneyDlg::LoadSkin()
{
	CString s=CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	// 2012.12.17 yyf 屏蔽
	//wsprintf(path,"%sdialog\\gethelpmoney_bk.bmp",skinfolder);
	//m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	////CGameImageHelper	ImageHandle(&m_bk);
	////HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bk.GetPixel(0,0),RGB(1,1,1));
	////if (hRgn!=NULL)
	////{
	////	SetWindowRgn(hRgn,TRUE);
	////	DeleteObject(hRgn);
	////}
	
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CGetHelpMoneyDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtClose.LoadButtonBitmap(path,false);

	CRect ClientRect;
	GetClientRect(&ClientRect);

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper	help(&m_bk);
	MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());*/
	
	MoveWindow(ClientRect.left,ClientRect.top,cx,cy);// 2012.12.17 yyf 

	//m_WebView.MoveWindow(ClientRect.left+5, ClientRect.top+25, help.GetWidth()-10, help.GetHeight()-30);
	//m_BtClose.MoveWindow(help.GetWidth()-36,0,m_bk.GetWidth()/4,m_bk.GetHeight(), FALSE);
	
	m_WebView.MoveWindow(ClientRect.left+5, ClientRect.top+25, cx-10, cy-30);// 2012.12.17 yyf 
	m_BtClose.MoveWindow(cx-36,0,cx/4,cy, FALSE);// 2012.12.17 yyf 

	//m_bkBrush1 = CreateSolidBrush(RGB(0,102,179));
	//m_bkBrush2 = CreateSolidBrush(m_bk.GetPixel(16,185));

	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
// 领取金币 页面
void CGetHelpMoneyDlg::SetWebAddr(CString& strWeb)
{
    m_WebView.Navigate(strWeb,NULL,NULL,NULL,NULL);
}


HBRUSH CGetHelpMoneyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    if(nCtlColor==CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetBkColor(RGB(0,102,179));
        pDC->SetTextColor(RGB(0,0,0));
        return m_bkBrush1;	
    }
    return hbr;
}

void CGetHelpMoneyDlg::OnPaint()
{
    CPaintDC dc(this);

    CRect ClientRect;
    GetClientRect(&ClientRect);

	/*// 2012.12.17 yyf 屏蔽
    CGameImageHelper	help(&m_bk);
    help.BitBlt(dc.GetSafeHdc(),0,0);*/

	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);
}

void CGetHelpMoneyDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if(point.y < 30)
        PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

void CGetHelpMoneyDlg::OnBnClickedClose()
{
    this->ShowWindow(SW_HIDE);
}

// 设置窗口圆角 2012.10.15 yyf
void CGetHelpMoneyDlg::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
 
    //左上角
	m_arPoints[0].x = winRect.left;
	m_arPoints[0].y = winRect.top+3;
	m_arPoints[1].x = winRect.left+3;
	m_arPoints[1].y = winRect.top;

	//右上角
	m_arPoints[2].x = winRect.right-3;
	m_arPoints[2].y = winRect.top;
	m_arPoints[3].x = winRect.right;
	m_arPoints[3].y = winRect.top+3;

	//右下角
	m_arPoints[4].x = winRect.right;
	m_arPoints[4].y = winRect.bottom-4;
	m_arPoints[5].x = winRect.right-4;
	m_arPoints[5].y = winRect.bottom;

	//左下角
	m_arPoints[6].x = winRect.left+4;
	m_arPoints[6].y = winRect.bottom;
	m_arPoints[7].x = winRect.left;
	m_arPoints[7].y = winRect.bottom-4;

	HRGN hRgn = CreatePolygonRgn(m_arPoints, 8, WINDING);
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
}