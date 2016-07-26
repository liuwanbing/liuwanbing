#include "StdAfx.h"
#include "GameListUI.h"
#include "DownloadListUI.h"

using namespace BzDui;


void CListBodyExUI::SetScrollPos(SIZE szPos)
{
	CListBodyUI::SetScrollPos(szPos);
}

void CListBodyExUI::SetPos(RECT rc)
{
	CListBodyUI::SetPos(rc);
}

void CListBodyExUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
		else CControlUI::DoEvent(event);
		return;
	}
	else if( m_iSepHeight != 0 )
	{
		if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
		{
			RECT rcSeparator = GetThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				m_uButtonState |= UISTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcItem;
				if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(this);
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				m_rcItem = m_rcNewPos;
				if( !m_bImmMode && m_pManager ) m_pManager->RemovePostPaint(this);
				NeedParentUpdate();
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
			{
				LONG cy = event.ptMouse.y - ptLastMouse.y;
				ptLastMouse = event.ptMouse;
				RECT rc = m_rcNewPos;
				if( m_iSepHeight >= 0 ) {
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight ) return;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.bottom ) return;
					rc.bottom += cy;
					if( rc.bottom - rc.top <= GetMinHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
						rc.bottom = rc.top + GetMinHeight();
					}
					if( rc.bottom - rc.top >= GetMaxHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
						rc.bottom = rc.top + GetMaxHeight();
					}
				}
				else {
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.top ) return;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight ) return;
					rc.top += cy;
					if( rc.bottom - rc.top <= GetMinHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
						rc.top = rc.bottom - GetMinHeight();
					}
					if( rc.bottom - rc.top >= GetMaxHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
						rc.top = rc.bottom - GetMaxHeight();
					}
				}

				BzDui::CRect rcInvalidate = GetThumbRect(true);
				m_rcNewPos = rc;
				m_cxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

				if( m_bImmMode ) {
					m_rcItem = m_rcNewPos;
					NeedParentUpdate();
				}
				else {
					rcInvalidate.Join(GetThumbRect(true));
					rcInvalidate.Join(GetThumbRect(false));
					if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
				}
				return;
			}
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = GetThumbRect(false);
			if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
				return;
			}
		}
	}
	if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
}

void CListBodyExUI::EnableScrollBar(bool bEnableVertical /* = true */, bool bEnableHorizontal /* = false */)
{
	if( bEnableVertical && !m_pVerticalScrollBar ) {
		m_pVerticalScrollBar = new CScrollBarExUI;
		m_pVerticalScrollBar->SetOwner(this);
		m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
		if ( m_pManager ) {
			LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
			if( pDefaultAttributes ) {
				m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
			}
		}
	}
	NeedUpdate();
}


void CScrollBarExUI::DoEvent(BzDui::TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
		else CControlUI::DoEvent(event);
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( !IsEnabled() ) return;

		m_nLastScrollOffset = 0;
		m_nScrollRepeatDelay = 0;
		if( ::PtInRect(&m_rcThumb, event.ptMouse) )
		{
			m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
			ptLastMouse = event.ptMouse;
			m_nLastScrollPos = m_nScrollPos;
			return;
		}		
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		m_nScrollRepeatDelay = 0;
		m_nLastScrollOffset = 0;

		if( (m_uThumbState & UISTATE_CAPTURED) != 0 )
		{
			m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
			Invalidate();
			return;
		}		
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uThumbState & UISTATE_CAPTURED) != 0 )
		{
			if( !m_bHorizontal )
			{

				int vRange = m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_cxyFixed.cx;

				if (vRange != 0)
					m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / vRange;

				if( m_pOwner != NULL ) m_pOwner->SetScrollPos(BzDui::CSize(m_pOwner->GetScrollPos().cx, \
					m_nLastScrollPos + m_nLastScrollOffset)); 
				else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);

				return;

			}
		}
	}
	CScrollBarUI::DoEvent(event);
}

CGameListUI::CGameListUI(CPaintManagerUI &pm)
	:m_PaintManager(pm)
{
	if (m_pList)
	{
		delete m_pList;
		m_pList = new CListBodyExUI(this);
	}
	root_node_ = new Node;
	root_node_->data().uDataType = GLK_GAME_USE;
	root_node_->data().child_visible_ = true;
	root_node_->data().list_elment_ = NULL;
}

CGameListUI::~CGameListUI(void)
{
}

bool CGameListUI::Add(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::Add(pControl);
}

bool CGameListUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CGameListUI::Remove(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::Remove(pControl);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

bool CGameListUI::RemoveAt(int iIndex)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::RemoveAt(iIndex);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

void CGameListUI::RemoveAll()
{
	CListUI::RemoveAll();
	for (int i = 0; i < root_node_->num_children(); ++i)
	{
		Node* child = root_node_->child(i);
		RemoveNode(child);
	}
	delete root_node_;

	root_node_ = new Node;
	root_node_->data().child_visible_ = true;
	root_node_->data().list_elment_ = NULL;
}

void CGameListUI::DoEvent(TEventUI& event) 
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
	{
		if (m_pParent != NULL)
			m_pParent->DoEvent(event);
		else
			CVerticalLayoutUI::DoEvent(event);
		return;
	}
	CListUI::DoEvent(event);
}

Node* CGameListUI::GetRoot()
{
	return root_node_;
}
/*
static bool OnLogoButtonEvent(void* event, LPARAM lParam, WPARAM wParam) {
	if( ((TEventUI*)event)->Type == UIEVENT_BUTTONDOWN ) {
		CControlUI* pButton = ((TEventUI*)event)->pSender;
		if( pButton != NULL ) {
			CListContainerElementUI* pListElement = (CListContainerElementUI*)(pButton->GetTag());
			if( pListElement != NULL ) pListElement->DoEvent(*(TEventUI*)event);
		}
	}
	return true;
}
*/
bool CGameListUI::RemoveNode(Node* node)
{
	if (!node || node == root_node_) return false;

	for (int i = 0; i < node->num_children(); ++i)
	{
		Node* child = node->child(i);
		RemoveNode(child);
	}

	CListUI::Remove(node->data().list_elment_);
	node->parent()->remove_child(node);

	delete node;

	return true;
}

void CGameListUI::SetChildVisible(Node* node, bool visible)
{
	if (!node || node == root_node_)
		return;

	if (node->data().child_visible_ == visible)
		return;

	node->data().child_visible_ = visible;
	
	if (!node->data().list_elment_->IsVisible())
		return;

	if (!node->has_children())
		return;

	for (int i=0; i!=node->num_children(); ++i)
	{
		Node* pchild = node->child(i);
		int index = pchild->data().list_elment_->GetIndex();
		CControlUI* control = GetItemAt(index);
		if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			if (visible) 
			{
				Node* local_parent = ((Node*)control->GetTag())->parent();
				if (local_parent->data().child_visible_ && local_parent->data().list_elment_->IsVisible())
				{
					control->SetVisible(true);
				}
			}
			else
			{
				control->SetVisible(false);
			}
		}
	}
}

bool CGameListUI::CanExpand(Node* node) const
{
	if (!node || node == root_node_)
		return false;

	return GLK_GAME_NAME == node->data().uDataType;
}

void CGameListUI::ShowNameItems(int iShowIndex)
{
	Node* pNode = root_node_;
	Node* pChild = NULL;
	if (!pNode)
	{
		return;
	}
	for (int i=0; i<pNode->num_children(); ++i)
	{
		pChild = pNode->child(i);
		if (!pChild)
		{
			continue;
		}
		if (-1 == iShowIndex)
		{
			pChild->data().list_elment_->SetVisible(true);
		}
		else
		{
			ComNameInfo* pNameInfo = (ComNameInfo*)pChild->data().GetData();
			if (pNameInfo && pNameInfo->uKindID==iShowIndex)
			{
				pChild->data().list_elment_->SetVisible(true);
			}
			else
			{
				SetChildVisible(pChild, false);
				pChild->data().list_elment_->SetVisible(false);
			}
		}
	}
}

Node* CGameListUI::AddGameNode(Node* pNode)
{
	if (!pNode)
	{
		return NULL;
	}
	Node* parent = root_node_;
	if (!parent)
	{
		return NULL;
	}
	//插入到最后
	int index = 0;
	if (0 < parent->has_children())
	{
		Node* prev = parent->get_last_child();
		index = prev->data().list_elment_->GetIndex() +1;	
	}

	CListUI::AddAt(pNode->data().list_elment_, index);

	parent->add_child(pNode);

	ComNameInfo* pNameInfo = (ComNameInfo*)pNode->data().GetData();

	m_mapNameNode[pNameInfo->uNameID] = pNode;

	return pNode;
}

Node* CGameListUI::AddGameNode(ComNameInfo* pNameInfo, int iKindIndex)
{
	if (!pNameInfo)
	{
		return NULL;
	}
	Node* parent = root_node_;
	if (!parent)
	{
		return NULL;
	}
	//分组节点的父节点为root_node_
	CGameElementUI* pListElement = NULL;
	CDialogBuilder dlgduild;
	CDialogBuilderCallbackEx cb(m_PaintManager);
	pListElement = static_cast<CGameElementUI*>(dlgduild.Create(_T("left/GameElement.xml"), (UINT)0, &cb, &m_PaintManager));
	if (pListElement == NULL)
		return NULL;
	
	//保存分组数据
	pListElement->SetData(pNameInfo);
	
	Node* node = new Node;
	node->data().uDataType = GLK_GAME_NAME;
	node->data().child_visible_ = false;
	node->data().list_elment_ = pListElement;

	pListElement->SetTag((UINT_PTR)node);
	
	CButtonUI* pBtn = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pListElement, _T("Btn_GameName")));
	if (pBtn)
	{
		CString strImg;
		strImg.Format(_T("file='../../game/GameKind_%d.bmp' source='0,0,211,58'"), iKindIndex);		
		pBtn->SetBkImage(strImg);
		strImg.Format(_T("../../game/%d.png"), pNameInfo->uNameID);
		pBtn->SetNormalImage(strImg);
		pBtn->SetTag((UINT_PTR)node);
	}

	CControlUI *pHotOrNewCtrl = static_cast<CControlUI*>(m_PaintManager.FindSubControlByName(pListElement,_T("flag_HotOrNew")));
	if(pHotOrNewCtrl)
	{
		CString strImg;
		if(pNameInfo->m_byHotOrNew == 1)
		{
			strImg.Format(_T("file='../../game/hot.png' source='0,0,40,40'"));
		}
		else if (pNameInfo->m_byHotOrNew == 0)
		{
			strImg.Format(_T("file='../../game/new.png' source='0,0,40,40'"));
		}
		else
		{
			strImg.Format(_T("file='' source='0,0,40,40'"));
		}
		pHotOrNewCtrl->SetBkImage(strImg);

		
	}
	CString cs;
	cs.Format("chs [Client] GameID=%d , HotOrNew = %d",pNameInfo->uNameID,pNameInfo->m_byHotOrNew);
	OutputDebugString(cs);

	//插入到最后
	int index = 0;
	if (0 < parent->has_children())
	{
		Node* prev = parent->get_last_child();
		index = prev->data().list_elment_->GetIndex() +1;	
	}

	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}
	parent->add_child(node);

	m_mapNameNode[pNameInfo->uNameID] = node;
	return node;
}

Node* CGameListUI::AddRoomNode(ComRoomInfo* pRoomInfo, Node* parent/* =NULL */)
{
	if (!pRoomInfo || !parent)
	{
		return NULL;
	}

	//判断该房间是否已经添加

	CRoomElementUI* pListElement = NULL;
	CDialogBuilder dlgduild;
	CDialogBuilderCallbackEx cb(m_PaintManager);
	pListElement = static_cast<CRoomElementUI*>(dlgduild.Create(_T("left/RoomElement.xml"), (UINT)0, &cb, &m_PaintManager));
	if (pListElement == NULL)
		return NULL;

	//保存分组数据
	pListElement->SetData(pRoomInfo);

	//新建分组节点
	Node* node = new Node;
	node->data().uDataType = GLK_GAME_ROOM;
	node->data().child_visible_ = true;
	node->data().list_elment_ = pListElement;

	if (!parent->data().child_visible_)
		pListElement->SetVisible(true);
	
	//保存节点数据
	pListElement->SetTag((UINT_PTR)node);

	CButtonUI* pBtn = static_cast<CButtonUI*>(m_PaintManager.FindSubControlByName(pListElement, _T("Btn_Join")));
	if (pBtn)
	{
		pBtn->SetTag((UINT_PTR)node);
	}

	pListElement->Refresh();

	//插入到最后
	int index = 0;
	if (parent->has_children())
	{
		Node* prev = parent->get_last_child();
		index = prev->data().list_elment_->GetIndex() +1;
	}
	else
	{
		if (parent == root_node_)
			index = 0;
		else
			index = parent->data().list_elment_->GetIndex() + 1;
	}

	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}
	parent->add_child(node);
	return node;
}

void CGameListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
	m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);	
}