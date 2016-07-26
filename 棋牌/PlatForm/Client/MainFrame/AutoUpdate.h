#pragma once
#ifndef AUTOUPDATE_HEAD_H
#define AUTOUPDATE_HEAD_H

#include "Afxinet.h"

//自动更新类
class CAutoUpdate
{
	//变量定义
protected:
	

	//函数定义
public:
	//构造函数
	CAutoUpdate(void);
	//析构函数
	~CAutoUpdate(void);

	//功能函数
public:
	//更新函数
	bool Start(bool bForce);
	//停止自动更新
	bool Stop();

	//线程函数
protected:
	//更新线程
	static unsigned __stdcall UpdateThread(LPVOID pThreadData);
};
#endif
