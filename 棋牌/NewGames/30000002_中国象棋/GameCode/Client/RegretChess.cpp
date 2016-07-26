#include "StdAfx.h"
#include "ClientGameDlg.h"
#include ".\regretchess.h"

CRegretChess::CRegretChess():CGameFaceGo(CRegretChess::IDD)
{
	m_pParnet=NULL;

	TCHAR path[MAX_PATH];
	wsprintf(path,TEXT("image\\huiqi.bmp"));
	m_bk.SetLoadInfo(path, CGameImageLink::m_bAutoLock);
}

CRegretChess::~CRegretChess(void)
{
}

void CRegretChess::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);	
}

BOOL CRegretChess::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();//SWP_NOMOVE|
	SetWindowPos(&wndBottom,0,0,m_bk.GetWidth(),m_bk.GetHeight(),SWP_NOZORDER);

	TCHAR filename[MAX_PATH];

	wsprintf(filename,".\\image\\agree_bt.bmp");
	m_btOk.LoadButtonBitmap(filename,false);

	wsprintf(filename,".\\image\\oppose.bmp");
	m_btCancel.LoadButtonBitmap(filename,false);

	CRect ContronRect;
	m_btCancel.GetClientRect(&ContronRect);

	if(m_btOk.GetSafeHwnd())
	{
		m_btOk.MoveWindow(54,120,ContronRect.Width(),ContronRect.Height(),false);//
		m_btCancel.MoveWindow(143,120,ContronRect.Width(),ContronRect.Height(),false);//
	}

	CenterWindow(GetParent());
	return TRUE;	
}
BEGIN_MESSAGE_MAP(CRegretChess, CGameFaceGo)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CRegretChess::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper handle(&m_bk);
	handle.BitBlt(dc,0,0);
}

void CRegretChess::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	if(m_pParnet)
		m_pParnet->SendMessage(IDM_AGREE_REGRET,1,1);

	CGameFaceGo::OnOK();
}

void CRegretChess::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	if(m_pParnet)
		m_pParnet->SendMessage(IDM_AGREE_REGRET,0,0);

	CGameFaceGo::OnCancel();
}

BOOL CRegretChess::ShowWindow(int nCmdShow)
{
	//CenterWindow(GetParent());

	CRect rect,rect2;				
	GetParent()->GetClientRect(&rect);
	GetClientRect(&rect2);

	int Offsetx = (rect.Width()-rect2.Width())/2 - 80;
	int Offsety = (rect.Height()-rect2.Height())/2;

	MoveWindow(Offsetx,Offsety,rect2.Width(),rect2.Height(),FALSE);

	return __super::ShowWindow(nCmdShow);
}