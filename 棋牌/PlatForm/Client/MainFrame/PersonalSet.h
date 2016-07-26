#pragma once
#include "resource.h"
#include "explorer1.h"
#include "afxwin.h"
#include "SkinMgr.h"

// CPersonalSet 对话框

class CPersonalSet : public CDialog
{
	DECLARE_DYNAMIC(CPersonalSet)
	CWnd		*pParentWnd;
	Image		*imgDialog;
	COLORREF	m_bkcolor;
	CString		m_strSetUrl;


public:
	CPersonalSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPersonalSet();

// 对话框数据
	enum { IDD = IDD_PERSONALSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExplorer1 m_ie;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	virtual BOOL OnInitDialog();
public:
	void setSkinIndex(int sk);
public:
	CNormalBitmapButtonEx m_bnClose;
public:
	afx_msg void OnPaint();
public:
	void drawSkin(void);
public:
	void openPersonalSet(void);
public:
	afx_msg void OnBnClickedClose();
private:
	CSkinMgr m_skinmgr;
};
