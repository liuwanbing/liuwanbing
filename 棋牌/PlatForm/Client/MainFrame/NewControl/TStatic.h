#pragma once
#ifndef TSTATIC_HEAD_H
#define TSTATIC_HEAD_H


// CTransparentStatic

class CTransparentStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStatic)

public:
	CTransparentStatic();
	virtual ~CTransparentStatic();
	COLORREF m_clrFontColor;

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetTextColor(COLORREF clrFont);
	afx_msg void OnPaint();
};
#endif


