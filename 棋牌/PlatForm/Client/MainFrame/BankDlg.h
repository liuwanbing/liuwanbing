#pragma once

#ifndef BANKDLG_HEAD_H
#define BANKDLG_HEAD_H

#include "BaseRoom.h"
#include "..\UILibrary2003\CconfigDlg.h"
//#include "DlgInMoney.h"
//#include "DlgOutMoney.h"
//#include "DlgTransferMoney.h"
// CBankDlg dialog

class CBankDlg : public CDialog
{
	DECLARE_DYNAMIC(CBankDlg)

public:
	CBankDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBankDlg();
	void SetBankMoney(int count);
	void SetRoomMoney(int count);
	int GetBankMoney();
	void SetSocketPoint(CTCPClientSocket* m_TCPSocket);
	void GetUserID(long UserID);
	void GetRoomName(TCHAR * roomName);
	void GetConnectPoint(CConnectDlg* connection);
	void getUserItem(UserItemStruct * pMeUserInf);
	bool GetRoomType(int uComType);

	void UpdateUserMoney(int OptType, long nMoney);	//更新界面，Opt=0,表示取出，Opt=1，表示存入，nMoney表示本次所操作的金币数
	//初始化函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	long int m_UserID;

	MSG_GR_S_CheckMoney checkMoney;

// Dialog Data
	//enum { IDD = IDD_DIALOGBANK };
	CNormalBitmapButtonEx	m_BtCancel;		///< 银行取消
	CNormalBitmapButtonEx	m_btCheckOut;	///< 银行取出
	CNormalBitmapButtonEx	m_btCheckIn;	///< 银行存入
	CNormalBitmapButtonEx	m_btTransfer;	///< 银行转帐
	CNormalBitmapButtonEx	m_btQuery;		///< 银行查询
	CconfigDlg  m_configDlg; //加载底图类. 2012.10.17 yyf
	HBRUSH m_bkBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// 剩余金币
	int m_BankMoney;
	int m_RoomMoney;
	afx_msg void OnBnClickedCheckout();
	afx_msg void OnBnClickedCheckin();
	afx_msg void OnBnClickedTransfer();
	afx_msg void OnBnClickedQuery();
	CTCPClientSocket* m_TCPSocket;
	CString m_RoomName;
	CConnectDlg * m_Connection;
	CGameImage		m_bkimage;
	int m_uComType;
	UserItemStruct					* m_pMeUserInfo;						//玩家信息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    
public:
	  // 设置窗口圆角 2012.10.17 yyf
	  void SetWndRgn(void);
};
#endif