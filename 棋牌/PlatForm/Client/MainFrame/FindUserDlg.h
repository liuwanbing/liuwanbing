#pragma once

#ifndef FINDUSERDLG_HEAD_H
#define FINDUSERDLG_HEAD_H

#include "Afxcmn.h"
#include "UserListCtrl.h"
#include ".\\newcontrol\\tstatic.h"
#include "BitmapListCtrl.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
//用户查找对话框 
class CFindUserDlg : public CGameFaceGo
{
	//变量定义
protected:
	UINT							m_uComType;						//游戏类型
	CWnd							* m_pParentWnd;						//父窗口

	HBRUSH m_bkBrush1, m_bkBrush2;

	//控件定义
public:
	CTrueEdit						m_UserName;							//用户名字
//	CTrueEdit						m_UserAcc;							//用户 ACC
	CNormalBitmapButtonEx			m_BtFind;							//查找按钮
	CNormalBitmapButtonEx			m_BtCancel;							//关闭按钮
	CNormalBitmapButtonEx			m_BtClose;
	CUserListCtrl					m_UserList;	//用户列表
	CTransparentStatic				m_static1;
	CGameImage						m_bk;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
	//函数定义
public:
	//构造函数
	CFindUserDlg(CWnd * pParentWnd, UINT uComType);
	//析够函数
	virtual ~CFindUserDlg();
	//更新用户信息
	void UpdateUserItem(UserItemStruct * pUserItem);
	//删除用户信息
	void DeleteUserItem(UserItemStruct * pUserItem);

	//重载函数
public:
	//数据绑定
	virtual void DoDataExchange(CDataExchange * pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定
	virtual void OnOK();
	//取消
	virtual void OnCancel() { DestroyWindow(); } 

	//消息函数
protected:
	//单击鼠标右键
	afx_msg void OnNMRclickList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CSkinMgr m_skinmgr;

	void Init();

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara); 
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};
#endif