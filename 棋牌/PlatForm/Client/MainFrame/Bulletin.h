#pragma once
#include "explorer1.h"
#include "resource.h"
#include "SkinMgr.h"
// CBulletin 对话框

class CBulletin : public CDialog
{
	DECLARE_DYNAMIC(CBulletin)

public:
	CBulletin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBulletin();
// 对话框数据
	enum { IDD = IDD_BULLETIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExplorer1 m_ie;
	CNormalBitmapButtonEx m_btClose;
	CNormalBitmapButtonEx m_btClose2;
	CGameImage					m_BackBitmap;					//对话框背景图
public:
	virtual BOOL OnInitDialog();
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
public:
	void OpenBulletin(CString surl);
public:
	afx_msg void OnPaint();
	void OnCancel();
private:
	CSkinMgr m_skinmgr;
private:
	void Init();
	//换肤
	void LoadSkin();
};
