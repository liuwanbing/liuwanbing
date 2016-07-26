#pragma once

#ifndef BankVerifyWnd_HEAD_H
#define BankVerifyWnd_HEAD_H

#include "StdAfx.h"
#include "SkinMgr.h"
#include "..\uilibrary2003\VerifyCodeCreater.h"

class CBankVerifyWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CBankVerifyWnd();
	virtual ~CBankVerifyWnd(void);

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
	BzDui::CEditUI* m_pPwdEdt;

	//验证码输入框
	BzDui::CEditUI* m_pVCodeEdt;

	//验证码生成器
	CVerifyCodeCreater* m_pVCCreater;

public:

	//验证信息
	MSG_GR_UserBank* m_pVerifyInfo;

	//用户信息
	SendMsgUserStruct* m_pMyInfo;

	//验证码的背景颜色
	COLORREF m_crVCodeBk;

	//1标示确定，0表示取消
	int m_nRetFlag;

private:
	//换肤
	CSkinMgr m_skinmgr;
};





#endif
/*******************************************************************************************************/