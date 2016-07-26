#pragma once


// CTransparentButton

class CTransparentButton : public CButton
{
	DECLARE_DYNAMIC(CTransparentButton)

public:
	CTransparentButton();
	virtual ~CTransparentButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT uCtlColor);
	afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
};


