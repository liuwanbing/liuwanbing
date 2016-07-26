#ifndef DLG_MATCH_AWARDS_HEAD_FILE
#define DLG_MATCH_AWARDS_HEAD_FILE

#pragma once

#include "afxwin.h"
#include "GameFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//比赛奖状
class GAME_FRAME_CLASS CDlgMatchAwards :public CDialog
{
	//控件变量
public:
	CSkinButton						m_btCloseAwards;					//关闭按钮
	CMD_GR_MatchResult				*m_pMatchResult;					//比赛结果

	//界面控件
protected:
	CSkinLayered					m_SkinLayered;						//分层窗口

	//函数定义
public:
	//构造函数
	CDlgMatchAwards(void);
	//析构函数
	virtual ~CDlgMatchAwards(void);
	//设置参数
	void SetMatchResult(CMD_GR_MatchResult *pMatchResult){m_pMatchResult=pMatchResult;}

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();

	afx_msg void OnPaint();

	virtual void DoDataExchange(CDataExchange* pDX);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	DECLARE_MESSAGE_MAP()

};

#endif