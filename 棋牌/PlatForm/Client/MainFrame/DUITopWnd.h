#pragma once

#ifndef TopDuWndClass_HEAD_H
#define TopDuWndClass_HEAD_H

#include "StdAfx.h"
#include "QuitAsk.h"
#include "SkinMgr.h"
#include <map>
#include <list>
#include "UIMarqueeText.h"
using namespace std;

class CTopWndClass;
class CGameListWnd;
class CUserInfoWnd;

class CTopDuWndClass : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CTopDuWndClass();
	virtual ~CTopDuWndClass(void);

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

	LRESULT OnExchangeSkin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnChangeHeadPic(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);

	void OnOK();

public:

	void OnHitMin();

	void OnHitSoundSet(bool bPlaySound);

	void OnHitMax();

	void OnHitRestore();

	void OnHitClose();

	void OnHitSkin();

	void OnHitYellow();

	void OnHitBlue();
private:

	//功能按钮
	void OnBnClickedBt1();

	void OnBnClickedBt2();

	void OnBnClickedBt3();

	void OnBnClickedBt4();

	void OnBnClickedBt5();

	void OnBnClickedBt6();

	void OnBnClickedBt7();

	void OnBnClickedBt8();

	void OnBnClickedBt9();

	void OnBnClickedBt10();

	void OnBnClickedBt11();

	void OnBnClickedBt21();

	void OnBnClickedBt22();

	void OnBnClickedBt23();

	void OnBnClickedBt24();

	void OnBnClickedBt25();


public:
	void MakeSkinSelUnVisible();

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//调整关闭、还原、最小化按钮位置
	void FixCtrlBtnPosition(UINT uType,bool bMaxSize);

	//得到通知已经收到flash地址
	void SetFlashAd();

	//获取bcf文件中顶部按钮的窗体最大宽度设置
	int GetTopBtnMaxWidth();

	//获取bcf文件中顶部按钮的窗体最小宽度设置
	int GetTopBtnMinWidth();

	//设置最大化或还原是否可用
	void SetMaxOrRestoreBtn(bool bEnable);
	//显示设置音效按钮
	void	ShowSoundSet();
	//
	void OnUserInfo();
private:
	void ChangeDUISkin();

public:

	BzDui::CPaintManagerUI m_pm;

	//老版本的TopWnd
	CTopWndClass* m_pOldTopWndClass;

	//游戏列表
	CGameListWnd* m_pGameListWnd;

public:
	CQuitAsk								m_quitAskdlg;
	static HHOOK							m_hMHook;
	static CTopDuWndClass					*m_pInstance;
	BzDui::CContainerUI						*m_pExSkinCtn;

	BzDui::CMarqueeTextUI					*m_pMarqueeTextUI;//跑马灯控件
	BzDui::CButtonUI						*m_pbtnUserPic;		//用户图像
	CUserInfoWnd							*m_pUserInfoWnd;				//用户资料管理框
	MSG_GP_R_LogonResult					*m_PlaceUserInfo;			//用户登陆资料
	

private:

	CSkinMgr	m_skinmgr;

	BzDui::CButtonUI* m_pYellowBtn;

	BzDui::CButtonUI* m_pBlueBtn;
};





#endif
/*******************************************************************************************************/