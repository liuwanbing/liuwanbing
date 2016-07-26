#include "stdafx.h"
#include "SoGameLogerImpl.h"
#include "SoGameFactory.h"
#include "SoIddeFine.h"

#include "SoDeskLogicImpl.h"

#include "stdlib.h"
#include "stdio.h"

using namespace MyGame;

static FILE *g_pLogFile = NULL;

#define CU_LOG_FILE_NAME  "log.txt"

SoGameLogerImpl::SoGameLogerImpl(CDeskLogicImpl *pDeskLogicImpl)
{
    m_pDeskLogicImpl = pDeskLogicImpl;
    m_pDeskLogicImpl->m_pGameFactory->RegisterInterface(MODULE_SOGAMELOGGER, this);

};

SoGameLogerImpl::~SoGameLogerImpl(void)
{
    
};

BOOL SoGameLogerImpl::InitLog()
{
	if (g_pLogFile == NULL)
	{
		g_pLogFile = fopen((char*)CU_LOG_FILE_NAME,"w");
		if (g_pLogFile)
		{
			return TRUE;
		}
        else
        {
            return FALSE;
        }
	}
	return TRUE;
}


void  SoGameLogerImpl::OutputTraceInfo(const char* msg, ...)
{
    if (InitLog() == FALSE)
    {
        return;
    }

    va_list arglist;
    va_start(arglist, msg);
    vfprintf(g_pLogFile, (char*)msg, arglist);
	fflush(g_pLogFile);

};

void  SoGameLogerImpl::OutputVerboseInfo(const char* msg, ...)
{
    if (InitLog() == FALSE)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, msg);
    vfprintf(g_pLogFile, (char*)msg, arglist);
	fflush(g_pLogFile);

};

void  SoGameLogerImpl::OutputErrorInfo(const char* msg, ...)
{
    if (InitLog() == FALSE)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, msg);
    vfprintf(g_pLogFile, (char*)msg, arglist);
	fflush(g_pLogFile);
};

