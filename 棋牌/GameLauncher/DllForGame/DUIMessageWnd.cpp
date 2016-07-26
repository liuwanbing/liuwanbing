#include "StdAfx.h"
#include "DUIMessageWnd.h"

using namespace BzDui;

#define IDT_SHOWWND_GAP_INC 100

CMessageWndDu::CMessageWndDu(void)
{
	m_iBkTransparent = 100;

	//对话框样式
	m_uStyle = 0;
}

CMessageWndDu::~CMessageWndDu(void)
{
}

LPCTSTR CMessageWndDu::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CMessageWndDu::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CMessageWndDu::OnFinalMessage(HWND hWnd) 
{ 
	::KillTimer(m_hWnd,IDT_SHOWWND_GAP_INC);
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CMessageWndDu::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("dialog\\MessageWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	Init();
	return 0;
}

LRESULT CMessageWndDu::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMessageWndDu::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMessageWndDu::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMessageWndDu::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMessageWndDu::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//------------ 鼠标可以 点击可移 上边框 ---------------------- 2012.10.11 yyf
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
	//--------------------------------------------------------------------
	return HTCLIENT;
}

LRESULT CMessageWndDu::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMessageWndDu::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SHOWWND_GAP_INC)
	{
		;
	}

	return 0;
}

LRESULT CMessageWndDu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CMessageWndDu::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			UINT uBtnType = m_uStyle & 0xf;

			if (uBtnType == MB_OK ||
				uBtnType == MB_OKCANCEL)
			{
				m_uRetFlag = IDOK;
				Close();
				return true;
			}

			if (uBtnType == MB_YESNOCANCEL ||
				uBtnType == MB_YESNO)
			{
				m_uRetFlag = IDYES;
				Close();
				return true;
			}

		}
		else if( wParam == VK_ESCAPE ) 
		{
			m_uRetFlag = IDCLOSE;
			Close();
			return true;
		}
	}
	return false;
}

void CMessageWndDu::OnOK()
{
}

void CMessageWndDu::Init() 
{
	LoadNormalCtrlSkin();

	m_pTextMain = static_cast<CTextUI*>(m_pm.FindControl("Text_Main"));

	if (m_pTextMain)
	{
		m_pTextMain->SetShowHtml(true);
	}

	m_pTextTemp = static_cast<CTextUI*>(m_pm.FindControl("Text_Temp"));

	if (m_pTextTemp)
	{
		m_pTextTemp->SetShowHtml(true);
		m_pTextTemp->SetVisible(false);
	}

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Main"));

	if (pLabCtrl)
	{
		pLabCtrl->SetShowHtml(true);
	}

	return;

}

void CMessageWndDu::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if(msg.pSender->GetName() == _T("Bnt_Close")) //关闭按钮
		{
			if (m_uRetFlag == 0)
			{
				m_uRetFlag = IDCLOSE;
			}
			
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_OK"))
		{
			m_uRetFlag = IDOK;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Cancel"))
		{
			m_uRetFlag = IDCANCEL;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Abort"))
		{
			m_uRetFlag = IDABORT;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Retry"))
		{
			m_uRetFlag = IDRETRY;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Ignore"))
		{
			m_uRetFlag = IDIGNORE;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Yes"))
		{
			m_uRetFlag = IDYES;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_No"))
		{
			m_uRetFlag = IDNO;
			Close();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_Continue"))
		{
			m_uRetFlag = IDCONTINUE;
			Close();
			return;
		}
	}
	else if( msg.sType == _T("settext") ) 
	{
		//如果文本内容有修改，重新计算文本的高度，保持文本的垂直居中。
		if (msg.pSender == m_pTextTemp)	 
		{
			m_pTextTemp->SetVisible(false);

			int iNewHeight = msg.wParam;

			SIZE si = {m_pTextTemp->GetFixedXY().cx,
				m_pTextTemp->GetFixedXY().cy + (m_pTextTemp->GetFixedHeight() - iNewHeight) / 2};

			m_pTextMain->SetFixedHeight(iNewHeight);
			m_pTextMain->SetFixedXY(si);

			m_pTextMain->SetText(m_pTextTemp->GetText());
		}

	}

	return;
}

void CMessageWndDu::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}

//加载常规控件皮肤
void CMessageWndDu::LoadNormalCtrlSkin()
{
}

//设置窗口标题
void CMessageWndDu::SetTitle(const char* szText)
{
	CLabelUI* pLabUI = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Title"));

	if (pLabUI)
	{
		pLabUI->SetText(szText);
	}


}

//设置文本
void CMessageWndDu::SetMainText(const char* szText,bool bFormat)
{
	if (bFormat)
	{
		m_pTextTemp->SetVisible(true);
		m_pTextTemp->SetText(szText);
	}
	else
	{
		CLabelUI* pLabUI = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Main"));
		if (pLabUI)
		{
			pLabUI->SetText(szText);
		}
	}

	return;
}

//设置按钮
void CMessageWndDu::SetStyle(UINT uStyle)
{
	CContainerUI* pCtn = NULL;

	UINT uBtnType = uStyle & 0xf;

	if (uBtnType == MB_OK)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_OK"));
	}
	else if(uBtnType == MB_OKCANCEL)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_OkCancel"));
	}
	else if(uBtnType == MB_ABORTRETRYIGNORE)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_AbortRetryIgnore"));
	}
	else if(uBtnType == MB_YESNOCANCEL)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_YesNoCancel"));
	}
	else if(uBtnType == MB_YESNO)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_YesNo"));
	}
	else if(uBtnType == MB_RETRYCANCEL)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_RetryCancel"));
	}
	else if(uBtnType == MB_CANCELTRYCONTINUE)
	{
		pCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_CancelRetryContinue"));
	}

	pCtn->SetVisible(true);

	CControlUI* pLogoImage = m_pm.FindControl("Img_logo");

	UINT uLogoType = uStyle & 0xf0;

	if (uLogoType == MB_ICONINFORMATION)
	{
		pLogoImage->SetBkImage("dialog\\msg_info.png");
	}
	else if (uLogoType == MB_ICONQUESTION)
	{
		pLogoImage->SetBkImage("dialog\\msg_quest.png");
	}
	else if (uLogoType == MB_ICONWARNING)
	{
		pLogoImage->SetBkImage("dialog\\msg_warn.png");
	}
	else if (uLogoType == MB_ICONERROR)
	{
		pLogoImage->SetBkImage("dialog\\msg_error.png");
	}

	m_uStyle = uStyle;

}

//显示窗口
void CMessageWndDu::ShowWindow(bool bShow/* = true*/, bool bTakeFocus/* = true*/)
{
// 	if (bShow)
// 	{
// 		::SetTimer(m_hWnd,IDT_SHOWWND_GAP_INC,50,NULL);
// 	}

	__super::ShowWindow(bShow, bTakeFocus);
}
