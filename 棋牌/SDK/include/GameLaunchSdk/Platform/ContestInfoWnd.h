#pragma once

#include "CommonDefine.h"
// CContestInfoWnd 对话框

class CContestInfoWnd : public CDialog
{
	DECLARE_DYNAMIC(CContestInfoWnd)

public:
	CContestInfoWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CContestInfoWnd();

// 对话框数据
	//enum { IDD = IDD_CONTESTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	GameInfoStructInExe* m_pGameInfo;
	UserItemStruct * m_pUserItem;

public:
	afx_msg void OnPaint();

public:
	CGameImage	m_bk;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	void SetBkImage(TCHAR  path[]);

	void SetGameInfo(GameInfoStructInExe* pGameInfo);
	void UpdateUser(UserItemStruct * pUserItem);


	bool m_bShow;
	void ShowWnd();
	void HideWnd();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
