#pragma once

#include "Stdafx.h"
#include "Resource.h"

//游戏设置信息类
class CGameInfoDlg : public CCoolView//CGameFaceGo
{
	//变量定义
public:
	enum				{ IDD = IDD_GAME_INFO };			// 对话框数据

	//控件变量
public:	
	CNormalBitmapButtonEx			m_btOk;								//确定按钮
	CNormalBitmapButtonEx			m_btCancel;							//取消按钮
	CWnd*				m_pPP;
	CGameImage						m_bk;
	bool                       m_bRequireCoachMoeny;                  //是否需要指导费
	//函数定义
public:
	//构造函数
	CGameInfoDlg(CWnd * pParent=NULL);
	//析够函数
	virtual ~CGameInfoDlg();
	//取消按钮
	virtual void OnCancel();
	//确定按钮
	virtual void OnOK();
	//设置游戏设置
	void SetGameSetInfo(int iJUMinute, int iJuSecond, int iBuMinute, int iBuSecond, int iDuMinute, int iDuSecond,int iGameMoney);
	//设置对话框的大小
	void AutoSize();

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	
};
