// DlgOutMoney.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgOutMoney.h"


// CDlgOutMoney dialog

IMPLEMENT_DYNAMIC(CDlgOutMoney, CDialog)

CDlgOutMoney::CDlgOutMoney(CWnd* pParent /*=NULL*/)
: CDialog(IDD_DLG_CHECKOUT, pParent)
, uCheckOutMoney(0)
, m_RoomCheckOut(_T(""))
{
	m_bkBrush = NULL;
}

CDlgOutMoney::~CDlgOutMoney()
{
	DeleteObject(m_bkBrush);
}

void CDlgOutMoney::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CHECKOUT, uCheckOutMoney);
	DDX_Text(pDX, IDC_OUTROOM, m_RoomCheckOut);
	DDV_MaxChars(pDX, m_RoomCheckOut, 22);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDOK,m_btOk);
}

//初始化函数
BOOL CDlgOutMoney::OnInitDialog()
{
	__super::OnInitDialog();

	Init();
	return true;
}
void CDlgOutMoney::Init()
{
	//贴图
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

	wsprintf(path,"%sdialog\\OutWallet_bk.bmp",skinfolder);
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
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);


	HRGN hRgn2;
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_BtCancel.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,m_bt.GetPixel(0,0),RGB(1,1,1));
	if(hRgn2)
		m_BtCancel.SetWindowRgn(hRgn2,true);
	//m_btcancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//wsprintf(path,"%sdialog\\WalletOk_bt.bmp",skinfolder);
	//m_bt.SetLoadInfo(path,true);
	m_btOk.LoadButtonBitmap(path,false);
	m_btOk.SetWindowRgn(hRgn2,true);
	//m_btOk.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	m_bkBrush = CreateSolidBrush(m_bkimage.GetPixel(32,55));
}
BEGIN_MESSAGE_MAP(CDlgOutMoney, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CDlgOutMoney::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	CFont Font, *OldFont;
	Font.CreateFont(15,10,0,0,0,0,0,0,GB2312_CHARSET
		,3,2,1,2,TEXT("宋体"));
	CWnd *pWnd=GetDlgItem(IDC_EDIT_CHECKOUT);
	CDC *pDC=pWnd->GetDC();
	OldFont = pDC->SelectObject(&Font);

	pWnd->SetFont(&Font,true);
 	/// {{Added by zxd 20100709 释放GDI资源
	pDC->SelectObject(OldFont);
	Font.DeleteObject();
	/// Added by zxd 20100709 释放GDI资源}}
	ReleaseDC(pDC);
}
LRESULT CDlgOutMoney::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetBkColor((HDC)wParam, m_bkimage.GetPixel(32,55));
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

// CDlgOutMoney message handlers

void CDlgOutMoney::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

LRESULT CDlgOutMoney::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	Init();
	Invalidate();
	return LRESULT(0);
}
