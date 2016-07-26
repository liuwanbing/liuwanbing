#pragma once

//同楼 普通版/视频版 编译  release/视频版  ，提牌器编译提牌器版

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#define GAMENAME						TEXT("扎金花")


#define NAME_ID							50300260								//名字 ID


//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//游戏信息

#define PLAY_COUNT						6								//游戏人数
#define MAX_CARD_COUNT                  3


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
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_SEND_CARD					20				//发牌状态
#define GS_WAIT_BACK					21				//等待扣压底牌
#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 

//按注类型

#define TYPE_GIVE_UP					0x00			//放弃
//#define TYPE_NOTE_100					0x01			//100
//#define TYPE_NOTE_200					0x02			//200
//#define TYPE_NOTE_400					0x03			//400注
#define TYPE_NOTE						0x01			//下注
#define TYPE_ADD						0x02			//加注
#define TYPE_FOLLOW						0x03			//跟注
//#define TYPE_SHUTTLE					0x04			//扎金花
#define TYPE_OPENCARD					0x04			//开牌
#define TYPE_LOOKCARD					0x05			//看牌



#define TYPE_OPEN_LOSE                  0x06            //开牌失败
#define TYPE_OPEN_WIN                   0x07            //开牌失败
#define TYPE_REACH_LIMITE_NEXT          0x08            //下一家下注后达到上限
#define TYPE_REACH_LIMITE_SELF          0x09            //自己下注后达到上限
#define TYPE_COMPARE_CARD               0x09           //比牌操作

/********************************************************************************/

//游戏数据包

/********************************************************************************/

//发送用户结果
struct tagUserResult
{
	//	BOOL bShowOnly;				//仅显示
	//BOOL bAddFlag;				//加倍标记
	BYTE bCountNotePeople;		//当前未放弃玩家数
	int bAddTime;				//押注次数
	BYTE iNowBigNoteStyle;		//下注类型
	BYTE iNowBigPeople;
	BYTE iCutOutLastpeople;
	BYTE iCutOutLastType;
	BYTE iOutPeople;			//下注者
	int iNowBigNote;			//当前下注
	int iTotalNote;				//总下注
    int iUserStation[PLAY_COUNT] ;//玩家下注类型
	int iFirstOutPeople;         //第一位置玩家
	int iUserNote[PLAY_COUNT];   //用户本轮下注

	bool bIsFirstNote;
	
	//	int iCurNote;		//当前下注数
};
//看牌数据
struct lookCardResult
{
   BYTE             bDeskStation;
   BYTE				iUserCardCount;				//用户手上的牌数
   BYTE				iUserCard[5];				//用户手上的牌
 //  bool  ming[PLAY_COUNT];
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
	//	char				pUpDateHttp[50];
	//游戏信息
	//	BYTE				iCardCount;						//扑克数目
};

//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_2 : GameStationBase
{
	BYTE                bEXFlag[12];                    //zht 2011-4-12, 不使用的字段，占位用
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	//游戏信息
	//	BYTE				iBeginStation;					//开始位置
	//	BYTE				iEndStation;					//结束位置
	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				iAllCardCount;						//扑克数目
	int					iRoomBasePoint;					//房间倍数
	int					iRunPublish;					//逃跑扣分
	BYTE                bEXFlag1[12];                    //zht 2011-4-12, 不使用的字段，占位用
	
	//   房间相关信息
	int					iLimitNote;						//下注封顶值   明注 / 暗注
	int					iBaseNote;						//底注	       暗注
	int                 iGuoDi;                         //锅底
	int                 iLimtePerNote;                   //            暗注上限，明注需要 * 2

	BOOL                bShowIdOrPlay;

};

//游戏状态数据包	（ 游戏正在发牌状态 ）
struct GameStation_3 : GameStationBase
{
	BYTE                bEXFlag[12];                    //zht 2011-4-12, 不使用的字段，占位用
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号

	//游戏信息
	//	BYTE				iBeginStation;					//开始位置
	//	BYTE				iEndStation;					//结束位置

	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				iAllCardCount;						//扑克数目
    BYTE				iBeenPlayGame;					//已经游戏的局数

	BYTE				iSysCardCount;					//还剩下牌数量
	BYTE                bEXFlag1[12];                    //zht 2011-4-12, 不使用的字段，占位用
		//状态信息
	//	BYTE				iUserCardCount;					//用户手上扑克数目
	//	BYTE				iUserCard[45];					//用户手上的扑克
	BYTE				iUserCardCount[PLAY_COUNT];				//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][5];				//用户手上的牌	
	//运行状态变量

	int					iUpGradePeople;					//庄家位置
	int					iRoomBasePoint;					//房间倍数
	int					iRunPublish;					//逃跑扣分
	
	int					iLimitNote;						//下注封顶值   明注 / 暗注
	int					iBaseNote;						//底注	       暗注
	int                 iGuoDi;                         //锅底
	int                 iLimtePerNote;                  //暗注上限，明注需要 * 2

	int					iTotalNote[PLAY_COUNT];					//用户当前下注
	BOOL                bShowIdOrPlay;
	char                strCityText[PLAY_COUNT][50];       //超级玩家随机地址
	char                strUserIDText[PLAY_COUNT][50];     //超级玩家随机ID
	bool                bSuperUser[PLAY_COUNT];             //该玩家是否是超级玩家

	
	
};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_4 : GameStationBase
{
	BYTE                bEXFlag[12];                    //zht 2011-4-12, 不使用的字段，占位用
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号

	//游戏信息
	//	BYTE				iBeginStation;					//开始位置

	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				iAllCardCount;						//扑克数目
	BYTE				iBeenPlayGame;					//已经游戏的局数
	BYTE				SysCard[28];					//还剩下的牌
	BYTE				iSysCardCount;					//还剩下牌数量
	//BYTE                bNtPeople;

	BYTE                bEXFlag2[12];                    //zht 2011-4-12, 不使用的字段，占位用
	int					iLimitNote;						//下注封顶值   明注 / 暗注
	int					iBaseNote;						//底注	       暗注
	int                 iGuoDi;                         //锅底
	int                 iLimtePerNote;                   //            暗注上限，明注需要 * 2

	int                 iCurNote;                       //上一位玩家下注值，已暗注计算

	int					iRoomBasePoint;					//房间倍数
	int					iRunPublish;					//逃跑扣分
	//运行状态变量
	BYTE                bNtPeople;                      //庄家位置
	BYTE				iUpGradePeople;					//下注位置
	BYTE				iOutCardPeople;					//现在出牌用户
	BYTE				iFirstOutPeople;				//先出牌的用户
	int					iTotalNote[PLAY_COUNT];					//用户当前下注
	int                 iThisGameNote[PLAY_COUNT];			//本局游戏下注数(明注)
	int					iTotalGameNote[PLAY_COUNT];
	BOOL                 iMing[PLAY_COUNT];              //明牌状态

	bool                bOpenLose[PLAY_COUNT];          //玩家比牌是否输了
	bool                bIsGiveUp[PLAY_COUNT];                      //玩家是否放弃

	bool               bIsFirstNote;                     //是否为第一次下注

	int				   iTimeRest;						// 定时器实际剩下的时间，用于表示断线重连时剩余时间值
	char                strCityText[PLAY_COUNT][50];       //超级玩家随机地址
	char                strUserIDText[PLAY_COUNT][50];     //超级玩家随机ID
	bool                bSuperUser[PLAY_COUNT];             //该玩家是否是超级玩家

	//状态信息
	//	int					iIngNote;						//当前下注用户
	//	int					iIngPlayStatus;					//正在游戏的动作
	//	int					iUserNote[4];					//用户本轮下注总数
	//	int					iTotalNote;						//所下总注
	tagUserResult		tag;
	BYTE				iUserCardCount[PLAY_COUNT];				//用户手上的牌数
	BYTE				iUserCard[PLAY_COUNT][5];				//用户手上的牌

	bool                bOpenLoser[PLAY_COUNT];

	BYTE                byWin[PLAY_COUNT];
		BOOL                bShowIdOrPlay;
	//	tagButtonStatus		ButtonPara;						//按钮状态
};

//游戏状态数据包	（ 等待下盘开始状态  ）
struct GameStation_5 : GameStationBase
{
	BYTE                bEXFlag[12];                    //zht 2011-4-12, 不使用的字段，占位用
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	int					iRoomBasePoint;					//房间倍数
	int					iRunPublish;					//逃跑扣分

	//游戏信息
	//	BYTE				iBeginStation;					//开始位置
	//	BYTE				iEndStation;					//结束位置
	BYTE				iThinkTime;						//游戏盘数
	BYTE				iBeginTime;						//游戏盘数
	BYTE				iAllCardCount;						//扑克数目

	BYTE                bEXFlag2[12];                    //zht 2011-4-12, 不使用的字段，占位用

	int					iLimitNote;						//下注封顶值   明注 / 暗注
	int					iBaseNote;						//底注	       暗注
	int                 iGuoDi;                         //锅底
	int                 iLimtePerNote;                   //            暗注上限，明注需要 * 2
	char                strCityText[PLAY_COUNT][50];       //超级玩家随机地址
	char                strUserIDText[PLAY_COUNT][50];     //超级玩家随机ID
	bool                bSuperUser[PLAY_COUNT];             //该玩家是否是超级玩家

	//运行状态变量
	int					iUpGradePeople;					//庄家位置

	BYTE				iBeenPlayGame;					//已经游戏的局数
		BOOL                bShowIdOrPlay;
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
	BYTE				AllCardCount;					//一共使用的牌数
	
	int					iBaseNote;						//底注	
	int					iLimitNote;						//最大注数
	int                 iGuoDi;                         //锅底
	int                 iLimtePerNote;                   //            暗注上限，明注需要 * 2

	BYTE				SysCard[52];					//当前牌

	BOOL                bIsShowIdOrPlay;                  //显示id还是玩家
	char                strCityText[PLAY_COUNT][50];
	char                strUserIDText[PLAY_COUNT][50];
	bool                bSuperUser[PLAY_COUNT];             //该玩家是否是超级玩家
	BYTE                bNtStation;                      //庄家      


	//BYTE				iNtStation;						//现在的主牌
	//BYTE				iStation[2];					//主牌的位置
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
	//BYTE				bDeskStatin;							//新一轮出牌者
	//	BYTE				bAddTime;								//押注次数
	//	BOOL				bEndFlag;								//结束标志
	//	SendCardStruct()
	//	{
	//		iOutPeople = 0;
	//		memset(iCard,0,sizeof(iCard));
	//		bAddTime=0;
	//		bEndFlag=FALSE;
	//	}
};
/*
struct SendCardStruct
{
BYTE				iCard[4][2];							//牌标号
BYTE				iOutPeople;								//新一轮出牌者
BYTE				bAddTime;								//押注次数
BOOL				bEndFlag;								//结束标志
SendCardStruct()
{
iOutPeople = 0;
memset(iCard,0,sizeof(iCard));
bAddTime=0;
bEndFlag=FALSE;
}
};
*/

//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;				//出牌的位置
	BYTE                bNtPeople;                      //庄家位置
	BYTE				byUserData[PLAY_COUNT][2];					//确定玩家是否机器人和玩家的牌大小排名（只发给机器人）lym
};

//用户出牌数据包 （发向服务器）
struct OutCardStruct
{
	int					iCardCount;						//扑克数目
	BYTE				iCardList[39];					//扑克信息
};



//用户出牌数据包 （发向客户端）
struct OutCardMsg
{
	int					iNextDeskStation;				//下一出牌者
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iCardList[39];					//扑克信息
};



//游戏结束统计数据包
struct GameEndStruct
{
	int					iUserState[PLAY_COUNT];					//四家状态(提前放弃,还是梭)

	BYTE				iCardShape[PLAY_COUNT];
	int					iTurePoint[PLAY_COUNT];					//庄家得分
	int					iChangeMoney[8];
	BYTE				iUpGradeStation;						//庄家位置
	BYTE				bCard[PLAY_COUNT][MAX_CARD_COUNT];		//牌标号	Added by zxd 20090928 为防止作弊，不再下发所有玩家的牌，因此游戏结束时要发
};

//游戏结束统计数据包
struct GameCutStruct
{
	int					bDeskStation;					//退出位置
	int					iTurePoint[PLAY_COUNT];					//庄家得分
	int					iChangeMoney[8];
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
/*
//按钮类型
enum BUTTON_TYPE
{
BUTTON_TYPE_NONE = -1,
BUTTON_TYPE_GIVEUP,//放弃
BUTTON_TYPE_100NOTE,//100注
BUTTON_TYPE_200NOTE,//200注
BUTTON_TYPE_400NOTE,//400注
BUTTON_TYPE_ADD,	//加注
BUTTON_TYPE_FOLLOW,//跟了
BUTTON_TYPE_SHUTTLE,//梭了
};*/

//用户处理信息
struct tagUserProcess
{
	BYTE iVrebType;//所处理的按钮
	int  iNote;		//下注数
	BOOL bMing;     //明牌否?
};

struct tagCompareCard :  public tagUserProcess 
{
	BYTE byComparedStation ;  //被对比牌的玩家   
};

struct tagCompareCardResult
{
	BYTE iNt;		//比牌者
	BYTE iNoNt;     //被比者
	BYTE iLoster;	//败者
	BYTE bWinner;		// Add By ZXD 20090820 胜利者，若比牌结束后，下一家马上达到下注上限，则此时客户端需要知道胜利者是谁
	int iCurNote;		//当前玩家下注数
	int iNote;          //当前有效注数
	BYTE bAddStyle;   //类型
	bool bGameFinish;
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
	BYTE bAddStyle;		//下注类型
	BYTE iOutPeople;	//下注者
	int iCurNote;		//当前玩家下注数
	int iNote;          //当前有效注数
	BYTE bNextDeskStation;
	int  iLimiteNote;             //玩家限注
};
struct GameFinishNotify
{
	TCHAR name[PLAY_COUNT][61];
	//int dwUserID[PLAY_COUNT];         //打印userid
	int	iBasePoint;
	BYTE iStyle;			//游戏类型是否为强退,还是正常结束
	int	iWardPoint[PLAY_COUNT];
	//__int64 iTotalPoint[4];
	BYTE iCardShape[PLAY_COUNT];
	int iMoney[PLAY_COUNT];
};
/*
//放弃结果数据
struct tagUserGiveUp
{
BOOL bShowOnly;		//仅显示
BOOL bAddFlag;	//加倍标记	
BYTE bAddTime;	//押注次数
BYTE bAddStyle;	//下注类型	
BYTE iGiveUpPeo;//放弃者
BYTE iOutPeople;//下注者
//	tagButtonStatus ButtonPara;//按钮参数
};
*/
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				//用户设置游戏
#define ASS_BEGIN_UPGRADE				51				//升级游戏开始
#define ASS_SEND_CARD					52				//发牌信息
#define ASS_SEND_FINISH					54				//发牌完成
#define ASS_GAME_PLAY					59				//开始游戏
#define ASS_SEND_ALLCARD				63				//发送所有的余牌
#define ASS_NEW_TURN					64				//新一轮开始
#define ASS_CONTINUE_END				65				//游戏结束
#define ASS_NO_CONTINUE_END				66				//游戏结束
#define ASS_CUT_END						67				//用户强行离开
#define ASS_SALE_END					68				//游戏安全结束
#define ASS_HAVE_THING					69
#define ASS_LEFT_RESULT					70
#define ASS_MESSAGE						71
#define ASS_STOP_THING					72
#define ASS_AGREE_STOP					73

#define ASS_SUPERSEDE_VREB_CHECK		78				//代替(断线)用户处理
#define ASS_VREB_CHECK					179				//用户处理

#define ASS_SHUTTLE						80				//梭了
#define ASS_FOLLOW						81				//跟了或不加
#define ASS_ADD_NOTE					82				//加注
#define ASS_SHOW_100					83				//押注100
#define ASS_SHOW_200					84				//押注200
#define ASS_SHOW_400					85				//押注400
#define ASS_GIVE_UP						86				//放弃
#define ASS_MODIFY_PLAY_NOTE			90				//更新用户所下注
#define ASS_NOTE						91				//下注
#define ASS_NOTE_RESULT					92				//下注结果
#define ASS_SEND_A_CARD					93				//新发一张牌
#define ASS_COLLECT_JETON				94				//收集筹码
#define ASS_BIPAI_RESULT                95              //比牌结果
#define ASS_GM_SUPERCLIENT				100				//超級客戶端
#define ASS_FINISH_COMPARE              101             
#define ASS_AI_WIN                      102             //机器人必赢
/********************************************************************************/
#endif
