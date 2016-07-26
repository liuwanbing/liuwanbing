#pragma once
#include<vector>
#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#ifdef VIDEO
#define GAMENAME						TEXT("牛牛")
#define NAME_ID							10900405							//名字 ID
#define PLAY_COUNT						4								//游戏人数
#else
#ifdef TWO_NN
#define GAMENAME						TEXT("牛牛(VIP)")
#define NAME_ID							10900200							//名字 ID
#define PLAY_COUNT						2								//游戏人数
#else
#define GAMENAME						TEXT("21点")
#define NAME_ID							10500400							//名字 ID
#define PLAY_COUNT						4								//游戏人数
#endif
#endif
//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					"MatchTable"
//游戏信息

#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录
#define SKIN_CARD						TEXT("CardSkin")		                 // 共用图片文件夹


//游戏开发版本
#define DEV_HEIGHT_VERSION				3								//内部开发高版本号
#define DEV_LOW_VERSION					10 								//内部开发低版本号

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_BET			                20              //下注状态
#define GS_SEND							21				//发牌状态
#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 



const int iNote[18]=
{
1,2,5,10,20,50,100,200,500,1000,2000,5000,10000,20000,50000,100000,200000,500000
};

const int iDouble[12]={1,5,10,50,100,500,1000,5000,10000,50000,100000,500000};

//游戏数据包
/********************************************************************************/
//游戏状态数据包 （游戏等待开始状态）
struct GameStation_1	
{

	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				bGameStation;					//游戏状态
	BYTE				bGameBeginWaitTime;				//游戏开始等待时间
	BYTE				bGameBetTime;					//游戏下注时间
	BYTE                bCallCardTime;					//游戏叫牌时间
	int					bUserReady[PLAY_COUNT] ;		//玩家准备状态
	GameStation_1()
	{
		memset(this,0,sizeof(GameStation_1));
	}
};

//游戏状态数据包	（ 游戏下注状态 ）
struct GameStation_2
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				bGameStation;					//游戏状态
	BYTE				bGameBeginWaitTime;				//游戏开始等待时间
	BYTE				bGameBetTime;					//游戏下注时间
	BYTE                bCallCardTime;					//游戏叫牌时间
	int	                bUserReady[PLAY_COUNT] ;		//玩家准备状态
	GameStation_2()
	{
		memset(this,0,sizeof(GameStation_2));
	}
};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_3
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				bGameStation;					//游戏状态
	BYTE				bGameBeginWaitTime;				//游戏开始等待时间
	BYTE				bGameBetTime;					//游戏下注时间
	BYTE                bCallCardTime;					//游戏叫牌时间
	int	                bUserReady[PLAY_COUNT] ;		//玩家准备状态

	GameStation_3()
	{
		memset(this,0,sizeof(GameStation_3));
	}
};

//游戏状态数据包	（ 等待下一盘开始状态 ）
struct GameStation_4	
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				bGameStation;					//游戏状态
	BYTE				bGameBeginWaitTime;				//游戏开始等待时间
	BYTE				bGameBetTime;					//游戏下注时间
	BYTE                bCallCardTime;					//游戏叫牌时间
	int	                bUserReady[PLAY_COUNT] ;		//玩家准备状态
	GameStation_4()
	{
		memset(this,0,sizeof(GameStation_4));
	}

};
struct AgreeGame
{
	BOOL flag;									//包标识
	BOOL bUserReady[PLAY_COUNT];				//玩家是否准备
	AgreeGame()
	{
		memset(this,0,sizeof(AgreeGame));
	}
};
//按钮数据包
struct GameStationButton
{
	BYTE		bDeskStation;				//点击者
	int			iBtType;					//按钮类型
	int			iBtParam;					//按钮参数
	__int64		i64Note;					//下注数量
	GameStationButton()
	{
		memset(this,0,sizeof(GameStationButton));
	}
};
//庄家消息包
struct GameMessageNt
{
	BOOL flag;				//发送庄家标识
	BYTE bNtStation;		//庄家位置
	GameMessageNt()
	{
		memset(this,0,sizeof(GameMessageNt));
		bNtStation=255;
	}
};
//下注消息包
struct GameMessageBet
{
	BOOL        flag;			//下注标示
	BYTE		bDeskStation;	//座位号
	__int64		i64Not;			//筹码值
	__int64		i64BetLimit[PLAY_COUNT];	//下注上限
	__int64		i64Bet[PLAY_COUNT][3];		//下注筹码
	GameMessageBet()
	{
		memset(this,0,sizeof(GameMessageBet));
		bDeskStation=255;
	}
};
//发牌消息包
struct GameMessageSendCard
{
	BOOL flag;					//发牌标识
	BYTE bFirstPeople;			//第一个发牌者
	BYTE bUserCard[PLAY_COUNT][2];		//用户扑克
	GameMessageSendCard()
	{
		memset(this,0,sizeof(GameMessageSendCard));
		bFirstPeople=255;
	}
};
//发牌消息包
struct GameMessageStart
{
	BOOL flag;					//发牌标识
	BYTE bFirstScoreStation;	//第一个叫分者
	GameMessageStart()
	{
		memset(this,0,sizeof(GameMessageStart));
		bFirstScoreStation=255;
	}
};
//游戏安全结束数据包
struct GameMessageEnd
{
	BOOL flag;					//发牌标识
	GameMessageEnd()
	{
		memset(this,0,sizeof(GameMessageEnd));
	}
};
//游戏正常结束数据包
struct GameMessageNormalFinish
{
	BOOL		flag;								//发牌标识
	int			iUserPoint[PLAY_COUNT];			//玩家点数
	__int64		i64UserScore[PLAY_COUNT];		//玩家输赢金币
	GameMessageNormalFinish()
	{
		memset(this,0,sizeof(GameMessageNormalFinish));
	}
};
//下注结果
struct GameMessageBetResult
{
	BYTE		bDeskStation;		//当前下注位置
	__int64		i64Note;			//下注金币
	GameMessageBetResult()
	{
		memset(this,0,sizeof(GameMessageBetResult));
	}
};

//要牌消息包
struct GameMessageNeedCard
{
	bool bFlag;							//能否要牌
	__int64	i64Note;					//加倍的下注
	int  iStyle;						//要牌类型
	int  iPoints;						//要牌点数
	BYTE bDeskStation;					//要牌位置
	BYTE bNextStation;					//下一个要牌位置
	BYTE bCard;							//所要牌的数据
	GameMessageNeedCard()
	{
		memset(this,0,sizeof(GameMessageNeedCard));
		bFlag=true;
	}
};

//-------------------------------------------------------------------------------
/// 通知客户端是否为超级客户端状态消息结构
typedef struct SuperUserState
{
	BYTE byDeskStation;      /**< 玩家位置 */
	bool bEnable;           /**< 是否开通 */
	SuperUserState()
	{
		memset(this,0,sizeof(SuperUserState));
	}
}SUPERSTATE;

//超端用户控制的结果
struct	SuperUserControlReslut
{
	bool    bySuperReslut;		//超端用户控制
	BYTE	byDeskStation;		//超端用户座位号
	SuperUserControlReslut()
	{
		memset(this,0,sizeof(SuperUserControlReslut));

	}
};
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/
//游戏消息
#define GAME_BUTTON					100				//按钮消息
#define ASS_AGREE					101				//下注消息
#define ASS_NT						102				//庄家
#define ASS_BET						103				//下注消息
#define ASS_SEND					104				//发牌消息
#define ASS_PLAY					105				//开始游戏
#define ASS_BETRESULT				106				//下注结果
#define ASS_SAFEEND					107				//游戏安全结束
#define ASS_CALL					108				//叫牌消息
#define ASS_CALLFINISH				109				//叫分消息
#define ASS_NORMALFINISH			110				//游戏正常结束
#define ASS_RESTART					111				//重新开始游戏
/********************************************************************************/



//点击消息
#define CLICK_START                 500				//开始
#define CLICK_OPERATE               501				//操作
#define CLICK_BET					502				//下注
#define CLICK_BETOPERATE			503				//下注操作
#define CLICK_PLAYOPERATE			504				//游戏中操作


//服务器发送消息的辅助标识

#define SERVER_EXTRA_DOUBLE							500			//加倍
#define SERVER_EXTRA_CLICK_GIVEUP                   501			//放弃
#define SERVER_EXTRA_CLICK_NEED					    502			//要牌
#define SERVER_EXTRA_CLICK_STOP			            503			//停牌
#define SERVER_EXTRA_CLICK_TIMEEND                  504			//时间到下一个人叫分

#define ASS_SUPER_USER					70				//超端用户消息
#define ASS_SUPER_RESULT				71				//超端用户控制结果消息包
#endif
