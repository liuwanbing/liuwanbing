#pragma once
#include "afxwin.h"
// CSetupDialog dialog

class CSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetupDialog)

public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetupDialog();

// Dialog Data
//	enum { IDD = IDD_SETGAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT serInfoID;
public:
	UINT nameID;
public:
	UINT deskPeople;
public:
	UINT supportType;
public:
	UINT sver;
public:
	CString gameName;//游戏名字
public:
	CString tableDbName;					//数据库表名字
public:
	CString  dllFileName;					//文件名字
public:
	CString writer;						//软件开发者
public:
	CString dllNote;						//备注信息
public:
	// //组件ID ServerInfoID
	CEdit editSiid;
};
