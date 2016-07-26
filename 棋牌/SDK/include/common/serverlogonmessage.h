#pragma once

#ifndef AFC_SERVER_LOGON_MESSAGE_HEAD_FILE
#define AFC_SERVER_LOGON_MESSAGE_HEAD_FILE

#include "MainManage.h"
/********************************************************************************************/

//版本定义
#define GAME_MAX_VER					0								//现在最高版本
#define GAME_LESS_VER					0								//现在最低版本

//常量定义
#define MAX_CONNECT						300								//最多连接数
#define DATA_BASE_NAME					TEXT("AFCService")		//TEXT("ServerAccess")			//数据库名字

/********************************************************************************************/

//	网络通信数据包定义 

/********************************************************************************************/

//登陆服务器登陆信息
struct MSG_SL_S_LogonByNameInfo
{
	char								szName[61];							//登陆名字
	char								szMD5Pass[50];						//登陆密码
	char								szServerGUID[37];					//服务器 GUID
};

//登陆服务器登陆信息
struct MSG_SL_R_ConnectInfo
{
	long int								dwUserPower;						//用户权限
	char								szSQLName[61];						//SQL 名字
	char								szSQLPass[61];						//SQL 密码
	char								szSQLAddr[16];						//SQL 地址
	char								szServiceIP[16];					//服务 IP
};

/********************************************************************************************/

//	数据库通信数据包定义 

/********************************************************************************************/

//登陆服务器登陆信息
struct DL_SL_I_ServerLogonInfo
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	MSG_SL_S_LogonByNameInfo			LogonInfo;							//登陆信息
	char								szAccessIP[16];						//登陆 IP
};

//登陆服务器登陆结果
struct DL_SL_O_ServerLogonInfo
{
	DataBaseResultLine					ResultHead;							//结果数据包头
	int								dwUserPower;						//用户权限
	char								szSQLName[61];						//SQL 名字
	char								szSQLPass[61];						//SQL 密码
	char								szSQLAddr[16];						//SQL 地址
	char								szServiceIP[16];					//服务 IP
};

/********************************************************************************************/

//	通信标识定义 

/********************************************************************************************/

//通信主标识
#define	MDM_SL_SERVICE					10									//服务主标识

//通信辅助标识
#define ASS_SL_LOGON_BY_NAME			1									//通过用户名字登陆
#define ASS_SL_LOGON_SUCCESS			2									//登陆成功
#define ASS_SL_LOGON_ERROR				3									//登陆失败

//错误代码
#define ERR_SL_ERROR_UNKNOW				0									//未知错误
#define ERR_SL_GUID_ERROR				1									//GUID 错误
#define ERR_SL_LOGON_IP_ERROR			2									//登陆 IP 错误 
#define ERR_SL_LOGON_NAME_ERROR			3									//登陆名字错误
#define ERR_SL_LOGON_NAME_LIMIT			4									//登陆名字限制
#define ERR_SL_LOGON_PASS_ERROR			5									//登陆密码错误
#define ERR_SL_LOGON_SUCCESS			6									//登陆成功

/********************************************************************************************/

//服务登陆处理标识
#define DTK_SL_LOGON_BY_NAME			1									//服务通过名字登陆
#define DTK_SL_CHECK_CONNECT			2									//检测断线连接

//服务登陆错误代码
#define DTR_SL_ERROR_UNKNOW				0									//未知错误
#define DTR_SL_GUID_ERROR				1									//GUID 错误 
#define DTR_SL_LOGON_IP_ERROR			2									//登陆 IP 错误 
#define DTR_SL_LOGON_NAME_ERROR			3									//登陆名字错误
#define DTR_SL_LOGON_NAME_LIMIT			4									//登陆名字限制
#define DTR_SL_LOGON_PASS_ERROR			5									//登陆密码错误
#define DTR_SL_LOGON_SUCCESS			6									//登陆成功

/********************************************************************************************/

#endif
