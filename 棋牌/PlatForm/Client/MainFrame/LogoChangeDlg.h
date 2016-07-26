#pragma once

#ifndef LogoChangeWnd_HEAD_H
#define LogoChangeWnd_HEAD_H


#include "StdAfx.h"

#define MSG_SET_LOGO	WM_USER+1314

class CLogoChangeDlg : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI
{

public:

	CLogoChangeDlg();
	virtual ~CLogoChangeDlg(void);

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
	
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

public:		
	int GetIndex(){return m_iIndex;}

	void SetIndex(int index){m_iIndex = index;}

	int GetBtnWidth(){return m_iBtnWidth;}

	int GetBtnHeight(){return m_iBtnHeight;}

	int GetBtnSpace() {return m_iSpace;}
	
private:

	BzDui::CPaintManagerUI m_pm;

	BzDui::CControlUI *m_pLogoFrame;
	
	int m_iColNumMax;
	int m_iSpace;
	int m_iBtnWidth;
	int m_iBtnHeight;

	int m_iIndex;
};
#endif
/*******************************************************************************************************/
