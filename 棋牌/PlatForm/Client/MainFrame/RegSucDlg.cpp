#include "StdAfx.h"
#include "RegSucDlg.h"

using namespace BzDui;


CRegSucDlg::CRegSucDlg(void)
{
}


CRegSucDlg::~CRegSucDlg(void)
{
}


LPCTSTR CRegSucDlg::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CRegSucDlg::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CRegSucDlg::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CRegSucDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("logon\\RegSuc.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CRegSucDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CRegSucDlg::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CRegSucDlg::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRegSucDlg::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRegSucDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CRegSucDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRegSucDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if (wParam == IDT_SET_FOCUS)
	{

	}

	return 0;
}

LRESULT CRegSucDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg) 
	{
	case WM_CREATE:
		{
			lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		}
		break;

	case WM_TIMER:
		{
			lRes = OnTimer(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_DESTROY:       
		{
			lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCACTIVATE:    
		{
			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCCALCSIZE:    
		{
			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCPAINT:       
		{
			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCHITTEST:     
		{
			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_SIZE:          
		{
			lRes = OnSize(uMsg, wParam, lParam, bHandled);
		}
		break;

	default:
		bHandled = FALSE;
	}

	if(bHandled)
	{
		return lRes;
	}

	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CRegSucDlg::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			//OnOK();
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			//m_nRetFlag = 0;
			Close();
			return true;
		}
	}
	return false;
}

void CRegSucDlg::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Bnt_Close")) 
		{
			Close();
		}
		else if( msg.pSender->GetName() == _T("Bnt_OK")) 
		{
			Close();
		}
	}
}

void CRegSucDlg::Init()
{

}

void CRegSucDlg::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}


void CRegSucDlg::SetNameID(CString szName, CString szID)
{
	BzDui::CLabelUI* pName = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Name"));
	CString strout;
	strout.Format("<f ºÚÌå 16><c 000080>%s</c></f>", szName);
	if (pName != NULL) pName->SetText(strout);

	BzDui::CLabelUI* pID = static_cast<CLabelUI*>(m_pm.FindControl("Lab_ID"));
	strout.Format("<f ºÚÌå 16><c 000080>%s</c></f>", szID);
	if (pID != NULL) pID->SetText(strout);
}