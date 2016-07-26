#pragma once

#include "resource.h"

#include "IMMain.h"
#include "AddFriend.h"
#include "SetFriend.h" 
#include "..\UILibrary2003\CconfigDlg.h" 

// CIMWnd 对话框

class CIMWnd : public CDialog
{
	DECLARE_DYNAMIC(CIMWnd)

public:
	CIMWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIMWnd();

// 对话框数据
	enum { IDD = IDD_DIALOG9 };

	CIMMain*		m_IMList;
	CGameImage		m_bkimage;
	HBRUSH			m_bkBrush;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedSet();
	afx_msg void OnBnClickedClose();

	//左击IM列表
	afx_msg void OnNMClickIMList(NMHDR *pNMHDR, LRESULT *pResult);
	//右击IM列表
	afx_msg void OnNMRclickIMList(NMHDR * pNMHDR, LRESULT * pResult);
	//双击IM列表
	afx_msg void OnNMDblclkIMList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()

	CNormalBitmapButtonEx  m_Close;
	CNormalBitmapButtonEx  m_Add;
	CNormalBitmapButtonEx  m_Set;
	CconfigDlg  m_configDlg; //加载底图类.
public:
	virtual BOOL OnInitDialog();

	void SetIM(CIMMain* pIM);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	CAddFriend addFriendDlg;
	CSetFriend setFriendDlg;

private:
	CSkinMgr m_skinmgr;

	void Init();
private:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

	void LoadSkin();
public:
	// 设置窗口圆角2012.10.15 yyf
	void SetWndRgn(void);
};
