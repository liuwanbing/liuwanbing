// 麻将框架底层文件
#ifndef WINDOWS_LOGIC
	// 服务端文件
	#include "SoDeskLogicImpl.cpp"
	#include "SoGameFactory.cpp"
	#include "SoGameLogerImpl.cpp"
	#include "SoGameProcessor.cpp"
	#include "SoGameSettingImpl.cpp"
	#include "SoUserManager.cpp"
	#include "SoBaseLogic.cpp"
	#include "CLog.cpp"
	#include "LoadMoudelHelp.cpp"

	#include "SoMJTimerMgr.cpp"
	#include "SoMJFrameLogic.cpp"
	#include "SoMJFrameSetting.cpp"
#else
    // 客户端文件
	#include "MJFrameLogic.cpp"
#endif

// 麻将框架逻辑公用库文件
#include "IMJStateEvent.cpp"
#include "MJ.cpp"
#include "MJRule.cpp"
#include "MJFanCounter.cpp"
#include "ComConfig.cpp"