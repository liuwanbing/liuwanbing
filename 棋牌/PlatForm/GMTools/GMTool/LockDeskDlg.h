#pragma once

//////////////////////////////////////////////////////////////////////////
///批量封桌
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "CommandMessage.h"

// CLockDeskDlg 对话框

class CLockDeskDlg : public CDialog
{
	DECLARE_DYNAMIC(CLockDeskDlg)

public:
	CLockDeskDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLockDeskDlg();

	void SetInfo(int itype, int imin, int imax);

// 对话框数据
	enum { IDD = IDD_DIALOG8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_Lower;		///区间下界
	int m_Upper;		///区间上界

	int m_Min;			///区间下界最小值
	int m_Max;			///区间上界最小值	
	int m_type;			///操作类型(1表示封桌，其它表示解桌)

	HWND m_hWnd;		///接收消息窗口句柄
public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
public:
	CString m_Infostatic;
};
