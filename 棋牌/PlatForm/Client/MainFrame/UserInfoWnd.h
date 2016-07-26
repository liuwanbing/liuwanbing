#pragma once

#ifndef USERINFOWND_HEAD_H
#define USERINFOWND_HEAD_H

#define SMG_HEADPIC_CHANGED		WM_USER + 1315

#include "StdAfx.h"
#include <map>
#include <vector>
using namespace std;

extern class CLogoChangeDlg;
class CUserInfoWnd : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CUserInfoWnd();
	virtual ~CUserInfoWnd(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

	//设置用户数据
	void SetPlaceUserInfo(MSG_GP_UserInfo* pUserInfo);

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

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//显示容器页面
	void ShowPage(int nIndex,int nSubIndex = 0);

	//初始化基本信息页面
	void InitBaseInfo();

	//初始化详细信息页面
	void InitDetail();

	//初始化锁机信息页面
	void InitLockPCInfo();

	//初始化手机绑定页面
	void InitBindMobile();

	//初始化省份地区关系
	static void InitAreaMap();

	//安全清理地区关系
	static void SafeClearAreaMap();

	///
	/// 提示
	/// @param pToCtrl 在某一控件上提示
	/// @param lpSecName ClientMessage.bcf中的段名
	/// @param lpKeyName ClientMessage.bcf中的键值
	/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
	/// @return void
	///
	static void ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...);
	//改变玩家图像
	int OnSetHeadPic(WPARAM wParam, LPARAM lParam);
private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);
	//把城市名字符串初始到vector中
	static vector<LPSTR>* PutInVector(vector<LPSTR>* vec,LPSTR pszCitys);
	//更新城市列表
	void UpdateCitys(int nProvIdx);

public:

	//更新用户的基本信息
	void OnUpdateBaseInfo();

	//更新用户的详细信息
	void OnUpdateDetailInfo();

	//点击更新手机号
	void OnModifyMobileOpen();

	//取消更新手机号
	void OnModifyMobileCancel();

	//确定更新手机号
	void OnModifyMobileOK();

	//进入锁机/解锁导向
	void OnLockOperatePage(bool bLock);

	//进入手机锁绑定导向
	void OnBindMobileOperatePage(bool bLock);

	//锁机/解锁按扭响应
	void OnLockPC(bool bLock);

	//锁机/解锁信息
	void OnLockResult(bool bLock);

	//绑定手机号码按扭响应
	void OnBindMobile(bool bLock);

	//绑定手机号码信息
	void OnBindMobileResult(bool bLock);

	//更新用户密码
	void OnUpdatePassword();

	//点击获取验证码的按钮
	void OnGetMobileVCode(UINT uType,BzDui::CButtonUI* pBtn,BzDui::CEditUI* pMobileNoEdt,BzDui::CEditUI* pVCodeEdt,UINT uTimerID);

	//清理所有重取验证码的定时器
	void ResetReVCodeUI();

	//点击更换头像按钮
	void OnChangeHeaderPic();


public:

	BzDui::CPaintManagerUI m_pm;

	//当前实例窗口句柄
	static HWND m_hInstanceWnd;

	CLogoChangeDlg				*m_pLogoChangeWnd ;	//更换头像
	BzDui::CButtonUI					*m_pBtnHeader;		//头像按钮

private:

	//用户信息
	MSG_GP_UserInfo* m_pUserInfo;

	//省份地区关系
	static map<char*,vector<char*>> m_mapArea;

public:

	//默认显示的页索引
	int m_nDefaultPage;

	//默认显示的子页索引
	int m_nDefaultSubPage;

//定义控件
public:

	//页面容器
	BzDui::CContainerUI* m_pPageContainer[5];

	//锁机设置页面窗口
	BzDui::CContainerUI* m_pLockMachineCtn[2];

	//手机绑定页面
	BzDui::CContainerUI* m_pBindMobileCtn[2];

	//获取手机验证码按钮1(修改用户详细信息tab页中)
	BzDui::CButtonUI* m_pVCodeBtn1;

	//获取手机验证码按钮2(修改锁机设置的tab页中)
	BzDui::CButtonUI* m_pVCodeBtn2;

	//获取手机验证码按钮3(手机绑定的tab页中)
	BzDui::CButtonUI* m_pVCodeBtn3;

private:

	//重取手机验证码的剩余时间
	int m_iReVCodeTime;	

};


#endif
/*******************************************************************************************************/