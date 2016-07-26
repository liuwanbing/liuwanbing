#pragma once

#ifndef DLGDIYCHAT_HEAD_H
#define DLGDIYCHAT_HEAD_H

#include "afxwin.h"
#include "AFCImage.h"
#include "AFCButton.h"
#include "CconfigDlg.h" 

// CDlgDIYChat 对话框

class EXT_CLASS CDlgDIYChat : public CDialog
{
	DECLARE_DYNAMIC(CDlgDIYChat)

public:
	CDlgDIYChat(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDIYChat();
	CListBox *m_listBox;
	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int m_GameOrHall;
	CGameImage		m_bkimage;
	// 对话框数据
	//enum { IDD = IDD_DIY_CHAT };
	CNormalBitmapButtonEx	m_btcancel;
	CNormalBitmapButtonEx	m_btOk;
	CNormalBitmapButtonEx	m_btDelete;
	CNormalBitmapButtonEx	m_btModify;
	CNormalBitmapButtonEx	m_btAdd;

	HBRUSH m_bkBrush;
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	//功能函数
	void ReadTalkINI();
	void WriteTalkINI();

	//消息响应函数
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedAdd();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkListChat();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	CString m_EditSzBuffer;
private:
	void ChangeSkin(const CString& strFileName,const CString& strSkinKey,const CString& strSkinPath);

public:
	void OnExchangeSkin(const CString& strFileName,const CString& strSkinKey,const CString& strSkinPath);
   
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};

#endif