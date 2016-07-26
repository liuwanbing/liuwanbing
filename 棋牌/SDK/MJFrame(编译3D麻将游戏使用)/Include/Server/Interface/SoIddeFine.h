
#if !defined(SOID_DEFINE_H)
#define SOID_DEFINE_H

#include "metainfo.h"

namespace MyGame
{	
	typedef char*  LPTSTR;
	
	#undef _T
	#define _T

	#define MODULE_USERMAGER				0x001
	#define MODULE_GAMEPROCESSOR			0x002
	#define MODULE_GAMESETTING				0x003
	#define MODULE_SOGAME					0x004
	#define MODULE_ITEMMANAGER				0x005
	#define MODULE_SOGAMELOGGER				0x006
	#define MODULE_SODBOPERATOR				0x007
	#define MODULE_XSOGAMESETTING				0x008

	#define EVENT_USERMANGER				0x100
	#define EVENT_USERMGRPROCESS            		0x101
	#define EVENT_GAMEPROCESSOR				0x102
	#define EVENT_ITEMMANAGER				0x103

	DEFINE_MODULE_ID(MODULE_USERMAGER,		ISoUserManager);
	DEFINE_MODULE_ID(MODULE_GAMEPROCESSOR,  ISoGameProcessor);
	DEFINE_MODULE_ID(MODULE_GAMESETTING,    ISoGameSetting);
	DEFINE_MODULE_ID(MODULE_SOGAME,			ISoGame);
	DEFINE_MODULE_ID(MODULE_ITEMMANAGER,	ISoItemManager);
	DEFINE_MODULE_ID(MODULE_SOGAMELOGGER,	ISoGameLogger);
	DEFINE_MODULE_ID(MODULE_SODBOPERATOR,	ISoDBOperator);
	DEFINE_MODULE_ID(MODULE_XSOGAMESETTING,   IXSoGameSetting);
	
	//////////////////////////////////////////////////////////////////////////
	// define event id
	DEFINE_EVENT_ID(0x00000001, ISoGameToUserEvent);
	DEFINE_EVENT_ID(0x00000002, ISoGameSDKMessageEvent);
 	DEFINE_EVENT_ID(0x00000003, ISoGameCallBackEvent);
	DEFINE_EVENT_ID(0x00000004, ISoGameToGameSettingEvent);
	DEFINE_EVENT_ID(0x00000005, ISoGameEvent);
	DEFINE_EVENT_ID(0x00000006, ISoUserManagerEvent);
	DEFINE_EVENT_ID(0x00000007, ISoUserMgrToGameProcessEvent);
	DEFINE_EVENT_ID(0x00000008, ISoGameProcessorEvent);
	DEFINE_EVENT_ID(0x00000009, ISoGameSettingEvent);
	DEFINE_EVENT_ID(0x0000000e, ISoItemManagerEvent);
	DEFINE_EVENT_ID(0x0000000f, ISoGameProcessorSendBaseInfoEvent);
	DEFINE_EVENT_ID(0x00000010, ISoGameProcessorGameEvent);
	DEFINE_EVENT_ID(0x00000011, ISoUserManagerTrusteeEvent);
	DEFINE_EVENT_ID(0x00000012, ISoGameProcessTimerEvent);
	DEFINE_EVENT_ID(0x00000013, ISoGameProcessInnerEvent);
	DEFINE_EVENT_ID(0x00000014, ISoGameProcessBaseMsgEvent);

	
	#define USER_EVENT_ID	0x00FFFFFF 
	// (0x01000000 ~ 0xFFFFFFFF is extern event id)
	// e.g: DEFINE_MODULE_ID(USER_EVENT_ID + 1, IMyUserManagerEvent);

};

#endif


