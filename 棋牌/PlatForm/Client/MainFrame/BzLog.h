// ***************************************************************
//  BzLog   version:  1.0   ·  date: 01/17/2011
//  -------------------------------------------------------------
//    游戏日志文件操作类
//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

class CBzLog
{

public:

	CBzLog(void);

	//创建文件夹
	static bool CreateGameRoomDir(char* szName);

	//创建日志
	bool CreateGameRoomLog(const ComRoomInfo* pRoomInfo);

	//释放日志
	bool ReleaseBzLog();

	//写日志
	bool Writeline(CString str);

	//获取日志ID
	const char* GetLogID();

public:

	~CBzLog(void);

public:

	static void init();

	//输出调试信息,带文本日志
	void OutputTraceStr(const char *lpszFormat, ...);

private:

	//日志文件
	CFile* m_pLogFile;

	//游戏ID
	UINT m_uGameID;

	//日志文件名
	char m_szName[MAX_PATH];

	//日志ID
	char m_szLogID[32];

	//目录名
	static char m_szRootName[64];

};