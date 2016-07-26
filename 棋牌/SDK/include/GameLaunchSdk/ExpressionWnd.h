#pragma once

#ifndef __EXPRESSIONWND__H__
#define __EXPRESSIONWND__H__

#include "StdAfx.h"
#include <map>
#include <list>
using namespace std;

//#include "F:\v3.02_0928\GameLauncher\DllForGame\NewGameFrame\XLogic.h"

#define MAX_EXPRESSION_NUM	70

class CExpressionWnd : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI
{
public:
	CExpressionWnd();

	virtual ~CExpressionWnd(void);

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

	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	CString GetSelExpression();

	void SetParentWnd(BzDui::CWindowWnd *pParent);

	static HHOOK m_hMHook;

	static CExpressionWnd* m_pInstance;

	//功能按钮
private:
	void OnBnClickedBt1();

	void SetExpression(const CString& strExpression);

	//加载常规控件皮肤
	void CreateExpressionBtns();

	//隐藏
	void MakeExpressionUnvisible();

public:
	//void DestroyExpressBtn();

private:
	BzDui::CPaintManagerUI		m_pm;
	//BzDui::CButtonUI *m_pBtnExpression[MAX_EXPRESSION_NUM];
	std::vector<BzDui::CButtonUI *> m_vecExpressionBt;
	CString m_strSelExpression;
	BzDui::CWindowWnd *m_parentWnd;
};

#endif
/*******************************************************************************************************/