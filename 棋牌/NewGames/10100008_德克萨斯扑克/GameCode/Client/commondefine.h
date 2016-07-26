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



//控制消息定义
#define CM_USER_STATE					1						//状态改变
#define CM_USER_SEND_TIMES_MONEY       2                       //用户获得按次赠送的金币
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
#define ACT_USER_SEND_TIMES_MONEY      11                      //用户获得按次送的金币

#define SPLIT_WIDE							2//4						//拆分条宽度
#define WM_GAME_QUIT				WM_USER + 1001					//游戏结束
#define WM_REQUEST_PLAYERLIST		WM_USER + 1002				//向平台请求同桌玩家名单
#define WM_SEND_GAME_DATA			WM_USER + 1003				// 发送消息到服务器端
#define WM_SEND_GAME_DATA_NO_PARAM	WM_USER + 1004				// 发送消息到服务器端
#define WM_USE_PROP					WM_USER + 1005				// 用户从道具面板中使用道具
#define WM_PROP_BUY					WM_USER + 1006				// 用户从道具面板中购买道具
#define WM_GIVE_PROP				WM_USER + 1007				// 用户从道具面板中赠送道具
#define WM_USE_BOARDCAST			WM_USER + 1008				// 用户使用大小喇叭

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
	int iSendMoney;
	int iSendTimes;
	int iLastTimes;	
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
		uisMeUserInfo.GameUserInfo.bDeskStation = 1;

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
		_tcscpy(szProcessName, TEXT("zjh.ico"));					//进程名字
		uDeskPeople = 3;						//游戏人数
		uRoomID = 2;							//房间表示
		dwGameMSVer = 2;						//主版本号
		dwGameLSVer = 0;						//副版本号
		uComType = 1;							//游戏类型
		uNameID = 100600302;							//名字表示
		_tcscpy(szGameName, TEXT("扎金花"));						//游戏名字
		uLessPoint = 0;							//最少经验值
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
};


#endif // _COMMON_DEFINE_H
