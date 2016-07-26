// FindNullDesk.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "FindNullDesk.h"

#define WM_EXCHANGE_SKIN			WM_USER + 1666				//更换皮肤
// CFindNullDesk 对话框

IMPLEMENT_DYNAMIC(CFindNullDesk, CDialog)

CFindNullDesk::CFindNullDesk(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FINDNULLDESK, pParent)
{
	m_bLastFindDesk=-1;
	m_bkBrush = NULL;
}

CFindNullDesk::~CFindNullDesk()
{
	DeleteObject(m_bkBrush);
}

void CFindNullDesk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btcancel);
	DDX_Control(pDX, IDOK, m_btok);
	DDX_Control(pDX, IDC_SIT, m_btsit);
}


BEGIN_MESSAGE_MAP(CFindNullDesk, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CFindNullDesk::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SIT, &CFindNullDesk::OnBnClickedSit)
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CFindNullDesk::OnExchangeSkin)
END_MESSAGE_MAP()


// CFindNullDesk 消息处理程序

void CFindNullDesk::OnPaint()
{
	CPaintDC dc(this);

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);*/
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);
}
LRESULT CFindNullDesk::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	Init();
	Invalidate();
	return LRESULT(0);
}
void CFindNullDesk::Init()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	CGameImage	m_bt;

	/*// 2012.12.17 yyf 屏蔽
	wsprintf(path,"%sdialog\\findnulldesk_bk.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();*/
	
	//*-------初始化 m_configDlg--------- 2012.10.17 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CFindNullDeskConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_bt.SetLoadInfo(path,true);
	m_btcancel.LoadButtonBitmap(path,false);
	m_btcancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sdialog\\finduser_bt.bmp",skinfolder); //向凌华 2012.10.25
	m_bt.SetLoadInfo(path,true);
	m_btok.LoadButtonBitmap(path,false);
	m_btok.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sdialog\\findnulldesk_sit.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btsit.LoadButtonBitmap(path,false);
	m_btsit.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	int issit1,issit2,issit3,issit4;
	issit1=AfxGetApp()->GetProfileInt(TEXT("FindNullDesk"),TEXT("one"),0);
	((CButton *)GetDlgItem(IDC_ONE))->SetCheck(issit1);
	issit2=AfxGetApp()->GetProfileInt(TEXT("FindNullDesk"),TEXT("two"),0);
	((CButton *)GetDlgItem(IDC_TWO))->SetCheck(issit2);
	issit3=AfxGetApp()->GetProfileInt(TEXT("FindNullDesk"),TEXT("uptwo"),0);
	((CButton *)GetDlgItem(IDC_UPTWO))->SetCheck(issit3);
	issit4=AfxGetApp()->GetProfileInt(TEXT("FindNullDesk"),TEXT("null"),0);
	((CButton *)GetDlgItem(IDC_NULL))->SetCheck(issit4);
	if(!issit1&&!issit2&&!issit3&&!issit4)
		((CButton *)GetDlgItem(IDC_ONE))->SetCheck(true);
	m_bLastFindDesk=-1;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	SetDlgItemText(IDC_STATIC_SEARCH,fMsg.GetKeyVal("FindDesk","Search","查找"));
	SetDlgItemText(IDC_ONE,fMsg.GetKeyVal("FindDesk","OneSeat","一个空座位"));
	SetDlgItemText(IDC_TWO,fMsg.GetKeyVal("FindDesk","TwoSeats","二个空座位"));
	SetDlgItemText(IDC_UPTWO,fMsg.GetKeyVal("FindDesk","AtlessOnePerson","至少有一个人的桌子"));
	SetDlgItemText(IDC_NULL,fMsg.GetKeyVal("FindDesk","NoBody","没有人的座位"));


	m_bkBrush = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(50,50));//m_bkimage

	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
BOOL CFindNullDesk::OnInitDialog()
{
	CDialog::OnInitDialog();

	Init();

	return TRUE; 
}

LRESULT CFindNullDesk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_configDlg.GetBKImageEX().GetPixel(50,50));//m_bkimage
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CFindNullDesk::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CFindNullDesk::OnBnClickedOk()
{
	int bOneNull, bTwoNull, bNoAllNull,bAllNull;
	bOneNull=((CButton *)GetDlgItem(IDC_ONE))->GetCheck();
	bTwoNull=((CButton *)GetDlgItem(IDC_TWO))->GetCheck();
	bNoAllNull=((CButton *)GetDlgItem(IDC_UPTWO))->GetCheck();
	bAllNull=((CButton *)GetDlgItem(IDC_NULL))->GetCheck();
	if(!bOneNull&&!bTwoNull&&!bNoAllNull&&!bAllNull)
		return ;
	m_bLastFindDesk=m_pDeskFrame->FindGameDesk(m_bLastFindDesk,bOneNull,  bTwoNull,  bNoAllNull, bAllNull);

}

void CFindNullDesk::OnBnClickedSit()
{
	if(m_bLastFindDesk==255)
		return ;
	BYTE bNullStation=m_pDeskFrame->GetNullStation(m_bLastFindDesk);
	if(bNullStation!=255)
		m_pGameRoomEx->SendMessage(WM_LEFT_MOUSE_HIT_DESK,m_bLastFindDesk,bNullStation);
}

void CFindNullDesk::OnCancel()
{
	int issit;
	issit=((CButton *)GetDlgItem(IDC_ONE))->GetCheck();
	AfxGetApp()->WriteProfileInt(TEXT("FindNullDesk"),TEXT("one"),issit);
	
	issit=((CButton *)GetDlgItem(IDC_TWO))->GetCheck();
	AfxGetApp()->WriteProfileInt(TEXT("FindNullDesk"),TEXT("two"),issit);

	issit=((CButton *)GetDlgItem(IDC_UPTWO))->GetCheck();
	AfxGetApp()->WriteProfileInt(TEXT("FindNullDesk"),TEXT("uptwo"),issit);

	issit=((CButton *)GetDlgItem(IDC_NULL))->GetCheck();
	AfxGetApp()->WriteProfileInt(TEXT("FindNullDesk"),TEXT("null"),issit);
	DestroyWindow();
	CDialog::OnCancel();
}

void CFindNullDesk::OnOK()
{
	
}

// 设置窗口圆角 2012.10.15 yyf
void CFindNullDesk::SetWndRgn(void)
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