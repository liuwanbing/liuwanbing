#pragma once

#ifndef BANKDLG_HEAD_H
#define BANKDLG_HEAD_H

//#include "BaseRoom.h"
//#include "DlgInMoney.h"
//#include "DlgOutMoney.h"
#include "resource.h"
#include "../common/GameRoomMessage.h"
#include "Platform/AFCImage.h"
#include "Platform/AFCButton.h"
#include "SkinMgr.h"

#define WM_BANK_CHECK_OUT	WM_USER + 228		///< 银行操作对话框中发送存钱消息到CLoveSendClassInExe窗口
/// CBankDlg dialog
/// 银行操作界面对话框
/// 由父窗口实时new出来，否则当父窗口收到网络消息关闭时，失控的子对话框循环会产生崩溃
class CBankDlg : public CDialog
{
	DECLARE_DYNAMIC(CBankDlg)

	HWND	m_hWndParent;			///< 父窗口句柄
	HBRUSH m_bkBrush;
public:
	CBankDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBankDlg();
	void SetBankMoney(int count);
	void SetRoomMoney(int count);
	int GetBankMoney();
	//void SetSocketPoint(CTCPClientSocket* m_TCPSocket);
	void GetUserID(long UserID);
	void GetRoomName(TCHAR * roomName);
	//void GetConnectPoint(CConnectDlg* connection);
	void getUserItem(UserItemStruct * pMeUserInf);
	bool GetRoomType(int uComType);

	/// 更新界面
	/// @param[in] Opt =0,表示取出，Opt=1，表示存入
	/// @param[in] nMoney 表示本次所操作的金币数
	/// @return void
	void UpdateUserMoney(int OptType, long nMoney);
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
	CNormalBitmapButtonEx	m_btTransfer;	///< 银行转帐
	CNormalBitmapButtonEx	m_btQuery;		///< 银行查询

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// 剩余金币
	int m_BankMoney;
	int m_RoomMoney;
	afx_msg void OnBnClickedCheckout();
	afx_msg void OnBnClickedTransfer();
	afx_msg void OnBnClickedQuery();
	CString m_RoomName;
	CGameImage		m_bkimage;
	int m_uComType;
	UserItemStruct					* m_pMeUserInfo;						//玩家信息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeCheckNum();

private:
	CSkinMgr m_skinmgr;
};
#endif