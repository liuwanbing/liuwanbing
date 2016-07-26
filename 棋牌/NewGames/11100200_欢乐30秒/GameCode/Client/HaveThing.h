#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "afxwin.h"

// CHaveThing 对话框

class CHaveThing : public CGameFaceGo
{
	DECLARE_DYNAMIC(CHaveThing)

public:
	CHaveThing(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHaveThing();

// 对话框数据
	enum { IDD = IDD_HAVE_THING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	CNormalBitmapButtonEx m_btnOK;          // 确定
	CNormalBitmapButtonEx m_btnCancel;      // 取消

	CGameImage    m_imgBackground;          // 背景图片
	bool	      m_bIsClick;               // 
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnSetfocusEdit();
	afx_msg void OnPaint();
public:
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
};
