#pragma once

#ifndef NAVIGATIONDLG_HEAD_H
#define NAVIGATIONDLG_HEAD_H

#include "StdAfx.h"
#include "GamePlace.h"

//宏定义
#define NAVIGATE_DLG_HIGH							52

//类说明
class CNavigationDlg;

/*******************************************************************************************************/

//按钮属性
class CNavButtonInfo
{
	//友元定义
	friend class CNavigationDlg;

	//变量定义
protected:
	UINT								m_uButtonHigh;						//按钮高度
	UINT								m_uButtonWide;						//按钮宽度
	CNormalBitmapButtonEx						* m_pPicButton;						//按钮指针

	//函数定义
protected:
	//构造函数
	CNavButtonInfo() { m_uButtonHigh=0; m_uButtonWide=0; m_pPicButton=NULL; };
	//析构函数
	~CNavButtonInfo() { delete m_pPicButton; };
};

/*******************************************************************************************************/

//导航窗口类
class CNavigationDlg : public CGameFaceGo
{
	//变量定义
public:
	CWnd								* m_pParentWnd;						//父窗口
	CGameImage							m_BackImage;						//背景图
	CPtrArray							m_ButtonPtrArray;					//按钮指针

	//函数定义
public:
	//构造函数
	CNavigationDlg(CWnd * pParentWnd);
	//析构函数
	virtual ~CNavigationDlg();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel() { return; }

	//功能函数
public:
	//获取窗口高度
	inline UINT GetWindowHigh() { return NAVIGATE_DLG_HIGH; };
	//加入按钮
	bool AddButton(UINT uButtonWide, UINT uButtonHigh, HINSTANCE hInstance, UINT uButtonID);
	//调整位置
	void FixControlSize();

	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
};

#endif
/*******************************************************************************************************/