#include "StdAfx.h"
#include "SafeCenterWnd.h"
#include "UserInfoWnd.h"
#include "MainRoomEx.h"

using namespace BzDui;

CSafeCenterWnd::CSafeCenterWnd(void)
{
	//用户信息
	m_pUserInfo = NULL;

	m_nRetFlag = 0;
}

CSafeCenterWnd::~CSafeCenterWnd(void)
{
}

LPCTSTR CSafeCenterWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CSafeCenterWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CSafeCenterWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CSafeCenterWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;

	CControlUI* pRoot = builder.Create(_T("logon\\SafeCheck.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CSafeCenterWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CSafeCenterWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CSafeCenterWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CSafeCenterWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CSafeCenterWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CSafeCenterWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CSafeCenterWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CSafeCenterWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			Close();
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
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

void CSafeCenterWnd::OnOK()
{

}

void CSafeCenterWnd::Init() 
{
	LoadNormalCtrlSkin();

	//加载名项目及状态
	if (!m_pUserInfo)
	{
		return;
	}

	TCheckNode tagChkNode;
	tagChkNode.name    = "没有进行主机绑定（绑定后帐号只能在绑定的主机登录）。";
	tagChkNode.opText  = "<b><u><c #0000FF>点击绑定</c></u></b>";
	tagChkNode.norText = "<c #008000>已绑定</c>";
	tagChkNode.OpFun = &CSafeCenterWnd::OnOperateLockMachine;
	tagChkNode.bOperated = (m_pUserInfo->iLockMathine == 1);
	m_mapSCTable.insert(pair<CheckItemType,TCheckNode>(CIT_LockMachine,tagChkNode));

	//身份验证模块
	if (CGamePlaceApp::GetModuleStatus("UserSecurity","IDCardVerify",0) == 1)
	{
		tagChkNode.name = "没有设置登陆身份验证（登陆时输入身份证码号码）。";
		tagChkNode.opText = "<b><u><c #0000FF>点击设置</c></u></b>";
		tagChkNode.norText = "<c #008000>已设置</c>";
		tagChkNode.OpFun = &CSafeCenterWnd::OnOperateSetIDVerify;
		tagChkNode.bOperated = (strlen(m_pUserInfo->szIDCardNo) > 0);
		m_mapSCTable.insert(pair<CheckItemType,TCheckNode>(CIT_IDVerify,tagChkNode));
	}

	//手机验证模块
	if (CGamePlaceApp::GetModuleStatus("UserSecurity","MobileVerify",0) == 1)
	{
		tagChkNode.name = "没有绑定手机号码(需要通过短信验证码登陆)。";
		tagChkNode.opText = "<b><u><c #0000FF>点击绑定</c></u></b>";
		tagChkNode.norText = "<c #008000>已绑定</c>";
		tagChkNode.OpFun = &CSafeCenterWnd::OnOperateBindMobileVerify;
		tagChkNode.bOperated = (m_pUserInfo->iBindMobile == 1);
		m_mapSCTable.insert(pair<CheckItemType,TCheckNode>(CIT_BindMobile,tagChkNode));
	}

	//加载显示文本
	CLabelUI* pLabCtrl = NULL;
	CButtonUI* pBntCtrl = NULL;
	TCHAR szName[32];
	TCHAR szLab[128];
	int iConuter = 0;

	if (!m_mapSCTable.empty())
	{
		map<CheckItemType,TCheckNode>::iterator itMap = m_mapSCTable.begin();
		while(itMap != m_mapSCTable.end())
		{
			iConuter++;
			TCheckNode* pCheckNode = &itMap->second;
			if (pCheckNode)
			{
				//左则文本
				::sprintf_s(szName,sizeof(szName),"Lab_Item%d",iConuter);
				pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl(szName));
				::sprintf_s(szLab,sizeof(szLab),"%d.%s",iConuter,pCheckNode->name.c_str());
				pLabCtrl->SetText(szLab);

				//右则操作按钮
				::sprintf_s(szName,sizeof(szName),"Bnt_Item%d",iConuter);
				pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(szName));
				pCheckNode->bntName = szName;
				pBntCtrl->SetShowHtml(true);
				if (!pCheckNode->bOperated)
				{
					pBntCtrl->SetText(pCheckNode->opText.c_str());
					//pBntCtrl->SetEnabled(true);
				}
				else
				{
					pBntCtrl->SetText(pCheckNode->norText.c_str());
					//pBntCtrl->SetEnabled(false);
				}
			}
			itMap++;
		}
	}
}

//加载Combol的列表项
void CSafeCenterWnd::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CSafeCenterWnd::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

void CSafeCenterWnd::Notify(TNotifyUI& msg)
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
			m_nRetFlag = 0;
			Close();
		}

		//通过响应的按钮名查找对就对应检测项目操作的函数指针，并执行
		//JianGuankun 2011.11.13
		if (!m_mapSCTable.empty())
		{
			map<CheckItemType,TCheckNode>::iterator itMap = m_mapSCTable.begin();
			while(itMap != m_mapSCTable.end())
			{
				TCheckNode* pCheckNode = &itMap->second;
				if (pCheckNode)
				{
					if (strcmp(pCheckNode->bntName.c_str(),msg.pSender->GetName()) == 0)
					{
						bool bRetVal = (this->*(pCheckNode->OpFun))(pCheckNode);
						break;
					}
				}
				itMap++;
			}
		}
	}
}

void CSafeCenterWnd::CenterWindow()
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
void CSafeCenterWnd::LoadNormalCtrlSkin()
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

//检测用户信息是否为安全，返回false为不安全，返回true为安全
bool CSafeCenterWnd::CheckSafe(const MSG_GP_UserInfo* pUserInfo)
{
	//检测锁机状态
	if (pUserInfo->iLockMathine == 0)
	{
		return false;
	}

	//身份验证模块
	if (CGamePlaceApp::GetModuleStatus("UserSecurity","IDCardVerify",0) == 1)
	{
		//检测身份证是否填写
		if (strlen(pUserInfo->szIDCardNo) == 0)
		{
			return false;
		}
	}

	//手机验证模块
	if (CGamePlaceApp::GetModuleStatus("UserSecurity","MobileVerify",0) == 1)
	{
		//检测是否绑定手机号码
		if (pUserInfo->iBindMobile == 0)
		{
			return false;
		}
	}

	return true;
}

//锁机操作
bool CSafeCenterWnd::OnOperateLockMachine(LPCVOID lpData)
{

	TCheckNode* pCheckNode = (TCheckNode*)lpData;
	
	CUserInfoWnd** pUserInfoWnd = &(GetMainRoom()->m_pGameListWnd->m_pUserInfoWnd);
	if (*pUserInfoWnd)
	{
		return false;
	}

	*pUserInfoWnd = new CUserInfoWnd();

	if(!*pUserInfoWnd)
	{
		return false;
	}
	
	(*pUserInfoWnd)->m_nDefaultPage = 2;

	//如果项目已设置(即已锁机，直接进入操作界面)，否则只停留在状态显示的Tab页
	if (pCheckNode->bOperated)
	{
		(*pUserInfoWnd)->m_nDefaultSubPage = 0;
	}
	else
	{
		(*pUserInfoWnd)->m_nDefaultSubPage = 1;
	}

	(*pUserInfoWnd)->SetPlaceUserInfo(m_pUserInfo);
	(*pUserInfoWnd)->Create(**pUserInfoWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	(*pUserInfoWnd)->CenterWindow();
	(*pUserInfoWnd)->ShowModal();

	delete (*pUserInfoWnd);
	(*pUserInfoWnd) = NULL;

	return true;
}

//身份验证操作
bool CSafeCenterWnd::OnOperateSetIDVerify(LPCVOID lpData)
{
	CUserInfoWnd** pUserInfoWnd = &(GetMainRoom()->m_pGameListWnd->m_pUserInfoWnd);
	if (*pUserInfoWnd)
	{
		return false;
	}

	*pUserInfoWnd = new CUserInfoWnd();

	if(!*pUserInfoWnd)
	{
		return false;
	}
	(*pUserInfoWnd)->m_nDefaultPage = 1;

	(*pUserInfoWnd)->SetPlaceUserInfo(m_pUserInfo);
	(*pUserInfoWnd)->Create(**pUserInfoWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	(*pUserInfoWnd)->CenterWindow();
	(*pUserInfoWnd)->ShowModal();

	delete (*pUserInfoWnd);
	(*pUserInfoWnd) = NULL;

	return true;
}

//手机绑定验证操作
bool CSafeCenterWnd::OnOperateBindMobileVerify(LPCVOID lpData)
{
	TCheckNode* pCheckNode = (TCheckNode*)lpData;

	CUserInfoWnd** pUserInfoWnd = &(GetMainRoom()->m_pGameListWnd->m_pUserInfoWnd);
	if (*pUserInfoWnd)
	{
		return false;
	}

	*pUserInfoWnd = new CUserInfoWnd();

	if(!*pUserInfoWnd)
	{
		return false;
	}

	(*pUserInfoWnd)->m_nDefaultPage = 3;

	//如果项目已设置(即已锁机，直接进入操作界面)，否则只停留在状态显示的Tab页
	if (pCheckNode->bOperated)
	{
		(*pUserInfoWnd)->m_nDefaultSubPage = 0;
	}
	else
	{
		(*pUserInfoWnd)->m_nDefaultSubPage = 1;
	}

	(*pUserInfoWnd)->SetPlaceUserInfo(m_pUserInfo);
	(*pUserInfoWnd)->Create(**pUserInfoWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	(*pUserInfoWnd)->CenterWindow();
	(*pUserInfoWnd)->ShowModal();

	delete (*pUserInfoWnd);
	(*pUserInfoWnd) = NULL;

	return true;
}

//设置某一安全的状态
void CSafeCenterWnd::SetItemSafeState(CheckItemType emType,bool bSafeType)
{
	map<CheckItemType,TCheckNode>::iterator itFind
		= m_mapSCTable.find(emType);

	if (itFind == m_mapSCTable.end())
	{
		return;
	}

	TCheckNode* pCheckNode = &itFind->second;
	pCheckNode->bOperated = bSafeType;

	CButtonUI* pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl(pCheckNode->bntName.c_str()));
	if (pBntCtrl)
	{
		if (!bSafeType)
		{
			pBntCtrl->SetText(pCheckNode->opText.c_str());
			//pBntCtrl->SetEnabled(true);
		}
		else
		{
			pBntCtrl->SetText(pCheckNode->norText.c_str());
			//pBntCtrl->SetEnabled(false);
		}
	}

	return;
}