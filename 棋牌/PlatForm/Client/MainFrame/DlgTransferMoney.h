#pragma once
#ifndef DLGTRANSFERMONEY_HEAD_H
#define DLGTRANSFERMONEY_HEAD_H

#include "afxwin.h"

//#include "Resource.h"
// CDlgTransferMoney dialog

class CDlgTransferMoney : public CDialog
{
	DECLARE_DYNAMIC(CDlgTransferMoney)

public:
	CDlgTransferMoney(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTransferMoney();

	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Dialog Data
	enum { IDD = IDD_DLG_TRANSFERMONEY };
	CNormalBitmapButtonEx	m_BtCancel;
	CNormalBitmapButtonEx	m_btOk;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// 取出钱数　
	UINT uCheckOutMoney;
public:
	CString m_RoomCheckOut;
	CGameImage		m_bkimage;

	HBRUSH m_bkBrush;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	static TCHAR m_uDestName[61];
public:
	afx_msg void OnBnClickedOk();
};

#endif