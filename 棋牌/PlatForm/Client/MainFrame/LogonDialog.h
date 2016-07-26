#pragma once

#ifndef LOGONDLG_HEAD_H
#define LOGONDLG_HEAD_H

#include "StdAfx.h"
#include <vector>

#include "GamePlace.h"
#include "LogoSelect.h"
#include "RegisterNewUserDlg.h"
#include "NewControl\\TStatic.h"
#include "SoftKey.h"
#include "RegSucDlg.h"
#include "SkinMgr.h"

//登陆方式
#define LK_BY_NAME						0									//名字登陆
#define LK_BY_ACC						1									//ACC 登陆
#define LK_BY_MOBILE					2									//手机登陆
#define LK_BY_ID						3									//ID登陆


class CLogonDialog : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI
{

public:

	CLogonDialog();
	virtual ~CLogonDialog(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void InitSkin();

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

	static DWORD __stdcall ThreadDownConnFile(LPVOID param);

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

private:
	//是否同意条款
	bool IsApproveAgreement();

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

//内部函数
private:

	//读取注册表信息
	bool LoadRegisterData();

	//设置登陆模式
	bool SetLogonType(BYTE bNewLogonType, BYTE bOldLogonType);

	//更新密码
	bool UpdatePassWord(CString strInputInfo);

	//填写用户
	static bool FillUserName(BzDui::CComboUI* pComBoxName, BzDui::CEditUI* pEditName = NULL);

	//填写登陆服务器
	static bool FillServer(BzDui::CComboUI* pComBoxName);

	//获取密码信息
	static CString GetUserPass(const TCHAR* szKindInfo, UINT uType);

	//获取登陆时间信息
	static DWORD GetUserLogonTime(const TCHAR* szKindInfo, UINT uType);

//功能函数
public:

	//写入密码信息
	static bool WriteUserPass(const TCHAR* szKindInfo, TCHAR* szPass, UINT uType);

	//写入登陆时间信息
	static bool WriteUserLogonTime(const TCHAR* szKindInfo, UINT uType);

	//生成密码
	static CString EncryptPass(const TCHAR* szPass);

	//解开密码
	static CString CrevassePass(const TCHAR* szEncryPass);

//消息函数
public:

	//申请新用户
	void OnBnClickedRegUser();

	//在线帮助
	void OnBnClickedHelp();

	//QQ在线帮助
	void OnBnClickedQQHelp();

	//使用代理服务器
	//afx_msg void OnBnClickedUseProxy();

	//设置代理是否使用安全验证
	//afx_msg void OnBnClickedUsePass();

	//帐号登陆
	void OnBnClickedAccounts();

	//ACC 登陆
	//afx_msg void OnBnClickedAccID();

	//手机登陆
	//afx_msg void OnBnClickedMobile();
	//选择改变

	//删除按钮
	void OnBnClickedDelete();

	//忘记密码找密码功能按钮
	void OnBnClickedFindPassword();

	//查看服务条款
	void OnBnClickedTermOfService();

	//设置头像
	void OnBnClickedSelectLogo();

	//新手帮助
	void OnBnClickedNewHelp();

	//设置头像
	void OnBnClickedApplyPassword();
	void OnBnClickedSoftkey();

	//用户名登录
	void OnBnClickedLogonByName();
	//用户ID登录
	void OnBnClickedLogonByID();

	void OnOpenIE(int iIndex);
#ifdef AI_MACHINE
	void AILogin();
#endif

public:

	BzDui::CPaintManagerUI m_pm;

	bool								bMouseDown;
	CPoint								savePoint;
	HBRUSH								m_bkBrush;
	//变量定义
public:
	//用户信息
	static bool							m_bInit;							//初始化标志
	static BYTE							m_bRemberPass;						//记住密码
	static BYTE							m_bUseProxy;						//使用代理
	static BYTE							m_bRegUser;							//注册用户
	static BYTE							m_bLogonType;						//登陆模式
	static BYTE							m_bBoy;								//是否男孩
	static UINT							m_bLogoID;							//头像 ID
	static int							m_iUserID;						//用户ID
	static TCHAR						m_szName[61];						//用户名字
	static TCHAR						m_szScrPass[61];					//用户原密码
	static char							m_szMD5Pass[50];					//加密后的密码
	static char							m_szRegSQL[210];					//",email='123'"//用户注册附加信息的SQL
	static int							m_dwAccID;							//ACC 号码
	static int							m_dwMobile;							//手机号码
	static TCHAR						m_szIDCard[64];						//证件编号
	static TCHAR						m_szMobileVCode[8];					//手机验证码
	static TCHAR						m_nickname[100];
	static TCHAR						m_szRecommenderName[61];
	//代理信息
	static BYTE							m_bSock5;							//SOCK 5 标志
	static BYTE							m_bUserCheck;						//需要密码
	static UINT							m_uProxyPort;						//代理端口
	static TCHAR						m_szGameServer[51];					//游戏 IP
	static TCHAR						m_szProxyServer[51];				//代理 IP
	static TCHAR						m_szProxyName[61];					//代理名字
	static TCHAR						m_szProxyPass[61];					//代理密码

	static bool							m_bIsShow;
	static bool							m_bUpdatedConnFile;					//是否已更新服务器列表

	int									logadvx;							//IE广告x
	int									logadvy;							//IE广告y
	int									logadvw;							//IE广告宽度
	int									logadvh;							//IE广告高度
	CString								szLogAdvUrl;						//IE广告地址，当它为空时表示没有

	bool								m_bisgetid;
	bool								m_bshow;
	TCHAR m_szUrl[300];
	TCHAR m_szcpUrl[300];
	TCHAR m_szerrUrl[300];
	TCHAR m_code[100];
	CString m_regurl;

//定义控件
public:

	//ID输入框
	BzDui::CComboUI* m_pUserIDCob;
	BzDui::CEditUI* m_pIDEdit;

	BzDui::CControlUI* m_pBkImgae;
	BzDui::CButtonUI* m_pThreetName;
	BzDui::CButtonUI* m_pThreetID;

	//密码输入框
	BzDui::CEditUI* m_pPwdEdit;

	//记住密码选择框
	BzDui::COptionUI* m_pSavePwd;

	//1标示确定，0表示取消
	int m_nRetFlag;

	//是否支持ID登录
	bool m_bLoginByID;

private:

	//服务器列表结点
	struct TSvrNode
	{
		char szLoginIP[32];
		char szName[64];
		TSvrNode(){::memset(this,0,sizeof(TSvrNode));}
	};

	//服务器列表
	static std::vector<TSvrNode> m_vtServer;

public:
	static bool m_bLogon;

private:
	CSkinMgr m_skinmgr;
	
	void ChangeSkin();
};





#endif
/*******************************************************************************************************/