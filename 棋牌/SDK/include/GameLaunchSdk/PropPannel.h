#pragma once
#include "afxwin.h"
#include "resource.h"
#include "Platform/explorer1.h"
#include "CommonDefine.h"
#include "SkinMgr.h"

using namespace Gdiplus;


// CPropPannel 对话框
class XLogic;
class CPropPannel : public CDialog
{
	DECLARE_DYNAMIC(CPropPannel)

	CWnd		*m_pParent;
	GameInfoStructInExe	*m_pGameInfo;			//游戏信息指针，从父窗口传来，需要得到玩家信息
	Image		*imgDialog;
	COLORREF	m_bkcolor;
	CString		m_strPropUrl;
	CString		m_strPropVIPUrl;

	int			curPropID;
	int			curPropPrice;
	int			curPropVipPrice;
	int			curVIPType;
	int			curBuyCount;
//	bool        m_bTypeChange;

	int			m_nOpenFrom;
	
	int			m_bkcolor_R;
	int			m_bkcolor_G;
	int			m_bkcolor_B;

	// test
	BOOL		m_bIsVIPButton;
	bool		m_bClickCancelGive;
	bool		m_bClickCancelUse;

	bool		m_UsingGive;
	bool		m_UsingUse;
	bool		m_UsingBuy;

	bool		m_bUsingNewUI;
	HBRUSH		m_bkBrush;

public:
	XLogic		*m_pLogic;
private:
	CSkinMgr m_skinmgr;
public:
	CPropPannel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropPannel();

// 对话框数据
	//enum { IDD = IDD_PROPPANNEL };

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
	CNormalBitmapButtonEx m_bnClose2;
	CNormalBitmapButtonEx m_bnCancleGive;
	CNormalBitmapButtonEx m_bnCancleUse;
	CNormalBitmapButtonEx m_bnCancleBuy;
	CNormalBitmapButtonEx m_bnAdd;
	CNormalBitmapButtonEx m_bnSub;

public:
	afx_msg void OnPaint();
public:
	void drawSkin(void);
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedClose2();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void openPannel(int openFrom,CString strGiveUser="",int propID=1);
	void ResetData(void);
public:
	CExplorer2 m_ie;
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
	CComboBox m_ctlGiveUser;
	CComboBox m_ctlUseUser;
public:
	CNormalBitmapButtonEx  m_bnUse;
public:
	CNormalBitmapButtonEx  m_bnBuy;
public:
	CNormalBitmapButtonEx  m_bnGive;
	CEdit m_uCountTip;
	CEdit m_uCount;

	CGameImage m_bk;

	//CEdit   m_nHoldCountCtr;
	CEdit	m_szGiveCount;
	CEdit	m_szBuyCount;
	CEdit	m_szGiveName;
	CStatic m_GiveCount;
	CStatic	m_GiveName;
	CStatic	m_tip2;
    bool	m_bGibe;
	bool	m_bGiveOrCancle;
	//CNormalBitmapButtonEx  m_cancelBtn;

	CRect	m_BtnRect1, m_BtnRect2, m_BtnRect3, m_BtnRect4;
public:
	afx_msg void OnEnChangeBuycount();
public:
	CEdit m_ctlBuyCount;
public:
	CString m_sGiveUser;
	CString m_sUseUser;
public:
	CEdit m_ctlGiveCount;
public:
	int m_nGiveCount;
public:
	afx_msg void OnEnChangeGiveuser();
public:
	afx_msg void OnEnChangeGivecount();
public:
	afx_msg void OnCbnSelChangeUseUser();
public:
	afx_msg void OnCbnSelChangeGiveUser();
public:
	afx_msg void OnCbnEditUpdateGiveuser();
public:
	afx_msg void OnBnClickedBuy();
public:
	void OnGetPropInformation(_TAG_USERPROP* userProp);
public:
	afx_msg void OnBnClickedUse();
public:
	afx_msg void OnBnClickedGive();
	afx_msg void OnBnClickedSubCount();
	afx_msg void OnBnClickedAddCount();
public:
	BOOL IsPropUseTarget(_TAG_USERPROP * pProp);
	BOOL IsPropUseTarget(int iPropID);
	void OnGivenProp(_TAG_PROP_GIVE_FOR * propGiveFor);
	void FixDefaultCtl();
	void FixNormalBtnPos();
	void FixButtonOnGive();
	void FixButtonOnUse();
	void FixButtonOnBuy();
	void OnBnClickedCancleGive();
	void OnBnClickedCancleUse();
	void OnBnClickedCancleBuy();
public:
	void OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
public:
	int GetMaxPeopleCount();
	UserItemStruct * GetUserItem(int iIdx);
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
public:
	void LoadSkin();

	void SetIEOnPropDlg();

	UINT GetPropDisplayTime();
private:
	CString m_surl;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
