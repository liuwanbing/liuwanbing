#include "StdAfx.h"
#include "IDKeyBoardWnd.h"

using namespace BzDui;

CIDKeyBoardWnd::CIDKeyBoardWnd(void)
{
}

CIDKeyBoardWnd::~CIDKeyBoardWnd(void)
{
}

LPCTSTR CIDKeyBoardWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CIDKeyBoardWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CIDKeyBoardWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CIDKeyBoardWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("logon\\IDCardKeyboard.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CIDKeyBoardWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CIDKeyBoardWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CIDKeyBoardWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CIDKeyBoardWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CIDKeyBoardWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT;
}

LRESULT CIDKeyBoardWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CIDKeyBoardWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CIDKeyBoardWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CIDKeyBoardWnd::OnOK()
{
}

void CIDKeyBoardWnd::Init() 
{
	LoadNormalCtrlSkin();

	unsigned char szKeys[] = {'0','1','2','3','4','5','6','7','8','9','X',0};
	RandomKeys((BYTE*)szKeys,11);

	for (int i = 0; i < 11; ++i)
	{
		TCHAR szBntName[16];
		::sprintf(szBntName,"Btn_Key%d",i);
		m_pBntKey[i] = static_cast<CButtonUI*>(m_pm.FindControl(szBntName));
		if (m_pBntKey[i])
		{
			unsigned char szKeyName[2] = {szKeys[i],0};
			m_pBntKey[i]->SetText((LPCTSTR)szKeyName);
		}
	}

	::memcpy(m_szVKeyTable,szKeys,11);

	
}

//加载Combol的列表项
void CIDKeyBoardWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CIDKeyBoardWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CIDKeyBoardWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if (msg.pSender->GetName() == _TEXT("Btn_Back"))
		{
			::SendMessage(GetParent(m_hWnd),UM_VIRTAUL_KEY,2,0);
			return;
		}

		if (msg.pSender->GetName() == _TEXT("Btn_ReSet"))
		{
			::SendMessage(GetParent(m_hWnd),UM_VIRTAUL_KEY,3,0);
			return;
		}

		for (int i = 0; i < 11; ++i)
		{
			TCHAR szBntName[16];
			::sprintf_s(szBntName,sizeof(szBntName),"Btn_Key%d",i);
			if( msg.pSender->GetName() == szBntName) 
			{
				unsigned long ulParam = 0x00000000L | m_szVKeyTable[i];
				::SendMessage(GetParent(m_hWnd),UM_VIRTAUL_KEY,1,ulParam);
				return;
			}
		}
		
	}
}

//加载常规控件皮肤
void CIDKeyBoardWnd::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	//加载所有文本按钮的皮肤
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ButtonUI"));
	CButtonUI* pBtnCtrl = NULL;
	do 
	{
		pBtnCtrl = static_cast<CButtonUI*>(pCtrls->GetAt(idx++));
		if (pBtnCtrl)
		{
			pBtnCtrl->SetNormalImage("file='dialog\\btn.png' dest='0,0,70,30' source='0,0,69,21' corner='3,3,3,3'");
			pBtnCtrl->SetHotImage("file='dialog\\btn.png' dest='0,0,70,30' source='0,21,69,42' corner='3,3,3,3'");
			pBtnCtrl->SetDisabledImage("file='dialog\\btn.png' dest='0,0,70,30' source='0,84,69,105' corner='3,3,3,3'");
		}
	} while (pBtnCtrl);
}

//产生随机键盘算法
void CIDKeyBoardWnd::RandomKeys(BYTE* pChar,int iNums)
{
	//清空数据源

	///打乱牌
	::srand(::GetTickCount());

	BYTE temp = 255;
	for(int i = 0; i < iNums; ++i)
	{
		temp = ::rand() % (iNums - i) + i;
		char a = pChar[temp];
		pChar[temp] = pChar[i];
		pChar[i] = a;
		//std::swap(pChar[temp],);
	}

	return;
}