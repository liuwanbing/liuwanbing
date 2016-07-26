#pragma once
#include "comstruct.h"
#include "GameListCtrl.h"

class CGameElementUI : public BzDui::CListContainerElementUI
{
public:
	void SetData(ComNameInfo* pNameInfo)
	{
		memcpy_s(&m_NameInfo, sizeof(m_NameInfo), pNameInfo, sizeof(ComNameInfo));
	}

	ComNameInfo* GetNameInfo()
	{
		return &m_NameInfo;
	}

	void Refresh()
	{

	}
private:
	ComNameInfo m_NameInfo;
};


class CRoomElementUI : public BzDui::CListContainerElementUI
{
public:
	void SetData(ComRoomInfo* pRoomInfo)
	{
		ZeroMemory(&m_RoomItem, sizeof(m_RoomItem));

		m_RoomItem.m_RoomInfo = *pRoomInfo;
		m_RoomItem.m_uDataType = GLK_GAME_ROOM;
		m_RoomItem.m_uUpdateTime=(LONG)time(NULL);
		TCHAR szBuffer[100];
		memset(szBuffer,0,sizeof(szBuffer));

		if (pRoomInfo->dwRoomRule&GRR_CONTEST)
		{
			m_RoomItem.m_bIsContest = true;
			m_RoomItem.m_RoomContest.push_back(*pRoomInfo);
		}
	}
	CAFCRoomItem* GetRoomItem()
	{
		return &m_RoomItem;
	}

	void Refresh()
	{
		BzDui::CTextUI* pTxt = static_cast<BzDui::CTextUI*>(m_pManager->FindSubControlByName(this, _T("Txt_RoomName")));
		if (pTxt)
		{
			pTxt->SetText(m_RoomItem.m_RoomInfo.szGameRoomName);
		}

		pTxt = static_cast<BzDui::CTextUI*>(m_pManager->FindSubControlByName(this, _T("Txt_People")));
		if (pTxt)
		{
			CString str = _T("空闲");
			if (5 > m_RoomItem.m_RoomInfo.uPeopleCount)
			{
				str = _T("空闲");
			}
			else if (30 >= m_RoomItem.m_RoomInfo.uPeopleCount && 5 <= m_RoomItem.m_RoomInfo.uPeopleCount)
			{
				str = _T("繁忙");
			}
			else if (50 >= m_RoomItem.m_RoomInfo.uPeopleCount)
			{
				str = _T("拥挤");
			}
			else
			{
				str = _T("爆满");
			}
			pTxt->SetText(str);
		}

	}
private:
	CAFCRoomItem m_RoomItem;
};

struct NodeData
{
	bool child_visible_;	//当前状态子项是否已显示，用于切换分组显示图标（+/-)
	UINT uDataType;
	LONG m_uUpdateTime;	//列表更新时间

	BzDui::CListContainerElementUI* list_elment_;

	NodeData()
	{
		ZeroMemory(this, sizeof(NodeData));
	}
	void* GetData()
	{
		if (GLK_GAME_NAME == uDataType)
		{
			CGameElementUI* pListEle = (CGameElementUI*)list_elment_;
			if (pListEle)
			{
				return pListEle->GetNameInfo();
			}
		}
		else if (GLK_GAME_ROOM == uDataType)
		{
			CRoomElementUI* pListEle = (CRoomElementUI*)list_elment_;
			if (pListEle)
			{
				return pListEle->GetRoomItem();
			}
		}
		return NULL;
	}
};

class Node
{
public:
	Node();
	explicit Node(NodeData t);
	Node(NodeData t, Node* parent);
	~Node();
	NodeData& data();
	int num_children() const;
	Node* child(int i);
	Node* parent();
	bool has_children() const;
	void add_child(Node* child);
	void remove_child(Node* child);
	Node* get_last_child();

private:
	void set_parent(Node* parent);

private:
	typedef std::vector <Node*>	Children;

	Children	children_;
	Node*		parent_;

	NodeData    data_;
};