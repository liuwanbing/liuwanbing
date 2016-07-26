//////////自动升级
#include "StdAfx.h"
#include "AutoUpdate.h"

//构造函数
CAutoUpdate::CAutoUpdate(void)
{
}
//析构函数
CAutoUpdate::~CAutoUpdate(void)
{
}
//更新函数
bool CAutoUpdate::Start(bool bForce)
{
	return true;
}

//停止自动更新
bool CAutoUpdate::Stop()
{	return true;
}

//更新线程
unsigned __stdcall CAutoUpdate::UpdateThread(LPVOID pThreadData)
{	//获取分析文件
	CInternetSession Seccion;
	return 0;
}