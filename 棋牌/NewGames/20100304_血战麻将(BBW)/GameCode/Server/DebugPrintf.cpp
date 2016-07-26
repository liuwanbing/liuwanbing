//////////////////////////////////////////////////////////////////////////
/// 调试信息、出错信息输出函数定义
/// 其中，DebugPrintf用于调试跟踪时输出信息，发行时可以修改函数体，不写文件直接返回
/// ErrorPrintf用于程序中异常状况的输出，发行时也一样输出，供跟踪错误时提供信息

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Debug_new.h"
#include "stdafx.h"
#define DEBUGFILE_NAME 20154400

/**
* @brief 获取运行程序所在路径名，字符串末尾不加反斜杠
*/ 
char *G_GetAppPath()
{
    static char s_Path[MAX_PATH];
    static bool s_bIsReady = false;
    if (!s_bIsReady)
    {
        ZeroMemory(s_Path,sizeof(s_Path));
        DWORD dwLength=GetModuleFileNameA(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
        char *p = /*_tcsrchr*/strrchr(s_Path, '\\');
        *p = '\0';
        s_bIsReady = true;
    }
    return s_Path;
}
/// 打印调试信息
void DebugPrintf_debug(const char *p, ...)
{
    char szTimeStr[32];
    char szDateStr[32];
    char szFilename[256];
    sprintf( szFilename, "%s\\Log\\%dLogic.txt", G_GetAppPath(), DEBUGFILE_NAME);
    FILE *fp = fopen( szFilename, "a" );
    if (NULL == fp)
    {
        return;
    }
    va_list arg;
    va_start( arg, p );
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    GetTimeFormatA( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, "HH:mm:ss", szTimeStr, 32);
    GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, "yyyy-MM-dd", szDateStr, 32);
    fprintf(fp, "[%s %s]--", szDateStr, szTimeStr);
    vfprintf(fp, p, arg );
    fprintf(fp,"\n");
    fclose(fp);
}


/// 打印错误信息
void ErrorPrintf_debug(const char *p, ...)
{
    char szTimeStr[32];
    char szDateStr[32];
    char szFilename[256];
    sprintf( szFilename, "%s\\Log\\%dLogic.txt", G_GetAppPath(), DEBUGFILE_NAME);
    FILE *fp = fopen( szFilename, "a" );
    if (NULL == fp)
    {
        return;
    }
    va_list arg;
    va_start( arg, p );
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    GetTimeFormatA( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, "HH:mm:ss", szTimeStr, 32);
    GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, "yyyy-MM-dd", szDateStr, 32);
    fprintf(fp, "[%s %s]--", szDateStr, szTimeStr);
    vfprintf(fp, p, arg );
    fprintf(fp,"\n");
    fclose(fp);
}

/// 打印调试信息
void LBWOutputDebugString_debug(const char *lpszFormat, ...)
{
    va_list pArg;
    va_start(pArg, lpszFormat);
    char szMessage[1024] = { 0 };
    _vsnprintf(szMessage, 1023, lpszFormat, pArg);
    va_end(pArg);
    OutputDebugStringA(szMessage);
}
