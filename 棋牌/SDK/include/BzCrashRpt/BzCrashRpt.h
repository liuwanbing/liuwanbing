#ifndef _BZ_CRASH_RPT_H_INCLUDE
#define _BZ_CRASH_RPT_H_INCLUDE

/**
 * BzCrashRpt 错误处理类
 *
 * 这个类的目的用于程序中出现异常或错误时，发送错误报告到指定的邮箱，以便于
 * 开发人员分析
 *
 * 作者: akinggw
 * 日期: 2010.3.12
 */

#include "BzCommon.h"

/// 初始化我们自己的异常处理机制
EXT_CLASS void          BzCrash_Initiation(void); 
/// 屏蔽windows系统中的异常处理机制(主要针对VS2005),在设置完自定义的异常处理机制后使用
EXT_CLASS void          BzCrash_DisableSetUnhandledExceptionFilter();
/// 设置作用项目名称
EXT_CLASS void          BzCrash_SetProjectName(const char* name);
/// 得到异常信息
EXT_CLASS const char*   BzCrash_GetExceptionString(void);
/// 得到作用项目名称
EXT_CLASS const char*   BzCrash_GetProjectName(void);
/// 设置压缩密码(暂时无用)
EXT_CLASS void          BzCrash_SetZipPassword(const char* pass);
/// 得到压缩密码
EXT_CLASS const char*   BzCrash_GetZipPassword(void);
/// 设置发送者邮件地址
EXT_CLASS void          BzCrash_SetEmailSender(const char* address);
/// 设置接收者邮件地址
EXT_CLASS void          BzCrash_SetEmailReceiver(const char* address);
/// 设置Smtp服务器
EXT_CLASS void          BzCrash_SetSmtpServer(const char* address);
/// 设置Smtp用户
EXT_CLASS void          BzCrash_SetSmtpUser(const char* name);
/// 设置Smtp用户密码
EXT_CLASS void          BzCrash_SetSmtpPassword(const char* pass);
/// 设置是否发送完成后删除文件（初始不删除）
EXT_CLASS void          BzCrash_DeleteSended(bool isDel);

#endif
