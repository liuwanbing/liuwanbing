// GameMsgPkg.h: game message package and define const.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UPGRADEMESSAGE_H_INCLUDE_)
#define _UPGRADEMESSAGE_H_INCLUDE_

#pragma pack(push,1)

#define CHECK_DEBUG   1

// 游戏名称信息
#define GAMENAME						TEXT("百变牛牛")                          // 游戏名称
#define GAME_DLL_AUTHOR                 TEXT("深圳红鸟网科技有限公司")            //
#define GAME_DLL_NOTE                   TEXT("牌类 -- 游戏组件")


#define NAME_ID							11100500								 // 名字 ID


#define MAX_NAME_INFO                   256
#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)

#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录
#define SKIN_CARD						TEXT("CardSkin")		                 // 共用图片文件夹
//-------------------------------------------------------------------------
#define GET_SKIN_FOLDER(A)				(sprintf(A,"%d",NAME_ID),strcat(A,""))
static TCHAR szTempStr[MAX_NAME_INFO] =		{0};

// 版本定义
#define GAME_MAX_VER					1								        // 现在最高版本
#define GAME_LESS_VER					1								        // 现在最低版本
#define GAME_CHANGE_VER					0								        // 修改版本

// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

#define PLAY_COUNT						180							        // 游戏人数

//游戏内部开发版本号
#define DEV_HIGH_VERSION				2				                        // 内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					1				                        // 内部开发低版本号

// 游戏状态定义
#define GS_WAIT_SETGAME					0				// 等待东家设置状态
#define GS_WAIT_AGREE					1				// 等待同意设置
#define GS_FREE							20				// 空闲状态
#define GS_BET							21				// 下注状态
#define GS_SEND_CARD					22				// 发牌状态
#define GS_PLAY_GAME					23				// 结算状态
#define GS_WAIT_NEXT					24				// 等待下一盘开始 

//百变牛牛下注区域定义定义
#define GF_TIANG				0			   // 天
#define GF_DI			        1			   // 地
#define GF_XUAN  				2			   // 玄
#define GF_HUANG				3			   // 黄

#define MAX_NOTE_COUNT          4              //下注区个数  本游戏人天 ，地 ，玄 ，黄 四个下注区

/** 保存路子把数 */
#define  GAME_SAVE_COUNT  72

/*------------------------------------------------------------------------*/
//断线重连基础数据包
struct	TagGameStation_Base
{
	
	BYTE		byGameStaion;	//游戏状态

	BYTE		bMaxBase;		/// 最大倍率即牛牛的倍率
	
	int			iXiaZhuTime ;	///下注时间
	int			iKaiPaiTime ;	///开牌时间		
	int			iFreeTime ;		///空闲时间	
	int			iRemaindTime;	///剩余时间

	int			iNowNtStation;	///当前庄家的位置
	int			iGameBeen;		///当前局数		
	int			iZhuangBaShu;	///庄家进行了几把	
	int			iGameCount;		///当前已经进行了几把
	__int64		i64NtWin;		///庄家输赢成绩
	
	int			iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	///最近X局的信息	
	short int	idzhuangstation[PLAY_COUNT+1][2];			//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;

	__int64		i64ShangZhuangLimit;			///上庄需要的最少金币
	__int64		i64MaxNote;						///个人游戏最大下注数
	__int64		i64RectNoteMax[MAX_NOTE_COUNT]; ///区域下注限制
	__int64		i64ChoumaValue[6];				// 六个筹码的值
	
	
	TagGameStation_Base()
	{
		memset(this,0,sizeof(TagGameStation_Base));
	}
};
/*------------------------------------------------------------------------*/
//断线重连空闲阶段数据包
struct	TagGameStation_WaiteAgee:public TagGameStation_Base
{
	TagGameStation_WaiteAgee()
	{
		memset(this,0,sizeof(TagGameStation_WaiteAgee));
	}
};
/*------------------------------------------------------------------------*/
//断线重连下注阶段数据包
struct	TagGameStation_Bet
{
	BYTE		byGameStaion;	//游戏状态
	BYTE		bMaxBase;		/// 最大倍率即牛牛的倍率

	int			iXiaZhuTime ;	///下注时间
	int			iKaiPaiTime ;	///开牌时间		
	int			iFreeTime ;		///空闲时间	
	int			iRemaindTime;	///剩余时间

	int			iNowNtStation;	///当前庄家的位置
	int			iGameBeen;		///当前局数		
	int			iZhuangBaShu;	///庄家进行了几把	
	int			iGameCount;		///当前已经进行了几把
	
	short int	idzhuangstation[PLAY_COUNT+1][2];			//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	char		iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	///最近X局的信息	
	
	__int64		i64NtWin;		///庄家输赢成绩
	__int64		i64ShangZhuangLimit;			///上庄需要的最少金币
	__int64		i64MaxNote;						///个人游戏最大下注数
	__int64		i64RectNoteMax[MAX_NOTE_COUNT]; ///区域下注限制
	__int64		i64ChoumaValue[6];				// 六个筹码的值
	
	__int64  iZhongZhu;						///本把当前总注额		
	__int64  iQuYuZhu[MAX_NOTE_COUNT];		///本把每个区域下的注额
	__int64  iQuYuZhuTrue[MAX_NOTE_COUNT];	///本把真实每个区域下的注额
	__int64  iMyZhu[MAX_NOTE_COUNT];		///本把自已下注
	TagGameStation_Bet()
	{
		memset(this,0,sizeof(TagGameStation_Bet));
	}
};
/*------------------------------------------------------------------------*/
//断线重连发牌阶段数据包
struct	TagGameStation_OpenCard
{
	BYTE		byGameStaion;	//游戏状态
	BYTE		bMaxBase;		/// 最大倍率即牛牛的倍率

	
	BYTE		byCard[5][5];	
	BYTE		byCardCount[5];

	int			iXiaZhuTime ;	///下注时间
	int			iKaiPaiTime ;	///开牌时间		
	int			iFreeTime ;		///空闲时间	
	int			iRemaindTime;	///剩余时间

	int			iNowNtStation;	///当前庄家的位置
	int			iGameBeen;		///当前局数		
	int			iZhuangBaShu;	///庄家进行了几把	
	int			iGameCount;		///当前已经进行了几把

	int			iComputePoint[MAX_NOTE_COUNT];	
	
	short int	idzhuangstation[PLAY_COUNT+1][2];			//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	char		iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	///最近X局的信息	
	
	__int64		i64NtWin;		///庄家输赢成绩
	__int64		i64ShangZhuangLimit;			///上庄需要的最少金币
	__int64		i64MaxNote;						///个人游戏最大下注数
	__int64		i64RectNoteMax[MAX_NOTE_COUNT]; ///区域下注限制
	__int64		i64ChoumaValue[6];				// 六个筹码的值
	
	__int64  iZhongZhu;						///本把当前总注额		
	__int64  iQuYuZhu[MAX_NOTE_COUNT];		///本把每个区域下的注额
	__int64  iQuYuZhuTrue[MAX_NOTE_COUNT];	///本把真实每个区域下的注额
	__int64  iMyZhu[MAX_NOTE_COUNT];		///本把自已下注
	TagGameStation_OpenCard()
	{
		memset(this,0,sizeof(TagGameStation_OpenCard));
	}
};
/*------------------------------------------------------------------------*/
//断线重连结算阶段数据包
struct	TagGameStation_ShowResult
{
	BYTE		byGameStaion;	//游戏状态
	BYTE		bMaxBase;		/// 最大倍率即牛牛的倍率

	BYTE		byCard[5][5];	
	BYTE		byCardCount[5];

	int			iXiaZhuTime ;	///下注时间
	int			iKaiPaiTime ;	///开牌时间		
	int			iFreeTime ;		///空闲时间	
	int			iRemaindTime;	///剩余时间

	int			iNowNtStation;	///当前庄家的位置
	int			iGameBeen;		///当前局数		
	int			iZhuangBaShu;	///庄家进行了几把	
	int			iGameCount;		///当前已经进行了几把

	
	
	short int	idzhuangstation[PLAY_COUNT+1][2];			//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	char		iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	///最近X局的信息	
	
	__int64		i64NtWin;		///庄家输赢成绩
	__int64		i64ShangZhuangLimit;			///上庄需要的最少金币
	__int64		i64MaxNote;						///个人游戏最大下注数
	__int64		i64RectNoteMax[MAX_NOTE_COUNT]; ///区域下注限制
	__int64		i64ChoumaValue[6];				// 六个筹码的值
	
	__int64  iZhongZhu;						///本把当前总注额		
	__int64  iQuYuZhu[MAX_NOTE_COUNT];		///本把每个区域下的注额
	__int64  iQuYuZhuTrue[MAX_NOTE_COUNT];	///本把真实每个区域下的注额
	__int64  iMyZhu[MAX_NOTE_COUNT];		///本把自已下注
	
	__int64  iZhuangFen;		///庄家的得分		
	__int64  iXianFen;			///闲家的得分	
	__int64  iUserFen;			///当前玩家的得分
	TagGameStation_ShowResult()
	{
		memset(this,0,sizeof(TagGameStation_ShowResult));
	}
};
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------*/
///游戏开始信息
typedef		struct tagBeginData
{	
	__int64  iNTdata[4];		//庄家的位置,总分（当前金币数），成绩(赢的总金币)，局数（坐庄的局数）
	__int64  iShangZhuangLimit;		///上庄需要的最少金币	
	__int64  iQiangZhuangMoney;	//20121116dwj 抢庄花费的金币
	__int64	 iMaxZhu;     ///区域能下的最大注
	__int64	 i64ChoumaValue[6];	// 六个筹码的值


	short int iNTlist[PLAY_COUNT+1][2];		//庄家列表位置 第一列表示庄家列表，第二列表示是否抢庄了

	int  iGameBeen;       ///当前局数     	 
	int  iZhuangBaShu;     ///庄家进行了几把	
	int  iNowNtStation;    ///当前庄家的位置 	
	int  iGameCount;      ///游戏次数  		      	
	int  iXiaZhuTime ;	///下注时间		
	int  iKaiPaiTime ;	///开牌时间		
	int  iFreeTime ;	///空闲时间

	BYTE iMaxBase;   //游戏最大倍率
	


	tagBeginData()
	{
		memset(this, 0, sizeof(tagBeginData));
		iNowNtStation=-1;
		memset(i64ChoumaValue, 0, sizeof(i64ChoumaValue));
	}
}BEGIN_DATA;
/*----------------------------------------------------------------------------------*/
///下注数据包
struct	TagXiaZhu
{	
	bool		bIsAi;		/// 是否机器人下注
	BYTE		byStation;	///位置	
	int			iBetArea;	///下注区域1-4
	int			iChouMaType;//筹码类型

	int			iMoney;		//下注值
	__int64  iMaxZhu;		///还可下最大注	
	__int64  iZhongZhu;    ///本把当前总注额  
	__int64		iUserXiaZhu[MAX_NOTE_COUNT+1];	//玩家区域下注
	__int64		iQuYuZhu[MAX_NOTE_COUNT];	///本把每个区域下的注额	
	__int64		iQuYuZhuTrue[MAX_NOTE_COUNT];		/// 真实区域注	
   

	TagXiaZhu()
	{
		memset(this, 0, sizeof(TagXiaZhu));
	}
};

/*---------------------------------------------------------------------*/
///开牌数据包
typedef		struct tagResult
{
	__int64		iZhuangFen;    ///庄家的得分 	
	__int64		iXianFen;		///闲家的得分	
	__int64		iUserFen;		///当前玩家的得分	
	__int64		iNtWin;			///当前庄家赢的金币
	int			iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	//路子信息
	tagResult()
	{
		memset(this,0,sizeof(tagResult));
	}
};

/*---------------------------------------------------------------------*/
///开牌数据包
typedef		struct tagKaiPai
{
	BYTE	pai[5][5];			//牌数据     
	int		iCardCount[5];		//牌张数
	int		iComputePoint[MAX_NOTE_COUNT];		///游戏的赢钱区域 		

	tagKaiPai()
	{
		memset(this, 0, sizeof(tagKaiPai));
		memset(pai, 255, sizeof(pai));
	}
}KAI_PAI;

/*-------------------------------------------------------------------------*/
///上庄数据包
typedef		struct tagShangZhuang
{
	///申请的位置
	int station;   
	bool shang;		
	bool qiang;
	__int64 iQiangZhuangMoney;
	//20121119dwj 设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	short int  dzhuangstation[PLAY_COUNT+1][2];
	///是否失败
	bool success;  
	/// 是否机器人
	BOOL bIsAi;      
	tagShangZhuang()
	{
		memset(this, 0, sizeof(tagShangZhuang));
		memset(dzhuangstation, -1, sizeof(dzhuangstation));
	}
}SHANG_ZHUANG;
/*-------------------------------------------------------------------------*/
///换庄
typedef struct tagChangeZhuang
{    ///上一个庄家
	int bLastStation;
	/// 当前庄家
	int bNowStation;  
	// 设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	short int  dzhuangstation[PLAY_COUNT+1][2];
	tagChangeZhuang()
	{
		memset(this, 0, sizeof(tagChangeZhuang));
		memset(dzhuangstation,-1,sizeof(dzhuangstation));
	}

}CHANGE_ZHUANG;


///等待消息数据包
typedef struct tagDengDai
{
	//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	short int  dzhuangstation[PLAY_COUNT+1][2];
	int  iZhuangBaShu;//庄家进行了几把      
	int  iZhuang;//庄家位置
	__int64  iNtWin; //当前庄家赢的金币    
	tagDengDai()
	{
		memset(dzhuangstation, -1, sizeof(dzhuangstation));
		iNtWin = 0;              //当前庄家赢的金币
		iZhuangBaShu = 0;        //庄家进行了几把
		iZhuang=-1;
	}
}DENG_DAI;

///测试数据包
typedef struct tagTest
{ 
	///测试类型
	int type;  
	tagTest()
	{
		memset(this, 0, sizeof(tagTest));
	}

}BJL_TEST;

//控制输赢数据包
struct CONTROL_WIN
{	
	BYTE bGameWinStation;   ///赢的位置，0-4 代表，庄位，天，地，玄，黄	
	BYTE bGameLoseStation;  ///输的位置，0-4 代表，庄位，天，地，玄，黄	
	BYTE bGameWinQueue[MAX_NOTE_COUNT];///天地玄黄名家控制的输赢0-3代表天，地，玄，黄，0代表输。1代表赢 255表示不控制
	CONTROL_WIN()
	{
		memset((void *)this,0,sizeof(CONTROL_WIN));
		memset(bGameWinQueue,255,sizeof(bGameWinQueue));
	}
};

/*--------------------------------------------------------------*/
///超端数据包
struct	TagSuperUser
{	
	BYTE		byStation;			///位置
	bool        bIsSuper;           /**< 是否开通 */

	TagSuperUser()
	{
		byStation = -1;
		bIsSuper = false;
	}
};
/*--------------------------------------------------------------*/
//超端设置数据包
struct	TagSuperSet
{
	BYTE		byStation;			///位置
	bool		bSuccesse;			//设置是否成功
	int			iAreaContrl[4];		///区域控制天地玄黄  -1表示输 0-表示不控制 1表示赢

	TagSuperSet()
	{
		memset(this,0,sizeof(TagSuperSet));
		byStation = 255;
		bSuccesse = false;
	}
	void	init()
	{
		memset(this,0,sizeof(TagSuperSet));
		byStation = 255;
		bSuccesse = false;
	}
};
/*--------------------------------------------------------------*/

#define ASS_SUPER_SET_CARD         79        /// 提牌器消息id，平台决定的游戏不能修改

// 消息ID
#define ASS_GAME_BEGIN				111			/// 游戏开始	DWJ
#define ASS_KAI_PAI					112			// 开牌消息	DWJ
#define ASS_RESULT					113			///结算		DWJ
#define ASS_FREE_TIME				114			/// 空闲消息		DWJ
#define ASS_WU_ZHUANG				115			/// 游戏无庄闲置消息消息	DWJ

#define ASS_IS_SUPER_USER			120			//通知玩家是超端	DWJ
#define ASS_SUPER_SET				121			//超端设置		

#define ASS_XIA_ZHU					130        /// 下注消息
#define ASS_SHANG_ZHUANG			131			/// 上庄消息		DWJ


#pragma pack(pop)
#endif // !defined(_POKERMESSAGE_H__INCLUDED_)
