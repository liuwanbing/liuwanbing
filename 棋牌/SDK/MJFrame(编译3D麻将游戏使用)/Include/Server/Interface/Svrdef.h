#if !defined(SO_SVRDEF_H)
#define SO_SVRDEF_H

#ifndef NULL
#define	NULL	0
#endif

typedef unsigned char       BYTE;
typedef unsigned int        UINT;

#ifndef WIN32
#ifndef BOOL
#define BOOL	int
#define TRUE	1
#define FALSE	0
#endif	// BOOL
#else
#include "wtypes.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "typeDef.h"

//BEGIN modified by nekeyzhong for 欢乐体系2006-1-9 sequence:mainsvr_111_20060109
#ifdef WIN32
#include <wtypes.h>
#define int64_t  INT64
#else
#include <stdint.h>

#endif
//END modified by nekeyzhong 


#define MAX_NUMBER_MODULE	20
#define MAX_PLUGIN_COUNT    20   // 最多支持20个插件，可以根据需要修改 Comment by Coolhomli

#endif

