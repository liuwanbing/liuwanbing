#pragma once

#include "Stdafx.h"
#include "Resource.h"

//有事离开对话框类
class CHaveThing : public CGameFaceGo
{
//	DECLARE_DYNAMIC(CHaveThing)

	//变量定义
public:
	CNormalBitmapButtonEx				m_btOk;							//确定按钮
	CNormalBitmapButtonEx				m_btCancel;						//取消按钮
	//CButton		m_btOk;
	//CButton		m_btCancel;
	enum					{ IDD = IDD_THING };			//对话框数据
	CWnd *m_pParnet;
	CGameImage			m_bk;
	bool	isclick;
	//函数定义
public:
	//构造函数
	CHaveThing();   
	//析构函数
	virtual ~CHaveThing();
	//初始化函数
	virtual BOOL OnInitDialog();
	virtual BOOL ShowWindow(int nCmdShow);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnSetfocusEdit();
protected:
	//取消函数
	virtual void OnCancel();
	//确定函数
	virtual void OnOK();
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
