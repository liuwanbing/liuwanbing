#pragma once

#include "resource.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" //2012.10.15 yyf
// CSetFriend 对话框

class CSetFriend : public CDialog
{
	DECLARE_DYNAMIC(CSetFriend)

public:
	CSetFriend(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetFriend();

	int m_iType;

// 对话框数据
	enum { IDD = IDD_DIALOG11 };

	CGameImage		m_bkimage;
	HBRUSH			m_bkBrush;
	
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.15 yyf
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual void OnOK();

	void SetType(int Type);

	CNormalBitmapButtonEx  m_Ok;
	CNormalBitmapButtonEx  m_Cancel;
	CNormalBitmapButtonEx  m_Close;


	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedClose();

private:
	CSkinMgr m_skinmgr;
private:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

	void Init();

	void LoadSkin();
public:
	// 设置窗口圆角 2012.10.15 yyf
	void SetWndRgn(void);
};
