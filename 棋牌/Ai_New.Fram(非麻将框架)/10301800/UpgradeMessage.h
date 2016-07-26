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
#define NAME_ID							10301800								 // 名字 ID
#else
#define NAME_ID							10301800								 // 名字 ID


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

#define	UG_VALUE_MASK					0x0F			//0000 1111
// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

#define PLAY_COUNT						180							        // 游戏人数

//游戏内部开发版本号
#define DEV_HIGH_VERSION				2				                        // 内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					0				                        // 内部开发低版本号

// 游戏状态定义
#define GS_WAIT_SETGAME					0				// 等待东家设置状态
#define GS_WAIT_AGREE					1				// 等待同意设置
#define GS_NOTE_STATE					20				// 下注状态
#define GS_SEND_CARD					21				// 发牌状态
#define GS_OPEN_RESULT					22				// 开牌状态
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


#define MAXCOUNT						30            


const	int	BET_ARES = 12;	//8个下注区域

struct LuziData
{
	BYTE		byZPoint;
	BYTE		byXpoint;

	BYTE		byWinResult;	//赢的区域 0=将 1-闲 2-和

	LuziData()
	{
		memset(this,255,sizeof(LuziData));
	}
};

/// 游戏基本信息
typedef struct tagGameBaseInfo
{
	//游戏版本
	BYTE byHighVersion;		// 游戏版本号
	BYTE byLowVersion;		// 游戏版本号



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
	short int  m_iNTlist[PLAY_COUNT+1]; 

	int  m_iShangZhuangLimit; //上庄需要的最少金币
	
	int  m_iXiaZhuTime ;		//下注时间
	int  m_iKaiPaiTime ;		//开牌时间
	int	 m_iShowResult;			//结算框显示时间
	int  m_iFreeTime ;			//空闲时间

	__int64 	 iMaxNote;			//房间下注封顶
	__int64	     m_iMaxZhu[BET_ARES];        //每个区域能下的最大注	

	int		m_iNowNtStation;     //当前庄家的位置
	int		m_iZhuangBaShu;      //庄家进行了几把
	__int64	m_i64NtMoney;		//庄家的金币数
	__int64	m_i64NtWin;			//庄家输赢情况

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

	int			station;//位置
	__int64		money;//金额
	int			type;//下注区域0-11
	int			moneytype;//筹码类型：1：百，2：千，3：万，4：十万，5：百万，6：五百万

	__int64 iMeZhongZhu;      //本把当前总注额
	__int64	iAllZhongZhu;		//所有总注

	__int64 m_iMaxZhu[BET_ARES];		//每个区域还能下多少注
	__int64 m_iQuYuZhu[BET_ARES];    //本把每个区域下的注额
	__int64 m_iAIMoney[BET_ARES];		///机器人的下注	
	__int64 m_iUserXiaZhu[BET_ARES];   //玩家当前在每个区域下的注额
	
	
	tagXiaZhu()
	{
		memset(this, 0, sizeof(tagXiaZhu));
	}
}XIA_ZHU;
///lym0512
///开牌数据包
typedef struct tagKaiPai
{

	BYTE    pai[2][3];        //庄闲的牌，0为庄，1为闲

	int  	 iZPaiXing[5];   //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int      iXPaiXing[5];   //闲家牌型
	int      iWinQuYu[BET_ARES];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和	

	
	tagKaiPai()
	{
		memset(this, 0, sizeof(tagKaiPai));
	}
}KAI_PAI;
///结算数据包lym1204
typedef struct tagJieSuan
{
	//20141020 dwjtest 这里有问题？
	LuziData	TLuziData[MAXCOUNT];	//路子信息
	__int64  i64ZhuangScore;		//庄家本局的得分
	__int64  i64MeScore;			//自己本局的得分
	__int64  i64MeFanFen;		//本把玩家返还的分，开和时出现
	__int64  i64UserWin;		//当前玩家的得分 （成绩）
	__int64  i64NtWin;          //当前庄家赢的金币（成绩）
	int		 iPlayCount;		//玩了多少盘
	int		 iWinCount;			//赢了多少盘

	tagJieSuan()
	{
		memset(this, 0, sizeof(tagJieSuan));
		memset(TLuziData,255,sizeof(TLuziData));
	}
}JIE_SUAN;
///lym0512
///上庄数据包
typedef struct tagShangZhuang
{
	bool shang;		//true为上庄，false 为下庄
	short int  zhuangstation[PLAY_COUNT+1]; 	
	bool isAI;		//是否机器人上庄
	int station;   //申请的位置
	int  m_iNowNtStation;		//当前庄家的位置	
	int  success;	//是否失败	 0-表示成功 1-金币不够 2-已经在庄家列表中

	tagShangZhuang()
	{
		memset(this, 0, sizeof(tagShangZhuang));
		memset(zhuangstation, -1, sizeof(zhuangstation));
		m_iNowNtStation = -1;
	}

}SHANG_ZHUANG;

/*--------------------------------------------------------------------------*/
//游戏基础数据
struct	GameStationBase
{
	BYTE		m_bGameStation;			///< 游戏状态
	
	int			m_iXiaZhuTime ;			//下注时间
	int			m_iKaiPaiTime ;			//开牌时间
	int			m_iShowResult;			//结算框显示时间
	int			m_iFreeTime ;			//空闲时间

	int			m_iZhuangBaShu;			//庄家进行了几把
	int			m_iNowNtStation;		//当前庄家的位置

	int			iZhuangWaitList[PLAY_COUNT+1];	//上庄列表 

	__int64		i64NtWin;				//庄家输赢
	__int64		i64MeWin;				//自己输赢

	__int64		iMaxNote;				//房间下注封顶
	__int64		m_iShangZhuangLimit;	//上庄需要的最少金币


	LuziData	TLuziData[MAXCOUNT];	//路子信息

	GameStationBase()
	{
		memset(this, 0, sizeof(GameStationBase));
		memset(TLuziData,255,sizeof(TLuziData));
	}
};
/*--------------------------------------------------------------------------*/
//下注状态数据
struct	GameStation_Bet:public GameStationBase
{
	__int64		i64AresMaxZhu[BET_ARES];			//每个区域还能下多少注
	__int64		i64XiaZhuData[BET_ARES];			//各区域的总下注
	__int64		i64UserXiaZhuData[BET_ARES];		//玩家在各区域的下注情况
	__int64		i64UserXiaZhuSum;					//玩家的总下注
	__int64		i64XiaZhuSum;						//所有玩家的总下注

	GameStation_Bet()
	{
		memset(this,0,sizeof(GameStation_Bet));
		memset(TLuziData,255,sizeof(TLuziData));
	}
};
/*--------------------------------------------------------------------------*/
//开牌状态
struct	GameStation_SendCard:public GameStationBase
{
	__int64		i64AresMaxZhu[BET_ARES];			//每个区域还能下多少注
	__int64		i64XiaZhuData[BET_ARES];			//各区域的总下注
	__int64		i64UserXiaZhuData[BET_ARES];		//玩家在各区域的下注情况
	__int64		i64UserXiaZhuSum;					//玩家的总下注
	__int64		i64XiaZhuSum;						//所有玩家的总下注

	BYTE		byUserCard[2][3];					//庄闲的牌数据
	BYTE		byZPaiXing;							//庄牌型
	BYTE		byXPaiXing;							//闲牌型
	BYTE		byWinAreas[BET_ARES];				//赢的区域//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

	GameStation_SendCard()
	{
		memset(this,0,sizeof(GameStation_Bet));
		memset(TLuziData,255,sizeof(TLuziData));
		memset(byUserCard,255,sizeof(byUserCard));
	}
};
/*--------------------------------------------------------------------------*/
//结算状态数据
struct	GameStation_Result:public GameStationBase
{
	__int64		i64AresMaxZhu[BET_ARES];			//每个区域还能下多少注
	__int64		i64XiaZhuData[BET_ARES];			//各区域的总下注
	__int64		i64UserXiaZhuData[BET_ARES];		//玩家在各区域的下注情况
	__int64		i64UserXiaZhuSum;					//玩家的总下注
	__int64		i64XiaZhuSum;						//所有玩家的总下注

	__int64		i64ZhuangScore;		//庄家本局的得分
	__int64		i64MeScore;			//自己本局的得分
	__int64		i64MeFanFen;		//本把玩家返还的分，开和时出现


	GameStation_Result()
	{
		memset(this,0,sizeof(GameStation_Result));
		memset(TLuziData,255,sizeof(TLuziData));
	}
};
/*--------------------------------------------------------------------------*/





///等待消息数据包
typedef struct tagDengDai
{	
	short int	zhuangstation[PLAY_COUNT+1]; 	
	int			iNtStation;
	__int64		i64NtWin;              //当前庄家赢的金币
	int			iZhuangBaShu;        //庄家进行了几把
	tagDengDai()
	{
		
		memset(zhuangstation, -1, sizeof(zhuangstation));
		i64NtWin = 0;              //当前庄家赢的金币
		iZhuangBaShu = 0;        //庄家进行了几把
		iNtStation = -1;
	}

}DENG_DAI;

///无庄息数据包
typedef struct tagNoZhuang
{	
	int			iNtStation;
	__int64		i64NtWin;              //当前庄家赢的金币
	int			iZhuangBaShu;        //庄家进行了几把
	tagNoZhuang()
	{
		i64NtWin = 0;              //当前庄家赢的金币
		iZhuangBaShu = 0;        //庄家进行了几把
		iNtStation = -1;
	}

}NO_ZHUANG;

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
	BYTE bDeskStation;//控制玩家的位置
	BYTE bState;   // 0:正常  1: 庄家赢 2: 庄家输 3:和
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

	bool    bIsClearAIWinMoney;     ///机器人赚的钱进行清零
	bool    bIsSetAICtrl;			///机器人重新设置机器人的配置
	tagAICtrlInfo()
	{
		memset(this, 0, sizeof(tagAICtrlInfo));
	}

}AICTRLINFO;









#define ASS_SUPER_SET_CARD         79        // 提牌器消息id，平台决定的游戏不能修改

/// 消息ID


#define ASS_CUT_END				  103		 // 用户强行离开
#define ASS_SAFE_END			  104		 // 用户安全结束

/// 下注消息
#define ASS_XIA_ZHU               130    
/// 开牌消息
#define ASS_KAI_PAI				  131        
/// 游戏结束 空闲消息 
#define ASS_FREE_STATE            132        
/// 上庄消息
#define ASS_SHANG_ZHUANG          133   
/// 上庄消息错误
#define ASS_SHANG_ZHUANG_ERR      143         
/// 百家乐开始消息
#define ASS_BJL_BEGIN             134        
/// 游戏无庄闲置消息消息
#define ASS_WU_ZHUANG             135    
/// 通知显示结算信息消息
#define ASS_SHOW_JS				  136        


#define ASS_SUPER_SETTING         150        // 由客户端发送消息控制本局输赢
#define ASS_SUPER_STATE           151        // 通知客户端是否为超级状态
#define ASS_SUPER_AI_INFO         152        /////机器人输赢控制 ： 通知客户端机器人当前的信息

#define ASS_CHANGE_NT			  154        /////换庄消息 add xyh 2011-12-8

#pragma pack(pop)
#endif // !defined(_POKERMESSAGE_H__INCLUDED_)
