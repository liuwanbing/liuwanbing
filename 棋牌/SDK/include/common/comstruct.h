#pragma once

#ifndef AFC_COM_STRUCT_HEAD_FILE
#define AFC_COM_STRUCT_HEAD_FILE

/********************************************************************************************/
#include "PropMessage.h"

///房间规则设置
#define GRR_MEMBER_ROOM			0x00000001L							///会员房间
#define GRR_IP_LIMITED			0x00000002L							///地址限制
#define GRR_ENABLE_WATCH		0x00000004L							///允许旁观
#define GRR_UNENABLE_WATCH		0x00000008L							///不许旁观
#define GRR_AUTO_SIT_DESK		0x00000010L							///自动坐下，现在用于防作弊场使用
#define GRR_LIMIT_DESK			0x00000020L							///限制座位
#define GRR_LIMIT_SAME_IP		0x00000040L							///限制同IP
#define GRR_RECORD_GAME			0x00000080L							///记录游戏
#define GRR_STOP_TIME_CONTROL	0x00000100L							///停止时间控制
#define GRR_ALL_ON_DESK			0x00000200L							///是否所有人坐下才开始启动
#define GRR_FORBID_ROOM_TALK	0x00000400L							///禁止房间聊天
#define GRR_FORBID_GAME_TALK	0x00000800L							///禁止游戏聊天
#define GRR_MATCH_REG			0x00001000L							///比赛报名
#define GRR_EXPERCISE_ROOM		0x00002000L							///训练场
#define GRR_VIDEO_ROOM			0x00004000L							///视频房间
///
///修改防作弊场可看见其他玩家姓名和头像问题！
#define GRR_NOTFUFENG			0x00008000L		///不允许负积分
#define GRR_NOTCHEAT			0x00010000L		///防作弊
///
///台费场
#define GRR_ALL_NEED_TAX		0x00020000L		///收台费场,所有人都需缴纳一定数额台费
#define GRR_QUEUE_GAME			0x00040000L		/// 排队机

#define GRR_NOT_COST_POINT		0x00080000L		///金币场不扣积分

#define GRR_CONTEST				0x00100000L		///定时淘汰比赛场

#define GRR_GAME_U3D			0x00200000L		///U3D游戏

///百家乐
#define DESK_TYPE_NORMAL		0		///
#define DESK_TYPE_BJL			1		///

#define		HUNDRED_PEOPLE_DESK_GAME_NAME   10301800					///百人桌游戏编号
#define		SUOHA_PEOPLE_DESK_GAME_NAME		30501800					///百变骰子梭哈ID
#define		NIUNIU_PEOPLE_DESK_GAME_NAME	10901800					///百变牛牛ID
#define		ERZHANGPAI_PEOPLE_DESK_GAME_NAME	11901800				///百变二张牌ID


///end of 百家乐
///
///
///游戏组件信息结构 （服务器 DLL 组件获取）
struct ServerDllInfoStruct
{
	///游戏组件信息
	UINT						uServiceVer;						///服务版本
	UINT						uNameID;							///游戏名字 ID
	UINT						uSupportType;						///支持类型
	UINT						uDeskPeople;						///游戏人数
	char						szGameName[61];						///游戏名字
	char						szGameTable[61];					///数据库表名字
	char						szDLLFileName[61];					///文件名字

	///辅助信息
	char						szWriter[61];						///软件开发者
	char						szDllNote[51];						///备注信息
};

///公共组件加载信息结构	（服务器设置数据）
struct ManageInfoStruct
{
    int                         iRoomFlag[2];						// 不使用的字段，占位用

	///游戏信息
	UINT						dwRoomRule;							///游戏设置规则
	UINT						dwUserPower;						///游戏附加权限
	char						szLockTable[31];					///锁定表名字
	char						szIPRuleTable[31];					///IP 限制表名字
	char						szNameRuleTable[31];				///UserName限制表名字

    int                         iRoomInfoFlag[2];					// 不使用的字段，占位用

	///设置信息
	UINT						uComType;							///游戏类型
	UINT						uDeskType;							///游戏桌类型，主要为添加百家乐桌
	UINT						uBasePoint;							///游戏基础分
	UINT						uLessPoint;							///游戏的最少分数
	UINT						uMaxPoint;							///游戏的最大分数
	UINT						uMaxLimite;							///最大封顶值
	UINT						uRunPublish;						///逃跑扣分
	UINT						uTax;								///税收比例
	UINT						uListenPort;						///监听端口
	UINT						uMaxPeople;							///最大连接数目
	int							iSocketSecretKey;					///socket加密的密钥

	//比赛专用
	int									iGameID;
	long int							iMatchID;
	int									iContestID;
	int									iRankNum;//排名数量
	int									*pAwards;//比赛奖励
	int									iLowCount;
	__int64								i64Chip;
	__int64								i64LowChip;
	__int64								i64TimeStart;
	__int64								i64TimeEnd;
	int									iTimeout;
	int									iLeasePeople;					///比赛人数少于此值，代表结束比赛
	int									iUpPeople;						///比赛人数达到此值，才开始分配桌子，并设置房间状态为开始游戏
	int									iRoomState;	

	//租赁房间ID与时间
	int									iLeaseID;
	__int64								i64LeaseTimeStart;
	__int64								i64LeaseTimeEnd;


	//每局超过时间是否赠送金币
	int									iTimeLength;
	__int64								iSendCount;

	///
	///Vip登陆功能
	UINT uVipReseverCount;///Vip保留登陆数目
	///

	bool                        bVIPRoom;                            ///< 金葫芦二代，是否VIP房间（VIP房间设置密码）

	UINT						uDeskCount;							///游戏桌数目
	char						szGameTable[31];					///信息表名字
	UINT						uStopLogon;							///停止登陆房间
	
	int						uVirtualUser;						
	int						uVirtualGameTime;					

	
	///混战场房间ID       --begin
	UINT                        uBattleRoomID;
	///混战场房间信息表
	char						szBattleGameTable[31];	

	///-------------------  end

	char			szRoomPassWord[61];						//房间密码	房间加密功能
	///服务器信息
	UINT						uRoomID;							///游戏服务器 ID 号码
	char						szGameRoomName[61];					///服务器名字
	
	///中心数据库信息
	char						szSQLIP[16];						///中心数据库地址
	char						szSQLName[61];						///中心数据库用户名字
	char						szSQLPass[61];						///中心数据库密码
	char						szSQLDataName[61];					///中心数据库名字

	///登陆数据库信息
	char						szLogonSQLIP[16];					///登陆数据库地址
	char						szLogonSQLName[61];					///登陆数据库用户名字
	char						szLogonSQLPass[61];					///登陆数据库密码
	char						szLogonSQLDataName[61];				///登陆数据库名字

	///本地数据库信息
	char						szNativeSQLIP[61];					///本地数据库地址
	char						szNativeSQLName[61];				///本地数据库用户名字
	char						szNativeSQLPass[61];				///本地数据库密码
	char						szNativeSQLDataName[61];			///本地数据库名字

	ManageInfoStruct()
	{
		//iSocketSecretKey = SECRET_KEY; 
	}
};

///内核数据 （服务器 DLL 组件启动设置）
struct KernelInfoStruct
{
	///游戏数据
	UINT						uNameID;							///游戏名字 ID 号码
	UINT						uDeskPeople;						///游戏人数


	UINT						uMinDeskPeople;						///部分游戏支持M-N个人游戏

	///内核数据
	BYTE						bMaxVer;							///软件最高版本
	BYTE						bLessVer;							///软件最低版本
	BYTE						bStartSQLDataBase;					///使用数据库连接
	BYTE						bNativeDataBase;					///使用本地数据库
	BYTE						bLogonDataBase;						///使用登陆数据库
	BYTE						bStartTCPSocket;					///是否启动网络
	UINT						uAcceptThreadCount;					///应答线程数目
	UINT						uSocketThreadCount;					///SOCKET 线程数目
};

/********************************************************************************************/

///列表项挂接数据类型
#define GLK_NO_DATA				0									///没有数据
#define GLK_GAME_KIND			1									///游戏类型
#define GLK_GAME_NAME			2									///游戏名字
#define GLK_GAME_ROOM			3									///游戏房间
#define GLK_GAME_USE			4									///游戏使用
#define GLK_GAME_WEB			5									///从INI读的WEB
#define GLK_GAME_ADD			12									///联系我们

//{add by rongqiufen 2010.7.2
#define GLK_GAME_KING_WEBGAME	6									///从bcf读的WEB
#define GLK_GAME_WEBGAME		7									///从bcf读的WEB
#define GLK_GAME_KING_EXEGAME	8									///从bcf读的WEB
#define GLK_GAME_EXEGAME		9									///从bcf读的WEB
#define GLK_GAME_SELLLIST		10									///销售列表
#define GLK_GAME_OPENINFO		11									///开放说明
//add by rongqiufen 2010.7.2}

///游戏列表辅助结构
struct AssistantHead
{
	UINT						uSize;								///数据大小
	UINT						bDataType;							///类型标识
};


///游戏类型de结构
struct ComKindInfo///加入游戏类型AddTreeData
{
	AssistantHead				Head;
	UINT						uKindID;							///游戏类型 ID 号码
	char						szKindName[61];						///游戏类型名字

	// duanxiaohui 20111107 游戏大厅二级目录功能
	UINT                        uParentKindID;                      ///父游戏类型ID号码
	// end duanxiaohui
};

///游戏名称结构
struct ComNameInfo
{
	AssistantHead				Head;
	UINT						uKindID;							///游戏类型 ID 号码
	UINT						uNameID;							///游戏名称 ID 号码
	char						szGameName[61];						///游戏名称
	char						szGameProcess[61];					///游戏进程名
	UINT						m_uOnLineCount;						///在线人数
	BYTE						m_byHotOrNew;						///热和新标志
	UINT						uVer;								///版本
	///
	///Kylin 20090202 游戏人数初始化问题
	ComNameInfo()
	{
		m_uOnLineCount=0;
		m_byHotOrNew = -1;
	}
	///

};

///游戏房间列表结构
struct ComRoomInfo
{
	AssistantHead				Head;
	UINT						uComType;							///游戏类型
	UINT						uKindID;							///游戏类型 ID 号码
	UINT						uNameID;							///游戏名称 ID 号码
	UINT						uRoomID;							///游戏房间 ID 号码
	UINT						uPeopleCount;						///游戏在线人数
	UINT						iUpPeople;							///比赛房间用户达到值后才开赛
	UINT						uDeskPeople;						///每桌游戏人数
	UINT						uDeskCount;							///游戏大厅桌子数目
	UINT						uServicePort;						///大厅服务端口
	char						szServiceIP[25];					///服务器 IP 地址
	char						szGameRoomName[61];					///游戏房间名称
	int							uVirtualUser;						///20081211 , Fred Huang
	int							uVirtualGameTime;					///20081211 , Fred Huang
	UINT						uVer;								///版本
	UINT						dwRoomRule;							///游戏房间规则
	bool                        bVIPRoom;                            ///< 金葫芦二代，是否VIP房间（VIP房间设置密码）

	int									iContestID;
	__int64								i64TimeStart;
	__int64								i64TimeEnd;

	//租赁房间ID与时间
	int									iLeaseID;
	__int64								i64LeaseTimeStart;
	__int64								i64LeaseTimeEnd;
	///wushuqun 2009.6.5
	///混战场房间ID       --begin
	UINT                        uBattleRoomID;
	///混战场房间信息表
	char						szBattleGameTable[31];	
	bool			bHasPassword;			// 有无密码	Add by lxl 2010-10-08
	UINT			dwTax;					// 房间房费	Add by wyl 11-5-16
	///
	///Kylin 20090202 游戏人数初始化问题

	ComRoomInfo()
	{
		uPeopleCount=0;
		bHasPassword = false;
	}
	///
};

///游戏大厅辅助结构
struct GameInsideInfo
{
	AssistantHead				Head;								///列表头
	UINT						uTrunID;							///转向 ID
	char						szDisplayName[61];					///显示名字
};

/********************************************************************************************/

///用户状态定义
#define USER_NO_STATE		 	0									///没有状态，不可以访问
#define USER_LOOK_STATE		 	1									///进入了大厅没有坐下
#define USER_SITTING		 	2									///坐在游戏桌
#define USER_ARGEE				3									///同意状态
#define USER_WATCH_GAME		 	4									///旁观游戏
#define USER_DESK_AGREE			5									///大厅同意
#define USER_CUT_GAME		 	20									///断线状态			（游戏中状态）
#define USER_PLAY_GAME		 	21									///游戏进行中状态	（游戏中状态）


static CString USER_STATESTRING[8] = {"", "空闲中", "坐下", "同意", "旁观", "大厅同意", "断线", "游戏中"};

///用户信息结构
struct UserInfoStruct
{
	long int					dwUserID;							///ID 号码
	long int					dwExperience;						///经验值
	int							dwAccID;							///ACC 号码
	int							dwPoint;							///分数
	__int64						i64Money;							///金币
	__int64						i64Bank;							///银行
	UINT						uWinCount;							///胜利数目
	UINT						uLostCount;							///输数目
	UINT						uCutCount;							///强退数目
	UINT						uMidCount;							///和局数目
	char						szName[61];							///登录名
	char						szClassName[61];					///游戏社团
	UINT						bLogoID;							///头像 ID 号码
	BYTE						bDeskNO;							///游戏桌号
	BYTE						bDeskStation;						///桌子位置
	BYTE						bUserState;							///用户状态
	BYTE						bMember;							///会员等级
	BYTE						bGameMaster;						///管理等级
	ULONG						dwUserIP;							///登录IP地址
	bool						bBoy;								///性别
	char						nickName[61];						///用户昵称
	UINT						uDeskBasePoint;						///设置的桌子倍数
	int							dwFascination;						///魅力
	int							iVipTime;							///会员时间
	int							iDoublePointTime;					///双倍积分时间
	int							iProtectTime;						///护身符时间，保留
	int							isVirtual;							///是否是扩展机器人 ///20081211 , Fred Huang
	UINT						dwTax;								 ///房费    add by wyl    11-5-16

	///玩家信息结构调整   yjj 090319
	char                        szOccupation[61];                    ///玩家职业
	char                        szPhoneNum[61];                      ///玩家电话号码
	char                        szProvince[61];                      ///玩家所在的省
	char                        szCity[61];                          ///玩家所在的市
	char                        szZone[61];                          ///玩家所在的地区
	bool                        bHaveVideo;                          ///是否具有摄像头

	///duanxiaohui 20111111  
	char						szSignDescr[128];			         ///个性签名
	///end duanxiaohui

	///wushuqun 2009.6.26
	///玩家类型信息
	///0 ,普通玩家
	///1 ,电视比赛玩家
	///2 ,VIP玩家
	///3 ,电视比赛VIP玩家
	int							userType;

	///作为扩展字段,为方便以后新加功能用
	///此处为以后平台中的新加功能需要修改用户信息结构时，不用重新编译所有游戏

	UINT                         userInfoEx1;    ///扩展字段1，用于邮游钻石身份作用时间，由黄远松添加
	UINT						 userInfoEx2;    ///扩展字段2，用于GM处理之禁言时效，由zxd添加于20100805

	int							 bTrader;		 //用于判断是不是银商  add by lxl 2010-12-10 


	///比赛专用
	int							iMatchID;	//比赛ID，唯一的标识一场比赛
	__int64						i64ContestScore;
	int							iContestCount;
	CTime						timeLeft;
	int							iRankNum;		//排行名次
	int							iRemainPeople;	//比赛中还剩下的人数
	int							iAward;			//比赛获取的奖励
	UserInfoStruct()
	{
		ZeroMemory(this, sizeof(UserInfoStruct));
	}
};

///用户信息结构 (小猪快跑)
struct UserInfoStructX
{
	long int					dwUserID;							///ID 号码
	long int					dwExperience;						///经验值
	int							dwAccID;							///ACC 号码
	int							dwPoint;							///分数
	__int64						i64Money;							///金币
	__int64						i64Bank;							///银行
	UINT						uWinCount;							///胜利数目
	UINT						uLostCount;							///输数目
	UINT						uCutCount;							///强退数目
	UINT						uMidCount;							///和局数目
	WCHAR						szName[64];							///登录名
	WCHAR						szClassName[64];					///游戏社团
	UINT						bLogoID;							///头像 ID 号码
	int							bDeskNO;							///游戏桌号
	int							bDeskStation;						///桌子位置
	int							bUserState;							///用户状态
	int							bMember;							///会员等级
	int							bGameMaster;						///管理等级
	ULONG						dwUserIP;							///登录IP地址
	BOOL						bBoy;								///性别
	WCHAR						nickName[64];						///用户昵称
	UINT						uDeskBasePoint;						///设置的桌子倍数
	int							dwFascination;						///魅力
	int							iVipTime;							///会员时间
	int							iDoublePointTime;					///双倍积分时间
	int							iProtectTime;						///护身符时间，保留
	int							isVirtual;							///是否是扩展机器人 ///20081211 , Fred Huang
	UINT						dwTax;								 ///房费    add by wyl    11-5-16

	///玩家信息结构调整   yjj 090319
	WCHAR                       szOccupation[64];                    ///玩家职业
	WCHAR                       szPhoneNum[64];                      ///玩家电话号码
	WCHAR                       szProvince[64];                      ///玩家所在的省
	WCHAR                       szCity[64];                          ///玩家所在的市
	WCHAR                       szZone[64];                          ///玩家所在的地区
	BOOL                        bHaveVideo;                          ///是否具有摄像头

	///duanxiaohui 20111111  
	WCHAR						szSignDescr[128];			         ///个性签名
	///end duanxiaohui

	///wushuqun 2009.6.26
	///玩家类型信息
	///0 ,普通玩家
	///1 ,电视比赛玩家
	///2 ,VIP玩家
	///3 ,电视比赛VIP玩家
	int							userType;

	///作为扩展字段,为方便以后新加功能用
	///此处为以后平台中的新加功能需要修改用户信息结构时，不用重新编译所有游戏

	UINT                         userInfoEx1;    ///扩展字段1，用于邮游钻石身份作用时间，由黄远松添加
	UINT						 userInfoEx2;    ///扩展字段2，用于GM处理之禁言时效，由zxd添加于20100805

	int							 bTrader;		 //用于判断是不是银商  add by lxl 2010-12-10 


///比赛专用
	__int64						i64ContestScore;
	int							iContestCount;
	CTime						timeLeft;
};

//用户信息结构(小猪快跑)
struct UserItemStructX
{
	BOOL							bMatchUser;							//是否比赛
	UINT							uSortIndex;							//排列索引
	UINT							uConnection;						//玩家关系
	long int						dwLogonTime;						//登陆时间
	UserInfoStructX					GameUserInfo;						//用户信息
};

//小猪快跑聊天消息
struct XzkpNormalTalk {
	int crColor;
	int iLength;
	int	dwSendID;
	int dwTargetID;
	int nDefaultIndex;
	WCHAR szMessage[108];
};

///用户规则结构
struct UserRuleStruct
{
	///变量定义
	BYTE						bSameIP;							///相同 IP
	BYTE						bIPLimite;							///不于IP前几位的玩家游戏
	BYTE						bPass;								///设置密码
	BYTE						bLimitCut;							///限制断线
	BYTE						bLimitPoint;						///限制分数
	BYTE						bCutPercent;						///逃跑率
	char						szPass[61];							///桌子密码
	long int					dwLowPoint;							///最低分数
	long int					dwHighPoint;						///最高分数 
};

struct SendUserMoneyByTimes         ///某种条件下，按次数赠送玩家游戏币
{	
	long int							dwUserID;							///用户 ID
	int                                 dwSrcGameMoney;                     ///原来的金额                    需要送进来前添加
	int                                 dwSrcMoneyInBank;                    ///银行里的金币
	int                                 dwSendGameMoney;                    ///赠送的金额  
	int                                 dwSendTimes;                        ///今天已经赠送的次数
	int                                 dwAllSendTImes;                     ///今天总共的赠送次数	
	int                                 dwSendFailedCause;                   ///赠送失败原因 0成功 
                                                                             ///1次数已满2未达到赠送标准
};

//自动赠送添加的结构体--2013-01-28 wangzhitu
typedef struct tag_RECEIVEMONEY
{
	bool      bISOpen;
	//bool      bISAuto;	
	__int64	  i64Money;
	__int64	  i64MinMoney;
	int		  iCount;
	int		  iTotal;
	int		  iTime;   
	int       iResultCode; 
	int       iLessPoint;
	struct tag_RECEIVEMONEY()
	{
		bISOpen = false;
		//bISAuto = false;		
		i64Money = 0;
		i64MinMoney = 0;
		iCount = 0;
		iTotal = 0;
		iTime  = 0;
		iResultCode = 0;
		iLessPoint  = 0;
	}
	
}RECEIVEMONEY;

// PengJiLin, 2011-4-15, 在线时长、局数送金币
struct SendMoneyOnTimesAndCounts
{
    int                                 dwUserID;
    int                                 dwGetMoney;             // 获赠的金币数
    int                                 dwMoneyOnTimes;         // 设置多少时长赠送的金币
    int                                 dwMoneyOnCounts;        // 设置多少局赠送的金币
    int                                 dwTimesNeed;            // 下一次差多少时长
    int                                 dwCountsNeed;           // 下一次差多少局
};

///wushuqun 2009.6.5
///混战房间记录结果
struct BattleRoomRecordRes
{
	///混战房间ID号
	///注：这里的房间ID 号是TGameRoomInfo 中的RoomID,而不是BattleRoomID
	///因为若BattleRoomID=RoomID时可能会有冲突

	UINT          uRoomID;   

	///记录结果
	UINT          uRecordResult;
};
/********************************************************************************************/


struct TLoginGameInfo
{
	int					iKindID;
	int					iGameID;
	int					iLoginCount;
	COleDateTime		oelLastTime;
};


#endif
