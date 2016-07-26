// DlgOutMoney.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTransferMoney.h"

TCHAR CDlgTransferMoney::m_uDestName[61];

// CDlgTransferMoney dialog

IMPLEMENT_DYNAMIC(CDlgTransferMoney, CDialog)

CDlgTransferMoney::CDlgTransferMoney(CWnd* pParent /*=NULL*/)
: CDialog(IDD_DLG_TRANSFERMONEY, pParent)
, uCheckOutMoney(0)
, m_RoomCheckOut(_T(""))
{
	m_bkBrush = NULL;
}

CDlgTransferMoney::~CDlgTransferMoney()
{
	DeleteObject(m_bkBrush);
}

void CDlgTransferMoney::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//	DDX_Text(pDX, IDC_TRANSFER_NAME, uCheckOutName);
	DDX_Text(pDX, IDC_TRANSFER_MONEY, uCheckOutMoney);
	//DDX_Text(pDX, IDC_OUTROOM, m_RoomCheckOut);
	//DDV_MaxChars(pDX, m_RoomCheckOut, 22);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDOK,m_btOk);
}

//初始化函数
BOOL CDlgTransferMoney::OnInitDialog()
{
	__super::OnInitDialog();
	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin0.bcf");
	CString key=Glb().m_skin;
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal("skin0","skinfolder","image\\client\\skin0\\");
	CGameImage	m_bt;

	wsprintf(path,"%sdialog\\TransferMoney_bk.bmp",skinfolder);
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

	//限制输入字符串长度
	((CEdit*)GetDlgItem(IDC_TRANSFER_NAME))->SetLimitText(20);
	((CEdit*)GetDlgItem(IDC_TRANSFER_MONEY))->SetLimitText(6);
	//
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

	return true;
}

BEGIN_MESSAGE_MAP(CDlgTransferMoney, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CDlgTransferMoney::OnBnClickedOk)
END_MESSAGE_MAP()


void CDlgTransferMoney::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	//CFont Font;//,*OldFont;
	//Font.CreateFont(15,10,0,0,0,0,0,0,GB2312_CHARSET
	//	,3,2,1,2,TEXT("宋体"));
	//CWnd *pWnd=GetDlgItem(IDC_EDIT_CHECKOUT);
	//CDC *pDC=pWnd->GetDC();
	//pDC->SelectObject(&Font);
	//pWnd->SetFont(&Font,true);
	////	DeleteObject(pDC->SelectObject(OldFont));
	//ReleaseDC(pDC);
}
LRESULT CDlgTransferMoney::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

// CDlgTransferMoney message handlers

void CDlgTransferMoney::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

void CDlgTransferMoney::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_TRANSFER_NAME)->GetWindowTextA(m_uDestName,21);
	OnOK();
}
