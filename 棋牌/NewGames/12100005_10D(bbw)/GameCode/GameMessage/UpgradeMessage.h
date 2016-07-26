#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义

#define GAMENAME						TEXT("十点半")
#define NAME_ID							12100005 						//名字 ID
#define PLAY_COUNT						5								//游戏人数
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
#define GS_ROB_NT						20              //抢庄状态
#define GS_NOTE                         21    //下底注状态
#define GS_SEND_CARD					22		//发牌状态
#define GS_PLAY_GAME					23	 //游戏中状态

#define GS_PLAY_FINISH					24   //游戏结束状态
#define GS_WAIT_NEXT					26	 //等待下一盘开始 


//按注类型
#define TYPE_GIVE_UP					0x00			//放弃
#define TYPE_NOTE						0x01			//下注
#define TYPE_TAKE_CARD                  0x02            //要牌
#define TYPE_STOP_CARD					0x03			//停牌
#define TYPE_PLAY_GAME                  0x04            //是玩家


#define MAX_SHAPE_COUNT                 6              //所有牌型种类
#define SH_USER_CARD					5               //牌的张数



/********************************************************************************/

//游戏数据包
/********************************************************************************/
/*------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------*/
typedef struct SuperUserSetData
{
	BYTE	byDeskStation;      /**< 玩家位置 */
	bool	bSetSuccese;		//是否设置成功了
	int		byMaxDesk[PLAY_COUNT];			//设置赢牌玩家的位置
	int		byMinDesk[PLAY_COUNT];			//设置输牌玩家的位置
	SuperUserSetData()
	{
		memset(this,255,sizeof(SuperUserSetData));
		bSetSuccese = false;
	}
};


/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 等待东家设置状态 ）
struct GameStation_Base
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号

	BYTE				byGameStation;					//游戏状态

	//游戏信息
	BYTE				iThinkTime;						//要牌时间
	BYTE				iBeginTime;						//准备时间
	BYTE				iSendCardTime;					//发牌时间
	BYTE				iCallScoreTime;					//叫庄时间
	BYTE				iXiaZhuTime;					//下注时间
	BYTE				byLostTime;						//定时器实际剩下的时间，用于表示断线重连时剩余时间值
	int					iRoomBasePoint;					//房间倍数
	int					iBaseNote;						//底注

	DWORD				iCardShape;						//牌型设置

	GameStation_Base()
	{
		memset(this,0,sizeof(GameStation_Base));
	}
};
/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_WaiteAgree : public GameStation_Base
{
	bool                bUserReady[PLAY_COUNT] ;        //玩家是否已准备

	GameStation_WaiteAgree()
	{
		memset(this,0,sizeof(GameStation_WaiteAgree));
	}
};
/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 叫庄状态 ）
struct GameStation_RobNt : public GameStation_Base
{
	BYTE			byCurrentCallScore;					//当前抢庄者 
	BYTE			byUserCallStation[PLAY_COUNT];		//各玩家抢庄情况	255-表示还没操作 0-表示不抢 1-表示已经抢了
	int				iUserStation[PLAY_COUNT];			//各玩家状态 标记是否中途加入的
	BYTE			byLostTime;							//定时器实际剩下的时间，用于表示断线重连时剩余时间值

	GameStation_RobNt()
	{
		memset(this,0,sizeof(GameStation_RobNt));
		memset(byUserCallStation,255,sizeof(byUserCallStation));
	}
};
/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 下注状态 ）
struct GameStation_Note : public GameStation_Base
{
	BYTE			byNtStation;							//庄家位置
	BYTE			byLostTime;								//定时器实际剩下的时间，用于表示断线重连时剩余时间值
	int				iUserStation[PLAY_COUNT];				//各玩家下注状态 -1-表示还没操作 0-表示不抢 1-表示已经抢了
	__int64			i64PerJuTotalNote[PLAY_COUNT];			//用户每局压总注
	__int64			iLimitNote[PLAY_COUNT];					//最大注数
	__int64			i64UserNoteLimite[PLAY_COUNT][4];		//玩家四个下注数字
	
	
	GameStation_Note()
	{
		memset(this,0,sizeof(GameStation_Note));
		memset(iUserStation,-1,sizeof(iUserStation));
	}
};

/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 发牌状态 ）
struct GameStation_SendCard : public GameStation_Base
{
	BYTE			byNtStation;							//庄家位置
	BYTE			byLostTime;								//定时器实际剩下的时间，用于表示断线重连时剩余时间值
	BYTE			iUserCardCount[PLAY_COUNT];				//用户手上扑克数目
	BYTE			iUserCard[PLAY_COUNT][SH_USER_CARD];	//用户手上的扑克
	int				iUserStation[PLAY_COUNT];				//各玩家状态 标记是否中途加入的
	__int64			i64PerJuTotalNote[PLAY_COUNT];			//用户每局压总注
	
	GameStation_SendCard()
	{
		memset(this,0,sizeof(GameStation_SendCard));
	}
};
/*------------------------------------------------------------------------------*/
//游戏状态数据包	（ 发牌状态 ）
struct GameStation_OpenCard : public GameStation_Base
{
	BYTE			byNtStation;							//庄家位置
	BYTE			byLostTime;								//定时器实际剩下的时间，用于表示断线重连时剩余时间值
	BYTE			iUserCardCount[PLAY_COUNT];				//用户手上扑克数目
	BYTE			iUserCard[PLAY_COUNT][SH_USER_CARD];	//用户手上的扑克
	BYTE			byOpenCardCount[PLAY_COUNT];				//用户手上已经亮牌的数目
	int				iOpenShape[PLAY_COUNT];					//牌型
	int				iUserStation[PLAY_COUNT];				//各玩家下注状态 -1-表示还没操作 0-表示不下注 1-表示已经下注
	__int64			i64PerJuTotalNote[PLAY_COUNT];			//用户每局压总注

	
	GameStation_OpenCard()
	{
		memset(this,0,sizeof(GameStation_OpenCard));
	}
};
/*------------------------------------------------------------------------------*/

//下注
struct UserNoteStruct
{
	int                 iNt;                               //庄家
	__int64				iLimitNote[PLAY_COUNT];				//最大注数
	__int64				i64UserNoteLimite[PLAY_COUNT][4];	//玩家四个下注数字
};


/*-------------------------------------------------------------------------------*/
/// 发牌数据包，一次将扑克全部发给客户端
struct SendAllCardStruct
{
	BYTE      iStartPos;                  //发牌起始位置
	BYTE      iUserCard[PLAY_COUNT][SH_USER_CARD];   //用户扑克
	BYTE      iUserCardCount[PLAY_COUNT];

	SendAllCardStruct()
	{
		memset(iUserCard,0,sizeof(iUserCard));
		memset(iUserCardCount,0,sizeof(iUserCardCount));
		iStartPos = 255;
	}
};
/*-------------------------------------------------------------------------------*/
//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;				//出牌的位置
};


//游戏结束统计数据包
struct GameEndStruct
{
	__int64				i64TurePoint[PLAY_COUNT];		//玩家得分
	__int64				iChangeMoney[8];
	int					iUserState[PLAY_COUNT];			//五家状态(提前放弃,还是梭)
	BYTE				iCardShape[PLAY_COUNT];			//牌型
	BYTE				iUpGradeStation;				//庄家位置
	BYTE				iCardList[PLAY_COUNT][5];		//扑克信息
	BYTE				byOpenCardCount[PLAY_COUNT];	//亮牌数量
};

//游戏结束统计数据包
struct GameCutStruct
{
	__int64				i64TurePoint[PLAY_COUNT];			//庄家得分
	__int64				iChangeMoney[8];
	int					bDeskStation;					//退出位置
	BYTE                iQuitType;                     //退出类型
};


//叫庄数据包
struct CallScoreStruct
{	
	BYTE				bDeskStation;							//当前叫庄者
	BYTE				byUserState[PLAY_COUNT];				//各玩家的状态(标记是否中途进入的 还是一直在游戏当中的玩家)
	int					iValue;									//叫庄类型（1叫分,0不叫分）
	bool 				bCallScoreflag;							//叫庄标记		
	bool				bPlayer;								//是否为玩家
	CallScoreStruct()
	{
		memset(this,0,sizeof(CallScoreStruct));
		memset(byUserState,255,sizeof(byUserState));
	}	
};
/*----------------------------------------------------------*/
//玩家要牌结构体
struct TakeCardStruct
{
	BYTE	byDeskStation;		//要牌玩家的位置	
	BYTE	byYaoPaiCount;		//要牌张数
	BYTE	byReadyYaoCard[PLAY_COUNT];//正在准备要牌的玩家
	int		iShape;				//牌型

	TakeCardStruct()
	{
		memset(this,0,sizeof(TakeCardStruct));
		byDeskStation = 255;
	}
};
//玩家停牌结构体
struct StopCardStruct
{
	BYTE	byDeskStation;		//停牌玩家的位置	
	BYTE	bReadyStopCard[PLAY_COUNT];//已经停牌的玩家	
	int		iShape;				//牌型

	StopCardStruct()
	{
		memset(this,0,sizeof(StopCardStruct));
		byDeskStation = 255;
	}
};

//用户下注处理信息
struct tagUserProcess
{
	BOOL bReadyStopCard[PLAY_COUNT];//已经停牌的玩家
	BYTE iVrebType;//所处理的按钮
	//BYTE bUpCard[3];
	int  iNoteType;//下注类型
	tagUserProcess()
	{
		memset(this,0,sizeof(tagUserProcess));
	}
};

//发送用户押注
struct NoteResult
{
	BYTE bAddStyle;		//下注类型
	BYTE iOutPeople;	//下注者
	__int64 iCurNote;		//当前玩家下注数
	BYTE NoHitStation;          //停牌位置
};
struct GameFinishNotify
{
	TCHAR name[PLAY_COUNT][21];
	int	iBasePoint;
	BYTE iStyle;			//游戏类型是否为强退,还是正常结束
	__int64	iWardPoint[PLAY_COUNT];
	//__int64 iTotalPoint[4];
	BYTE iCardShape[PLAY_COUNT];
	__int64 iMoney[PLAY_COUNT];
};


//发送超端控制信息
struct T_CS_SuperControlData
{
	BYTE	byWinUser[PLAY_COUNT];
	BYTE	byLoseUser[PLAY_COUNT];
	T_CS_SuperControlData()
	{
		memset(byWinUser,0xff,sizeof(byWinUser));
		memset(byLoseUser,0xff,sizeof(byLoseUser));
	}
};
//-------------------------------------------------------------------------------


/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏

#define ASS_CALL_SCORE					51				//通知叫分		
#define ASS_USER_SCORE					52				//玩家叫分		
#define ASS_CALL_SCORE_RESULT			53				//通知叫分结果	
#define ASS_CALL_SCORE_FINISH			54				//通知叫分结束	
#define ASS_CALL_NOTE					55				//通知下注		
#define ASS_USER_NOTE					56				//玩家下注		
#define ASS_CALL_NOTE_RESULT			57				//通知下注结果	
#define ASS_CALL_SEND_CARD				58				//通知发牌信息	
#define ASS_CALL_SEND_FINISH			59				//发牌完成		
#define ASS_GAME_PLAY					60				//开始游戏	
#define ASS_USER_TAKE_CARD              61				//玩家要牌
#define ASS_TAKE_CARD_RESULT			62				//玩家要牌结果	
#define ASS_USER_STOP					63				//玩家停牌		
#define ASS_USER_STOP_RESULT			64				//玩家停牌结果
#define ASS_CONTINUE_END				65				//游戏结束
#define ASS_NO_CONTINUE_END				66				//游戏结束
#define ASS_CUT_END						67				//用户强行离开
#define ASS_SALE_END					68				//游戏安全结束


#define ASS_SUPER_USER					79				//超端用户消息
#define ASS_SUPER_USER_SET				80				//超端用户设置消息
#define ASS_SUPER_USER_SET_RESULT		81				//超端用户设置结果消息

#define ASS_CONTROL_WIN					90       /// 超级客户端控制输赢
#define ASS_CAN_CONTROL_WIN				91       /// 通知超级客户端可以控制输赢
#define ASS_CONTROL_SUCCEED				92      /** 由服务端发送客户端超端设置成功消息 */







/********************************************************************************/
#endif
