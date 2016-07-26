#include "StdAfx.h"
#include "DUITopWnd.h"
#include "AfcTopWnd.h"
#include "..\\UILibrary2003\\ConfirmDlg.h"
#include "quitask.h"
#include "GamePlace.h"
#include "AfcGamelistWnd.h"
#include <io.h>
#include "GamePlaceDlg.h"

using namespace BzDui;

HHOOK CTopDuWndClass::m_hMHook  = NULL;
CTopDuWndClass* CTopDuWndClass::m_pInstance = NULL;

// ////////////////////////////////////////////////////////////////////
// 鼠标勾子
LRESULT CALLBACK TopDuWndMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
	bool bHide = false;

	if(nCode == HC_ACTION) 
	{ 
		PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT)lParam; 

		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam ==WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)
		{ 
			CTopDuWndClass* pDuTopWnd = CTopDuWndClass::m_pInstance;

			if (pDuTopWnd)
			{
				CContainerUI* pExSkinCtn = pDuTopWnd->m_pExSkinCtn;
				if (pExSkinCtn->IsVisible())
				{
					// 如果鼠标不点击在本窗口内, 则隐藏该窗口
					RECT rect;
					::GetWindowRect(pDuTopWnd->GetHWND(),&rect);
					RECT rcSkinCtn;
					rcSkinCtn.left = rect.left + pExSkinCtn->GetFixedXY().cx;
					rcSkinCtn.top = rect.top + pExSkinCtn->GetFixedXY().cy;
					rcSkinCtn.right = rcSkinCtn.left + pExSkinCtn->GetFixedWidth();
					rcSkinCtn.bottom = rcSkinCtn.top + pExSkinCtn->GetFixedHeight();

// 					CString str;
// 					str.Format("DClient: TopDuWndMouseHookProc (%d,%d,%d,%d) pt = (%d,%d)",
// 						rcSkinCtn.left,rcSkinCtn.top,rcSkinCtn.right,rcSkinCtn.bottom,
// 						mhs->pt.x,mhs->pt.y);
// 					OutputDebugString(str);

					if (!::PtInRect(&rcSkinCtn,mhs->pt))
					{
						pExSkinCtn->SetVisible(false);
						bHide = true;
					}
				}
			}
		}
	}

	LRESULT lRet = CallNextHookEx(CTopDuWndClass::m_hMHook, nCode, wParam, lParam); 

	if (bHide)
	{
		UnhookWindowsHookEx(CTopDuWndClass::m_hMHook);
	}

	return lRet;
}

CTopDuWndClass::CTopDuWndClass(void)
{
	//老版本的TopWnd
	m_pOldTopWndClass = NULL;

	//游戏列表
	m_pGameListWnd = NULL;

	CTopDuWndClass::m_pInstance = this;

}

CTopDuWndClass::~CTopDuWndClass(void)
{
	CTopDuWndClass::m_pInstance = NULL;
}

LPCTSTR CTopDuWndClass::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CTopDuWndClass::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CTopDuWndClass::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
	//delete this;
};

LRESULT CTopDuWndClass::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);

	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb(m_pm);
	CControlUI* pRoot = builder.Create(_T("top\\TopWnd.xml"), (UINT)0, &cb, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	MakeSkinSelUnVisible();

	return 0;
}
void CTopDuWndClass::MakeSkinSelUnVisible()
{
	m_pExSkinCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Ex_Skin"));
	m_pExSkinCtn->SetVisible(false);
}
LRESULT CTopDuWndClass::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CTopDuWndClass::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CTopDuWndClass::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CTopDuWndClass::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CTopDuWndClass::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CTopDuWndClass::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CTopDuWndClass::OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	CString str;
	PRECT lp = (RECT *)lParam;

	HWND hMainFrame = AfxGetApp()->m_pMainWnd->GetSafeHwnd();

	RECT rc;
	::GetWindowRect(hMainFrame,&rc);
	::MoveWindow(hMainFrame,lp->left,lp->top,rc.right - rc.left,rc.bottom - rc.top,true);
	::MoveWindow(m_hWnd,lp->left,lp->top,rc.right - rc.left,120,true);

	if (m_pGameListWnd->m_UserTabDuWnd.GetHWND())
	{
		::MoveWindow(m_pGameListWnd->m_UserTabDuWnd.GetHWND(),lp->left + 6 +15,lp->top + 90 + 85,246,31,TRUE);
	}

	return 0;
}

LRESULT CTopDuWndClass::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return 0;
}

LRESULT CTopDuWndClass::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	//MakeSkinSelUnVisible();

	return 0;
}

LRESULT CTopDuWndClass::OnChangeHeadPic(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	bHandled = TRUE;
	int iPicIndex = (int)(wParam);
	CString strImg;
	strImg.Format("../../log/UserLogos/Logo_%d.png",iPicIndex);

	CButtonUI *pBtnHeadPic = static_cast<CButtonUI*>(m_pm.FindControl("Btn_UserPic"));
	if(pBtnHeadPic != NULL)
	{
		pBtnHeadPic->SetBkImage(strImg);
		pBtnHeadPic->SetNormalImage(strImg);
	}
// 	CLabelUI *pLabNickName = static_cast<CLabelUI*>(m_pm.FindControl("UserNickname"));
// 	if(pLabNickName != NULL)
// 	{
// 		pLabNickName->SetText((LPCTSTR)lParam);
// 	}
	return 0;
}

LRESULT CTopDuWndClass::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

// 	if (uMsg == WM_MOUSELEAVE)
// 	{
// 		CString str;
// 		str.Format("DClient: CTopDuWndClass::HandleMessage uMsg = 0x%04x",uMsg);
// 		OutputDebugString(str);
// 	}

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

		case WM_MOVING:
			{
				lRes = OnMoving(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_MOVE:
			{
				lRes = OnMove(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_MOUSELEAVE:
			{
				lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled);
			}
		case SMG_HEADPIC_CHANGED:
			{
				//TODO:change topwnd user headPicture
				lRes = OnChangeHeadPic(uMsg,wParam,lParam,bHandled);
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
void CTopDuWndClass::SetMaxOrRestoreBtn(bool bEnable)
{
// 	CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Max"));
// 	if (NULL != pBtnCtrl && bEnable != pBtnCtrl->IsEnabled())
// 	{
// 		pBtnCtrl->SetEnabled(bEnable);
// 	}
// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Restore"));
// 	if (NULL != pBtnCtrl && bEnable != pBtnCtrl->IsEnabled())
// 	{
// 		pBtnCtrl->SetEnabled(bEnable);
// 	}
}

//显示设置音效按钮
void	CTopDuWndClass::ShowSoundSet()
{
	CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Sound"));
	if (NULL != pBtnCtrl)
	{
		pBtnCtrl->SetVisible(Glb().m_bRoomSound);
		pBtnCtrl->SetEnabled(Glb().m_bRoomSound);
	}
	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_NoSound"));
	if (NULL != pBtnCtrl)
	{
		pBtnCtrl->SetVisible(!Glb().m_bRoomSound);
		pBtnCtrl->SetEnabled(!Glb().m_bRoomSound);
	}
}

LRESULT CTopDuWndClass::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CTopDuWndClass::OnOK()
{

}

void CTopDuWndClass::OnHitYellow()
{
	if ("1" == m_skinmgr.GetValOfSkinBcf())
		return;

	//1.设置皮肤
	m_skinmgr.SetCurUserSelSkin("1");

	//2.通知MFC界面换肤
	m_skinmgr.Notify();

	//3.通知UI界面换肤
	ChangeDUISkin();

	//4.设置提示框的皮肤
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	ConfirmDlg::SetSkinKey(m_skinmgr.GetKeyVal(strSkin));
}

void CTopDuWndClass::OnHitBlue()
{
	if ("2" == m_skinmgr.GetValOfSkinBcf())
		return;

	//1.设置皮肤
	m_skinmgr.SetCurUserSelSkin("2");

	//2.通知MFC界面换肤
	m_skinmgr.Notify();

	//3.通知UI界面换肤
	ChangeDUISkin();

	//4.设置提示框的皮肤
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	ConfirmDlg::SetSkinKey(m_skinmgr.GetKeyVal(strSkin));
}

void CTopDuWndClass::OnHitMin()
{
	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_SYSCOMMAND,SC_MINIMIZE,0);
}

void CTopDuWndClass::OnHitSoundSet(bool bPlaySound)
{
	Glb().m_bRoomSound = bPlaySound;
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_SOUND_SET,0,0);		//音效设置

	//显示设置音效按钮
	ShowSoundSet();	
}

void CTopDuWndClass::OnHitMax()
{
//	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_SYSCOMMAND,SC_MAXIMIZE,0);
//
//	CButtonUI* pBtnCtrl = NULL;
//
//// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Max"));
//// 	pBtnCtrl->SetVisible(false);
//
//// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Restore"));
//// 	pBtnCtrl->SetVisible(true);
//
//	RECT rect = {0,0,0,0};
//	m_pm.SetCaptionRect(rect);
//
//	CHorizontalLayoutUI * pCtrl = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl("HorizontalLayoutUI5"));
//	
//	if (pCtrl != NULL)
//		pCtrl->SetFixedWidth(GetTopBtnMaxWidth());
//
//	if (((CGamePlaceDlg*)AfxGetMainWnd()) != NULL)
//		((CGamePlaceDlg*)AfxGetMainWnd())->Invalidate();
}

int CTopDuWndClass::GetTopBtnMaxWidth()
{
	//CString skinBcf = CBcfFile::GetAppPath();
	//CString strSkinfilename = m_skinmgr.GetSkinBcfFileName();
	//CBcfFile skinPath(skinBcf + strSkinfilename);

	////屏幕像素大于1280才使用最大左边距设置
	//if(::GetSystemMetrics(SM_CXSCREEN) < 1280)
	//{
	//	return skinPath.GetKeyVal("TopWnd","topbtnMinWidth",330);
	//}

	//return skinPath.GetKeyVal("TopWnd","topbtnMaxWidth",600);

	return GetTopBtnMinWidth();
}

int CTopDuWndClass::GetTopBtnMinWidth()
{
	return 120;
	//CString skinBcf = CBcfFile::GetAppPath();
	//CString strSkinfilename = m_skinmgr.GetSkinBcfFileName();
	//CBcfFile skinPath(skinBcf + strSkinfilename);
	//return skinPath.GetKeyVal("TopWnd","topbtnMinWidth",330);
}

void CTopDuWndClass::OnHitSkin()
{
	m_pExSkinCtn = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Ex_Skin"));
	m_pExSkinCtn->SetVisible(true);

	//移动换肤窗口到换肤按钮旁边
	CControlUI* pSkinBtn = m_pm.FindControl("Btn_skin");
	SIZE si = {pSkinBtn->GetX() - m_pExSkinCtn->GetFixedWidth() + pSkinBtn->GetFixedWidth(),\
		pSkinBtn->GetY() + pSkinBtn->GetFixedHeight()};
	m_pExSkinCtn->SetFixedXY(si);

	// 设置鼠标勾子
	m_hMHook = ::SetWindowsHookEx(WH_MOUSE,(HOOKPROC)TopDuWndMouseHookProc,AfxGetInstanceHandle(),AfxGetThread()->m_nThreadID);

}

void CTopDuWndClass::OnHitRestore()
{
//	HWND hMainFrame = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
//	::PostMessage(hMainFrame,ID_REST_MAX,0,0);
//
//	CButtonUI* pBtnCtrl = NULL;
//
//	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Close"));
//
//	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Min"));
//
//// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Max"));
//// 	pBtnCtrl->SetVisible(true);
//
//// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Restore"));
//// 	pBtnCtrl->SetVisible(false);
//
//	RECT rect = {0,0,0,30};
//	m_pm.SetCaptionRect(rect);
//
//	CHorizontalLayoutUI * pCtrl = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl("HorizontalLayoutUI5"));
//	if (pCtrl != NULL)
//		pCtrl->SetFixedWidth(GetTopBtnMinWidth());
//
//	return;
}

void CTopDuWndClass::OnHitClose()
{
	m_quitAskdlg.DoModal();
}

//大厅功能按钮
//
void CTopDuWndClass::OnBnClickedBt1()
{
	//::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,1,1);		//官方网站
	// above line commentted by hansen; 2014-12-22
	
	CString cs = CBcfFile::GetAppPath();
	CBcfFile file(cs + "bzgame.bcf");
	CString key = "BZW";
	TCHAR szItem[20] = {0};
	wsprintf(szItem,"hallurl1");
	CString url = file.GetKeyVal(key,szItem,"blank");
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE | SW_SHOW);

}

//
void CTopDuWndClass::OnBnClickedBt2()
{
	//::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_BANK,0,0);	
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_CALL_GM,0,0);    //呼叫GM
}

//
void CTopDuWndClass::OnBnClickedBt3()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_BANK,0,0);
	//AfxMessageBox("按钮3");
}

//
void CTopDuWndClass::OnBnClickedBt4()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,11,11);	//上传照片
	//AfxMessageBox("按钮4");

	
}

//
void CTopDuWndClass::OnBnClickedBt5()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,12,12);	//我的好友
	// above commentted by hansen 2014-12-22

}

//
void CTopDuWndClass::OnBnClickedBt6()
{
	//::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,2,2);		//用户充值
	//AfxMessageBox("按钮6");
	CString cs = CBcfFile::GetAppPath();
	CBcfFile file(cs + "bzgame.bcf");
	CString key = "BZW";
	TCHAR szItem[20] = {0};
	wsprintf(szItem,"gameurl1");
	CString url = file.GetKeyVal(key,szItem,"blank");

	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE | SW_SHOW);
}

//
void CTopDuWndClass::OnBnClickedBt7()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,3,3);		//划账
	//AfxMessageBox("按钮7");
}

//
void CTopDuWndClass::OnBnClickedBt8()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,4,4);		//排行榜
	//AfxMessageBox("按钮8");
}

//
void CTopDuWndClass::OnBnClickedBt9()
{
	//投诉  yjj 090310
	//::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,5,5);
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_MESSAGE_APPEAL,0,0); //反馈
	//AfxMessageBox("按钮9");
}

//锁定机器 zxj 2009-11-12
void CTopDuWndClass::OnBnClickedBt10()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_LOCK_ACCOUNT,0,0);
}
//大厅添加好友按钮 2013-02-03 wangzhitu
void CTopDuWndClass::OnBnClickedBt11()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IM,0,0);
}

void CTopDuWndClass::OnBnClickedBt21()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,21,21);
}

void CTopDuWndClass::OnBnClickedBt22()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,22,22);
}

void CTopDuWndClass::OnBnClickedBt23()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,23,23);
}

void CTopDuWndClass::OnBnClickedBt24()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,24,24);
}

void CTopDuWndClass::OnBnClickedBt25()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),IDM_OPEN_IE,25,25);
}

void CTopDuWndClass::Init() 
{
	LoadNormalCtrlSkin();

	//RECT rect = {0,0,0,0};
	//m_pm.SetCaptionRect(rect);

	CHorizontalLayoutUI* pCtrl = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl("HorizontalLayoutUI5"));

	if (pCtrl != NULL)
		pCtrl->SetFixedWidth(GetTopBtnMaxWidth());

	// 跑马灯
	m_pMarqueeTextUI = static_cast<CMarqueeTextUI*>(m_pm.FindControl("marqueetext"));

	if (m_pMarqueeTextUI != NULL)
	{
		if(m_pMarqueeTextUI->GetText().IsEmpty())
		{
			m_pMarqueeTextUI->SetVisible(false);
		}
	}
	
	ShowSoundSet();
}

void CTopDuWndClass::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Btn_Close")) 
		{
			OnHitClose();
			return;
		}
		else if( msg.pSender->GetName() == _T("Btn_Min")) 
		{
			OnHitMin();
			return;
		}
		else if ( msg.pSender->GetName() == _T("Btn_Sound"))
		{
			OnHitSoundSet(false);
			return;
		}
		else if ( msg.pSender->GetName() == _T("Btn_NoSound"))
		{
			OnHitSoundSet(true);
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_skin"))
		{
			OnHitSkin();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_yellow"))
		{
			OutputDebugString("press yellow");
			OnHitYellow();
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_blue"))
		{
			OutputDebugString("press blue");
			OnHitBlue();
			return;
		}
		else if(msg.pSender->GetName() == _T("Btn_Fn_01"))
		{
			OnBnClickedBt1();
			return;
		}
		else if(msg.pSender->GetName() == _T("Btn_Fn_02"))
		{
			OnBnClickedBt2();
			return;
		}
		else if(msg.pSender->GetName() == _T("Btn_Fn_03"))
		{
			OnBnClickedBt3();
			return;
		}
		else if(msg.pSender->GetName() == _T("Btn_Fn_06"))
		{
			OnBnClickedBt6();
			return;
		}
		else if(msg.pSender->GetName() == _T("Btn_UserPic"))
		{
			OnUserInfo();
			return;
		}
	}
	return;
}

void CTopDuWndClass::OnUserInfo()
{
	m_PlaceUserInfo = &(GetMainRoom()->m_PlaceUserInfo);
	m_pUserInfoWnd = new CUserInfoWnd();
	if(!m_pUserInfoWnd)
	{
		return;
	}
	m_pUserInfoWnd->SetPlaceUserInfo(m_PlaceUserInfo);
	m_pUserInfoWnd->Create(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_pUserInfoWnd->CenterWindow();
// 	RECT rect,rect1;
// 	GetClientRect(GetMainRoom()->m_hWnd,&rect);
// 	GetClientRect(m_pUserInfoWnd->GetHWND(),&rect1);
// 	::MoveWindow(m_pUserInfoWnd->GetHWND(),(rect.right*3)/4,(rect.bottom - rect.top)/2,rect1.right-rect1.left,rect1.bottom - rect1.top,true);
	m_pUserInfoWnd->ShowModal();
}
//加载常规控件皮肤
void CTopDuWndClass::LoadNormalCtrlSkin()
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

//调整关闭、还原、最小化按钮位置
void CTopDuWndClass::FixCtrlBtnPosition(UINT uType,bool bMaxSize)
{
	//CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("IE_Ad")));
	//if (pActiveXUI)
	//{
	//	//屏幕像素大于1280才显示广告
	//	if(::GetSystemMetrics(SM_CXSCREEN) >= 1280)
	//	{
	//		if (!bMaxSize)
	//		{
	//			pActiveXUI->SetVisible(false);
	//		}
	//		else
	//		{
	//			pActiveXUI->SetVisible(true);
	//		}
	//	}
	//	else 
	//	{
	//		pActiveXUI->SetVisible(false);
	//	}
	//}


// 	RECT rcPWnd;
// 	CContainerUI* pContainCtrl = static_cast<CContainerUI*>(m_pm.FindControl("ContainerUI123"));
// 	int wndWidth = pContainCtrl->GetFixedWidth();
// 	int wndHeight = pContainCtrl->GetHeight();
// 
// 	CButtonUI* pBtnCtrl = NULL;
// 	SIZE si;
// 	si.cy = 0;
// 
// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Close"));
// 	si.cx = wndWidth - pBtnCtrl->GetWidth();
// 	if (uType == ID_REST_MAX || (uType == SC_RESTORE && !bMaxSize))
// 	{
// 		si.cx -= 58;
// 	}
// 	pBtnCtrl->SetFixedXY(si);
// 
// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Restore"));
// 	si.cx -= pBtnCtrl->GetWidth();
// 	pBtnCtrl->SetFixedXY(si);
// 
// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Max"));
// 	pBtnCtrl->SetFixedXY(si);
// 
// 	pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Min"));
// 	si.cx -= pBtnCtrl->GetWidth();
// 	pBtnCtrl->SetFixedXY(si);

	return;
}

//得到通知已经收到flash地址
void CTopDuWndClass::SetFlashAd()
{
	////网页内容
	//CString strBuffer;
	//strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strRallAddvtisFlashADDR),1,1);

	//CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("IE_Ad")));
	//if( pActiveXUI ) 
	//{
	//	IWebBrowser2* pWebBrowser = NULL;
	//	pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	//	if( pWebBrowser != NULL ) 
	//	{
	//		pWebBrowser->Navigate(CA2W(strBuffer.GetBuffer()),NULL,NULL,NULL,NULL);  
	//		//pWebBrowser->Navigate(L"http://mdm.kfgame.com",NULL,NULL,NULL,NULL);  
	//		pWebBrowser->Release();
	//	}

	//	//屏幕像素大于1280才显示广告
	//	if(::GetSystemMetrics(SM_CXSCREEN) >= 1280)
	//	{
	//		pActiveXUI->SetVisible(true);
	//	}
	//	else
	//	{
	//		pActiveXUI->SetVisible(false);
	//	}
	//}
}
void CTopDuWndClass::ChangeDUISkin()
{
	CString LocalPath = CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile filePath( LocalPath + strSkin);
	CString skinfolder = 
	filePath.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	
	TCHAR szUIPath[MAX_PATH];
	wsprintf(szUIPath,"%s",skinfolder);

	BzDui::CPaintManagerUI::SetResourcePath(szUIPath);
	BzDui::CPaintManagerUI::ReloadSkin();
}
