#pragma once
#include "Resource.h"

// CShowAIInfo 对话框

class CShowAIInfo : public CDialog
{
	DECLARE_DYNAMIC(CShowAIInfo)

public:
	CShowAIInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowAIInfo();

// 对话框数据
	enum { IDD = IDD_SHOW_AI_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//初始化函数
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

public:
	
	tagAICtrlInfo	m_AIInfo;	//机器人信息
	

	bool	bIsClearAIWinMoney;		//已赢钱清零
	bool	bIsSetAICtrl;			//修改设置
	bool    bAIWinAndLostAutoCtrl;	//开启控制

	//显示对话框
	void ShowAIInfoWindow();

public:
	afx_msg void OnBnClickedButSendChange();
public:
	afx_msg void OnBnClickedChChange();
public:
	afx_msg void OnBnClickedChClearWin();
public:
	afx_msg void OnBnClickedChChangeSet();
public:
	int m_GoalMoney;
public:
	int m_iAveMoney;
public:
	afx_msg void OnEnChangeEditGoalMoney();
public:
	afx_msg void OnEnChangeEditAveMoney();
};
