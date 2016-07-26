#include "Stdafx.h"
#include "GameDaoSHiDlg.h"
#include "ClientGameDlg.h"
#include ".\gameinfodlg.h"


BEGIN_MESSAGE_MAP(CGameDaoSHiDlg, CCoolView)		
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CGameDaoSHiDlg::CGameDaoSHiDlg(CWnd * pParent) : CCoolView(CGameDaoSHiDlg::IDD,pParent)
{
	m_pPP=pParent;
	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	m_bk.SetLoadInfo(path,true);
}

/**
 * 析够函数
 */
CGameDaoSHiDlg::~CGameDaoSHiDlg()
{
}

/**
 * DDX/DDV 支持
 */
void CGameDaoSHiDlg::DoDataExchange(CDataExchange* pDX)
{
	CCoolView::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

/**
 * 取消按钮
 */
void CGameDaoSHiDlg::OnCancel()
{
	OutputDebugString("-------OnCancel-------");
	CClientPlayView *p = CONTAINING_RECORD(this,CClientPlayView,m_GameDaoShi);
	CClientGameDlg *pp = CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	::PostMessage(pp->GetSafeHwnd(),WM_CLOSE,0,0); 	
	CCoolView::OnClose();
	return;
}

/**
 * 确定按钮
 */
void CGameDaoSHiDlg::OnOK()
{
	OutputDebugString("-------OnOK-------");
	CClientPlayView *p = CONTAINING_RECORD(this,CClientPlayView,m_GameDaoShi);
	CClientGameDlg *pp = CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	pp->PostMessage(IDM_DAOSHI_MONEY,0,0);		
	CCoolView::OnClose();
	return;
}

//设置对话框信息
void CGameDaoSHiDlg::SetInfo(CString &str)
{
	SetDlgItemText(IDC_SET_STATIC,str);
}

void CGameDaoSHiDlg::AutoSize()
{
	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight());
}

BOOL CGameDaoSHiDlg::OnInitDialog()
{
	CCoolView::OnInitDialog();

	AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	LoadDialogBitmap(path,RGB(255,0,255),RGB(1,1,1));
	AfxSetResourceHandle(GetModuleHandle(NULL));

	m_bk.SetLoadInfo(".\\image\\set_bk.bmp",true);	

	wsprintf(path,TEXT(".\\image\\ok.bmp"));
	m_btOk.LoadButtonBitmap(path,false);

	wsprintf(path,TEXT(".\\image\\cancel.bmp"));
	m_btCancel.LoadButtonBitmap(path,false);

	return TRUE;
}

LRESULT CGameDaoSHiDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
	case WM_CTLCOLORSTATIC :
		{
				SetBkMode((HDC)wParam,TRANSPARENT);
				HBRUSH brush = (HBRUSH)::GetStockObject(NULL_BRUSH);
				return (LRESULT)brush;
		}
	}
	return CCoolView::WindowProc(message, wParam, lParam);
}
