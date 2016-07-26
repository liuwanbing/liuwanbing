#pragma once

#ifndef DIALOGCALLGM_HEAD_H
#define DIALOGCALLGM_HEAD_H
// CDialogCallGM dialog
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h"

class CDialogCallGM : public CDialog
{
	DECLARE_DYNAMIC(CDialogCallGM)

public:
	CDialogCallGM(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogCallGM();

// Dialog Data
	//enum { IDD = IDD_CALL_GM };
	CNormalBitmapButtonEx	m_btok;
	CNormalBitmapButtonEx	m_btcancel;

	HBRUSH m_bkBrush;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CString szCallGMmsg;
public:
	bool game_error;
	bool court;
	bool publicPoison;
	bool others;
	CGameImage		m_bkimage;
	CconfigDlg  m_configDlg; //º”‘ÿµ◊Õº¿‡. 2012.10.17 yyf
private:
	CSkinMgr m_skinmgr;
private:
	void Init();

	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

	void LoadSkin();
public:
	  // …Ë÷√¥∞ø⁄‘≤Ω« 2012.10.17 yyf
	  void SetWndRgn(void);
};

#endif