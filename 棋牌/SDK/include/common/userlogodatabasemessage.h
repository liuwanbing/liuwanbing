#pragma once

#ifndef AFC_USER_LOGO_DATAMESSAGE_HEAD_FILE_
#define AFC_USER_LOGO_DATAMESSAGE_HEAD_FILE_

#include "MainManage.h"

//数据库消息
#define DTK_GP_USERLOGO_UPDATE			100						//更新头像ID
#define DTK_GP_USERLOGO_GETMD5			110						//取头像的MD5值

//更新用户头像数据库操作结构
typedef struct
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long								dwUserID;							//用户ID号
	int									nIconIndex;							//==0xFF，自定义，其它，系统默认
	char								szLogoMD5[33];						//用户头像文件的MD5值
} UL_GP_I_UpdateUserLogo;

//从数据库中取得头像的MD5值
typedef struct  
{
	DataBaseLineHead					DataBaseHead;
	long								dwRequestUserID;					//请求取头像MD5值的用户ID号
	long								dwUserID;							//被请求取头像MD5值的用户ID号
}UL_GP_I_RequestInformationLogoMD5;

typedef struct  
{
	DataBaseResultLine					ResultHead;							//数据包头
	MSG_UL_S_INFORMATION				UserLogoInformation;

}UL_GP_I_UserLogoInformation;

#endif