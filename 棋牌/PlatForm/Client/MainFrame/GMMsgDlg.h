#pragma once
#include "..\uilibrary2003\confirmdlg.h"

class CGMMsgDlg :
	public ConfirmDlg
{
private:
	int m_sec;
public:
	CGMMsgDlg(CString strInfo, int sec = 5);
public:
	~CGMMsgDlg(void);
public:
	virtual BOOL OnInitDialog();
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnOK();
};
