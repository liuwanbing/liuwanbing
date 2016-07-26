#include "stdafx.h"
#include "LeaveMessage.h"
#include "ClientGameDlg.h"


BEGIN_MESSAGE_MAP(CLeaveMessage, CGameFaceGo)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CLeaveMessage::CLeaveMessage() : CGameFaceGo(CLeaveMessage::IDD)
{
	m_pParent=NULL;
}

/**
 * 析构函数
 */
CLeaveMessage::~CLeaveMessage()
{
}

/**
 * 初始化函数
 */
BOOL CLeaveMessage::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();
	
	TCHAR path[MAX_PATH];
	TCHAR skin[MAX_PATH];
	wsprintf(path,TEXT(".\\image\\thingmsg.bmp"));
	m_bk.SetLoadInfo(path,true);
	SetWindowPos(&wndBottom,0,0,m_bk.GetWidth(),m_bk.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

//	TCHAR path[MAX_PATH];
	wsprintf(path,TEXT(".\\image\\agree_bt.bmp"));
	m_btOk.LoadButtonBitmap(path,false);
	wsprintf(path,TEXT(".\\image\\oppose.bmp"));
	m_btCancel.LoadButtonBitmap(path,false);
	CRect ContronRect;
	m_btCancel.GetClientRect(&ContronRect);
	if(m_btOk.GetSafeHwnd())
	{
		GetDlgItem(IDC_LEFT_USER)->MoveWindow(19,47,100,13,false);
		GetDlgItem(IDC_LEAVE_MSG)->MoveWindow(19,67,282,59,false);
		m_btOk.MoveWindow(88,145,ContronRect.Width(),ContronRect.Height(),false);//
		m_btCancel.MoveWindow(186,145,ContronRect.Width(),ContronRect.Height(),false);//
	}
	return TRUE;
}

/**
 * DDX/DDV 支持
 */
void CLeaveMessage::DoDataExchange(CDataExchange* pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
//	DDX_Control(pDX, IDC_LEFT_USER, m_Name);
	DDX_Control(pDX, IDC_LEFT_USER, m_LeaveNameC);
}

void CLeaveMessage::OnPaint() 
{
	CPaintDC dc(this);

	//CRect ClientRect;
	//GetClientRect(&ClientRect);
	
	//this->MapWindowPoints(this->GetParent(),&ClientRect);
	CGameImageHelper	help(&m_bk);
	//this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
	help.BitBlt(dc.GetSafeHdc(),0,0);
	
//	dc.SetBkMode(TRANSPARENT);
//	dc.SetBkColor(RGB(0,0,0));
//	dc.SetTextColor(RGB(200,230,255)) ;
//	dc.SelectObject(&CGameImageLink::m_HFont);
//	dc.TextOut(18,20,"玩家请求离开");
//	DrawBackFace(&dc,0,0,ClientRect.Width(),ClientRect.Height());
	return;
}

/**
 * 取消函数
 */
void CLeaveMessage::OnCancel()
{
	if(m_pParent)
	{
//		if(m_bTouxiang)//投降
//			m_pParent->SendMessage(IDM_ARGEE_LEAVE,2,2);
//		else
			m_pParent->SendMessage(IDM_ARGEE_LEAVE,0,0);
	}
	CGameFaceGo::OnCancel();
}

/**
 * 确定函数
 */
void CLeaveMessage::OnOK()
{
	if(m_pParent)
	{
//		if(m_bTouxiang)//投降
//			m_pParent->SendMessage(IDM_ARGEE_LEAVE,3,3);
//		else
			m_pParent->SendMessage(IDM_ARGEE_LEAVE,1,1);
	}
	CGameFaceGo::OnOK();
}

/**
 * 设置信息
 */
void CLeaveMessage::SetLeaveMessage(TCHAR * szMessage, TCHAR * szUserName)
{
//	if (GetSafeHwnd())
//	{
		GetDlgItem(IDC_LEFT_USER)->SetWindowText(szUserName);
		GetDlgItem(IDC_LEAVE_MSG)->SetWindowText(szMessage);
//	}
	return;
}

void CLeaveMessage::OnLButtonDown(UINT nFlags,CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

HBRUSH CLeaveMessage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	return hbr;
}