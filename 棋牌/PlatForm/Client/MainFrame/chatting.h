#pragma once
#ifndef CHATTING_HEAD_H
#define CHATTING_HEAD_H

#include "StdAfx.h"
#include "Resource.h"
//#include "afcdialog.h"
//#include "afcWeb.h"
//游戏信息类
class CChattingWnd : public CDialog
{
	//变量定义
protected:
	CGameImage				m_bk;
	CIEClass				m_WebView;						// 广告
	int						m_iWidth,m_iHeight;				//高宽
	CString					m_URL;							//链接
	enum					{ IDD = IDD_CHATTING };			//对话框数据
	//函数定义
public:
	CWnd *m_pParnet;
	//构造函数
	CChattingWnd();
	//析构函数
	virtual ~CChattingWnd();

	virtual BOOL OnInitDialog();

	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
	
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);

	DECLARE_MESSAGE_MAP()
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//更新网页内容
	void UpdateIE();
public:
	afx_msg void OnClose();
};

#endif