#include "StdAfx.h"
#include "LogoChangeDlg.h"
#include "commonuse.h"
using namespace BzDui;

CLogoChangeDlg::CLogoChangeDlg(void)
{
	CBcfFile fLogoFile(CBcfFile::GetAppPath()+_T("bzgame.bcf"));

	CString strKey= _T("LogoSet");
	m_iColNumMax = fLogoFile.GetKeyVal(strKey, _T("EachRowNum"), 5);
	m_iSpace = fLogoFile.GetKeyVal(strKey, _T("LogoSpace"), 2);
	m_iBtnWidth=fLogoFile.GetKeyVal(strKey, _T("LogoBtnWidth"), 40);
	m_iBtnHeight=fLogoFile.GetKeyVal(strKey, _T("LogoBtnHeight"), 40);
	m_pLogoFrame = NULL;
	m_iIndex = -1;
}

CLogoChangeDlg::~CLogoChangeDlg(void)
{
}

LPCTSTR CLogoChangeDlg::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CLogoChangeDlg::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};
 
 void CLogoChangeDlg::OnFinalMessage(HWND hWnd) 
 {
 	m_pm.RemovePreMessageFilter(this);
 	delete this;
 };

LRESULT CLogoChangeDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("..\\..\\log\\DeskLogos\\LogoChange.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

void CLogoChangeDlg::Init()
{
	CControlUI *pContain = static_cast<CControlUI*>(m_pm.FindControl(_T("Container1")));
	if (pContain)
	{
		pContain->SetBkImage(_T("..\\..\\log\\DeskLogos\\ConBk.png"));
	}
	m_pLogoFrame = static_cast<CControlUI*>(m_pm.FindControl(_T("Frame")));
	if (m_pLogoFrame)
	{
		m_pLogoFrame->SetBkImage(_T("..\\..\\log\\DeskLogos\\LogoBk.png"));
	}
	//获取所有的控件
	CStdPtrArray* pCtnS = m_pm.FindSubControlsByClass(m_pm.GetRoot(), _T("ButtonUI"));
	int idx = 0;
	int iButtonCount=0;
	RECT rc;
	TCHAR szPath[MAX_PATH];
	CButtonUI *pBtn = NULL;
	CString strData;
	do
	{
		pBtn = static_cast<CButtonUI*>(pCtnS->GetAt(idx++));
		if (pBtn)
		{
			iButtonCount++;
			pBtn->SetFixedWidth(m_iBtnWidth);
			pBtn->SetFixedHeight(m_iBtnHeight);
			strData.Format(_T("%d"), idx);
			memset(szPath, 0, sizeof(szPath));
			wsprintf(szPath, _T("..\\..\\log\\DeskLogos\\Logo_Small_%d.png"), idx);
			rc.left = ((iButtonCount-1)%m_iColNumMax)*(m_iBtnWidth+m_iSpace) + m_iSpace;
			rc.top = (iButtonCount-1)/m_iColNumMax*(m_iBtnHeight+m_iSpace) + m_iSpace;
			rc.right = rc.left+m_iBtnWidth;
			rc.bottom = rc.top+m_iBtnHeight;
			pBtn->SetPos(rc);
			pBtn->SetBkImage(szPath);
			pBtn->SetUserData(strData);//获取的名字格式:Logo_0，则设置该值为0，以此为索引值
			if (m_pLogoFrame && m_iIndex == idx)
			{
				RECT rc = pBtn->GetPos();
				rc.left-=m_iSpace;	rc.top-=m_iSpace;	rc.right+=m_iSpace;	rc.bottom+=m_iSpace;
				m_pLogoFrame->SetPos(rc);
				m_pLogoFrame->SetVisible(true);
			}
		}
	}while(pBtn);
	if (iButtonCount%m_iColNumMax==0)
		this->ResizeClient((m_iBtnWidth+m_iSpace)*m_iColNumMax+m_iSpace*2, (m_iBtnHeight+m_iSpace)*(iButtonCount/m_iColNumMax)+m_iSpace*2);
	else
		this->ResizeClient((m_iBtnWidth+m_iSpace)*m_iColNumMax+m_iSpace*2, (m_iBtnHeight+m_iSpace)*(iButtonCount/m_iColNumMax+1)+m_iSpace*2);
	return;
}

void CLogoChangeDlg::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if (_tcscmp(msg.pSender->GetClass(), _T("ButtonUI")) == 0 )
		{
			m_iIndex = atoi(msg.pSender->GetUserData());
			if (m_iIndex != -1)
			{
				::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),MSG_SET_LOGO,m_iIndex,0);
			}
		}
	}
	return;
}

LRESULT CLogoChangeDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CLogoChangeDlg::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CLogoChangeDlg::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLogoChangeDlg::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLogoChangeDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	case WM_KILLFOCUS:
		{
			this->ShowWindow(SW_HIDE);
			lRes = TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		{			
			lRes = OnMouseMove(uMsg, wParam, lParam, bHandled);
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

LRESULT CLogoChangeDlg::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

LRESULT CLogoChangeDlg::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; 
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);

	CControlUI *pTemp = m_pm.FindControl(pt);
	if (pTemp && _tcscmp(pTemp->GetClass(), _T("ButtonUI")) == 0)
	{
		if (m_pLogoFrame)
		{
			int iTemp = m_iSpace;
			RECT rc = pTemp->GetPos();
			rc.left-=iTemp;	rc.top-=iTemp;	rc.right+=iTemp;	rc.bottom+=iTemp;
			m_pLogoFrame->SetPos(rc);
			m_pLogoFrame->SetVisible(true);
			return TRUE;
		}
	}	

	if (m_pLogoFrame)
	{
		m_pLogoFrame->SetVisible(false);
	}
	return FALSE;
}