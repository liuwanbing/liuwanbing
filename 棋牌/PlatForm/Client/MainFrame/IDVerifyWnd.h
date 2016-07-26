#pragma once

#ifndef IDVerifyWnd_HEAD_H
#define IDVerifyWnd_HEAD_H

#include "StdAfx.h"
#include "IDKeyBoardWnd.h"
#include "..\uilibrary2003\VerifyCodeCreater.h"
#include "SkinMgr.h"

class CIDVerifyWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CIDVerifyWnd();
	virtual ~CIDVerifyWnd(void);

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

	//虚拟键盘按键
	LRESULT OnVirtualKey(UCHAR uc,UINT nType);


	void OnOK();

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

public:

	BzDui::CPaintManagerUI m_pm;

//控件定义
protected:

	//身份证输入框
	BzDui::CEditUI* m_pIDCardEdt;

	//验证码输入框
	BzDui::CEditUI* m_pVCodeEdt;

	//数据键盘窗口
	CIDKeyBoardWnd* m_pIDKeyBoardWnd;

	//验证码生成器
	CVerifyCodeCreater* m_pVCCreater;

public:

	//验证信息
	MSG_GP_R_IDVerify* m_pVerifyInfo;

	//1标示确定，0表示取消
	int m_nRetFlag;							

private:
	CSkinMgr m_skinmgr;
};





#endif
/*******************************************************************************************************/