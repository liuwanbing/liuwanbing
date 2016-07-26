#pragma once
#include "explorer1.h"
#include "Stdafx.h"


// CIEAdvertise 对话框

class CIEAdvertise : public CLoadFileBmp
{

public:
	CIEAdvertise();   // 标准构造函数
	virtual ~CIEAdvertise();



public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
public:
	afx_msg void OnPaint();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	CExplorer1 m_ie;
	CNormalBitmapButtonEx m_btClose;
};
