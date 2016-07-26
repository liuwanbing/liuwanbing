#pragma once

#ifndef MANAGEDLG_HEAD_H
#define MANAGEDLG_HEAD_H

#include "StdAfx.h"
#include "Resource.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
class CGameRoomEx;

/******************************************************************************************************************/

//玩家权限控制类
class CManageDlg : public CGameFaceGo//CAFCBaseDialog
{
	//变量定义
protected:
	long int						m_dwUserID;							//用户 ID
	GameInfoStruct					* m_pGameInfo;						//游戏信息

	HBRUSH m_bkBrush;

	//控件变量
public:
//	CAFCPicButton					m_BtOK;								//应用按钮
	CNormalBitmapButtonEx					m_BtOK;								//应用按钮
	CNormalBitmapButtonEx					m_BtCancel;							//取消按钮
	CGameImage						m_bk;
	enum						{ IDD = IDD_MANAGE_DLG };
	
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
	DECLARE_MESSAGE_MAP()

	//函数定义
public:
	//构造函数
	CManageDlg(CWnd * pParent, GameInfoStruct * pGameInfo);
	//析构函数
	virtual ~CManageDlg();

	//重载函数
protected:
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消函数
	virtual void OnCancel();

	//功能函数
public:
	//设置管理用户
	void SetManageUser(UserItemStruct * pUserItem);
public:
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnPaint();

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara); 
	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
};
#endif

/******************************************************************************************************************/
