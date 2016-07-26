#pragma once

#include "StdAfx.h"
#include "AFCLoader.h"

//授权设置
class CAccessDialog : public CDialog
{
	//变量定义
public:
	TCHAR							m_szServerIP[51];								//服务器 IP
	TCHAR							m_szServerGUID[37];								//服务器 ID

	//函数定义
public:
	//构造函数
	CAccessDialog();
	//析构函数
	virtual ~CAccessDialog();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();

	//功能函数
public:
	//设置信息
	bool SetServerInfo(TCHAR * szServerGUID, TCHAR * szServerIP);
	//获取信息
	bool GetServerInfo(TCHAR * szServerGUID, TCHAR * strServerIP);

	DECLARE_MESSAGE_MAP()
};
