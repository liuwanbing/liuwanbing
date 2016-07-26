
#include "stdafx.h"
#include "ExpressionWnd.h"

//#include "..\\UILibrary2003\\ConfirmDlg.h"
//#include "quitask.h"
//#include "GamePlace.h"
//#include "AfcGamelistWnd.h"
//#include <io.h>
//#include "GamePlaceDlg.h"

#define EXPRESSION_WIDTH_AND_HEIGHT		22
#define FIRST_EXPRESSION_OFFSET			2
#define EXPRESSION_BUTTON_SIZE			30

using namespace BzDui;

HHOOK CExpressionWnd::m_hMHook  = NULL;
CExpressionWnd* CExpressionWnd::m_pInstance = NULL;


// ////////////////////////////////////////////////////////////////////
// 鼠标勾子
LRESULT CALLBACK ExpressionWndMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 

	if(nCode == HC_ACTION) 
	{ 
		PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT)lParam; 

		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam ==WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)
		{ 
			CExpressionWnd* pExpressionWnd = CExpressionWnd::m_pInstance;

			if (pExpressionWnd)
			{
				// 如果鼠标不点击在本窗口内, 则隐藏该窗口
				RECT rect;
				::GetWindowRect(pExpressionWnd->GetHWND(),&rect);

				if (!::PtInRect(&rect,mhs->pt))
				{
					UnhookWindowsHookEx(CExpressionWnd::m_hMHook);
					pExpressionWnd->Close();
					return 0;
				}
			}
		}
	}

	LRESULT lRet = CallNextHookEx(CExpressionWnd::m_hMHook, nCode, wParam, lParam); 

	return lRet;
}


CExpressionWnd::CExpressionWnd(void)
{
	m_vecExpressionBt.clear();

	CExpressionWnd::m_hMHook = NULL;
	CExpressionWnd::m_pInstance = this;
}

CExpressionWnd::~CExpressionWnd(void)
{
	CExpressionWnd::m_hMHook = NULL;
	CExpressionWnd::m_pInstance = NULL;
}

// void CExpressionWnd::DestroyExpressBtn()
// {
// 	for (int i=0; i<MAX_EXPRESSION_NUM; i++)
// 	{
// 		if (m_vecExpressionBt[i] != NULL)
// 		{
// 			delete m_vecExpressionBt[i];
// 			m_vecExpressionBt[i] = NULL;
// 		}
// 	}
// }
LPCTSTR CExpressionWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CExpressionWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CExpressionWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CExpressionWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);

	CDialogBuilder builder;

	CControlUI* pRoot = builder.Create(_T("dialog\\ExpressionWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	return 0;
}
void CExpressionWnd::MakeExpressionUnvisible()
{
	CContainerUI* pContainer = static_cast<CContainerUI*>(m_pm.FindControl("ContainerUI1"));
	pContainer->SetVisible(false);
	CControlUI* pControler = static_cast<CControlUI*>(m_pm.FindControl("ControlUI1"));
	pControler->SetVisible(false);
	Close();
}
LRESULT CExpressionWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CExpressionWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CExpressionWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CExpressionWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CExpressionWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CExpressionWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CExpressionWnd::OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	CString str;
	PRECT lp = (RECT *)lParam;

	HWND hMainFrame = AfxGetApp()->m_pMainWnd->GetSafeHwnd();

	RECT rc;
	::GetWindowRect(hMainFrame,&rc);
	::MoveWindow(hMainFrame,lp->left,lp->top,rc.right - rc.left,rc.bottom - rc.top,true);
	::MoveWindow(m_hWnd,lp->left,lp->top,rc.right - rc.left,87,true);

	//if (m_pGameListWnd->m_UserTabDuWnd.GetHWND())
	//{
	//	::MoveWindow(m_pGameListWnd->m_UserTabDuWnd.GetHWND(),lp->left + 6 +15,lp->top + 90 + 85,246,31,TRUE);
	//}

	return 0;
}

LRESULT CExpressionWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CExpressionWnd::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	//MakeSkinSelUnVisible();

	return 0;
}

LRESULT CExpressionWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	// 	if (uMsg == WM_MOUSELEAVE)
	// 	{
	// 		CString str;
	// 		str.Format("DClient: CExpressionWnd::HandleMessage uMsg = 0x%04x",uMsg);
	// 		OutputDebugString(str);
	// 	}

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

	case WM_MOUSELEAVE:
		{
			lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled);
		}

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

LRESULT CExpressionWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CExpressionWnd::Init() 
{
	CreateExpressionBtns();

	RECT rect = {0,0,0,0};
	m_pm.SetCaptionRect(rect);

	// 设置鼠标勾子
	m_hMHook = ::SetWindowsHookEx(WH_MOUSE,(HOOKPROC)ExpressionWndMouseHookProc,AfxGetInstanceHandle(),AfxGetThread()->m_nThreadID);
}
//
void CExpressionWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click"))
	{
		if(msg.pSender->GetTag() >= 0 && msg.pSender->GetTag() < MAX_EXPRESSION_NUM) 
		{
			CString strExpression("");
			strExpression.Format("/:%02d",msg.pSender->GetTag());
			SetExpression(strExpression);

			MakeExpressionUnvisible();
			//if (m_parentWnd!=NULL)
			//{
			//	::SendMessage(m_parentWnd->GetHWND(), WM_HIT_EXPMESSTION,0,0);
			//}
		}
	}
	return;
}
void CExpressionWnd::SetParentWnd(BzDui::CWindowWnd *pParent)
{
	m_parentWnd = pParent;
}
CString CExpressionWnd::GetSelExpression()
{
	return m_strSelExpression;
}
void CExpressionWnd::SetExpression(const CString& strExpression)
{
	m_strSelExpression = strExpression;
}
//创建所有表情按钮
void CExpressionWnd::CreateExpressionBtns()
{
	//获取控件位置
	CControlUI *pCtrl = static_cast<CControlUI*>(m_pm.FindControl("ControlUI1"));
	pCtrl->SetVisible(true);
	pCtrl->SetBkColor(0x1000000000);

	//修正第一个表情位置
	RECT rcCtrl = pCtrl->GetPos();
	rcCtrl.left = rcCtrl.left + FIRST_EXPRESSION_OFFSET;
	rcCtrl.top = rcCtrl.top + FIRST_EXPRESSION_OFFSET;

	
	//动态创建所有表情按钮
	TCHAR buf[MAX_PATH];
	CContainerUI* pContainer = static_cast<CContainerUI*>(m_pm.FindControl("ContainerUI1"));
	for (int i=0; i<MAX_EXPRESSION_NUM; i++)
	{
		memset(buf,NULL,MAX_PATH);
		wsprintf(buf,"%s%d","Btn_",i);
		CButtonUI* p = new CButtonUI();
		

		p->SetName(buf);
		p->SetTag(i);
		p->SetFloat(true);

		RECT rc(rcCtrl);
		rc.left += (i % 10) * EXPRESSION_BUTTON_SIZE;
 		rc.top += (i / 10) * EXPRESSION_BUTTON_SIZE;
		SIZE si = {rc.left,rc.top};

		p->SetFixedXY(si);
		p->SetFixedHeight(EXPRESSION_BUTTON_SIZE);
		p->SetFixedWidth(EXPRESSION_BUTTON_SIZE);

		pContainer->Add(p);

		m_vecExpressionBt.push_back(p);
	}
}
