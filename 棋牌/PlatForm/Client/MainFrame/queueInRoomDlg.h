#pragma once
#ifndef QUEUEINROOM_HEAD_H
#define QUEUEINROOM_HEAD_H

#include "Stdafx.h"
#include "SkinMgr.h"

///排队机对话框类
class CQueueInRoomDlg: public CLoadFileBmp
{
public:
	TCHAR								m_szShowMsg[30];					///<显示信息
	CGameImage							m_bk;								///<背景
	//Image *								m_bk;
	CNormalBitmapButtonEx				m_BtQueue;							///<排队按钮			
	CNormalBitmapButtonEx				m_BtNoQueue;						///<不排队按钮
	CNormalBitmapButtonEx				m_BtExit;							///<退出
	COLORREF							m_MsgColor;							///<显示消息颜色
	CFont								m_Font;								///<字体
	RECT								m_rc;								///<显示信息的区域
	int									m_iIndex;							///<动画索引
	UINT								m_uRoomID;							///<房间ID



public:
	//构造
	CQueueInRoomDlg();
	//析构
	~CQueueInRoomDlg();
	///设置消息显示
	bool SetMessageText(const TCHAR * szMessage);
	///销毁对话框
	void DestroyDlg();

public:
	virtual BOOL OnInitDialog();
	///确定
	virtual void OnOK() { return; }
	///取消
	virtual void OnCancel() { return; }
	
public:
	///定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	///排队处理
	afx_msg void OnQueue();
	///不排队处理
	afx_msg void OnNoQueue();
	///退出处理
	afx_msg void OnExit();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);


protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

private:
	void ChangeSkin();
};

#endif