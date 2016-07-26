#ifndef GAME_PROPERTY_HEAD_HEAD_FILE
#define GAME_PROPERTY_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//MFC 文件
#include <Afxmt.h>
#include <AtlBase.h>
#include <AfxTempl.h>

//平台文件
#include "Platform.h"

//组件文件
#include "ServiceCoreHead.h"

//组件文件
#include "WHImageHead.h"
#include "UserServiceHead.h"
#include "SkinControlHead.h"
#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef GAME_PROPERTY_CLASS
	#ifdef  GAME_PROPERTY_DLL
		#define GAME_PROPERTY_CLASS _declspec(dllexport)
	#else
		#define GAME_PROPERTY_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define GAME_PROPERTY_DLL_NAME	TEXT("GameProperty.dll")			//组件名字
#else
	#define GAME_PROPERTY_DLL_NAME	TEXT("GamePropertyD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//道具大小
#define PROPERTY_IMAGE_CX			30									//图标大小
#define PROPERTY_IMAGE_CY			30									//图标大小

//图标大小
#define PROPERTY_SYMBOL_CX			120									//图标大小
#define PROPERTY_SYMBOL_CY			120									//图标大小

//命令定义
#define CMD_CLOSE_WND   			200                                 //关闭命令
#define CMD_USER_CHANGE				201                                 //选中用户

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//道具图片
struct tagPropertyImage
{
	LPCTSTR							pszFlash;							//动画路径
	LPCTSTR							pszImageID;							//控制图片
	LPCTSTR							pszSymbolID;						//标志图片
	HINSTANCE						hResInstance;						//资源句柄
};

//道具图片
struct tagPropertyResource
{
	//资源变量
	UINT							uImageL;							//图片标识
	UINT							uImageM;							//图片标识
	UINT							uImageR;							//图片标识
	UINT							uImageItem;							//图片标识
	HINSTANCE						hResInstance;						//资源句柄

	//位置信息
	SIZE							SizeExcursion;						//偏移位置
	POINT							PointExcursion;						//偏移位置
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGamePropertySink INTERFACE_VERSION(1,1)
	static const GUID IID_IGamePropertySink={0xf9dcc979,0xce63,0x4fa5,0xab,0xa9,0xad,0x9e,0x16,0xa7,0x50,0xa6};
#else
	#define VER_IGamePropertySink INTERFACE_VERSION(1,1)
	static const GUID IID_IGamePropertySink={0xb7fb684b,0x4c23,0x4d47,0x9f,0xdb,0xed,0xad,0x05,0x44,0x7b,0x9e};
#endif

//道具接口
interface IGamePropertySink : public IUnknownEx
{
	//购买道具
	virtual bool OnEventBuyProperty(LPCTSTR pszNickName, WORD wItemCount, WORD wPropertyIndex)=NULL;
	//查询用户
	virtual IClientUserItem * OnQueryClientUserItem()=NULL;
	//购前检查
	virtual bool OnEventBuyPropertyPrep(LPCTSTR pszNickName,WORD wPropertyIndex,LPTSTR pszMessage)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGamePropertyUseSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGamePropertyUseSink={0xb1dfdc0b, 0x57dd, 0x43f6, 0xba, 0x8b, 0x6b, 0xac, 0xd, 0x4e, 0x1f, 0xfa};
#else
	#define VER_IGamePropertyUseSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGamePropertyUseSink={0x1b0bad1a, 0x9694, 0x42a4, 0x9e, 0xf0, 0xf7, 0x35, 0x1e, 0x2d, 0x11, 0x34};
#endif

//道具接口
interface IGamePropertyUseSink : public IUnknownEx
{
	//使用道具
	virtual bool OnEventUseProperty(WORD wPropertyIndex)=NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

//导出文件

#ifndef GAME_PROPERTY_DLL
	#include "DlgTrumpetItem.h"
	#include "GamePropertyCtrl.h"
	#include "GamePropertyItem.h"
	#include "GamePropertyManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif