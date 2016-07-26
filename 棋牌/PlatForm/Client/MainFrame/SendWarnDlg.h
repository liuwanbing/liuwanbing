#pragma once
#ifndef SENDWARNDLG_HEAD_H
#define SENDWARNDLG_HEAD_H

#include "resource.h"
#include "stdafx.h"

// CSendWarnDlg dialog

class CSendWarnDlg : public CDialog
{
	DECLARE_DYNAMIC(CSendWarnDlg)

public:
	CSendWarnDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSendWarnDlg();
	int		m_targetid;
	CString	m_name;
	CString	m_warnmsg;
// Dialog Data
	enum { IDD = IDD_WARNNING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};

#endif