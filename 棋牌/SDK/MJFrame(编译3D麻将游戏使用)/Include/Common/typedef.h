/******************************************************************
Copyright (C), 2002-2010, BOZONG TECHNOLOGLIES CO., LTD.
File name:		typeDef.h
Version:		1.0
Description:	定义内部的常用数据类型
Author:			duanxiaohui
Create Date:	2010-07-15
History:
------------------------------------

******************************************************************/

#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__


#ifdef SER_WIN
  #include "afxwin.h"
	#include "windows.h"
    #include "stdlib.h"
    #include "stdio.h"
    
#pragma warning(disable:4996)
 
#else
    #include "stdlib.h"
    #include "stdio.h"
#endif

//以下类型不推介使用，应明确使用后面带32等类型说明
typedef int  STATUS;

#define RE_SUCCEED		(0)
#define RE_FAIL			(-1)

#ifdef  SER_WIN
    #define USING_NAME_STD using namespace stdext;using namespace std;
    //#define USING_NAME_STD using namespace std;

    #define snprintf    _snprintf
	#ifdef  _DLL_EXPORTS
		#define SERAPI __declspec(dllexport)
	#else
		#define SERAPI __declspec(dllimport)
	#endif
#else
    #define USING_NAME_STD using namespace std; using namespace __gnu_cxx;

	#ifdef  _DLL_EXPORTS
		#define SERAPI __attribute__ ((visibility("default")))
	#else
		#define SERAPI 
	#endif
	
#endif

#undef  max
#ifndef  max
#define  max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#undef  min
#ifndef  min
#define  min(a,b)  (((a) < (b)) ? (a) : (b))
#endif



//#define LOG_FREE_LOG        1
//#define LOG_MALLOC_LOG      1

#ifdef  LOG_FREE_LOG
#define  LOG_FREE(p)                                                    \
{                                                                       \
    if (p)                                                              \
    {                                                                   \
        (p)->testWriteTestData(__LINE__, __FILE__, ((struct STMemItem*)(p)));                   \
    };                                                                  \
};       
#else
#define  LOG_FREE(p) 
#endif

#if (LOG_FREE_LOG || LOG_MALLOC_LOG )
#define  LOG_PRINTF(p)                                                  \
{                                                                       \
    if (p)                                                              \
    {                                                                   \
        (p)->testPrintfTestData();                                      \
    };                                                                  \
};   

#else
#define  LOG_PRINTF(p) 
#endif

#ifdef LOG_MALLOC_LOG
#define  LOG_MALLOC(p)                                                  \
{                                                                       \
    if (p)                                                              \
    {                                                                   \
        (p)->testWriteTestData(__LINE__, __FILE__,((struct STMemItem*)(p)));                     \
    };                                                                  \
}
#else
#define  LOG_MALLOC(p)
#endif

#endif
