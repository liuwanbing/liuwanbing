#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义

#define GAMENAME						TEXT("四人牛牛")
#define NAME_ID							12100604						//名字 ID
#define PLAY_COUNT						4								//游戏人数
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
#define GS_NOTE                         21     //下底注状态
#define GS_SEND_CARD					22		//发牌状态
#define GS_OPEN_CARD					24	//摆牛状态
#define GS_WAIT_NEXT					23	//等待下一盘开始 



//按注类型
#define TYPE_GIVE_UP					0x00			//放弃
#define TYPE_RESET                      0x01            //要牌
#define TYPE_OPEN                       0x02            //开牌
#define TYPE_BULL						0x03			//牛
#define TYPE_PLAY_GAME                  0x04            //是玩家
#define TYPE_NOTE						0x06			//下注

#define MAX_SHAPE_COUNT                 15              //所有牌型种类
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
	BYTE	byMaxDesk;			//设置最大玩家的位置
	BYTE	byMinDesk;			//设置最小玩家的位置
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
	BYTE				iThinkTime;						//摊牌时间
	BYTE				iBeginTime;						//准备时间
	BYTE				iSendCardTime;					//发牌时间
	BYTE				iCallScoreTime;					//叫庄时间
	BYTE				iXiaZhuTime;					//下注时间

	BYTE				iAllCardCount;					//扑克数目

	int					iRoomBasePoint;					//房间倍数
	int					iBaseNote;						//底注

	DWORD				iCardShape;						//牌型设置
	__int64             i64AcculateGrade[PLAY_COUNT];	//累积的成绩
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
	int				iUserStation[PLAY_COUNT];				//各玩家下注状态 -1-表示还没操作 0-表示不抢 1-表示已经抢了
	BYTE			iUserCardCount[PLAY_COUNT];				//用户手上扑克数目
	BYTE			iUserCard[PLAY_COUNT][SH_USER_CARD];	//用户手上的扑克

	BYTE			byOpenUnderCount[PLAY_COUNT];			//底牌张数
	BYTE			byOpenUnderCard[PLAY_COUNT][3];			//底牌的三张牌
	BYTE			byOpenUpCard[PLAY_COUNT][2];			//升起来的2张牌
	int				iOpenShape[PLAY_COUNT];					//摆牛牌型

	__int64			i64PerJuTotalNote[PLAY_COUNT];			//用户每局压总注
	
	GameStation_OpenCard()
	{
		memset(this,0,sizeof(GameStation_OpenCard));
	}
};
/*------------------------------------------------------------------------------*/

//游戏开始
struct BeginUpgradeStruct
{
	BYTE				AllCardCount;					//一共使用的牌数	
	__int64				iLimitNote[PLAY_COUNT];						//最大注数
	__int64				i64UserNoteLimite[PLAY_COUNT][4];	//玩家四个下注数字
	int                 iNt;                            //庄家
	BeginUpgradeStruct()
	{
		memset(this,0,sizeof(BeginUpgradeStruct));
	}
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
	BeginPlayStruct()
	{
		memset(this,0,sizeof(BeginPlayStruct));
	}
};








//游戏结束统计数据包
struct GameEndStruct
{
	int					iUserState[PLAY_COUNT];					//四家状态(提前放弃,还是梭)

	BYTE				iCardShape[PLAY_COUNT];
	__int64				iTurePoint[PLAY_COUNT];					//庄家得分
	__int64				iChangeMoney[8];
	BYTE				iUpGradeStation;				//庄家位置
	BYTE                iUpBullCard[PLAY_COUNT][3];      //升起的牌
	BYTE				iCardList[PLAY_COUNT][10];					//扑克信息
	__int64				i64AcculateGrade[PLAY_COUNT];//玩家累积的成绩
	GameEndStruct()
	{
		memset(this,0,sizeof(GameEndStruct));
	}
};

//游戏结束统计数据包
struct GameCutStruct
{
	int					bDeskStation;					//退出位置
	__int64					iTurePoint[PLAY_COUNT];					//庄家得分
	__int64				iChangeMoney[8];
	BYTE                iQuitType;                     //退出类型
	GameCutStruct()
	{
		memset(this,0,sizeof(GameCutStruct));
	}
};


//叫分数据包
struct CallScoreStruct
{	

	BYTE				bDeskStation;							//当前叫分者
	int					iValue;									//叫分类型（1叫分,0不叫分）
	bool 				bCallScoreflag;							//叫分标记		
	bool				bPlayer;
	BYTE				byUserState[PLAY_COUNT];				//各玩家的状态(标记是否中途进入的 还是一直在游戏当中的玩家)
	CallScoreStruct()
	{
		memset(this,0,sizeof(CallScoreStruct));
		memset(byUserState,255,sizeof(byUserState));
	}
	
};
/*----------------------------------------------------------*/
//玩家摊牌结构体
struct UserTanPai
{
	BYTE	byDeskStation;		//摊牌玩家的位置
	BOOL	bReadyOpenCard[PLAY_COUNT];//已经摊牌的玩家
	BYTE	byUnderCount;		//底牌张数
	BYTE	byUnderCard[3];		//底牌的三张牌
	BYTE	byUpCard[2];		//升起来的2张牌
	int		iShape;				//摆牛牌型

	UserTanPai()
	{
		memset(this,0,sizeof(UserTanPai));
		byDeskStation = 255;
	}
};

//用户处理信息
struct tagUserProcess
{
	BOOL bReadyOpenCard[PLAY_COUNT];//已经摊牌的玩家
	BYTE iVrebType;//所处理的按钮
	BYTE bUpCard[3];
	int  iNoteType;//下注数
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
	NoteResult()
	{
		memset(this,0,sizeof(NoteResult));
	}
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
	GameFinishNotify()
	{
		memset(this,0,sizeof(GameFinishNotify));
	}
};
//离开桌消息包
struct LeaveDeskMes
{
	BYTE byDeskStation;
	LeaveDeskMes()
	{
		byDeskStation=255;
	}
};
//离开桌消息包
struct NotifyComeMes
{
	BYTE byDeskStation;
	__int64 i64AcculateGrade;//玩家累积的成绩
	NotifyComeMes()
	{
		byDeskStation=255;
	}
};
//-------------------------------------------------------------------------------


/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏

#define ASS_CALL_SCORE					51				//通知叫分		DWJ
#define ASS_USER_SCORE					52				//玩家叫分		DWJ
#define ASS_CALL_SCORE_RESULT			53				//通知叫分结果	DWJ
#define ASS_CALL_SCORE_FINISH			54				//通知叫分结束	DWJ
#define ASS_CALL_NOTE					55				//通知下注		DWJ
#define ASS_USER_NOTE					56				//玩家下注		DWJ
#define ASS_CALL_NOTE_RESULT			57				//通知下注结果	DWJ
#define ASS_CALL_SEND_CARD				58				//通知发牌信息	DWJ
#define ASS_CALL_SEND_FINISH			59				//发牌完成		DWJ
#define ASS_CALL_OPEN					60				//通知开牌摆牛	DWJ
#define ASS_USER_OPEN					61				//玩家摆牛		DWJ
#define ASS_CALL_OPEN_RESULT			62				//玩家摆牛结果	DWJ



#define ASS_CONTINUE_END				65				//游戏结束
#define ASS_NO_CONTINUE_END				66				//游戏结束
#define ASS_CUT_END						67				//用户强行离开
#define ASS_SALE_END					68				//游戏安全结束
#define ASS_LEAVE_DESK					69				//玩家离开游戏桌消息
#define ASS_COME_DESK					70				//玩家进入游戏桌




#define ASS_SUPER_USER					79				//超端用户消息
#define ASS_SUPER_USER_SET				80				//超端用户设置消息
#define ASS_SUPER_USER_SET_RESULT		81				//超端用户设置结果消息







/********************************************************************************/
#endif
