#include "StdAfx.h"
#include "..\..\SDK\include\GameLaunchSdk\Platform\TipWnd.h"

using namespace BzDui;

#define IDT_SHOWWND_GAP_INC 100
#define IDT_BUBLE_VALID_TIME 1

CTipWnd::CTipWnd(void)
{
	m_iBkTransparent = 100;
}

CTipWnd::~CTipWnd(void)
{
}

LPCTSTR CTipWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CTipWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CTipWnd::OnFinalMessage(HWND hWnd) 
{ 
	::KillTimer(m_hWnd,IDT_SHOWWND_GAP_INC);
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CTipWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("dialog\\MessageTipWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	SetTimer(m_hWnd,IDT_BUBLE_VALID_TIME,500,NULL);
	return 0;
}

LRESULT CTipWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CTipWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CTipWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CTipWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CTipWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}

LRESULT CTipWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CTipWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SHOWWND_GAP_INC)
	{
		;
	}
	if (wParam == IDT_BUBLE_VALID_TIME)
	{
		KillTimer(m_hWnd,IDT_BUBLE_VALID_TIME);
		Close();
	}
	return 0;
}

LRESULT CTipWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case WM_SIZE:          
			{
				lRes = OnSize(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_TIMER:          
			{
				lRes = OnTimer(uMsg, wParam, lParam, bHandled);
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

LRESULT CTipWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			return true;
		}
	}
	return false;
}

void CTipWnd::OnOK()
{
}

void CTipWnd::Init() 
{
	LoadNormalCtrlSkin();

	m_pMainTxt = static_cast<BzDui::CTextUI*>(m_pm.FindControl("Txt_MainText"));
	return;

}

void CTipWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		Close();
	}

	return;
}

//加载常规控件皮肤
void CTipWnd::LoadNormalCtrlSkin()
{
}

//设置文本
void CTipWnd::SetText(char* szLabelText)
{
	if (m_pMainTxt != NULL)
		m_pMainTxt->SetText(szLabelText);
}

//显示窗口
void CTipWnd::ShowWindow(bool bShow/* = true*/, bool bTakeFocus/* = true*/)
{
// 	if (bShow)
// 	{
// 		::SetTimer(m_hWnd,IDT_SHOWWND_GAP_INC,50,NULL);
// 	}

	__super::ShowWindow(bShow, bTakeFocus);
}