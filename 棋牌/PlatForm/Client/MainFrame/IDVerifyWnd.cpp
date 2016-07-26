#include "StdAfx.h"
#include "IDVerifyWnd.h"
#include "LogonDialog.h"

using namespace BzDui;

#define IDT_SET_FOCUS	1

CIDVerifyWnd::CIDVerifyWnd(void)
{
	//身份证输入框
	m_pIDCardEdt = NULL;

	m_pVerifyInfo = NULL;

	m_nRetFlag = 0;

	//数据键盘窗口
	m_pIDKeyBoardWnd = NULL;

	//验证码生成器
	m_pVCCreater = NULL;
}

CIDVerifyWnd::~CIDVerifyWnd(void)
{
	//释放数据键盘窗口
	delete m_pIDKeyBoardWnd;
	m_pIDKeyBoardWnd = NULL;
}

LPCTSTR CIDVerifyWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CIDVerifyWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CIDVerifyWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CIDVerifyWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;

	CControlUI* pRoot = builder.Create(_T("logon\\IDVerify.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	m_pIDKeyBoardWnd = new CIDKeyBoardWnd();

	if(m_pIDKeyBoardWnd)
	{
		m_pIDKeyBoardWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	}

	Init();
	return 0;
}

LRESULT CIDVerifyWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CIDVerifyWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CIDVerifyWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CIDVerifyWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pIDKeyBoardWnd)
	{
		SIZE sizeEdt = m_pIDCardEdt->GetFixedXY();
		RECT rcTop;
		RECT rcKb;
		::GetWindowRect(GetHWND(),&rcTop);
		::GetWindowRect(m_pIDKeyBoardWnd->GetHWND(),&rcKb);
		int iW = rcKb.right - rcKb.left;
		int iH = rcKb.bottom - rcKb.top;
		int iL = rcTop.left + sizeEdt.cx + m_pIDCardEdt->GetWidth() + 3;
		int iT = rcTop.top + sizeEdt.cy - 5;

		::MoveWindow(m_pIDKeyBoardWnd->GetHWND(),iL,iT,iW,iH,true);
		//m_pIDKeyBoardWnd->ShowWindow(false);
	}

	return 0;
}

LRESULT CIDVerifyWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CIDVerifyWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CIDVerifyWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SET_FOCUS)
	{
		::KillTimer(m_hWnd,IDT_SET_FOCUS);
		m_pIDCardEdt->SetFocus();
	}

	return 0;
}


LRESULT CIDVerifyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case UM_VIRTAUL_KEY:
			{
				lRes = OnVirtualKey((unsigned char)lParam,(unsigned int)wParam);
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

	if (uMsg == WM_PAINT)
	{
		if (m_pVCCreater)
		{
			m_pVCCreater->Draw(155,225);
		}
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CIDVerifyWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CIDVerifyWnd::OnOK()
{
	if (m_pIDCardEdt->GetText().GetLength() == 0)
	{
		//AFCMessageBox("请填写身份证号");
	     DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请填写身份证号");
		m_pIDCardEdt->SetFocus();
		return;
	}

	if (m_pVCodeEdt->GetText().GetLength() == 0)
	{
		//AFCMessageBox("请填写验证码");
	     DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请填写验证码");
		m_pIDCardEdt->SetFocus();
		return;
	}

	if (!m_pVCCreater)
	{
		return;
	}

	if (!m_pVCCreater->IsVerify(m_pVCodeEdt->GetText()))
	{
		//AFCMessageBox("验证码不正确");
	     DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","验证码不正确");
		m_pVCodeEdt->SetFocus();
		return;
	}

	MD5_CTX Md5;
	char szMD5Input[36];
	::memcpy_s(szMD5Input,sizeof(szMD5Input),m_pIDCardEdt->GetText().GetData(),32);
	Md5.MD5Update((unsigned char*)szMD5Input,lstrlen(szMD5Input));
	unsigned char szMDTemp[16];
	Md5.MD5Final(szMDTemp);
	for (int i=0;i<16;i++) 
	{
		wsprintf(&szMD5Input[i*2],"%02x",szMDTemp[i]);
	}

	if (strcmp((const char*)szMD5Input,m_pVerifyInfo->szMD5IDCardNo) == 0)
	{
		//设置数据
		lstrcpy(CLogonDialog::m_szIDCard,m_pIDCardEdt->GetText().GetData());
		m_nRetFlag = 1;
		Close();
		return;
	}
	else
	{
		//AFCMessageBox("身份证号不对，请再输入");
	     DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","身份证号不对，请再输入");
		m_pIDCardEdt->SetFocus();
	}

}

//虚拟键盘按键
LRESULT CIDVerifyWnd::OnVirtualKey(UCHAR uc,UINT nType)
{
	unsigned char szID[32];
	strcpy((char*)szID,m_pIDCardEdt->GetText());
	int len = strlen((const char*)szID);
	unsigned char* p = szID + len;

	if (nType == 1) //字符输入
	{
		if (len > 18)
		{
			return false;
		}
		*p = uc;
		*(++p) = 0;
		m_pIDCardEdt->SetText((LPCTSTR)szID);
	}
	else if (nType == 2) //退格
	{
		if (len == 0)
		{
			return false;
		}
		*(--p) = 0;
		m_pIDCardEdt->SetText((LPCTSTR)szID);
	}
	else if (nType == 3) //重置
	{
		m_pIDCardEdt->SetText(_T(""));
	}


	return true;
}

void CIDVerifyWnd::Init() 
{
	LoadNormalCtrlSkin();

	m_pIDCardEdt = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_IDCardNo")));
	m_pIDCardEdt->SetMaxChar(20);
	m_pVCodeEdt = static_cast<CEditUI*>(m_pm.FindControl(_T("Edt_VCode")));
	m_pVCodeEdt->SetMaxChar(4);

	m_pIDKeyBoardWnd->ShowWindow(false);

	//获得验证码控制对象
	m_pVCCreater = CVerifyCodeCreater::GetInstance();
	m_pVCCreater->SetParent(m_hWnd);
	m_pVCCreater->ReCreateBMP(130,37,RGB(0xEC,0xF6,0xFF));

	::SetTimer(m_hWnd,IDT_SET_FOCUS,500,NULL);
	
}

//加载Combol的列表项
void CIDVerifyWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CIDVerifyWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CIDVerifyWnd::Notify(TNotifyUI& msg)
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
		if( msg.pSender->GetName() == _T("Btn_UseVKeyBorad")) 
		{
			if(m_pIDKeyBoardWnd)
			{
				SIZE sizeEdt = m_pIDCardEdt->GetFixedXY();
				RECT rcTop;
				RECT rcKb;
				::GetWindowRect(GetHWND(),&rcTop);
				::GetWindowRect(m_pIDKeyBoardWnd->GetHWND(),&rcKb);
				int iW = rcKb.right - rcKb.left;
				int iH = rcKb.bottom - rcKb.top;
				int iL = rcTop.left + sizeEdt.cx + m_pIDCardEdt->GetWidth() + 3;
				int iT = rcTop.top + sizeEdt.cy - 5;

				::MoveWindow(m_pIDKeyBoardWnd->GetHWND(),iL,iT,iW,iH,true);
				m_pIDKeyBoardWnd->ShowWindow(true);

			}
		}
		if ( msg.pSender->GetName() == _T("Bnt_Change"))
		{
			if (m_pVCCreater)
			{
				m_pVCCreater->ReCreateBMP(130,37,RGB(0xEC,0xF6,0xFF));
				m_pVCCreater->Draw(155,225);
			}
		}
	}
	else if (msg.sType == _T("setfocus"))
	{
		m_pIDKeyBoardWnd->ShowWindow(false);
	}
}

void CIDVerifyWnd::CenterWindow()
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
void CIDVerifyWnd::LoadNormalCtrlSkin()
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