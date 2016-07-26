#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE


//文件名字定义
#define GAMENAME						TEXT("五人梭哈")

//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//游戏信息
#define CLIENT_DLL_NAME					TEXT("12100305.ico")
#define SERVER_DLL_NAME					TEXT("12100305.DLL")
#define NAME_ID							12100305							//名字 ID
#define PLAY_COUNT						5							//游戏人数
#define SKIN_FOLDER						TEXT("sh")					//图片文件夹
#define SKIN							TEXT("cardskin1")					//两套图片中用一套

#define SKIN_CARD						TEXT("CardSkin")		//共用图片文件夹

#define MAX_NAME_INFO                   256

//游戏开发版本
#define DEV_HEIGHT_VERSION				8								//内部开发高版本号
#define DEV_LOW_VERSION					4								//内部开发低版本号

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_SEND_CARD					20				//发牌状态
#define GS_WAIT_BACK					21				//等待扣压底牌
#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 

//按注类型
#define TYPE_GIVE_UP					0x00			//放弃
#define TYPE_NOTE						0x01			//下注
#define TYPE_ADD						0x02			//加注
#define TYPE_FOLLOW						0x03			//跟注
#define TYPE_SHUTTLE					0x04			//梭哈


#define SHOWHA_TIME						2				//第三张牌便可以梭
//#define FIRST_CAN_GIVEUP				true			//第一把可放弃
#define NOTE_LIMIT						200				//下注上限





static TCHAR szTempStr[MAX_PATH] = {0};//暂存字符串，用来获取 dll_name 等的时候使用
#define GET_CLIENT_DLL_NAME(A)          (sprintf(A,"%d",NAME_ID),strcat(A,".ico")) //获取客户端 
#define GET_SERVER_DLL_NAME(A)          (sprintf(A,"%d",NAME_ID),strcat(A,".dll")) //获取服务器端 
#define GET_SKIN_FOLDER(A)              (sprintf(A,"%d",NAME_ID),strcat(A,""))     //获取图片文件夹
/********************************************************************************/
//#define SUPER_VERSION					//超级客户端
#define SET_JUDGE						//设置胜率败率

/********************************************************************************/

//游戏开始
struct GameBeginStruct
{
	bool				bUserState[PLAY_COUNT];			//玩家状态
	bool				bIsSuperUser;					//判断自己是否超级客户端：由服务器传来数据，每局都校验
	bool				bChangeRobotCard;				//是否改变了机器人的牌 true 改变了 false 没有改变
	BYTE				iBeenPlayGame;					//游戏回合
	__int64				iBaseNote;						//底注	
	__int64				iLimitNote;						//最大注数
	__int64				i64HalfLessMoney;				
};

//收集筹码动画
struct CollectJetonStruct
{
	__int64				JetonTotal;
	BYTE				bDeskStation;
};

//发牌数据包
struct SendCardStruct
{
	BYTE				iCard;							//牌标号
	BYTE				bDeskStatin;					//新一轮出牌者
};
//发牌数据包
struct SendOneTurnCardStruct
{
	BYTE    iStartPos;                    ///发牌起始位置
	BYTE    iCardList[PLAY_COUNT][5];     ///发牌
	BYTE    iCardCount[PLAY_COUNT];       /// 当前每个玩家发牌的张数
	int		iFirstSendCardCount;		  ///第一次发的牌张数
	SendOneTurnCardStruct()
	{
		memset(this,0,sizeof(SendOneTurnCardStruct));
		iStartPos = 255; 
	}
};
//发牌一轮扑克数据包
struct SendOneTurnCard
{
	BYTE                iTurnCard[PLAY_COUNT];     //一轮扑克数据
	BYTE                bStartPos;                 //起始位置
	SendOneTurnCard()
	{
		memset(iTurnCard,0,sizeof(iTurnCard));
		bStartPos = 255 ;
	}
};

//限注
struct LimitStruct
{
	BYTE				iNoteTime;						//下注次数
	__int64				iLimit;							//限注
};

//发牌結束数据包
struct SendCardFinishStruct
{
	BYTE				bDeskStatin;					//发牌完成
};

//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				bDeskStaton;					//出牌的位置
};

//发送用户结果
struct tagUserResult
{
	BYTE				bAddTimes;						//加倍标记
	BYTE				bNoteTimes;						//押注次数
	BYTE				iNowBigNoteStyle;				//下注类型
	BYTE				iOutPeople;						//下注者
	BYTE				m_byUserAddNum[PLAY_COUNT];		//本轮各个玩家加注次数 2011-12-13
	__int64				iNowBigNote;					//当前下注
	__int64				iTotalNote;						//总下注
	bool				bNewTurn;						//新一轮

	tagUserResult()
	{
		memset(m_byUserAddNum,0,sizeof(m_byUserAddNum));
		bNewTurn = false;
	}
};

//游戏结束统计数据包
struct GameEndStruct
{	
	BYTE				iCardShape[PLAY_COUNT];					//牌形
	BOOL				bShowCardShape[PLAY_COUNT];				//显示牌形
	int					iUserState[PLAY_COUNT];					//四家状态(提前放弃,还是梭)
	__int64				iTurePoint[PLAY_COUNT];					//玩家得分
	__int64				iChangeMoney[PLAY_COUNT];
	__int64				i64AcculateGrade[PLAY_COUNT];//玩家累积的成绩
	__int64				i64UpGrade[PLAY_COUNT];//上一局的成绩
};

//游戏提前结束包
struct GameAheadStruct
{
	BYTE				bDeskStation;					//胜者
	__int64				iTurePoint[PLAY_COUNT];					//庄家得分
	__int64				iChangeMoney[PLAY_COUNT];				//金币
};
//游戏结束统计数据包
struct GameCutStruct
{
	int					bDeskStation;					//退出位置
	__int64				iTurePoint[PLAY_COUNT];			//庄家得分
	__int64				iChangeMoney[PLAY_COUNT];
};

struct HaveThingStruct
{
	BYTE				pos;
	char				szMessage[61];
};

struct LeaveResultStruct
{
	BYTE				bDeskStation;
	int					bArgeeLeave;
};

struct MessageStruct							
{
	char				 Message[200];
};

struct StopResultStruct
{
	bool				bArgee;
};


//用户处理信息
struct tagUserProcess
{
	BYTE				iVrebType;					//所处理的按钮
	__int64				iNote;						//下注数
};

//代替断线用户处理信息
struct ReplaceUserNoteStruct
{
	BYTE				bDeskStation;				//代替的位置
	BYTE				iVrebType;					//所处理的按钮
	__int64				iNote;						//下注数
};

//机器人托管
struct UseAIStation
{
	BYTE				bDeskStation;				//坐号
	BOOL				bState;						//状态
};

//发送用户押注
struct NoteResult
{
	BYTE				bNoteStyle;					//下注类型
	BYTE				iOutPeople;					//下注者
	__int64				iCurNote;					//当前下注数
	__int64				iPlayerTotalNote[PLAY_COUNT];	//本局游戏中各玩家下注的总数  xyh 20110818
	
};

//结算
struct GameFinishNotify
{
	TCHAR				name[PLAY_COUNT][61];
	BYTE				iCardShape[PLAY_COUNT];		//牌型
	__int64			    iBasePoint;
	BYTE				iStyle;			
	__int64				iWardPoint[PLAY_COUNT];
	__int64				iMoney[PLAY_COUNT];

	GameFinishNotify()
	{
		memset(this,0,sizeof(GameFinishNotify));
	}
};
//colin90511牌信息
struct CardInfo
{
    BYTE                bDeskStation;              //座位号
	BYTE                iUserCard[5];               //五张牌值                 
};

struct GameStation_Base
{
	bool				bUserState[PLAY_COUNT];			//玩家状态
	bool				bIsSuperUser;					//判断自己是否超级客户端：由服务器传来数据，每局都校验
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				bGameStation;					//游戏状态
	//游戏信息
	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				iAllCardCount;					//扑克数目
	BYTE                byUserFollow;					//第几把必跟
	BYTE                bShowHa;                        //第几轮可以梭哈
	BYTE				m_byAddNum;					/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
	BYTE                bPK;                            ///是否PK
	int					iRoomBasePoint;					//房间倍数
	int				    iRunPublish;					//逃跑扣分
	__int64				iLimitNote;						//最高注
	__int64				iBaseNote;						//底注
	__int64             i64AcculateGrade[PLAY_COUNT];	//累积的成绩
	__int64             i64UpGrade[PLAY_COUNT];//上一局的成绩
	GameStation_Base()
	{
		memset(this,0,sizeof(GameStation_Base));
	}
};

//游戏状态数据包
struct GameStation_1 : GameStation_Base
{
	BYTE				bOtherdata;						//预留数据
	GameStation_1()
	{
		memset(this,0,sizeof(GameStation_1));
	}
};

//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_2 : GameStation_Base
{
	bool				bAgree[PLAY_COUNT];				//玩家是否准备
	BYTE				bOtherdata;						//预留数据
	GameStation_2()
	{
		memset(this,0,sizeof(GameStation_2));
	}
};

//游戏状态数据包	（ 游戏正在发牌状态 ）
struct GameStation_3 : GameStation_Base
{
	__int64				iTotalNote[PLAY_COUNT];			//用户当前下注
	int					iUserStation[PLAY_COUNT];		/**< 记录用户状态 */
	//运行状态变量
	int					iUpGradePeople;					//庄家位置
	BYTE				iBeenPlayGame;					//已经游戏的局数
	//状态信息
	BYTE				iUserCardCount[PLAY_COUNT];		//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][5];		//用户手上的牌
	GameStation_3()
	{
		memset(this,0,sizeof(GameStation_3));
	}
};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_4 : GameStation_Base	
{
	//运行状态变量
	int					iUpGradePeople;					//庄家位置
	int					iOutCardPeople;					//现在出牌用户
	int					iFirstOutPeople;				//先出牌的用户
	__int64				iTotalNote[PLAY_COUNT];			//用户当前下注
	__int64				iThisTurnNote[PLAY_COUNT];		//当前这轮下注
	
	BYTE				iBeenPlayGame;					//已经游戏的局数
	tagUserResult		tag;
	BYTE				iUserCardCount[PLAY_COUNT];		//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][5];		//用户手上的牌
	
	BOOL				ibNetCutPlayer[PLAY_COUNT];	    //断线玩家
	int					iUserStation[PLAY_COUNT];		/**< 记录用户状态 */
	BYTE				m_byUserAddNum[PLAY_COUNT];		//本轮各个玩家加注次数 2011-12-13
	GameStation_4()
	{
		memset(this,0,sizeof(GameStation_4));
	}
};

//游戏状态数据包	（ 等待下盘开始状态  ）
struct GameStation_5 : GameStation_Base
{
	int					iUserStation[PLAY_COUNT];		/**< 记录用户状态 */
	bool				bAgree[PLAY_COUNT];				//玩家是否准备
	//运行状态变量
	int					iUpGradePeople;					//庄家位置

	BYTE				iBeenPlayGame;					//已经游戏的局数
	GameStation_5()
	{
		memset(this,0,sizeof(GameStation_5));
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
	__int64 i64UpGrade;//上一局的成绩
	NotifyComeMes()
	{
		byDeskStation=255;
	}
};
//新一轮开始
struct NewTurn
{
	bool bFlag;
	NewTurn()
	{
		bFlag=true;
	}
};
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏
#define ASS_UG_USER_SET_CHANGE			51				//玩家设置更改
#define ASS_GAME_BEGIN					52				//游戏开始

#define ASS_SEND_CARD					53				//发牌信息
#define ASS_SEND_FINISH					54				//发牌完成
#define ASS_GAME_PLAY					55				//开始游戏
#define ASS_NOTE_LIMIE					56				//本轮限注

#define ASS_NOTE						60				//下注
#define ASS_VREB_CHECK					61				//用户处理
#define ASS_REPLACE_VREB_CHECK			62				//代替(断线)用户处理
#define ASS_NOTE_RESULT					63				//下注结果

#define ASS_COLLECT_JETON				70				//收集筹码
#define ASS_AI_STATION					71				//机器人托管

#define ASS_NEW_TURN					80				//新一轮开始
#define ASS_SEND_A_CARD					81				//新发一张牌

#define ASS_CONTINUE_END				90				//游戏结束
#define ASS_NO_CONTINUE_END				91				//游戏结束
#define ASS_CUT_END						92				//用户强行离开
#define ASS_SALE_END					93				//游戏安全结束
#define ASS_NO_WINER					94				//游戏结束无胜出者
#define ASS_AHEAD_END					95				//提前结束

#define ASS_HAVE_THING					100
#define ASS_LEFT_RESULT					101
#define ASS_MESSAGE						102
#define ASS_STOP_THING					103				//请求提前结束
#define ASS_AGREE_STOP					104
#define ASS_PLAY_FLASH                  105             //colin播放Flash动画
#define ASS_BEGIN_FLASH                 106 
#define ASS_LEAVE_DESK					107				//玩家离开游戏桌消息
#define ASS_COME_DESK					108				//玩家进入游戏桌
#define ASS_NEW_TURN					109				//新一轮游戏开始

/********************************************************************************/
#endif
