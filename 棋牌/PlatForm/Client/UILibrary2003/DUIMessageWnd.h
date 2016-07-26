#pragma once

#ifndef MessageWndDu_HEAD_H
#define MessageWndDu_HEAD_H

#include "StdAfx.h"

class CMessageWndDu : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CMessageWndDu();
	virtual ~CMessageWndDu(void);

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

	void OnOK();

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//设置窗口标题
	void SetTitle(const char* szText);

	//设置文本
	void SetMainText(const char* szText,bool bFormat);

	//设置按钮
	void SetStyle(UINT uStyle);

	//设置背景图片
	void SetBKImage();

	//显示窗口
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);

	//设置倒计时
	void SetLeftTime(int nLeftSecond);

public:

	//返回值
	UINT m_uRetFlag;


//控件定义
protected:

	//内容文字
	BzDui::CTextUI* m_pTextMain;

	//内容文字
	BzDui::CTextUI* m_pTextTemp;

	//确定按钮
	BzDui::CButtonUI* m_pOKBtn;

private:

	BzDui::CPaintManagerUI m_pm;

	//背景透明值
	int m_iBkTransparent;

	//对话框样式
	UINT m_uStyle;

	//倒计时间
	int m_nLeftTime;

					
};





#endif
/*******************************************************************************************************/