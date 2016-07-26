
#include "StdAfx.h"
#include "UseBoardCastWnd.h"
#include "PropMessage.h"
#include "GameRoomEx.h"
#include "MainRoomEx.h"


using namespace BzDui;
#define WM_PROP_BUY					WM_USER + 1006				// 用户从道具面板中购买道具

CUseBoardCastWnd::CUseBoardCastWnd()
{
	m_userInfo = "";
	m_RGB = RGB(255,0,0);
	m_ChannelService = new CChannelService();
}

CUseBoardCastWnd::~CUseBoardCastWnd()
{
	if (m_ChannelService != NULL)
	{
		delete m_ChannelService;
		m_ChannelService = NULL;
	}
}

LPCTSTR CUseBoardCastWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
}

UINT CUseBoardCastWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_CHILD; 
}

void CUseBoardCastWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

void CUseBoardCastWnd::Init() 
{
	//用户个性签名
	m_pREdtCtrl = static_cast<CRichEditUI*>(m_pm.FindControl(_T("Edt_Ctn1_Sign")));//int

	m_TextTip = static_cast<CTextUI*>(m_pm.FindControl(_T("TEXTTip")));
	m_LabCaption = static_cast<CLabelUI*>(m_pm.FindControl(_T("LabCaption")));
	m_btExpress = static_cast<CButtonUI*>(m_pm.FindControl(_T("Btn_Expression")));

	LoadNormalCtrlSkin();

	COptionUI* _big = static_cast<COptionUI*>(m_pm.FindControl(_T("Opt_big")));
	if (_big != NULL)
		_big->Selected(true);
	SetPropID(BRD_MSG_BIG);
}
void CUseBoardCastWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType != _T("click")) 
		return;
	if(msg.pSender->GetName() == _T("Btn_Close")) 
		Close();
	else if(msg.pSender->GetName() == _T("Btn_OK")) 
		OnOK();
	else if (msg.pSender->GetName() == _T("Btn_Cancel"))
		Close();
	else if (msg.pSender->GetName() == _T("Btn_Expression"))
		OnClickEXpress();
	else if (msg.pSender->GetName() == _T("Opt_little"))
		SetPropID(BRD_MSG_SMALL);
	else if (msg.pSender->GetName() == _T("Opt_big"))
		SetPropID(BRD_MSG_BIG);
}

void CUseBoardCastWnd::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}

LRESULT CUseBoardCastWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("room_right\\LoadSpeakerWnd.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CUseBoardCastWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CUseBoardCastWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CUseBoardCastWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUseBoardCastWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT CUseBoardCastWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CUseBoardCastWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
//消息处理
LRESULT CUseBoardCastWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	case WM_PROP_BUY:
		{
			lRes = OnSendBuyProp(wParam,lParam);
		}
	case WM_HIT_EXPMESSTION:
		{
			lRes = OnExpress(wParam, lParam);
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
//消息处理
LRESULT CUseBoardCastWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			Close();
			return true;
		}
	}
	else if (uMsg == WM_KEYUP)
	{
		return true;
	}
	return false;
}
//设置道具id
void CUseBoardCastWnd::SetPropID(int iPropID)
{
	const CString _strtip[2] = {"<f 宋体 12><c 000080>注：小喇叭%d金币一个，每条消息不超过60个汉字,120个英文。消息<n>将在房间内以醒目的形式发出</c></f>", 
		"<f 宋体 12><c 000080>注：大喇叭%d金币一个，每条消息不超过127个汉字,254个英文。消息<n>将在房间内以醒目的形式发出</c></f>"};
	const CString _strcaption[2] = {"<f 宋体 12><c 000080>您能购买的小喇叭数量为【%d】个</c></f>",
		"<f 宋体 12><c 000080>您能购买的大喇叭数量为【%d】个</c></f>"};

	CString		s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin.bcf");

	m_nPropID = iPropID;
	if (m_nPropID == BRD_MSG_BIG)
	{
		CString strout;
		CString strTip, strCaption;
		strTip = f.GetKeyVal("BoardCast", "bigBoardCastTip", _strtip[1]);
		strCaption = f.GetKeyVal("BoardCast", "bigBoardCastCaption", _strcaption[1]);

		int _price = GetPropPrice(m_nPropID);
		strout.Format(strTip, _price);
		if (m_TextTip != NULL) m_TextTip->SetText(strout);
		strout.Format(strCaption, GetMainRoom()->m_PlaceUserInfo.i64Bank / _price);
		if (m_LabCaption != NULL) m_LabCaption->SetText(strout);
		m_pREdtCtrl->SetLimitText(254);
	}
	else if (m_nPropID == BRD_MSG_SMALL)
	{
		CString strout;
		CString strTip, strCaption;
		strTip = f.GetKeyVal("BoardCast", "smallBoardCastTip", _strtip[0]);
		strCaption = f.GetKeyVal("BoardCast", "smallBoardCastCaption", _strcaption[0]);
		int _price = GetPropPrice(m_nPropID);
		strout.Format(strTip, _price);
		if (m_TextTip != NULL) m_TextTip->SetText(strout);
		if (m_TextTip != NULL) m_TextTip->SetText(strout);
		strout.Format(strCaption, GetMainRoom()->m_PlaceUserInfo.i64Bank / _price);
		if (m_LabCaption != NULL) m_LabCaption->SetText(strout);
		m_pREdtCtrl->SetLimitText(120);
	}
}


int CUseBoardCastWnd::GetPropPrice(int iPropID)
{
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP *userProp=NULL;
	bool bPropExist=false;
	CString stime;

	int _att;
	if (iPropID == BRD_MSG_BIG)
		_att = 0x80;
	else if (iPropID == BRD_MSG_SMALL)
		_att = 0x100;


	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	bool isVip = GetMainRoom()->m_PlaceUserInfo.iVipTime > curtime;


	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->attribAction & _att)
		{
			if (isVip)
				return userProp->iVipPrice;
			else
				return userProp->iPrice;
		}
	}
	return -1;
}

_TAG_USERPROP* CUseBoardCastWnd::GetProp(int iPropID)
{
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP *userProp=NULL;
	bool bPropExist=false;
	CString stime;

	int _att;
	if (iPropID == BRD_MSG_BIG)
		_att = 0x80;
	else if (iPropID == BRD_MSG_SMALL)
		_att = 0x100;


	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	bool isVip = GetMainRoom()->m_PlaceUserInfo.iVipTime > curtime;


	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->attribAction & _att)
		{
			return userProp;
		}
	}
	return NULL;
}

//设置父窗体指针
void CUseBoardCastWnd::SetParentWnd(CWnd * pParWnd)
{
	m_pParentWnd = pParWnd;
}

void CUseBoardCastWnd::OnClickEXpress()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) 
		m_ExpressWnd.CreateExpressionWnd(NULL, GetHWND());

	//移动窗口
	RECT ButtonRect;
	SIZE ExpressSize;
	RECT ClientRect;
	GetClientRect(GetHWND(), &ClientRect);
	m_ExpressWnd.GetFixSize(ExpressSize);
	GetWindowRect(GetHWND(), &ButtonRect);
	//ButtonRect = m_btExpress->GetPos();
	//m_btExpress
	//m_btExpress->GetWindowRect(&ButtonRect);
	::MoveWindow(m_ExpressWnd.GetSafeHwnd(),max(ButtonRect.left, 0), max(0,ButtonRect.top - m_btExpress->GetHeight()),ExpressSize.cx,ExpressSize.cy, true);


	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();
	return;
}


void CUseBoardCastWnd::UseProp_Buy(int nPropIndex)
{
	CGameRoomEx *pGameRoom=(CGameRoomEx*)m_pParentWnd;
	if (pGameRoom != NULL)
	{
		
		pGameRoom->SendBoardCast(&m_BoardCast, m_nPropID);
	}

	

	Close();

}

//确定
void CUseBoardCastWnd::OnOK()
{


	CString strInput = m_pREdtCtrl->GetText();

	int inputlen = strInput.GetLength();
	if (inputlen > BROADCAST_MESSAGE_MAX)
	{
		//AFCMessageBox("输入字符不能超过最大限制");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","输入字符不能超过最大限制");
		return;
	}

	if (strInput == "")
	{
		//AFCMessageBox("输入字符不能为空");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","输入字符不能为空");
		return;
	}

	CMainRoomEx* pMainRoomEx = GetMainRoom();

	::memset(&m_BoardCast,0,sizeof(m_BoardCast));
	_TAG_USERPROP* _prop = GetProp(m_nPropID);
	if (_prop != NULL)
	{
		m_BoardCast.nPropID= _prop->nPropID;
		m_BoardCast.dwUserID=   pMainRoomEx->m_PlaceUserInfo.dwUserID;
		lstrcpy(m_BoardCast.szMessage, strInput.GetBuffer());
		m_BoardCast.iLength = strInput.GetLength();
		m_BoardCast.crColor = RGB(255,0,0);
		lstrcpy(m_BoardCast.szUserName,pMainRoomEx->m_PlaceUserInfo.nickName);


		_TAG_PROP_BUY propBuy;
		propBuy.dwUserID = pMainRoomEx->m_PlaceUserInfo.dwUserID;
		propBuy.nPropID = _prop->nPropID;
		propBuy.iPropPayMoney = 0;
		propBuy.nPropBuyCount = 1;
		propBuy.bUse = true;

		// 0表示在房间购买，1表示在游戏端购买
		pMainRoomEx->SendData(&propBuy,sizeof(_TAG_PROP_BUY),MDM_GP_PROP,ASS_PROP_BUY_NEW,0);

	}

}
// 发送购买道具消息
LRESULT CUseBoardCastWnd::OnSendBuyProp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


LRESULT CUseBoardCastWnd::OnExpress(WPARAM wParam, LPARAM lParam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wParam);
	if (pExpItem!=NULL)
	{
		CString strBuffer = m_pREdtCtrl->GetText();
		strBuffer+=pExpItem->m_szTrancelate;
		m_pREdtCtrl->SetText(strBuffer);
		m_pREdtCtrl->SetFocus();
		m_pREdtCtrl->SetSel(strBuffer.GetLength(),strBuffer.GetLength());
	}
	return 0;
}

//加载常规控件皮肤
void CUseBoardCastWnd::LoadNormalCtrlSkin()
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
			pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,0,500,22' corner='3,4,2,2'");
			pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,22,500,44' corner='3,4,2,2'");
			pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,66,500,88' corner='3,4,2,2'");
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
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,0,500,22' corner='5,2,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,22,500,44' corner='5,2,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,44,500,66' corner='5,2,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,66,500,88' corner='5,2,22,0'");
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