#pragma once
//#include "e:\20050611_pmbgame\newgame\客户端组件\界面库.2003\afcdialog.h"

#include "Stdafx.h"
#include "Resource.h"

class CAdmitDefeat :
	public CGameFaceGo
{

	public:
	CNormalBitmapButtonEx					m_btOk;							//确定按钮
	CNormalBitmapButtonEx					m_btCancel;						//取消按钮
	enum					{ IDD = IDD_REN_SHU };			//对话框数据
	CWnd *m_pParnet;
	CGameImage							m_bk;
public:
	CAdmitDefeat(void);
	~CAdmitDefeat(void);
protected:
//	virtual void OnOK();
//	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL ShowWindow(int nCmdShow);
};
