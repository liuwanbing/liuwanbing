#pragma once

#ifndef A41A948D_8E6F_405c_AB5B_04AE11E53BD
#define A41A948D_8E6F_405c_AB5B_04AE11E53BD

#include "AFCLock.h"

//进程管理类
class EXT_CLASS CProcessManage
{
	//变量定义
protected:
	

protected:
	static bool							g_bInit;					//是否初始化
	static CPtrArray					g_pManageArray;				//进程管理类数组
	static CAFCSignedLock				g_Lock;						//资源访问锁

	//函数定义
public:
	//构造函数
	CProcessManage(void);
	//析构函数
	~CProcessManage(void);

	//功能函数
public:
	//启动进程

};

#endif