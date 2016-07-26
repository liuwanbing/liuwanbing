// GameListTip.cpp : implementation file
//

#include "stdafx.h"
#include "GameListTip.h"
//#include "LongonDialog.h"


DWORD CGameListTip::m_sCount = 0;
// CGameListTip dialog

IMPLEMENT_DYNAMIC(CGameListTip, CDialog)

CGameListTip::CGameListTip(CWnd* pParent /*=NULL*/)
	: CDialog(CGameListTip::IDD, pParent)
{

}

CGameListTip::~CGameListTip()
{
}

void CGameListTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameListTip, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CGameListTip::OnBnClickedButtonNext)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


// CGameListTip message handlers
//初始化
void CGameListTip::Init()
{
	TCHAR Path[MAX_PATH];
	wsprintf(Path,"%sdialog\\GameListTip.bmp",m_skinmgr.GetSkinPath());
	m_backPic.SetLoadInfo(Path,CGameImageLink::m_bAutoLock);

	m_bnNext.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BUTTON_NEXT);
	wsprintf(Path,"%sdialog\\GameListTipBn.bmp",m_skinmgr.GetSkinPath());
	m_bnNext.LoadButtonBitmap(Path,false);

	GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText("(1/3)下一步");

	CRect rc;
	GetClientRect(&rc);
	GetDlgItem(IDC_BUTTON_NEXT)->MoveWindow(rc.right/2-100,50+61,88,30);


	LoadSkin();
}

BOOL CGameListTip::OnInitDialog()
{
	Init();
	return true;
}

LRESULT CGameListTip::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	Init();
	Invalidate();
	return LRESULT(0);
}

void CGameListTip::OnBnClickedButtonNext()
{
	CRect rect;
	GetWindowRect(&rect);
	m_sCount++;

	CString strPath = CBcfFile::GetAppPath();
	strPath += "\\ListTip.ini";

	switch (m_sCount%3)
	{
	case 0:
		//是否以后不再提示
		if((((CButton *)GetDlgItem(IDC_CHECK_NOTIFY))->GetCheck()==BST_CHECKED))
		{
			CString csIsNotify;
			csIsNotify.Format("%d",0);
			WritePrivateProfileString("IsNotify", "notify", csIsNotify, strPath);
		}
		OnOK();
		break;
	case 1:
		LoadSkin(1);
		GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText("(2/3)下一步");
		break;
	case 2:
		LoadSkin(2);
		GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText("(3/3)我知道了");
		GetDlgItem(IDC_CHECK_NOTIFY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_NOTIFY)->SetWindowText("以后不再提示");

		break;
	default:
		break;
	}
	// TODO: Add your control notification handler code here

	//MoveWindow(m_sCount*50+50, m_sCount*50+100, rect.Width(), rect.Height());
	Invalidate();
}

void CGameListTip::OnPaint()
{
	// device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	CGameImageHelper TopListBarBottomHandle(&m_bkimage);

	AFCStretchImage(&dc,
		0,0,		
		m_bkimage.GetWidth(),m_bkimage.GetHeight(),
		m_bkimage,
		0,0,
		m_bkimage.GetWidth(),m_bkimage.GetHeight(),
		m_bkimage.GetPixel(0,0));
}

HBRUSH CGameListTip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH   hbr   =   CDialog::OnCtlColor(pDC,   pWnd,   nCtlColor); 
	if(pWnd-> GetDlgCtrlID()   ==   IDC_STATIC_INFOMATION) 
	{
		pDC-> SetBkMode(TRANSPARENT); 
		pDC-> SetTextColor(RGB(0,0,0));
		return     HBRUSH(GetStockObject(HOLLOW_BRUSH)); 
	} 
	if(pWnd-> GetDlgCtrlID()   ==  IDC_CHECK_NOTIFY ) 
	{ 
		pDC-> SetBkMode(TRANSPARENT); 
		pDC-> SetTextColor(RGB(0,0,0));
		return     HBRUSH(GetStockObject(HOLLOW_BRUSH)); 
	} 
	return   hbr; 
}

int CGameListTip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CGameListTip::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	CRect rect;
	GetWindowRect(&rect);
	MoveWindow(50+164, 100+35, rect.Width(), rect.Height());
	UpdateWindow();
}

void CGameListTip::LoadSkin(int index)
{
	CRect rect;
	int r,g,b;
	int cx,cy;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key="LogonDialog";
	TCHAR path[MAX_PATH];
	CString skinfolder;

	m_bkBrush = CreateSolidBrush(m_bkcolor); 

	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%sdialog\\GameListTip%d.bmp",skinfolder,index);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
}