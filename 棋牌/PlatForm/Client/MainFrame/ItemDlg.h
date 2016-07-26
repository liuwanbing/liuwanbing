#pragma once

#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h"
// CItemDlg 对话框

class CItemDlg : public CDialog
{
	DECLARE_DYNAMIC(CItemDlg)

public:
	CItemDlg();   // 标准构造函数
	virtual ~CItemDlg();

// 对话框数据
	//enum { IDD = IDD_DIG_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	//CString ReadFile(CString filename);
	afx_msg void OnPaint();
	void OnClickedClose();

	CGameImage	m_bk;
	CString m_strText;
	CFile m_File;
	CNormalBitmapButtonEx				m_Bt1;	
	CNormalBitmapButtonEx				m_Bt2;	
	DECLARE_MESSAGE_MAP()

private:
	CSkinMgr m_skinmgr;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
 
	  // 设置窗口圆角 2012.10.15 yyf
	  void SetWndRgn(void);
};
