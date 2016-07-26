#pragma once
#include "afxwin.h"
#include "explorer1.h"
#include "resource.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h"
// ----------------------------------------------------------------------------
//
// PengJiLin, 2011-4-14, 领取金币 对话框
//
// ----------------------------------------------------------------------------

class CGetHelpMoneyDlg : public CGameFaceGo
{
	DECLARE_DYNAMIC(CGetHelpMoneyDlg)

public:
	CGetHelpMoneyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGetHelpMoneyDlg();

public:
    virtual BOOL OnInitDialog();

    // 领取金币 页面
    void SetWebAddr(CString& strWeb);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
    afx_msg void OnPaint();
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedClose();

private:
    CGameImage			m_bk;

    CWnd*               m_pParentWnd;
    HBRUSH              m_bkBrush1;
    HBRUSH              m_bkBrush2;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
public:
// 对话框数据
	enum { IDD = IDD_GETHELPMONEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
    CNormalBitmapButtonEx m_BtClose;
public:
    CIEClass m_WebView;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
	 
	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
private:
	CSkinMgr m_skinmgr;
	void Init();

private:
	void LoadSkin();
};
