#pragma once

#ifndef AFC_GAME_ROOM_MESSAGE_HEAD_FILE
#define AFC_GAME_ROOM_MESSAGE_HEAD_FILE

/********************************************************************************************/

#include "BaseMessage.h"
#include "ComStruct.h"
#include <vector>
using namespace std;
///游戏房间登陆
struct MSG_GR_S_RoomLogon
{
	UINT								uNameID;							///名字 ID
	LONG								dwUserID;							///用户 ID
	UINT								uRoomVer;							///大厅版本
	UINT								uGameVer;							///游戏版本
	CHAR								szMD5Pass[50];						///加密密码
};

///游戏房间登陆
struct MSG_GR_R_LogonResult
{
	LONG								dwGamePower;						///用户权限
	LONG								dwMasterPower;						///管理权限
	LONG								dwRoomRule;							///设置规则
	UINT								uLessPoint;							///最少经验值
	UINT								uMaxPoint;							///最多经验值
	UserInfoStruct						pUserInfoStruct;						///用户信息
	RECEIVEMONEY                        strRecMoney;                        //非比赛场玩家金币不足自动赠送

	///wushuqun 2009.6.6
	///登录房间时即时获取虚拟玩家人数
	int									nVirtualUser;
	int									nPresentCoinNum;  ///< 赠送金币数量


	
	//比赛专用
	int									iContestID;
	int									iLowCount;
	__int64								i64Chip;
	__int64								i64TimeStart;
	__int64								i64TimeEnd;
	__int64								i64LowChip;
	int									iTimeout;
	int									iBasePoint;


};

///游戏房间登陆
struct MSG_GR_R_OtherRoom
{
	int									iRoomID;
	CHAR								szGameRoomName[61];					///房间名字
};

///游戏桌子状态
struct MSG_GR_DeskStation
{
	BYTE								bDeskStation[100];					///桌子状态
	BYTE								bDeskLock[100];						///锁定状态
	int									iBasePoint[100];					///桌子倍数
	BYTE                                bVirtualDesk[100];                 ///虚拟状态  yjj 090325
};

struct CDeskStation
{
	vector<BYTE> vLocks;
	vector<BYTE> vVirtuals;
	vector<BYTE> vPlayings;
};

///打开钱柜
struct MSG_GR_UserBank
{
	long int						dwUserID;
	__int64						    i64Bank;								///钱柜
	char							szTwoPassword[50];					//银行密码
	int								nVer;								//银行版本
};
struct MSG_GR_ContestAward
{
	long int							dwUserID;
	int									iAward;
};
///打开银行(使用二级密码)
struct MSG_GR_OpenBank
{
	char							szMD5Pass[50];						///加密密码
};

///取钱
struct MSG_GR_CheckMoney
{
	long int						dwUserID;							///用户 ID
	__int64							i64CheckOut;						///取出金额
	__int64							i64MoneyInBank;						///剩余
	__int64							i64MoneyInRoom;
	int								uGameID;							///游戏ID
};
///用户进入房间
struct MSG_GR_R_UserCome
{
	UserInfoStruct						pUserInfoStruct;						///用户信息
};

///用户离开房间
struct MSG_GR_R_UserLeft
{
	long int							dwUserID;							///用户 ID
};

///用户坐下
struct MSG_GR_S_UserSit
{
	BYTE								bDeskIndex;							///桌子索引
	BYTE								bDeskStation;						///桌子位置
	TCHAR								szPassword[61];						///桌子密码
};

///取钱
struct MSG_GR_S_CheckMoneyWithPwd_t
{
	__int64								i64Count;							///金额
	long int							UserID;								///用户ID
	TCHAR								szMD5Pass[50];						///银行密码
};

///存钱
struct MSG_GR_S_CheckMoney
{
	__int64								i64Count;							///金额
	long int							UserID;								///用户ID
};

///转帐
struct MSG_GR_S_TransferMoney
{
	long int							UserID;								///用户ID
	long int							destUserID;							///转给谁
	__int64								i64Money;								///欲转帐金额
	__int64								i64ActualTransfer;					///实际到帐金额
	int                                 bTranTax;                           ///是否扣税，非0不扣税
	TCHAR								szMD5Pass[50];						///银行密码
	TCHAR								szNickName[50];						///用户昵称		add by lxl 2010-11-10
	TCHAR								szDestNickName[50];					///被转账用户昵称
	bool								bUseDestID;
    UINT                                uHandleCode;        // PengJiLin, 2010-8-23, 用来区分是否是“全转”功能
	MSG_GR_S_TransferMoney()
	{
		ZeroMemory(this, sizeof(MSG_GR_S_TransferMoney));
	}
};
///修改密码
struct MSG_GR_S_ChangePasswd_t
{
	long int							UserID;								///用户ID
	TCHAR								szMD5PassOld[50];					///银行密码旧
	TCHAR								szMD5PassNew[50];					///银行密码新
	MSG_GR_S_ChangePasswd_t()
	{
		ZeroMemory(this, sizeof(MSG_GR_S_ChangePasswd_t));
	}
};

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
struct MSG_GR_S_GetNickNameOnID_t
{
    long int    iUserID;        // 用户ID
    TCHAR       szNickName[61]; // 用户昵称
    MSG_GR_S_GetNickNameOnID_t()
    {
        ZeroMemory(this, sizeof(MSG_GR_S_GetNickNameOnID_t));
    }
};

///用户转帐记录
struct MSG_GR_S_TransferRecord_t
{
	long int							dwUserID;							///用户 ID
};
///用户转帐记录结果
struct MSG_GR_R_TransferRecord_t
{
	int								nSeriNo;							///< 序号
	UINT							dwUserID;							///< 用户 ID
	UINT							destUserID;							///< 转给谁
	__int64							i64Money;							///< 欲转帐金额
	__int64							i64ActualTransfer;					///< 实际到帐金额
	COleDateTime					oleDateTime;							///< 时间
	MSG_GR_R_TransferRecord_t()
	{
		ZeroMemory(this, sizeof(MSG_GR_R_TransferRecord_t));
	}
};
///付钱
struct MSG_GR_S_Pay
{
	int									uCount;								///金额
	long int							UserID;								///用户ID
	int									iPayID;								///付钱ID（为什么付钱）
};
///用户断线
struct MSG_GR_R_UserCut
{
	long int							dwUserID;							///用户 ID
	BYTE								bDeskNO;							///游戏桌号
	BYTE								bDeskStation;						///位置号码
};

///用户坐下或者起来
struct MSG_GR_R_UserSit
{
	LONG								dwUserID;							///用户 ID
	BYTE								bLock;								///是否密码
	BYTE								bDeskIndex;							///桌子索引
	BYTE								bDeskStation;						///桌子位置
	BYTE								bUserState;							///用户状态
	BYTE								bIsDeskOwner;						///台主离开
};

// 走马灯，输出
// struct MSG_MarQueueInfo
// {
// 	CHAR buffer[8][120];											//走马灯信息
// };

//发送视频服务器信息
struct SetVideoSrvStruct
{
	char szIP[30];
	int  videoport;
	int  audioport;

	SetVideoSrvStruct()
	{
		::memset(this,0,sizeof(SetVideoSrvStruct));
	}
};

///聊天结构 
struct MSG_GR_RS_NormalTalk
{
	COLORREF							crColor;							///字体颜色
	SHORT								iLength;							///信息长度
	LONG								dwSendID;							///用户 ID
	LONG								dwTargetID;							///目标 ID
	int									nDefaultIndex;						///=0，输入的内容，>0，选择的内容
	CHAR								szMessage[MAX_TALK_LEN+1];			///聊天内容
};

// PengJiLin, 2010-9-10, 踢人卡、防踢卡 结构
struct MSG_GR_RS_KickProp
{
    long int    dwUserID;
    int         iPropID;
    int         iTotalTime;     // 总时间s
};

///用户同意结构
struct MSG_GR_R_UserAgree
{
	BYTE								bDeskNO;							///游戏桌号
	BYTE								bDeskStation;						///位置号码
	BYTE								bAgreeGame;							///同意标志
};
///用户分数
struct MSG_GR_R_InstantUpdate
{
	long int							dwUserID;							///用户 ID
	int									dwPoint;							///用户分数
	int									dwMoney;							///用户金币
};
///用户经验值
struct MSG_GR_R_UserPoint
{
	long int							dwUserID;							///用户 ID
	__int64									dwPoint;							///用户经验值
	__int64									dwMoney;							///用户金币
	__int64									dwSend;								///赠送
	BYTE								bWinCount;							///胜局
	BYTE								bLostCount;							///输局
	BYTE								bMidCount;							///平局
	BYTE								bCutCount;							///逃局
	RECEIVEMONEY                        strAutoSendMoney;                   //添加自动赠送 2013-01-29 wangzhitu
};

///用户比赛信息
struct MSG_GR_ContestChange
{
	long int					dwUserID;							///用户 ID	
	int							iContestCount;						///比赛局数
	__int64						i64ContestScore;					///比赛分数
	int							iRankNum;							///比赛排名
	int							iRemainPeople;						///比赛中还剩多少人
	char						szAward[30];						///比赛获取的奖励
	MSG_GR_ContestChange()
	{
		ZeroMemory(this, sizeof(MSG_GR_ContestChange));
	}
};

struct MSG_GR_ContestApply
{
	int iApplyResult;//报名结果
	int	iContestBegin;//比赛是否开始
};
///用户形象更改信息
struct MSG_GR_AvatarChange
{
	long int						dwUserID;						///用户 ID	
	UINT							nNewLogoID;						///新的头像ID
};

///修改用户经验值
struct MSG_GR_S_RefalshMoney
{
	long int							dwUserID;							///用户 ID
	__int64								i64Money;							///用户金币
};

///打开经验盒
struct MSG_GR_S_OpenPoint
{
	long int							dwUserID;							///用户 ID
	int									dwPoint;							///经验
};

///取出结果
struct MSG_GR_S_GetPoint
{
	long int							dwUserID;							///用户 ID
	int									dwPoint;							///经验
};

///修改用户桌子倍数设定
struct MSG_GR_S_SetDeskBasePoint
{
	long int							dwUserID;							///用户ID
	int									iDeskBasePoint;						///用户设定的桌子倍数
	BYTE								bDeskOwner;							///是否台主
};
///设置数据包
struct MSG_GR_S_UserSet
{
	UserRuleStruct						m_Rule;								///用户规则
};

///邀请加入数据包
struct MSG_GR_SR_InviteUser
{
	long int							dwUserID;							///用户 ID
	long int							dwTargetID;							///目标用户 ID
};

///比赛用户数据包
struct MSG_GR_SR_MatchUser
{
	long int							dwUserID;							///用户 ID
	char								szUserName[61];						///用户名字
	BYTE								bDeskIndex;							///桌子号码
	BYTE								bDeskStation;						///桌子位置
};

///取消比赛数据包
struct MSG_GR_SR_MatchDelete
{
	long int							dwUserID;							///用户 ID
	BYTE								bDeskIndex;							///桌子号码
	BYTE								bDeskStation;						///桌子位置
};

///踢玩家下线数据包
struct MSG_GR_S_CutUser
{
	long int							dwUserID;							///管理员用户 ID
	long int							dwTargetID;							///目标用户 ID
};

///呼叫GM
struct MSG_GR_Call_GM
{
	BYTE								bCallReason;						///呼叫类型
	UINT								uLength;							///消息长度
	char								szMsg[201];							///呼叫留言
};

///警告数据包
struct MSG_GR_SR_Warning
{
	long int							dwTargetID;							///目标用户
	UINT								uLength;							///消息长度
	char								szWarnMsg[201];						///警告消息
};

///用户权限设置数据包
struct MSG_GR_SR_GamePowerSet
{
	BYTE								bShowActive;						///是否显示
	BYTE								bBindKind;							///绑定类型
	UINT								uLockTime;							///锁定时间
	long int							dwUserID;							///用户 ID
	long int							dwGamePowerID;						///游戏权限
	long int							dwMasterPowerID;					///管理权限
};


///魅力值
struct MSG_GR_Charm_Struct
{
	long int							dwUserID;							///玩家ID
	int									iCharmValue;						///魅力变动值
};

///队例情况
struct MSG_GR_Queue
{
	int									iCount;								///当前队例中有多少人排队
	int									iReserver;							///保留
};


/********************************************************************************************/

///游戏信息
struct MSG_GM_S_ClientInfo
{
	BYTE								bEnableWatch;						///允许旁观
};

///游戏信息
struct MSG_GM_S_GameInfo
{
	BYTE								bGameStation;						///游戏状态
	BYTE								bWatchOther;						///允许旁观
	BYTE								bWaitTime;							///等待时间
	BYTE								bReserve;							///保留字段
	char								szMessage[1000];					///系统消息
};

///旁观设置
struct MSG_GM_WatchSet
{
	long int							dwUserID;							///设置对象
};

///道具
struct PropStruct
{
	BYTE							sendDeskStation;				///发放位置
	BYTE							recvDeskStation;				///接收位置
	int								iValue;							///动画id
};

///好友相关
const int DB_BUFFER_MSG_MAX_LEN = 255;

///好友消息
struct tagIM_MSG
{
	int								iMessageID;							///消息ID
	long int						dwSendUserID;						///发送者ID
	long int						dwRecvUserID;						///接收者ID

	DBTIMESTAMP						tmSendTime;							///时间戳
	int								iDataLen;							///数据长度
	BYTE							Data[DB_BUFFER_MSG_MAX_LEN];		///数据
};
/********************************************************************************************/

///消息数据包
struct MSG_GA_S_Message
{
	BYTE								bFontSize;							///字体大小
	BYTE								bCloseFace;							///关闭界面
	BYTE								bShowStation;						///显示位置
	CHAR								szMessage[1000];					///消息内容
};

// PengJiLin, 2010-9-14, 使用踢人卡踢人的结果
struct MSG_GA_NEW_KICKUSERPROP
{
    long int    dwUserID;
    long int    dwDestID;
    BYTE        byKickOther;        // 1=自己踢别人
    BYTE        byKickResult;
};



/******************************************************************************************/
//新银行相关

//游戏钱包结构
struct bank_game_wallet
{
	int	_game_id;
	char _game_name[30];
	__int64	_money;
};

//普通操作存取钱
struct bank_op_normal
{
	int _operate_type;
	int	_user_id;
	int	_game_id;
	TCHAR _szMD5Pass[50];  ///银行密码
	__int64	_money;
};

//转帐
struct bank_op_transfer
{
	int	_user_id1;
	int	_user_id2;
	int _recode_id;
	__int64	_money;
	char _user_name1[60];
	char _user_name2[60];
};

//存单,充值
struct bank_op_deposit
{
	int _operate_type;  //1为创建存单,2为充值
	int	_user_id;
	TCHAR _szNickName[50];  ///被转账用户昵称
	TCHAR _szMD5Pass[50];   ///银行密码
	int	_state;
	int	_csds;
	__int64	_money;
	char _deposit_id[16];
	char _deposit_password[16];
	char _time[32];
};

//存单查询结构
struct bank_op_deposit_node
{
	int	_serial_no;	    ///< 序号
	char _deposit_id[16];
	char _deposit_password[16];
	__int64	_money;
	COleDateTime _create_time;
	int	_csds;
	int	_user_id;
	int	_state;
	int	_used_user_id;
};


//游戏公告消息ATT
struct GameNoticeMessage
{
	BYTE			bDeskIndex;			//桌子号
	BYTE			bAwardCardShape;	//牌型奖励
	TCHAR			szMessage[255];		//未进入游戏 在房间 右边框显示公告内容
	GameNoticeMessage()
	{
		::memset(this,0,sizeof(GameNoticeMessage));
	}
};

/*****************************************************************************************/

/********************************************************************************************/

///	通信标识定义 

/********************************************************************************************/

///大厅主标识
#define	MDM_GR_LOGON					100									///大厅登陆
#define MDM_GR_USER_LIST				101									///用户列表
#define MDM_GR_USER_ACTION				102									///用户动作
#define MDM_GR_ROOM						103									///房间信息
#define MDM_GR_MATCH_INFO				104									///比赛信息
#define MDM_GR_MANAGE					105									///管理消息
#define MDM_GR_MESSAGE					106									///信息消息
#define MDM_GR_MONEY					107									///划账，扣钱
#define MDM_GR_POINT					108									///经验操作

///#define MDM_GR_BROADCAST_MESSAGE		109									/// 广播消息
///#define MDM_GR_UPDATE_HOLD_ITEM_DATA	110									/// 更新物品数据；
///#define MDM_GR_GET_ITEM_COUNT			111									/// 获取道具总数
#define MDM_GR_FRIEND_MANAGE			113									///好友消息
#define MDM_GR_QUEUE_MSG				114									///排队消息
#define MDM_GR_DESK						115
#define MDM_GR_NETSIGNAL                116                                 ///网络信号消息	

// PengJiLin, 2010-10-14, 由房间（起始点）发给游戏端（接收终点）的消息
#define MDM_GR_ROOM2GAME                120
#define ASS_GR_R2G_USEPROP              1       // PengJiLin, 2010-10-14, 游戏端道具的即买即用功能


/********************************************************************************************/

///游戏主标识
#define MDM_GM_GAME_FRAME				150									///框架消息
#define MDM_GM_MESSAGE					151									///信息消息
#define MDM_GM_GAME_NOTIFY				180									///游戏消息
#define ASS_GM_MESSAGE_EX				180									///游戏消息特殊游戏ATT
/********************************************************************************************/

///大厅登陆
#define ASS_GR_LOGON_BY_ID				5									///通过用户 ID 登陆
#define ASS_GR_LOGON_SUCCESS			2									///登陆成功
#define ASS_GR_LOGON_ERROR				3									///登陆失败
#define ASS_GR_SEND_FINISH				4									///登陆完成
#define ASS_GR_IS_VIPROOM               16                                   ///<是否VIP房间
#define ASS_GR_VIP_PW                   17                                   ///<VIP房间需要密码
#define ASS_GR_VIP_NO_PW                18                                   ///< VIP房间不需要密码(第一个进入不需要密码)
#define ASS_GR_NO_VIP                   19                                   ///< 不是VIP房间
#define ASS_GR_VIPROOM_PW               20                                   ///< VIP房间密码
#define ASS_GR_VIPROOM_PW_RIGHT         21                                   ///< VIP房间密码正确

///用户列表
#define ASS_GR_ONLINE_USER				1									///在线用户
#define ASS_GR_NETCUT_USER				2									///断线用户
#define ASS_GR_DESK_STATION				3									///桌子状态

///用户动作
#define ASS_GR_USER_UP					1									///用户起来
#define ASS_GR_USER_SIT					2									///用户坐下
#define ASS_GR_WATCH_UP					3									///旁观起来
#define ASS_GR_WATCH_SIT				4									///旁观坐下
#define ASS_GR_USER_COME				5									///用户进入
#define ASS_GR_USER_LEFT				6									///用户离开
#define ASS_GR_USER_CUT					7									///用户断线
#define ASS_GR_SIT_ERROR				8									///坐下错误
#define ASS_GR_SET_TABLE_BASEPOINT		9									///改变桌子倍数
#define ASS_GR_USER_HIT_BEGIN			10									///用户同意开始
#define ASS_GR_JOIN_QUEUE				11									///<加入排队机
#define ASS_GR_QUIT_QUEUE				12									///<退出排队机
#define ASS_GR_QUEUE_USER_SIT			13									///<排队用户坐下
#define ASS_GR_LEASE_TIMEOVER			14									///<排队用户坐下
#define ASS_GR_CONTEST_APPLY			15									//比赛场报名


///房间信息
#define ASS_GR_NORMAL_TALK				1									///普通聊天
#define ASS_GR_HIGH_TALK				2									///高级聊天
#define ASS_GR_USER_AGREE				3									///用户同意
#define ASS_GR_GAME_BEGIN				4									///游戏开始
#define ASS_GR_GAME_END					5									///游戏结束
#define ASS_GR_USER_POINT				6									///用户经验值
#define ASS_GR_USER_CONTEST				17									///用户比赛信息
#define ASS_GR_AVATAR_LOGO_CHANGE		18									///用户形象更改信息
#define	ASS_GR_CAHNGE_ROOM				19									///比赛开始，未报名用户切换房间
#define ASS_GR_CONTEST_GAMEOVER			20									///比赛结束
#define ASS_GR_CONTEST_KICK				21									///用户被淘汰
#define ASS_GR_CONTEST_WAIT_GAMEOVER	22									///比赛结束，但是有用户还在打最后一局，通知已打完的用户等待排名
#define ASS_GR_INIT_CONTEST				23									///比赛开始，初始化用户排名和在线人数

#define ASS_GR_SHORT_MSG				7									///聊短信息
#define ASS_GR_ROOM_SET					8									///房间设置
#define ASS_GR_INVITEUSER				9									///邀请用户
#define ASS_GR_INSTANT_UPDATE			10									///即时更新分数金币
#define ASS_GR_UPDATE_CHARM				11									///即时更新魅力
#define ASS_GR_ROOM_PASSWORD_SET		12									///房间设置
#define ASS_GR_ROOM_QUEUE_READY			13									///排队机准备
#define ASS_GR_GET_NICKNAME_ONID        14          // 根据ID获取昵称，PengJiLin, 2010-8-23
#define ASS_GR_OWNER_T_ONE_LEFT_ROOM    15          // 房主踢玩家离开房间, PengJiLin, 2010-8-26
#define ASS_GR_GET_NICKNAME_ONID_INGAME 16          // 根据ID获取昵称 游戏中 lxl 2010-11-17
///比赛信息
#define ASS_GR_MATCH_USER				1									///比赛用户
#define ASS_GR_MATCH_DELETE				2									///取消比赛

///管理消息
#define ASS_GR_CUT_USER					1									///踢用户下线
#define ASS_GR_WARN_MSG					2									///警告消息
#define ASS_GR_POWER_SET				3									///权限设置
#define	ASS_GR_FORBID_USER				4									///禁止用户帐号
#define	ASS_GR_FORBID_USER_IP			5									///禁止用户IP
#define ASS_GR_LIMIT_OTHER_ROOM_TALK	6									///禁止用户大厅聊天
#define ASS_GR_CALL_GM					7									///呼叫GM
#define ASS_GR_CALL_GM_SUCCESS			8									///呼叫GM成功
#define ASS_GR_CALL_GM_FAIL				9									///呼叫GM失败
#define ASS_GR_GAME_POWER_SET_SUCCESS	10									///设置游戏权限成功
#define ASS_GR_GAME_POWER_SET_FAIL		11									///设置游戏权限失败
#define ASS_GR_GAME_POWER_SET_RETURN	12									///设置游戏权限反馈
#define ASS_GR_GAME_POWER_SET_RELEASE	13									///解除游戏权限成功反馈
#define ASS_GR_OPT_USER					14									///添加好友	

#define ASS_GR_ALONE_DESK				15									///封桌20081212 Fred Huang
#define ASS_GR_DEALONE_DESK				16									///解除封桌20081212 Fred Huang

///信息消息
#define ASS_GR_SYSTEM_MESSAGE			1									///系统消息

///钱柜划账
#define ASS_GR_OPEN_WALLET				1									///察看余额
#define ASS_GR_CHECK_OUT				2									///取出金币
#define ASS_GR_CHECK_IN 				3									///存入金币
#define ASS_GR_REFLASH_MONEY			4									///同步各房间金币
#define ASS_GR_PAY_MONEY				5									///扣钱
#define ASS_GR_TRANSFER_MONEY			6									///转帐
#define ASS_GR_SENDMONEY_TIMES          7                                   ///按次送钱
#define ASS_GR_OPEN_WALLET_INGAME		8									///游戏中打开平台察看余额
#define ASS_GR_CHECK_OUT_INGAME			9									///游戏中取出金币
#define ASS_GR_TRANSFER_RECORD			10									///< 转帐历史记录
#define ASS_GR_CHECK_IN_INGAME 			11									///< 游戏中存入金币
#define ASS_GR_TRANSFER_MONEY_INGAME	12									///< 游戏中转帐
#define ASS_GR_TRANSFER_RECORD_INGAME	13									///< 游戏中转帐历史记录
#define ASS_GR_CHANGE_PASSWD			14									///< 修改密码
#define ASS_GR_CHANGE_PASSWD_INGAME		15									///< 游戏中修改密码

///wushuqun 2009.6.5
///混战房间结果记录信息
#define ASS_GR_BATTLEROOM_RESULT       8

///不在混战场活动时间内
#define DTR_GR_BATTLEROOM_TIMEOUT_ERROR   21

///
// PengJiLin, 2010-8-23, 根据用户ID获取昵称 结果
#define ASS_GR_GETNICKNAME_SUCCESS      0           // 操作成功
#define ASS_GR_GETNICKNAME_NOTEXIST     1           // ID 不存在
#define ASS_GR_GETNICKNAME_ISNULL       2           // 昵称为空

///钱柜划账结果
#define ASS_GR_OPEN_WALLET_ERROR		1									///打开钱柜错误
#define ASS_GR_OPEN_WALLET_SUC			2									///打开钱柜成功
#define ASS_GR_OPEN_WALLET_ERROR_PSW	3									///打开钱框错误
///房间－＞钱柜
#define ASS_GR_CHECKIN_ERROR			1									///存钱失败
#define ASS_GR_CHECKIN_SUC				2									///存钱成功
#define ASS_GR_CHECKIN_ERROR_PLAYING	3								    ///游戏中，不能存款
#define ASS_GR_CHECKIN_ZHUANGJIA		4									///百人类游戏，庄家不能存款 add by lxl 2010-11-5
#define ASS_GR_CHECKOUT_ERROR_PLAYING   5									///游戏中  不能取款	add by lxl 2010-12-6
///房间< －钱柜
#define ASS_GR_CHECKOUT_ERROR			1									///取钱错误
#define ASS_GR_CHECKOUT_SUC				2									///取钱成功

///转帐
#define ASS_GR_TRANSFER_ERROR			1									///转帐错误
#define ASS_GR_TRANSFER_SUC				2									///转帐成功
#define ASS_GR_TRANSFER_NO_DEST			3									///转帐目标不存在
#define ASS_GR_PASSWORD_ERROR			4									///转帐密码错误
#define ASS_GR_TRANSFER_TOTAL_LESS		5									///银行总额太小，不够资格
#define ASS_GR_TRANSFER_TOO_LESS		6									///单笔转帐数目太少
#define ASS_GR_TRANSFER_MULTIPLE		7									///单笔转帐数目必须是某数的倍数
#define ASS_GR_TRANSFER_NOT_ENOUGH		8									///银行金额不够本次转帐
#define ASS_GR_TRANSFER_TOO_MANY_TIME   9                                   ///< 当天转账的次数太多了
/********************************************************************************************/

///游戏取经验
#define ASS_GR_OPEN_POINT				1									///打开经验盒
#define ASS_GR_OPEN_SUC					2									///打开完成
#define ASS_GR_OPEN_ERROR				3									///打开错误
#define ASS_GR_GET_POINT				4									///取出经验
#define ASS_GR_GETP_SUC					5									///取出经验完成
#define ASS_GR_GETP_ERROR				6									///取出错误

///框架消息
#define ASS_GM_GAME_INFO				1									///游戏信息
#define ASS_GM_GAME_STATION				2									///游戏状态
#define ASS_GM_FORCE_QUIT				3									///强行退出
#define ASS_GM_NORMAL_TALK				4									///普通聊天
#define ASS_GM_HIGH_TALK				5									///高级聊天
#define ASS_GM_WATCH_SET				6									///旁观设置	
#define ASS_GM_CLEAN_USER               9                                   //比赛场清理用户信息
#define ASS_GM_USE_KICK_PROP            7									// 使用踢人卡
#define ASS_GM_USE_ANTI_KICK_PROP       8									// 使用防踢卡
#define ASS_GM_SET_VIDEOADDR			10									//设置视频服务器地址

// PengJiLin, 2010-9-10, 踢人卡、防踢卡错误消息
#define ERR_GR_KICK_PROP_NULL           1       // 没有道具
#define ERR_GR_KICK_SET_TIME_FAIL       2       // 设置时间错误
#define ERR_GR_KICK_SET_TIME_SUCCESS    3       // 设置时间成功

// PengJiLin, 2010-9-13, 使用踢人卡踢人错误消息
#define ERR_GR_NEW_KICKUSER_SUCCESS             1       // 成功
#define ERR_GR_NEW_KICKUSER_NOTIME              2       // 自己的道具已过期，或者没有道具
#define ERR_GR_NEW_KICKUSER_HAD_ANTI            3       // 对方有防踢卡时间
#define ERR_GR_NEW_KICKUSER_HAD_VIP             4       // 对方有VIP时间
#define ERR_GR_NEW_KICKUSER_PLAYING             5       // 游戏中不踢人

///信息消息
#define ASS_GM_SYSTEM_MESSAGE			1									///系统消息

#define ASS_GM_SYSTEM_MESSAGE_EX		2									///系统消息特殊游戏专用（ATT）

///游戏消息
#define ASS_GM_AGREE_GAME				1									///同意游戏
#define ASS_PROP						2									///同意游戏

///排序消息
#define ASS_GR_QUEUE_ERROR				0									///错误
#define ASS_GR_ADD_QUEUE				1									///加入排队
#define ASS_GR_ADD_QUEUE_RESULT			2									///排队结果
#define ASS_GR_PLAN_SIT					3									///分配桌子
#define ASS_GR_AGREE_PLAN_SIT			4									///服务分配桌子
#define ASS_GR_REFUSE_PLAN_SIT			5									///不服从分配坐号

/********************************************************************************************/

///错误代码
#define ERR_GR_ERROR_UNKNOW				0									///未知错误
#define ERR_GR_LOGON_SUCCESS			1									///登陆成功
#define ERR_GR_USER_NO_FIND				2									///用户不存在
#define ERR_GR_USER_PASS_ERROR			3									///用户密码错误
#define ERR_GR_USER_VALIDATA			4									///用户帐号禁用
#define ERR_GR_USER_IP_LIMITED			5									///登陆 IP 禁止
#define ERR_GR_IP_NO_ORDER				6									///不是指定地址
#define ERR_GR_ONLY_MEMBER				7									///会员游戏房间
#define ERR_GR_IN_OTHER_ROOM			8									///正在其他房间
#define ERR_GR_ACCOUNTS_IN_USE			9									///帐号正在使用
#define ERR_GR_PEOPLE_FULL				10									///人数已经满
#define ERR_GR_LIST_PART				11									///部分用户列表
#define ERR_GR_LIST_FINISH				12									///全部用户列表
#define ERR_GR_STOP_LOGON				13									///暂停登陆服务

#define ERR_GR_CONTEST_NOSIGNUP			14
#define ERR_GR_CONTEST_TIMEOUT			15
#define ERR_GR_CONTEST_KICK				16
#define ERR_GR_CONTEST_NOTSTRAT			17
#define ERR_GR_CONTEST_OVER				18
#define ERR_GR_CONTEST_BEGUN			19

#define ERR_GR_MATCH_LOGON				160									///游戏房间
#define ERR_GR_TIME_OVER				161									///时间到期

///wushuqun 2009.6.5
///不在混战场活动时间内
#define ERR_GR_BATTLEROOM_OUTTIME       162

///用户坐下错误码
#define ERR_GR_SIT_SUCCESS				50									///成功坐下
#define ERR_GR_BEGIN_GAME				51									///游戏已经开始
#define ERR_GR_ALREAD_USER				52									///已经有人存在
#define ERR_GR_PASS_ERROR				53									///密码错误
#define ERR_GR_IP_SAME					54									///IP 相同
#define ERR_GR_CUT_HIGH					55									///断线率太高
#define ERR_GR_POINT_LOW				56									///经验值太低
#define ERR_GR_POINT_HIGH				57									///经验值太高
#define ERR_GR_NO_FRIEND				58									///不受欢迎
#define ERR_GR_POINT_LIMIT				59									///经验值不够
#define ERR_GR_CAN_NOT_LEFT				60									///不能离开
#define ERR_GR_NOT_FIX_STATION			61									///不是这位置
#define ERR_GR_MATCH_FINISH				62									///比赛结束
#define ERR_GR_MONEY_LIMIT				63									///金币太低
#define ERR_GR_MATCH_WAIT				64									///比赛场排队提示
#define ERR_GR_IP_SAME_3				65									///IP前3 相同
#define ERR_GR_IP_SAME_4				66									///IP前4 相同
#define ERR_GR_UNENABLE_WATCH			67									///不允许旁观
#define ERR_GR_DESK_FULL				68									/// 百家乐 桌子座位满了，无法分配座位给玩家 add by wlr 09.07.14


//排列位置
#define SK_ENEMY_USER						0								//敌人排列索引
#define SK_NORMAL_USER						1								//玩家排列索引

#define SK_NORMAL_MEMBER					2								//普通会员索引
#define SK_HIGHT_MEMBER	                    3								//高级会员索引
#define SK_SPECIAL_MEMBER					4								//特邀嘉宾索引
#define SK_FOREVER_MEMBER					5								//永久会员索引
#define SK_SENIOR_MEMBER					6								//元老玩家索引

#define SK_FRIEND_USER						7								//朋友排列索引
#define SK_MASTER_USER						8								//管理排列索引
#define SK_ME_USER							9								//自己排列索引
#define SK_ALWAYS_USER						10								//固顶排列索引

//关系定义
#define CM_NORMAL							0								//普通关系
#define CM_ENEMY							1								//敌人关系
#define CM_FRIEND							2								//朋友关系

//银行消息定义
#define MDM_BANK				131

#define ASS_BANK_NORMAL			1
#define ASS_BANK_TRANSFER		2
#define ASS_BANK_DEPOSIT		3
#define ASS_BANK_WALLET			4
#define ASS_BANK_QUERY_DEPOITS	5

#define HC_BANK_OP_SUC			0			//操作成功
#define HC_BANK_OP_NO_USER		1			//没有找到用户
#define HC_BANK_OP_ERR_PWD		2			//错误密码
#define HC_BANK_OP_NOT_MONEY	4			//余额不足
#define HC_BANK_OP_NO_SERIAL	5			//序列号已用完
#define HC_BANK_OP_INVDEP		8			//充值已作废
#define HC_BANK_OP_CHECKIN_PLAYING		9	//游戏中，不能存款
#define HC_BANK_OP_CHECKOUT_PLAYING		10	//游戏中，不能取款
#define HC_BANK_OP_CHECKIN_ZHUANG		11  //百人类游戏，庄家不能存款
#define HC_BANK_OP_CHECKMONEY_INV		5	//非法存取钱


// Unity3d消息ID 初始化数据
#define IPC_MAIN_GAMEBASEINFO				8
#define IPC_SUB_GAMEBASEINFO				0
#define IPC_SUB_DATAOVER					1
#define IPC_SUB_REINIT						2

#define IPC_MAIN_USERINFO					10
#define IPC_SUB_USERINFOLIST				0								//用户列表
#define IPC_SUB_USERAGREE					1								//用户同意
#define IPC_SUB_USERSITDOWN					2								//用户坐下
#define IPC_SUB_USERSTANDUP					3								//用户起来
#define IPC_SUB_USEOFFLINE					4								//用户断线
#define IPC_SUB_CLICKBEGIN					5								//用户点击开始游戏
#define IPC_SUB_USERDETAILINFO				6								//用户详细信息
#define IPC_SUB_UPDATEUSERINFO				7								//刷新用户信息
#define IPC_SUB_MATCHRANK					8								//比赛排名

#define IPC_SUB_USERSTANDUP_STATION			9								//用户站起位置
#define IPC_SUB_USERSIT_STATION				10								//用户坐下位置

//更新金币经验魅力
#define IPC_MAIN_UPDATE						11								//更新金币主ID
#define IPC_SUB_USERPOINT					1								//更新金币分数
#define IPC_SUB_USEREXPERIENCE				2								//更新用户经验
#define IPC_SUB_USERFASCINATION				3								//更新用户魅力

#define IPC_MAIN_QUEUE						13
#define IPC_SUB_JOIN						1
#define IPC_SUB_QUIT						2
#define IPC_SUB_BEGIN						3

#define IPC_MAIN_TASKAWARDLIST				14
#define IPC_SUB_TASKAWARDLIST				1
#define IPC_SUB_TASKAWARD					2

#define IPC_MAIN_TASKDAILY					15
#define IPC_SUB_TASKDAILY_LIST				1
#define IPC_SUB_TASKDIALY_UPDATE			2

#define IPC_MAIN_CONTESTINFO				16
#define IPC_SUB_CONTEST_REALLOTDESK			1
#define IPC_SUB_CONTEST_AUTO				2
#define IPC_SUB_CONTEST_FINISH_INFO			3
#define IPC_SUB_OPEN_RANK_PAGE				4
#define IPC_SUB_CONTEST_NOSCORE				5
#define IPC_SUB_CONTEST_FINISH_TIME			6

#define IPC_MAIN_BANK						17
#define	IPC_SUB_CHECK_IN_MONEY				1			//存钱
#define IPC_SUB_CHECK_OUT_MONEY				2			//取钱
#define IPC_SUB_CHECKIN_ERROR				3			//存钱失败
#define IPC_SUB_CHECKIN_ERROR_PLAYING		4			//游戏中不能存
#define IPC_SUB_CHECKOUT_ERROR				5			//取款失败
#define IPC_SUB_CHECKOUT_ERROR_PASSWORD		6			//二级密码错误

#define IPC_SYSTEMMSG						18
#define IPC_SUB_PUREMSG						1
#define IPC_SUB_SYSTEMMSG					2

#define IPC_MAIN_ACHIEVE					19
#define IPC_SUB_SET_USERINFO				1

#define IPC_MAIN_GAMEMASTER					20
#define IPC_SUB_GM_LIST						1
#define IPC_SUB_GM_TALK_TOGM				2
#define IPC_SUB_GM_TALK_FROMGM				3

//自己定义的协议
#define IPC_MAIN_GAMEMASSAGE				180
#define IPC_FRAME_GAMEMASSAGE				150
#define IPC_SYSTEM_GAMEMESSAGE				151
#define IPC_SUB_GAMEINFO					1

//一个游戏强制退出为安全退出
#define ASS_ONE_FINISH						96

// Unity3d消息结构
struct u3dSGameBaseInfo {
	u3dSGameBaseInfo() {
		ZeroMemory(this, sizeof(u3dSGameBaseInfo));
	}
	int					iDeskPeople;								//房间人数
	int					iLessExperience;							//最少经验值
	int					iPower;										//倍率
	long				lRoomRule;									//房间规则
	int					iGameNameID;								//游戏ID
	BOOL				bCanTalk;									//是否可以聊天
	UINT				uTaskAwardType;								//房间奖励类型序列号 以此为下标中找
	UINT				uTaskAwardUnit;								//房间奖励类型倍数，通常是正整数
	WCHAR				szGameRoomName[64];							//房间名称	
	WCHAR				money[320];									
	WCHAR				virtualMoney[320];	
	int					iContestType;								//比赛类型
	int					iDeskNO;									//桌子号
};

//用户基本信息
struct u3dUserBaseInfo {
	u3dUserBaseInfo() {ZeroMemory(this, sizeof(u3dUserBaseInfo));}
	~u3dUserBaseInfo(){}
	
	UINT			uiUserID;										//ID号码
	int				iImageNO;										//个人形象图片文件名
	UINT			uiAge;											//年龄
	int				bBoy;											//0表示女
	int				StarTag;										//星座
	int				BornTag;										//生肖
	int				BloodTag;										//血型
	int				iLevel;											//用户等级
	int				iDeskNO;										//游戏桌号
	int				iDeskStation;									//桌子位置
	int				iRoomID;										//房间号
	int				iUserState;										//用户状态
	int				iMoney;											//用户金币
	int				iBank;											//用户财富
	int				iTreasure;										//元宝
	int				iFlag;											//0自己，1玩家，2旁观
	int				iWinCount;										//胜利数目
	int				iLostCount;										//输数目
	int				iDrawCount;										//和局数目
	int				iPoint;											//积分 
	int				nMedalID;										
	int				bUsingBank;
	WCHAR			szNickName[64];									//昵称
	WCHAR			szCountry[32];									//国家
	WCHAR			szProvince[64];									//省份
	WCHAR			szCity[64];										//城市
	WCHAR			szGameSign[64];									//备注
	__int64			dwBirthday;										//生日
};

//用户同意消息结构
struct u3dUserAgree {
	int		bDeskNO;						//游戏桌号
	int		bDeskStation;					//位置号码
	int		bAgreeGame;						//同意标志
	int		bNotUse;						//仅仅用于对齐占位置
};

/********************************************************************************************/

#endif
