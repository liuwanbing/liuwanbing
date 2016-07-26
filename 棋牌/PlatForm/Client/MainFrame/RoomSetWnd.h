#pragma once

#ifndef ROOMSETWND_HEAD_H
#define ROOMSETWND_HEAD_H

#include "StdAfx.h"

//接受邀请模式
#define INVALI_ALL					0			//接受所有邀请
#define INVALI_ONLY_FRIEND			1			//只接受朋友邀请
#define INVALI_NO					2			//不接收任何邀请

class CRoomSetWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CRoomSetWnd();
	virtual ~CRoomSetWnd(void);

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

//控件变量
public:

	bool								m_bAllowSetDesk;					//是否允许设置桌面
	BYTE								m_bSaveTalk;						//保存记录
	BYTE								m_bShowUser;						//显示用户
	BYTE								m_bShowMessage;						//显示信息

public:
	BYTE								m_bSameIP;							//相同 IP
	BYTE								m_bSameIPNumber;					//不于前几位IP相同的玩家游戏
	BYTE								m_bPass;							//设置密码
	BYTE								m_bLimitCut;						//限制断线
	BYTE								m_bLimitPoint;						//限制经验值
	BYTE								m_bCutPercent;						//逃跑率
	BYTE								m_bInvaliMode;						//邀请模式
	TCHAR								m_szPass[61];						//桌子密码
	long int							m_dwLowPoint;						//最低经验值（积分）
	long int							m_dwHighPoint;						//最高经验值（积分）
	long int							m_dwLowMoney;						//最低经验值（金币）
	long int							m_dwHighMoney;						//最高经验值（金币）
	static bool								m_bPlayBkMusic;						//播放背景音乐

	int									m_nRetFlag;							//1标示确定，0表示取消
};





#endif
/*******************************************************************************************************/