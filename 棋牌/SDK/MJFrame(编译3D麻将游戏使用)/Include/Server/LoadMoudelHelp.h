#ifndef __LOAD_MOUDEL_HELP_H__
#define __LOAD_MOUDEL_HELP_H__

#include "afxwin.h"
#include "IMainGameFactory_i.h"

//用此些方法进行加栽各个模块

IMainGameFactory* LoadDllMoudel(char* pMoudelName, BOOL bFindProc);

#endif
