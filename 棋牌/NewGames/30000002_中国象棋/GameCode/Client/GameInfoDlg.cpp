#include "Stdafx.h"
#include "GameInfoDlg.h"
#include "ClientGameDlg.h"
#include ".\gameinfodlg.h"


BEGIN_MESSAGE_MAP(CGameInfoDlg, CCoolView)	
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CGameInfoDlg::CGameInfoDlg(CWnd * pParent) : CCoolView(CGameInfoDlg::IDD,pParent)
{
	m_bRequireCoachMoeny = false;
	m_pPP=pParent;
	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	m_bk.SetLoadInfo(path,true);
}

/**
 * 析够函数
 */
CGameInfoDlg::~CGameInfoDlg()
{
}

/**
 * DDX/DDV 支持
 */
void CGameInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CCoolView::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

/**
 * 取消按钮
 */
void CGameInfoDlg::OnCancel()
{
	CClientPlayView *p = CONTAINING_RECORD(this,CClientPlayView,m_GameInfo);
	CClientGameDlg *pp = CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	::PostMessage(pp->GetSafeHwnd(),WM_CLOSE,0,0); 
	CCoolView::OnClose();
}

/**
 * 确定按钮
 */
void CGameInfoDlg::OnOK()
{
	CClientPlayView *p = CONTAINING_RECORD(this,CClientPlayView,m_GameInfo);
	CClientGameDlg *pp = CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	if (true == m_bRequireCoachMoeny)
	{
		pp->PostMessage(IDM_AGREE_INFO,0,0);
	}
	else
	{
		pp->PostMessage(IDM_BEGIN,0,0);
	}

	CCoolView::OnClose();
	return;
}

void CGameInfoDlg::AutoSize()
{
	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight());
}

/**
 * 设置游戏设置
 */
void CGameInfoDlg::SetGameSetInfo(int iJUMinute, int iJuSecond, int iBuMinute, int iBuSecond, int iDuMinute, int iDuSecond, int iGameMoney)
{
	/*if (GetSafeHwnd()==NULL)
		Create(IDD_GAME_INFO,m_pPP);*/	
	SetDlgItemInt(IDC_EDIT_SET_JU_TIME_MINUTE,iJUMinute);
	SetDlgItemInt(IDC_EDIT_SET_JU_TIME_SECOND,iJuSecond);
	SetDlgItemInt(IDC_EDIT_SET_BU_TIME_MINUTE,iBuMinute);
	SetDlgItemInt(IDC_EDIT_SET_BU_TIME_SECOND,iBuSecond);
	SetDlgItemInt(IDC_EDIT_SET_DU_MIAO_MINUTE,iDuMinute);
	SetDlgItemInt(IDC_EDIT_SET_DU_MIAO_SECOND,iDuSecond);	
	SetDlgItemInt(IDC_EDIT_SET_MONEY,iGameMoney);
	return;
}

BOOL CGameInfoDlg::OnInitDialog()
{
	CCoolView::OnInitDialog();

	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	LoadDialogBitmap(path,RGB(255,0,255),RGB(1,1,1));
	//AfxSetResourceHandle(GetModuleHandle(NULL));

	m_bk.SetLoadInfo(".\\image\\set_bk.bmp",true);	

	wsprintf(path,TEXT(".\\image\\ok.bmp"));
	m_btOk.LoadButtonBitmap(path,false);

	wsprintf(path,TEXT(".\\image\\cancel.bmp"));
	m_btCancel.LoadButtonBitmap(path,false);
	

	if (false == m_bRequireCoachMoeny)
	{
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SET_MONEY);
		if (pEdit)
		{
			pEdit->ShowWindow(SW_HIDE);
		}
		CTransparentStc *pStatic = (CTransparentStc *)GetDlgItem(IDC_STATIC_NAME);	
		if (pStatic)
		{
			pStatic->ShowWindow(SW_HIDE);
		}
		pStatic = (CTransparentStc *)GetDlgItem(IDC_STATIC_MONEY_TYPE);
		if (pStatic)
		{
			pStatic->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;
}

LRESULT CGameInfoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	//case WM_CTLCOLORSTATIC :
	//	{
	//			SetBkMode((HDC)wParam,TRANSPARENT);
	//			HBRUSH brush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	//			return (LRESULT)brush;
	//	}
	}
	return CCoolView::WindowProc(message, wParam, lParam);
}

HBRUSH CGameInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_STATIC_TYPE_NAME:
	case IDC_STATIC_MONEY_TYPE:
		{
			pDC-> SetTextColor(RGB(255,255,255));
			pDC->SetBkMode(TRANSPARENT); 
			HBRUSH hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
			return  hbr; 
		}
	default: break;
	}

	return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);
}

