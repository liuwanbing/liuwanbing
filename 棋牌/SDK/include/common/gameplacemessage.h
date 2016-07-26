#pragma once

#ifndef AFC_GAME_PLACE_MESSAGE_HEAD_FILE
#define AFC_GAME_PLACE_MESSAGE_HEAD_FILE

/********************************************************************************************/

///	网络通信数据包定义 

/********************************************************************************************/

///版本定义
#define GAME_PLACE_MAX_VER				4									///游戏大厅最新版本
#define GAME_PLACE_LESS_VER				4									///游戏大厅最低版本

///用户登陆（帐号）结构
struct MSG_GP_S_LogonByNameStruct
{
	UINT								uRoomVer;							///大厅版本
	char								szName[64];							///登陆名字
	char								TML_SN[128];						
	char								szMD5Pass[52];						///登陆密码
	char								szMathineCode[64];					///本机机器码 锁定机器
    char                                szCPUID[24];						//CPU的ID
    char                                szHardID[24];						//硬盘的ID
	char								szIDcardNo[64];						//证件号
	char								szMobileVCode[8];					//手机验证码
	int									gsqPs;	
	int									iUserID;							//用户ID登录，如果ID>0用ID登录
};

///锁定机器
struct MSG_GP_S_LockAccount
{
	UINT								dwUserID;				///用户ID
	UINT								dwCommanType;			///命令请求类型,1表示要求锁定，0表示要求解除锁定
	char								szMathineCode[64];		///机器码
	char								szMD5Pass[50];			///加密密码
	char								szMobileNo[50];			///手机号码  Added by Jianguankun 2012.5.16
};

///绑定手机号码
struct MSG_GP_S_BindMobile
{
	UINT								dwUserID;				///用户ID
	UINT								dwCommanType;			///命令请求类型,1表示要求绑定，0表示要求解除绑定
	char								szMobileNo[50];			///手机号码  Added by Jianguankun 2012.5.16
};

///测网速  yjj
struct MSG_GP_NETSignal
{
        UINT                                                                dwUserID;
        UINT                                                                dwIndex;
        UINT                                                                dwTimeFlag;
};

///服务器返回后加上游戏房间的时间标签，供游戏使用 yjj
struct MSG_GP_Game_NETSignal
{
	UINT                                                                dwUserID;
	UINT                                                                dwIndex;
	UINT                                                                dwTimeFlag;
	UINT                                                                dwSignalIndex;
};

///用户登陆（ACC）
struct MSG_GP_S_LogonByAccStruct
{
	UINT								uRoomVer;							///大厅版本
	int								dwAccID;							///ACC 号码
	char								szMD5Pass[50];						///登陆密码
};

///用户登陆（手机）
struct MSG_GP_S_LogonByMobileStruct
{
	UINT								uRoomVer;							///大厅版本
	int								dwMobile;							///手机号码
	char								szMD5Pass[50];						///登陆密码
};

///用户注册信息数据包
struct MSG_GP_S_RegisterStruct
{
	UINT								uRoomVer;							///大厅版本
	BYTE								bBoy;								///是否男性别
	UINT								bLogoID;							///用户头像 ID 号码
	char								szName[61];							///用户登录名
	char								nickName[32];						///用户昵称
	char								szRecommenderName[61];				///推荐人
	char								szMD5Pass[50];						///用户加密密码
	char								szRegSQL[210];						///用户注册附加信息的SQL
};

//用户设置头像
struct MSG_GP_S_SET_LOGO
{
	long int iUserID;
	UINT     iLogoID;
};

//Added by JianGuankun 20111108
struct MSG_GP_R_IDVerify
{
	long int							dwUserID;							///用户 ID 
	char								szMD5IDCardNo[36];						///证件号
};

//Added by JianGuankun 2012.05.17
struct MSG_GP_R_MobileVerify
{
	long int							dwUserID;							///用户 ID 
	char								szMobileNo[50];						///手机号
};
//统计登录人数
struct ONLINEUSERSCOUNT
{
	UINT								uiLogonPeopCount;					///登录人数
};

///大厅登陆返回数据包
struct MSG_GP_R_LogonResult
{
	long int							dwUserID;							///用户 ID 
	long int							dwGamePower;						///游戏权限
	long int							dwMasterPower;						///管理权限
	int									dwMobile;							///手机号码
	int									dwAccID;							///Acc 号码
	ULONG								dwLastLogonIP;						///上次登陆 IP
	ULONG								dwNowLogonIP;						///现在登陆 IP
	UINT								bLogoID;							///用户头像
	bool								bBoy;								///性别
	char								szName[61];							///用户登录名
	char								TML_SN[128];						///数字签名
	char								szMD5Pass[50];						///用户密码
	char								nickName[32];						///用户昵称
	__int64								i64Money;							///用户金币
	__int64								i64Bank;							///用户财富
	int									dwFascination;						///魅力
	
	//JianGK 20111107新用户资料
	char								szSignDescr[128];					///个性签名
	char								szRealName[20];						///真实姓名
	char								szIDCardNo[36];						///证件号
	char								szMobileNo[50];						///移动电话
	char								szQQNum[20];						///QQ号码
	char								szAdrNation[50];					///玩家的国藉
	char								szAdrProvince[50];					///玩家所在的省份
	char								szAdrCity[50];						///玩家所在的城市
	char								szZipCode[10];						///邮政编码
	char								szEmail[50];						///电子邮件
	char								szAddress[128];						///联系地址
	char								szSchooling[20];					///文化程度
	char								szHomePage[128];					///个人主页
	char								szTelNo[20];						///固定电话
	char								szMSNID[50];						//MSN帐号
	//end JianGK 20111107

	int									dwTimeIsMoney;						///上次登陆时长所换取的金币
	int									iVipTime;							///
	int									iDoublePointTime;					///双倍积分时间
	int									iProtectTime;						///护身符时间，保留
	bool								bLoginBulletin;						///是否有登录公告，Fred Huang,2008-05-20
	int									iLockMathine;						///当前帐号是否锁定了某台机器，1为锁定，0为未锁定 zxj 2009-11-13
	int									iBindMobile;						///当前帐号是否绑定手机号码，1为绑定，0为未绑定 jianguankun 2012.10.10

	int									iAddFriendType;						///是否允许任何人加为好友
};

//Added by JianGuankun 20111108,用户信息结构及修改信息的数据包定义
typedef MSG_GP_R_LogonResult MSG_GP_UserInfo;

struct MSG_GP_S_UserInfo
		: public MSG_GP_UserInfo
{
	MSG_GP_S_UserInfo(){::memset(this,0,sizeof(MSG_GP_S_UserInfo));}
};

struct MSG_GP_S_ChPassword
{
	long int dwUserID;		//用户ID 
	char szHardID[24];		//硬盘ID
	char szMD5OldPass[80];  //用户老密码
	char szMD5NewPass[80];  //用户新密码
	MSG_GP_S_ChPassword(){::memset(this,0,sizeof(MSG_GP_S_ChPassword));}
};

//end JianGuankun add

// add wyl  销售列表信息
struct MSG_GP_R_SellGameListResult
{
	char szGameName[61];						//游戏名称
	char szKindName[61];						//类型名称
	long NameID;								//游戏ID
	long KindID;								//类型ID
	long ParamID;								//游戏索引ID
};

///锁定（解除锁定）机器返回数据包	zxj 2009-11-16
struct MSG_GP_R_LockAndUnlock
{
	UINT	dwUserID;
	UINT	dwCommanType;			///请求命令类型
	UINT	dwCommanResult;			///请求的结果
};

///绑定/解除绑定手机返回数据包
struct MSG_GP_R_BindMobile
{
	UINT	dwUserID;
	UINT	dwCommanType;			///请求命令类型
	UINT	dwCommanResult;			///请求的结果
};

///获取游戏房间数据包
struct MSG_GP_SR_GetRoomStruct
{
	UINT								uKindID;							///类型 ID
	UINT								uNameID;							///名字 ID
};
///玩家列表操作
struct MSG_GP_User_Opt_Struct
{
	LONG								dwTargetID;							///操作对像
	LONG								dwUserID;							///操作id
	UINT								uType;								///操作类型
};

///获取游戏房间数据包
struct MSG_GP_SR_OnLineStruct
{
	///UINT								uType;							///类型 ID
	///UINT								uID;							///名字 ID
	UINT								uKindID;							///类型 ID
	UINT								uNameID;							///名字 ID
	UINT								uOnLineCount;						///在线人数
};

//{{ Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议
/// 获取在线GM列表
struct MSG_GP_R_GmList_t
{
	int _nCount;				///< 数量
	int _arGmID[8];				///< 在线GM的ID
	MSG_GP_R_GmList_t(){memset(this, 0, sizeof(MSG_GP_R_GmList_t));}
};

/// 普通玩家与在线GM聊天
struct MSG_GP_S_Talk2GM_t
{
	int iUserID;		///< 发送者ID
	int iGMID;			///< GMID
	TCHAR szMsg[256];	///< 欲发送的信息
	MSG_GP_S_Talk2GM_t(){memset(this, 0, sizeof(MSG_GP_S_Talk2GM_t));}
};
// Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议}}

struct MSG_PROP_RESULT
{
	DWORD dwResult;
};

// JianGuankun,2012-1-4,根据用户ID取昵称，或根据昵称取用户ID专用结构
struct MSG_GP_NickNameID_t
{
	long int _user_id;			//用户ID
	TCHAR	 _nickname[61];		//用户昵称
	BYTE	 _nType;			//0代表通过用户ID取用户昵称\
								//1代表通过用户昵称取用户ID
	BYTE	 _nUseful;			//用途标识	
	MSG_GP_NickNameID_t()
	{
		ZeroMemory(this, sizeof(MSG_GP_NickNameID_t));
	}
};
//End Add

struct MSG_GP_GetContestRoomID
{
	long int	iUserID;			//用户ID
	UINT		iGameID;			//游戏ID
	int			iContestID;			//比赛ID
	UINT		iUpPeople;			//比赛达到此人数后才开赛
};

struct DL_GetContestRoomID
{
	UINT				iGameID;		//游戏ID
	int					iRoomID;		//比赛房间ID
	int					iContestID;		//比赛ID
};

///登陆服务器登陆信息
struct DL_GP_RoomListPeoCountStruct
{
	UINT							uID;							///用户 ID 
	UINT							uOnLineCount;						///在线人数
	int								uVirtualUser;					///扩展机器人人数///20081211 , Fred Huang
};

//进入密码房间时向服务器提交房间密码 , add by lxl , 2010-10-08  
struct MSG_GP_CheckRoomPasswd 
{
	UINT uRoomID;			 //房间ID
	char szMD5PassWord[50];  //房间MD5密码
};


struct DL_CheckRoomPasswd
{
	bool bRet;		// 成功与否
	UINT uRoomID;	// 房间号
};

///领取金币
struct TReceiveMoney
{
	int			iUserID;
	__int64		i64Money;
	__int64		i64MinMoney;
	int			iCount;
	int			iTotal;
	int			iTime;
};

//兑换列表
struct TCharmExchange_Item
{
	int iID;
	int iPoint;
	int iPropID;
	int iPropCount;
};

struct TCharmExchange
{
	int iUserID;
	int iID;
};

struct TMailItem
{
	int iMsgID;
	int	iTargetUserID;
	int iIsSend;
	int iMsgType;
	__int64 i64TimeSend;
	char szSName[50];
	char szTitle[100];
	char szMsg[500];
};

//走马灯，输出
struct MSG_MarQueueInfo
{
	int  InformCount;												//消息数量
	CHAR buffer[8][120];											//走马灯信息
};

//手机短信验证码获取，Added by JianGuankun 2012.5.16
struct MSG_GP_SmsVCode
{
	char szName[61];		//用户名
	char szMobileNo[50];	//手机号码
	char szVCode[36];		//产生的验证码，使用MD5加密
	UINT nType;				//短信验证码类型，0为登陆验证\
											//1为锁机验证\
											//2为解锁机验证\
											//3为修改手机验证
											//4为绑定手机验证
											//5为解除绑定手机验证
};

//获取用户的财务消息，Added by JianGuankun 2012.7.12
struct TMSG_GP_BankFinanceInfo
{
	int		iUserID;			//用户ID
	__int64 i64BankMoney;		//用户银行的钱
	__int64 i64WalletMoney;		//用户取出的钱
	int     iLotteries;			//用户奖券数
};

/********************************************************************************************/

///	通信标识定义 

/********************************************************************************************/

///通信主标识
#define	MDM_GP_LOGON					100								///大厅登陆

///通信辅助标识
#define ASS_GP_LOGON_BY_NAME			1									///通过用户名字登陆
#define ASS_GP_LOGON_BY_ACC				2									///通过用户ACC 登陆
#define ASS_GP_LOGON_BY_MOBILE			3									///通过用户手机登陆
#define ASS_GP_LOGON_REG				4									///用户注册
#define ASS_GP_LOGON_SUCCESS			5									///登陆成功
#define ASS_GP_LOGON_ERROR				6									///登陆失败
#define ASS_GP_LOGON_ALLO_PART			7									///异地登陆
#define ASS_GP_LOGON_LOCK_VALID			8									///锁机验证

// PengJiLin, 2010-6-2, 如果是第三方启动，发此消息通知服务端。(鸥朴)
#define ASS_GP_LOGON_BY_SOFTWARE        10

#define ASS_GP_LOGON_MOBILE_VALID		11									///手机验证

///通信主标识
#define	MDM_GP_LIST						101								///游戏列表

///通信辅助标识
#define ASS_GP_LIST_KIND				1									///获取游戏类型列表
#define ASS_GP_LIST_NAME				2									///获取游戏名字列表
#define ASS_GP_LIST_ROOM				3									///获取游戏房间列表
#define ASS_GP_LIST_COUNT				4									///获取游戏人数列表
#define ASS_GP_ROOM_LIST_PEOCOUNT		5									///获取游戏人数列表
#define ASS_GP_ROOM_PASSWORD			6									//发送房间密码 Add by lxl 2010-10-08，试图进入密码房间时发送此消息
#define	ASS_GP_GET_SELLGAMELIST			7									//获取游戏销售列表

///通信主标识
#define	MDM_GP_REQURE_GAME_PARA					(MDM_GP_LIST+1	)		///请求游戏全局参数

///通信主标识
#define	MDM_GP_MESSAGE					103								///消息接发

#define ASS_GP_GETMARQUEEUINFORM		80								//获取走马灯信息辅助ID

#define	MDM_GP_SYSMSG					104								///系统消息

#define	MDM_GP_USER_OPT					105								///大厅增加好友
#define ASS_UPDATE_FRIEND				1									///更新好友
#define ASS_UPDATE_EMENY				2									///更新坏人

///锁定机器命令主ID zxj 2009-11-12
#define MDM_GP_LOCK_ACCOUNT				106									
#define ASS_LOCK_ACCOUNT				1


//领取金币
#define MDM_GP_RECEIVEMONEY					108
#define ASS_RECEIVE						1		//成功
#define ASS_RECEIVE_PASS				2		//身上的钱超过
#define ASS_RECEIVE_OVER				3		//已经全部领取完毕
#define ASS_RECEIVE_TIMELESS			4		//时间还没到
#define ASS_RECEIVE_ERROR				5		//领取出错


///魅力兑换
#define MDM_GP_CHARMEXCHANGE			109
#define ASS_GETLIST						1
#define ASS_EXCHANGE					2

///通信辅助标识
#define ASS_GP_NEWS_SYSMSG				1								///新闻和系统消息
#define ASS_GP_DUDU						2									///小喇叭

//{{ Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议
/// 通信主标识
#ifndef MDM_GP_PLAYER_2_GM
#define MDM_GP_PLAYER_2_GM				107								/// 普通玩家与GM之间的通信协议
#	define ASS_GP_GET_GM_LIST			1									///<获取当前在线GM列表
#	define ASS_GP_TALK_2_GM				2									///<向指定编号的GM发送消息
#	define ASS_GP_MSG_FROM_GM			3									///<接收从GM处发来的消息
#	define ASS_GR_MSG_BE_KICKED			4									///<被GM踢出房间
#	define ASS_GR_MSG_BE_BANDID			5									///<被GM封号
#	define ASS_GR_MSG_BE_AWARD			6									///<被GM奖励
#	define ASS_GR_MSG_PRINTLOG			7									///<GM打印日志
#endif//MDM_GP_PLAYER_2_GM
// Added by zxd 20100805 增加GM功能，添加普通玩家与GM间的通讯协议}}



//{{ Added by JianGuankun 20111108 添加用户资料管理通讯协议
#define MDM_GP_USERINFO	 				115
#define	ASS_GP_USERINFO_UPDATE_BASE		1		//用户更新基本信息
#define ASS_GP_USERINFO_UPDATE_DETAIL	2		//用户更新详细信息
#define ASS_GP_USERINFO_UPDATE_PWD		3		//用户修改密码
#define ASS_GP_USERINFO_ACCEPT			5		//服务端已接受
#define ASS_GP_USERINFO_NOTACCEPT		6		//服务端未能接受
#define ASS_GP_USERINFO_NICKNAMEID		10		//根据玩家昵称找ID或ID找昵称
//Added by JianGuankun 20111108}}

//{{ Added by JianGuankun 20111108 添加用户资料管理通讯协议
#define MDM_GP_MAIL	 				116
#define	ASS_GP_MAILINFO				1			//用户更新基本信息

//{{ Added by JianGuankun 2012.5.16 添加手机短讯协议
#define MDM_GP_SMS					117
#define ASS_GP_SMS_VCODE			1			//手机验证码短信
//End Added by JianGuankun}}

#define MDM_GP_LASTLOINGGAME		118
#define ASS_GP_LASTLOINGGAME		1

//=======================================分布式时，Z与B的通讯协议 rongqf20120425
#define MDM_ZD_PACKAGE					123
#define		ASS_ZD_CONNECT					1
#define		ASS_ZD_LOGON					2
#define			HDC_ZD_CONNECT				1
#define			HDC_ZD_DISCONNECT			2
#define			HDC_ZD_KICK					3
#define		ASS_ZD_BATCH					3
#define		ASS_ZD_SINGLE					4
#define			HDC_ZD_FAIL					2

//{{ Added by rende 20120920 统计登录人数
#define MDM_GP_LOGONUSERS	 				119
#define	ASS_GP_LOGONUSERS_COUNT					1

//{{ Added by JianGuankun 2012.10.10 绑定手机
#define MDM_GP_BIND_MOBILE	 				112
#define	ASS_GP_BIND_MOBILE					1

#define MDM_GP_SET_LOGO						113
#define ASS_GP_SET_LOGO						1


struct TZServerInfo
{
	int iZid;
	int	iServerPort;
};

struct MSG_ZDistriInfo
{
	DWORD	dwUserID;
	int		nZID;
};
//=======================================分布式时，Z与B的通讯协议 rongqf20120425

/********************************************************************************************/

///错误代码
#define ERR_GP_ERROR_UNKNOW				0									///未知错误
#define ERR_GP_LOGON_SUCCESS			1									///登陆成功
#define ERR_GP_USER_NO_FIND				2									///登陆名字错误
#define ERR_GP_USER_PASS_ERROR			3									///用户密码错误
#define ERR_GP_USER_VALIDATA			4									///用户帐号禁用
#define ERR_GP_USER_IP_LIMITED			5									///登陆 IP 禁止
#define ERR_GP_USER_EXIST				6									///用户已经存在
#define ERR_GP_PASS_LIMITED				7									///密码禁止效验
#define ERR_GP_IP_NO_ORDER				8									///不是指定地址 
#define ERR_GP_LIST_PART				9									///部分游戏列表
#define ERR_GP_LIST_FINISH				10									///全部游戏列表
#define ERR_GP_USER_LOGON				11									///此帐号已经登录
#define ERR_GP_USERNICK_EXIST			12									///此昵称已经存在
#define ERR_GP_USER_BAD					13									///未法字符
#define ERR_GP_IP_FULL					14									///IP已满
#define ERR_GP_LOCK_SUCCESS				15									///锁定机器成功	2009-11-12
#define	ERR_GP_ACCOUNT_HAS_LOCK			16									///机器已经处于锁定状态	2009-11-12
#define	ERR_GP_UNLOCK_SUCCESS			17									///解除锁定成功 2009-11-12
#define	ERR_GP_NO_LOCK					18									///机器根本就没有锁定，所以解锁失败 2009-11-12
#define	ERR_GP_CODE_DISMATCH			19									///机器码不匹配，解锁失败。	2009-11-12
#define ERR_GP_ACCOUNT_LOCKED			20									///本账号锁定了某台机器，登录失败 2009-11-12 zxj

#define ERR_GP_MATHINE_LOCKED			21

// PengJiLin, 2010-6-7, 第三方认证返回的错误码。鸥朴
#define ERR_GP_USER_NOT_EXIST           30      // 用户不存在
#define ERR_GP_USER_OVERDATE            31      // 用户已过期
#define ERR_GP_DATABASE_FUNC_ERROR      32      // 数据库操作失败
#define ERR_GP_DATABASE_RETURN_ERROR    33      // 数据库返回执行错误结果

/********************************************************************************************/
///异常代码

//JianGuankun 20111110
#define EXP_GP_ALLO_PARTY				50								///本账号异地登陆

#define MDM_GP_GET_CONTEST_ROOMID		51		//比赛房间，用户双击列表时向服务器索要房间号
#define ERR_GP_GET_CONTEST_ROOMID		1
/********************************************************************************************/

#define MDM_GP_NETSIGNAL                110                                ///网络信号消息	

#define MDM_GP_NOTICE_MSG				111									////提示消息  add by lxl 2010-11-10

static CString ERR_RP_STRING[21] = {"未知错误", "登录成功", "登录用户名错误", "用户密码错误", "用户帐号被禁", 
"登陆IP被禁", "用户已经存在", "密码禁止效验", "不是指定地址", "部分游戏列表", 
"全部游戏列表", "此帐号已经登录", "此昵称已经存在", "内含有非法字符", "同一IP每天只能注册20个帐号", 
"锁定机器成功", "机器已经处于锁定状态", "解除锁定成功", "机器没有锁定，解锁失败", "机器码不匹配，解锁失败", 
"本账号锁定了某台机器，登录失败"};

/********************************************************************************************/

///Center Server///
///通信主标识
#define	MDM_GP_REQURE_GAME_PARA					(MDM_GP_LIST+1	)		///请求游戏全局参数



#include"CenterServerhead.h"


///

#endif
