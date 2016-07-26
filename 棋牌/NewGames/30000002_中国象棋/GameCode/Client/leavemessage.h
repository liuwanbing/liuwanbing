#pragma once

#include "stdafx.h"
#include "Resource.h"
#include "TransparentStatic.h"

//玩家请求离开消息
class CLeaveMessage : public CGameFaceGo
{

	//变量定义
protected:
	CNormalBitmapButtonEx				m_btOk;							//确定按钮
	CNormalBitmapButtonEx				m_btCancel;						//取消按钮
//	CTransparentStatic					m_Name;
	enum					{ IDD = IDD_THING_MESSAGE };
	CGameImage				m_bk;										//背景图

	//函数定义
public:
	//构造函数
	CLeaveMessage();
	//析构函数
	virtual ~CLeaveMessage();
	//设置信息
	void SetLeaveMessage(TCHAR * szMessage, TCHAR * szUserName);
	//初始化函数
	virtual BOOL OnInitDialog();
	CWnd *m_pParent;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	//取消函数
	virtual void OnCancel();
	//确定函数
	virtual void OnOK();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	CTransparentStc m_LeaveNameC;
};
