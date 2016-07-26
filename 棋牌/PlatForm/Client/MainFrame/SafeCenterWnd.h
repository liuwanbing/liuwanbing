#pragma once

#ifndef SafeCenterWnd_HEAD_H
#define SafeCenterWnd_HEAD_H

#include "StdAfx.h"
#include <map>
#include <list>
using namespace std;

#include "SkinMgr.h"
class CSafeCenterWnd;

typedef bool (CSafeCenterWnd::*lpOperateFun)(LPCVOID);	//检测项目操作标准函数指针类型

class CSafeCenterWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CSafeCenterWnd();
	virtual ~CSafeCenterWnd(void);

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

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void OnOK();

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//检测用户信息是否为安全，返回false为不安全，返回true为安全
	static bool CheckSafe(const MSG_GP_UserInfo* pUserInfo);

//安全检测项目的操作函数
public:

	//锁机设置操作
	bool OnOperateLockMachine(LPCVOID lpData);

	//身份验证操作
	bool OnOperateSetIDVerify(LPCVOID lpData);

	//手机绑定验证操作
	bool OnOperateBindMobileVerify(LPCVOID lpData);

private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

public:

	BzDui::CPaintManagerUI m_pm;

	//用户信息
	MSG_GP_UserInfo* m_pUserInfo;

	//一个检测项目
	struct TCheckNode
	{
		std::string name;    //项目名
		std::string opText;	 //操作文本，按作按钮上的文本
		std::string norText; //常态文本，按作按钮上的文本，不能点击的
		std::string bntName; //绑定的操作按钮名
		lpOperateFun OpFun;	 //操作执行函数
		bool bOperated;		 //用户是否已完成该项目的设置
	};

	enum CheckItemType
	{
		CIT_LockMachine = 0, //锁机检测
		CIT_IDVerify	= 1, //身份证检测
		CIT_BindMobile	= 2, //手机绑定检测
	};

	map<CheckItemType,TCheckNode> m_mapSCTable;

public:

	//设置某一安全的状态
	void SetItemSafeState(CheckItemType emType,bool bSafeType);

//控件定义
protected:

public:

	//1标示确定，0表示取消
	int m_nRetFlag;							

private:
	CSkinMgr m_skinmgr;
};





#endif
/*******************************************************************************************************/