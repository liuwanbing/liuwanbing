#pragma once

#ifndef GAME_ROOM_LOGON_DT_HEAD_FILE
#define GAME_ROOM_LOGON_DT_HEAD_FILE

#include "../common/ComStruct.h"
#include "../common/GameRoomMessage.h"
//2013-01-28 wangzhitu
#include "../common/comstruct.h"

//包含道俱处理头文件
//#include "itemdatemessage.h"
/********************************************************************************************/

//	数据库通信数据包定义 

/********************************************************************************************/

//登陆服务器登陆信息
struct DL_GR_I_LogonByIDStruct
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	MSG_GR_S_RoomLogon					LogonInfo;							//登陆信息
	ULONG								uAccessIP;							//登陆 IP
};

//wushuqun 2009.5.6
//清除断线后的标志
struct DL_GR_I_ClearOnlineFlag
{
	DataBaseLineHead					DataBaseHead;
	long								lUserID;
};


//用户断线重连写W记录 2009-4-28
struct DL_GR_I_UserRecome
{
	DataBaseLineHead		DataBaseHead;
	long int				lUserID;
};

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
struct DL_GR_I_GetNickNameOnID
{
    DataBaseLineHead					DataBaseHead;
    MSG_GR_S_GetNickNameOnID_t          stGetNickNameOnID;
};



//登陆服务器登陆信息
struct DL_GR_I_RegisterLogon
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	char								szAccessIP[16];						//用户 IP
};

/// 用来传入批量更新用户数据库参数结构
struct UpdateUserInfo_t
{
	long int							dwUserID;							//用户 ID
	long int							dwOnLineTimeCount;					//在线时长
	__int64								dwChangePoint;						//更改分数
	__int64								dwChangeMoney;						//更改金币
	__int64								dwChangeTaxCom;						//更改税收
	UINT								uWinCount;							//胜局数目
	UINT								uLostCount;							//输局数目
	UINT								uCutCount;							//强退数目
	UINT								uMidCount;							//和局数目
	bool                                bISAutoSendMoney;                   //是否自动赠送金币 //2013-01-28
	//UINT								uContestRank;						//比赛排名
	UpdateUserInfo_t()
	{
		ZeroMemory(this, sizeof(UpdateUserInfo_t));
	}
};
/// 批量更改用户分数信息用结构
/// 为了改进数据库存储过程调用效率，减少不必要的调用。采用更高效的办法来完成用户数据的记录
/// 数据可变，不再是定长的结构，内容采用指针的形式来传递
struct DL_GR_I_AllUserData_t
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	int									nUserCount;							///< 有几个玩家的数据
	int									nPlayTime;			///< 游戏时间
	int									iRoundPlayTime;		///< 当局的时长
	UpdateUserInfo_t					*pUpdateUserInfo;					///< 玩家数据指针，由调用者保证数据的合法性
	DL_GR_I_AllUserData_t()
	{
		ZeroMemory(this, sizeof(DL_GR_I_AllUserData_t));
	}
};


struct DL_GR_I_UserContestData
{
	DataBaseLineHead					DataBaseHead;
	int iUserID;
	int iContestID;
	int	iLowChip;
	int iCheckedScore;
	int ChangeScore;
	int ChangePoint;
	int iDeskNo;
	int iSitNo;
	int iRoomID;
	int	iGameID;
	long int iMatchID;
};

struct DL_GR_I_UserContestRank
{
	DataBaseLineHead					DataBaseHead;
	int* pUserIDs;						//用户ID集
	int iIDNums;						//ID集的数量
};

struct DL_GR_O_UserContestRank
{
	DataBaseResultLine					ResultHead;							//数据包头
	MSG_GR_ContestChange				_data;
};

struct DL_GR_I_ContestReturnFee
{
	DataBaseLineHead					DataBaseHead;
	int									iUserID;
	int									iContestID;
	int									iRoomID;
	int									iGameID;
};

//更改用户分数信息
struct DL_GR_I_UserLeft
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	long int							uMaster;							//管理员等级
	long int							dwAccessLogID;						//记录 ID
	long int							dwPlayTimeCount;					//游戏时间
	long int							dwOnLineTimeCount;					//在线时长
	int									dwScrPoint;							//原来分数
	int									dwChangePoint;						//更改分数
	int									dwChangeMoney;						//更改金币
	int									dwChangeTaxCom;						//更改税收
	int									dwResultPoint;						//结果分数
	UINT								uWinCount;							//胜局数目
	UINT								uLostCount;							//输局数目
	UINT								uCutCount;							//强退数目
	UINT								uMidCount;							//和局数目
	ULONG								uAccessIP;							//访问地址
};

struct DL_GR_I_ContestBegin
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	UINT								uNameID;							//游戏ID
	UINT								uRoomID;							//房间ID
	int									iContestID;							//比赛标志
};

struct DL_GR_O_ContestBegin
{
	DataBaseResultLine					ResultHead;						//数据包头
	UINT								uMatchID;						//比赛ID
	int*								pUserIDs;						//参赛选手ID集合
	int									iUserNum;						//参赛选手数
};


struct DL_GR_O_ContestGameOver
{
	DataBaseResultLine					ResultHead;
	MSG_GR_ContestAward					oAward;
};

struct DL_GR_I_ContestApply
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							iUserID;							//用户ID
	int									iTypeID;							//操作类型：0-报名，1-退赛
};

struct DL_GR_O_ContestApply
{
	DataBaseResultLine					ResultHead;						//数据包头
	long int							iUserID;						//用户ID
	int									iApplyResult;					//数据库操作返回结果
	int									iContestBegin;					//0-比赛还未开始，1-比赛开始
};

//更新服务器在线信息
struct DL_GR_I_UpdateServerInfo
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	UINT								uNameID;							//游戏 ID
	UINT								uRoomID;							//房间 ID
	UINT								uOnLineUserCount;					//在线人数
};

struct DL_GR_I_SendMonet_By_Times         //某种条件下，按次数赠送玩家游戏币
{
	DataBaseLineHead					DataBaseHead;						//数据包头  
	long int							dwUserID;							//用户 ID
	__int64                             i64SrcGameMoney;                     //原来的金额                    需要送进来前添加
	__int64                             i64SrcMoneyInBank;                    //银行里的金币
	BYTE                                dwGameFinishCount;                   //送金币的次数
	//int                                 dwSendGameMoney;                    //赠送的金额  
	//int                                 dwSendTimes;                        //今天已经赠送的次数
	//int                                 dwAllSendTImes;                     //今天总共的赠送次数	
	//int                                 dwSendFailedCause;                   //赠送失败原因 0成功 
		                                                                    //1次数已满2未达到赠送标准
		                                                                                 
};
struct DL_GR_O_SendMonet_By_Times         //某种条件下，按次数赠送玩家游戏币
{
	DataBaseResultLine					ResultHead;							//数据包头
	long int							dwUserID;							//用户 ID
	__int64                             i64SrcGameMoney;                     //原来的金额                    需要送进来前添加
	__int64                             i64SrcMoneyInBank;                    //银行里的金币
	int                                 dwSendGameMoney;                    //赠送的金额  
	int                                 dwSendTimes;                        //今天已经赠送的次数
	int                                 dwAllSendTImes;                     //今天总共的赠送次数	
	int                                 dwSendFailedCause;                   //赠送失败原因 0成功 
                                                                     //1次数已满2未达到赠送标准
};

//自动赠送添加的结构体--2013-01-28 wangzhitu
/*typedef struct tag_RECEIVEMONEY
{
	bool      bISAuto;	
	__int64	  i64Money;
	__int64	  i64MinMoney;
	int		  iCount;
	int		  iTotal;
	int		  iTime;   
	int       iResultCode; 
	struct tag_RECEIVEMONEY()
	{
		bISAuto = false;		
		i64Money = 0;
		i64MinMoney = 0;
		iCount = 0;
		iTotal = 0;
		iTime  = 0;
		iResultCode = 0;
	}

}RECEIVEMONEY;*/

// PengJiLin, 2011-4-16, 在线时长、局数送金币功能
struct DL_GR_O_SendMoney_On_TC
{
    DataBaseResultLine					ResultHead;
    int                                 dwUserID;
    int                                 dwGetMoney;             // 获赠的金币数
    int                                 dwMoneyOnTimes;         // 设置多少时长赠送的金币
    int                                 dwMoneyOnCounts;        // 设置多少局赠送的金币
    int                                 dwTimesNeed;            // 下一次差多少时长
    int                                 dwCountsNeed;           // 下一次差多少局
	RECEIVEMONEY                        strAutoSendMoney;       //自动赠送 --2013-01-28 wangzhitu
};


//游戏记录请求
struct DL_GR_I_GameRecord
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	UINT								uRoomID;							//房间 ID
	BYTE								bDeskIndex;							//游戏桌号
	long int							dwTax;								//每人实缴税收
	long int							dwUserID[MAX_PEOPLE];				//用户 ID
	__int64								dwChangePoint[MAX_PEOPLE];			//用户改变分数
	__int64								dwChangeMoney[MAX_PEOPLE];			//用户改变金币
	__int64								dwScrPoint[MAX_PEOPLE];				//用户起步分数
	__int64								i64ScrMoney[MAX_PEOPLE];				//用户当前金币
	__int64								dwTaxCom[MAX_PEOPLE];				//每人税收
	long int							dwBeginTime;						//开始时间
};

//权限更改请求
struct DL_GR_I_PowerSet
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	int									dwGMID;								//管理员ID
	int							        dwUserID;							//用户 ID
	int									dwGamePower;						//游戏权限
	int									dwMasterPower;						//管理权限
	long int							uLockTime;							//锁定时间
	BYTE								bBindKind;						   //锁定范围
};

//GM警告用户纪录
struct DL_GR_I_Warn_User_Record
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwGMID;								//管理员ID
	long int							dwTargetID;							//目标用户
	char								szWarnMsg[201];						//警告消息
};

//wushuqun 2009.6.6
//即时封桌功能
struct DL_GR_I_ALone_Data
{
	DataBaseLineHead					DataBaseHead;						//数据包头

	long int							dwUserID;							//用户 ID
	BYTE								bDeskIndex;							//桌子号码
	BYTE								bDeskStation;						//桌子位置
	//房间ID号
	UINT                                uRoomID;

	int                                 bDeskPeople;                        //桌子人数,人数为负时表解封人数
};
///////

//GM踢人纪录体
struct DL_GR_I_Kick_User_Record
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	char						        szTargetUserName[61];				//目标用户登录名
	long int							GMID;								//管理员ID
};
//呼叫GM请求
struct DL_GR_I_CallGM
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	char								szName[61];							//登录名
	UINT								uDeskNO;							//桌号
	UINT								uDeskStation;						//凳子号
	BYTE								bCallReason;						//呼叫类型
	char								szMsg[202];							//呼叫留言
};

//清理用户请求 
struct DL_GR_I_UnRegisterUser
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	//CHAR								szMD5Pass[50];						//加密密码
};

//清理用户请求 
struct DL_GR_I_ContestUserLeft
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	int									iUserID;							//用户 ID
	int									iContestID;
	//CHAR								szMD5Pass[50];						//加密密码
};

//wushuqun 2009.5.22
//更新写入WLoginRecord 记录时机结构
struct DL_GR_I_WriteWLoginRecord
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	UINT								uRoomID;							//房间 ID
};
///////


//打开经验盒 
struct DL_GR_I_OpenPoint
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
};

//打开经验盒结果 
struct DL_GR_O_OpenPointResult
{
	DataBaseResultLine					ResultHead;							//数据包头
	long int							dwUserID;							//用户 ID
	DWORD								dwPoint;							//经验值
};

//取出经验
struct DL_GR_I_GetPoint
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	DWORD								dwPoint;							//经验值
	DWORD								dwsrcPoint;							//原来经验值
};

//取出经验
struct DL_GR_O_GetPointResult
{
	DataBaseResultLine					ResultHead;							//数据包头
	long int							dwUserID;							//用户 ID
	DWORD								dwPoint;							//经验值
};

//打开钱柜
struct DL_GR_O_OpenWalletResult
{
	DataBaseResultLine					ResultHead;							//数据包头
	struct MSG_GR_UserBank				stUserBank;
};
///存钱请求
struct DL_GR_I_CheckOutMoney
{
	DataBaseLineHead					DataBaseHead;							//数据包头
	long int							dwUserID;							//用户 ID
	__int64								i64Count;							//金额
};
///带密码取钱请求
struct DL_GR_I_CheckOutMoneyWithPwd_t
{
	DataBaseLineHead					DataBaseHead;							//数据包头
	long int							dwUserID;							//用户 ID
	__int64								i64Count;							//金额
	TCHAR								szMD5Pass[50];						//加密密码
};
//取钱结果
struct DL_GR_O_CheckOutMoney
{
	DataBaseResultLine					ResultHead;							//数据包头
	long int							dwUserID;							//用户 ID
	__int64								i64CheckOut;						//取出金额
	__int64								i64MoneyInBank;						//剩余
	__int64								i64MoneyInRoom;
};
//修改密码请求
struct DL_GR_I_ChangePasswd_t
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	struct	MSG_GR_S_ChangePasswd_t		stChangePasswd;						//修改密码数据包
};
//修改密码结果
struct DL_GR_O_ChangePasswd_t
{
	DataBaseResultLine					ResultHead;							//数据包头
	int									nResult;							//修改密码结果，0表示成功，1表示失败
};
//转帐请求
struct DL_GR_I_TransferMoney
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	struct	MSG_GR_S_TransferMoney		stTransfer;							//转帐数据包
};
//转帐结果
struct DL_GR_O_TransferMoney
{
	DataBaseResultLine					ResultHead;							//数据包头
	struct	MSG_GR_S_TransferMoney		stTransfer;							//转帐数据包
};
///请求转帐记录
struct DL_GR_I_TransferRecord_t
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	struct	MSG_GR_S_TransferRecord_t	stTransfer;							//转帐数据包
};
///请求转帐记录结果
struct DL_GR_O_TransferRecord_t
{
	DataBaseResultLine					ResultHead;							//数据包头
	struct	MSG_GR_R_TransferRecord_t	transferRecord;						//转帐记录数据包
};

//即时更新
struct DL_GR_Update_InstantMoney
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	int									dwChangeMoney;						//更改金币
	int									dwChangePoint;						//更改分数
	BYTE								bCutValue;							//是否为断线
};
//即时更新玩家魅力值
struct DL_GR_I_AddCharm
{
	DataBaseLineHead					DataBaseHead;						//数据包头
	long int							dwUserID;							//用户 ID
	int									iCharmValue;								//魅力值
};

/// 记录房间密码修改(金葫芦2代)
struct DL_GR_RoomPWChange
{
	///< 数据包头
	DataBaseLineHead					DataBaseHead;
	///< 用户ID
	long int                            dwUserID;
	///< 房间ID
	long int                            dwRoomID; 
};

// PengJiLin, 2010-9-10, 踢人卡、防踢卡道具
struct DL_GR_I_UseKickProp
{
    DataBaseLineHead					DataBaseHead;
    long int                            dwUserID;
    int                                 iPropID;
};

// PengJiLin, 2010-9-13, 踢人卡踢人功能
struct DL_GR_I_NewKickUserProp
{
    DataBaseLineHead					DataBaseHead;
    long int                            dwUserID;
    long int                            dwDestID;
    UINT                                dwDestIndex;        // socket
    DWORD                               dwDestHandleID;     // socket
};

///////////////////////////////////////////////2008.3.5道俱




//好友
//struct DL_GR_I_FriendManage
//{
//	DataBaseLineHead					DataBaseHead;						// 数据包头
//	tagIM_Msg							Data;
//};

//好友管理
//struct DL_GR_O_FriendManage
//{
//	DataBaseResultLine					ResultHead;							//数据包头
//	tagIM_Msg							Data;
//};

////好友
//struct DL_GR_I_BufferIMMessage
//{
//	DataBaseLineHead					DataBaseHead;						// 数据包头
//	tagIM_Msg							Data;
//};

////好友
//struct DL_GR_O_BufferIMMessage
//{
//	DataBaseResultLine					ResultHead;							//数据包头
//	tagIM_Msg							Data;
//};

/********************************************************************************************/

//登陆服务器登陆信息
struct DL_GR_O_RegisterLogon
{
	DataBaseResultLine					ResultHead;							//数据包头
	UINT								uRoomID;							//房间 ID	
	char								szGameRoomName[61];					//房间名字
};

//用户登陆结果
struct DL_GR_O_LogonResult
{
	DataBaseResultLine					ResultHead;							//数据包头
	UserInfoStruct						pUserInfoStruct;						//用户信息
	ULONG								dwLastLogonIP;						//上次登陆 IP
	ULONG								dwNowLogonIP;						//本次登陆 IP
	long int							dwGamePower;						//用户权限
	long int							dwMasterPower;						//管理权限
	char								szMD5Pass[50];						//MD5 密码
	BYTE								bRegOnline;							//是否注册
	char								nickName[61];							//用户名
	RECEIVEMONEY                        strRecMoney;                        //非比赛场玩家金币不足自动赠送

	//wushuqun 2009.6.6
	//登录房间时即时获取虚拟玩家人数
	int									nVirtualUser;
	int									nPresentCoinNum;  ///< 赠送金币数量
};

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
struct DL_GR_O_GetNickNameOnIDResult
{
    DataBaseResultLine					ResultHead;
    long int                            iUserID;
    TCHAR                               szNickName[61];
};

//wushuqun 2009.6.5
//混战场游戏信息记录结构
struct DL_GR_O_BattleRecordResult
{
	DataBaseResultLine					ResultHead;							//数据包头


	UINT								uRoomID;							//房间 ID	,非BattleRoomID
	//记录结果
	UINT          uRecordResult;
};

// PengJiLin, 2010-9-10, 踢人卡、防踢卡道具
struct DL_GR_O_UseKickProp
{
    DataBaseResultLine					ResultHead;
    long int                            dwUserID;
    int                                 iPropID;
    int                                 iTotalTime;     // 总时间
};

// PengJiLin, 2010-9-13, 踢人卡踢人功能
struct DL_GR_O_UseNewKickUserProp
{
    DataBaseResultLine					ResultHead;
    long int                            dwUserID;
    long int                            dwDestID;
    UINT                                dwDestIndex;
    DWORD                               dwDestHandleID;
};


//登陆服务器登陆信息错误返回
struct DL_GR_O_LogonError
{
	DataBaseResultLine					ResultHead;							//数据包头
	UINT								uRoomID;							//房间 ID	
	char								szGameRoomName[61];					//房间名字
};




struct TpropChange_I
{
	DataBaseLineHead					DataBaseHead;
	TPropChange							info;
};

struct TpropChange_O
{
	DataBaseResultLine					ResultHead;
	TPropChange							info;
};
///
/********************************************************************************************/
//钱柜划账
#define DTK_GR_DEMAND_MONEY				32									//打开钱柜
#define DTK_GR_CHECKOUT_MONEY			33									//取出钱
#define DTK_GR_CHECKIN_MONEY			34									//存钱
#define DTK_GR_TRANSFER_MONEY			40									//转帐
#define DTK_GR_SENDMONEY_TIMES          41                                  //按次数送钱  yjj add for send money
#define DTK_GR_TRANSFER_RECORD			42									//转帐记录
#define DTK_GR_CHANGE_PASSWD			43									///< 修改密码
//管理权限纪录
#define DTK_GR_GM_KICK_USER_RECORD      30									//管理员踢人纪录
#define DTK_GR_GM_WARNING_USER_RECORD   31									//管理员发警告消息纪录

//管理权限处理
#define DTK_GR_GAME_POWER               25									//管理权限修改
#define DTK_GR_GAME_POWER_RESULT		26									//管理权限修改结果
#define DTK_GR_GAME_POWER_FAIL			27									//管理权限修改失败
#define DTK_GR_GAME_POWER_SUCCESS		28									//管理权限修改成功
#define DTK_GR_GAME_POWER_RELEASE		29									//解除管理权限限制成功

//房间动作处理标示
#define DTK_GR_CALL_GM					20									//呼叫GM处理
#define DTK_GR_CALL_GM_RESULT			21									//呼叫GM数据库处理结果
#define DTK_GR_CALL_GM_SUCCESS			22									//呼叫成功
#define DTK_GR_CALL_GM_FAIL				23									//呼叫失败
#define DTK_GR_CALL_GM_IN_ONE_MI		24									//呼叫在１分钟内超过１次

//经验盒操作
#define DTK_GR_OPEN_POINT				35									//打开经验盒
#define DTK_GR_GET_POINT				36									//取出经验
#define DTK_GR_OPEN_POINT_RESULT		37									//打出结果
#define DTK_GR_GET_POINT_RESULT			38									//取出结果
//#define DTK_GR_USER_OPT					39									//玩家 处理


#define DTK_GR_OPEN_ERROR				1									//打开失败
#define DTK_GR_GET_ERROR				2									//取出失败
#define DTK_GR_OPEN_SUC					3									//打开成功
#define DTK_GR_GET_SUC					4									//取出成功

//房间登陆处理标识
#define DTK_GR_LOGON_BY_ID				1									//通过 ID 登陆
#define DTK_GR_USER_LEFT				2									//用户离开大厅
#define DTK_GR_UPDATE_INFO				3									//更新在线信息
#define DTK_GR_RECORD_GAME				4									//记录游戏信息
#define DTK_GR_CHECK_CONNECT			5									//检测断线连接
#define DTK_GR_POWER_SET				6									//用户权限设置
#define DTK_GR_UNREGISTER_ONLINE		7									//取消注册用户
//#define DTK_GR_RE_WRITE_DATA			8									//重写数据库

#define DTK_GR_USER_CONTEST_LEFT		8									//用户离开包括


//wushuqun 2009.6.5
#define DTK_GR_BATTLEROOM_RES           9									//混战场记录信息

//房间登陆错误代码
#define DTR_GR_ERROR_UNKNOW				0									//未知错误
#define DTR_GR_DATABASE_CUT				1									//数据库断开
#define DTR_GR_LOGON_SUCCESS			10									//登陆成功
#define DTR_GR_USER_NO_FIND				11									//登陆名字错误
#define DTR_GR_USER_PASS_ERROR			12									//用户密码错误
#define DTR_GR_USER_VALIDATA			13									//用户帐号禁用
#define DTR_GR_USER_IP_LIMITED			14									//登陆 IP 禁止
#define DTR_GR_IP_NO_ORDER				15									//不是指定地址 
#define DTR_GR_ONLY_MEMBER				16									//会员游戏房间
#define DTR_GR_IN_OTHER_ROOM			17									//正在其他房间	
#define DTR_GR_MATCH_LOGON    		    18									//游戏比赛注册房间	
#define DTR_GR_OVER_TIMER				19									//免费期已到

#define DTR_GR_CONTEST_NOSIGNUP			23									//比赛未报名
#define DTR_GR_CONTEST_TIMEROUT			24									//比赛退出超时
#define DTR_GR_CONTEST_NOSTART			25									//比赛还没开始
#define DTR_GR_CONTEST_OVER				26									//比赛已经结束
#define DTR_GR_CONTEST_BEGUN			27									//比赛已经开始


// PengJiLin, 2010-8-23, 根据用于ID获取昵称 错误代码
#define DTR_GR_GETNICKNAME_SUCCESS      1           // 操作成功
#define DTR_GR_GETNICKNAME_NOTEXIST     2           // ID 不存在
#define DTR_GR_GETNICKNAME_ISNULL       3           // 昵称为空

//wushuqun 2009.6.5
//不在混战场活动时间内
#define DTR_GR_BATTLEROOM_TIMEOUT         20 

//wushuqun 2009.6.6
//管理员即时封桌功能
#define DTR_GR_ALONEDESK_INTIME           21
#define DTR_GR_UNALONEDESK_INTIME         22   //即时解除封桌
//////////////////////////////

//钱柜划账
#define DTR_GR_OPEN_WALLET_ERROR		40									//打开钱柜错误
#define DTR_GR_OPEN_WALLET_SUC			41									//打开钱柜成功
#define DTR_GR_CHECKOUT_MONEY_SUC		42									//成功取出钱到房间
#define DTR_GR_CHECKIN_MONEY_SUC		43									//成功取出钱到房间
#define DTR_GR_OPEN_WALLET_ERROR_PSW	44									//打开钱柜二级密码错误
#define DTR_GR_TRANSFER_MONEY_SUC		45									//成功转帐
#define DTR_GR_TRANSFER_MONEY_NO_DEST	46									//转帐目标不存在
#define DTR_GR_TRANSFER_TOTAL_NOT_ENOUGH	47								//银行存款太少，不允许转帐
#define DTR_GR_TRANSFER_TOO_LESS		48									//单笔转帐额太小
#define DTR_GR_TRANSFER_MULTIPLE		49									//单笔转帐额必须是某数的整数倍
#define DTR_GR_TRANSFER_NO_ENOUGH_MONEY	50									//银行存款不足本次转帐
#define DTR_GR_TRANSFER_TOO_MANY_TIME	51									///<当天转账的次数太多了

#define DTK_GR_FRIEND_MANAGE_SUC					53						//好友操作成功
#define DTR_GR_CHECKIN_MONEY_INV					54						//非法存钱异常
#define DTR_GR_CHECKOUT_MONEY_INV					55						//非法取钱异常


//写游戏数据
#define DTK_GR_UPDATE_GAME_RESULT					10						//游戏数据
#define DTK_GR_UPDATE_INSTANT_MONEY					11						//即时更新金币分数
#define DTK_GR_UPDATE_CHARM							12						//更新魅力值
#define DTK_GR_UPDATE_GAME_RESULT_ALLUSER			13						//一次性更新所有玩家的游戏数据

#define DTK_GR_UPDATE_CONTEST_RESULT				14						//一次性更新所有玩家的游戏数据
#define DTK_GR_GET_CONTEST_RESULT					15						//一次性获取所有玩家的游戏数据

#define DTK_GR_FRIEND_MANAGE						49						//好友操作消息
#define DTK_GR_BUFFER_IM_MESSAGE					50						//IM缓冲数据包


/********************************************************************************************/
#define DTK_GR_CLEAR_ONLINE_FLAG	   51									//清楚在线标志,2009-4-17添加
#define DTK_GR_USER_RECOME			   52									//用户断线重连写W记录 2009-4-28

//wushuqun 2009.5.22
//更新写入WLoginRecord 记录时机
#define DTK_GR_WRITE_WLOGINRECORD      53                       

/// 从游戏中打开银行界面Added by zxd 20091215
#define DTK_GR_DEMAND_MONEY_IN_GAME				54									///<从游戏中打开银行界面
#define DTK_GR_CHECKOUT_MONEY_INGAME			55									///<从游戏中取钱
#define DTK_GR_CHECKIN_MONEY_INGAME				56									///<从游戏中存钱
#define DTK_GR_TRANSFER_MONEY_INGAME			57									///<从游戏中转帐
#define DTK_GR_CHANGE_PASSWD_INGAME				58									///<从游戏中修改密码
#define DTK_GR_TRANSFER_RECORD_INGAME			59									///<从游戏中取转帐记录

#define DTK_GR_ROOM_PW_CHANGE                   60                                  ///< 记录房间密码修改

#define DTK_GR_GETNICKNAME_ONID                 70      // PengJiLin, 2010-8-23, 根据用户ID获取昵称

#define DTK_GR_USE_KICK_PROP                    71      // PengJiLin, 2010-9-10, 使用踢人卡道具
#define DTK_GR_USE_ANTI_KICK_PROP               72      // PengJiLin, 2010-9-10, 使用防踢卡道具

#define DTK_GR_USER_NEW_KICK_USER_PROP          73      // PengJiLin, 2010-9-13, 踢人卡踢人功能

#define DTK_GR_GETNICKNAME_ONID_INGAME          74      // PengJiLin, 2010-8-23, 根据用户ID获取昵称

// PengJiLin, 2011-4-15, 在线时长、局数送金币
#define DTK_GR_SEND_MONEY_ON_TIMES_COUNTS       75

// PengJiLin, 2010-9-10, 踢人卡，防踢卡使用 错误码
#define DTR_GR_USE_KICKPROP_SUCCESS             1       // 使用成功
#define DTR_GR_USE_KICKPROP_NULL                2       // 没有道具
#define DTR_GR_USE_KICKPROP_SET_SUCCESS         3       // 设置道具的时间成功
#define DTR_GR_USE_KICKPROP_SET_ERROR           4       // 设置道具的时间失败

// PengJiLin, 2010-9-13, 踢人卡踢人结果
#define DTR_GR_NEW_KICKUSER_SUCCESS             1       // 成功
#define DTR_GR_NEW_KICKUSER_NOTIME              2       // 自己的道具已过期，或者没有道具
#define DTR_GR_NEW_KICKUSER_HAD_ANTI            3       // 对方有防踢卡时间
#define DTR_GR_NEW_KICKUSER_HAD_VIP             4       // 对方有VIP时间


#define DTK_GR_PROP_CHANGE						76

#define DTK_GR_CONTEST_BEGIN					77		//比赛开始
#define DTK_GR_CONTEST_BEGIN_RESULT				78		//比赛开始
#define	DTK_GR_CONTEST_RETURN_FEE				79		//退回用户因为报名而未能参赛的报名费
#define DTK_GR_CONTEST_GAME_OVER				86		//比赛结束，获取奖励
#define DTK_GR_CONTEST_GAME_OVER_RESULT			87		//获取比赛结果
#define DTK_GR_CONTEST_APPLY					88		//用户报名比赛

#endif