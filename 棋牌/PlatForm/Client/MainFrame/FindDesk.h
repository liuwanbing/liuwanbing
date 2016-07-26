#pragma once

#include "Stdafx.h"
#include "Resource.h"

//查找位置
class CFindDesk : public CAFCBaseDialog
{
	//变量定义
public:
	bool							m_bFindOne;								//查找条件
	bool							m_bFindTwo;								//查找条件
	bool							m_bFindNoAllNull;						//查找条件
	bool							m_bFindAllNull;							//查找条件
	BYTE							m_bLastFindDesk;						//查找结果

	//控件指针
protected:
	CWnd							* m_pParent;							//大厅窗口

	//控件变量
public:
	CAFCPicButton					m_BtCancel;								//取消按钮
	CAFCPicButton					m_BtSitDown;							//坐下按钮
	CAFCPicButton					m_BtFind;								//查找按钮

	//函数定义
public:
	//构造函数
	CFindDesk(CWnd * pParent);   
	//析构函数
	virtual ~CFindDesk();

	//重载函数
public:
	//控件绑定函数
	virtual void DoDataExchange(CDataExchange* pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//取消
	virtual void OnCancel();
	//确定
	virtual void OnOK();

	//消息函数
protected:
	//坐下
	afx_msg void OnBnClickedSitdown();

	DECLARE_MESSAGE_MAP()
};
