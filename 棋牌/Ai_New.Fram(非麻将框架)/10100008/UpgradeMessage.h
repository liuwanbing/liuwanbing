
// ******************************************************************************************************
//  UpgradeMessage   version:  1.0   ·  date: 03/29/2011
//  --------------------------------------------------------------------------------------------------

//	德克萨斯扑克，游戏服务器-客户端协议\
NameID：51300280

//  --------------------------------------------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ******************************************************************************************************
// 
// ******************************************************************************************************

#pragma once

#ifndef AFC_SERVER_DKSSPK_11302800_MESSAGE_HEAD_FILE
#define AFC_SERVER_DKSSPK_11302800_MESSAGE_HEAD_FILE

#define SUPER_PLAYER                    10				// 超级玩家

//文件名字定义
#define GAMENAME						TEXT("德克萨斯扑克")

//版本定义
#define GAME_MAX_VER					1				// 现在最高版本
#define GAME_LESS_VER					1				// 现在最低版本
#define GAME_CHANGE_VER					0				// 修改版本

//游戏开发版本
#define DEV_HEIGHT_VERSION				10				// 内部开发高版本号
#define DEV_LOW_VERSION					1				// 内部开发低版本号

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

//游戏信息
#define NAME_ID							11302800		// 名字 ID
#define PLAY_COUNT						8				// 游戏人数
#define GET_SERVER_INI_NAME(str)		sprintf(str,"%s%d_s.ini",CINIFile::GetAppPath(),NAME_ID);
#define MAX_NAME_INFO                   256

//游戏数据长度相关
#define MAX_CARDS_NUM					216				// 四副牌的总牌数，也是所支持最大的牌数
#define MAX_HANDCARDS_NUM				52				// 手上持有的最大的牌张数
#define MAX_510K_CARDS_NUM				48				// 四副牌5/10/K最大的牌张数
#define MAX_FIRST_CARDS					1				// 发牌数量
#define MAX_DEAL_CARDS					2				// 发牌数量
#define MAX_THREE_CARDS					3				// 发牌数量
#define MAX_BACKCARDS_NUM				16				// 最大的底牌张数

#define TYPE_PLAY_GAME					10000			// 是玩家



#define TAG_INIT(tagName) \
    tagName(){Clear();} \
    void Clear()

//******************************************************************************************
//数据包处理辅助标识
//******************************************************************************************

#define ASS_UG_USER_SET					50				//用户设置游戏
#define ASS_UG_USER_SET_CHANGE			51				//玩家设置更改

#define ASS_GAME_BEGIN					120				//升级游戏开始
#define ASS_SYS_DESK_CFG				121				//确定定注结果消息
#define ASS_CFG_DESK					122				//玩家设置底注
#define ASS_DESK_CFG					123				//确定定注结果消息
#define ASS_AGREE_DESK_CFG				124             //同意底注
#define ASS_BET							125             //玩家下注信息
#define ASS_SEND_A_CARD					130				//发1张牌信息
#define ASS_SEND_3_CARD					131				//发3张牌信息
#define ASS_SEND_4_5_CARD				132				//发4,5张牌信息
#define ASS_SEND_CARD_FINISH			133				//发牌结束
#define ASS_BETPOOL_UP					134				//边池更新
#define ASS_BET_REQ						135				//下注请求
#define ASS_BET_INFO					136				//下注消息
#define ASS_NEW_USER					137				//下一个玩家操作
#define ASS_PASS_USER					138				//弃牌的玩家操作
#define ASS_SUB_MENOY                   139             //代入金币限制
#define ASS_COMPARE_CARD                140             //比牌信息

#define ASS_CALLMANDATE				    141				//跟注托管
#define ASS_CHECKMANDATE                142             //过牌托管
#define ASS_CALLANYMANDATE              143				//跟任何注托管
#define ASS_PASSABANDONMANDATE          144             //过牌或弃牌托管

#define ASS_USER_LEFT_DESK              145             ///

#define ASS_SORT_OUT					170				//清理图片

#define ASS_TOKEN						186				//令牌信息
#define ASS_RESULT						191				//回合算分
#define ASS_SHOW_IN_MONEY				192				//显示代入金额



//异常类消息
#define ASS_NO_PLAYER					155				//没有玩家进行游戏
#define ASS_AGREE						156				//玩家是否同意的消息
#define ASS_CFG_DESK_TIMEOUT			157				//设置底注超时
#define ASS_NOT_ENOUGH_MONEY			158				//玩家金币不足的清除玩家信息
#define ASS_MONEY_LACK					159				//玩家金币不足的提示

#define ASS_NEXT_ROUND_REQ				150				//玩家下一回合请求
#define ASS_NEW_ROUND_BEGIN				151				//下一回合开始


//调试类消息
#define ASS_FINISH_ROUND_REQ			180				//结束本轮游戏

//******************************************************************************************
// 下注类型
enum emType
{
    ET_UNKNOWN 		= 0,	//未知类型
    ET_BET			= 1,	//下注
    ET_CALL			= 2,	//跟注
    ET_ADDNOTE 		= 3,	//加注
    ET_CHECK		= 4,	//过牌
    ET_FOLD    		= 5,    //弃牌
    ET_ALLIN		= 6,	//全下
    ET_AUTO			= 7,    //自动下注
};

// 发牌类型
enum emTypeCard
{
    UNKNOWN			= 0,	//未知类型
    SEND_A_CAND		= 1,	//下发一张牌牌
    SEND_3_CAND		= 2,	//下发 3 张牌牌
    SEND_4_5_CAND   = 3,	//下发 4, 5 张牌牌
};

// 代入金额类型
enum emTypeMoney
{
	NT_UNKNOWN	= 0,	//未知类型
	NT_LOAD		= 1,	//确定代入金额
	NT_EXIT		= 2,    //退出游戏
	NT_ADD		= 3,    //增加金额
	NT_SUB		= 4,    //减少金额
};

// 托管状态
enum emToken
{
	TO_UNKNOWN					= 0,	// 未知类型
	TO_CALLMANDATE				= 1,	// 跟注托管按钮
	TO_CHECKMANDATE				= 2,    // 过牌托管按钮
	TO_CALLANYMANDATE			= 3,    // 跟任何注按钮
	TO_PASSABANDONMANDAT		= 4,    // 过牌/弃牌托管按钮
};


//通知设置游戏包
struct TCfgDesk
{
    //底注相关
    struct TDizhu
    {
        bool	bCanSet;		//玩家可以是否可以设置，下发时用
        int		iLowers[5];		//存放5个可选的底注值，下发时用
        bool    bSelects[5];	//这5个值的可选择性
        BYTE	bySelect;		//玩家从iValues选择了哪一个底注值，上发时用

        void Clear()
        {
            bCanSet = false;	//玩家可以是否可以设置
            memset(iLowers,0,sizeof(iLowers));		 //存放三个可选的底注值
            memset(bSelects,false,sizeof(bSelects)); //这三个值的可选择性
            bySelect = 255;	//玩家从iValues选择了哪一个底注值
        }

    } dz;

    TAG_INIT(TCfgDesk)
    {
        dz.Clear();
    }

};

//有关桌面配置
struct TDeskCfg
{
    //底注相关
    struct TDizhu
    {
        __int64	iLowers[5];						// 存放5个可选的底注值
        __int64	iUppers[5];						// 存放5个顶注值
        __int64 iRoomMultiple;                  // 房间倍数
        __int64	iLower;							// 系统自动设置的底注值
        __int64	iUpper;							// 系统自动设置的顶注值
        __int64	iSmallBlindNote;				// 小盲注设置的底注值
        __int64	iBigBlindNote;					// 大盲注设置的底注值

        // 代入金币设置
        bool    bIsRoomSubMoney;                // 此房间是否需要代入金币才可玩
        __int64 iSubMinMoney;                   // 本房间最低代入金币
        __int64 iSubMaxMoney;                   // 本房间最高代入金币 
		__int64	iSubPlanMoney;					// 当前准备代入金币
		__int64	iMinusMoney;					// 修改注额加减金币
		__int64	iMinMoney;						// 最小下注金币
		__int64	iUntenMoney;					// 少于多少金额时带入 

		emTypeMoney nType;						// 请求包类型

        void Clear()
        {
            memset(iLowers,0,sizeof(iLowers));	// 存放5个可选的底注值
            memset(iUppers,0,sizeof(iUppers));	// 存放5个顶注值
            iLower = 0;							// 系统自动设置的底注值
            iUpper = 0;							// 系统自动设置的顶注值
            iSmallBlindNote = 0;				// 小盲注设置的底注值
            iBigBlindNote = 0;					// 大盲注设置的底注值
			nType = NT_UNKNOWN;					// 未知类型
        }

    } dz;

    //时间相关
    struct TTime
    {
        BYTE   byOperate;    	 // 玩家操作的时间

        void Clear()
        {
            byOperate = 0;	 	 // 玩家操作的时间
        }
    }Time;

    //规则相关
    struct TRule
    {
        int  nPokeNum;			//使用多少幅扑克
        void Clear()
        {
            nPokeNum = 1;		//使用多少幅扑克
        }

    }Rule;

    //其它共性
    bool bShowUserInfo;			// 是否显示玩家和身份，多用于比赛场

    TAG_INIT(TDeskCfg)
    {
        dz.Clear();
        Time.Clear();
        Rule.Clear();
    }

};

//某玩家是否意底注的消息包
struct TAgree 
{
    BYTE byUser; 				// 消息所属的玩家
    bool bAgree; 				// 是否同意
    bool bAllAgree; 			// 当前是否所有玩家都同意游戏

    TAG_INIT(TAgree)
    {
        byUser = 255;			// 消息所属的玩家
        bAgree = true;			// 是否同意
        bAllAgree = false;		// 当前是否所有玩家都同意游戏
    }
};

//请求下注包
struct TBet
{
    emType	nType;				// 下注类型
    BYTE	byUser;				// 下注的玩家
    BYTE	bUserman[8];			// 弃牌玩家
    __int64 nMoney;				// 金币
	__int64	nBetMoney;			// 下注最大的玩家

    TAG_INIT(TBet)
    { 
        nType = ET_UNKNOWN;		// 下注类型
        byUser = 255;			// 下注的玩家
        //bUserman = 255;			// 弃牌玩家
		memset(bUserman,0,sizeof(bUserman));
        nMoney = 0;				// 玩家的选择
		nBetMoney = 0;			// 下注最大的玩家
    }
};

// 代入金币
struct TSubMoney
{
	BYTE	bDeskStation;
	__int64 nMoney[PLAY_COUNT];
	bool	bIn[PLAY_COUNT];       // 准备状态
	
	TSubMoney()
	{
		memset(nMoney, 0, sizeof(nMoney));
		memset(bIn, 0, sizeof(bIn));
		bDeskStation = 255;
	}
}rmb;


// 比牌数据包
struct TCompareCard
{
    int  nCardKind[PLAY_COUNT];		 // 玩家牌型
    BYTE bCards[PLAY_COUNT][5];		 // 所有玩家组成的牌数据
	BYTE bCardsEx[PLAY_COUNT][2];    // 剩下2张没用的牌
	BYTE bHandCards[PLAY_COUNT][2];  // 玩家手牌
	int nCardCount;                  // 组合牌张数
    int nHandCardCount[PLAY_COUNT];  // 玩家手牌张数

    TCompareCard()
    {
        memset(this, 0, sizeof(TCompareCard));
    }
};

//用于下发牌数据给客户端的数据包
struct TCards
{
    emTypeCard		nTypeCard;					//发牌类型
    BYTE   			byUser;						//该牌发给的玩家
    BYTE			byCards[MAX_HANDCARDS_NUM];	//牌数据
    int				iCardsNum;					//各玩家手上牌的数量

    TAG_INIT(TCards)
    {
        nTypeCard	= UNKNOWN;					//发牌类型
        byUser		= 255;						//该牌发给的玩家
        memset(byCards,0,sizeof(byCards));		//牌数据
        iCardsNum = 0;							//玩家手上牌的数量
    }
};

//令牌包，用来激活用户进行活动，如出牌等等
struct TToken
{
    BYTE	  byUser;									// 所属玩家
    BYTE	  byVerbFlag;								// 允许动作标志
    int		  iSelPoints[5];							// 加注值选择表
	emToken	  nemToken;									// 托管状态类型
    bool	  bNewTurn;                                 // 是否为新的一轮开始
	__int64   nCallMoney;                               // 当前可以跟注的金币

    TAG_INIT(TToken)
    {
		nemToken = TO_UNKNOWN;						// 未知类型
        byUser = 255;								// 所属玩家
        byVerbFlag = 0;								// 允许动作标志
        bNewTurn = false;
		nCallMoney = 0;
        memset(iSelPoints,0,sizeof(iSelPoints));	// 加注值选择表
    }
};

// 玩家下注边池更新包
struct TBetPool
{
    __int64 iBetPools[8];			//下注边池

    TAG_INIT(TBetPool)
    {
        memset(iBetPools, 0, sizeof(iBetPools));  //边池初始化
    }
};


//结算结构包
struct TResult
{
	int  nbyUserID[PLAY_COUNT];				//赢家ID
	char szName[8][61];						//赢家昵称
    __int64  nBetPools[8];					//所有下注池
    __int64  nUserBet[PLAY_COUNT][8];		//每个玩家下注数据
	bool bWinBetPools[PLAY_COUNT][8];		//赢的下注池
	__int64  nWinPoolsMoney[PLAY_COUNT][8];	//赢的下注池金币
    __int64  nScore[PLAY_COUNT];     		//输赢积分
    __int64  nMoney[PLAY_COUNT];     		//实际输赢金币
	__int64  nMoneyEx[PLAY_COUNT];          //输赢金币 
    __int64  nSubMoney[PLAY_COUNT];  		//当前每个玩家代入金币
    __int64  nSelfMoney[PLAY_COUNT]; 		//自己的金币更新: 客户端获取的金币有可能还没有刷新, 所以在这里发下去
	bool bWin[PLAY_COUNT];       			//记录赢家
	bool bExit;                  			//true:玩家需要退出, false:不需要退出
	BYTE bOrder[PLAY_COUNT];                //玩家排名, 按金币最多的排

    TResult()
    {
        ::memset(this, 0, sizeof(TResult));
    }
};

//新的一回合请求包
struct TNextRoundReq
{
    enum emType
    {
        UNKNOWN		= 0,	//未知类型
        READY		= 1,	//准备好
        EXIT		= 2,    //想退出游戏
    };

    emType nType;			//请求包类型

    TAG_INIT(TNextRoundReq)
    {
        nType = UNKNOWN;	//未知类型
    }
};

//新的一回合消息包
struct TNextRoundBeginInfo
{
    BYTE byNTUser;		//庄家位置
    BYTE bSmallBlind;	//小盲注
    BYTE bBigBlind;		//大盲注

    TAG_INIT(TNextRoundBeginInfo)
    {
        ::memset(this,0,sizeof(TNextRoundBeginInfo));
        byNTUser = 255;
        bSmallBlind = 255;
        bBigBlind = 255;
    }
};

//没有玩家在桌的通知
struct TNoPlayer
{
    bool bGameFinished;	//桌子已散掉

    TAG_INIT(TNoPlayer)
    {
        bGameFinished = false;	//桌子已散掉
    }
};

// 玩家金额不足
struct TMoneyLack
{
	int nbyUser;			// 金额不足玩家

	TAG_INIT(TMoneyLack)
	{
		nbyUser = 255;
	}
};

//******************************************************************************************
// 游戏状态定义
//******************************************************************************************

#define GS_WAIT_SETGAME					0	//等待东家设置状态
#define GS_WAIT_ARGEE					1	//等待同意设置
#define GS_CONFIG_NOTE					20	//设置底注状态
#define GS_AGREE_NOTE					21	//同意底注状态
#define GS_PLAY_GAME					25	//游戏过程状态
#define GS_SEND_CARD					26  //发牌状态
#define GS_WAIT_NEXT					27  //下一回合开始
#define GS_WAIT_NEXT_ROUND				28	//等待下一回合结束状态

/******************************************************************************************
*									德克萨斯扑克状态定义                                  *
*******************************************************************************************/

#define GS_USER_STAKE					30	// 玩家下注额状态

/******************************************************************************************
*																						  *
*******************************************************************************************/
//游戏状态包的基本
struct TGSBase 
{
    /*-GSBase基本属性-*/
    int  iEvPassTime;		//当前事件已消耗的时间（秒）
    int	 iVerbTime;			//游戏中各动作的思考时间
	bool bBoy[PLAY_COUNT];  //玩家性别
	bool bHaveUser[PLAY_COUNT]; //桌上各位置有没有玩家

    /*-桌面基本配置-*/
    TDeskCfg tagDeskCfg;

    TAG_INIT(TGSBase)
    {
        ::memset(this,0,sizeof(TGSBase));
    }

};

//游戏状态包：等待同意/未开始时
struct TGSWaitAgree : public TGSBase 
{
	bool	  bReady[PLAY_COUNT];               //是否准备
    __int64   nSubMoney[PLAY_COUNT];            //每个玩家代入的金币

    TAG_INIT(TGSWaitAgree)
    {
        ::memset(this,0,sizeof(TGSBase));
    }
};

//游戏状态包：游戏中
struct TGSPlaying : public TGSBase 
{
    BYTE  byHandCard[2];	                //玩家手上的扑克
    int   nHandCardNums;		            //玩家手上扑克数目

    BYTE  byCards[5];                       //桌面上公共牌
    int   nCardsCount;                      //桌面公共牌数
	int   nID[5];							//五个金额按钮
    __int64   nBetMoney[PLAY_COUNT][8];     //下注金币
    bool  bNoBet[PLAY_COUNT];               //记录不可下注的玩家位置
    bool  bGiveUp[PLAY_COUNT];              //记录放弃的玩家位置

    BYTE  byTokenUser;		                //得到令牌的用户
    BYTE  byNTUser;					        //庄家
    BYTE  bySmallBlind;                     //小盲家
    BYTE  byBigBlind;                       //大肓家
	__int64	  nCallMoney;					//跟注金币

    BYTE  byVerbFlag;                       //操作属性
    bool  bNewTurn;                         //新的一轮操作

    __int64   nBetPools[8];			        //下注边池金币
    __int64   nSubMoney[PLAY_COUNT];        //每个玩家代入的金币

	int   iUserStation ;
 
    TAG_INIT(TGSPlaying)
    {
        ::memset(this,0,sizeof(TGSPlaying));
    }
};

//游戏状态包：等待下一回合
struct TGSWaitNextRound : public TGSBase 
{
    __int64   nSubMoney[PLAY_COUNT];            //每个玩家代入的金币

    TAG_INIT(TGSWaitNextRound)
    {
        ::memset(this,0,sizeof(TGSPlaying));
    }
};
struct UserLeftDeskStruct
{
	BYTE    bDeskStation ; 
	bool    bClearLogo ;
};

#endif
