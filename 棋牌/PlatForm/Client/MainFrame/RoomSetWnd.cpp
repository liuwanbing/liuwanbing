#include "StdAfx.h"
#include "RoomSetWnd.h"

using namespace BzDui;
bool	CRoomSetWnd::m_bPlayBkMusic = true;
CRoomSetWnd::CRoomSetWnd(void)
{	

	//是否允许设置桌面密码
	m_bAllowSetDesk = true;

	m_szPass[0]=0;
	m_bCutPercent=10;
	m_bPass=FALSE;
	m_bSameIP=false;
	m_bSameIPNumber=0;
	m_bLimitCut=FALSE;
	m_bLimitPoint=0x00;
	m_bInvaliMode=INVALI_ALL;
	m_dwLowPoint=-100000L;
	m_dwHighPoint=100000L;

	m_dwLowMoney=-100000L;
	m_dwHighMoney=100000L;

	m_bSaveTalk=TRUE;
	m_bShowUser=TRUE;
	m_bShowMessage=TRUE;
}

CRoomSetWnd::~CRoomSetWnd(void)
{
}

LPCTSTR CRoomSetWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CRoomSetWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CRoomSetWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CRoomSetWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("room_mid\\RoomSet\\RoomSet.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	Init();
	return 0;
}

LRESULT CRoomSetWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CRoomSetWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CRoomSetWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRoomSetWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRoomSetWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CRoomSetWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRoomSetWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CRoomSetWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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
	return false;
}

void CRoomSetWnd::OnOK()
{

	m_bLimitPoint = 0x00;

	COptionUI* pOptCtrl = NULL;
	CComboUI* pCobCtrl = NULL;
	CEditUI* pEdtCrl = NULL;
	TCHAR sz[32];

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_1"));
	m_bSameIP = pOptCtrl->IsSelected();

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_1"));
	LPCTSTR lpSel = pCobCtrl->GetText().GetData();
	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_2"));
	if (pOptCtrl->IsSelected())
	{
		::sscanf_s(lpSel,"%d",&m_bSameIPNumber);
	}
	else
	{
		m_bSameIPNumber = 0;
	}


	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_3"));
	m_bLimitCut = pOptCtrl->IsSelected();

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_2"));
	::sscanf_s(pCobCtrl->GetText().GetData(),"%d",&m_bCutPercent);

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_4"));
	if (pOptCtrl->IsSelected())
	{
		m_bLimitPoint |= 0x40;
	}

	pEdtCrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_2"));
	::sscanf_s(pEdtCrl->GetText().GetData(),"%d",&m_dwLowPoint);

	pEdtCrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_3"));
	::sscanf_s(pEdtCrl->GetText().GetData(),"%d",&m_dwHighPoint);


	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_8"));
	if (pOptCtrl->IsSelected())
	{
		m_bLimitPoint |= 0x80;
	}

	pEdtCrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_5"));
	::sscanf_s(pEdtCrl->GetText().GetData(),"%d",&m_dwLowMoney);

	pEdtCrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_6"));
	::sscanf_s(pEdtCrl->GetText().GetData(),"%d",&m_dwHighMoney);


	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_5"));
	m_bShowUser = pOptCtrl->IsSelected();

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_6"));
	m_bPass = pOptCtrl->IsSelected();

	pEdtCrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_4"));
	strcpy(m_szPass,pEdtCrl->GetText());

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_7"));
	m_bPlayBkMusic = pOptCtrl->IsSelected();

	m_nRetFlag = 1;
	Close();
}

void CRoomSetWnd::Init() 
{
	LoadNormalCtrlSkin();

	COptionUI* pOptCtrl = NULL;
	CComboUI* pCobCtrl = NULL;
	CEditUI* pEdtCtrl = NULL;
	TCHAR sz[128];

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_1"));
	pOptCtrl->Selected(m_bSameIP);
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_2"));
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_1"));
	LPCTSTR szItems1[2] = {_T("3"),_T("4")};
	LoadComboItems(pCobCtrl,szItems1,2);
	if (m_bSameIPNumber > 0) 
	{
		::sprintf_s(sz,sizeof(sz),"%d",m_bSameIPNumber);
		SelectComboItem(pCobCtrl,sz);
		pOptCtrl->Selected(true);
		pCobCtrl->SetEnabled(true);
	}
	else
	{
		SelectComboItem(pCobCtrl,szItems1[0]);
		pOptCtrl->Selected(false);
		pCobCtrl->SetEnabled(false);
	}

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_3"));
	pOptCtrl->Selected(m_bLimitCut);
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_2"));
	LPCTSTR szItems2[9] = {_T("10"),_T("20"),_T("30"),_T("40"),_T("50"),_T("60"),_T("70"),_T("80"),_T("90")};
	LoadComboItems(pCobCtrl,szItems2,9);
	::sprintf_s(sz,sizeof(sz),"%d",m_bCutPercent);
	SelectComboItem(pCobCtrl,sz);
	pCobCtrl->SetEnabled(m_bLimitCut);

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_4"));
	pOptCtrl->Selected(m_bLimitPoint & 0x40);
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_8"));
	pOptCtrl->Selected(m_bLimitPoint & 0x80);
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_5"));
	pOptCtrl->Selected(m_bShowUser);

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_6"));
	pOptCtrl->Selected(m_bPass);
	if (!m_bAllowSetDesk)
	{
		pOptCtrl->SetEnabled(false);
		pOptCtrl->SetVisible(false);
	}

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_2"));
	::sprintf_s(sz,sizeof(sz),"%d",m_dwLowPoint);
	pEdtCtrl->SetText(sz);
	pEdtCtrl->SetEnabled(m_bLimitPoint & 0x40);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_3"));
	::sprintf_s(sz,sizeof(sz),"%d",m_dwHighPoint);
	pEdtCtrl->SetText(sz);
	pEdtCtrl->SetEnabled(m_bLimitPoint & 0x40);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_5"));
	::sprintf_s(sz,sizeof(sz),"%d",m_dwLowMoney);
	pEdtCtrl->SetText(sz);
	pEdtCtrl->SetEnabled(m_bLimitPoint & 0x80);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_6"));
	::sprintf_s(sz,sizeof(sz),"%d",m_dwHighMoney);
	pEdtCtrl->SetText(sz);
	pEdtCtrl->SetEnabled(m_bLimitPoint & 0x80);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_4"));
	pEdtCtrl->SetText(m_szPass);
	pEdtCtrl->SetEnabled(m_bPass);

	if (!m_bAllowSetDesk)
	{
		pEdtCtrl->SetEnabled(false);
	}

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_7"));
	pOptCtrl->Selected(m_bPlayBkMusic);

	//限制IP
	/*if(Glb().m_release)
	{
		m_bSameIP=true;
		((CButton *)GetDlgItem(IDC_SAME_IP))->SetCheck(BST_CHECKED);
		((CButton *)GetDlgItem(IDC_SAME_IP))->EnableWindow(0);
	}*/
	//设置邀请方式
// 	if (m_bInvaliMode==INVALI_ALL) ((CButton *)GetDlgItem(IDC_INVALI_ALL))->SetCheck(BST_CHECKED);
// 	else if (m_bInvaliMode==INVALI_ONLY_FRIEND) ((CButton *)GetDlgItem(IDC_FRIEND_INVALI))->SetCheck(BST_CHECKED);
// 	else ((CButton *)GetDlgItem(IDC_NO_INVALI))->SetCheck(BST_CHECKED);
// 
// 	//设置输入
// 	if (m_bSaveTalk) ((CButton *)GetDlgItem(IDC_SAVE_TALK))->SetCheck(BST_CHECKED);
// 	if (m_bShowUser) ((CButton *)GetDlgItem(IDC_SHOW_USER))->SetCheck(BST_CHECKED);
	
//	int r,g,b;

}

//加载Combol的列表项
void CRoomSetWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CRoomSetWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CRoomSetWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
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
	}
	else if(msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == _T("Opt_2"))
		{
			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_2"));
			CComboUI* pCobCtrl = NULL;
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_1"));
			pCobCtrl->SetEnabled(pOptCtrl->IsSelected());
		}
		else if (msg.pSender->GetName() == _T("Opt_3"))
		{
			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_3"));
			CComboUI* pCobCtrl = NULL;
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_2"));
			pCobCtrl->SetEnabled(pOptCtrl->IsSelected());
		}
		else if (msg.pSender->GetName() == _T("Opt_4"))
		{
			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_4"));
			CEditUI* pEdtCtrl = NULL;
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_2"));
			pEdtCtrl->SetEnabled(pOptCtrl->IsSelected());
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_3"));
			pEdtCtrl->SetEnabled(pOptCtrl->IsSelected());
		}
		else if (msg.pSender->GetName() == _T("Opt_6"))
		{
			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_6"));
			CEditUI* pEdtCtrl = NULL;
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_4"));
			pEdtCtrl->SetEnabled(pOptCtrl->IsSelected());
		}
		else if (msg.pSender->GetName() == _T("Opt_8"))
		{
			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_8"));
			CEditUI* pEdtCtrl = NULL;
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_5"));
			pEdtCtrl->SetEnabled(pOptCtrl->IsSelected());
			pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_6"));
			pEdtCtrl->SetEnabled(pOptCtrl->IsSelected());
		}
	}
}

//加载常规控件皮肤
void CRoomSetWnd::LoadNormalCtrlSkin()
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
			if (pEditUI->GetName() == _T("Edt_ID"))
			{
				continue;
			}
			pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
			pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
			pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
		}
	} while (pEditUI);

	//加载所有下拉列表框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ComboUI"));
	CComboUI* pComboUI = NULL;
	do 
	{
		pComboUI = static_cast<CComboUI*>(pCtrls->GetAt(idx++));
		if (pComboUI)
		{
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,0,500,22' corner='5,0,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,22,500,44' corner='5,0,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,44,500,66' corner='5,0,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,66,500,88' corner='5,0,22,0'");
			RECT rc = {6,0,0,0};
			pComboUI->SetItemTextPadding(rc);

// 			CScrollBarUI* pVScroll = pComboUI->GetVerticalScrollBar();
// 			if (pVScroll)
// 			{
// 				pVScroll->SetBkNormalImage("file='dialog\\vscrollbar_back.png' dest='0,0,14,100' source='0,0,14,100' corner='0,0,0,0'");
// 				pVScroll->SetButton1NormalImage("file='dialog\\vscrollbar_up.png' dest='0,0,16,16' source='0,0,16,16' corner='0,0,0,0'");
// 			}
		}
	} while (pComboUI);

	//加载所有多选框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("OptionUI"));
	COptionUI* pOptionUI = NULL;
	do 
	{
		pOptionUI = static_cast<COptionUI*>(pCtrls->GetAt(idx++));
		if (pOptionUI)
		{
			pOptionUI->SetNormalImage("file='dialog\\check.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
			pOptionUI->SetHotImage("file='dialog\\check.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
			pOptionUI->SetSelectedImage("file='dialog\\check.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
		}
	} while (pOptionUI);

}