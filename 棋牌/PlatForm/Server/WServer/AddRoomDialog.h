#pragma once

#include "afxcmn.h"


// CAddRoomDialog dialog

class CAddRoomDialog : public CDialog
{
	DECLARE_DYNAMIC(CAddRoomDialog)

public:
	CAddRoomDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddRoomDialog();

	virtual void OnFinalRelease();

// Dialog Data
	//enum { IDD = IDD_DLL_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	// 房间ID
	int m_roomID;
public:
	CString m_szGameRoomName;
public:
	// 登陆数据库IP
	CIPAddressCtrl m_LogonSQLIP;
public:
	// 中心数据库IP
	CIPAddressCtrl m_SQLIP;
public:
	// 本地数据库IP
	CIPAddressCtrl m_NativeSQLIP;
public:
	//游戏类型
	int m_uComType;
public:
	// 监听端口
	int m_uListenPort;
public:
	// 挂接类型
	int m_uKindID;
public:
	// 桌子数
	int m_uDeskCount;
public:
	// 最大人数
	int m_uMaxPeople;
public:
	// 基础分数
	int m_uBasePoint;
public:
	// 最少分数
	int m_uLessPoint;
public:
	// 房间规则
	unsigned int m_dwRoomRule;
};
