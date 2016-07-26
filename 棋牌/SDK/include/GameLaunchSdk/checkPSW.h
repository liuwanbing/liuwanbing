#pragma once
#ifndef CHECKPSW_HEAD_H
#define CHECKPSW_HEAD_H

#include "resource.h"
#include "Platform/AFCImage.h"
#include "Platform/AFCButton.h"
#include "MD5.h"
#include "sha1.h"
#include "SkinMgr.h"

//登陆对话框类
class CCheckPSW: public CDialog
{
	DECLARE_DYNAMIC(CCheckPSW)
public:
	//构造
	CCheckPSW(CWnd *pWndParent);
	//析构
	~CCheckPSW();
public:
	char							m_szScrPass[61];					//用户原密码
	char							m_szMD5Pass[50];					//加密后的密码
	CGameImage							m_bk;				//背景
	CNormalBitmapButtonEx				m_BtCancel;				//还原
	CNormalBitmapButtonEx				m_BtOk;

	
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	CSkinMgr m_skinmgr;
};

#endif