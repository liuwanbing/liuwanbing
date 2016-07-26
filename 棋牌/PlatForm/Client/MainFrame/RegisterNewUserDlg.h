#pragma once
#ifndef REGISTERNEWUSERDLG_HEAD_H
#define REGISTERNEWUSERDLG_HEAD_H

#include "Stdafx.h"
#include "Resource.h"
#include "SkinMgr.h"
#include "explorer1.h"
//#include "NewControl\\MacButtons.h"

// CRegisterNewUserDlg 对话框

class CRegisterNewUserDlg : public CLoadFileBmp
{
public:
	CRegisterNewUserDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegisterNewUserDlg();
	CString m_regname;
	CString m_regID;


	HBRUSH m_bkBrush;
// 对话框数据
	enum { IDD = IDD_REGISTER_NEW_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CNormalBitmapButtonEx		m_btQuit;
public:
	CGameImage					m_bk;
	CGameImage					m_bt;
	COLORREF					m_bkcolor;

	//表情消息
	LRESULT OnHitLogoMessage(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedCancel();

	//	afx_msg void OnPaint();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	void LoadSkin(void);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	CExplorer1 m_ie;
public:
	DECLARE_EVENTSINK_MAP()
public:
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
public:
	void NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);

private:
	CSkinMgr m_skinmgr;
};

#endif
