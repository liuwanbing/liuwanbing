#pragma once
#ifndef CHECKPSW_HEAD_H
#define CHECKPSW_HEAD_H

#include "Stdafx.h"

//登陆对话框类
class CCheckPSW: public CLoadFileBmp
{
public:
	static char							m_szScrPass[61];					//用户原密码
	static char							m_szMD5Pass[50];					//加密后的密码
	CGameImage							m_bk;				//背景
	CNormalBitmapButtonEx				m_BtCancel;				//还原
	CNormalBitmapButtonEx				m_BtOk;

public:
	//构造
	CCheckPSW();
	//析构
	~CCheckPSW();
	
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);
};

#endif