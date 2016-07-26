#include "StdAfx.h"
#include "GameTrottingWnd.h"
using namespace BzDui;

#define IDT_SET_FOCUS	1

CGameTrottingWnd::CGameTrottingWnd(CWnd* pParent)
{


}

CGameTrottingWnd::~CGameTrottingWnd(void)
{
}

LPCTSTR CGameTrottingWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CGameTrottingWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CGameTrottingWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CGameTrottingWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("..\\skin1\\game\\Trotting.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	m_pm.SetTransparent(180);

	Init();
	return 0;
}

LRESULT CGameTrottingWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CGameTrottingWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CGameTrottingWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CGameTrottingWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CGameTrottingWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CGameTrottingWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CGameTrottingWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam) {
		case IDT_PMD:
			{
				::KillTimer(m_hWnd, IDT_PMD);
				::KillTimer(m_hWnd, IDT_PIECE);
				ShowWindow(SW_HIDE);
				break;
			}
			break;
		case IDT_PIECE:
			{
				if (m_bDir) {
					m_LabelPos.x -= 1;
					RECT rect;
					rect.left	= m_LabelPos.x;
					rect.top	= m_LabelPos.y;
					rect.right	= m_LabelPos.x + (m_LabelRect.right - m_LabelRect.left);
					rect.bottom = m_LabelPos.y + (m_LabelRect.bottom - m_LabelRect.top);

					if (rect.right < 0)
						m_LabelPos.x = 516;
					m_pLabel->SetPos(rect);
				}
				else {
					m_LabelPos.x += 1;
					RECT rect;
					rect.left	= m_LabelPos.x;
					rect.top	= m_LabelPos.y;
					rect.right	= m_LabelPos.x + (m_LabelRect.right - m_LabelRect.left);
					rect.bottom	= m_LabelPos.y + (m_LabelRect.bottom - m_LabelRect.top);
					if (rect.left >= 516)
						m_LabelPos.x = m_LabelRect.left;
					m_pLabel->SetPos(rect);
				}
			}
			break;
	}
	return 0;
}


LRESULT CGameTrottingWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case WM_MOVE:
			{
				MoveWindow(m_hWnd, m_AdjustPos.x, m_AdjustPos.y + ADJUST, LENGTH, HEIGHT, true);
				SetWindowPos(m_hWnd, HWND_TOP, m_AdjustPos.x, m_AdjustPos.y + ADJUST, LENGTH, HEIGHT, SWP_NOMOVE);
				lRes = 0;
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

LRESULT CGameTrottingWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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


void CGameTrottingWnd::Init() 
{
	LoadNormalCtrlSkin();
}

//加载Combol的列表项
void CGameTrottingWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CGameTrottingWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CGameTrottingWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
	}
}

void CGameTrottingWnd::CenterWindow()
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
void CGameTrottingWnd::LoadNormalCtrlSkin()
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

void CGameTrottingWnd::init(HWND hwndParent) 
{
	if (!hwndParent) return ;
	Create(hwndParent, NULL, UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	m_pLabel = static_cast<BzDui::CLabelUI*>(m_pm.FindControl("Lab_Content"));
	if (m_pLabel) {
		RECT rect;
		rect.left	= 0;
		rect.top	= 0;
		rect.right	= LENGTH;
		rect.bottom = HEIGHT;
		m_LabelRect = rect;
		m_pLabel->SetPos(rect);
	}
}

void CGameTrottingWnd::AdjustPos(HWND hWnd)
{
	RECT ParentRect, ChildRect;
	POINT AdjustPos;

	GetClientRect(hWnd, &ParentRect);
	POINT MidPoint;
	MidPoint.x = (ParentRect.left + ParentRect.right)/2;
	MidPoint.y = ParentRect.top;
	ClientToScreen(hWnd, &MidPoint);

	GetWindowRect(this->GetHWND(), &ChildRect);
	int iLength = ChildRect.right - ChildRect.left;
	int iHeight = ChildRect.bottom - ChildRect.top;

	AdjustPos.x	= MidPoint.x - iLength/2;
	AdjustPos.y	= MidPoint.y;

	m_AdjustPos = AdjustPos;

	MoveWindow(m_hWnd, AdjustPos.x, AdjustPos.y + ADJUST, LENGTH, HEIGHT, true);
	SetWindowPos(m_hWnd, HWND_TOP, AdjustPos.x, AdjustPos.y + ADJUST, LENGTH, HEIGHT, SWP_NOMOVE);
}

void CGameTrottingWnd::ShowCtrl(bool bShow/* =true */)
{
	if (bShow) {
		ShowWindow(SW_SHOW);
	}
	else {
		ShowWindow(SW_HIDE);
	}
}

void CGameTrottingWnd::StartPmdLabel(int tmLen, char *szText, int tmSpeed/* =200 */, bool bDir/* =true */)
{
	if (tmLen < 0 || (!szText) || (tmSpeed < 0)) return ;
	::memset(m_Text, 0, TEXT_SIZE * sizeof(char));
	::memcpy(m_Text, szText, strlen(szText));
	::SetTimer(m_hWnd, IDT_PMD, tmLen, NULL);
	::SetTimer(m_hWnd, IDT_PIECE, tmSpeed, NULL);
	
	char buf[BUF_SIZE];
	::memset(buf, 0, BUF_SIZE * sizeof(char));
	sprintf(buf, "<f %s %d>%s</f>", m_szFont, m_iSize, m_Text);
	SIZE size;
	m_pLabel->SetText(buf);
	
	HDC hdc = ::GetDC(m_hWnd);
	LOGFONT font;
	font.lfHeight = m_iSize;
	_tcscpy(TEXT(font.lfFaceName), m_szFont);
	HFONT hfont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	hfont = ::CreateFontIndirect(&font);
	HFONT hOldfont = (HFONT)::SelectObject(hdc, (HGDIOBJ)hfont);
	::GetTextExtentPoint32(hdc, szText, strlen(szText), &size);
	::SelectObject(hdc, hOldfont);
	::ReleaseDC(m_hWnd, hdc);

	m_bDir = bDir;

	if (bDir) {
		if (m_pLabel) {
			m_LabelRect.left	= 516;
			m_LabelRect.top		= 0;
			m_LabelRect.right	= m_LabelRect.left + size.cx;
			m_LabelRect.bottom	= m_LabelRect.top + 24;
			m_LabelPos.x		= m_LabelRect.left;
			m_LabelPos.y		= m_LabelRect.top;
			m_pLabel->SetPos(m_LabelRect);
		}
	}
	else {
		if (m_pLabel) {
			char *pTempText = new char [strlen(m_Text)];
			::memset(pTempText, 0, sizeof(pTempText));
			
			wchar_t *pWTempText = new wchar_t[strlen(m_Text)];
			::memset(pWTempText, 0, sizeof(pWTempText));
			char2Wchar(m_Text, pWTempText);

			int i = 0;
			int j = 0;
			for (i = 0, j = wcslen(pWTempText)-1; i < j; i ++, j --) {
				wchar_t temp	= pWTempText[i];
				pWTempText[i]	= pWTempText[j];
				pWTempText[j]	= temp;
			}
			wchar2char(pWTempText, pTempText);

			::memset(buf, 0, BUF_SIZE * sizeof(char));
			sprintf(buf, "<f %s %d>%s</f>", m_szFont, m_iSize, pTempText);
			m_pLabel->SetText(buf);
			
			m_LabelRect.right	= 0;
			m_LabelRect.top		= 0;
			m_LabelRect.bottom	= m_LabelRect.top + 24;
			m_LabelRect.left	= m_LabelRect.right - size.cx;
			m_LabelPos.x		= m_LabelRect.left;
			m_LabelPos.y		= m_LabelRect.top;
			m_pLabel->SetPos(m_LabelRect);
		}
	}
}

void CGameTrottingWnd::SetLabelTextFont(char *szFont, int iSize)
{
	if (!szFont || iSize < 0) return ;
	::memset(m_szFont, 0, 32);
	::memcpy(m_szFont, szFont, strlen(szFont));
	
	m_iSize = iSize;
} 

//多字节编码转换成宽字节编码
void char2Wchar(char* pchar,wchar_t* wchar)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

//宽字节编码转换成多字节编码
void wchar2char(wchar_t* wchar,char* pchar)
{
	int len = WideCharToMultiByte(CP_ACP,0,wchar,(int)wcslen(wchar),NULL,0,NULL,NULL);   
	WideCharToMultiByte(CP_ACP,0,wchar,(int)wcslen(wchar),pchar,len,NULL,NULL);
}
