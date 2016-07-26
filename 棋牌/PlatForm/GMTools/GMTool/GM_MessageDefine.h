//////////////////////////////////////////////////////////////////////////
/// 用于GM功能的Socket消息定义，数据访问协议定义
/// 创建：曾宪冬
/// 日期：20100722
//////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __GM_MESSAGE_DEFINE_H
#define __GM_MESSAGE_DEFINE_H

#include <windows.h>
#include "basemessage.h"	///< 获得struct NetMessageHead定义

/// 用来在结构体里初始化成员的宏定义，把所有成员都赋初值0
/// ★★★使用注意事项，使用本宏的结构体内不能有虚函数★★★
///
#define INIT_STRUCT( T ) \
	T()\
	{\
	ZeroMemory(this, sizeof(T));\
	}
/////////////// 示例代码 ///////////////////
// struct Test_t
// {
// 	 int _nLen;
//	 char _pStr[256];
//	 INIT_STRUCT( Test_t );	///< 结构体里加上这个宏，即相当于添加了一个初始化成员的构造函数
// };

#ifndef STRUCT_DATALINEHEAD
#define STRUCT_DATALINEHEAD
//数据队列信息头
struct DataLineHead
{
	UINT						uSize;								//数据大小
	UINT						uDataKind;							//数据类型
	INIT_STRUCT(DataLineHead);
};
#else
struct DataLineHead;
#endif//STRUCT_DATALINEHEAD

#ifndef STRUCT_DATABASERESULTLINE
#define STRUCT_DATABASERESULTLINE
///数据库结果消息结构定义
struct DataBaseResultLine
{
	DataLineHead						LineHead;					///队列头
	UINT								uHandleRusult;				///结果结果
	UINT								uHandleKind;				///处理类型
	UINT								uIndex;						///对象索引
	DWORD								dwHandleID;					///对象标识
	INIT_STRUCT(DataBaseResultLine);
};
#endif //STRUCT_DATABASERESULTLINE

#ifndef STRUCT_DATABASELINEHEAD
#define STRUCT_DATABASELINEHEAD
///数据库数据包头结构
struct DataBaseLineHead
{
	DataLineHead					DataLineHead;							///队列头
	UINT							uHandleKind;							///处理类型
	UINT							uIndex;									///对象索引
	DWORD							dwHandleID;								///对象标识
	INIT_STRUCT(DataBaseLineHead);
};
#endif //STRUCT_DATABASELINEHEAD

///---------------------
/// 操作类型定义
///---------------------
#define GM_ACT_AWORD		1	///< 奖励，奖励金额保存到银行中，不实时写入用户钱包里，本条消息发送到Z服务器
#define GM_ACT_NO_TALK		2	///< 禁言，被操作的玩若当前在某房间中，GM管理工具客户端须同时向该玩家所在的W服务器发本条消息
#define GM_ACT_KICK_OUT		3	///< 踢出房间，被操作的玩若当前在某房间中，GM管理工具客户端须同时向该玩家所在的W服务器发本条消息
#define GM_ACT_BANDON_ID	4	///< 禁止登录，即封号，被操作的玩若当前在某房间中，GM管理工具客户端同时向该玩家所在的W服务器发本条消息


///=======================================================================
/// Socket协议定义
///=======================================================================

//{{ Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议
/// 通信主标识
#ifndef MDM_GP_PLAYER_2_GM
#define MDM_GP_PLAYER_2_GM				107								/// 普通玩家与GM之间的通信协议
#	define ASS_GP_GET_GM_LIST			1									///<获取当前在线GM列表
#	define ASS_GP_TALK_2_GM				2									///<向指定编号的GM发送消息
#	define ASS_GP_MSG_FROM_GM			3									///<接收从GM处发来的消息
#endif//MDM_GP_PLAYER_2_GM
// Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议}}


#define MDM_GAMEMASTER_MESSAGE	255	///< GM消息主ID

///-----------------------------------------------------------------------
/// 下列为GM消息辅ID定义
///-----------------------------------------------------------------------
/// 1、下发奖励方案
/// 2、下发指定玩家的名字ID信息、历史处罚记录和当前状态
/// 3、上传对玩家的操作
/// 4、与某特定玩家发言，及广播消息，公告
/// 6、用于Z服务器，取得道具ID与名称对应关系
/// 7、GM登录协议，与普通玩家登录采用不同协议
///-----------------------------------------------------------------------
#	define	ASS_GM_MESSAGE_AWARD_TYPE		1	///< 上行时无参数，下行有参数结构
#		define HC_GM_GET_LIST_OK				1	///< 下行时表示本条数据为有效数据，未完待续
#		define HC_GM_GET_LIST_END				2	///< 下行时表示本条数据为结束指示
#	define	ASS_GM_MESSAGE_OPERATE_HISTORY	2	///< 上行时有简单参数，参数为类型加用户ID或登录名或昵称，下行有参数结构
#	define	ASS_GM_MESSAGE_OPERATE_ACT		3	///< 上行时有参数，下行无参数
#	define	ASS_GM_MESSAGE_BROADCAST		4	///< 上行时有参数，下行沿用原有广播消息
#	define	ASS_GM_MESSAGE_LOGON_USER		5	///< 无上行，下行有参数结构
#	define	ASS_GM_MESSAGE_PROP_INFO		6	///< 上行时无参数，下行有参数结构，取道具ID与名称对应关系列表
#	define	ASS_GM_MESSAGE_GM_LOGON			7	///< 上行时同普通用户登录，下行时与普通用户登录相比有所减少，无头像ID之类
#	define  ASS_GM_MESSAGE_PRINTLOG			8	///< 通知W打印日志的消息
/// 以上为GM消息辅ID定义



///错误代码，若在其它地方已经定义则不需要定义了
#ifndef ERR_GP_ERROR_UNKNOW
#	define ERR_GP_ERROR_UNKNOW				0									///未知错误
#	define ERR_GP_LOGON_SUCCESS				1									///登陆成功
#	define ERR_GP_USER_NO_FIND				2									///登陆名字错误
#	define ERR_GP_USER_PASS_ERROR			3									///用户密码错误
#	define ERR_GP_USER_VALIDATA				4									///用户帐号禁用
#	define ERR_GP_USER_IP_LIMITED			5									///登陆 IP 禁止
#	define ERR_GP_USER_EXIST				6									///用户已经存在
#	define ERR_GP_PASS_LIMITED				7									///密码禁止效验
#	define ERR_GP_IP_NO_ORDER				8									///不是指定地址 
#	define ERR_GP_LIST_PART					9									///部分游戏列表
#	define ERR_GP_LIST_FINISH				10									///全部游戏列表
#	define ERR_GP_USER_LOGON				11									///此帐号已经登录
#	define ERR_GP_USERNICK_EXIST			12									///此昵称已经存在
#	define ERR_GP_USER_BAD					13									///未法字符
#	define ERR_GP_IP_FULL					14									///IP已满
#	define ERR_GP_LOCK_SUCCESS				15									///锁定机器成功	2009-11-12
#	define	ERR_GP_ACCOUNT_HAS_LOCK			16									///机器已经处于锁定状态	2009-11-12
#	define	ERR_GP_UNLOCK_SUCCESS			17									///解除锁定成功 2009-11-12
#	define	ERR_GP_NO_LOCK					18									///机器根本就没有锁定，所以解锁失败 2009-11-12
#	define	ERR_GP_CODE_DISMATCH			19									///机器码不匹配，解锁失败。	2009-11-12
#	define ERR_GP_ACCOUNT_LOCKED			20									///本账号锁定了某台机器，登录失败 2009-11-12 zxj
#endif//ERR_GP_ERROR_UNKNOW
#	define ERR_GM_OPERATOR_SUCCESS			201									///< GM对玩家的操作成功
#	define ERR_GM_OPERATOR_FAILED			202									///< GM对玩家的操作失败
///-----------------------------------------------------------------------
/// 协议结构定义
///-----------------------------------------------------------------------

/// 1、下发奖励方案用结构
struct GM_AwardType_t
{
	struct PropMemo_t
	{
		int iPropID;			///< 道具编号
		int iCount;				///< 数量
	};
	int iSeriNo;				///< 方案序号
	int iGoldNum;				///< 金币数量，可为零
	PropMemo_t iPropAward[4];	///< 道具名称和数量，最多四种，可都为0
	INIT_STRUCT(GM_AwardType_t);
};

/// 2、下发指定玩家曾经被操作过的记录及在线状态
/// 用于上行，按玩家ID, 或登录名或昵称取操作历史
struct GM_GetOperationByPlayer_t
{
	union PlayerInfo_u
	{
		int UserID;				///< 玩家ID号
		TCHAR szName[32];		///< 登录名，或昵称
	};
	BYTE bType;					///< 玩家ID，登录名还是昵称，0表示ID号，1登录名，2表示昵称
	PlayerInfo_u playerInfo;	///< 玩家信息，视bType而定，到底是ID，还是登录名，还是昵称
	INIT_STRUCT(GM_GetOperationByPlayer_t);
};
/// 用于下行，玩家名字的对应关系，及该玩家当前的状态，如无相应记录，则为0值或""空字符串
struct GM_OperatedPlayerInfo_t
{
	int iUserID;				///< 用户ID
	TCHAR szUserName[32];		///< 用户登录名
	TCHAR szNickName[32];		///< 用户昵称
	/// 当前登录状态
	int ZID;					///< 当前登录的ZServer编号，0表示无
	int WID;					///< 当前登录的WServer编号，即房间号，0表示无
	/// GM处罚记录信息，若无记录，则各项值为0
	int iGMID;					///< 进行该项操作的GM编号
	int iOperation;				///< 对该用户的操作
	int dtDate;					///< 操作时间
	int iPeriod;				///< 持续时间，单位分
	TCHAR szReason[64];			///< 作本次操作的原因
	INIT_STRUCT(GM_OperatedPlayerInfo_t);
};

/// 3、上传对玩家的操作
struct GM_OperateByID_t
{
	int iOperation;		///< 操作类型编号，由此来确定惩罚还是奖励
	int iUserID;		///< 对象玩家ID
	int iGMID;			///< 管理员ID
	int iWorkMinutes;	///< 用于惩罚时，表示作用时间；用于奖励时，填写方案序号
	TCHAR szReason[64];	///< 原因
	TCHAR szNickName[50]; ///< 用户昵称
	INIT_STRUCT(GM_OperateByID_t);
};

/// 4、与某特定玩家发言，及广播消息，公告
struct GM_Broadcast_t
{
	int iUserID;		///< 玩家ID，如为零，表示广播
	int iGMID;			///< GMID，如广播消息，可为零，指定了目标玩家ID，则不能为零，否则目标玩家不知道是哪个GM发的消息
	TCHAR szMsg[256];	///< 需要广播或发送的消息
	INIT_STRUCT(GM_Broadcast_t);
};

/// 5、用于Z服务器，在GM自己登录时，主动下发当前已经登录的玩家列表，需要在客户端保存用户ID与昵称的对应关系
/// 6、道具ID与名称对应关系
struct GM_GetPropInfo_t
{
	int iPropID;			///< 道具ID
	TCHAR szPropName[32];	///< 道具名称
	INIT_STRUCT(GM_GetPropInfo_t);
};
/// 7、GM登录协议，结构体与普通玩家登录有所差异

/// GM登录（帐号）结构
struct GM_MSG_S_LogonByName_t
{
	UINT								uRoomVer;							///< 大厅版本
	char								szName[61];							///< 登录名字
	char								szMD5Pass[50];						///< 登录密码
	char								szMathineCode[32];					///< 本机机器码 zxj 2009-11-10 锁定机器
	INIT_STRUCT(GM_MSG_S_LogonByName_t);
};

///打印日志的通知
struct GM_MSG_PrintLog_t
{
	UINT uRoomID;	///< 房间ID
	INIT_STRUCT(GM_MSG_PrintLog_t);
};

///=======================================================================
/// 数据库访问协议定义
///=======================================================================
#define GM_DT_MESSAGE_START	220	///< 起始
#define GM_DT_MESSAGE_END	225	///< 结束

/// 1、获取奖励方案
/// 2、获取历史操作过的玩家列表
/// 3、写入对玩家的操作到记录表中
/// 4、获取道具ID与名称对应关系
/// 5、GM帐号登录信息

///-----------------------------------------------------------------------
/// 与GM相关的数据库操作指令宏定义安排在200以后
/// 参见GameLogonDT.h, GameRoomLogonDT.h文件

#define DTK_GET_AWARD_TYPE_GM			221			///< 获取奖励方案
#define DTK_GET_OPEATED_USER_GM			222			///< 获取指定的玩家的被操作过的信息及当前状态
#define DTK_PUT_OPEATION_USER_GM		223			///< 写入对玩家的操作
#define DTK_GET_PROP_INFO_GM			224			///< 获取道具ID与名称对应关系列表
#define DTK_LOGON_BY_NAME_GM			225			///< GM采用GM工具登录


/// 数据库存储过程返回值定义
//-- 0 登录成功
//-- 1 用户不存在
//-- 2 密码不正确
//-- 3 没有GM权限
//-- 4 机器码不正确
//-- 5 已经有另一个同名GM登录中
#define DTR_GM_LOGON_SUCCESS		0
#define DTR_GM_USER_NOT_FOUND		1
#define DTR_GM_PASSWORD_ERROR		2
#define DTR_GM_HAS_NOT_RIGHT		3
#define DTR_GM_MACHINECODE_ERR		4
#define DTR_GM_ALREADY_LOGON		5
#define DTR_GM_OPERATE_SUCCESS		6

/// 取数据集指示值定义
#	define DTR_GM_GET_RECORD_OK		0
#	define DTR_GM_GET_RECORD_END	1
///--------------------------------------
/// GM获取奖励方案用
///--------------------------------------

/// 1、返回奖励方案用结构，如有多条，则在数据集里循环，发送多次返回结果
struct GM_DT_O_AwardType_t
{
	DataBaseResultLine					ResultHead;							///< 结果数据包头
	GM_AwardType_t						awardType;							///< 奖励类型
	INIT_STRUCT(GM_DT_O_AwardType_t);
};
///--------------------------------------
/// 2、GM获取指定玩家的名字信息，当前在线状态，历史操作记录
///--------------------------------------
/// 输入参数
struct GM_DT_I_GetOperatedUser_t
{
	DataBaseLineHead					DataBaseHead;						///< 数据包头
	GM_GetOperationByPlayer_t			requestPlayer;						///< 请求的玩家信息
};
/// 输出用
struct GM_DT_O_OperatedUser_t
{
	DataBaseResultLine					ResultHead;							///< 结果数据包头
	GM_OperatedPlayerInfo_t				operatedPlayer;						///< 对玩家的操作记录
	INIT_STRUCT(GM_DT_O_OperatedUser_t);
};
///--------------------------------------
/// 3、写入对玩家的操作记录
///--------------------------------------
struct GM_DT_I_OperateUser_t
{
	DataBaseLineHead					DataBaseHead;						///< 数据包头
	GM_OperateByID_t					operation;							///< 对玩家的操作参数
	INIT_STRUCT(GM_DT_I_OperateUser_t);
};

///--------------------------------------
/// 5、道具ID、名称对应关系用
///--------------------------------------
struct GM_DT_O_GetPropInfoResult_t
{
	DataBaseResultLine					ResultHead;							///< 结果数据包头
	GM_GetPropInfo_t					propInfo;							///< 道具信息
};
///--------------------------------------
/// 6、GM登录用
///--------------------------------------

/// GM登录服务器登录信息
struct GM_DT_I_LogonByName_t
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	GM_MSG_S_LogonByName_t				logonInfo;							//登录信息
	char								szAccessIP[16];						//登录 IP
	INIT_STRUCT(GM_DT_I_LogonByName_t);
};
///GM大厅登陆返回数据包
struct GM_MSG_O_LogonResult_t
{
	UINT							dwUserID;							///< 用户 ID 
	int								iMasterRight;						///< 管理权限
	ULONG								dwLastLogonIP;						///< 上次登陆 IP
	ULONG								dwNowLogonIP;						///< 现在登陆 IP
	char								szName[61];							///< 用户登录名
	char								szMD5Pass[50];						///< 用户密码
	INIT_STRUCT(GM_MSG_O_LogonResult_t);
};
/// GM登录数据库返回结果
struct GM_DT_O_LogonResult_t
{
	DataBaseResultLine					ResultHead;							///< 结果数据包头
	GM_MSG_O_LogonResult_t				logonResult;						///< 登录结果返回数据结构
	INIT_STRUCT(GM_DT_O_LogonResult_t);
};
/// 


#endif//__GM_MESSAGE_DEFINE_H
