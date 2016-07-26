#include "StdAfx.h"
#include "LeftWnd.h"
#include "MainRoomEx.h"

using namespace BzDui;

CLeftWnd::CLeftWnd(void)
{
}


CLeftWnd::~CLeftWnd(void)
{
	UnInit();
}

LPCTSTR CLeftWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CLeftWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CLeftWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CLeftWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilderCallbackEx cb(m_pm);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("left\\GameList.xml"), (UINT)0, &cb, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CLeftWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CLeftWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CLeftWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLeftWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLeftWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CLeftWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLeftWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLeftWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	case WM_TIMER:
		{
			lRes = OnTimer(uMsg, wParam, lParam, bHandled);
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
			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);	//empty
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
/*
	case WM_SIZE:          
		{
			lRes = OnSize(uMsg, wParam, lParam, bHandled);
		}*/
		break;

	default:
		bHandled = FALSE;
	}

	if(bHandled)
	{
		return lRes;
	}
	
	if(32769!=uMsg && m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CLeftWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void CLeftWnd::Init()
{
	m_pHorOpt = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("Hor_Opt")));
	m_pGameList = static_cast<CGameListUI*>(m_pm.FindControl(_T("GameList")));
	m_pTxtOnlineCount =static_cast<CTextUI*>(m_pm.FindControl(_T("Txt_People")));

	UnInit();
}
#define MS_GET_ROOM						2							//获取房间
void CLeftWnd::Notify(BzDui::TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		CString strName = msg.pSender->GetName();
		if (strName == _T("Btn_GameName"))
		{
			Node* pNode = (Node*)msg.pSender->GetTag();
			if (pNode && pNode->data().uDataType == GLK_GAME_NAME)
			{
				ComNameInfo* pNameInfo = (ComNameInfo*)pNode->data().GetData();
				if (!pNameInfo)
				{
					return;
				}
				if ((pNode->data().m_uUpdateTime+60) <= time(NULL) && 
					GetMainRoom()->AddMission(MS_GET_ROOM, pNameInfo->uKindID, pNameInfo->uNameID))
				{
					pNode->data().m_uUpdateTime = (LONG)time(NULL);
				}
				if (m_pGameList->CanExpand(pNode))
				{
					m_pGameList->SetChildVisible(pNode, !pNode->data().child_visible_);
				}

				//打开网页
				static UINT uNameID = 0;
				if(uNameID != pNameInfo->uNameID)
				{
					CString url;
					//url.Format("%sGameInfo3.aspx?id=%d",Glb().m_CenterServerPara.m_strHomeADDR, pNameInfo->uNameID);
					url.Format("%sgamedetails.aspx?id=%d",Glb().m_CenterServerPara.m_strHomeADDR,pNameInfo->uNameID);
					GetMainRoom()->IE(url);
					uNameID = pNameInfo->uNameID;
					
					CString cs;
					cs.Format("chs homeaddr=%s;;;target-addr=%s",Glb().m_CenterServerPara.m_strHomeADDR,url);
					OutputDebugString(cs);
				}
			}
		}
		else if (strName == _T("Btn_Join"))
		{
			Node* pNode = (Node*)msg.pSender->GetTag();
			if (pNode && pNode->data().uDataType == GLK_GAME_ROOM)
			{
				CAFCRoomItem* pRoomItem = (CAFCRoomItem*)pNode->data().GetData();
				if (pRoomItem)
				{
					if (pRoomItem->m_bIsContest)
					{
						GetMainRoom()->OnGetRoomInfo(pRoomItem);
					}
					else
					{
						GetMainRoom()->OnCentranceGameRoom(pRoomItem);//游戏房间,,创建新房
					}
				}
			}
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == _T("Opt_GameKind"))
		{
			ComKindInfo* pKindInfo = (ComKindInfo*)msg.pSender->GetTag();
			int iShowIndex = -1;
			if (pKindInfo)
			{
				iShowIndex = pKindInfo->uKindID;
			}
			m_pGameList->ShowNameItems(iShowIndex);
		}
	}
}

void CLeftWnd::HandleKindData(ComKindInfo* pKindInfoPtr, int iCount)
{
	ComKindInfo* pKindInfo = NULL;
	for (UINT i=0;i<iCount&&i<3;i++)
	{
		pKindInfo = pKindInfoPtr + i;
		if (pKindInfo && pKindInfo->uKindID != 999)
		{
			ComKindInfo* pNewKindInfo = m_mapKindInfo[pKindInfo->uKindID];
			if (!pNewKindInfo)
			{
				pNewKindInfo = new ComKindInfo;
				memcpy(pNewKindInfo, pKindInfo, sizeof(ComKindInfo));
				m_mapKindInfo[pKindInfo->uKindID] = pNewKindInfo;
				if (m_pHorOpt)
				{
					CControlUI* pCtrl = m_pHorOpt->GetItemAt(i+1);
					if (pCtrl)
					{
						pCtrl->SetText(pNewKindInfo->szKindName);
						pCtrl->SetTag((UINT_PTR)pNewKindInfo);
						m_vecKindID.push_back(pNewKindInfo->uKindID);
					}
				}
			}
		}
	}
}

void CLeftWnd::HandleNameData(ComNameInfo* pNameInfoPtr, int iCount)
{
	int iKindIndex = 0;
	ComNameInfo *pNameInfo = NULL;
	for (int i=0; i<iCount; ++i)
	{
		pNameInfo = pNameInfoPtr + i;

		CString cs;
		cs.Format("chs [Client2] GameID= %d, HotOrNew=%d",pNameInfo->uNameID,pNameInfo->m_byHotOrNew);
		OutputDebugString(cs);

		if (!pNameInfo)
		{
			continue;
		}
		for (int j=0; j<m_vecKindID.size(); ++j)
		{
			if (m_vecKindID[j] == pNameInfo->uKindID)
			{
				iKindIndex = j+1;
				break;
			}
		}
		if (m_pGameList)
		{
			Node* pNode = m_pGameList->AddGameNode(pNameInfo, iKindIndex);
			m_mapNameNode[pNameInfo->uNameID] = pNode;
		}
	}	
}

void CLeftWnd::HandleRoomData(ComRoomInfo * pRoomInfoPtr, int iCount, UINT uKindID, UINT uNameID)
{
	ComRoomInfo* pRoomInfo = NULL;
	for (int i=0; i<iCount; ++i)
	{
		pRoomInfo = pRoomInfoPtr + i;
		if (!pRoomInfo)
		{
			continue;
		}
		Node* pNode = m_mapNameNode[uNameID];
		if (!pNode)
		{
			continue;
		}
		bool bExist = false;
		Node* pChild = NULL;
		for (int i=0; i!=pNode->num_children(); ++i)
		{
			pChild = pNode->child(i);
			if (pChild)
			{
				CAFCRoomItem* pExistRoomItem = (CAFCRoomItem*)pChild->data().GetData();
				if (pExistRoomItem && (pExistRoomItem->m_RoomInfo.uRoomID == pRoomInfo->uRoomID ||
					(pExistRoomItem->m_bIsContest && pExistRoomItem->m_iContestID==pRoomInfo->iContestID)))
				{
					CRoomElementUI* pEle = static_cast<CRoomElementUI*>(pChild->data().list_elment_);
					if (pEle)
					{
						pEle->SetData(pRoomInfo);
						bExist = true;
						break;
					}	
				}						
			}
		}
		if (!bExist)
		{
			CString sdp;
			sdp.Format("sdp_list 更新获得房间信息 uNameID=%u,uKindID=%u,uRoomID=%u", pRoomInfo->uNameID, pRoomInfo->uKindID, pRoomInfo->uRoomID);
			OutputDebugString(sdp);

			Node* pRoomNode = m_pGameList->AddRoomNode(pRoomInfo, pNode);
			if (pRoomNode)
			{
				m_mapRoomNode[pRoomInfo->uRoomID] = pRoomNode;
			}
		}
	}
}

//查找比赛房间子项
ComRoomInfo * CLeftWnd::FindContestRoomItem(CAFCRoomItem * pRoomItem, int iRoomID)
{
	if (pRoomItem != NULL && pRoomItem->m_bIsContest)
	{
		for (int i = 0; i < pRoomItem->m_RoomContest.size(); i++)
		{
			if (pRoomItem->m_RoomContest[i].uRoomID == iRoomID)
				return &(pRoomItem->m_RoomContest[i]);
		}
	}
	return NULL;
}

bool CLeftWnd::HandleNameOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount)
{
	for (UINT i=0;i<uCount;i++)
	{
		Node* pNode = m_mapNameNode[(pOnLineCountPtr+i)->uID];
		if (!pNode)
		{
			continue;
		}
		ComNameInfo* pNameInfo = (ComNameInfo*)pNode->data().GetData();
		if (!pNameInfo)
		{
			continue;
		}
		pNameInfo->m_uOnLineCount = (pOnLineCountPtr+i)->uOnLineCount;
	}	
	return true;
}

bool CLeftWnd::HandleRoomOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount)
{
	Node* pRoomNode = NULL;
	CAFCRoomItem* pRoomItem = NULL;
	for (UINT i=0;i<uCount;i++)
	{
		pRoomNode = m_mapRoomNode[(pOnLineCountPtr+i)->uID];
		if (!pRoomNode)
		{
			continue;
		}
		pRoomItem = (CAFCRoomItem*)pRoomNode->data().GetData();
		if (pRoomItem!=NULL)
		{
			if (!pRoomItem->m_bIsContest)
			{
				pRoomItem->m_RoomInfo.uPeopleCount=(pOnLineCountPtr+i)->uOnLineCount;
				CRoomElementUI* pEle = static_cast<CRoomElementUI*>(pRoomNode->data().list_elment_);
				if (pEle)
				{
					pEle->Refresh();
				}
			}
			else
			{
				ComRoomInfo* pRoomInfo = FindContestRoomItem(pRoomItem, (pOnLineCountPtr+i)->uID);
				if (pRoomInfo)
				{
					pRoomInfo->uPeopleCount = (pOnLineCountPtr+i)->uOnLineCount;
				}
			}
		}
	}	
	return true;
}

bool CLeftWnd::GetProcessName(CAFCRoomItem * pGameRoomItem, TCHAR * szProcessName, UINT uBufferSize)
{
	if (!pGameRoomItem)
	{
		return false;
	}
	Node* pNode = m_mapNameNode[pGameRoomItem->m_RoomInfo.uNameID];
	if (pNode)
	{
		ComNameInfo* pNameInfo = (ComNameInfo*)pNode->data().GetData();
		if (pNameInfo)
		{
			CopyMemory(szProcessName, pNameInfo->szGameProcess,__min(uBufferSize,sizeof(pNameInfo->szGameProcess)));
			szProcessName[uBufferSize/sizeof(TCHAR)-1]=0;
			return true;
		}
	}
	return false;
}

bool CLeftWnd::GetGameName(CAFCRoomItem * pGameRoomItem, TCHAR * szGameName, UINT uBufferSize)
{
	if (!pGameRoomItem)
	{
		return false;
	}
	Node* pNode = m_mapNameNode[pGameRoomItem->m_RoomInfo.uNameID];
	if (pNode)
	{
		ComNameInfo* pNameInfo = (ComNameInfo*)pNode->data().GetData();
		if (pNameInfo)
		{
			CopyMemory(szGameName, pNameInfo->szGameName,__min(uBufferSize,sizeof(pNameInfo->szGameName)));
			szGameName[uBufferSize/sizeof(TCHAR)-1]=0;
			return true;
		}
	}
	return false;
}

void CLeftWnd::UpdatePeopleCount()
{
	if (m_pTxtOnlineCount)
	{
		CString str;
		str.Format("锦江在线(%d)", GetMainRoom()->GetonlineUserCount());
		m_pTxtOnlineCount->SetText(str);
	}
}

void CLeftWnd::UnInit()
{
/*	map<UINT, ComKindInfo*>::iterator iter = m_mapKindInfo.begin();
	ComKindInfo *pKindInfo = NULL;
	for (; iter!=m_mapKindInfo.end(); ++iter)
	{
		pKindInfo = iter->second;
		if (pKindInfo)
		{
			delete pKindInfo;
			pKindInfo = NULL;
		}
	}
	m_mapKindInfo.clear();*/

	m_mapNameNode.clear();

	m_mapRoomNode.clear();

//	m_vecKindID.clear();

	if (m_pGameList)
	{
		m_pGameList->RemoveAll();
	}
}