#ifndef _SOLOGGER_H
#define _SOLOGGER_H

#include "Svrdef.h"
#include "gameobject_i.h"
#include "gameevent_i.h"


// SO的日志系统：作为一个模块，使用的时候，首先Query之，然后调用其接口进行日志的输出。支持动态刷新log的配置文件
// log的配置文件，对所有房间生效，是个全局的配置。
// [setting]
// log = evtf1s	//log的含义： e(error开关）  v（verbose开关）t（trace开关）f1（输出到文件）s（输出到屏幕）
// logPath = "../log/DaGunZi"

namespace MyGame
{
									// ->8765 4321<-
									//  "0000,0000"
	enum 
	{
		configed = 0x80,		//  8bit:是否已经读取配置
		excludelog = 0x40,		//  7bit:排除 exclude
        //tomultifile = 0x10,	//  5bit:是否写入分离文件
		toscreen = 0x10,		//  5bit:是否printf到终端（配置文件中：s）
		tosinglefile = 0x8,		//  4bit:是否写入统一文件（配置文件中：f1）
		logerror = 0x4,			//  3bit:是否Log错误 
		logverbose = 0x2,		//  2bit:是否Log详细
		logtrace = 0x1			//  1bit:是否Log普通 
	};
	class ISoGameLogger : public IGameObject
	{
	public:
		virtual void  OutputTraceInfo(const char* msg, ...) = 0;     
		virtual void  OutputVerboseInfo(const char* msg, ...) = 0;     
		virtual void  OutputErrorInfo(const char* msg, ...) = 0; 
// 		virtual BYTE GetGlobalLoggerFlag() = 0;
// 		virtual void OutputLog(const char* lpszLogBuf) = 0;
	};

}

#endif //_SOLOGGER_H
