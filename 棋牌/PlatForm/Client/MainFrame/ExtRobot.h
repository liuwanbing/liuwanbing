#pragma once

#include "resource.h"
// CExtRobot 对话框

class CExtRobot : public CDialog
{
	DECLARE_DYNAMIC(CExtRobot)

public:
	CExtRobot(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExtRobot();

// 对话框数据
	enum { IDD = IDD_EXTROBOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_extRobot;
public:
	afx_msg void OnBnClickedOk();
};
