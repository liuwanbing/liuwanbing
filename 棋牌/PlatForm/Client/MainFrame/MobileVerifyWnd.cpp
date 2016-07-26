#include "StdAfx.h"
#include "MobileVerifyWnd.h"
#include "LogonDialog.h"
#include "MainRoomEx.h"

using namespace BzDui;

#define IDT_SET_FOCUS	1
#define IDT_RE_VCODE	2	//重取手机验证码的定时器

#define RE_VCODE_TIME	80	//重取手机验证码的时间

CMobileVerifyWnd::CMobileVerifyWnd(void)
{
	//身份证输入框
	m_pMobileEdt = NULL;

	m_nRetFlag = 0;

	//重取手机验证码的剩余时间
	m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();

	//清理验证码重置
	CMainRoomEx::ClearMobileVCode();
}

CMobileVerifyWnd::~CMobileVerifyWnd(void)
{
}

LPCTSTR CMobileVerifyWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CMobileVerifyWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CMobileVerifyWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CMobileVerifyWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;

	CControlUI* pRoot = builder.Create(_T("logon\\MobileVerify.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CMobileVerifyWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMobileVerifyWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CMobileVerifyWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMobileVerifyWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMobileVerifyWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CMobileVerifyWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMobileVerifyWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SET_FOCUS)
	{
		::KillTimer(m_hWnd,IDT_SET_FOCUS);
		m_pMobileEdt->SetFocus();
	}
	else if (wParam == IDT_RE_VCODE)
	{
		if (m_iReVCodeTime == 0)
		{
			::KillTimer(m_hWnd,IDT_RE_VCODE);
			CMainRoomEx::ClearMobileVCode();

			m_pReVCodeBtn->SetEnabled(true);
			m_pReVCodeBtn->SetText(_T("获取验证码"));
			m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
			return 0;
		}

		char szText[32] = {0};
		_stprintf(szText,"%2d秒后重取",m_iReVCodeTime--);
		m_pReVCodeBtn->SetText(szText);
	}

	return 0;
}


LRESULT CMobileVerifyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CMobileVerifyWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			OnOK();
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

void CMobileVerifyWnd::OnOK()
{
	CStdString strInpVCode = m_pVCodeEdt->GetText();
	if (strInpVCode.GetLength() == 0)
	{
		//AFCMessageBox("请填写验证码");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请填写验证码");
		m_pVCodeEdt->SetFocus();
		return;
	}

	MD5_CTX Md5;
	char szMD5Input[36];
	::memcpy_s(szMD5Input,sizeof(szMD5Input),m_pMobileEdt->GetText().GetData(),32);
	Md5.MD5Update((unsigned char*)szMD5Input,lstrlen(szMD5Input));
	unsigned char szMDTemp[16];
	Md5.MD5Final(szMDTemp);
	for (int i=0;i<16;i++) 
	{
		wsprintf(&szMD5Input[i*2],"%02x",szMDTemp[i]);
	}

	if (!CMainRoomEx::ValidMobileVCode((char*)strInpVCode.GetData()))
	{
		//AFCMessageBox("验证码不正确");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","验证码不正确");
		m_pVCodeEdt->SetFocus();
		return;
	}

	::lstrcpy(CLogonDialog::m_szMobileVCode,strInpVCode.GetData());

	m_nRetFlag = 1;
	Close();

	return;

}

void CMobileVerifyWnd::Init() 
{
	LoadNormalCtrlSkin();

	m_pMobileEdt = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_MobileNo")));
	m_pMobileEdt->SetEnabled(false);
	if (m_pVerifyInfo)
	{
		m_pMobileEdt->SetText(m_pVerifyInfo->szMobileNo);
	}

	//重取手机验证按钮
	m_pReVCodeBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_GetVCode")));
	
	m_pVCodeEdt = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_VCode")));
	m_pVCodeEdt->SetText(_T(""));
	m_pVCodeEdt->SetMaxChar(6);

	::SetTimer(m_hWnd,IDT_SET_FOCUS,500,NULL);
	
}

//加载Combol的列表项
void CMobileVerifyWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CMobileVerifyWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CMobileVerifyWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Bnt_Close")) 
		{
			m_nRetFlag = 0;
			Close();
		}
		if( msg.pSender->GetName() == _T("Btn_OK")) 
		{
			OnOK();
		}
		if( msg.pSender->GetName() == _T("Btn_Cancel")) 
		{
			m_nRetFlag = 0;
			Close();
		}
		if (msg.pSender->GetName() == _T("Btn_GetVCode"))
		{
			m_pVCodeEdt->SetText(_T(""));
			m_pVCodeEdt->SetFocus();
			GetMainRoom()->OnSendMobileVCodeSms(m_pVerifyInfo->szMobileNo,0);

			CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(msg.pSender);
			pBtnCtrl->SetEnabled(false);
			pBtnCtrl->SetText("获取...");

			::SetTimer(m_hWnd,IDT_RE_VCODE,1000,NULL);
		}
	}
}

void CMobileVerifyWnd::CenterWindow()
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
void CMobileVerifyWnd::LoadNormalCtrlSkin()
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

//清理所有重取验证码的定时器
void CMobileVerifyWnd::ResetReVCodeUI()
{
	//if (m_iReVCodeTime < CMobileVerifyWnd::GetVCodeValidTime();)
	if (m_hWnd != NULL)
	{
		::KillTimer(m_hWnd,IDT_RE_VCODE);
		CMainRoomEx::ClearMobileVCode();

		m_pReVCodeBtn->SetEnabled(true);
		m_pReVCodeBtn->SetText(_T("获取验证码"));
		m_iReVCodeTime = CMobileVerifyWnd::GetVCodeValidTime();
	}
}


//获取验证码的有效时间
int CMobileVerifyWnd::GetVCodeValidTime()
{
	CBcfFile f("bzgame.bcf");

	return f.GetKeyVal("MobileVerify","ValidTime",300);

}