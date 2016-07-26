#pragma once
#include "afxcmn.h"
#include "Resource.h"

// CSuperGM 对话框

class CSuperGM : public CDialog
{
	DECLARE_DYNAMIC(CSuperGM)

public:
	CSuperGM(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSuperGM();

// 对话框数据
	enum { IDD = IDD_SUPERGM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchanged1071(NMHDR *pNMHDR, LRESULT *pResult);


public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedRadio1();
public:
	afx_msg void OnBnClickedRadio2();
public:
	afx_msg void OnBnClickedRadio3();
public:
	afx_msg void OnBnClickedRadio4();
public:
	afx_msg void OnBnClickedCheckRobotInfo();


public:
	CListCtrl m_ListCtrl;

	int		m_SelectedArea;

	bool	m_bShowRobotInfo;

	void UpdateListData(SUPERDATA *pSuperData);
	///更新控制提示
	void UpdataCtrlText();
	///更新下注消息
	void UpDataNoteRateInfo(__int64 iNtWin , __int64 iXianWin ,__int64 iHeWin) ; 
	///显示窗口
	void ShowSuperWindow(bool show);
	///发送控制消息
	void SendCtrlMsg();

	//设置控制结果
	void SetCtrlResult(BYTE result);
	void OnSendDesktopXzInfo();
};
