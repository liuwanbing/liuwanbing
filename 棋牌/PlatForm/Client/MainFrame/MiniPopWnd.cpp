#include "StdAfx.h"
#include "MiniPopWnd.h"

using namespace BzDui;

#define IDT_TRANSPAENT_INC		1
#define IDT_TRANSPAENT_DEC		2

CMiniPopWnd::CMiniPopWnd(CWnd* pParent)
{
	m_pRoom = pParent;

	m_iTransparent = 0;
}

CMiniPopWnd::~CMiniPopWnd(void)
{
}

LPCTSTR CMiniPopWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CMiniPopWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CMiniPopWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	delete this;
};

LRESULT CMiniPopWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
// 	styleValue &= ~WS_EX_APPWINDOW;
// 	styleValue |= WS_EX_TOOLWINDOW;
	::SetWindowLong(*this, GWL_STYLE,styleValue|WS_CLIPSIBLINGS|WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("main\\MiniPopWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CMiniPopWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMiniPopWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMiniPopWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMiniPopWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMiniPopWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CMiniPopWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMiniPopWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_TRANSPAENT_INC)
	{
		m_iTransparent += 40;
		if (m_iTransparent > 255)
		{
			m_iTransparent = 255;
		}
		m_pm.SetTransparent(m_iTransparent);

		if (m_iTransparent == 255)
		{
			::KillTimer(m_hWnd,IDT_TRANSPAENT_INC);
		}
	}
	else if (wParam == IDT_TRANSPAENT_DEC)
	{
		m_iTransparent -= 40;
		if (m_iTransparent < 80)
		{
			m_iTransparent = 0;
		}
		m_pm.SetTransparent(m_iTransparent);

		if (m_iTransparent == 0)
		{
			::KillTimer(m_hWnd,IDT_TRANSPAENT_DEC);
			Close();
		}
	}
	return 0;
}


LRESULT CMiniPopWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		if (uMsg != WM_PAINT)
		{
			return lRes;
		}

	}

	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		if (uMsg != WM_PAINT)
		{
			return lRes;
		}
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMiniPopWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			m_nRetFlag = 0;
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


void CMiniPopWnd::Init() 
{
	LoadNormalCtrlSkin();

	//加载登陆框顶部显示的网页
	CString szLogAdvUrl;
	szLogAdvUrl.Format("%s%s",Glb().m_CenterServerPara.m_strWebRootADDR,m_tagMailMsg.szMsg);

	CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("IE_UI")));
	if( pActiveXUI ) {
		IWebBrowser2* pWebBrowser = NULL;
		pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
		if( pWebBrowser != NULL ) {

			pWebBrowser->Navigate(CA2W(szLogAdvUrl),NULL,NULL,NULL,NULL);  
			//pWebBrowser->Navigate(L"http://192.168.0.235:8012/Html/Msg/MsgHtml16.html",NULL,NULL,NULL,NULL);  
			pWebBrowser->Release();
		}
	}

	//::SetWindowPos(m_hWnd,CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	m_pm.SetTransparent(0);
	::SetTimer(m_hWnd,IDT_TRANSPAENT_INC,50,NULL);

	RECT rcWnd;
	::GetClientRect(m_hWnd,&rcWnd);

	HRGN hRgn = ::CreateRoundRectRgn(-1,-1,rcWnd.right + 2,rcWnd.bottom + 2,16,16);
	SetWindowRgn(m_hWnd,hRgn,true);
	::DeleteObject(hRgn);

	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	CLabelUI* pLabUI = static_cast<CLabelUI*>(m_pm.FindControl("Lab_UI1"));
	pLabUI->SetText(m_tagMailMsg.szTitle);
	::SetWindowText(m_hWnd,m_tagMailMsg.szTitle);
}

//加载Combol的列表项
void CMiniPopWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
{
	if (!pCtrl || nums <= 0)
	{
		return;
	}

	for (int i = 0; i < nums; ++i)
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(items[i]);
		pCtrl->Add(p);
	}

	return;
}

//根据文本设置选择列表项
bool CMiniPopWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
{
	if (!pCtrl)
	{
		return false;
	}

	int itNums = pCtrl->GetCount();

	if (itNums <= 0)
	{
		return false;
	}

	for (int i = 0; i < itNums; ++i)
	{
		if (strcmp(szText,pCtrl->GetItemAt(i)->GetText().GetData()) == 0)
		{
			pCtrl->SelectItem(i);
			return true;
		}
	}

	return false;
}

//设置弹窗消息
void CMiniPopWnd::SetMessage(const TMailItem* pPopWndMsg)
{
	m_tagMailMsg = *pPopWndMsg;
}


//移动窗口到右下角
void CMiniPopWnd::MoveWindowToRightBottom()
{
	//将窗口移动到屏幕最右下角
	RECT rcWA;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rcWA,0);

// 	int nScrWidth = GetSystemMetrics(SM_CXSCREEN);
// 	int nScrHeight = GetSystemMetrics(SM_CYSCREEN);
	RECT rcWindow;
	::GetWindowRect(m_hWnd,&rcWindow);

	::MoveWindow(m_hWnd,(rcWA.right - rcWA.left) - (rcWindow.right - rcWindow.left),\
		(rcWA.bottom - rcWA.top)  - (rcWindow.bottom - rcWindow.top),
		rcWindow.right - rcWindow.left,\
		rcWindow.bottom - rcWindow.top,\
		true);
	
}


void CMiniPopWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Btn_Close")) 
		{
			::SetTimer(m_hWnd,IDT_TRANSPAENT_DEC,50,NULL);
			//MoveWindowToRightBottom();
		}
	}
}

void CMiniPopWnd::CenterWindow()
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
void CMiniPopWnd::LoadNormalCtrlSkin()
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
			pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
			pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
		}
	} while (pEditUI);

	//加载所有多选框/单选框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("OptionUI"));
	COptionUI* pOptionUI = NULL;
	do 
	{
		pOptionUI = static_cast<COptionUI*>(pCtrls->GetAt(idx++));
		if (pOptionUI)
		{
			if (lstrlen(pOptionUI->GetGroup()) == 0) //多选框
			{
				pOptionUI->SetNormalImage("file='dialog\\check.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\check.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\check.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
				RECT rc = {9,0,0,0};
				pOptionUI->SetTextPadding(rc);
			}
			else  //单选框
			{
				pOptionUI->SetNormalImage("file='dialog\\radio.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\radio.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\radio.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
				RECT rc = {18,0,0,0};
				pOptionUI->SetTextPadding(rc);
			}

		}
	} while (pOptionUI);
}