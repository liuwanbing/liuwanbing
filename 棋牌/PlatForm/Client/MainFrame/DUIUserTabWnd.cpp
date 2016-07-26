#include "StdAfx.h"
#include "DUIUserTabWnd.h"
#include "AfcTopWnd.h"

#include "quitask.h"
#include "AfcGamelistWnd.h"
#include <io.h>

using namespace BzDui;

CUserTabDuWnd::CUserTabDuWnd(void)
{
	//老版本的TopWnd
	m_pGameListWnd = NULL;
}

CUserTabDuWnd::~CUserTabDuWnd(void)
{
}

LPCTSTR CUserTabDuWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CUserTabDuWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CUserTabDuWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CUserTabDuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;

	CControlUI* pRoot = builder.Create(_T("tab\\UserTabWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CUserTabDuWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CUserTabDuWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CUserTabDuWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserTabDuWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserTabDuWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CUserTabDuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CUserTabDuWnd::OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CUserTabDuWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

// 	POINTS pt;
// 	pt = MAKEPOINTS(wParam);

// 	HWND hMainFrame = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
// 
// 	RECT rc;
// 	::GetWindowRect(hMainFrame,&rc);
// 	::MoveWindow(hMainFrame,pt.x,pt.y,rc.right - rc.left,rc.bottom - rc.top,true);


	return 0;
}

LRESULT CUserTabDuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case WM_MOVING:
			{
				lRes = OnMoving(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_MOVE:
			{
				lRes = OnMove(uMsg, wParam, lParam, bHandled);
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

LRESULT CUserTabDuWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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
	else if (uMsg == WM_KEYDOWN)
	{
		return true;
	}
	return false;
}

void CUserTabDuWnd::OnOK()
{
}

void CUserTabDuWnd::Init() 
{
	LoadNormalCtrlSkin();

	CButtonUI* pBtnCtrl = NULL;

	CString strTipText;

	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin0.bcf");

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_ChongZhi"));
	strTipText = f.GetKeyVal("GameListWnd","btn01_tip_text","");
	pBtnCtrl->SetToolTip(strTipText.GetBuffer());

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_FacExchange"));
	strTipText = f.GetKeyVal("GameListWnd","btn02_tip_text","");
	pBtnCtrl->SetToolTip(strTipText.GetBuffer());

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_IMList"));
	strTipText = f.GetKeyVal("GameListWnd","btn03_tip_text","");
	pBtnCtrl->SetToolTip(strTipText.GetBuffer());

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_UserInfo"));
	strTipText = f.GetKeyVal("GameListWnd","btn04_tip_text","");
	pBtnCtrl->SetToolTip(strTipText.GetBuffer());

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_MailClick"));
	strTipText = f.GetKeyVal("GameListWnd","btn05_tip_text","");
	pBtnCtrl->SetToolTip(strTipText.GetBuffer());

	RECT rect = {0,0,0,0};
	m_pm.SetCaptionRect(rect);
}

void CUserTabDuWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Btn_FacExchange")) 
		{
			if (m_pGameListWnd)
			{
				m_pGameListWnd->OnFacExchange();
			}
			return;
		}
		else if( msg.pSender->GetName() == _T("Btn_IMList")) 
		{
			if (m_pGameListWnd)
			{
				m_pGameListWnd->OnIMList();
			}
			return;
		}
		else if( msg.pSender->GetName() == _T("Btn_UserInfo")) 
		{
			if (m_pGameListWnd)
			{
				m_pGameListWnd->OnUserInfo();
			}
			return;
		}
		else if( msg.pSender->GetName() == _T("Btn_MailClick")) 
		{
			if (m_pGameListWnd)
			{
				m_pGameListWnd->OnMailClick();
			}
			return;
		}

	}
}

//加载常规控件皮肤
void CUserTabDuWnd::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	//加载所有文本编辑框的皮肤
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("EditUI"));
	CEditUI* pEditUI = NULL;
	do 
	{
		pEditUI = static_cast<CEditUI*>(pCtrls->GetAt(idx++));
		if (pEditUI)
		{
			pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
			//pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
			//pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
		}
	} while (pEditUI);
}

//调整关闭、还原、最小化按钮位置
void CUserTabDuWnd::FixCtrlPosition(UINT uType,bool bMaxSize)
{
	RECT rcPWnd;
	CContainerUI* pContainCtrl = static_cast<CContainerUI*>(m_pm.FindControl("ContainerUI123"));
	int wndWidth = pContainCtrl->GetFixedWidth();
	int wndHeight = pContainCtrl->GetHeight();

	CButtonUI* pBtnCtrl = NULL;
	SIZE si;
	si.cy = 0;

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Close"));
	si.cx = wndWidth - pBtnCtrl->GetWidth();
	if (uType == ID_REST_MAX || (uType == SC_RESTORE && !bMaxSize))
	{
		si.cx -= 58;
	}
	pBtnCtrl->SetFixedXY(si);

// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Restore"));
// 	si.cx -= pBtnCtrl->GetWidth();
// 	pBtnCtrl->SetFixedXY(si);

// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Max"));
// 	pBtnCtrl->SetFixedXY(si);

	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Min"));
	si.cx -= pBtnCtrl->GetWidth();
	pBtnCtrl->SetFixedXY(si);

	return;
}