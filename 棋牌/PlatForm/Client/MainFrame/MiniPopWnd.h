#pragma once

#ifndef MiniPopWnd_HEAD_H
#define MiniPopWnd_HEAD_H

#include "StdAfx.h"

class CMiniPopWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CMiniPopWnd(CWnd* pParent);
	virtual ~CMiniPopWnd(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

	void CenterWindow();

public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	bool OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

public:

	//设置弹窗消息
	void SetMessage(const TMailItem* pPopWndMsg);

	//移动窗口到右下角
	void MoveWindowToRightBottom();

public:

	BzDui::CPaintManagerUI m_pm;

//控件定义
protected:

public:

	//1标示确定，0表示取消
	int m_nRetFlag;			

private:

	//父窗口指针
	CWnd* m_pRoom;

	//弹窗消息
	TMailItem m_tagMailMsg;

	//透明度变化值
	int m_iTransparent;

};





#endif
/*******************************************************************************************************/