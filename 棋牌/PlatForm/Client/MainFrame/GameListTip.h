#pragma once
#include "resource.h"
#include "afxwin.h"
#include "SkinMgr.h"
// CGameListTip dialog

class CGameListTip : public CDialog
{
	DECLARE_DYNAMIC(CGameListTip)

public:
	CGameListTip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGameListTip();

// Dialog Data
	enum { IDD = IDD_GAMELIST_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CGameImage	m_bkimage;
	HBRUSH		m_bkBrush;
	COLORREF	m_bkcolor;
	static DWORD		m_sCount;
	CNormalBitmapButtonEx	m_bnNext;
	CGameImage m_backPic;
public:
	virtual BOOL OnInitDialog();
	void LoadSkin(int index = 0);
public:
	afx_msg void OnBnClickedButtonNext();
public:
	afx_msg void OnPaint();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

private:
	CSkinMgr m_skinmgr;

	void Init();

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};
