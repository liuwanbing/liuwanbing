#pragma once


#include "gameplacemessage.h"
#include "resource.h"
#include "MyListCtrl.h"
#include "SkinMgr.h"
#include <vector>
#include <algorithm>
#include <functional>
#include "afxcmn.h"
 
#include "..\UILibrary2003\CconfigDlg.h" 

using namespace std;
// CCharmExchange 对话框


class CCharmExchange : public CDialog
{
	DECLARE_DYNAMIC(CCharmExchange)

public:
	CCharmExchange(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharmExchange();

	HBRUSH	m_bkBrush;

// 对话框数据
	enum { IDD = IDD_DIALOG8 };

	vector<TCharmExchange_Item> m_ChangeList;

public:
	void OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CHappyGoDlgT m_list;

	CNormalBitmapButtonEx  m_Change;
	CNormalBitmapButtonEx  m_Cancel;
	CNormalBitmapButtonEx  m_Close;

	CGameImage		m_bkimage;
	//CDUIWnd m_duiWnd;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.15 yyf

	afx_msg void OnBnClickedChange();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedClose();

	
	void Clear();
public:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:

	//更新魅力值
	void UpdateFascination();
	 
	// 设置窗口圆角 2012.10.15 yyf
	void SetWndRgn(void);
private:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

	void LoadSkin();

	void Init();
private:
	CSkinMgr m_skinmgr;
	
};
