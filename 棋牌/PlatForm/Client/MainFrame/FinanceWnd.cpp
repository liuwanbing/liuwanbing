#include "StdAfx.h"
#include "FinanceWnd.h"

using namespace BzDui;

#define IDT_SHOWWND_GAP_INC 100

CFinanceWnd::CFinanceWnd(void)
{
	m_iBkTransparent = 100;
}

CFinanceWnd::~CFinanceWnd(void)
{
}

LPCTSTR CFinanceWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CFinanceWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CFinanceWnd::OnFinalMessage(HWND hWnd) 
{ 
	::KillTimer(m_hWnd,IDT_SHOWWND_GAP_INC);
	m_pm.RemovePreMessageFilter(this);
	delete this;
};

LRESULT CFinanceWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("left\\FinanceWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CFinanceWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFinanceWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CFinanceWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFinanceWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFinanceWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}

LRESULT CFinanceWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFinanceWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SHOWWND_GAP_INC)
	{
		;
	}

	return 0;
}

LRESULT CFinanceWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CFinanceWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CFinanceWnd::OnOK()
{
}

void CFinanceWnd::Init() 
{
	LoadNormalCtrlSkin();

	CContainerUI* pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Main"));
	if (pCtnCtrl)
	{
		pCtnCtrl->SetVisible(false);
	}

	CLabelUI* pLabCtrl = NULL;
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Loading"));
	pLabCtrl->SetText("正在获取数据...");

	return;

}

void CFinanceWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		;
	}

	return;
}

//加载常规控件皮肤
void CFinanceWnd::LoadNormalCtrlSkin()
{
}

//显示窗口
void CFinanceWnd::ShowWindow(bool bShow/* = true*/, bool bTakeFocus/* = true*/)
{
// 	if (bShow)
// 	{
// 		::SetTimer(m_hWnd,IDT_SHOWWND_GAP_INC,50,NULL);
// 	}

	__super::ShowWindow(bShow, bTakeFocus);
}


//显示财务数据内容
void CFinanceWnd::ShowFinanceInfo(TMSG_GP_BankFinanceInfo* pFinanceInfo)
{
	CLabelUI* pLabCtrl = NULL;

	if (pFinanceInfo)
	{

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Loading"));
		pLabCtrl->SetVisible(false);

		CContainerUI* pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Main"));
		if (pCtnCtrl)
		{
			pCtnCtrl->SetVisible(true);

			TCHAR szNum[128] = {0};
			TCHAR szTemp[128] = {0};

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Money"));
			if (pLabCtrl)
			{
				__int64 i64Money = pFinanceInfo->i64BankMoney + pFinanceInfo->i64WalletMoney;
				GetNumString(szNum,i64Money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
				_stprintf(szTemp,_T("<b>%s</b>"),szNum);
				pLabCtrl->SetShowHtml(true);
				pLabCtrl->SetText(szTemp);
			}

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Lotteries"));
			if (pLabCtrl)
			{
				_stprintf(szTemp,_T("<b>%d</b>"),pFinanceInfo->iLotteries);
				pLabCtrl->SetShowHtml(true);
				pLabCtrl->SetText(szTemp);
			}
		}
	}
	else
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Loading"));
		pLabCtrl->SetText("数据加载失败。");
		return;
	}

	return;
}