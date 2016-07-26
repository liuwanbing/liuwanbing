////#include "stdafx.h"

#include "StdAfx.h"
#include <winsock.h>
#include "socketinit.h"

CSocketInit::CSocketInit(void)
{
	WSAData wsaData;
	int nReturnCode = ::WSAStartup(MAKEWORD(2,0), &wsaData);
}

CSocketInit::~CSocketInit(void)
{
	::WSACleanup();
}

static CSocketInit   g_static_sktinit;
