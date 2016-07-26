#pragma once
#include "afxwin.h"
#include "resource.h"
#include "explorer1.h"
#include "SkinMgr.h"
#include "..\UILibrary2003\CconfigDlg.h" 
// CPropPannel 对话框

class CPropPannel : public CDialog
{
	DECLARE_DYNAMIC(CPropPannel)

	CWnd		*pParentWnd;
	Image		*imgDialog;
	COLORREF	m_bkcolor;
	CString		m_strPropUrl;

	int			curPropID;
	int			curPropPrice;
	int			curPropVipPrice;

	int			m_nOpenFrom;

	int			m_bkcolor_R;
	int			m_bkcolor_G;
	int			m_bkcolor_B;

	HBRUSH m_bkBrush;

public:
	CPropPannel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropPannel();

// 对话框数据
	enum { IDD = IDD_PROPPANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ctlTip;
public:
	virtual BOOL OnInitDialog();
	void setSkinIndex(int sk);
public:
	CNormalBitmapButtonEx m_bnClose;
public:
	afx_msg void OnPaint();
public:
	void drawSkin(void);
public:
	afx_msg void OnBnClickedClose();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void openPannel(int openFrom,CString strGiveUser="",int propID=1);
public:
	CExplorer1 m_ie;
public:
	DECLARE_EVENTSINK_MAP()
public:
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
public:
	void NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);
public:
	void processLocalProp();
public:
	CString GetUrlValue(CString szUrl, CString szField);

public:
	int m_nHoldCount;
public:
	int m_nBuyCount;
public:
	CString m_strBuyMoney;
public:
	CEdit m_ctlGiveUser;
public:
	CNormalBitmapButtonEx  m_bnUse;
public:
	CNormalBitmapButtonEx  m_bnBuy;
public:
	CNormalBitmapButtonEx  m_bnGive;
	//CNormalBitmapButtonEx  m_cancelBtn;
public:
	afx_msg void OnEnChangeBuycount();
public:
	CEdit m_ctlBuyCount;
public:
	CString m_sGiveUser;
public:
	CEdit m_ctlGiveCount;
public:
	int m_nGiveCount;
public:
	afx_msg void OnEnChangeGiveuser();
public:
	afx_msg void OnEnChangeGivecount();
public:
	afx_msg void OnBnClickedBuy();
public:
	void OnGetPropInformation(_TAG_USERPROP* userProp);
public:
	afx_msg void OnBnClickedUse();
public:
	afx_msg void OnBnClickedGive();
public:
	void OnGivenProp(_TAG_PROP_GIVE_FOR * propGiveFor);
public:
	void OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
private:
	CSkinMgr m_skinmgr; 
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
public:
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};
