/******************************************************************
Copyright (C), 2002-2010, BOZONG TECHNOLOGLIES CO., LTD.
File name:		CLog.h
Version:		1.0
Description:	用于调此或是正式发布时，输出一个log文件，方便监视运行状态
Author:			duanxiaohui
Create Date:	2011-12-05
History:
------------------------------------

******************************************************************/
#ifndef __CLog_H__
#define __CLog_H__

#include "afxwin.h"
#include "typeDef.h"

#define myPrintf(fmt)								\
    do{												\
    CLog::OutStr(fmt);					    	    \
    printf(fmt);                                    \
    }while(0)

#define myPrintf1(fmt, p1)							\
    do{												\
    CLog::OutStr(fmt, p1);					        \
    printf(fmt, p1);                                \
    }while(0)

#define myPrintf2(fmt, p1, p2)						\
    do{												\
    CLog::OutStr(fmt, p1, p2);				        \
    printf(fmt, p1, p2);                            \
    }while(0)

#define myPrintf3(fmt, p1, p2, p3)					\
    do{												\
    CLog::OutStr(fmt, p1, p2, p3);                  \
    printf(fmt, p1, p2, p3);                        \
    }while(0)

#define myPrintf4(fmt, p1, p2, p3, p4)				\
    do{												\
    CLog::OutStr(fmt, p1, p2, p3, p4);			    \
    printf(fmt, p1, p2, p3, p4);                    \
    }while(0)

#define myPrintf5(fmt, p1, p2, p3, p4, p5)			\
    do{												\
    CLog::OutStr(fmt, p1, p2, p3, p4, p5);		    \
    printf(fmt, p1, p2, p3, p4, p5);                \
    }while(0)

#define myPrintf6(fmt, p1, p2, p3, p4, p5, p6)		\
    do{												\
    CLog::OutStr(fmt, p1, p2, p3, p4, p5, p6);	    \
    printf(fmt, p1, p2, p3, p4, p5, p6);            \
    }while(0)

#define myPrintf7(fmt, p1, p2, p3, p4, p5, p6, p7)  \
    do{												\
    CLog::OutStr(fmt, p1, p2, p3, p4, p5, p6, p7);	\
    printf(fmt, p1, p2, p3, p4, p5, p6, p7);        \
    }while(0)

#ifdef _DEBUG
#define OUT_EBUG 1
#endif

//调试用
#ifdef  OUT_EBUG 
#define _DOUT(str) 							        myPrintf((char*)str)
#define _DOUT1(str, p1)						        myPrintf1((char*)str, p1)
#define _DOUT2(str, p1, p2) 					    myPrintf2((char*)str, p1, p2)
#define _DOUT3(str, p1, p2, p3) 				    myPrintf3((char*)str, p1, p2, p3)
#define _DOUT4(str, p1, p2, p3, p4) 			    myPrintf4((char*)str, p1, p2, p3, p4)
#define _DOUT5(str, p1, p2, p3, p4, p5) 		    myPrintf5((char*)str, p1, p2, p3, p4, p5)
#define _DOUT6(str, p1, p2, p3, p4, p5, p6) 	    myPrintf6((char*)str, p1, p2, p3, p4, p5, p6)
#define _DOUT7(str, p1, p2, p3, p4, p5, p6, p7)     myPrintf7((char*)str, p1, p2, p3, p4, p5, p6, p7)
#else
#define _DOUT(str)
#define _DOUT1(str, p1)
#define _DOUT2(str, p1, p2)
#define _DOUT3(str, p1, p2, p3)
#define _DOUT4(str, p1, p2, p3, p4) 	
#define _DOUT5(str, p1, p2, p3, p4, p5)
#define _DOUT6(str, p1, p2, p3, p4, p5, p6)
#define _DOUT7(str, p1, p2, p3, p4, p5, p6, p7)

#endif

extern UINT logToFile;
extern UINT logToMem;

class CLog
{
public:
	CLog();
	~CLog();
	static CLog*		GetInstance(VOID);
	static FILE*		GetLogFile(VOID);
	static STATUS		Init(char* fileName);
	static VOID	        OutStr(const char* str, ...);
	static VOID			Release(VOID);


protected:
private:
	static CLog*		g_instance;
	static FILE*		m_HLogFile;

public:
    static VOID         (*m_pOutIpc)(char* str);
    static BOOL        m_EnableOutIpc;
    static BOOL        m_EnableOutToFile;
};

#endif
