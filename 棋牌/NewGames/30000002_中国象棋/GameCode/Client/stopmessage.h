#pragma once

#include "stdafx.h"
#include "Resource.h"

//玩家请求离开消息
class CStopMessage : public CGameFaceGo
{

	//变量定义
protected:
	CNormalBitmapButtonEx				m_btOk;							//确定按钮
	CNormalBitmapButtonEx				m_btCancel;						//取消按钮
	enum					{ IDD = IDD_STOP_MESSAGE };
	CGameImage							m_bk;

	//函数定义
public:
	//构造函数
	CStopMessage();
	//析构函数
	virtual ~CStopMessage();
	//初始化函数
	virtual BOOL OnInitDialog();
	CWnd *m_pParent;

protected:
	//取消函数
	virtual void OnCancel();
	//确定函数
	virtual void OnOK();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL ShowWindow(int nCmdShow);
};
