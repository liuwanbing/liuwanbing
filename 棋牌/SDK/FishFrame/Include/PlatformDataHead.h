#ifndef PLATFORM_DATA_HEAD_HEAD_FILE
#define PLATFORM_DATA_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//平台文件

//平台文件
#include "Platform.h"

//类头文件
#include "WHImageHead.h"
#include "WHSocketHead.h"
#include "SkinControlHead.h"
#include "AvatarControlHead.h"
#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef PLATFORM_DATA_CLASS
	#ifdef  PLATFORM_DATA_DLL
		#define PLATFORM_DATA_CLASS _declspec(dllexport)
	#else
		#define PLATFORM_DATA_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformData.dll")			//组件 DLL 名字
#else
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformDataD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef PLATFORM_DATA_DLL
	#include "GlobalServer.h"
	#include "GlobalUserInfo.h"
	#include "OptionParameter.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif