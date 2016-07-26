#pragma once

//#include "Resource.h"
// CDlgOutMoney dialog
#ifndef DLGOUTMONEY_HEAD_H
#define DLGOUTMONEY_HEAD_H
#include "SkinMgr.h"
class CDlgOutMoney : public CDialog
{
	DECLARE_DYNAMIC(CDlgOutMoney)

public:
	CDlgOutMoney(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOutMoney();

	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Dialog Data
	//enum { IDD = IDD_DLG_CHECKOUT };
	CNormalBitmapButtonEx	m_BtCancel;
	CNormalBitmapButtonEx	m_btOk;

	HBRUSH m_bkBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// 取出钱数　
	UINT uCheckOutMoney;
public:
	CString m_RoomCheckOut;
	CGameImage		m_bkimage;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

private:
	void Init();
};

#endif