#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

////文件名字定义
//#ifdef VIDEO
//#define GAMENAME						TEXT("牛牛")
//#define NAME_ID							10900405							//名字 ID
//#define PLAY_COUNT						4								//游戏人数
//#else
//#ifdef TWO_NN
//#define GAMENAME						TEXT("牛牛(VIP)")
//#define NAME_ID							10900200							//名字 ID
//#define PLAY_COUNT						2								//游戏人数
//#else
//
//#endif
//#endif

#define GAMENAME						TEXT("通比牛牛")
#define NAME_ID							12100706							//名字 ID
#define PLAY_COUNT						6								//游戏人数
//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					"MatchTable"

//游戏信息
static TCHAR szTempStr[MAX_PATH] =		{0};	
#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录	                 // 共用图片文件夹
#define GET_SKIN_FOLDER(A)				(sprintf(A,"%d",NAME_ID),strcat(A,""))
#define SEC								1000			/**<一秒*/
#define USER_HAND_CARD					5				/**<用户手中牌数*/
#define ALL_CARD_COUNT					54				/**<使用牌总数*/

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_SEND_CARD					20				//发牌状态
#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 

#define MAX_SHAPE_COUNT                 15              //所有牌型种类
/********************************************************************************/

//游戏数据包
//#define SUPER_VERSION
/********************************************************************************/

///读取配制文件的数据
typedef struct IniDataStruct
{
	int		iBaseNote;					//游戏底分
	BYTE	byBTime;					//开始思考时间
	BYTE	bySCTime;					//游戏发牌时间
	BYTE	byTTime;					//出牌思考时间
	BYTE	byRTime;					//结算等待时间
	DWORD	iCardShape;					//牌型设置
	BYTE	iACCount;					//扑克数目

	IniDataStruct()
	{
		::memset(this,0,sizeof(IniDataStruct));
	}

	IniDataStruct operator=(IniDataStruct IDate)
	{
		this->iBaseNote = IDate.iBaseNote;
		this->byBTime = IDate.byBTime;
		this->bySCTime = IDate.bySCTime;
		this->byTTime = IDate.byTTime;
		this->byRTime = IDate.byRTime;
		this->iCardShape = IDate.iCardShape;
		this->iACCount = IDate.iACCount;
		return *this;
	}
}IniData;

typedef struct GameBaseData
{
	BYTE	bGameStation;				//游戏状态
	int		iRBPoint;					//房间倍数
	int		iRPublish;					//逃跑扣分
	bool	bIsInGame[PLAY_COUNT];		//玩家是否在游戏中
	bool	bHaveBaiPai[PLAY_COUNT];	//玩家是否摆牌
	bool	bIsAuto[PLAY_COUNT];		//玩家是否托管
	bool    bIsSuper;					//是否可以使用配置器

	GameBaseData()
	{
		bGameStation = GS_WAIT_SETGAME;
		iRBPoint = 0;
		iRPublish = 0;
		::memset(bIsInGame,false,sizeof(bIsInGame));
		::memset(bHaveBaiPai,false,sizeof(bHaveBaiPai));
		::memset(bIsAuto,false,sizeof(bIsAuto));
		bIsSuper = FALSE;
	}

	GameBaseData operator=(GameBaseData _BDate)
	{
		this->bGameStation = _BDate.bGameStation;
		this->iRBPoint = _BDate.iRBPoint;
		this->iRPublish = _BDate.iRPublish;
		this->bIsSuper = _BDate.bIsSuper;
		for(int i = 0; i < PLAY_COUNT; ++i)
		{
			this->bIsInGame[i] = _BDate.bIsInGame[i];
			this->bHaveBaiPai[i] = _BDate.bHaveBaiPai[i];
			this->bIsAuto[i] = _BDate.bIsAuto[i];		
		}

		return *this;
	}

}BData;

struct GameStationBase
{
	BData	sBData;				//游戏基本数据
};

//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_2 : public  GameStationBase
{
	IniData				s_Data;							//服务端基本数据
};

//游戏状态数据包	（ 游戏正在发牌状态 ）
struct GameStation_3 : public  GameStationBase
{
	IniData				s_Data;							//服务端基本数据

	int					iUCCount[PLAY_COUNT];			//玩家手牌数量

	BYTE				byUCard[USER_HAND_CARD];		//玩家手牌

	GameStation_3()
	{
		::memset(iUCCount,0,sizeof(iUCCount));
		::memset(byUCard,0,sizeof(byUCard));
	}
};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_4 : public  GameStation_3	
{
	IniData				s_Data;							//服务端基本数据

	BYTE                byDownCard[USER_HAND_CARD];		//玩家摆的牛牌

	int					iSurplusTime;					//剩余的时间

	GameStation_4()
	{
		::memset(byDownCard,0,sizeof(byDownCard));
		iSurplusTime = 0;
	}
};

//游戏状态数据包	（ 等待下盘开始状态  ）
struct GameStation_5 : public  GameStationBase
{
	IniData				s_Data;							//服务端基本数据
};


//游戏开始
struct BeginUpgradeStruct
{
	bool				bIsInGame[PLAY_COUNT];			//玩家是否在游戏中

	BeginUpgradeStruct()
	{
		::memset(bIsInGame,false,sizeof(bIsInGame));
	}
};

/// 发牌数据包，一次将扑克全部发给客户端
struct SendCardStruct
{
	BYTE		byUCard[PLAY_COUNT][USER_HAND_CARD];	//用户扑克
	int			iUCCount[PLAY_COUNT];

	SendCardStruct()
	{
		memset(byUCard,0,sizeof(byUCard));
		memset(iUCCount,0,sizeof(iUCCount));
	}
};

//用户处理信息
struct TanPaiStruct
{
	BYTE	byStaiton;							//摆牌玩家位置
	BYTE	byDownCard[USER_HAND_CARD];			//下墩牌
	BYTE	byShape;							//牌型


	TanPaiStruct()
	{
		byStaiton = 255;
		byShape = 255;
		::memset(byDownCard,0,sizeof(byDownCard));
	}
};

//游戏结束统计数据包
struct GameEndStruct
{
	bool				bIsPrepare[PLAY_COUNT];//是否准备
	bool				bIsCowUser[PLAY_COUNT];//是否牛牛玩家
	BYTE                byDownCard[PLAY_COUNT][USER_HAND_CARD];//下墩牌
	BYTE				byUCard[PLAY_COUNT][USER_HAND_CARD];//手牌
	BYTE				byCowUserNum;//牛牛玩家人数
	int					iCardShape[PLAY_COUNT];
	__int64				iMoney[PLAY_COUNT];//结算金币

	GameEndStruct()
	{
		::memset(iCardShape,-1,sizeof(iCardShape));
		::memset(bIsPrepare,0,sizeof(bIsPrepare));
		::memset(byDownCard,0,sizeof(byDownCard));
		::memset(byUCard,0,sizeof(byUCard));
		::memset(iMoney,0,sizeof(iMoney));
		::memset(iMoney,0,sizeof(iMoney));
		::memset(bIsCowUser, false, sizeof(bIsCowUser));
		byCowUserNum = 0;
	}
};

//托管数据包
struct AutoCardStruct
{
	BYTE				bDeskStation;				//玩家坐位号			
	bool				bAutoCard;					//是否托管;true:托管;false:取消托管

	AutoCardStruct()
	{
		::memset(this,0,sizeof(AutoCardStruct));
	}
};

//旁观数据包
struct WatchOtherStruct
{
	bool				bWatch;			/**<是否充许旁观*/			

	WatchOtherStruct()
	{
		memset(this,0,sizeof(WatchOtherStruct));
	}
};

//玩家强退数据包
struct UserQuitStruct
{
	BYTE				bDeskStation;				//玩家坐位号		
	UserQuitStruct()
	{
		memset(this,0,sizeof(UserQuitStruct));
	}
};

//超端设置数据包
struct SuperStruct
{
	BYTE				bSuper;				//超端玩家坐位号
	BYTE				bDeskStation;		//被设置玩家坐位号
	BYTE				bSuperTag;			//超端设置(0为初始值，1为赢，2为输)
	SuperStruct()
	{
		memset(this,0,sizeof(SuperStruct));
		bSuper = 255;
		bDeskStation = 255;
	}
};

/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏
#define ASS_BEGIN_UPGRADE				51				//游戏开始
#define ASS_SEND_CARD					52				//发牌信息
#define ASS_TAN_PAI						53				//摊牌消息
#define ASS_CONTINUE_END				65				//游戏结束
#define ASS_AUTO						66				/**<托管*/
#define ASS_SUPER						67				//超端设置消息

#define ASS_WATCH_OTHER					102				/**<旁观消息*/
#define ASS_USER_QUIT_GAME				103				/**<玩家强退*/

/********************************************************************************/
#endif
