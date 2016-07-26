#include "Stdafx.h"
#include "HaveThing.h"
#include "ClientGameDlg.h"
#include ".\Stop.h"

//IMPLEMENT_DYNAMIC(CHaveThing, CAFCBaseDialog)

BEGIN_MESSAGE_MAP(CStop, CGameFaceGo)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CStop::CStop() : CGameFaceGo(CStop::IDD)
{
	m_pParnet=NULL;
	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\stop.bmp"));
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
}

/**
 * 析构函数
 */
CStop::~CStop()
{
}

/**
 * DDX/DDV 支持
 */
void CStop::DoDataExchange(CDataExchange* pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

/**
 * 初始化函数
 */
BOOL CStop::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();
	SetWindowPos(&wndBottom,0,0,m_bk.GetWidth(),m_bk.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\ok.bmp"));
	m_btOk.LoadButtonBitmap(path,false);

	wsprintf(path,TEXT(".\\image\\cancel.bmp"));
	m_btCancel.LoadButtonBitmap(path,false);
	CRect ContronRect;
	m_btCancel.GetClientRect(&ContronRect);
	if(m_btOk.GetSafeHwnd())
	{
		m_btOk.MoveWindow(54,120,ContronRect.Width(),ContronRect.Height(),false);//
		m_btCancel.MoveWindow(143,120,ContronRect.Width(),ContronRect.Height(),false);//
	}
	return TRUE;
}

/**
 * 取消函数
 */
void CStop::OnCancel()
{
//	CGameFaceGo::OnClose();
	CGameFaceGo::OnCancel();
	m_pParnet->SetFocus();
}

/**
 * 确定函数
 */
void CStop::OnOK()
{
	if(m_pParnet)
		m_pParnet->SendMessage(IDM_STOP_THING,0,0);
	CGameFaceGo::OnOK();
}

void CStop::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper handle(&m_bk);
	handle.BitBlt(dc,0,0);
}

BOOL CStop::ShowWindow(int nCmdShow)
{
	CRect rect,rect2;				
	GetParent()->GetClientRect(&rect);
	GetClientRect(&rect2);

	int Offsetx = (rect.Width()-rect2.Width())/2 - 80;
	int Offsety = (rect.Height()-rect2.Height())/2;

	MoveWindow(Offsetx,Offsety,rect2.Width(),rect2.Height(),FALSE);

	return __super::ShowWindow(nCmdShow);
}
