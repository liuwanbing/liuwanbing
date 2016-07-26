#include "Stdafx.h"
#include "GameSet.h"
#include "ClientGameDlg.h"


BEGIN_MESSAGE_MAP(CGameSet, CCoolView)
ON_WM_PAINT()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CGameSet::CGameSet(CWnd * pParent) : CCoolView(CGameSet::IDD,pParent)
{
	m_bBackMusic=TRUE;
	m_bSound=TRUE;
	m_bShowUser=TRUE;
	m_bEnableWatch=TRUE;
	m_pParent=pParent;
}

/**
 * 析构函数
 */
CGameSet::~CGameSet()
{
}

/**
 * DDX/DDV 支持
 */
void CGameSet::DoDataExchange(CDataExchange* pDX)
{
	CCoolView::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, IDC_SOUND, m_check_1);
	DDX_Control(pDX, IDC_USER_MESSAGE, m_check_2);
	DDX_Control(pDX, IDC_WATCH, m_check_3);
	DDX_Control(pDX, IDC_RIGHT_POWER, m_check_4);
//	DDX_Control(pDX, IDC_BK_SOUND, m_check_5);
}

/**
 * 确定按钮
 */
void CGameSet::OnOK()
{
	m_bSound=(m_check_1.GetCheck()==BST_CHECKED);
	m_bShowUser=(m_check_2.GetCheck()==BST_CHECKED);
	m_bEnableWatch=(m_check_3.GetCheck()==BST_CHECKED);
	m_bRightPower=(m_check_4.GetCheck()==BST_CHECKED);
	//m_bBackMusic=(m_check_5.GetCheck()==BST_CHECKED);
	CCoolView::OnOK();
}

/**
 * 初始化对话框
 */
BOOL CGameSet::OnInitDialog()
{
	CCoolView::OnInitDialog();

	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	TCHAR path[MAX_PATH];
	TCHAR skin[MAX_PATH];
	wsprintf(path,TEXT(".\\image\\ok_bt.bmp"));
	m_Ok.LoadButtonBitmap(path,false);
	wsprintf(path,TEXT(".\\image\\cancel_bt.bmp"));
	m_Cancel.LoadButtonBitmap(path,false);
	wsprintf(path,TEXT(".\\image\\game_set.bmp"));
	m_bk.SetLoadInfo(path,true);

	if (m_bSound) m_check_1.SetCheck(BST_CHECKED);
	if (m_bShowUser) m_check_2.SetCheck(BST_CHECKED);
	if (m_bEnableWatch) m_check_3.SetCheck(BST_CHECKED);
	if (m_bRightPower) m_check_4.SetCheck(BST_CHECKED);
	//if (m_bBackMusic) m_check_5.SetCheck(BST_CHECKED);

	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight());
	CenterWindow(GetParent());
	//AutoSize();
	((CWnd*)GetDlgItem(IDC_RIGHT_POWER))->ShowWindow(SW_HIDE);
//	((CWnd*)GetDlgItem(IDC_BK_SOUND))->ShowWindow(SW_HIDE);
	//AfxSetResourceHandle(GetModuleHandle(NULL));
	return TRUE; 
}

/**
 * 自动调整大小
 */
void CGameSet::AutoSize()
{	
	CRect rect,clientrect;
	int cx,cy,csx,csy,x,y;
	((CClientGameDlg*)m_pParent)->m_PlayView.GetClientRect(&rect);
	cx=rect.Width();
	cy=rect.Height();
	csx = (GetSystemMetrics(SM_CXSCREEN)>1024?1024:GetSystemMetrics(SM_CXSCREEN));
	csy = (GetSystemMetrics(SM_CYSCREEN)>768?768:GetSystemMetrics(SM_CYSCREEN));
	GetClientRect(clientrect);
	//x = clientrect.Width();
	x=m_bk.GetWidth();
	//y = clientrect.Height();
	y=m_bk.GetHeight();
	MoveWindow((csx-cx)/2 + (cx-x),(csy-cy)/2 +(cy-y)/2,x,y);
}

LRESULT CGameSet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
				SetBkMode((HDC)wParam,TRANSPARENT);
				HBRUSH brush = (HBRUSH)::GetStockObject(NULL_BRUSH);
				return (LRESULT)brush;
	}
	//InvalidateRect(NULL);	
	return CCoolView::WindowProc(message, wParam, lParam);
}

void CGameSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CGameImageHelper	help(&m_bk);	
	help.BitBlt(dc.GetSafeHdc(),0,0);
}
