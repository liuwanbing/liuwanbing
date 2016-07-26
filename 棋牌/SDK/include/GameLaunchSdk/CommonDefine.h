///////////////////////////////////////////////////////////////////////////////
// 公共定义头文件
// 把多处要用到的定义放到这个文件里来，以避免循环包含头文件的情况

#ifndef _COMMON_DEFINE_H
#define _COMMON_DEFINE_H

//等级判断函数
extern EXT_CLASS TCHAR * GetGameOrderNameInExe(__int64 dwPoint);
// 输出调试信息到文件
extern EXT_CLASS void DebugPrintf(const char *p, ...);
extern EXT_CLASS void DebugPrintH(const void *buf, int nSize);
/// 获取指定数值的字符串
/// 本函数同时支持UNICODE和ANSI
/// @param[out] str 字符串数组，须由调用者分配空间
/// @param[in] nNum 须显示的整数数值
/// @param[in] nPower 以10的几次幂显示，如123显示成123000，则此参数为3
/// @return 返回str所在的位置
///
extern EXT_CLASS TCHAR *GlbGetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);



//控制消息定义
#define CM_USER_STATE					1						//状态改变
#define CM_USER_SEND_TIMES_MONEY		2						//用户获得按次赠送的金币
//动作代码
#define ACT_USER_UP						1						//用户起来
#define ACT_WATCH_UP					2						//旁观起来
#define ACT_USER_SIT					3						//用户坐下
#define ACT_WATCH_SIT					4						//旁观坐下
#define ACT_USER_CUT					5						//用户断线
#define ACT_USER_AGREE					6						//用户同意
#define ACT_GAME_BEGIN					7						//游戏开始
#define ACT_GAME_END					8						//游戏结束
#define ACT_USER_POINT					9						//用户经验值
#define ACT_USER_CHARM					10						//用户魅力值
#define ACT_USER_SEND_TIMES_MONEY		11						//用户获得按次送的金币
#define ACT_USER_BANKMONEY				14						//银行总金币数的更新
#define CM_USER_PROP					12						//游戏道具使用消息

#define	IPC_USER_PROP_RESULT			13						//道具消息

//JianGuankun 2012-1-12 跑马灯消息
#define	IPC_TROTTING					17						//跑马灯消息
//end

#define		IPC_EX_SKIN						18	

#define ACT_USER_CONTESTPOINT			15
#define ACT_USER_QUEUE_DISMISS			18						//排队机解散
#define ACT_USER_AVATAR_CHANGE			19						//玩家形象更新
#define ACT_CONTEST_GAMEOVER			20						//比赛结束
#define ACT_CONTEST_KICK				21						//比赛玩家被淘汰
#define ACT_CONTEST_WAIT_GAMEOVER		22

#define SPLIT_WIDE							2//4						//拆分条宽度
#define WM_GAME_QUIT				WM_USER + 1001					//游戏结束
#define WM_REQUEST_PLAYERLIST		WM_USER + 1002				//向平台请求同桌玩家名单
#define WM_SEND_GAME_DATA			WM_USER + 1003				// 发送消息到服务器端
#define WM_SEND_GAME_DATA_NO_PARAM	WM_USER + 1004				// 发送消息到服务器端
#define WM_USE_PROP					WM_USER + 1005				// 用户从道具面板中使用道具
#define WM_PROP_BUY					WM_USER + 1006				// 用户从道具面板中购买道具
#define WM_GIVE_PROP				WM_USER + 1007				// 用户从道具面板中赠送道具
#define WM_USE_BOARDCAST			WM_USER + 1008				// 用户使用大小喇叭
#define WM_SET_STATION				WM_USER + 1009				// 设置游戏状态



// PengJiLin, 2010-10-13, 商店道具的即时购买功能
#define WM_PROP_BUY_NEW			WM_USER + 1010
// VIP购买    11-5-21     add wyl;
#define WM_BUY_VIP				WM_USER + 1011				// VIP购买    //add wyl;

#define WM_PROP_BUYANDUSE				WM_USER + 1022				// VIP购买    //add wyl;



// PengJiLin, 2011-6-24, 激活消息
#define WM_ACTIVE_TO_ROOM       WM_USER + 1012

#define WM_FRIEND				WM_USER + 1013				// 添加好友
#define WM_FRIEND_R				WM_USER + 1014				// 添加好友
#define WM_GETFRIENDLIST		WM_USER + 1015				// 获取好友列表
#define WM_WINDOW_MOVE			WM_USER + 1016              // 	
#define WM_SCREEN_SIZE_CHANGE	WM_USER + 1017              // 屏幕分辨率变化
// duanxiaohui 2011-11-11 加入黑名单
#define WM_EMENY                WM_USER + 1020              // 加入黑名单
#define WM_EMENY_R              WM_USER + 1021              // 加入黑名单    
// end duanxiaohui

//新版银行窗口消息
//Add by JianGuankun 2011-12-30
#define WM_BANK2_CLOSE				WM_USER + 1030				//关闭银行消息
#define WM_BANK2_TEST				WM_USER + 5500				//关闭银行消息
#define WM_BANK2_CHECK_SUC			WM_USER + 1031			//银行存取成功
//End Add

struct SendGameDataStruct
{
	BYTE bMainID;
	BYTE bAssistantID;
	BYTE bHandleCode;
	void *pData;
	UINT uBufLen;
};
struct SendGameDataStructWithoutParam
{
	BYTE bMainID;
	BYTE bAssistantID;
	BYTE bHandleCode;
};

#define IDM_GETPOINT						WM_USER + 430				//积分
#define IDM_GET_ROOMNAME					WM_USER+435					//得到房间名称
#define IDM_CLOSE_GAME						WM_USER+436					//告诉父窗口已经关闭
//消息定义
#define WM_HIT_LOGO				WM_USER+30								//单击头像

//定时器
#define ID_GIF_TIMER					50						//gif动画
#define ID_GIF_TIMER2					51						//gif动画


//用户状态改变
struct CM_UserState_Change_ForExe
{
	BYTE								bActionCode;			//动作代码
	BYTE								bDeskStation;			//桌子位置
	UserItemStruct						uisUserItem;			//用户指针
};
//send to game talking
struct CM_UserState_Send_Times_Money_For_Exe
{
	int dwUserID;
    int                                 dwGetMoney;             // 获赠的金币数
    int                                 dwMoneyOnTimes;         // 设置多少时长赠送的金币
    int                                 dwMoneyOnCounts;        // 设置多少局赠送的金币
    int                                 dwTimesNeed;            // 下一次差多少时长
    int                                 dwCountsNeed;           // 下一次差多少局
};

// 游戏信息
struct GameInfoStructInExe
{
	GameInfoStructInExe()
	{
		bDeskOnwer = true;							//是否台主
		bGameStation = 0;						//游戏状态
		memset(&uisMeUserInfo, 0, sizeof(UserItemStruct));			//玩家信息
		_tcscpy(uisMeUserInfo.GameUserInfo.nickName, TEXT("张三"));
		_tcscpy(uisMeUserInfo.GameUserInfo.szName, TEXT("张三丰"));
		uisMeUserInfo.GameUserInfo.dwUserID = 100001;
		uisMeUserInfo.GameUserInfo.bDeskNO = 3;
		uisMeUserInfo.GameUserInfo.bDeskStation = 255;

		dwGamePower = 0;						//用户权限
		dwMasterPower = 0;						//管理权限
		dwRoomRule = 0;							//设置规则
		pOrderName = GetGameOrderNameInExe;						//等级函数

		//设置参数
		bEnableSound = true;						//允许声音
		bEnableWatch = false;						//允许旁观
		bShowUserInfo = true;						//显示进出
		bEnableBackSound = true;					//背景音乐

		//大厅数据
		_tcscpy(szProcessName, TEXT("10901800.exe"));					//进程名字
		uDeskPeople = 180;						//游戏人数
		uRoomID = 2;							//房间表示
		dwGameMSVer = 2;						//主版本号
		dwGameLSVer = 0;						//副版本号
		uComType = 1;							//游戏类型
		uNameID = 10901800;							//名字表示
		//uNameID = 10003300;							//名字表示
		_tcscpy(szGameName, TEXT("斗地主"));		//游戏名字
		uLessPoint = 0;							//最少经验值
		memset(szGameRoomName, 0, sizeof(szGameRoomName));

		iBasePoint = 1;
		iLowCount = 0;
		i64Chip = 0;
		i64TimeStart = 0;
		i64TimeEnd = 0;
		i64LowChip = 0;
	}
	//IFindUserInterface				* pIFindUser;						//查找接口
	//共享信息
	bool							bDeskOnwer;							//是否台主
	BYTE							bGameStation;						//游戏状态
	UserItemStruct					uisMeUserInfo;						//玩家信息
	long int						dwGamePower;						//用户权限
	long int						dwMasterPower;						//管理权限
	long int						dwRoomRule;							//设置规则
	GetOrderName					* pOrderName;						//等级函数

	//设置参数
	bool							bEnableSound;						//允许声音
	bool							bEnableWatch;						//允许旁观
	bool							bShowUserInfo;						//显示进出
	bool							bEnableBackSound;					//背景音乐

	//大厅数据
	TCHAR							szProcessName[31];					//进程名字
	UINT							uDeskPeople;						//游戏人数
	UINT							uRoomID;							//房间表示
	DWORD							dwGameMSVer;						//主版本号
	DWORD							dwGameLSVer;						//副版本号
	UINT							uComType;							//游戏类型
	UINT							uNameID;							//名字表示
	TCHAR							szGameName[61];						//游戏名字
	UINT							uLessPoint;							//最少经验值

	bool                            bIsInRecord;                        //是否在录像中 yjj  090223


	int								iBasePoint;
	//比赛专用
	int									iLowCount;
	__int64								i64Chip;
	__int64								i64TimeStart;
	__int64								i64TimeEnd;
	__int64								i64LowChip;

	char						szGameRoomName[61];					//游戏房间名称
};


#endif // _COMMON_DEFINE_H
