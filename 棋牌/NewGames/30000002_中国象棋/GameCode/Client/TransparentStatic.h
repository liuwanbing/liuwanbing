// TransparentStatic.h: Headers and defines for CTransparentStc
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TRANSPARENTSTATIC_H__INCLUDED_)
#define _TRANSPARENTSTATIC_H__INCLUDED_


// CTransparentStc

class CTransparentStc : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStc)

public:
	CTransparentStc();
	virtual ~CTransparentStc();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT uCtlColor);
	afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
};
#endif // !defined(_TRANSPARENTSTATIC_H__INCLUDED_)