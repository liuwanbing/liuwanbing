#pragma once
#include "afxwin.h"
#include "SkinMgr.h"
#include "resource.h"
// CPassDesk 对话框

class CPassDesk : public CDialog
{
	DECLARE_DYNAMIC(CPassDesk)

public:
	CPassDesk(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPassDesk();
	CGameImage							m_bk;				//背景


// 对话框数据
	enum { IDD = IDD_PASS_DESK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_pass;
public:
	afx_msg void OnPaint();
public:
	virtual BOOL OnInitDialog();
public:
	CNormalBitmapButtonEx m_BtOk;
public:
	CNormalBitmapButtonEx m_BtCancel;

private:
	CSkinMgr m_skinmgr;

	void LoadSkin();
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};
