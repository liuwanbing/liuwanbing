//////////////////////////////////////////////////////////////////////////
/// 调试信息、出错信息输出函数定义
/// 其中，DebugPrintf用于调试跟踪时输出信息，发行时可以修改函数体，不写文件直接返回
/// ErrorPrintf用于程序中异常状况的输出，发行时也一样输出，供跟踪错误时提供信息

#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
//#include "Debug_new.h"

#define DEBUGFILE_NAME 10003300

/// 获取运行程序所在路径名，字符串末尾不加反斜杠
TCHAR *G_GetAppPath()
{
	static TCHAR s_Path[MAX_PATH];
	static bool s_bIsReady = false;
	if (!s_bIsReady)
	{
		ZeroMemory(s_Path,sizeof(s_Path));
		DWORD dwLength=GetModuleFileName(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
		TCHAR *p = _tcsrchr(s_Path, TEXT('\\'));
		*p = TEXT('\0');
		s_bIsReady = true;
	}
	return s_Path;
}
/// 打印调试信息
void DebugPrintf(const TCHAR *p, ...)
{
	TCHAR szTimeStr[32];
	TCHAR szDateStr[32];
	TCHAR szFilename[256];
	wsprintf( szFilename, TEXT("%s\\Log\\%dLogic.txt"), G_GetAppPath(), DEBUGFILE_NAME );
	va_list arg;
	va_start( arg, p );
	FILE *fp = NULL;
	//fopen_s(&fp, szFilename, "a" );  _tfopen
	fp = _tfopen(szFilename, TEXT("a") );
	if (NULL == fp)
	{
		return;
	}
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	GetTimeFormat( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, TEXT("HH:mm:ss"), szTimeStr, 32);
	GetDateFormat( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, TEXT("yyyy-MM-dd"), szDateStr, 32);
	_ftprintf(fp, TEXT("[%s %s]--"), szDateStr, szTimeStr);
	_vftprintf(fp, p, arg );
	_ftprintf(fp, TEXT("\n"));
	fclose(fp);
}


/// 打印错误信息
void ErrorPrintf(const TCHAR *p, ...)
{
	TCHAR szTimeStr[32];
	TCHAR szDateStr[32];
	TCHAR szFilename[256];
	wsprintf( szFilename, TEXT("%s\\Log\\%dLogicError.txt"), G_GetAppPath(), DEBUGFILE_NAME );
	va_list arg;
	va_start( arg, p );
	FILE *fp = NULL;
	//fopen_s(&fp, szFilename, "a" );  _tfopen
	fp = _tfopen(szFilename, TEXT("a") );
	if (NULL == fp)
	{
		return;
	}
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	GetTimeFormat( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, TEXT("HH:mm:ss"), szTimeStr, 32);
	GetDateFormat( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, TEXT("yyyy-MM-dd"), szDateStr, 32);
	_ftprintf(fp, TEXT("[%s %s]--"), szDateStr, szTimeStr);
	_vftprintf(fp, p, arg );
	_ftprintf(fp, TEXT("\n"));
	fclose(fp);
}