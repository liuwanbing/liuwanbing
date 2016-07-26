#pragma once
#include "afxwin.h"
#include "MainRoomEx.h"
#include "GameRoomEx.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 


// CDlgRoomFee dialog

class CDlgRoomFee : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoomFee)

public:
	CDlgRoomFee(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRoomFee();
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_ROOMFEE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnBnClickedOk();

public:
	int m_iLeft;
	int m_iTop;
	int m_iRight;
	int m_iBottom;
	bool m_bMouseInRect;
public:
	CNormalBitmapButtonEx m_btOK;
	CNormalBitmapButtonEx m_btCancle;
	CGameImage m_backPic;
	CSkinMgr m_skinmgr;
	CconfigDlg  m_configDlg; //º”‘ÿµ◊Õº¿‡. 2012.10.17 yyf
public:
	CStatic m_RoomInfo;
	DWORD m_dwCount;

	CGameRoomEx *m_pGameRoom;
	CAFCRoomItem *m_pRoomItem;
	CNormalBitmapButtonEx m_BtUrl;
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	  // …Ë÷√¥∞ø⁄‘≤Ω« 2012.10.17 yyf
	  void SetWndRgn(void);
};

