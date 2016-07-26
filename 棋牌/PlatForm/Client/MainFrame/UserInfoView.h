#pragma once
#ifndef USERINFOVIEW_HEAD_H
#define USERINFOVIEW_HEAD_H

#include "StdAfx.h"
#include "Afxcmn.h"

//用户信息框
class CUserInfoView : public CGameFaceGo
{
	//变量定义
protected:
	GetOrderName						* m_pOrderName;						//等级函数

	//控件变量
public:
	CIEHelper							m_FlashView;						//FLASH 窗口
	CNormalBitmapButtonEx						m_BtCancel;							//关闭按钮
	CNormalBitmapButtonEx						m_BtHighInfo;						//详细信息
	CNormalBitmapButtonEx						m_BtUpdateInfo;						//更新资料

	//函数定义
public:
	//构造函数
	CUserInfoView(GetOrderName * pOrderName);
	//析够函数
	virtual ~CUserInfoView();

	//重载函数
public:
	//数据绑定
	virtual void DoDataExchange(CDataExchange * pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定
	virtual void OnOK() { DestroyWindow(); };
	//取消
	virtual void OnCancel() { DestroyWindow(); };

	//功能函数
public:
	//设置用户信息
	void SetUserInfo(UserItemStruct * pUserItem);
	//调整位置
	void FixControlSize(int iWidth, int iHeight);

	DECLARE_MESSAGE_MAP()

	//按钮消息
public:
	//更新按钮
	void OnBnClickedUpdate();
	
	//消息函数
public:
	//绘画消息
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#endif