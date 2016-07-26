#pragma once
#ifndef PERSONALINFO_HEAD_H
#define PERSONALINFO_HEAD_H
#include "afcdevhead.h"

// CPersonalInfo 对话框
#define			WINDOW_APP			1
#define			WINDOW_DISAPP		2

class EXT_CLASS CPersonalInfo : public CDialog
{
	DECLARE_DYNAMIC(CPersonalInfo)
private:
	UserInfoStruct m_userInfoItem;

	HBRUSH m_bkBrush;
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
	CNormalBitmapButtonEx			m_btInvite;							///<邀请已经坐在桌子上玩家一起游戏按钮	
	CNormalBitmapButtonEx	        m_btWatch;
	CNormalBitmapButtonEx	        m_btGift;

	COLORREF						m_FontClr;							//JMod-字体颜色 20090506
public:
	void getRoomPtr(CWnd* room);
	int GetWidth(){return m_bkimage.GetWidth();}
	int GetHeight(){return m_bkimage.GetHeight();}
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
};

#endif
