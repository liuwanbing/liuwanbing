#pragma  once
// ***************************************************************
//  struct   version:  1.0   ·  date: 08/21/2008
//  -------------------------------------------------------------
//  外部和内部公用的结构体
//  -------------------------------------------------------------
//  Copyright (C) 2008 - All Rights Reserved
// ***************************************************************
// 如果要在结构中添加或减少字段，必须重新编译录像插件
// ***************************************************************

//把mdm 平台和金博平台的数据结构做了综合，最大化支持现有平台
//赋值过程能会比较麻烦，在平台里写两个转换函数来支持数据结构间的转换，如果没有字段选为 或 0
//
struct RecordUserInfoStruct
{
	long int					dwUserID;							//ID 号码
	long int					dwExperience;						//经验值	
	__int64						dwAccID;							//ACC 号码
	__int64						dwPoint;							//分数
	__int64						dwMoney;							//金币	
	UINT						uWinCount;							//胜利数目
	UINT						uLostCount;							//输数目
	UINT						uCutCount;							//强退数目
	UINT						uMidCount;							//和局数目
	TCHAR						szName[61];							//登录名	
	UINT						bLogoID;							//头像 ID 号码
	BYTE						bDeskNO;							//游戏桌号
	BYTE						bDeskStation;						//桌子位置
	BYTE						bUserState;							//用户状态
	BYTE						bMember;							//会员等级
	BYTE						bGameMaster;						//管理等级
	ULONG						dwUserIP;							//登录IP地址
	bool						bBoy;								//性别
	TCHAR						nickName[61];						//用户昵称
	//金博平台中独有的字段
	int                         iAgencyID;                          //用户的代理商编号
	long int					dwReputation;						//声誉值    //yjj  090223
	__int64					    dwMobileNum;						//手机号码   yjj   090223   for tonglou platform
	__int64                     dwScore;                            //当前积分   yjj   090223   for tonglou platform
	bool						bSayHello;							//是否欢迎此人
	BYTE                        bUserType;                          //玩家类型(普通玩家、特殊玩家)
	//mdm 平台中独有的字段
	int							dwBank;								//银行
	char						szClassName[61];					//游戏社团
	UINT						uDeskBasePoint;						//设置的桌子倍数
	int							dwFascination;						//魅力
	int							iVipTime;							//会员时间
	int							iDoublePointTime;					//双倍积分时间
	int							iProtectTime;						//护身符时间，保留
};
//用户信息
struct RecordUserItemStruct
{
	bool							bMatchUser;							//是否比赛
	UINT							uSortIndex;							//排列索引
	UINT							uConnection;						//玩家关系
	long int						dwLogonTime;						//登陆时间
	RecordUserInfoStruct			GameUserInfo;						//用户信息
};
//关于游戏的一些信息
struct RecordGameInfoStruct
{

	//record 组件加入的字段，必须赋值,客户端ico的名称
	TCHAR							szClientDLLName[31];				//ico 文件名字
	BYTE						    bMeDeskStation;	
	//目前所有平台都有的字段
	//共享信息
	bool							bDeskOnwer;							//是否台主
	BYTE							bGameStation;						//游戏状态
	//UserItemStruct					* pMeUserInfo;						//玩家信息
	long int						dwGamePower;						//用户权限
	long int						dwMasterPower;						//管理权限
	long int						dwRoomRule;							//设置规则


	//设置参数
	bool							bEnableSound;						//允许声音
	bool							bEnableWatch;						//允许旁观
	bool							bShowUserInfo;						//显示进出
	

	

	//大厅数据
	UINT							uDeskPeople;						//游戏人数
	UINT							uRoomID;							//房间表示
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	UINT							uComType;							//游戏类型
	UINT							uNameID;							//名字表示	
	TCHAR							szGameName[61];						//游戏名字
	UINT							uLessPoint;							//最少分数
	TCHAR							szKindName[100];					//类型名字
	TCHAR							szRoomName[100];					//房间名字


	int							uVirtualUser;						//20081211 , Fred Huang
	int							uVirtualGameTime;					//20081211 , Fred Huang
	bool                         bIsInRecord;                        //是否在录像中 yjj  090223  
	//金博平台特有的字段
	int                             iRoomSelcet; //進入不同底數房間
	UINT                            uTaiMoney;                         //选入房间的每台金额
	UINT                            uDiMoney;                          //选入房间的底金额

	//mdm平台特有字段
	bool							bEnableBackSound;					//背景音乐
};
//此消息应该是所有平台保持一致的，但本插件不会解析该消息。
