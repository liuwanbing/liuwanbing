#include "StdAfx.h"
#include "RightFrame.h"
#include "LoveSendClassInExe.h"

using namespace BzDui;

#include "UserListDlg.h"
#include "MenuDu.h"
#include "Platform\TipWnd.h"
#include "UseBoardCastWnd.h"
#include "propmessage.h"

#define IDT_TRANSPARENT				101		//透明用的定时器
#define IDT_SHOW_TALKBUF			102		//显示聊天消息缓存
#define IDT_MESSAGE_ON_PROP			103		//关于道具的提示信息

//用户列表菜单消息
#define UM_MENU_ULIST				(WM_USER + 100)
#define MMSG_ULIST_INFO				1		//详细信息
#define MMSG_ULIST_COPYNAME			2		//复制用户名
#define MMSG_ULIST_ADDFRIEND		3		//加为好友
#define MMSG_ULIST_ADDBLACKLIST		4		//加入黑名单
#define MMSG_ULIST_KICK				5		//踢人
#define MMSG_ULIST_KICK_PROP		6		//踢人
#define MMSG_ULIST_PROP_FL0WER		7		//赠送鲜花道具
#define MMSG_ULIST_PROP_BLICK       8       //释放拍砖头道具

//Avatar系统相关
#define BzGetAvatarIndex(obj) \
	GetAvatarIndex((obj)->GameUserInfo.bLogoID,(obj)->GameUserInfo.i64Bank,(obj)->GameUserInfo.i64Money,\
		(obj)->GameUserInfo.dwPoint,((CLoveSendClassInExe*)m_pRoom)->m_pGameInfo->uNameID, \
		((CLoveSendClassInExe*)m_pRoom)->m_pGameInfo->uComType)

CRightFrame::CRightFrame(CWnd* pParent)
{
	m_pRoom = pParent;

	//是否能隐藏该窗口
	m_bCanHide;

	//当前顶部被显示信息的用户
	m_pShowUser = NULL;

	//玩家列表控件
	m_pUsersList = NULL;

	//道具按钮群
	for (int i = 0; i < MAX_PROP_NUMS; ++i)
	{
		m_pPropBtn[i] = NULL;
	}

	//道具每页包含按钮的数量
	m_uPropPageSize = 0;

	//当前显示道具的页
	m_uCurrPropPage = 0;

	//道具页数
	m_uPropPageNums = 0;

	//是否百家乐游戏
	m_bBJLGame = 0;

	//玩家是否用踢人道具
	m_bUseKickUserProp = 0;

	//窗口的最大宽度
	m_uMaxWidth = 0;

	//窗口的最小宽度
	m_uMinWidth = 0;

	//窗口透明度
	m_iTransparent = 255;

	//聊天框
	m_pTalkBoxList = NULL;

	//开聊天框缓冲文本
	m_pTalkTempText = NULL;

	//缓冲的聊天消息
	m_dqTalkBuff.clear();

	//喇叭窗口
	m_pBoardCastWnd = NULL;

	//标签窗口
	m_pTipWnd = NULL;
}

CRightFrame::~CRightFrame(void)
{
	if (m_pUsersList)
	{
		delete m_pUsersList;
		m_pUsersList = NULL;
	}
	::KillTimer(m_hWnd,IDT_MESSAGE_ON_PROP);
	if (m_pTipWnd != NULL)
	{
		m_pTipWnd->Close();
		m_pTipWnd = NULL;
	}
}

LPCTSTR CRightFrame::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CRightFrame::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CRightFrame::OnFinalMessage(HWND hWnd) 
{
	m_dqTalkBuff.clear();
	m_pm.RemovePreMessageFilter(this);
	delete this;
};

LRESULT CRightFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("game\\RightFrame.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	::SetTimer(m_hWnd,IDT_SHOW_TALKBUF,200,NULL);

	return 0;
}

LRESULT CRightFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CRightFrame::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CRightFrame::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRightFrame::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CRightFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CRightFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	FixPropUI(LOWORD(lParam),HIWORD(lParam));
	bHandled = FALSE;
	return 0;
}

LRESULT CRightFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;

	if (wParam == IDT_TRANSPARENT)
	{
		m_iTransparent -= 50;

		m_pm.SetTransparent(m_iTransparent);

		if (m_iTransparent <= 20)
		{
			CWindowWnd::ShowWindow(false);
			::KillTimer(m_hWnd,IDT_TRANSPARENT);
			return 0;
		}

	}
	else if (wParam == IDT_SHOW_TALKBUF)
	{

		if (m_dqTalkBuff.size() > 0)
		{
			CString str = m_dqTalkBuff.front();
			m_dqTalkBuff.pop_front();

			m_pTalkTempText->SetVisible(true);
			m_pTalkTempText->SetText(str);
		}

		return 0;
	}
	else if (wParam == IDT_MESSAGE_ON_PROP)
	{
		if(m_pTipWnd != NULL)
		{
			m_pTipWnd->Close();
			m_pTipWnd = NULL;
		}
	}
	return 0;
}

LRESULT CRightFrame::OnCmdUserListMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);

	switch(wParam)
	{
		case MMSG_ULIST_INFO: //详细信息
			{
				return pGameFrame->OnCmdShowUserInfo();
			}
			break;

		case MMSG_ULIST_COPYNAME: //复制用户名
			{
				return pGameFrame->OnCmdCopyUserInfo();
			}
			break;

		case MMSG_ULIST_ADDFRIEND: //加为好友
			{
				if (m_pShowUser)
				{
					return pGameFrame->m_pGameRoom->PostMessage(WM_FRIEND,0,m_pShowUser->GameUserInfo.dwUserID);
				}
			}
			break;

		case MMSG_ULIST_ADDBLACKLIST: //加入黑名单
			{
				if (m_pShowUser)
				{
					return pGameFrame->m_pGameRoom->PostMessage(WM_EMENY,0,m_pShowUser->GameUserInfo.dwUserID);
				}
			}
			break;

		case MMSG_ULIST_KICK: //踢人
			{
				return pGameFrame->OnCmdKickUser();
			}
			break;

		case MMSG_ULIST_KICK_PROP: //踢人(道具)
			{
				return pGameFrame->OnCmdKickUserProp();
			}
			break;

		case MMSG_ULIST_PROP_FL0WER: //赠送鲜花道具
			{
				return pGameFrame->OnCmdFlowerProp();
			}
			break;
		case MMSG_ULIST_PROP_BLICK: //拍砖头
			{
				return pGameFrame->OnCmdBlickProp();
			}
			break;
	}

	return 0;
}


LRESULT CRightFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case UM_MENU_ULIST:
			{
				lRes = OnCmdUserListMenu(uMsg, wParam, lParam, bHandled);
			}
			break;

		default:
			bHandled = FALSE;
	}


// 	if(bHandled)
// 	{
// 		if (uMsg != WM_PAINT)
// 		{
// 			return lRes;
// 		}
// 
// 	}
// 
// 	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
// 	{
// 		if (uMsg != WM_PAINT)
// 		{
// 			return lRes;
// 		}
// 	}

	if(bHandled)
	{
		return lRes;
	}

	if(m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LPCTSTR CRightFrame::GetItemText(BzDui::CControlUI* pControl, int iIndex, int iSubItem)
{
	if (m_pUsersList)
	{
		if (pControl->GetParent()->GetParent() == m_pUsersList->m_pListCtrl)
		{
			CLoveSendClassInExe* pGameFrame = (CLoveSendClassInExe*)m_pRoom;

			if (!pGameFrame)
			{
				return 0;
			}

			bool bSecret = false;

			 UserItemStruct* pSelUser = m_pUsersList->GetUser(iIndex);

			 if (!pSelUser)
			 {
				 return 0;
			 }

			if ((pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT && pSelUser->GameUserInfo.dwUserID != pGameFrame->GetMeUserInfo()->dwUserID) || pGameFrame->m_pGameInfo->dwRoomRule & GRR_CONTEST)
			{
				bSecret = true;
			}
			bool bShowName = !(pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT) || (pSelUser->GameUserInfo.dwUserID == pGameFrame->GetMeUserInfo()->dwUserID);

			char szBuffer[128] = {0};
			m_pUsersList->OnGetItemText(pSelUser,iIndex,iSubItem,szBuffer,bSecret, bShowName); 
			CString strRet;

			if (IsVipEffectTime(pSelUser->GameUserInfo.iVipTime))
			{
				strRet.Format("<c FF0000>%s</c>",szBuffer);
			}
			else
			{
				strRet.Format("%s",szBuffer);
			}

			return strRet;
		}
	}

	return _T("");
}

//窗口显隐
void CRightFrame::ShowWindow(bool bShow/* = true*/, bool bTakeFocus/* = true*/)
{
	if (!bShow) //渐变消失
	{
		::SetTimer(m_hWnd,IDT_TRANSPARENT,40,NULL);
		return;
	}
	else
	{
		::KillTimer(m_hWnd,IDT_TRANSPARENT);
		m_pm.SetTransparent(257);
		m_iTransparent = 255;
	}

	CWindowWnd::ShowWindow(bShow,bTakeFocus);
}
//显示消息提示
void CRightFrame::ShowMessageTip(const BzDui::CButtonUI* pButton,const CString& strMsg,EBUBLEPOS pos)
{
	//创建窗体
	if (NULL == m_pTipWnd)
	{
		m_pTipWnd = new CTipWnd();
		m_pTipWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	}
	else
		return;

	//相关区域位置
	RECT rc,rect;
	GetClientRect(m_pTipWnd->GetHWND(),&rc);
	GetWindowRect(m_pRoom->GetSafeHwnd(),&rect);
	SIZE btXY = pButton->GetFixedXY();

	//相关控件
	CContainerUI *pTalk = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	CContainerUI *pProp = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Prop"));

	//偏移量读取
	int propX = 0;
	int propY = 0;
	CString strPropXY;
	CBcfFile bcfFile(CBcfFile::GetAppPath()+"..\\"+m_skinmgr.GetSkinBcfFileName());

	//设置位置
	int moveX = 0;
	int moveY = 0;
	_TAG_USERPROP* userProp = (_TAG_USERPROP*)pButton->GetTag();
	switch (pos)
	{
	case BUBLE_ON_PROP_POS:
		{
			if (userProp!= NULL)
			{
				strPropXY.Format("prop_btn0%d_posX",userProp->nPropID);
				propX = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),strPropXY,150);
				strPropXY.Format("prop_btn0%d_posY",userProp->nPropID);
				propY = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),strPropXY,40);
			}
			else
			{
				strPropXY = "prop_btn01_posX";
				propX = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),strPropXY,150);
				strPropXY = "prop_btn01_posY";
				propY = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),strPropXY,40);
			}
			moveX = rect.right-(pProp->GetWidth()-btXY.cx)-propX;
			moveY = rect.bottom-pTalk->GetHeight()-(pProp->GetHeight()-btXY.cy)-propY;
		}
		break;
	case BUBLE_ON_BOARDCAST_POS:
		{
			propX = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),"boardcast_posX",100);
			propY = bcfFile.GetKeyVal(_T("CustomDuiMessagePos"),"boardcast_posY",-20);
			moveX = rect.right-(pProp->GetWidth()-btXY.cx)-propX;
			moveY = rect.bottom-(pProp->GetHeight()-btXY.cy)-propY;
		}
		break;
		//位置不同只须添加枚举值和对应的处理即可
	default:
		{}
	}

	//移动位置
	::MoveWindow(m_pTipWnd->GetHWND(),moveX,moveY,rc.right-rc.left,rc.bottom-rc.top,true);

	//设置显示字体
	TCHAR textBuf[100] = {0};
	lstrcpy(textBuf,strMsg);
	m_pTipWnd->SetText(textBuf);
	m_pTipWnd->ShowWindow(SW_SHOW);

	//定时器按时释放窗口
	::SetTimer(m_hWnd,IDT_MESSAGE_ON_PROP,500,NULL);
}
//点击道具按钮
void CRightFrame::OnBnProp(BzDui::CButtonUI* pButton)
{
	//判空
	if (NULL == pButton)
		return;

	//判空
	CLoveSendClassInExe* pGameFrame = (CLoveSendClassInExe*)m_pRoom;
	if (NULL == pGameFrame)
		return;
	
	//初始
	CString strMsg("");
	CString strKey("");
	CBcfFile fsr(CBcfFile::GetAppPath() + "..\\SpecialRule.bcf");
	strKey.Format("%d",pGameFrame->m_pGameInfo->uNameID);

	//不能使用道具游戏场
	if (pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT)
		strMsg = _T("对不起,防作弊场不能使用道具!");
	else if (pGameFrame->m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
		strMsg = _T("对不起, 体验场(免费场)不能使用游戏道具!");
	else if (fsr.GetKeyVal (_T("BJLType"),strKey,0) > 0)
		strMsg = _T("对不起,无法在百人类游戏使用道具!");

	//提示
	if (strMsg != "")
	{
		ShowMessageTip(pButton,strMsg,BUBLE_ON_PROP_POS);
		return;
	}

	//打开道具框
	if (pButton->GetName() == _T("Prop_Btn_0")) //VIP道具按钮
	{	
		pGameFrame->OnGetBuyItemData(9999);
	}
	else
	{
		_TAG_USERPROP* userProp = (_TAG_USERPROP*)pButton->GetTag();
		if (userProp)
		{
			pGameFrame->OnGetBuyItemData(userProp->nPropID);;
		}
	}

	return;
}

//点击选择表情
void CRightFrame::OnBnExpression()
{
	CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);
	CGameTalkText* pExpressWnd = &pGameFrame->m_ExpressWnd;

	//建立表情窗口
	if (pExpressWnd->GetSafeHwnd() == NULL)
	{
		pExpressWnd->CreateExpressionWnd(pGameFrame);
	}

	//移动窗口

	CButtonUI* pFaceBtn = static_cast<CButtonUI*>(m_pm.FindControl("Talk_Btn_Face"));
	RECT rcFaceBtn = pFaceBtn->GetPos();

	RECT rcRightWnd;
	RECT rcGameWnd;
	::GetWindowRect(m_hWnd,&rcRightWnd);
	::GetWindowRect(pGameFrame->m_hWnd,&rcGameWnd);

	::CSize siExpress;
	pExpressWnd->GetFixSize(siExpress);

	pGameFrame->SafeMoveWindow(pExpressWnd,
		(rcRightWnd.left + rcFaceBtn.right) - siExpress.cx + 176,
		rcRightWnd.top + rcFaceBtn.top - siExpress.cy - 3,
		siExpress.cx,siExpress.cy);

	pExpressWnd->ShowWindow(SW_SHOW);
	pExpressWnd->SetFocus();

}

//打开用户列表的菜单
void CRightFrame::OnPopupUserListMenu(POINT pt)
{
	CLoveSendClassInExe* pGameFrame = (CLoveSendClassInExe*)m_pRoom;

	//比赛场不能没有右键菜单
	if (pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT)
	{
		return;
	}

	CListUI* pUserlist = m_pUsersList->m_pListCtrl;

	CControlUI* p = m_pm.FindControl(pt);

	int nFindPos = m_pUsersList->FindItem(p);
	if (nFindPos == -1)
	{
		return;
	}

	UserItemStruct* pSelUser = m_pUsersList->GetUser(nFindPos);
	UserInfoStruct* pMeUser = pGameFrame->GetMeUserInfo();

	int iForbitSay = 8 & Glb().m_CenterServerPara.m_nFunction;

	POINT ptS = pt;
	::ClientToScreen(*this,&pt);	

	CMenuDu* pMenu = new CMenuDu(m_hWnd,UM_MENU_ULIST);
	if (!pMenu)
	{
		return;
	}

	char szTime[32];
	_stprintf(szTime,"%d",CTime::GetCurrentTime());

	if (/*!m_bBJLGame && */pSelUser->GameUserInfo.dwUserID != pMeUser->dwUserID)
	{
		if(pMeUser->iVipTime > ::atoi(szTime)) // 会员才有此功能
		{
			pMenu->AddMenuItem(_T("请他离开"),MMSG_ULIST_KICK);
			pMenu->AddMenuSeparator();
		}
		else  //可配置是否有踢人卡的踢人功能
		{
			if(m_bUseKickUserProp) // 显示踢人菜单
			{
				pMenu->AddMenuItem(_T("请他离开"),MMSG_ULIST_KICK_PROP);
				pMenu->AddMenuSeparator();
			}
		}
	}

	pMenu->AddMenuItem(_T("详细信息"),MMSG_ULIST_INFO);
	pMenu->AddMenuItem(_T("复制用户名"),MMSG_ULIST_COPYNAME);

	if (pSelUser->GameUserInfo.dwUserID != pMeUser->dwUserID)
	{
		pMenu->AddMenuSeparator();
		pMenu->AddMenuItem(_T("送鲜花"),MMSG_ULIST_PROP_FL0WER);
		pMenu->AddMenuItem(_T("拍砖头"),MMSG_ULIST_PROP_BLICK);
	}
	
	if (!iForbitSay && pSelUser->GameUserInfo.dwUserID != pMeUser->dwUserID
			&& !(pGameFrame->m_pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK))
	{
		pMenu->AddMenuSeparator();
		pMenu->AddMenuItem(_T("加为好友"),MMSG_ULIST_ADDFRIEND);
		pMenu->AddMenuItem(_T("加入黑名单"),MMSG_ULIST_ADDBLACKLIST);
	}

	pMenu->SetShowPoint(pt);
	pMenu->Init();
	pMenu->ShowWindow(true);

	return;
}

//聊天框信息已准备好
void CRightFrame::OnTalkBufferReady(BzDui::CStdString strBuf,int iTextHeight)
{
	if (strBuf.GetLength() == 0)
	{
		return;
	}

	CTextUI* pText = new CTextUI();
	pText->SetFloat(false);
	pText->SetShowHtml(true);
	pText->SetText(strBuf);

	CListContainerElementUI* pCtn = new CListContainerElementUI();
	pCtn->SetFixedHeight(iTextHeight);
	pCtn->Add(pText);

	m_pTalkBoxList->Add(pCtn);

	return;
}

LRESULT CRightFrame::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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
	else if (uMsg == WM_KEYDOWN)
	{
		return true;
	}
	return false;
}

void CRightFrame::Init() 
{
	CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);

	CString sPath = CBcfFile::GetAppPath();

	//获取窗口的最大宽度与最小宽度
	CBcfFile fGSkin(sPath + "GameOption.bcf");
	m_uMaxWidth = fGSkin.GetKeyVal(_T("RightFrame"),"max_width",324);
	m_uMinWidth = fGSkin.GetKeyVal(_T("RightFrame"),"min_width",248);
	m_uOldWidth = fGSkin.GetKeyVal("RightFrame","old_width",0);
	//是否能隐藏该窗口
	m_bCanHide  = fGSkin.GetKeyVal(_T("RightFrame"),"can_hide",1);

	//判断是否百家乐游戏
	TCHAR szKey[10];
	
	CBcfFile fsr(sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", pGameFrame->m_pGameInfo->uNameID);
	m_bBJLGame = (fsr.GetKeyVal(_T("BJLType"),szKey,0) == 1);

	//玩家是否用踢人道具
	CBcfFile fOr(sPath + "..\\Order.bcf");
	m_bUseKickUserProp = (fOr.GetKeyVal(_T("ClassInExe"),_T("UseKickUserProp"),0) == 1);

	//如果是百人游戏，显示与隐藏窗口按钮无效
	CButtonUI* pBtnCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Btn_Show_Wnd"));
	if (pBtnCtrl)
	{
		pBtnCtrl->SetEnabled(m_bCanHide && (!m_bBJLGame));
	}

	//用户列表指针
	CListUI* pListCtrl = static_cast<CListUI*>(m_pm.FindControl("Lst_List_Users"));
	pListCtrl->SetTextCallback(this);
	pListCtrl->SetContextMenuUsed(true);

	m_pUsersList = new CGameUserListCtrlDu(pListCtrl);

	//加载常用控件的样式
	LoadNormalCtrlSkin();

	//初始化道具栏
	InitProp();

	CFont f;
	f.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,						   // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline下划线标记，需要下划线把这里设置成TRUE
		0,                         // cStrikeOut
		ANSI_CHARSET,			   // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));               // lpszFacename

	//聊天框
	m_pTalkBoxList = static_cast<CListUI*>(m_pm.FindControl("Talk_Lst_Main"));

	//开聊天框缓冲文本
	m_pTalkTempText = static_cast<CTextUI*>(m_pm.FindControl("Talk_Text_Temp"));

	OnReLoadTalkINI();

}

//加载Combol的列表项
void CRightFrame::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
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
bool CRightFrame::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
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

//初始化道具栏
void CRightFrame::InitProp()
{
	CContainerUI* pPropMainCtn = static_cast<CContainerUI*>(m_pm.FindControl("Prop_Ctn_Main"));

	if (!pPropMainCtn)
	{
		return;
	}
	RECT rc = {0,20,1,0};
	char szBuf[32] = {0};

	for (int i = 0; i < MAX_PROP_NUMS; ++i)
	{
		m_pPropBtn[i] = new CButtonUI();

		_stprintf(szBuf,"Prop_Btn_%d",i);
		m_pPropBtn[i]->SetName(szBuf);

		m_pPropBtn[i]->SetText("");
		m_pPropBtn[i]->SetTextColor(0xFFFFFFFF);
		m_pPropBtn[i]->SetAttribute("align","right");
		m_pPropBtn[i]->SetTextPadding(rc);

		m_pPropBtn[i]->SetFixedHeight(34);
		m_pPropBtn[i]->SetFixedWidth(34);
		m_pPropBtn[i]->SetFloat(true);
		m_pPropBtn[i]->SetTag(0);

		pPropMainCtn->Add(m_pPropBtn[i]);
	}

	return;
}

//道具栏UI位置匹配
void CRightFrame::FixPropUI(int cx,int cy)
{
	if (cy <= 0)
	{
		return;
	}

	//匹配用户信息栏位置////////////////////////////////////////////////////////////////////////////////
	CContainerUI* pInfoLeft = static_cast<CContainerUI*>(m_pm.FindControl("Info_Ctn_Left"));
	if (pInfoLeft)
	{
		pInfoLeft->SetFixedWidth(cx <= 300 ? 5 : 100);
	}

	//匹配道具栏的位置///////////////////////////////////////////////////////////////////////////////////

	CContainerUI* pPropMainCtn = static_cast<CContainerUI*>(m_pm.FindControl("Prop_Ctn_Main"));
	if (pPropMainCtn)
	{
		RECT rcWnd;
		::GetWindowRect(m_hWnd,&rcWnd);

		int iWidth = rcWnd.right - rcWnd.left - 50 - 34;

		const int iSpace = 4;
		const int iHorMums = iWidth / (34 + iSpace) + 1;
		m_uPropPageSize = iHorMums * 2;
		int iCount = 0;

		//计算页数
		m_uPropPageNums = (MAX_PROP_NUMS % m_uPropPageSize) == 0 ? 
			MAX_PROP_NUMS / m_uPropPageSize : MAX_PROP_NUMS / m_uPropPageSize + 1;

		for (int iPage = 0; iPage < m_uPropPageNums; ++iPage)
		{
			for (int iCol = 0; iCol < 2; ++iCol)
			{
				for (int iRow = 0; iRow < iHorMums; ++iRow)
				{
					if (iCount < MAX_PROP_NUMS)
					{
						SIZE sz = {(34 + 4) * iRow,15 + (34 + 4) * iCol};	
						m_pPropBtn[iCount++]->SetFixedXY(sz);
					}
				}
			}
		}

		//每次重定位后，都显示第一页
		ShowPropPage(0);

	}

	//匹配聊天框的位置///////////////////////////////////////////////////////////////////////////////////

	int x = 0,y = 0,width = 0,height = 0;

	CContainerUI* pCtrl = NULL;
	
	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	x += pCtrl ? pCtrl->GetInset().left : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Talk_Main"));
	x += pCtrl ? pCtrl->GetInset().left : 0;


	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Info"));
	y += pCtrl ? pCtrl->GetFixedHeight() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_UserList"));
	y += pCtrl ? pCtrl->GetFixedHeight() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Prop"));
	y += pCtrl ? pCtrl->GetFixedHeight() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	y += pCtrl ? pCtrl->GetInset().top : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Talk_Main"));
	y += pCtrl ? pCtrl->GetInset().top : 0;


	width = cx - x;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	width -= pCtrl ? pCtrl->GetInset().right : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Talk_Main"));
	width -= pCtrl ? pCtrl->GetInset().right : 0;


	height = cy - y;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	height -= pCtrl ? pCtrl->GetInset().bottom : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk_Control"));
	height -= pCtrl ? pCtrl->GetFixedHeight() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Talk_Main"));
	height -= pCtrl ? pCtrl->GetInset().bottom : 0; 

	//调整聊天输入框位置//////////////////////////////////////////////////////////////////////////////////////////

	width = cx;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk"));
	width -= pCtrl ? pCtrl->GetInset().left : 0;
	width -= pCtrl ? pCtrl->GetInset().right : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Hor_Talk_Control"));
	width -= pCtrl ? pCtrl->GetInset().left : 0;
	width -= pCtrl ? pCtrl->GetInset().right : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Talk_Ctn_01"));
	width -= pCtrl ? pCtrl->GetFixedWidth() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Talk_Ctn_02"));
	width -= pCtrl ? pCtrl->GetFixedWidth() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Talk_Ctn_04"));
	width -= pCtrl ? pCtrl->GetFixedWidth() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Talk_Ctn_05"));
	width -= pCtrl ? pCtrl->GetFixedWidth() : 0;

	pCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Talk_Ctn_03"));
	width -= pCtrl ? pCtrl->GetInset().left : 0;
	width -= pCtrl ? pCtrl->GetInset().right : 0;


	CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
	if (pEdtCtrl)
	{
		pEdtCtrl->SetFixedWidth(width - 30);
	}


	return;
}

//显示道具页
void CRightFrame::ShowPropPage(UINT uPage)
{
	if (uPage < 0)
	{
		return;
	}

	if (uPage >= m_uPropPageNums)
	{
		return;
	}

	//当前显示道具的页
	m_uCurrPropPage = uPage;

	int iMinPos = uPage * m_uPropPageSize;
	int iMaxPos = iMinPos + m_uPropPageSize - 1;

	for (int i = 0; i < MAX_PROP_NUMS; ++i)
	{
		if (m_pPropBtn[i])
		{
			m_pPropBtn[i]->SetVisible(i >= iMinPos && i <= iMaxPos);
		}
	}

	CButtonUI* pBtnPrev = static_cast<CButtonUI*>(m_pm.FindControl("Prop_Btn_Prev"));
	CButtonUI* pBtnNext = static_cast<CButtonUI*>(m_pm.FindControl("Prop_Btn_Next"));

	pBtnPrev->SetEnabled(uPage > 0);
	pBtnNext->SetEnabled(uPage < m_uPropPageNums - 1);

	return;
}

void CRightFrame::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		if (msg.pSender->GetName() == _T("Btn_Show_Wnd"))
		{
			CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);
			pGameFrame->OnBnClickedHide();
			return;
		}

		if (msg.pSender->GetName() == _T("Prop_Btn_Prev"))
		{
			ShowPropPage(--m_uCurrPropPage);
			return;
		}

		if (msg.pSender->GetName() == _T("Prop_Btn_Next"))
		{
			ShowPropPage(++m_uCurrPropPage);
			return;
		}

		if (msg.pSender->GetName().Left(8).Compare("Prop_Btn") == 0) //道具框内按钮消息响应
		{
			OnBnProp(static_cast<CButtonUI*>(msg.pSender));
			return;
		}

		if (msg.pSender->GetName() == "Talk_Btn_Face")
		{
			OnBnExpression();
			return;
		}

		if (msg.pSender->GetName() == "Chat_Btn_Send")
		{
			CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);
			pGameFrame->SendTalkMessage();
			return;
		}
		if (msg.pSender->GetName() == "Talk_Btn_BoardCast")
		{
			//获取对象指针
			CButtonUI *pButton = static_cast<CButtonUI*>(msg.pSender);
			CLoveSendClassInExe* pGameFrame = (CLoveSendClassInExe*)m_pRoom;
			if (!pGameFrame)
				return;

			//初始化
			CString strMsg("");
			CString strKey("");
			CBcfFile fsr(CBcfFile::GetAppPath() + "..\\SpecialRule.bcf");
			strKey.Format("%d",pGameFrame->m_pGameInfo->uNameID);

			//不能使用道具的游戏场判断
			if (pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT)
				strMsg = _T("对不起,防作弊场不能使用道具!");
			else if (pGameFrame->m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
				strMsg = _T("对不起, 体验场(免费场)不能使用游戏道具!"); 
			else if (fsr.GetKeyVal (_T("BJLType"),strKey,0) > 0)
				strMsg = _T("对不起,无法在百人类游戏使用道具!");

			//提示
			if (strMsg != "")
			{
				ShowMessageTip(pButton,strMsg,BUBLE_ON_BOARDCAST_POS);
				return;
			}

			//弹出喇叭窗口
			if (!m_pBoardCastWnd)
			{
				m_pBoardCastWnd = new CUseBoardCastWnd();
				m_pBoardCastWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
				m_pBoardCastWnd->SetParentWnd(m_pRoom);
				m_pBoardCastWnd->CenterWindow();
				m_pBoardCastWnd->ShowModal();
				if (m_pBoardCastWnd != NULL)
				{
					delete m_pBoardCastWnd;
					m_pBoardCastWnd = NULL;
				}
			}
		}
	}

	else if(msg.sType == _T("return"))
	{
		if (msg.pSender->GetName() == "Talk_Edt_Inp")
		{
			CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);
			pGameFrame->SendTalkMessage();
			return;
		}
	}

	else if (msg.sType == _T("itemselect"))
	{
		if (msg.pSender == m_pUsersList->m_pListCtrl)
		{
			ShowUserInfo(m_pUsersList->GetUser(msg.wParam));
			return;
		}

		if( msg.pSender->GetName() == _T("Talk_Cob_Inp")) 
		{
			CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
			pEditCtrl->SetText(msg.pSender->GetText());

			CLoveSendClassInExe* pGameFrame = static_cast<CLoveSendClassInExe*>(m_pRoom);
			pGameFrame->SendTalkMessage(msg.wParam);

			return;
		}
	}

	else if (msg.sType == _T("menu"))
	{
		if (msg.pSender == m_pUsersList->m_pListCtrl)
		{
			OnPopupUserListMenu(msg.ptMouse);
			return;
		}
	}

	else if( msg.sType == _T("settext") ) 
	{
		if (msg.pSender == m_pTalkTempText)
		{
			m_pTalkTempText->SetVisible(false);
			OnTalkBufferReady(m_pTalkTempText->GetText(),msg.wParam);
			return;
		}

	}
	else if (msg.sType == _T("ScrollSetRange"))
	{
		if (m_pTalkBoxList)
		{
			if (msg.pSender == m_pTalkBoxList->GetVerticalScrollBar())
			{
				m_pTalkBoxList->Scroll(0,msg.wParam);
			}
		}

	}

// 	CString str;
// 	str.Format("DClient: Notify msg = %s",msg.sType);
// 	OutputDebugString(str);
	
}

void CRightFrame::CenterWindow()
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
void CRightFrame::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("EditUI"));
	CEditUI* pEditUI = NULL;
	RECT rcText = {4,1,4,0};
	do 
	{
		pEditUI = static_cast<CEditUI*>(pCtrls->GetAt(idx++));

		if (pEditUI)
		{
			if (pEditUI->GetName() == _T("Talk_Edt_Inp"))
			{
				continue;
			}

			if (!pEditUI->IsReadOnly())
			{
				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			}
			else
			{
				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			}

			pEditUI->SetTextPadding(rcText);
		}
	} while (pEditUI);

	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ComboUI"));
	CComboUI* pComboUI = NULL;
	RECT rc = {6,0,30,0};
	do 
	{
		pComboUI = static_cast<CComboUI*>(pCtrls->GetAt(idx++));
		if (pComboUI)
		{
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,0,500,22' corner='5,0,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,22,500,44' corner='5,0,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,44,500,66' corner='5,0,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,66,500,88' corner='5,0,22,0'");
			pComboUI->EnableScrollBar(true,false);
			pComboUI->SetItemTextPadding(rc);
		}
	} while (pComboUI);

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

//列表选中的用户消息，显示在上方板块
void CRightFrame::ShowUserInfo(UserItemStruct* pUser)
{
	if (!pUser)
	{
		return;
	}

	m_pShowUser = pUser;

	CLoveSendClassInExe* pGameFrame = (CLoveSendClassInExe*)m_pRoom;
	pGameFrame->m_dwCurrentUserID = pUser->GameUserInfo.dwUserID;

	bool bSecret = false;
	if ((pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT && pUser->GameUserInfo.dwUserID != pGameFrame->GetMeUserInfo()->dwUserID) || pGameFrame->m_pGameInfo->dwRoomRule & GRR_CONTEST)
	{
		bSecret = true;
	}

	CLabelUI* pLabCtrl = NULL;
	CControlUI* pCtnCtrl = NULL;
	char szBuffer[128];



	//用户ID
	if ((pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT && pUser->GameUserInfo.dwUserID != pGameFrame->GetMeUserInfo()->dwUserID))
	{
		_stprintf(szBuffer,"玩家");
	}
	else
	{
		//用户名
		_stprintf(szBuffer,"%s", pUser->GameUserInfo.nickName);
	}

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_Name"));
	pLabCtrl->SetText(szBuffer);

	//用户ID
	if ((pGameFrame->m_pGameInfo->dwRoomRule & GRR_NOTCHEAT && pUser->GameUserInfo.dwUserID != pGameFrame->GetMeUserInfo()->dwUserID))
	{
		_stprintf(szBuffer,"???");
	}
	else
	{
		_stprintf(szBuffer,"%d",pUser->GameUserInfo.dwUserID);
	}
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_ID"));
	pLabCtrl->SetText(szBuffer);

	//积分
	if (bSecret)
	{
		_stprintf(szBuffer,"???");
	}
	else
	{
		int dwPoint = pUser->GameUserInfo.dwPoint;
		_stprintf(szBuffer,"%d",dwPoint - GetNormalOrderBasePoint(dwPoint));
	}
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_Points"));
	pLabCtrl->SetText(szBuffer);

	//金币
	if (bSecret)
	{
		_stprintf(szBuffer,"???");
	}
	else
	{
		::GlbGetNumString(szBuffer,pUser->GameUserInfo.i64Money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
	}
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_Coints"));
	pLabCtrl->SetText(szBuffer);

	//魅力
	if (bSecret)
	{
		_stprintf(szBuffer,"???");
	}
	else
	{
		_stprintf(szBuffer,"%d",pUser->GameUserInfo.dwFascination);
	}
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_Glamour"));
	pLabCtrl->SetText(szBuffer);

	//魅力等级
	_stprintf(szBuffer,"%s",bSecret?"???":GetCharmGrade(pUser->GameUserInfo.dwFascination));
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_Info_Level"));
	pLabCtrl->SetText(szBuffer);

	//logo
	pCtnCtrl = m_pm.FindControl("Img_Info_logo");
	GetUserLogoFile(pUser,szBuffer);
	pCtnCtrl->SetBkImage(szBuffer);

	return;
}

//获取玩家头像的logo文件路径
void CRightFrame::ShowUserInfo()
{
	ShowUserInfo(m_pShowUser);

	return;
}

//获得列表选中的用户
UserItemStruct* CRightFrame::GetShowUser()
{
	return m_pShowUser;
}

//获取玩家头像的logo文件路径
void CRightFrame::GetUserLogoFile(UserItemStruct* pUser,char* sz_out_file)
{
	if(!pUser)
	{
		return;
	}

	// 绘画人物头像
	_stprintf(sz_out_file,TEXT("..\\..\\..\\image\\log\\%s\\gamelog%ld.png"),pUser->GameUserInfo.bBoy?"Boy":"Girl",BzGetAvatarIndex(pUser));

	return;
}

//设置道具按钮图片
void CRightFrame::OnSetPropImages()
{
	_TAG_USERPROP *userProp = NULL;
	int propCout = (int)Glb().userPropLibrary.GetCount();

	char szPropImage[MAX_PATH];
	char szBuffer[256];

	//第一个格固定为VIP功能
	if (m_pPropBtn[0])
	{
		m_pPropBtn[0]->SetEnabled(true);
		m_pPropBtn[0]->SetNormalImage("file='game\\Prop\\vip01.png' dest='0,0,34,34' source='0,0,34,34'");
		m_pPropBtn[0]->SetHotImage("file='game\\Prop\\vip01.png' dest='0,0,34,34' source='34,0,68,34'");
		m_pPropBtn[0]->SetDisabledImage("file='game\\Prop\\vip01.png' dest='0,0,34,34' source='102,0,136,34'");
		m_pPropBtn[0]->SetText(_T(""));
	}

	CArray<_TAG_USERPROP*,_TAG_USERPROP*> userPropLibraryTmp;
	for(int i=0; i<Glb().userPropLibrary.GetCount(); ++i)
	{			
		userProp = Glb().userPropLibrary.GetAt(i);

		//由于大小喇叭已在聊天框旁实现，道具箱中的喇叭道具没需显示
		if (userProp != NULL 
			&& (::strcmp(userProp->szPropName,"大喇叭") == 0 
				|| ::strcmp(userProp->szPropName,"小喇叭") == 0))
			continue;
	
		userPropLibraryTmp.Add(userProp);
	}

	for(int i=1; i<MAX_PROP_NUMS; ++i)
	{
		if (NULL == m_pPropBtn[i])
		{
			continue;
		}

		if (i < propCout+1 && NULL != userPropLibraryTmp.GetAt(i-1)) //有道具的格子
		{
			try
			{
				m_pPropBtn[i]->SetEnabled(true);
				userProp = userPropLibraryTmp.GetAt(i-1);

				wsprintf(szPropImage,TEXT("game\\Prop\\Prop%d.png"),i);

				_stprintf(szBuffer,"file='%s' dest='0,0,34,34' source='0,0,34,34'",szPropImage);
				m_pPropBtn[i]->SetNormalImage(szBuffer);

				_stprintf(szBuffer,"file='%s' dest='0,0,34,34' source='34,0,68,34'",szPropImage);
				m_pPropBtn[i]->SetHotImage(szBuffer);

				_stprintf(szBuffer,"file='%s' dest='0,0,34,34' source='102,0,136,34'",szPropImage);
				m_pPropBtn[i]->SetDisabledImage(szBuffer);

				m_pPropBtn[i]->SetToolTip(userProp->szPropName);

				_stprintf(szBuffer,"%d",userProp->nHoldCount);
				m_pPropBtn[i]->SetText(szBuffer);

				//绑定道具ID于按钮
				m_pPropBtn[i]->SetTag((UINT_PTR)userProp);
			}
			catch (...)
			{
				DUIMessageBox(GetHWND(),MB_OK|MB_ICONINFORMATION,"提示","加载道具资源异常");
				return;
			}
		}
		else //没有道具的空格子
		{
			try
			{
				m_pPropBtn[i]->SetEnabled(false);
				m_pPropBtn[i]->SetNormalImage("file='game\\Prop\\PropEmpty.png' dest='0,0,34,34' source='0,0,34,34'");
				m_pPropBtn[i]->SetHotImage("file='game\\Prop\\PropEmpty.png' dest='0,0,34,34' source='34,0,68,34'");
				m_pPropBtn[i]->SetDisabledImage("file='game\\Prop\\PropEmpty.png' dest='0,0,34,34' source='102,0,136,34'");
				m_pPropBtn[i]->SetToolTip(_T("点击购买道具"));
				m_pPropBtn[i]->SetText(_T(""));
			}
			catch(...)
			{
				DUIMessageBox(GetHWND(),MB_OK|MB_ICONINFORMATION,"提示","加载道具资源异常");
				return;
			}
		}
	}

	return;
}

//重读talkIngame.ini
void CRightFrame::OnReLoadTalkINI()
{
	CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Talk_Cob_Inp"));
	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
	if (!pCobCtrl || !pEditCtrl)
	{
		return;
	}

	//清除原来内容
	pCobCtrl->RemoveAll();

	CBcfFile f("talkInGame.bcf");
	CString key="BZW_TALK";
	CString szTalk;
	CString szNumber;
	int nIdx = 0;
	do
	{
		szNumber.Format("%d",++nIdx);
		szTalk = f.GetKeyVal(key,"T"+szNumber,"");
		if (!szTalk.IsEmpty())
		{
			CListLabelElementUI* p = new CListLabelElementUI();
			p->SetText(szTalk.GetBuffer());
			pCobCtrl->Add(p);
		}
	}
	while(!szTalk.IsEmpty());

	SIZE si = {-1,600};
	pCobCtrl->SetDropBoxSize(si);

	pEditCtrl->SetText(_T(""));

	return;
}

//获得聊天框输入的内容
int CRightFrame::GetTalkInputMessage(char* szMsg)
{
	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
	if (!pEditCtrl)
	{
		return 0;
	}

	if (szMsg)
	{
		strcpy(szMsg,pEditCtrl->GetText().GetData());
	}

	return pEditCtrl->GetText().GetLength();

}

//设置焦点于聊天输入框
void CRightFrame::SetFocusToTalkInput()
{
	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
	if (pEditCtrl)
	{
		pEditCtrl->SetFocus();
	}
}

//清空聊天输入框
void CRightFrame::ClearTalkInput()
{
	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));
	if (pEditCtrl)
	{
		pEditCtrl->SetText(_T(""));
	}
}

//添加表情到聊天输入框
void CRightFrame::AddExpressionToTalkInput(char* szExpression)
{
	CEditUI* pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Talk_Edt_Inp"));

	CStdString str = pEditCtrl->GetText();
	str += szExpression;

	pEditCtrl->SetText(str);

	return;
}


bool CRightFrame::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	return true;
}

//向聊天Box插入消息内容
void CRightFrame::InsertTalkMessage(char* szMsgBuf)
{
	char* p = szMsgBuf;
	std::string strRet = "";
	CString strImageTmp("");
	while(*p != 0)
	{
		if (*p == '/' 
			&& (*(p + 1) != 0 && *(p + 1) == ':')
			&& (*(p + 2) != 0 && ::isdigit(*(p + 2)))
			&& (*(p + 3) != 0 && ::isdigit(*(p + 3)))
			)
		{
			strImageTmp.Format("<I dialog\\Image_biaoqing.png 10 7 %c %c>",*(p + 3),*(p + 2));
			strRet += strImageTmp;
			p += 4;
			continue;
		}
		else
		{
			strRet += *p;
		}
		p++;
	}

	CString str;
	str.Format("DGameL:%s",strRet.c_str());
	OutputDebugString(str);

	m_dqTalkBuff.push_back(strRet.c_str());

	return;
}