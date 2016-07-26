#pragma once

#include "OpenglEngineHead.h"

class OPENGL_ENGINE_CLASS CGLLapseCount
{

	//变量定义
protected:
	DWORD							m_dwQueryTickCount;					//查询时间

	//变量定义
protected:
	static DWORD					m_dwCurrentTickCount;				//当前时间

	//函数定义
public:
	//构造函数
	CGLLapseCount();
	//析构函数
	virtual ~CGLLapseCount();

	//功能函数
public:
	//流逝配置
	VOID Initialization();
	//流逝判断
	DWORD GetLapseCount(DWORD dwPulseCount);

	//功能函数
public:
	//更新时间
	static VOID PerformLapseCount();
};
