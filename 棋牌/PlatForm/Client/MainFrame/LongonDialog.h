#pragma once
#ifndef LOGONDIALOG_HEAD_H
#define LOGONDIALOG_HEAD_H

#include "Stdafx.h"
#include "GamePlace.h"
#include "LogoSelect.h"
#include "RegisterNewUserDlg.h"
#include "NewControl\\TStatic.h"
#include "SoftKey.h"
//#include "NewControl\\TStatic.h"
//#include "NewControl\\MacButtons.h"
//#include "NewControl\\BtnST.h"

/*******************************************************************************************************/

//登陆方式
#define LK_BY_NAME						0									//名字登陆
#define LK_BY_ACC						1									//ACC 登陆
#define LK_BY_MOBILE					2									//手机登陆

//登陆对话框类
class CLongonDialog : public CLoadFileBmp
{
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
	static TCHAR						m_szName[61];						//用户名字
	static TCHAR						m_szScrPass[61];					//用户原密码
	static char							m_szMD5Pass[50];					//加密后的密码
	static char							m_szRegSQL[210];					//",email='123'"//用户注册附加信息的SQL
	static int							m_dwAccID;							//ACC 号码
	static int							m_dwMobile;							//手机号码
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

	int									logadvx;							//IE广告x
	int									logadvy;							//IE广告y
	int									logadvw;							//IE广告宽度
	int									logadvh;							//IE广告高度
	CString								szLogAdvUrl;						//IE广告地址，当它为空时表示没有

	CSoftKey						*	hSoftKey;

	bool								m_bisgetid;
	bool								m_bshow;
	TCHAR m_szUrl[300];
	TCHAR m_szcpUrl[300];
	TCHAR m_szerrUrl[300];
	TCHAR m_code[100];
	CString m_regurl;
	//控制信息
	UINT								m_uMaxHide;							//最大高度
	UINT								m_uMaxWidth;						//最大宽度
	CRect								m_NormalWindowRect;					//正常窗口
	
	//控件变量
public:
	CTrueEdit							m_PassWord;							//密码框
	CLogoSelect							m_LogoSelect;						//头像选择
	CNormalBitmapButtonEx				m_btQuit;							//退出按钮
	CNormalBitmapButtonEx				m_btLogon;							//登陆按钮
	CNormalBitmapButtonEx				m_btRegister;						//申请按钮
	//CNormalBitmapButtonEx				m_btUpdateList;						//更新列表
	CNormalBitmapButtonEx				m_btDeleteName;						//删除名字
	CNormalBitmapButtonEx				m_btLogoSelect;						//头像选择
	CNormalBitmapButtonEx				m_btFindPassword;						//头像选择
	CNormalBitmapButtonEx				m_btApplyPassword;						//头像选择
	CNormalBitmapButtonEx				m_btNewHelp;						//新手帮助

	CNormalBitmapButtonEx				m_btSoftkey;

	CToolTipCtrl						m_ToolTip;							//工具提示
	CIEClass							m_Web;
	//CTransparentStatic					m_static_1;
	CTransparentStatic					m_static_2;
	CTransparentStatic					m_static_3;
	CTransparentStatic					m_static_Name;
	CTransparentStatic					m_state10;
	CTransparentStatic					m_state5;
	CButton							m_check_1;
	//CButton							m_check_2;
	CButton							m_check_2;
	CHyperLink						m_copyright;
	CGameImage						m_bkimage;
	COLORREF						m_bkcolor;
	//函数定义	
public:
	//构造函数
	CLongonDialog();   
	//析构函数
	virtual ~CLongonDialog();
	//初始化函数
	virtual BOOL OnInitDialog();
	HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF transparentColor);
protected:
	//确定
	virtual void OnOK();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);    
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	DECLARE_MESSAGE_MAP()

	//内部函数
private:
	//读取注册表信息
	bool LoadRegisterData(); //OK
	//设置登陆模式
	bool SetLogonType(BYTE bNewLogonType, BYTE bOldLogonType); //OK HALF
	//更新密码
	bool UpdatePassWord(CString strInputInfo); //OK
	//填写用户
	static bool FillUserName(CComboBox * pComBoxName);  //OK
	//填写登陆服务器
	static bool FillServer(CComboBox * pComBoxServer); //OK
	//获取密码信息
	static CString GetUserPass(const TCHAR * szKindInfo, UINT uType); //OK

	//功能函数
public:
	//写入密码信息
	static bool WriteUserPass(const TCHAR * szKindInfo, TCHAR * szPass, UINT uType); //OK
	//生成密码
	static CString EncryptPass(const TCHAR * szPass); //OK
	//解开密码
	static CString CrevassePass(const TCHAR * szEncryPass); //OK

	//消息函数
public:
	//表情消息
	LRESULT OnHitLogoMessage(WPARAM wparam, LPARAM lparam);
	//申请新用户
	afx_msg void OnBnClickedRegUser();
	//使用代理服务器
	//afx_msg void OnBnClickedUseProxy();
	////设置代理是否使用安全验证
	//afx_msg void OnBnClickedUsePass();
	//帐号登陆
	afx_msg void OnBnClickedAccounts();
	////ACC 登陆
	//afx_msg void OnBnClickedAccID();
	////手机登陆
	//afx_msg void OnBnClickedMobile();
	//选择改变
	afx_msg void OnCbnSelchangeInput();
	//删除按钮
	afx_msg void OnBnClickedDelete();
	//设置头像
	afx_msg void OnBnClickedSelectLogo();
	//删除按钮
	afx_msg void OnBnClickedFindPassword();

	//新手帮助
	afx_msg void OnBnClickedNewHelp();
	
	//设置头像
	afx_msg void OnBnClickedApplyPassword();
	afx_msg void OnBnClickedSoftkey();
	//
	afx_msg void OnPaint();
protected:
	virtual void OnCancel();
//public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
//	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void LoadSkin(void);
#ifdef AI_MACHINE
	void AILogin();
#endif
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#endif
/*******************************************************************************************************/
