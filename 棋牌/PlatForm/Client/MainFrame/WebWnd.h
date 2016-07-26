#pragma once
#include "explorer1.h"
#include "resource.h"

// CWebWnd 对话框

class CWebWnd : public CDialog
{
	DECLARE_DYNAMIC(CWebWnd)

public:
	CWebWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWebWnd();

// 对话框数据
	enum { IDD = IDD_WEB_WND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExplorer1 m_ie;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void Browse(CString strURL);
};
