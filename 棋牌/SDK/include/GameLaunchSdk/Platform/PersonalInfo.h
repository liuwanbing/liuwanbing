#pragma once
#ifndef PERSONALINFO_HEAD_H
#define PERSONALINFO_HEAD_H
#include "publicdefine.h"
#include "afcdevhead.h"
#include "SkinMgr.h"

// CPersonalInfo 对话框
#define			WINDOW_APP			1
#define			WINDOW_DISAPP		2

class EXT_CLASS CPersonalInfo : public CDialog
{
	DECLARE_DYNAMIC(CPersonalInfo)
	HBRUSH m_bkBrush;
private:
	UserInfoStruct m_userInfoItem;
public:
	CPersonalInfo(CWnd* pParent = NULL);   // 标准构造函数
	
	virtual ~CPersonalInfo();

	virtual BOOL OnInitDialog();
	
	bool UpdateUserItem(UserInfoStruct user);
	
	afx_msg void OnPaint();
	
public:
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	void close();
	
	CGameImage						m_bkimage;
	GetOrderName					* pOrderName;						//等级函数
	CWnd*							m_roomWnd;
	CNormalBitmapButtonEx			m_btChat;
	CNormalBitmapButtonEx			m_btInvite;		
	CNormalBitmapButtonEx	        m_btWatch;
	CNormalBitmapButtonEx	        m_btGift;
public:
	void getRoomPtr(CWnd* room);
// 对话框数据
	//enum { IDD = IDD_PERSONINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
private:
	int m_time;
	CString m_szNiCheng;
	__int64 m_i64CaiFu;
	long m_lJiFeng;
	CString m_szJiBie;

	afx_msg void OnBnClickedChat();
	afx_msg void OnBnClickedCall();
	afx_msg void OnBnClickedWatchGame();
	afx_msg void OnBnClickedGift();

private:
	CSkinMgr m_skinmgr;
};

#endif