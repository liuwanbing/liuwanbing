#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ColorComboBox.h"
#include "IMMessage.h"
#include "..\UILibrary2003\MessageHandle.h"
#include "SkinMgr.h"
// CIMSession 对话框

#define WM_CLOSE_SESSION	WM_USER+1

class CIMSession : public CDialog
{
	DECLARE_DYNAMIC(CIMSession)
private:
	CGameImage		m_bkimage;

	HWND		pHwnd;
	bool		sendOnleyEnter;
	CHARFORMAT	charFormat;
	CString		m_charFontName;
	CHARFORMAT	charTipFormat;
	CWnd		*pWnd;
	Image			*imgDialog;
	int				m_skinIndex;
	COLORREF		m_bkcolor;

	CGameTalkText	m_ExpressWnd;						//表情窗口

	HBRUSH m_bkBrush;
public:
	CIMSession(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIMSession();
	int			m_dwUserId;
	int			m_dwLocalUserID;
	CString		m_szName;
	bool		m_bMale;
	bool		m_bOnline;
// 对话框数据
	enum { IDD = IDD_IM_SESSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	void Init();

public:
	void LoadSkin();
public:
	afx_msg void OnClose();
public:
	void setWindowOwner(HWND pParentHwnd);
public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedExpression();
public:
	CNewMiniDlg   m_chat;
	CRichEditCtrl m_inroom;
	CLookingClass m_MessageHandle;
	CAFCColorResource	m_colorresource;
public:
	CComboBox m_cmbFontname;
public:
	CComboBox m_cmbFontsize;
public:
	CColorComboBox m_cmbColor;
public:
	afx_msg void OnBnClickedSendkey();
public:
	void setUserOnline(bool bOnline);
public:
	void setUser(int dwUserID, CString szName,CWnd *pParentWnd , bool bMale, bool bOnline=false);
public:
	CString m_strSend;
public:
	CRichEditCtrl m_ctlSend;
public:
	afx_msg void OnCbnSelchangeCombocolor();
public:
	afx_msg void OnCbnSelchangeCombofontsize();
public:
	afx_msg void OnCbnSelchangeCombofontname();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void OnGetMessage(MSG_IM_S_MSG * msg);
public:
	afx_msg void OnBnClickedClosesession();
	afx_msg void OnBnClickedMinsession();
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
public:
	afx_msg void OnPaint();
public:
	void setSkinIndex(int sk);
public:
	void drawSkin(void);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CNormalBitmapButtonEx m_bnClose;
	CNormalBitmapButtonEx m_bnMin;
	CNormalBitmapButtonEx m_bnClose2;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CNormalBitmapButtonEx m_bnSend;
	CNormalBitmapButtonEx m_BtExpression;						//表情按钮
public:
	CNormalBitmapButtonEx m_bnSendKey;

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};
