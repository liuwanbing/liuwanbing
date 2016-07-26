#pragma once

#ifndef PBank4GameWnd_HEAD_H
#define PBank4GameWnd_HEAD_H

#include "StdAfx.h"

class CPBank4GameWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CPBank4GameWnd(CWnd* pParent,GameInfoStructInExe* pGameInfo);
	virtual ~CPBank4GameWnd(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

	void CenterWindow();

	void ShowWindow(bool bShow = true, bool bTakeFocus = true);

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

	//存入OK
	void OnInOK();

	//取出OK
	void OnOutOK();

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

public:

	/// 设置银行版本
	void SetBankVersion(int nVer);

	/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
	/// 函数里不对指针的合法性作判断
	/// @param szMD5Pass 加密后的字符串
	/// @param szSrcPass 源字符串
	/// @return 加密后的字符串指针
	static TCHAR* GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass);

	///
	/// 设置财富信息
	/// @param nBank 银行存款
	/// @param nWallet 手中现金
	/// @return void
	///
	void SetWealthInfor(__int64 i64Bank, __int64 i64Wallet);

	///
	/// 设置财富信息
	/// @param nWallet 手中现金
	/// @return void
	///
	void SetWealthInfor(__int64 i64Wallet);

	///
	/// 更新财富信息
	/// @return void
	///
	void UpdateWealthInfor();

	//把数字输入框，在输入过程中同步转换为中文大写
	void ChangeEtToChinese(BzDui::CEditUI* pEditCtrl,BzDui::CTextUI* pTextCtrl);

	///
	/// 提示
	/// @param pToCtrl 在某一控件上提示
	/// @param lpSecName ClientMessage.bcf中的段名
	/// @param lpKeyName ClientMessage.bcf中的键值
	/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
	/// @return void
	///
	static void ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...);

	//切换存取类型
	void OnBnClickedAccessIn();
	void OnBnClickedAccessOut();

	//点击右则的按钮添加金币处理过程
	void AddMoneyToEdtCtrl(BzDui::CEditUI* pEdtUI,__int64 i64Money);

public:

	BzDui::CPaintManagerUI m_pm;

//控件定义
protected:

	//金币设置编辑框
	BzDui::CEditUI* m_pCointEdt;

public:

	//1标示确定，0表示取消
	int m_nRetFlag;

	//银行密码
	char m_szTwoPassword[50];					

private:

	//父窗口指针
	CWnd* m_pRoom;

	//银行版本
	int m_nBankVer;

	//游戏信息
	GameInfoStructInExe* m_pGameInfo;

	bool bFlag;

	//银行里的存款
	__int64 m_i64BankMoney;

	//游戏里钱包的钱
	__int64 m_i64GameWallet;

};





#endif
/*******************************************************************************************************/