#pragma once


// CEdit2

class CEdit2 : public CWnd
{
	DECLARE_DYNAMIC(CEdit2)

public:
	CEdit2();
	virtual ~CEdit2();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


