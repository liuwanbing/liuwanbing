#pragma once

#ifndef DLGLEFTSHOW_HEAD_H
#define DLGLEFTSHOW_HEAD_H

#include "Stdafx.h"
#include "PublicDefine.h"
#include "Resource.h"
#include "AFCButton.h"
#include "AFCFunction.h"
#include "AFCImage.h"
#include "AFCResource.h"
#include "SkinMgr.h"

// CDlgLeftShow 对话框
#define WM_CLOSE_GAME				WM_USER+30

class EXT_CLASS CDlgLeftShow : public CDialog
{
	DECLARE_DYNAMIC(CDlgLeftShow)
	HBRUSH m_bkBrush;

public:
	bool Showing;
	CDlgLeftShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLeftShow();
	int m_time;
	void GetNickName(TCHAR * nickName);
	void BeginTimer();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();


	CGameImage						m_bkimage;
	CNormalBitmapButtonEx			m_btOk;
// 对话框数据
	//enum { IDD = IDD_SHOW_LEFT };
	public:
	CString m_nickName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CSkinMgr m_skinmgr;

public:
	void LoadSkin();
};
#endif