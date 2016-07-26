#pragma once
#include "afxwin.h"
#include "resource.h"
#include "gameplacemessage.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
#include <vector>
#include <algorithm>
using namespace std;

#define  WM_TALK_2_GM       WM_USER + 1114 

class CGMListDlg :
	public CDialog
{
public:
	CGMListDlg(CWnd * pParentWnd);

	enum { IDD = IDD_DLG_GMLIST };
public:
	~CGMListDlg(void);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	void OpenWindow(MSG_GP_R_GmList_t list);

private:
	CNormalBitmapButtonEx m_btnCancle;
	CNormalBitmapButtonEx m_btnOk;
	CGameImage            m_imgDlgBk;
	CNormalBitmapButtonEx m_bnClose;

	CListCtrl m_GMListCtrl;
	vector<int> m_GMlist;

	CWnd			* m_pParentWnd;						//处理窗口
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
	afx_msg void OnPaint();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedClose();
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CSkinMgr m_skinmgr;

	void Init();
	void LoadSkin();
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara); 
	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
};
