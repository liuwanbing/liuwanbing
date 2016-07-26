// DlgNotify.cpp : implementation file
//

#include "stdafx.h"
//#include "DllForGame.h"
#include "DlgNotify.h"

// 
// CDlgNotify::CDlgNotify()
// {
// 
// }
// 
// CDlgNotify::~CDlgNotify()
// {
// }

#if 1
// CDlgNotify dialog

IMPLEMENT_DYNAMIC(CDlgNotify, CDialog)

CDlgNotify::CDlgNotify(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNotify::IDD, pParent)
{

}

CDlgNotify::CDlgNotify()
: m_TextVar(_T(""))
{

}

CDlgNotify::~CDlgNotify()
{
}

void CDlgNotify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_Text);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_TextVar);
}


BEGIN_MESSAGE_MAP(CDlgNotify, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNotify::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgNotify::OnBnClickedButton1)
END_MESSAGE_MAP()

#endif
// CDlgNotify message handlers

void CDlgNotify::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgNotify::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect rect;

	ScreenToClient(&rect);

	dc.SetBkMode(TRANSPARENT);

	CGameImageHelper helprb(&m_backPic);
	helprb.BitBlt(dc.GetSafeHdc(),0,0);
}

BOOL CDlgNotify::OnInitDialog()
{
	TCHAR path[MAX_PATH];

	m_btOK.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDOK);

	wsprintf(path,"..\\%sdialog\\RoomFeeBtOK.bmp",m_skinmgr.GetSkinPath());
	m_btOK.LoadButtonBitmap(path,false);

	m_btCancle.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BUTTON1);
	
	wsprintf(path,"..\\%sdialog\\RoomFeeClose.bmp",m_skinmgr.GetSkinPath());
	m_btCancle.LoadButtonBitmap(path,false);

	wsprintf(path,"..\\%sdialog\\msg_bk.bmp",m_skinmgr.GetSkinPath());
	m_backPic.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CRect rc;
	GetClientRect(&rc);
	m_btCancle.MoveWindow(rc.right-23,0,23,23);
	m_btOK.MoveWindow(rc.right/2-29,145,67,29);
	m_btOK.SetWindowText("È·¶¨");
	return true;
}

HBRUSH CDlgNotify::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH   hbr   =   CDialog::OnCtlColor(pDC,   pWnd,   nCtlColor); 

	if(pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT) 
	{
		pDC-> SetBkMode(TRANSPARENT); 
		pDC-> SetTextColor(RGB(0,0,0));
		return     HBRUSH(GetStockObject(HOLLOW_BRUSH)); 
	} 
	return   hbr; 
}

void CDlgNotify::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
