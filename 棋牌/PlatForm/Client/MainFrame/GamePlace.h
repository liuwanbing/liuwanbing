#pragma once
#ifndef GAMEPLACE_HEAD_H
#define GAMEPLACE_HEAD_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "StdAfx.h"
#include "Afxmt.h"
#include "Resource.h"
#include "SkinMgr.h"

//应用程序对象
class CGamePlaceApp : public CWinApp
{
	//函数定义
public:
	//构造函数 
	CGamePlaceApp();
	~CGamePlaceApp();
	//初始化函数
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()

public:

	static int GetModuleStatus(char* module,char* compoment,int defval = 0);

private:
	CSkinMgr m_skinmgr;
};

//应用程序对象
extern CGamePlaceApp theApp;
//extern char TML_//SN[128];
//extern CString TML_//SN;///////定义全局变量
#endif