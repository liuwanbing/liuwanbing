#pragma once
#include "afxwin.h"
#include "SkinMgr.h"

// CDlgNotify dialog

// class CDlgNotify
// {
// public:
// 	CDlgNotify();
// 	~CDlgNotify();
// };

#if 1
class CDlgNotify : public CDialog
{
	DECLARE_DYNAMIC(CDlgNotify)

public:
	CDlgNotify();
	CDlgNotify(CWnd* pParent/* = NULL*/);   // standard constructor
	virtual ~CDlgNotify();

// Dialog Data
	enum { IDD = IDD_DIALOG_NOTIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Text;
public:
	CString m_TextVar;
	CNormalBitmapButtonEx m_btOK;
	CNormalBitmapButtonEx m_btCancle;
	CGameImage m_backPic;

public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnPaint();

	BOOL OnInitDialog();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedButton1();

private:
	CSkinMgr m_skinmgr;
};

#endif