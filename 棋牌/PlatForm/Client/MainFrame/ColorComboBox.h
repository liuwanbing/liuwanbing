#pragma once


// CColorComboBox

class CColorComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CColorComboBox)

public:
	CColorComboBox();
	virtual ~CColorComboBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


