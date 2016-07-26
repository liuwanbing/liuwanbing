#pragma once
#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE


#define GAMENAME						TEXT("510K")
#define NAME_ID							10100403						//名字 ID
#define PLAY_COUNT						3								//游戏人数

//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					"MatchTable"

#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录


//游戏开发版本
#define DEV_HEIGHT_VERSION				3								//内部开发高版本号
#define DEV_LOW_VERSION					10 								//内部开发低版本号

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_SEND							20				//发牌状态
#define GS_PLAY_GAME					21				//游戏中状态
#define GS_WAIT_NEXT					25				//等待下一盘开始 

#define CARD_NUMS						18				//每个玩家所拥有扑克数目


//服务端发给客服端数据包

struct GameStation
{

	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				iThinkTime;						//思考时间
	BYTE				iBeginTime;						//开始等待时间
	int					iGameStation;					//游戏状态
	int                 bUserReady[PLAY_COUNT] ;        //玩家是否已准备
	GameStation()
	{
		memset(this,0,sizeof(GameStation));
	}
};

struct GameStation_0
{

	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				iThinkTime;						//思考时间
	BYTE				iBeginTime;						//开始等待时间
	BYTE                bNextPeople;					//当前叫分者
	BYTE				bCardList[CARD_NUMS];			//手中的牌
	BYTE				bLeftCard[CARD_NUMS];			//要押的牌
	BYTE				bHavePoint[PLAY_COUNT][12];		//玩家有分牌
	BYTE				DeskBehave[PLAY_COUNT][CARD_NUMS];	//玩家表现
	BYTE                bRoundHaveCard[12];				//当轮有分牌
	int					iPeopleHandCount[PLAY_COUNT];	//玩家牌数量
    int					iGameStation;					//游戏状态
	int				    iClockLabel;					//时钟标签
	__int64				iPoint[PLAY_COUNT];				//玩家分数
	GameStation_0()
	{
		memset(this,0,sizeof(GameStation_0));
	}
};


//用户同意游戏
struct GameServerToClient_ArgeeGame
{
	BYTE								bDeskNO;							///游戏桌号
	BYTE								bDeskStation;						///位置号码
	BYTE								bAgreeGame;							///同意标志
	GameServerToClient_ArgeeGame()
	{
		memset(this,0,sizeof(GameServerToClient_ArgeeGame));
	}
};
//托管
struct GameServerToClient_Auto
{
	bool								bIsAuto;							//是否托管
	BYTE								bDeskStation;						///位置
	GameServerToClient_Auto()
	{
		memset(this,0,sizeof(GameServerToClient_Auto));
	}
};
/// 发牌数据包，一次将扑克全部发给客户端
struct GameServerToClient_SendCard
{
	BYTE      iUserCard[PLAY_COUNT][CARD_NUMS];   //用户扑克
	BYTE      iUserCardCount[PLAY_COUNT];		  //用户扑克张数
	BYTE      bNtPeople;						  //当前庄家
	GameServerToClient_SendCard()
	{
		memset(this,0,sizeof(GameServerToClient_SendCard));
		memset(iUserCard,255,sizeof(iUserCard));
		memset(iUserCardCount,0,sizeof(iUserCardCount));
	}
};

//游戏开始数据包
struct GameServerToClient_StartGame
{
	BYTE				iOutDeskStation;				//出牌起始位置
	GameServerToClient_StartGame()
	{
		memset(this,0,sizeof(GameServerToClient_StartGame));
		iOutDeskStation=255;
	}
};

//通知下一个玩家出牌数据包
struct GameServerToClient_NextPeople
{
	bool				bLeftFlag;					//上家是否出牌
	BYTE				bNextStation;				//要出牌的玩家
	BYTE				bLeftStation;				//上家
	BYTE				bCardList[CARD_NUMS];		//上家所出的牌
	BYTE				bHaveCard[12];				//当轮有分牌
	int					iPeopleCount[PLAY_COUNT];	//当前玩家牌数量
	__int64				i64PeoplePoint[PLAY_COUNT];			//当前玩家得分

	GameServerToClient_NextPeople()
	{
		memset(this,0,sizeof(GameServerToClient_NextPeople));
		bNextStation=255;
	}
};
//客服端显示得分牌
struct GameServerToClient_StyleCard
{
	BYTE				bDeskStation;				//当轮赢家
	BYTE				bCardList[12];				//当轮赢家所赢的牌
	GameServerToClient_StyleCard()
	{
		memset(this,0,sizeof(GameServerToClient_StyleCard));
		bDeskStation=255;
	}
};
//最后一名摊牌
struct GameServerToClient_ShowCard
{
	BYTE				bLastStation;				//当轮最后一名
	BYTE				bRemainCard[CARD_NUMS];		//最后一名剩余的牌
	GameServerToClient_ShowCard()
	{
		memset(this,0,sizeof(GameServerToClient_ShowCard));
		bLastStation=255;
	}
};
//结束
struct GameServerToClient_Finish
{
	__int64	         i64Point[PLAY_COUNT];		//玩家得分
	BYTE			 iRanking[PLAY_COUNT];		//排名
	GameServerToClient_Finish()
	{
		memset(this,0,sizeof(GameServerToClient_Finish));
		
	}
};
//断线结束
struct GameServerToClient_CutEnd
{
	bool bFlag;
	GameServerToClient_CutEnd()
	{
		memset(this,0,sizeof(GameServerToClient_CutEnd));

	}
};
//查看此轮桌面牌分
struct GameServerToClient_SeeStyle
{
	__int64	         i64Point;		        //此轮桌面牌分
	BYTE			 bStyleCard[12];       //此轮有分牌
	GameServerToClient_SeeStyle()
	{
		memset(this,0,sizeof(GameServerToClient_SeeStyle));
		
	}
};
//客服端发给服务器数据包
//用户出牌数据包
struct GameClientToServer_OutCardMsg
{
	int					iNextDeskStation;				//下一出牌者
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iCardList[39];					//扑克信息
};
//按钮数据包
struct GameClientToServer_Button
{
	bool		bFlag;						//是否自己操作(标志位根据按钮类型而有不同意义)
	BYTE		bDeskStation;				//点击者
	int			iBtType;					//按钮类型
	int			iBtParam;					//按钮参数
	int			iCardCount;					//所出的牌数量
	BYTE		bCardList[CARD_NUMS];		//所出的牌
	GameClientToServer_Button()
	{
		memset(this,0,sizeof(GameClientToServer_Button));
	}
};
//游戏消息(客服端给服务端)
#define CLIENTTOSERVER_BUTTON					100				//按钮消息
#define CLIENTTOSERVER_AGREE                    101             //同意游戏		
//游戏消息(服务端给客服端)
#define SERVERTOCLIENT_AGREE					200					//发牌消息
#define SERVERTOCLIENT_SEND						201					//发牌消息
#define SERVERTOCLIENT_START					202					//开始游戏消息
#define SERVERTOCLIENT_NEXT					    203					//通知下一个玩家出牌消息
#define SERVERTOCLIENT_SHOW					    204					//展示得分消息
#define SERVERTOCLIENT_NORMALFINISH				205					//游戏正常结束消息
#define SERVERTOCLIENT_AUTO					    206					//托管消息
#define SERVERTOCLIENT_HANDUP					207					//最后一名摊牌消息
#define SERVERTOCLIENT_CUTEND					208					//断线结束
//点击消息
#define CLICK_START								500				//开始
#define CLICK_OPERATE							501				//操作
#define CLICK_OUTCARD							502				//出牌
#define CLICK_NOOUT							    503				//不出
#define CLICK_TIP							    504				//提示
#define CLICK_AUTO							    505				//托管,查看牌分，排序操作
#define CLICK_SET							    506			    //游戏设置
#endif
