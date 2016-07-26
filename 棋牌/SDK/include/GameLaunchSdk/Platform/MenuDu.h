#pragma once

#ifndef MenuDu_HEAD_H
#define MenuDu_HEAD_H

#include "StdAfx.h"
using namespace std;

class CMenuDu : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CMenuDu(HWND hParent,UINT uID);

	virtual ~CMenuDu(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

public:

	//设置显示位置
	void SetShowPoint(POINT pt);

	//添加菜单项
	void AddMenuItem(char* szName,UINT uMsgID);

	//添加分隔线
	void AddMenuSeparator();

protected:

	//修正菜单窗口的位置
	void FixPosition();

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

public:

	BzDui::CPaintManagerUI m_pm;

private:

	//父窗口的句柄
	HWND m_hParent;

	//菜单ID
	UINT m_uID;

	//菜单显示的起始点坐标
	POINT m_ptPos;

	//列表控件
	BzDui::CListUI* m_pListCtrl;

	struct TMenuNode
	{
		char szName[32];
		UINT uMsgID;		//消息
		TMenuNode()
		{
			::memset(szName,0,sizeof(szName));
			uMsgID = 0;
		}
	};

	//菜单项
	std::vector<TMenuNode> m_vtMenu;

	


};





#endif
/*******************************************************************************************************/