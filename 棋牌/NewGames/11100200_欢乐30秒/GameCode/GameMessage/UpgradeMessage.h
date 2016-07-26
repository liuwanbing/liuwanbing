// GameMsgPkg.h: game message package and define const.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UPGRADEMESSAGE_H__INCLUDED_)
#define _UPGRADEMESSAGE_H__INCLUDED_

#pragma pack(push,1)

#define CHECK_DEBUG   1

// 游戏名称信息
#define GAMENAME						TEXT("百家乐")                          // 游戏名称
#define GAME_DLL_AUTHOR                 TEXT("深圳红鸟网络科技")            //
#define GAME_DLL_NOTE                   TEXT("牌类 -- 游戏组件")

#ifdef VIDEO
#define NAME_ID							11100200								 // 名字 ID
#else
#define NAME_ID							11100200								 // 名字 ID


#endif
#define MAX_NAME_INFO                   256
#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)

#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录
#define SKIN_CARD						TEXT("CardSkin")		                 // 共用图片文件夹

// 版本定义
#define GAME_MAX_VER					1								        // 现在最高版本
#define GAME_LESS_VER					1								        // 现在最低版本
#define GAME_CHANGE_VER					0								        // 修改版本

// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

#define PLAY_COUNT						180							        // 游戏人数

//游戏内部开发版本号
#define DEV_HIGH_VERSION				2				                        // 内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					0				                        // 内部开发低版本号

// 游戏状态定义
#define GS_WAIT_SETGAME					0				// 等待东家设置状态
#define GS_WAIT_AGREE					1				// 等待同意设置
#define GS_SEND_CARD					20				// 发牌状态
//#define GS_PLAY_GAME					21				// 游戏中状态
#define GS_PLAY_GAME					22				// 开牌状态
#define GS_WAIT_NEXT					23				// 等待下一盘开始 

#define NO_WAITING                      0               // 不处于等待状态
#define WAITING_USER_BEGIN              1               // 等待其他玩家开始
 
#define ID_VIEW_CLOCKWISE               1               // 顺时针
#define ID_VIEW_ANTICLOCKWISE           0               // 逆时针

#define USER_HAND_CARD_COUNT            27              // 用户手中的牌
#define HAVE_THING_LENGHT               61              // 有事要走的消息长度
#define USER_AGREE_LEAVE                1               // 同意玩家离开
#define USER_DISAGREE_LEAVE             0               // 不同意玩家离开

#define ONE_SECOND                      1000            // 1 秒时间

//72把一局
#ifdef SET72 
//lym2010-03-17每局的最大把数
#define MAXCOUNT						72             
#else
//lym2010-03-17每局的最大把数
#define MAXCOUNT						30            
#endif
/// 游戏基本信息
typedef struct tagGameBaseInfo
{
	//游戏版本
	BYTE byHighVersion;		// 游戏版本号
	BYTE byLowVersion;		// 游戏版本号
	BYTE bySendCardOrder;    // 发牌顺序
	int iSendCardInterval;   // 发牌时间间隔(ms)	
	int iThinkTime;          // 出牌思考时间(s)
	int iAlarmTime;          // 告警时间(s)
	int iBeginTime;          // 开始时间(s)
	int iSendCardTime;		 // 发牌时间(ms)
	int iSendCardZhen;		 // 发牌帧数
	tagGameBaseInfo()
	{
		memset(this,0, sizeof(tagGameBaseInfo));
		byHighVersion = DEV_HIGH_VERSION;
		byLowVersion = DEV_LOW_VERSION;
	}
}GAME_BASE_INFO;

// 通知更新视图
typedef struct tagUpdateClientView
{	
	BYTE abySeatID[PLAY_COUNT];
}UPDATE_CLIENT_VIEW;

// 游戏开始时的发牌数据包
typedef struct tagBeginSendCard
{	
	WORD wSentCount;                  // 已发牌张数
	WORD wMingCard;                   // 明牌	
	WORD wUserCard[USER_HAND_CARD_COUNT]; // 玩家自己的牌
	BYTE byUserCardCount[PLAY_COUNT]; // 各玩家手中的牌数量
	BYTE byFirstSendPos;              // 第一位发牌玩家
	tagBeginSendCard()
	{
		memset(this, 0, sizeof(tagBeginSendCard));
	}
}BEGIN_SEND_CARD;

// 发牌后游戏开始数据包
typedef struct tagGameBegin
{
	BYTE byOutCardSeatID;       // 出牌玩家	
	BYTE abySeatID[PLAY_COUNT];  //(如果有调整、则为调整后的位置)
	tagGameBegin()
	{
		memset(this, 0, sizeof(tagGameBegin));
	}
}GAME_BEGIN;


// 游戏状态数据包	（ 等待其他玩家开始 ）
typedef struct tagGameStationWaiting
{
	// 游戏的基本信息
	GAME_BASE_INFO  sBaseInfo;
}GAME_STATION_WAITING;

// 游戏状态数据包	（ 发牌状态 ）
typedef struct tagGameStationSendCard
{
	// 游戏的基本信息
	GAME_BASE_INFO  sBaseInfo;
	// 发牌数据包
	BEGIN_SEND_CARD sSendCard;
}GAME_STATION_SEND_CARD;

// 游戏状态数据包	（ 游戏中 ）
typedef struct tagGameStationPlaying
{
	// 游戏的基本信息
	GAME_BASE_INFO  sBaseInfo;

	// 发牌数据包
	BEGIN_SEND_CARD sSendCard;

	// 玩家位置
	UPDATE_CLIENT_VIEW sUpdateClientView;

	// 其他游戏信息 （游戏本身决定）

}GAME_STATION_PLAYING;



// 托管消息
typedef struct tagTrusteeCard
{
	BYTE byDeskStation;            // 座位号
	bool bIsTrustee;               // 是否为托管状态
	tagTrusteeCard()
	{
		memset(this, 0, sizeof(tagTrusteeCard));
	}
}TRUSTEE_CARD;

// 游戏强退、断线结束统计数据包
typedef struct tagGameCutFinish
{
	BYTE				byDeskStation;					// 退出位置
	int					iTurePoint[PLAY_COUNT];			// 各玩家得分
	int					iChangeMoney[PLAY_COUNT];		// 玩家银子
	int					iAwardPoint[PLAY_COUNT];		// 奖励分
	tagGameCutFinish()
	{
		memset(this, 0, sizeof(tagGameCutFinish));
	}
}GAME_CUT_FINISH;

//强退结构 add by wlr 20090807
typedef struct tagGameForceQuit
{
	BYTE				byDeskStation;					// 退出位置
	tagGameForceQuit()
	{
		byDeskStation = 0;
	}
}GAME_FORCE_QUIT;
//end of add by wlr 20090807

typedef struct tagHaveThing
{
	BYTE byDeskStation;                      // 有事要走的座位
	char szMessage[HAVE_THING_LENGHT];       // 消息内容
	tagHaveThing()
	{
		memset(this, 0, sizeof(tagHaveThing));
	}
}S_HAVE_THING;

typedef struct tagAgreeUserLeave
{
	BYTE byDeskStation;                      // 答复的座位
	BYTE byAgreeLeave;                       // 1 同意,其他不同意
	tagAgreeUserLeave()
	{
		memset(this, 0, sizeof(tagAgreeUserLeave));
	}
}S_AGREE_USER_LEAVE;

typedef struct tagWatchSetChange
{
	BYTE byDeskStation;
	BYTE byCardCount;
	WORD wCardList[USER_HAND_CARD_COUNT];
	tagWatchSetChange()
	{
		memset(this, 0, sizeof(tagWatchSetChange));
	}
}S_WATCH_SET_CHANGE;

///lym0511百家乐游戏开始信息
typedef struct tagBeginData
{
	BYTE  m_iResultInfo[MAXCOUNT][4];//最近MAXCOUNT把的信息	
	short int  m_iNTlist[PLAY_COUNT+1]; 
	BOOL bHaveUserInList;      //是否有非机器人在庄家列表中
	int  m_iShangZhuangLimit; //上庄需要的最少金币
	int  m_iBaSHu;            //本局（30把一局）进行的把数
	int  m_iZhuangBaShu;      //庄家进行了几把
	int  m_iNowNtStation;     //当前庄家的位置
	int  m_iGameCount;        //当前已经进行了几把
	int  m_iXiaZhuTime ;		//下注时间
	int  m_iKaiPaiTime ;		//开牌时间
	int  m_iFreeTime ;			//空闲时间
	int  m_iNowJuShu;			//本局进行的局数，进行到第几局（72把一局）
	__int64 	 iMaxNote;			//房间下注封顶
	__int64      m_iNTdata[4]; //庄家的位置,总分（当前金币数），成绩(赢的总金币)，局数（坐庄的局数）
	__int64	     m_iMaxZhu[8];        //每个区域能下的最大注	
	tagBeginData()
	{
		memset(this, 0, sizeof(tagBeginData));	
		m_iNowNtStation=-1;
	}
}BEGIN_DATA;
///lym0512
///下注数据包
typedef struct tagXiaZhu
{
	bool m_bIsAI;//是否机器人下注	
	int station;//位置
	__int64 money;//金额
	int type;//下注区域0-7
	int moneytype;//筹码类型：1：百，2：千，3：万，4：十万，5：百万，6：五百万
	__int64 m_iMaxZhu[8];//每个区域还能下多少注
	__int64 m_iMeZhongZhu;      //本把当前总注额
	__int64 m_iQuYuZhu[8];    //本把每个区域下的注额
	__int64 m_iAIMoney[8];		///机器人的下注	
	__int64 m_iUserXiaZhu[8];   //玩家当前在每个区域下的注额
	int  m_nPowerOfGold;//放大倍数
	CPoint point;
	tagXiaZhu()
	{
		memset(this, 0, sizeof(tagXiaZhu));
	}
}XIA_ZHU;
///lym0512
///开牌数据包
typedef struct tagKaiPai
{

	WORD    pai[2][3];        //庄闲的牌，0为庄，1为闲
	BYTE    m_CardCount[2];//双方的牌张数  
	BYTE    m_iResultInfo[MAXCOUNT][4]; //牌信息

	short   int  zhuangstation[PLAY_COUNT+1]; 	

	int  	 m_iHistoryPlayData[MAXCOUNT]; //保存历史开庄闲信息	
	int  	 m_iWinner;			//赢家1 庄，2闲，3和，本赢方
	int  	 m_iKaiPai;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和	
	int  	 m_iZPaiXing[5];   //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int      m_iXPaiXing[5];   //闲家牌型
	int      m_iWinQuYu[8];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和	

	__int64  m_iZhuangFen;     //庄家的得分
	__int64  m_iXianFen;	   //闲家的得分
	__int64  m_iUserFen;		//当前玩家的得分
	
	tagKaiPai()
	{
		memset(this, 0, sizeof(tagKaiPai));
		m_CardCount[0]=255;
		m_CardCount[1]=255;
		memset(zhuangstation, -1, sizeof(zhuangstation));		
	}
}KAI_PAI;
///结算数据包lym1204
typedef struct tagJieSuan
{
	BYTE  m_iResultInfo[MAXCOUNT][4]; //牌信息

	__int64  m_iZhuangFen;		//庄家的得分
	__int64  m_iXianFen;		//闲家的得分

	__int64  m_iUserFen;		//当前玩家的得分 (成绩)
	__int64  m_iNtWin;          //当前庄家赢的金币（成绩）


	int  m_iZPaiXing[5];	//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int  m_iXPaiXing[5];	//闲家牌型	
	int  zhuangstation[4];	//庄家列表
	__int64   m_iWinQuYu[8];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	__int64   m_iMeFanFen;			//本把玩家返还的分，开和时出现
	int m_iUserXiaZhuInfo[MAXCOUNT];	//玩家下注输赢和等信息
	int m_iResultInfoArea[MAXCOUNT];	//玩家下注输赢和等信息
	tagJieSuan()
	{
		memset(this, 0, sizeof(tagJieSuan));
		memset(m_iUserXiaZhuInfo, 0, sizeof(m_iUserXiaZhuInfo));
		memset(m_iResultInfoArea, 0, sizeof(m_iResultInfoArea));
	}
}JIE_SUAN;
///lym0512
///上庄数据包
typedef struct tagShangZhuang
{
	bool shang;		//true为上庄，false 为下庄
	bool bXiaZhu;   //是否下注状态
	short int  zhuangstation[PLAY_COUNT+1]; 	
	bool isAI;		//是否机器人上庄
	int station;   //申请的位置
	int  m_iNowNtStation;		//当前庄家的位置	
	int  success;	//是否失败	
	__int64	 m_iMaxZhu[8];        //每个区域能下的最大注
	tagShangZhuang()
	{
		memset(this, 0, sizeof(tagShangZhuang));
		memset(zhuangstation, -1, sizeof(zhuangstation));
		m_iNowNtStation = -1;
	}

}SHANG_ZHUANG;

///断线重连数据包
typedef struct tagChongLian
{
	//int  zhuangstation[4];		//庄家列表
	/// 庄家列表总的庄家位置 最后一名存诸队表总数
	short int  zhuangstation[PLAY_COUNT+1]; 	
	WORD  pai[2][3];			//庄闲的牌，0为庄，1为闲
	int  m_iZPaiXing[5];		//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int  m_iXPaiXing[5];		//闲家牌型
	__int64  m_iZhuangFen;			//庄家的得分
	__int64  m_iXianFen;			//闲家的得分
	__int64  m_iUserFen;			//当前玩家的得分
	int  m_iWinner;				//赢家1 庄，2闲，3和，本赢方
	int  m_iKaiPai;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	BYTE m_CardCount[2];		//双方的牌张数   
	int  m_iNTdata[4];			//庄家的位置,总分（当前金币数），成绩(赢的总金币)，局数（坐庄的局数）
	BYTE  m_iResultInfo[MAXCOUNT][4];	//最近30局的信息
	__int64  m_iShangZhuangLimit;	//上庄需要的最少金币
	int  m_iBaSHu;				//本局（30把一局）进行的把数
	int  m_iZhuangBaShu;		//庄家进行了几把
	int  m_iNowNtStation;		//当前庄家的位置
	int  m_iGameCount;			//当前已经进行了几把
	int  m_iSYTime;             //剩余时间

	__int64 	 iMaxZhu[8];			//每个区域能下的最大注
	__int64      iZhongZhu;			//本把当前总注额
	__int64  m_iQuYuZhu[8];			//本把每个区域下的注额
	__int64  m_iQuYuJinBi[8][6];	//每区域下各类（共6类）金币的个数
	int	 m_iMeZhu[9];			//我在每个区域下的注 和总注 0-7每个区域的下注，8总注lym 
	int  m_iAIMoney[8];			///机器人的下注
	int  m_iXiaZhuTime ;		//下注时间
	int  m_iKaiPaiTime ;		//开牌时间
	int  m_iFreeTime ;			//空闲时间
	int  m_iSendInterval;		//发牌间隔时间
	int  m_iSendCardTime;		//发牌时间(ms)
	int  m_iSendCardZhen;		//发牌帧数
	BYTE m_bGameStation;		///< 游戏状态lym2010-03-19
	int  m_iNowJuShu;			//本局进行的局数，进行到第几局（72把一局）
	bool m_bIsJieSuan;			//是否已经结算2010-05-21
	__int64   iMaxNote;			//房间下注封顶
	__int64  m_iBaseMaxNote;		//普通玩家最大下注
	BOOL bIsVipDouble;               /**<是否为双倍下注房间*/
	int m_iUserXiaZhuInfo[MAXCOUNT];	//玩家下注输赢和等信息
	int m_iResultInfoArea[MAXCOUNT];	//历史开庄闲和和等信息
	tagChongLian()
	{
		memset(this, 0, sizeof(tagChongLian));
		memset(zhuangstation, -1, sizeof(zhuangstation));	
		m_iNowNtStation=-1;
		m_bGameStation= 255;

	}

}CHONG_LIAN;

///等待消息数据包
typedef struct tagDengDai
{	
	//int  zhuangstation[4];//庄家列表
	/// 庄家列表总的庄家位置 最后一名存诸队表总数
	short int  zhuangstation[PLAY_COUNT+1]; 	
	__int64  m_iNtWin;              //当前庄家赢的金币
	int  m_iZhuangBaShu;        //庄家进行了几把
	tagDengDai()
	{
		memset(zhuangstation, -1, sizeof(zhuangstation));
		m_iNtWin = 0;              //当前庄家赢的金币
		m_iZhuangBaShu = 0;        //庄家进行了几把
	}

}DENG_DAI;

///玩家的得分信息
typedef struct tagUserScore
{
	bool     bShowInfo[PLAY_COUNT] ; 
	__int64  llUserFen[PLAY_COUNT] ; ///所有玩家的得分情况

	tagUserScore()
	{
		memset(this ,0 ,sizeof(tagUserScore)) ; 
	}

}USER_SCORE_STRUCT;  


///开始前的洗好的牌数据lym2010-03-17
typedef struct tagResultData
{
	WORD  Z_Pai[MAXCOUNT][3];			//庄的72把牌数据
	WORD  X_Pai[MAXCOUNT][3];			//闲的72把牌数据
	BYTE  m_iWinner[MAXCOUNT];			//赢家1 庄，2闲，3和，本赢方
	BYTE  m_iKaiPai[MAXCOUNT];			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	BYTE  m_iZPaiXing[MAXCOUNT][5];		//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	BYTE  m_iXPaiXing[MAXCOUNT][5];		//闲家牌型
	BYTE  m_iWinQuYu[MAXCOUNT][8];		//赢牌区域
	BYTE  m_CardCount[MAXCOUNT][2];		//双方的牌张数 
	BYTE  m_iNowBaShu;					//当前把数

	tagResultData()
	{
		Init();
	}
	void Init()
	{
		memset(Z_Pai, 0, sizeof(Z_Pai));
		memset(X_Pai, 0, sizeof(X_Pai));
		memset(m_iWinner, 0, sizeof(m_iWinner));
		memset(m_iKaiPai, 0, sizeof(m_iKaiPai));
		memset(m_iZPaiXing, 0, sizeof(m_iZPaiXing));
		memset(m_iXPaiXing, 0, sizeof(m_iXPaiXing));
		memset(m_iWinQuYu, 0, sizeof(m_iWinQuYu));
		memset(m_CardCount, 0, sizeof(m_CardCount));
		m_iNowBaShu = 0;				//当前把数
	}
}RESULT_DATA;

///lym0512
///测试数据包
typedef struct tagTest
{
	int type;   //测试类型
	tagTest()
	{
		memset(this, 0, sizeof(tagTest));
	}

}BJL_TEST;

//控制庄家输赢数据包
typedef struct tagSuperSetting
{
	char name[256];	//控制玩家的昵称
	BYTE bDeskStation;//控制玩家的位置
	BYTE bState;   // 0:正常  1: 庄家赢 2: 庄家输
	tagSuperSetting()
	{
		memset(this, 0, sizeof(tagSuperSetting));
	}
}SUPERSETTING;

///通知客户端是否为超级客户端状态
typedef struct tagSuperState
{
	BYTE bDeskStation;
	BOOL bEnable;     // 是否开通
	__int64  m_iOldMomey[PLAY_COUNT];              //当前庄家赢的金币
	tagSuperState()
	{
		memset(this, 0, sizeof(tagSuperState));
	}
}SUPERSTATE;


///机器人输赢控制 ：机器人控制信息
typedef struct tagAICtrlInfo
{
	BYTE	bDeskStation;			///通知的位置
	bool	bAIWinAndLostAutoCtrl ;	///机器人输赢控制：是否开启机器人输赢自动控制
	int		iAIWantWinMoney;		///机器人输赢控制：机器人要赢的钱
	int		iAIHaveWinMoney;		///机器人输赢控制：机器人已经赢了多少钱
	int		iAIMaxWinInOne;			///机器人输赢控制：机器人平均每把最大赢钱数	（小于等于0时为不限）		
	int		iAICtrlCount;			///机器人输赢控制：当前机器人已经进行了自动控制的把数 
	bool    bIsClearAIWinMoney;     ///机器人赚的钱进行清零
	bool    bIsSetAICtrl;			///机器人重新设置机器人的配置
	tagAICtrlInfo()
	{
		memset(this, 0, sizeof(tagAICtrlInfo));
	}

}AICTRLINFO;




// 超级GM下注情况
typedef struct tagSuperGM 
{
	__int64 iUserStation;		/* 玩家的位置 */
	__int64	iOldMoney;			/* 玩家的带入金币 */
	__int64 iUserNote[8];		/* 0号区域的下注情况 */
	__int64	iAllNote;			/* 玩家总的下注情况 */
	TCHAR cUserName[MAX_PATH];	/* 玩家的姓名 */
	bool bUsed;					/* 使用了这个结构 仅作为判断 */
	bool bIsRobot;				/* 是否为机器人 */
	void Init()					/* 初始化 */
	{
		iUserStation = -1;
		iOldMoney = 0;
		memset(iUserNote, 0, sizeof(iUserNote));
		iAllNote = 0;
		memset(cUserName, 0, sizeof(TCHAR) * MAX_PATH);
		bUsed = false;
		bIsRobot = false;
	}
} SUPERGM;

typedef struct tagSuperData {

	SUPERGM sUserInfo[PLAY_COUNT];		/* 所有玩家的情况 */
	void Init()								/* 初始化 */
	{
		for (int i = 0; i < PLAY_COUNT; i ++)
		{
			sUserInfo[i].Init();
		}
	}
} SUPERDATA;


#define ASS_SUPER_SET_CARD         79        // 提牌器消息id，平台决定的游戏不能修改

/// 消息ID
#define ASS_UPDATE_CLIENT_VIEW    100        // 通知更新客户端界面  
#define ASS_SEND_CARD             101        // 发牌
#define ASS_GAME_BEGIN            102        // 发完牌游戏开始
#define ASS_CUT_END				  103		 // 用户强行离开
#define ASS_SAFE_END			  104		 // 用户安全结束

#define ASS_TRUSTEE               110        // 托管
#define ASS_HAVE_THING            120        // 有事要走消息
#define ASS_USER_AGREE_LEAVE      121        // 有事要走消息答复结果
#define ASS_ALL_USER_AGREEED      122        // 有事要走消息所有人答复完
#define ASS_WATCH_SET_CHANGE      123        // 玩家修改旁观属性
/// 下注消息
#define ASS_XIA_ZHU               130    
/// 开牌消息
#define ASS_KAI_PAI				  131        
/// 等待消息
#define ASS_DENG_DAI              132        
/// 上庄消息
#define ASS_SHANG_ZHUANG          133         
/// 百家乐开始消息
#define ASS_BJL_BEGIN             134        
/// 游戏无庄闲置消息消息
#define ASS_WU_ZHUANG             135    
/// 通知显示结算信息消息
#define ASS_SHOW_JS				  136        

/// 断线重连时申请游戏数据消息，lym2010-03-18
#define ASS_APPLY_GAME_DATA		  137   

#define ASS_USER_SCORE            138        ///玩家得分信息

#define ASS_BJL_TEST              140        // 测试消息

#define ASS_SUPER_SETTING         150        // 由客户端发送消息控制本局输赢
#define ASS_SUPER_STATE           151        // 通知客户端是否为超级状态
#define ASS_SUPER_AI_INFO         152        /////机器人输赢控制 ： 通知客户端机器人当前的信息

#define ASS_XIA_ZHU_FULL		  153        /////下注已满消息 add xyh 2011-12-6
#define ASS_CHANGE_NT			  154        /////换庄消息 add xyh 2011-12-8

#pragma pack(pop)
#endif // !defined(_POKERMESSAGE_H__INCLUDED_)
