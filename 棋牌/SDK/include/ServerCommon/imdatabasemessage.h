#pragma once

#ifndef AFC_IM_DATAMESSAGE_HEAD_FILE_
#define AFC_IM_DATAMESSAGE_HEAD_FILE_

#include "AFCDataBase.h"
#include "MainManage.h"
//数据库消息
#define DTK_GP_IM_CLEAR					200						//服务器启动时，用户清0，单服务器适用
#define DTK_GP_IM_GETCOUNT				201						//取得用户好友数
#define DTK_GP_IM_USERLIST				202						//返回用户的好友信息(列表)
#define DTK_GP_IM_USERONLINE			203						//上、下线返回用户资料
#define DTK_GP_IM_ONLINE				204						//上线向用户发资料
#define DTK_GP_IM_OFFLINE				205						//下线向数据库写记录
#define DTK_GP_IM_SETUSER				206						//设置好友分组


typedef struct  
{
	DataBaseLineHead					DataBaseHead;				//数据包头
	long								dwUserID;					//用户ID
	long								dwRemoteID;					//好友的ID
	int									groupID;					//组ID，1-好友，3-黑名单，0-删除
}IM_GP_I_SETUSER;
//用户下线
typedef struct  
{
	DataBaseLineHead					DataBaseHead;				//数据包头
	long								dwUserID;					//下线用户的ID号
}IM_GP_I_USEROFFLINE;

//取好友数量，未使用，保留
typedef struct
{
	DataBaseLineHead					DataBaseHead;				//数据包头
	long								dwUserID;					//用户ID号
	int									dwUserCount;				//好友数
} IM_GP_I_GETUSERCOUNT;

//取好友的数量，现未使用，保留
typedef struct  
{
	DataBaseResultLine					ResultHead;					//数据包头
	long								dwUserID;					//用户ID号
	int									dwUserCount;				//好友数
	int									dwUserCountReturn;			//返回的好友数
}IM_GP_O_GETUSERCOUNT;

//取好友列表
typedef struct
{
	DataBaseResultLine					ResultHead;
	int									dwListCount;				//此消息中有几个用户
	int									dwLength;					//消息体长度
	char								szData[MAX_BLOCK_MSG_SIZE];	//最大长度
}IM_GP_O_USERLIST;

//单个用户信息
typedef struct  
{
	DataBaseResultLine					ResultHead;
	MSG_IM_S_GETUSERINFO				userInfo;
}IM_GP_O_USERINFO;


//销售列表信息
struct DL_GP_O_SellGameListResult
{
	DataBaseResultLine					ResultHead;
	MSG_GP_R_SellGameListResult         SellGameListResult;
};
#endif