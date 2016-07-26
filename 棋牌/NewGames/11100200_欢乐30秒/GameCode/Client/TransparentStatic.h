// TransparentStatic.h: Headers and defines for CTransparentStatic
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TRANSPARENTSTATIC_H__INCLUDED_)
#define _TRANSPARENTSTATIC_H__INCLUDED_


// CTransparentStatic

class CTransparentStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStatic)

public:
	CTransparentStatic();
	virtual ~CTransparentStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT uCtlColor);
	afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
};
#endif // !defined(_TRANSPARENTSTATIC_H__INCLUDED_)