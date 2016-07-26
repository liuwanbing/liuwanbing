#pragma once
#include "resource.h"


// CPersonalInfo 对话框
#define			WINDOW_APP			1
#define			WINDOW_DISAPP		2

class CPersonalInfo : public CDialog
{
	DECLARE_DYNAMIC(CPersonalInfo)
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

public:
	void getRoomPtr(CWnd* room);
// 对话框数据
	enum { IDD = IDD_PERSONINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
private:
	int m_time;
	CString m_szNiCheng;
	long m_lCaiFu;
	long m_lJiFeng;
	CString m_szJiBie;

	afx_msg void OnBnClickedChat();
	afx_msg void OnBnClickedCall();
	afx_msg void OnBnClickedWatchGame();
};
