#include "StdAfx.h"
#include "MenuDu.h"

using namespace BzDui;

CMenuDu::CMenuDu(HWND hParent,UINT uID)
{
	//父窗口的句柄
	m_hParent = hParent;

	//清空菜单数据
	m_vtMenu.clear();

	//列表控件
	m_pListCtrl = NULL;

	//菜单ID
	m_uID = uID;
}

CMenuDu::~CMenuDu(void)
{
}

LPCTSTR CMenuDu::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CMenuDu::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CMenuDu::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CMenuDu::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("game\\menu.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	return 0;
}

LRESULT CMenuDu::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuDu::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMenuDu::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMenuDu::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMenuDu::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CMenuDu::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMenuDu::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_hWnd != (HWND)wParam)
	{
		PostMessage(WM_CLOSE);
	}
	return 0;
}

LRESULT CMenuDu::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == VK_ESCAPE)
	{
		Close();
	}
	return 0;
}

LRESULT CMenuDu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case WM_KILLFOCUS:
			{
				lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
			}
			break;

		case WM_KEYDOWN:
			{
				lRes = OnKeyDown(uMsg, wParam, lParam, bHandled);
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

LRESULT CMenuDu::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			Close();
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			Close();
			return true;
		}
	}
	else if (uMsg == WM_KEYDOWN)
	{
		return true;
	}
	return false;
}

void CMenuDu::Init() 
{
	this->Create(m_hParent,NULL,WS_POPUP,WS_EX_TOOLWINDOW);

	LoadNormalCtrlSkin();

	m_pListCtrl = static_cast<CListUI*>(m_pm.FindControl("Lst_Menu"));

	CControlUI* pBkCtn = m_pm.FindControl("Ver_Menu");

	RECT rcInset = {30,0,0,0};
	int iWndHeight = 0;
	if (!m_vtMenu.empty())
	{
		for (int i = 0; i < m_vtMenu.size(); ++i)
		{
			CListContainerElementUI* pCtnCtrl = new CListContainerElementUI();
			if (m_vtMenu[i].uMsgID > 0)
			{
				pCtnCtrl->SetFixedHeight(22);
				pCtnCtrl->SetInset(rcInset);

				CLabelUI* pLabCtrl = new CLabelUI();
				pLabCtrl->SetText(m_vtMenu[i].szName);
				pLabCtrl->SetMouseEnabled(false);
				pLabCtrl->SetFloat(false);
				pCtnCtrl->Add(pLabCtrl);
				iWndHeight += 22;
			}
			else if (m_vtMenu[i].uMsgID == 0) //分隔线
			{
				pCtnCtrl->SetFixedHeight(1);
				pCtnCtrl->SetInset(rcInset);
				pCtnCtrl->SetBkImage("game\\menu_sep.png");

				CControlUI* pImgCtrl = new CControlUI();
				pCtnCtrl->Add(pImgCtrl);
				pCtnCtrl->SetEnabled(false);
				iWndHeight += 1;
			}
			
			m_pListCtrl->Add(pCtnCtrl);
			
		}
	}

	rcInset = m_pListCtrl->GetInset();
	iWndHeight += rcInset.top;
	iWndHeight += rcInset.bottom;

	this->ResizeClient(-1,iWndHeight);

	FixPosition();
}

void CMenuDu::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
	}

	if( msg.sType == _T("itemselect")) 
	{
		if (msg.wParam < 0 || msg.wParam >= m_vtMenu.size())
		{
			return;
		}

		TMenuNode* p = (TMenuNode*)&m_vtMenu[msg.wParam];
		if (p->uMsgID == 0) //分隔线
		{
			return;
		}

		::SendMessage(m_hParent,m_uID,p->uMsgID,0);
		Close();
		return;
	}
}

void CMenuDu::SetShowPoint(POINT pt)
{
	m_ptPos = pt;
}

//添加菜单项
void CMenuDu::AddMenuItem(char* szName,UINT uMsgID)
{
	TMenuNode tagMenuNode;
	::strcpy(tagMenuNode.szName,szName);
	tagMenuNode.uMsgID = uMsgID;
	m_vtMenu.push_back(tagMenuNode);
}

//添加分隔线
void CMenuDu::AddMenuSeparator()
{
	TMenuNode tagMenuNode;
	tagMenuNode.uMsgID = 0;
	m_vtMenu.push_back(tagMenuNode);
	return;
}

//修正菜单窗口的位置
void CMenuDu::FixPosition()
{
	BzDui::CRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.GetWidth();
	int nHeight = rcWnd.GetHeight();
	rcWnd.left = m_ptPos.x;
	rcWnd.top = m_ptPos.y;
	rcWnd.right = rcWnd.left + nWidth;
	rcWnd.bottom = rcWnd.top + nHeight;
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	BzDui::CRect rcWork = oMonitor.rcWork;

	if( rcWnd.bottom > rcWork.bottom ) {
		if( nHeight >= rcWork.GetHeight() ) {
			rcWnd.top = 0;
			rcWnd.bottom = nHeight;
		}
		else {
			rcWnd.bottom = rcWork.bottom;
			rcWnd.top = rcWnd.bottom - nHeight;
		}
	}
	if( rcWnd.right > rcWork.right ) {
		if( nWidth >= rcWork.GetWidth() ) {
			rcWnd.left = 0;
			rcWnd.right = nWidth;
		}
		else {
			rcWnd.right = rcWork.right;
			rcWnd.left = rcWnd.right - nWidth;
		}
	}
	::SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

}

//加载常规控件皮肤
void CMenuDu::LoadNormalCtrlSkin()
{
	;
}