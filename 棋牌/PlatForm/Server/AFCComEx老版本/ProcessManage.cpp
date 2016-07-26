#include "StdAfx.h"
#include "ProcessManage.h"

//变量定义
bool						CProcessManage::g_bInit=false;				//是否初始化
CPtrArray					CProcessManage::g_pManageArray;				//进程管理类数组
CAFCSignedLock				CProcessManage::g_Lock;						//资源访问锁

//构造函数
CProcessManage::CProcessManage(void)
{
//	g_Lock.Lock();
	if (!g_bInit)
	{
		g_bInit=true;
		g_pManageArray.SetSize(10);
	}
	g_pManageArray.Add(this);
//	g_Lock.UnLock();
	return;
}

//析构函数
CProcessManage::~CProcessManage(void)
{
//	g_Lock.Lock();
	int uCount=(int)g_pManageArray.GetUpperBound();
	for (int i=0;i<uCount;i++)
	{
		if (g_pManageArray.GetAt(i)==this) 
		{
			g_pManageArray.RemoveAt(i);
			break;
		}
	}
//	g_Lock.UnLock();
	return;
}
