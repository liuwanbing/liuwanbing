#pragma once

// CDlgInMoney dialog
#ifndef DLGINMONEY_HEAD_H
#define DLGINMONEY_HEAD_H

#include "SkinMgr.h"

class CDlgInMoney : public CDialog
{
	DECLARE_DYNAMIC(CDlgInMoney)

public:
	CDlgInMoney(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInMoney();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
// Dialog Data
//	enum { IDD = IDD_DLG_CHECKIN };
	CNormalBitmapButtonEx	m_BtCancel;
	CNormalBitmapButtonEx	m_btOk;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// È¡³öÇ®±ÒÊý
	UINT uCheckInMoney;
public:
	CString m_RoomCheckIn;
	CGameImage		m_bkimage;
	HBRUSH m_bkBrush;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
private:
	void Init();

private:
	CSkinMgr m_skinmgr;
};

#endif