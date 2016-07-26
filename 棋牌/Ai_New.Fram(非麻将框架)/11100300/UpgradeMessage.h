#pragma once

//同楼 普通版/视频版 编译  release/视频版  ，提牌器编译提牌器版
#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
//---------------------------------------------------------------------------------------//
//文件名字定义
#define GAMENAME						TEXT("龙虎斗")
#ifdef VIDEO
#define NAME_ID							10321800								//名字 ID
#else
#define NAME_ID							10321800								//名字 ID
#endif
//---------------------------------------------------------------------------------------//
//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本
//---------------------------------------------------------------------------------------//
//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//---------------------------------------------------------------------------------------//
//游戏内部开发版本号
#define DEV_HEIGHT_VERSION				2				//内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					0				//内部开发低版本号
//---------------------------------------------------------------------------------------//
//游戏信息
#define PLAY_COUNT						180								//游戏人数
#define SKIN_CARD						TEXT(".\\image\\cardskin")		//共用图片文件夹
static TCHAR szTempStr[MAX_PATH] = {0};//暂存字符串，用来获取 dll_name 等的时候使用
#define GET_CLIENT_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))
#define GET_SERVER_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))
#define GET_CLIENT_BCF_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".bcf"))
//---------------------------------------------------------------------------------------//
//游戏状态定义
static	const	int	GS_WAIT_SETGAME		=	0;			//等待东家设置状态
static	const	int	GS_WAIT_ARGEE		=	1;			//等待同意设置

static	const	int	GS_GAME_COMPARE		=	19;			//游戏准备阶段

static	const	int	GS_FREE_STATUS		=	20;			//游戏空闲阶段
static	const	int	GS_BET_STATUS		=	21;			//游戏下注阶段
static	const	int	GS_SEND_STATUS		=	22;			//发牌阶段

static	const	int	GS_WAIT_NEXT		=	23;			//等待下一盘开始阶段


static	const	BYTE	BET_ARES	=	3;			//下注区域数
#define ASS_SUPER_USER					70				//超端用户消息

#define ASS_AHEAD_END					95				/** 提前结束 */


#define ASS_CUT_END				  103		 // 用户强行离开
#define ASS_SAFE_END			  104		 // 用户安全结束

//定时器类型
#define TIME_FREE_TIME		32	//空闲定时器
#define TIME_XIAZHU_TIME	33 //下注定时器
#define TIME_SEND_TIME		34 //发牌定时器

#define BETSTYLE 6  //筹码类型
// 数据包处理辅助标识(50-99)
//warning:79号ID预留给超级客户端,永不使用
/********************************************************************************/
#define ASS_UG_USER_SET					50				//设置游戏
#define ASS_BEGIN_APPLY					51				//开始申请上庄消息
#define ASS_CHAOGUO				52				//下注总数超过自己的金币

#define ASS_GAME_FREE					53				//空闲消息
#define ASS_BEGIN_BET					54				//开始下注
#define ASS_GAME_SEND					55				//开始开牌
#define ASS_GAME_COME					56				//玩家进入游戏消息

#define ASS_START					57				//开始



#define ASS_GAME_SENDCARD				58				//开始发牌
#define ASS_GAME_ACCOUNT                    59              //游戏结算

#define ASS_APPLY_NT					60				//申请上庄消息
#define ASS_APPLY_RESULT				61				//申请上庄结果消息
#define ASS_APPLY_LEAVE					62				//申请下庄消息
#define ASS_LEAVE_RESULT				63				//申请下庄结果消息
#define ASS_USER_BET					64				//玩家下注消息
#define ASS_BET_RESULT					65				//玩家下注结果消息
#define ASS_SEL_AREA					66				//庄家藏签消息
#define ASS_GM_STATUS_GAME				67			//获取游戏状态消息
#define ASS_GAME_SHOWRESULT             68              //显示结算框
#define ASS_GAME_DATA                   69              //游戏结算数据
#define ASS_SUPER_RESULT				70				//超端用户控制结果消息包


//下注区域赔率
static	const	int		G_iArePeiLv[3] = {1,1,8};

//筹码类型值
static	const	int		G_iChouMaValue[6] = {100,1000,10000,100000,1000000,10000000};

//申请上庄消息
struct	ApplyNtMsg
{
	BYTE	byDeskStation;		//申请座位号
	ApplyNtMsg()
	{
		memset(this,-1,sizeof(ApplyNtMsg));
		
	}
};

//超端用户控制的结果
struct	SuperUserControlReslut
{
	BYTE	byDeskStation;		//超端用户座位号
	BYTE    bySuperReslut;		//超端用户控制的结果
	SuperUserControlReslut()
	{
		memset(this,-1,sizeof(SuperUserControlReslut));

	}
};

//强退结构 add by wlr 20090807
struct tagGameForceQuit
{
	BYTE				byDeskStation;					// 退出位置
	tagGameForceQuit()
	{
		byDeskStation = 0;
	}
};
//申请上庄消息
struct	ChaoguoMsg
{
	BYTE	byDeskStation;		//申请座位号		
	ChaoguoMsg()
	{
		memset(this,-1,sizeof(ChaoguoMsg));
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

//-------------------------------------------------------------------------------
//下注消息包
struct BeginBetMsg
{
	bool		bFlag;			//下注标识
	bool		bLuziClearFlag;			//路子清除标志
	BYTE		byNtStation;	//庄家位置
	BYTE		byBetTime;		//下注时间
	BYTE		byGameStation;//游戏状态
	BYTE		byNtWaitList[PLAY_COUNT];//庄家列表
	BYTE		byEvent;//游戏局数
	BYTE		byNtcount;//庄家坐庄次数
	__int64		i64NtScore;//庄家成绩

	BeginBetMsg()
	{
		memset(this,0,sizeof(BeginBetMsg));
		memset(byNtWaitList,255,sizeof(byNtWaitList));
		byNtStation=255;
		bLuziClearFlag=FALSE;
	}
};
//发牌消息包
struct BeginSendCard
{
	bool bFlag;//发牌标识
	BYTE Bcard;//发牌类型
	BYTE cardSort;//发到第几张牌
	BYTE cardCount;//发牌个数
	BYTE byNtStation;//庄家位置
	BYTE byPlayStation;//玩家位置
	BeginSendCard()
	{
		memset(this,0,sizeof(BeginSendCard));
	}
};
//结算框消息包
struct BeginShowResultData
{
		BYTE      bDeskStation;
	  __int64     i64ChangeMoney;//玩家得分
	  __int64     i64ReturnMoney;//返还的金币数
	  __int64     i64PlayerGrade;//玩家成绩
	  BeginShowResultData()
	  {
		  memset(this,0,sizeof(BeginShowResultData));
	  }
};
//结算框消息包
struct BeginShowResult
{
	  BYTE        byGameStation;
	  BeginShowResult()
	  {
		  memset(this,0,sizeof(BeginShowResult));
		 
	  }
};
//游戏开始消息包
struct StartMsg
{
	BYTE    startTime;   //庄家位置 
	StartMsg()
	{
		memset(this,0,sizeof(StartMsg));
	}
};
//-------------------------------------------------------------------------------
//发牌消息包
struct BeginSendCardMsg
{
	BYTE	byWinArea;//开奖区域
	BYTE    byCardCount;//发牌个数
	BYTE    byCardStyle[52];//发牌类型
	BYTE	byGameStation;
	BYTE	byNtWaitList[PLAY_COUNT];
	BYTE    byLuZi[72];//路子

	BeginSendCardMsg()
	{
		memset(this,0,sizeof(BeginSendCardMsg));
		memset(byNtWaitList,255,sizeof(byNtWaitList));
		memset(byLuZi,255,sizeof(byLuZi));
		byWinArea=255;
	}
};
//玩家进入游戏包
struct UserComePlayerMsg
{
	BYTE    byPlayStation;//玩家位置
	UserComePlayerMsg()
	{
		byPlayStation=255;
	}
};
//游戏等待开始消息包
struct GameWaitStartMsg
{
	bool flag;
	GameWaitStartMsg()
	{
		flag=false;
	}
};
//开牌状态消息包
struct BeginKaiPaiStation
{
	bool	bFlag;			//空闲标识
	BYTE	byFreeTime;		//空闲时间
	BYTE    byNtStation;//庄家位置
	BYTE    byPlayStation;//玩家位置
	BYTE    byRemaindTime;//剩余时间
	BYTE    bGameStation;	//游戏状态
	BYTE	byWaitNtList[PLAY_COUNT];	//申请上庄列表
	int     iBetSumLimite;	//下注总数限制
	int     iApplyNtLimite;	//上庄条件
	int     iNtPlayCount;		//庄家坐庄次数
	BeginKaiPaiStation()
{
		memset(this,0,sizeof(BeginKaiPaiStation));
	}
};
//-------------------------------------------------------------------------------
//空闲消息包
struct BeginFreeMsg
{
	bool	bFlag;			//空闲标识
	BYTE	byGameStation ;         ///当前的游戏状态
	BYTE	byFreeTime;		//空闲时间
	BYTE    byNtStation;//庄家位置
	BYTE    byPlayStation;//玩家位置
	BYTE    byNtcount;//庄家坐庄次数
	BYTE    byWaitNtList[PLAY_COUNT];	//上庄列表
	__int64 i64NtScore;//庄家成绩
	BeginFreeMsg()
	{
		memset(this,0,sizeof(BeginFreeMsg));
		byNtStation = 255;
		memset(byWaitNtList, 255, sizeof(byWaitNtList));
	}
};
//准备阶段退出结构
struct GamePrepareStageQuit
{
	BYTE				byNtStation;	//庄家位置
	BYTE				byNtWaitList[PLAY_COUNT];//庄家列表
	BYTE                byNtcount;//坐庄次数
	__int64             i64NtScore;//庄家成绩
	GamePrepareStageQuit()
	{
		memset(this,0,sizeof(GamePrepareStageQuit));
		memset(byNtWaitList,255,sizeof(byNtWaitList));
		byNtStation=255;
	}
};
//-------------------------------------------------------------------------------
//空闲状态消息包
struct BeginFreeStation
{
	bool	bFlag;			//空闲标识
	BYTE	byFreeTime;		//空闲时间
	BYTE    byNtStation;//庄家位置
	BYTE    byPlayStation;//玩家位置
	BYTE    byRemaindTime;//剩余时间
	BYTE    bGameStation;	//游戏状态
	BYTE	byWaitNtList[PLAY_COUNT];	//申请上庄列表
	int     iBetSumLimite;	//下注总数限制
	int     iApplyNtLimite;	//上庄条件
	int iNtPlayCount;//庄家坐庄次数
	
	BeginFreeStation()
	{
		memset(this,0,sizeof(BeginFreeStation));
	}
};
//申请上庄结果消息
struct	ApplyNtResultMsg
{
	BOOL    bNtFlag;			//在没有庄家的情况下上庄的标志
	bool	bApplay;			//是否因为金币没有达到上庄金币而上庄不成功
	BYTE	byDeskStation;		//申请座位号
	BYTE	byWaitNtList[PLAY_COUNT];	//申请上庄列表
	BYTE	byNtStation;		//庄家的位置
	BYTE    byNtcount;//庄家坐庄次数
	BYTE    bGameStation;
	BYTE    bEvent;//游戏局数
	__int64 i64NtScore;//庄家成绩
	ApplyNtResultMsg()
	{
		memset(this,-1,sizeof(ApplyNtResultMsg));
		memset(byWaitNtList,255,sizeof(byWaitNtList));
		byNtStation=255;
		bApplay=false;
		bNtFlag=false;
	}
};
//申请下庄消息包

struct	ApplyLiveNtMsg
{
	BYTE	byDeskStation;		//申请座位号
	ApplyLiveNtMsg()
	{
		memset(this,-1,sizeof(ApplyLiveNtMsg));
	}
};
//申请下庄结果消息包

struct	ApplyLiveNtResultMsg
{
	
	BYTE    byNtStation;//庄家位置
	BYTE	byDeskStation;		//申请座位号
	BYTE    byWaitNtList[PLAY_COUNT];	//上庄列表
	BYTE    byNtcount;//庄家坐庄次数
	__int64 i64NtScore;//庄家成绩
	ApplyLiveNtResultMsg()
	{
		memset(this,-1,sizeof(ApplyLiveNtResultMsg));
		memset(byWaitNtList,255,sizeof(byWaitNtList));
		byNtStation=255;
		byDeskStation=255;
		byNtcount=0;
		i64NtScore=0;
	}
};
//游戏空闲消息
struct GameFreeMsg
{
	BYTE	byFreeTime;		//空闲时间
	BYTE	byNtStation;	//庄家位置
	BYTE	byWaitNtList[PLAY_COUNT];	//上庄列表
	BYTE	byLuziList[10];		//路子
	BYTE	byCurrResult;		//当局开奖结果
	BYTE	byLastResult;		//上局开奖结果
	int		iNtPlayCount;	//庄家坐庄次数
	__int64	i64NtHaveWin;	//庄家输赢情况
	__int64	i64MeHaveWin;	//自己的输赢情况

	GameFreeMsg()
	{
		memset(this,0,sizeof(GameFreeMsg));
	}	
};



//玩家下注
struct	UserBetMsg
{
	BYTE	byDeskStation;		//下注座位号
	BYTE	byBetArea;			//下注区域
	BYTE	byChouMaType;		//下注筹码类型
	UserBetMsg()
	{
		memset(this,0,sizeof(UserBetMsg));
	}
};


//结算消息包
struct GameEndMsg
{
	BYTE		byLuzi[13];							//最近10局路子情况
	BYTE		byShowResultTime;					//结算显示时间
	BYTE  byWaitNtList[PLAY_COUNT];
	int			iNtPlayCount;						//庄家坐庄的次数
	__int64		i64NtHaveWin;						//庄家输赢情况
	__int64		i64MeHaveWin;						//自己的总输赢情况
	__int64		i64TurePoint[PLAY_COUNT];			//玩家得分
	__int64		i64ChangeMoney[PLAY_COUNT];			//玩家输赢的金币
	__int64		i64ReturnMoney[PLAY_COUNT];			//返还的金币数
	 

	GameEndMsg()
	{
		memset(this,0, sizeof(GameEndMsg));
	}
};
struct GameAccountMsg
{
	BYTE        winArea;     //中奖区域
	BYTE        byNtStation;//庄家位置
	BYTE        byPlayStation;//玩家位置
	BYTE        byWaitNtList[PLAY_COUNT];//上庄列表
	__int64     i64PlayScore;//闲家得分
	__int64     i64PlayReturnMoney;//闲家返还金币数
	__int64     i64NtScore;//庄家得分
	__int64     i64NtReturnMoney;//庄家家返还金币数
	GameAccountMsg()
	{
		memset(this,0, sizeof(GameAccountMsg));
		byNtStation=255;
		byPlayStation=255;
		memset(byWaitNtList,255, sizeof(byWaitNtList));
	}
};
struct GameBetMsg
{
	BYTE	byFreeTime;		//空闲时间
	BYTE    byNtStation;//庄家位置
	BYTE    byPlayStation;//玩家位置
	BYTE    byRemaindTime;//剩余时间

	BYTE    bGameStation;	//游戏状态
	BYTE	byWaitNtList[PLAY_COUNT];	//申请上庄列表
	int     iBetSumLimite;	//下注总数限制
	int     iApplyNtLimite;	//上庄条件
	int     iNtPlayCount;		//庄家坐庄次数
	GameBetMsg()
	{
		memset(this,0, sizeof(GameBetMsg));
	}
};

//-------------------------------------------------------------------------------
//游戏状态基础数据包
struct	GameStation_Base
{
	//游戏版本
	BYTE		iVersion;						//游戏版本号
	BYTE		iVersion2;						//游戏版本号

	BYTE		bGameStation;		           //游戏状态

	BYTE		byFreeTime;					   //空闲时间
	BYTE		byBetTime;			           //下注时间
	BYTE		bySendTime;	                  //开牌时间

	BYTE		bySendCardTime;	        //发牌时间
	BYTE		byAccountTime;	        //结算时间
	BYTE        byShowResult;	        //展示结果时间
	BYTE		bySendCardSpaceTime;	//发牌间隔时间
	BYTE		bySpeedTime;		   //遮挡牌移动速度
	BYTE        byShowWinTime;//显示输赢
	BYTE		byRemaindTime;		   //剩余时间

	BYTE		byNtStation;		//庄家位置

	
	BYTE		byLuziList[72];		//路子
	BYTE		byWaitNtList[PLAY_COUNT];	//上庄列表
	BYTE		byMaxNtPeople;		//最大上庄人数
	BYTE        byEvent;//游戏局数
	int			iNtPlayCount;		      //庄家坐庄次数
	int			iApplyNtLimite;		     //上庄条件
	__int64		i64NtGrade;		        //庄家成绩
	__int64     i64PlayerGrade;			//閑家成績
	
	GameStation_Base()
	{
		memset(this,0, sizeof(GameStation_Base));
	}
};


//-------------------------------------------------------------------------------
//游戏下注状态数据包
struct	GameStation_Bet:public GameStation_Base
{
	BYTE		byCurrResult;	//庄家藏签的结果
	__int64		i64UserBetCount[PLAY_COUNT][BET_ARES];	//各玩家在每个区域的下注情况
};

//-------------------------------------------------------------------------------
//游戏出签状态数据包
struct	GameStation_OpenResult:public GameStation_Base
{
	BYTE		byCurrResult;		//当前开奖结果
	__int64		i64UserBetCount[PLAY_COUNT][BET_ARES];	//各玩家在每个区域的下注情况
};
//-------------------------------------------------------------------------------
//游戏出签状态数据包
struct	GameStation_ShowResult:public GameStation_Base
{
	__int64		i64UserBetCount[PLAY_COUNT][BET_ARES];	//各玩家在每个区域的下注情况
	__int64		i64UserChangeMoney[PLAY_COUNT];			//玩家输赢情况
	__int64		i64ReturnMoney[PLAY_COUNT];				//返还的金币数
};

//-------------------------------------------------------------------------------
//下注结果
struct	UserBetResultMsg
{
	BYTE	    byDeskStation;		//下注座位号
	BYTE		byBetArea;			//下注区域
	BYTE		byChouMaType;		//下注筹码类型
	BYTE	    byBetNumFlag;		//下注100W，500W，超500W的标志
	__int64     i64UserBetCount[BET_ARES]; //闲家在每个区域的下注情况
	__int64		i64AreaBetAllSum[BET_ARES];		//各区域的总下注
	__int64     i64SumAllBet;
	UserBetResultMsg()
	{
		memset(this,0,sizeof(UserBetResultMsg));
	}
};

//發送所有牌數據
struct SendAllStruct
{
	BYTE             iUserCardCount;		//发牌数量
	BYTE             iUserCardList[108];				//发牌队例
};


///底牌类型
enum BackCardType
{
	TYPE_NONE        = 0 ,//什么牌型都不是
	TYPE_DOUBLE_CARD = 10 , 
	TYPE_SAME_HUA    = 11,
	TYPE_SMALL_KING  = 12 ,
	TYPE_BIG_KING    = 13 , //大王
	TYPE_TRIPLE_CARD = 14 ,
	TYPE_STRAIT      = 15,
	TYPE_ROCKET      = 16
};
///任务类型
enum LastTaskType
{
	TYPE_LAST_NONE         = 0 ,    ///无任何牌型 
	TYPE_HAVE_A_CARD       = 100 ,  ///有某张牌
	TYPE_SOME_SHAPE        = 101 ,  ///有某种牌型
	TYPE_SINGLE_SOME_CARD  = 102 ,  ///打的某张牌
	TYPE_DOUBLE_SOME_CARD  = 103   ///打的一对某种牌
};

///游戏任务结构
struct GameTaskStruct
{
	BYTE     byBackCardType ; 
	BYTE     byTaskType ; 
	BYTE     bySpecifyShape ; 
	BYTE     bySpecifyCard ;
	BYTE     byBackCardMutiple ;  ///底牌倍数

	GameTaskStruct()
	{
		memset(this , 0 , sizeof(GameTaskStruct)) ; 
	}
	void IniData(void)
	{
		memset(this , 0 ,sizeof(GameTaskStruct)) ;
	}

};
///游戏中的倍数
struct  GameMutipleStruct
{
	int       sBaseMutiple ;                 ///游戏中的基本底注
	int       sBackCardMutiple ;             ///底牌倍数
	int       sBombCount ;                   ///炸弹倍数
	int       sSprintMutiple ;               ///春天
	int       sCardShapeMutiple  ;           ///牌型倍数（一般指任务中的倍数）

	BYTE      sAddGameMutiple[PLAY_COUNT] ;  /// 游戏中加倍
	BYTE      sRobNtMutiple[PLAY_COUNT] ;    ///抢地主倍数
	BYTE      sMingPaiMutiple[PLAY_COUNT] ;  ///每个人的明牌倍数

	GameMutipleStruct()
	{
		sBaseMutiple = 1 ; 
		sBackCardMutiple = 1 ; 
		sBombCount = 0 ; 
		sSprintMutiple = 1;
		sCardShapeMutiple = 100 ; 

		memset(sAddGameMutiple , 0 , sizeof(sAddGameMutiple)) ; 
		memset(sRobNtMutiple , 0 , sizeof(sRobNtMutiple)) ; 
		memset(sMingPaiMutiple , 0 , sizeof(sMingPaiMutiple)) ; 
	};
	void  IniData(int  iBaseMutiple)
	{
		sBaseMutiple = iBaseMutiple ; 
		sBombCount = 0 ; 
		sBackCardMutiple = 1 ; 
		sSprintMutiple = 1;
		sCardShapeMutiple = 100; 

		memset(sAddGameMutiple , 0 , sizeof(sAddGameMutiple)) ; 
		memset(sRobNtMutiple , 0 , sizeof(sRobNtMutiple)) ; 
		memset(sMingPaiMutiple , 0 , sizeof(sMingPaiMutiple)) ; 
	}
	///获取明牌最大倍数
	int  GetMingMaxMutiple(void)
	{
		int iMingMutiple = max(max(sMingPaiMutiple[0] , sMingPaiMutiple[1]) ,sMingPaiMutiple[2]) ; 

		return (iMingMutiple >0?iMingMutiple :1) ; 
	}
	///获取器抢地主倍数
	int  GetRobNtMutiple(void)
	{
		int  iRobMutiple = 0 ; 
		int  iRobCount   = 0 ; 

		for(int  i = 0  ; i <PLAY_COUNT ; i++)
		{
			iRobCount += sRobNtMutiple[i] ; 
		}

		iRobMutiple = pow(2.0, iRobCount) ; 

		return iRobMutiple ; 
	}
    ///获取公共倍数
    int  GetPublicMutiple(void)
	{
		int  iBombMutiple = pow(2.0 , sBombCount) ; 

		int  iGameMutiple  = sBaseMutiple*sBackCardMutiple*iBombMutiple*sSprintMutiple*sCardShapeMutiple*GetRobNtMutiple()*GetMingMaxMutiple()/100 ;
			
		return iGameMutiple ; 
		
	}
};


/********************************************************************************/
//游戏数据包

/********************************************************************************/
//用户出牌数据包 （发向服务器）
struct OutCardStruct
{
	int					iCardCount;						//扑克数目
	BYTE				iCardList[45];					//扑克信息
};

//代替玩家出牌
struct ReplaceOutCardStruct
{
	BYTE				bDeskStation;					//代替出牌的位置
	int					iCardCount;						//扑克数目
	BYTE				iCardList[45];					//扑克信息
};

//用户出牌数据包 （发向客户端）
struct OutCardMsg
{
	int					iNextDeskStation;				//下一出牌者
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者	
	BYTE				iCardList[45];					//扑克信息
};
//新一轮
struct NewTurnStruct
{
	BYTE				bDeskStation;					//坐号
	BYTE				bReserve;						//保留
};
//游戏状态数据包	（ 等待东家设置状态 ）
struct GameStation_1
{
	//游戏版本
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	//游戏信息
};

//游戏状态数据包	（ 等待其他玩家开始 ）
struct GameStation_2
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号
	bool                bUserReady[PLAY_COUNT] ;        ///玩家是否已准备

	BYTE				iBeginTime;						//开始准备时间
	BYTE				iThinkTime;						//出牌思考时间
	BYTE				iCallScoreTime;					//叫分计时
	BYTE                iRobNTTime ;                    //抢地主时间
	BYTE				iAddDoubleTime;					//加棒时间

	int                 iGameMutiple   ;             ///游戏中的倍数
	int                 iAddDoubleLimit;             //加倍限制
	int                 iGameMaxLimit ;              ///游戏最大输赢

	DWORD				iCardShape;						//牌型设置
	//游戏倍数
	UINT				iDeskBasePoint;					//桌面基础分
	UINT				iRoomBasePoint;					//房间倍数
	__int64				iRunPublish;					//逃跑扣分
	
};


//游戏状态数据包	（ 等待扣押底牌状态 ）
struct GameStation_3
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号

	BYTE				iBackCount;						//底牌数
	BYTE				iBeginTime;						//开始准备时间
	BYTE				iThinkTime;						//出牌思考时间
	BYTE                iRobNTTime ;                    //抢地主时间
	BYTE				iCallScoreTime;					//叫分计时
	BYTE				iAddDoubleTime;					//加棒时间

	BYTE				iCallScorePeople;				//当前叫分人
	BYTE				iGameFlag;						//叫分标记
	BYTE				iCallScoreResult;				//所叫的分
	
	int					iUpGradePeople;					//庄家位置
	int                 iCurOperator ;                 ///当前操作的人

	UINT				iDeskBasePoint;					//桌面基础分
	UINT				iRoomBasePoint;					//房间倍数

	DWORD				iCardShape;						//牌型设置
	int                 iGameMutiple   ;             ///游戏中的倍数
	int                 iAddDoubleLimit;             //加倍限制
	int                 iGameMaxLimit ;              ///游戏最大输赢

	bool				bAuto[PLAY_COUNT];				//托管情况
	bool				bCanleave[PLAY_COUNT];			//能否点退出
	BYTE				iCallScore[PLAY_COUNT];			//几家叫分情况
	int 				iRobNT[PLAY_COUNT];				//抢地主情况
	int                 iUserDoubleValue[PLAY_COUNT] ;  ///玩家加倍情况
	BYTE				iUserCardCount[PLAY_COUNT];		//用户手上扑克数目
	BYTE				iUserCardList[195];				//用户手上的扑克
	BYTE                iGameBackCard[12] ;                 ///底牌
	BYTE 				iBackCardCount;						//底牌数量
	__int64				iRunPublish;					//逃跑扣分

	GameMutipleStruct   gameMutiple ;                      ///游戏倍数  
	GameTaskStruct      gameTask ;                         ///游戏任务


};

//游戏状态数据包	（ 游戏中状态 ）
struct GameStation_4	
{
	bool				bIsLastCard;						//是否有上轮数据
	
	BYTE				iVersion;							//游戏版本号
	BYTE				iVersion2;							//游戏版本号
	BYTE				iBackCount;							//底牌数

	BYTE				iBeginTime;							//开始准备时间
	BYTE				iThinkTime;							//出牌思考时间
	BYTE                iRobNTTime ;                    //抢地主时间
	BYTE				iCallScoreTime;						//叫分计时
	BYTE				iAddDoubleTime;						//加棒时间

	BYTE				bIsPass;							//是否不出

	int					iRunPublish;						//逃跑扣分	
	int					iBase;								//当前炸弹个数
	int					iUpGradePeople;						//庄家位置
	
	int 				iCallScoreResult;					//叫分结果
	int					iOutCardPeople;						//现在出牌用户
	int					iFirstOutPeople;					//先出牌的用户
	int					iBigOutPeople;						//先出牌的用户

	UINT				iDeskBasePoint;						//桌面基础分
	UINT				iRoomBasePoint;						//房间倍数

	int                 iGameMutiple   ;             ///游戏中的倍数
	DWORD				iCardShape;							//牌型设置
	int                 iAddDoubleLimit;             //加倍限制
	int                 iGameMaxLimit ;              ///游戏最大输赢

	BYTE				iAwardPoint[PLAY_COUNT];			//奖分
	BYTE				iPeopleBase[PLAY_COUNT];			//加棒
	BYTE				iRobNT[PLAY_COUNT];					//抢地主


	bool				bAuto[PLAY_COUNT];					//托管情况
	bool				bCanleave[PLAY_COUNT];				//能否点退出

	BYTE				iUserCardCount[PLAY_COUNT];			//用户手上扑克数目
	BYTE				iUserCardList[195];					//用户手上的扑克

	BYTE				iBaseOutCount;						//出牌的数目
	BYTE                iBaseCardList[45] ;                //桌面上的牌

	BYTE				iDeskCardCount[PLAY_COUNT];			//桌面扑克的数目
	BYTE                iDeskCardList[PLAY_COUNT][45] ;    ///桌面上的牌

	BYTE				iLastCardCount[PLAY_COUNT];			//上轮扑克的数目
	BYTE				iLastOutCard[PLAY_COUNT][45];		//上轮的扑克

	bool                bPass[PLAY_COUNT] ;                //不出
	bool                bLastTurnPass[PLAY_COUNT] ;         //上一轮不出

	BYTE                iGameBackCard[12] ;                 ///底牌
	BYTE 				iBackCardCount;						//底牌数量

	GameMutipleStruct   gameMutiple ;                      ///游戏倍数  
	GameTaskStruct      gameTask ;                         ///游戏任务

};

//游戏状态数据包	（ 等待下盘开始状态  ）
struct GameStation_5
{
	BYTE				iVersion;							//游戏版本号
	BYTE				iVersion2;							//游戏版本号
	BYTE				iBeginTime;							//开始准备时间
	BYTE				iThinkTime;							//出牌思考时间
	BYTE                iRobNTTime ;                    //抢地主时间
	BYTE				iCallScoreTime;						//叫分计时
	BYTE				iAddDoubleTime;						//加棒时间

	int                 iGameMutiple   ;               ///游戏中的倍数
	int                 iAddDoubleLimit;             //加倍限制
	int                 iGameMaxLimit ;              ///游戏最大输赢

	bool                bUserReady[PLAY_COUNT] ;     ///玩家准备
	DWORD				iCardShape;							//牌型设置

	UINT				iDeskBasePoint;						//桌面基础分
	UINT				iRoomBasePoint;						//房间倍数
	__int64				iRunPublish;						//逃跑扣分
};

//用户同意游戏
struct UserArgeeGame
{
	BYTE				iPlayCount;							//游戏盘数
	BYTE				iCardCount;							//扑克数目
};
///明牌开始
struct  UserMingStruct
{
	bool    bStart  ;       ///是否为开始
	bool    bMing ;         ///玩家是否明牌
	BYTE    bDeskStaion ;   ///玩家的位置
	BYTE    byCardCount ;   ///明牌时玩家扑克的数量
};

//游戏开始
struct BeginUpgradeStruct
{
	BYTE				iPlayLimit;							//游戏总局数
	BYTE				iBeenPlayGame;						//已经玩了多少局
	BYTE                bUserMing[PLAY_COUNT] ;            ///玩家明牌情况
};
//游戏准备数据包
struct PrepareStruct
{
	BYTE				bCard;									//明牌
};
//发牌数据包
struct SendCardStruct
{
	BYTE				bDeskStation;							//玩家
	BYTE				bCard;									//牌标号
};


//发牌结束
struct SendCardFinishStruct
{
	BYTE		bReserve;									
};

//底牌数据包
struct BackCardExStruct
{
	BYTE				iGiveBackPeople;				//底牌玩家
	BYTE				iBackCardCount;					//扑克数目
	BYTE				iBackCard[12];					//底牌数据

	GameTaskStruct      gameTask ;                      //游戏任务  

};
//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;				//出牌的位置
};


//用户甩牌失败数据包 （发向客户端）
struct OutShowCardStruct
{
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iResultCount;					//结果扑克数目
	BYTE				iCardList[78];					//扑克信息
};

//代替开始
struct ReplaceAgreeStruct
{
	BYTE bDeskStation;									
};

//叫分数据包
struct CallScoreStruct
{	
	BYTE				bDeskStation;							//当前叫分者
	int					iValue;									//叫分类型
	bool 				bCallScoreflag;							//叫分标记								
};
//抢地主
struct RobNTStruct
{
	BYTE   bDeskStation;										//抢地主坐号
	BYTE   byRobCount ;                                         //玩家抢地主次数
	int    iValue;												//抢地主情况
};

//明牌
struct ShowCardStruct
{
	BYTE bDeskStation;										//坐号
	BYTE iCardList[54];					                   //扑克信息
	BYTE iCardCount;						              //扑克数目
	int iValue;											  //保留
	int iBase;											  //倍数 20081204
};

//代替亮牌
struct ReplaceShowCardStruct
{
	BYTE bDeskStation;										//坐号
	BYTE bRepDeskStation;									//代替位置
	int iValue;												//保留
};
//加棒
struct AddDoubleStruct
{
	BYTE bDeskStation;										//加棒位置
	int iValue;												//加棒情况
};

//代替叫分数据包
struct ReplaceCallScoreStruct
{	
	BYTE		bDeskStation;								//代替叫分者
	int		    NowCallScorePeople;							//当前叫分者
	int			CallScore;									//叫分类型
	bool 		CallScoreFlag;								//叫分标记								
};

//奖分
struct AwardPointStruct
{
	BYTE	iAwardPoint;									//奖分
	BYTE	bDeskStation;									//坐号
	int		iBase;											//倍数
};

//托管数据结构
struct AutoStruct
{
	BYTE bDeskStation;
	bool bAuto;
};

//机器人托管
struct UseAIStation
{
	BYTE bDeskStation;					//坐号
	BOOL bState;						//状态
};

//冠军结果
struct ChampionStruct
{
	BYTE bDeskStation ;
};

//用户出牌数据包 （发向客户端）
struct BatchOutCardMsg
{
	BYTE				iCardCount;						//扑克数目
	BYTE				iCardList[156];					//扑克信息
};

////道俱
//struct PropStruct
//{
//	BYTE				sendDeskStation;				//发放位置
//	BYTE				recvDeskStation;				//接收位置
//	int					iValue;
//};
//游戏结束统计数据包
struct GameEndStruct
{
	bool                bFinishTask ;                    //是否完成了任务
	BYTE				iUpGradeStation;				//庄家位置
	BYTE			    iUserCardCount[PLAY_COUNT];	    //用户手上扑克数目
	BYTE			    iUserCard[PLAY_COUNT][45];		//用户手上的扑克

	GameMutipleStruct   gameMutiple ; 

    __int64  			iTurePoint[PLAY_COUNT];			//玩家得分
	__int64				iChangeMoney[8];				//玩家金币

	GameEndStruct()
	{
		memset(this , 0 ,sizeof(GameEndStruct)) ; 
	}

};

//游戏结束统计数据包
struct GameCutStruct
{
	int					iRoomBasePoint;					//倍数
	int					iDeskBasePoint;					//桌面倍数
	int					iHumanBasePoint;				//人头倍数

	int					bDeskStation;					//退出位置
	__int64				iChangeMoney[8];				//玩家金币
	__int64				iTurePoint[PLAY_COUNT];			//庄家得分

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

//游戏结算
struct GameFinishNotify
{
	TCHAR name[PLAY_COUNT][61];
	//	int	iBasePoint;
	BYTE iStyle;			//游戏类型是否为强退,还是正常结束
	int	iBasePoint;			//系统基数
	int	iDeskBasePoint;		//桌面基础分
	int iUpGradePeople;		//莊家
	int iGrade2;			//莊家盟友
	__int64	iWardPoint[PLAY_COUNT];
	__int64 iAwardPoint[PLAY_COUNT];	//讨赏
	__int64 iTotalScore[PLAY_COUNT];	//总积分
	__int64 iMoney[PLAY_COUNT];			//比赛所留钱数
	BYTE iGameStyle;					//游戏类型
};
struct UserleftDesk
{
	BYTE bDeskStation;                //断线玩家
};
/********************************************************************************/

// 数据包处理辅助标识
//warning:79号ID预留给超级客户端,永不使用
/********************************************************************************/
#define ASS_UG_USER_SET					50				//设置游戏
#define ASS_GAME_BEGIN					51				//游戏开始
#define ASS_GAME_PREPARE				52				//准备工作

#define ASS_SEND_CARD					53				//发牌信息
#define ASS_SEND_CARD_MSG				54				//发牌过程中处理消息
#define ASS_SEND_ALL_CARD				55				//发送所有牌(一下发放全部)
#define ASS_SEND_FINISH					56				//发牌完成
#define ASS_CALL_SCORE					57				//叫分
#define ASS_CALL_SCORE_RESULT			58				//叫分结果
#define ASS_CALL_SCORE_FINISH			59				//叫分结束
#define ASS_REPLACE_CALL_SCORE			60				//代替叫分
#define ASS_XIAZHU				61				//下注消息

#define ASS_ROB_NT						61				//抢地主
#define ASS_ROB_NT_RESULT				62				//抢地主结果
#define ASS_REPLACE_ROB_NT				63				//代替抢地主
#define ASS_GAME_MULTIPLE				64				//游戏倍数(抢地主后会加倍)
#define ASS_ROB_NT_FINISH				65				//抢地主结果

#define ASS_BACK_CARD					66				//底牌数据
#define ASS_BACK_CARD_EX				67				//扩展底牌数据(其他玩家可见)
#define ASS_ADD_DOUBLE					68				//加棒
#define ASS_ADD_DOUBLE_RESULT			69				//加棒结果
#define ASS_REPLACE_ADD_DOUBLE			70				//代替加棒
#define ASS_ADD_DOUBLE_FINISH			71				//加棒结束
#define ASS_SHOW_CARD					72				//亮牌
#define ASS_SHOW_CARD_RESULT			73				//亮牌结果
#define ASS_REPLACE_SHOW_CARD			74				//代替亮牌
#define ASS_SHOW_CARD_FINISH			75				//亮牌结束

#define ASS_GAME_PLAY					76				//开始游戏
#define ASS_OUT_CARD					77				//用户出牌
#define ASS_OUT_CARD_RESULT				78				//出牌結果
#define ASS_REPLACE_OUT_CARD			80				//代替出牌(79留给超级客户端发牌器)
#define ASS_ONE_TURN_OVER				81				//一轮完成(使客户端上一轮可用)
#define ASS_NEW_TURN					82				//新一轮开始
#define ASS_AWARD_POINT					83				//奖分(炸弹火箭)

#define ASS_CONTINUE_END				84				//游戏结束
#define ASS_NO_CONTINUE_END				85				//游戏结束
#define ASS_NO_CALL_SCORE_END			86				//无人叫分
#define ASS_CUT_END						87				//用户强行离开
#define ASS_SAFE_END					88				//游戏安全结束
#define ASS_TERMINATE_END				89				//意外结束
#define ASS_AHEAD_END					90				//提前结束
#define ASS_AUTO						91				//用户托管
#define ASS_HAVE_THING					92				//有事
#define ASS_LEFT_RESULT					93				//有事离开结果
#define ASS_MESSAGE						94				//文字消息
#define ASS_AI_STATION					95				//机器人托管(断线户用)
#define ASS_REPLACE_GM_AGREE_GAME		96				//代替玩家开始
#define ASS_USER_LEFTDESK               97               //玩家离开桌子或断线
#define ASS_PREPARE_END					98				//游戏准备阶段用户退出
/********************************************************************************/
#endif
