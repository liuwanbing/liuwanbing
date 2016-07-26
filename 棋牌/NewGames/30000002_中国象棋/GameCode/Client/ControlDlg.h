#pragma once

#include "StdAfx.h"
#include "Resource.h"

//控制对话框
class CControlDlg : public CAFCRgnDialog
{
	//变量定义
private:
	long int							m_dwMaxMoney;					//最大金币

	//变量定义
public:
	CAFCPicButton						m_BtPoint1;						//筹码按钮
	CAFCPicButton						m_BtPoint5;						//筹码按钮
	CAFCPicButton						m_BtPoint10;					//筹码按钮
	CAFCPicButton						m_BtPoint50;					//筹码按钮
	CAFCPicButton						m_BtPoint100;					//筹码按钮
	CAFCPicButton						m_BtPoint500;					//筹码按钮
	CAFCPicButton						m_BtFollow;						//跟注按钮
	CAFCPicButton						m_BtSelect;						//选注按钮
	CAFCPicButton						m_BtBegin;						//开始按钮
	CAFCPicButton						m_BtGiveUp;						//放弃按钮
	CAFCPicButton						m_BtShowHand;					//梭哈按钮
	CWnd								* m_pHandleWnd;					//处理窗口

	//函数定义 
public:
	//构造函数
	CControlDlg();
	//析构函数
	virtual ~CControlDlg();
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel() { return; }

	DECLARE_MESSAGE_MAP()

	//重载函数
protected:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//功能函数
public:
	//设置窗口
	void SetHandleWnd(CWnd * pHandleWnd) { m_pHandleWnd=pHandleWnd; } ;
	//设置启用
	void SetEnableControl(bool bEnable, bool bShowHand);
	//设置金币
	void SetMaxMoney(long int dwMaxPoint);

	//消息函数
public:
	//开始
	void OnBegin();
	//位置函数
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
