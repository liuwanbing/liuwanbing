#pragma once

#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
// CLockAccountDlg 对话框

class CLockAccountDlg : public CLoadFileBmp
{
	DECLARE_DYNAMIC(CLockAccountDlg)
	HBRUSH	m_bkBrush;
public:
	CLockAccountDlg(CWnd* pParent = NULL, int nType=0);   // 标准构造函数
	virtual ~CLockAccountDlg();

	static char							m_szScrPass[61];					//用户原密码
	static char							m_szMD5Pass[50];					//加密后的密码
	CGameImage							m_bk;				//背景
	CNormalBitmapButtonEx				m_BtCancel;			//取消
	CNormalBitmapButtonEx				m_BtOk;				//确定
	CNormalBitmapButtonEx				m_BtClose;			//关闭
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
// 对话框数据
	enum { IDD = IDD_LOCK_ACCOUNT };
private:
	int m_nShowType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void drawSkin();
public:
	afx_msg void OnPaint(); 
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedClose();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCancel();

private:
	CSkinMgr m_skinmgr;

public:
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};
