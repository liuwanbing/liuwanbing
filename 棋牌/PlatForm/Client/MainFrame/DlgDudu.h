#pragma once

#ifndef AFC_CDLGDUDU_HEAD_FILE
#define AFC_CDLGDUDU_HEAD_FILE


#include "..\界面库.2003\AFCFunction.h"

// CDlgDudu 对话框

class CDlgDudu : public CDialog
{
	DECLARE_DYNAMIC(CDlgDudu)

public:
	CDlgDudu(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDudu();
	
	CWnd* m_pGameRoom;
// 对话框数据
	//enum { IDD = IDD_DLG_DUDU };

protected:

	bool		m_bCanSend;

	CGameImage	m_bkimage;
	CNormalBitmapButtonEx	m_btcancel;
	CNormalBitmapButtonEx	m_btok;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	afx_msg void OnEnChangeEdit1();

	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	// 输入字符
	CString m_InputStr;
public:
	afx_msg void OnBnClickedCancel();
};
#endif