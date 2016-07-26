/******************************************************************
Copyright (C), 2002-2010, BOZONG TECHNOLOGLIES CO., LTD.
File name:		CLog.cpp
Version:		1.0
Description:	用于调此或是正式发布时，输出一个log文件，方便监视运行状态
Author:			duanxiaohui
Create Date:	2010-07-15
History:
------------------------------------
******************************************************************/

#include "stdafx.h"
#include "stdarg.h"
#include "CLog.h"
#include <string>
using namespace std;

CLog* CLog::g_instance = NULL;
FILE* CLog::m_HLogFile = NULL;

VOID (*(CLog::m_pOutIpc))(char* str) = NULL;
BOOL CLog::m_EnableOutIpc = 0;
BOOL CLog::m_EnableOutToFile = 0;
    
char g_logStrBuf[4098];


UINT logToFile  = 1;
UINT logToMem   = 0;
CLog::CLog()
{
    m_pOutIpc = NULL;
	m_HLogFile = NULL;
    m_pOutIpc = NULL;


}

int CLog::Init(char* fileName)
{
	if (fileName)
	{
		m_HLogFile = fopen((char*)fileName, "w");
		if (m_HLogFile)
		{
			return RE_SUCCEED;
		}
	}
	return RE_FAIL;
}


VOID CLog::OutStr(const char* str, ...)
{
    va_list arglist;

	string strOut;

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);
	char szTemp[1024] = {'\0'};
	sprintf(szTemp, "%d:%d:%d: ", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
	strOut = szTemp;

	va_start(arglist, str);

	strOut += str;
	strOut += "\n";
    

	if (m_HLogFile && (logToFile & 0x01))
	{
		//va_start(arglist, str);
        vfprintf(m_HLogFile, (char*)strOut.c_str(), arglist);
		fflush(m_HLogFile);

        memset(szTemp, 0, sizeof(szTemp));
        vsprintf((char*)szTemp, str, arglist);
        strcat(szTemp, "\r\n");
        OutputDebugString(szTemp);
	}	

    if (logToMem && m_pOutIpc)
    {
        char    outBuf[1024];
        //vsprintf((char*)outBuf, str, arglist);
        _vsnprintf((char*)outBuf, 1024, (char*)strOut.c_str(), arglist); 
        m_pOutIpc((char*)outBuf);
    }
}

VOID CLog::Release(VOID)
{
    if (m_HLogFile)
    {
        fclose(m_HLogFile);
        m_HLogFile = NULL;
	}

    if (g_instance)
    {
        delete g_instance;
        g_instance = NULL;
    }
}

CLog::~CLog()
{
	if (m_HLogFile)
	{
		fclose(m_HLogFile);
		m_HLogFile = NULL;
	}
}

/*我们希望在全局的某个地方，全部一次性调用此方法
 *以此完成此对象的初始化，以此实现单件工厂模式
 */
CLog* CLog::GetInstance(VOID)
{
	if (g_instance == NULL)
	{
		g_instance = new CLog;
	}
    return g_instance;
}

FILE* CLog::GetLogFile(VOID)
{
	return m_HLogFile;
}
