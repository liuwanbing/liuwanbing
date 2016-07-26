#pragma once

//同楼 普通版/视频版 编译  release/视频版  ，提牌器编译提牌器版

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#define GAMENAME						TEXT("扯旋")


#define NAME_ID							12100006						//名字 ID


//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//游戏信息

#define PLAY_COUNT						6								//游戏人数
#define MAX_CARD_COUNT                  4


//游戏内部开发版本号
#define DEV_HEIGHT_VERSION				3				//内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					1				//内部开发低版本号

//#define SKIN_FOLDER						TEXT("ddz")	//图片文件夹
//#define SKIN							TEXT("cardskin1")	//两套图片中用一套
#define SKIN_CARD						TEXT("cardskin")		//共用图片文件夹
static TCHAR szTempStr[MAX_PATH] = {0};//暂存字符串，用来获取 dll_name 等的时候使用
#define GET_CLIENT_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))
#define GET_SERVER_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))
#define GET_SKIN_FOLDER(A)              (sprintf(A,"%d",NAME_ID),strcat(A,""))     //获取图片文件夹
#define GET_KEY_NAME(A, B, C, D)			(sprintf(A,"%s%d%s", B, C, D),strcat(A,""))
#define GET_CLIENT_BCF_NAME(A)          (sprintf(A,"%d",NAME_ID),strcat(A,".bcf"))    

#define SUPER_PLAYER                    10                         //超级玩家
#define MAX_NAME_INFO                   256

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_AGREE					1				//等待同意设置
#define GS_SEND_CARD					20				//发牌状态
#define GS_WAIT_BACK					21				//等待扣压底牌
#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 


//----------------------------------------------------------------------
#define GS_SET_BOBO_TIME						0x00			//设置簸簸时间
#define GS_SEND_CARD_TIME						0x01			//发牌时间
#define GS_PLAY_GAME_TIME						0x02			//玩游戏时间
#define GS_SPARE_CARD_TIME						0x03			//分牌时间
//----------------------------------------------------------------------
//按注类型




#define TYPE_OPENCARD					0x04			//开牌
#define TYPE_LOOKCARD					0x05			//看牌
#define TYPE_NORMAL                     0x06            //正常状态

#define TYPE_OPEN_LOSE                  0x06            //开牌失败
#define TYPE_OPEN_WIN                   0x07            //开牌失败
#define TYPE_REACH_LIMITE_NEXT          0x08            //下一家下注后达到上限
#define TYPE_REACH_LIMITE_SELF          0x09            //自己下注后达到上限
#define TYPE_COMPARE_CARD               0x09            //比牌操作

//按注类型
#define TYPE_FOLLOW						0x00			//跟
#define TYPE_ALL_NOTE					0x01			//敲
#define TYPE_ADD						0x02			//大
#define TYPE_PAUSE						0x03			//休
#define TYPE_GIVE_UP					0x04			//丢
/********************************************************************************/

//游戏数据包

/********************************************************************************/
//发送用户结果
struct tagUserResult
{
	BYTE iNowBigPeople;
	BYTE iCutOutLastpeople;
	BYTE iOutPeople;			//下注者
	int iTotalNote;				//总下注
    int iUserStation[PLAY_COUNT] ;//玩家下注类型
	int iFirstOutPeople;         //第一位置玩家
	__int64 iUserNote[PLAY_COUNT];   //用户本轮下注
};

struct GameStationBase
{
	BYTE bGameStation;
};
//游戏状态数据包	（ 等待东家设置状态 ）

struct GameStation_1 : GameStationBase
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
};

//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_2 : GameStationBase
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	//游戏信息
	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				bySeparateCardTime;				//分牌时间
	BYTE				byBoBoTime;						//设置簸簸时间
	bool                bUserReady[PLAY_COUNT] ;		///玩家准备
	//   房间相关信息
	__int64					i64BoBase;						//底皮
	__int64					i64SmallBobo;					//最小簸簸
};

//游戏状态数据包	（ 游戏正在发牌状态 ）
struct GameStation_3 : GameStationBase
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	BYTE				iBeginTime;						//开始游戏时间
	BYTE				byBoBoTime;						//设置簸簸时间
	BYTE				iThinkTime;						//思考时间
	BYTE				bySeparateCardTime;				//分牌时间
	BYTE				iTimeRest;						//实际余下时间
	//状态信息
	BYTE				byGameStation;									//状态小分解
	BYTE				byAlreadySendCardCount[PLAY_COUNT];				//已经发的牌
	BYTE				iUserCardCount[PLAY_COUNT];						//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][MAX_CARD_COUNT];			//用户手上的牌	
	//运行状态变量
	int					iUpGradePeople;						//庄家位置
	bool				bAlreadSetBobo[PLAY_COUNT];		//设置簸簸数是否已经可以了
	bool                bPlayer[PLAY_COUNT];
	__int64				i64Bobo[PLAY_COUNT];			//用户所设置的簸簸数值
	__int64				i64BoBase;						//底皮
	__int64				i64SmallBobo;					//最小簸簸
};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_4 : GameStationBase
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	//游戏信息
	BYTE				iBeginTime;						//开始时间
	BYTE				byBoBoTime;						//设置簸簸时间
	BYTE				iThinkTime;						//思考时间
	BYTE				bySeparateCardTime;				//分牌时间
	BYTE                bNtPeople;                      //庄家位置
	BYTE				iUpGradePeople;					//下注位置
	BYTE				iOutCardPeople;					//现在出牌用户
	BYTE				iFirstOutPeople;				//先出牌的用户
	BYTE				byGameStation;					//动作小分解
	BYTE				byOperationState[PLAY_COUNT];	//用记的操作
	BYTE				iUserCardCount[PLAY_COUNT];					//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][MAX_CARD_COUNT];		//用户手上的牌
	BYTE				byAlreadySendCardCount[PLAY_COUNT];			//已经发的牌
	BYTE				byNoteTime[PLAY_COUNT];						//下注的次数
	int                 iCurNote;									//上一位玩家下注值，已暗注计算
	int				    iTimeRest;									// 定时器实际剩下的时间，用于表示断线重连时剩余时间值
	bool                bPlayer[PLAY_COUNT];	
	//运行状态变量
	__int64				i64NoteMoney[PLAY_COUNT][3];	//每次所下的注信息
	__int64				iTotalNote[PLAY_COUNT];			//用户当前下注
	__int64             iThisGameNote[PLAY_COUNT];		
	__int64				iTotalGameNote[PLAY_COUNT];		//本局游戏下注数
	__int64				i64Bobo[PLAY_COUNT];			//用户所设置的簸簸数值
	__int64				i64BoBase;						//底皮
	__int64				i64SmallBobo;					//最小簸簸
	tagUserResult		tag;
};

//游戏状态数据包	（ 等待下盘开始状态  ）
struct GameStation_5 : GameStationBase
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	//游戏信息
	BYTE				iBeginTime;							//开始时间
	BYTE				byBoBoTime;							//簸簸时间
	BYTE				iThinkTime;							//思考时间
	BYTE				bySeparateCardTime;					//分牌时间
	
	//运行状态变量
	int					iUpGradePeople;					//庄家位置
	__int64				i64BoBase;						//底皮
	__int64				i64SmallBobo;					//最小簸簸
};

//用户同意游戏
struct UserArgeeGame
{
	BYTE				iBeginStation;					//开始位置
	BYTE				iEndStation;					//结束位置
	BYTE				iPlayCount;						//游戏盘数
	BYTE				iCardCount;						//扑克数目
};

//游戏开始
struct BeginUpgradeStruct
{
	bool				bPlay[PLAY_COUNT];				//记录是否是中途加入的
	BYTE				AllCardCount;					//一共使用的牌数
	BYTE                bNtStation;                     //庄家
	__int64				i64BoBase;						//底皮
	__int64				i64SmallBobo;					//最小簸簸
	__int64				i64BoboData[PLAY_COUNT];		//簸簸数据
};
//收集筹码动画
struct CollectJetonStruct
{
	int JetonTotal;
	BYTE bDeskStation;
};
//发牌数据包
struct SendCardStruct
{
	BYTE				bCard[PLAY_COUNT][MAX_CARD_COUNT];					//牌标号
	BYTE                bCardCount[PLAY_COUNT];                             //牌张数
	BYTE				byNoteTime[PLAY_COUNT];								//第几次下注
	__int64				i64BoboMoney[PLAY_COUNT];
	__int64				i64dNoteData[PLAY_COUNT][3];						//所下的筹码数据
	SendCardStruct()
	{
		memset(bCard,0,sizeof(bCard));
		memset(bCardCount,0,sizeof(bCardCount));
		memset(i64BoboMoney,0,sizeof(i64BoboMoney));
		memset(byNoteTime,0,sizeof(byNoteTime));
		memset(i64dNoteData,0,sizeof(i64dNoteData));
	}
};


//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;							//出牌的位置
	BYTE                bNtPeople;									 //庄家位置
	BYTE				byUserOperatorType[PLAY_COUNT];				//玩家的操作
	__int64				i64dBobo[PLAY_COUNT];						//用户的簸簸数
	BeginPlayStruct()
	{
		memset(byUserOperatorType,0xff,sizeof(byUserOperatorType));
		memset(i64dBobo,0x00,sizeof(i64dBobo));
	}
};

//通知发牌
struct T_Notice_SendCard
{
	BYTE bySendCardFirstDesk;							//发牌的起始位置
	BYTE bySendCardCount;								//要发牌的张数
};

//游戏结束统计数据包
struct T_SC_GameEndStruct
{
	BYTE				byFinishType;							//结束类型
	__int64				i64TotalNote[PLAY_COUNT];				//用户所下的筹码总数
	__int64				i64WinMoney[PLAY_COUNT];				//玩家所赢得的金币数
	__int64				i64NetIncome[PLAY_COUNT];				//净收入
	T_SC_GameEndStruct()
	{
		memset(i64TotalNote,0x00,sizeof(PLAY_COUNT));
		memset(i64WinMoney,0x00,sizeof(PLAY_COUNT));
		memset(i64NetIncome,0x00,sizeof(PLAY_COUNT));
	}
};

//游戏结束统计数据包
struct GameCutStruct
{
	int						bDeskStation;								//退出位置
	__int64					iTurePoint[PLAY_COUNT];					//庄家得分
	__int64					iChangeMoney[8];
};

struct HaveThingStruct
{
	BYTE pos;
	char szMessage[61];
};

struct LeaveResultStruct
{
	BYTE bDeskStation;
	int bArgeeLeave;
};

struct MessageStruct	//客户端到服务器
{
	char Message[200];
};

struct StopResultStruct
{
	bool bArgee;
};
//配牌数据包
struct SetCardStruct
{
	BYTE bCardCount;//牌总数
	BYTE bPlayCount;//玩家数
	BYTE bPlayCard[8][60]; // 每个玩家的牌
	int  nPlayCardCount[8]; //每个玩家配牌数
	SetCardStruct()
	{
		memset(this,0,sizeof(SetCardStruct));
	}
};

//用户处理信息
struct tagUserProcess
{
	BYTE iVrebType;		//所处理的按钮
	__int64  iNote;		//下注数
	tagUserProcess()
	{
		memset(this,0,sizeof(tagUserProcess));
	}
};

struct tagCompareCard :  public tagUserProcess 
{
	BYTE byComparedStation ;  //被对比牌的玩家   
};

//代替断线用户处理信息
struct SupersedeUserNote
{
	BYTE bDeskStation;//代替的位置
	BYTE iVrebType;//所处理的按钮
	int  iNote;		//下注数
};
//換底牌數據結果
struct ChangeBackStruct
{
	BYTE bDeskStation;//位置
	BYTE bSourceCard;//源牌
	BYTE bResultCard;//結果牌
	BOOL bExtVal;	//是否替換成功
};

//发送用户押注
struct NoteResult
{
	BYTE iOutPeople;									//本次下注者
	BYTE bNextDeskStation;								//下一下玩家
	BYTE byOperatType;									//所操作的类型
	BYTE byNoteTime;									//第几次加注
	BYTE byFirstOutPeople;								//第一个出牌的人
	__int64	i64FrontNote;								//前面人下注的值
	__int64	i64NoteMoney;								//下注值
};
struct GameFinishNotify
{
	TCHAR name[PLAY_COUNT][61];
	__int64				i64TotalNote[PLAY_COUNT];				//用户所下的筹码总数
	__int64				i64WinMoney[PLAY_COUNT];				//玩家所赢得的金币数
	__int64				i64NetIncome[PLAY_COUNT];				//净收入
};

//设置簸簸结构体
struct tSetBoboStruct
{
	BYTE byMyDesk;
	__int64 i64BoboMoney;
	tSetBoboStruct()
	{
		byMyDesk = 0xff;
		i64BoboMoney = 0x00;
	}
};

//发牌结束结构体
struct tSendOneCardFinsh
{
	BYTE iOutDeskStation;								//当前要操作之人
	BYTE byUserOperatorType[PLAY_COUNT];				//玩家的操作
	tSendOneCardFinsh()
	{
		iOutDeskStation = 0x00;
		memset(byUserOperatorType,0xff,sizeof(byUserOperatorType));
	}
};

//通知可以分牌
struct tNoticeSeparetCard
{
	BYTE bySeparetCard;
	tNoticeSeparetCard()
	{
		bySeparetCard = 0xff;
	}
};

//分牌结构体
struct T_CS_SeparatCard
{
	BYTE byDeskStation;						//分牌桌位号
	BYTE bySeparatCard[MAX_CARD_COUNT];		//分牌的数据
	BYTE byLeftCardShap;					//左边牌型
	BYTE byRightCardShap;					//右边牌型
	T_CS_SeparatCard()
	{
		byDeskStation = 0xff;
		byLeftCardShap = 0xff;
		byRightCardShap = 0xff;
		memset(bySeparatCard,0x00,sizeof(bySeparatCard));
	}
};

//分牌成功消息
struct T_SC_SeparatSuccess
{
	BYTE byDeskStation;
	T_SC_SeparatSuccess()
	{
		byDeskStation = 0xff;
	}
};

//分牌完成数据包
struct T_SC_SeparatFinish
{
	BYTE  bySeparatCard[PLAY_COUNT][MAX_CARD_COUNT];			//牌数据
	BYTE  byCardCount[PLAY_COUNT];
	T_SC_SeparatFinish()
	{
		memset(&bySeparatCard,0xff,sizeof(bySeparatCard));
		memset(byCardCount,0x00,sizeof(byCardCount));
	}
};
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏
#define ASS_BEGIN_UPGRADE				51				//升级游戏开始(设置簸簸数)
#define ASS_SEND_CARD					52				//发牌信息
#define ASS_SEND_FINISH					54				//发牌完成
#define ASS_GAME_PLAY					59				//开始游戏
#define ASS_NEW_TURN					64				//新一轮开始
#define ASS_CONTINUE_END				65				//游戏结束
#define ASS_NO_CONTINUE_END				66				//游戏结束
#define ASS_CUT_END						67				//用户强行离开
#define ASS_SALE_END					68				//游戏安全结束
#define ASS_EARLY_END					69				//提前结束

#define ASS_VREB_CHECK					179				//用户处理
#define ASS_SET_BOBO_DATA				180				//设置簸簸数
#define ASS_CS_SEPARAT_CARD				181				//分牌消息
#define ASS_SC_SEPARAT_CARD				182				//分牌成功消息

#define ASS_SHUTTLE						80				//梭了
#define ASS_ADD_NOTE					82				//加注
#define ASS_GIVE_UP						86				//放弃
#define ASS_MODIFY_PLAY_NOTE			90				//更新用户所下注
#define ASS_NOTE						91				//下注
#define ASS_GM_SUPERCLIENT				100				//超級客戶端           
#define ASS_AI_WIN                      102             //机器人必赢

#define ASS_SET_TEST_CARD               120             //发牌器设定牌

#define ASS_SC_NOTICE_SEND_CARD			121				//通知发牌
#define ASS_SC_SEND_ONE_CARD_FINSH		122				//发一张牌结束
#define ASS_SC_NOTIC_SEPARET_CARD		123				//通知可以分牌
#define ASS_SC_SHOW_ALL_SEPARAT_CARD	124				//通知所有玩家分牌完成
/********************************************************************************/
#endif
