#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#define GAMENAME						TEXT("金蝉捕鱼")


#define NAME_ID							30000600								//名字 ID


//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//游戏信息

#define PLAY_COUNT						6								//游戏人数


const	int CLIENT_WIDTH		=	1280;	//屏幕宽
const	int CLIENT_HEIGHT		=	740;	//屏幕高


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
#define GET_KEY_NAME(A, B, C, D)		(sprintf(A,"%s%d%s", B, C, D),strcat(A,""))
#define GET_CLIENT_BCF_NAME(A)          (sprintf(A,"%d",NAME_ID),strcat(A,".bcf"))    

#define SUPER_PLAYER                    10                         //超级玩家
#define MAX_NAME_INFO                   256

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置
#define GS_PLAY_GAME					20				//发牌状态
#define GS_WAIT_BACK					21				//等待扣压底牌
#define GS_WAIT_NEXT					23				//等待下一盘开始 

//按注类型


/********************************************************************************/

//游戏数据包

/********************************************************************************/
/*-------------------------------------------------------------------------------*/
//游戏状态基础数据包
struct	GameStation_Base
{
	BYTE				iVersion;						//游戏版本号
	BYTE				iVersion2;						//游戏版本号

	bool		bHitFlag;		//是否作为机器人击中鱼的逻辑判断
	BYTE		bHitDesk;
	BYTE		byGameStation;					//游戏状态
	int			iBgindex;		//背景索引


	int			iFireBase;								//炮火初始倍数
	int			iUserFireBase[PLAY_COUNT];				//各玩家的炮火倍数
	int			iUserScore[PLAY_COUNT];					//玩家上分数

	GameStation_Base()
	{
		memset(this, 0 , sizeof(GameStation_Base));
	}
};
/*-------------------------------------------------------------------------------*/
//玩家进入游戏数据包
struct	GameUserComeData
{
	BYTE		byDeskStation;	//玩家座位号
	bool		bComeOrLeave;	//进入/离开
	bool		bIsRobot;		//是否机器人
	int			iFireBase;		//炮火倍数
	int			iScore;			//上分数
	GameUserComeData()
	{
		memset(this, 255, sizeof(GameUserComeData));
		iFireBase	= 0;
		iScore		= 0;
		bComeOrLeave = true;
	}
};
/*-------------------------------------------------------------------------------------*/
struct BulletInfo
{
	int			iBeilv;			//子弹倍率
	bool 		bHave;			//子弹存在
	bool		bIsSuper;		//是否超级炮弹

	BulletInfo()
	{
		memset(this , 0, sizeof(BulletInfo));
	}
};
/*-------------------------------------------------------------------------------------*/
//初始轨迹点和角度
struct	FishTrace
{
	float x;    //鱼儿X坐标
	float y;    //Y坐标
	float rotation;//当前角度
	float movetime;//移动时间保持这个轨迹运动的时间
	float changetime;//改变时间，从之前的角度改变到现在角度规定在这个时间内完成
	bool  m_isHave ;//坐标是否过期，默认为过期就是这个坐标默认不存在
	int   m_fishid;//这个坐标对应的FISH的ID号
	int   m_fishtype;//这个坐标对应的FISH的ID号
	int   m_ptindex;
	int   m_BuildTime;
	int   m_shootCount;
	int   m_mustShoot;
	int   m_fudaifishtype;//类型为21和22的鱼可以附带小鱼
	int   m_speed;

	FishTrace()
	{
		memset(this,0,sizeof(FishTrace));
	}
};
/*-------------------------------------------------------------------------------------*/
//游戏状态
struct	CMD_S_FishTrace
{
	bool							bRegular;								//规则标识
	FishTrace                       m_fishtrace[5];

	CMD_S_FishTrace()
	{
		memset(this, 0, sizeof(CMD_S_FishTrace));
	}
};
/*-------------------------------------------------------------------------------------*/
//切换场景
struct	CMD_S_ChangeScene
{
	WORD     					    SceneIndex;						        //当前场景
	WORD                            RmoveID;                                //预留这里

	CMD_S_ChangeScene()
	{
		memset(this, 0, sizeof(CMD_S_ChangeScene));
	}
};










/*-------------------------------------------------------------------------------------*/
//购买子弹
struct	CMD_C_BuyBullet
{
	bool			bAddOrMove;		//上或者是下
	int				iScore;			//上下分数		

	CMD_C_BuyBullet()
	{
		memset(this, 0, sizeof(CMD_C_BuyBullet));
	}
};
/*-------------------------------------------------------------------------------------*/
//购买子弹数目
struct	CMD_S_BulletCount
{
	bool			bAddOrMove;			//上或者是下
	BYTE			byDeskStation;		//上下分座位号
	__int64			i64UserScore;		//玩家上下总分数
	__int64			i64UserMoney;		//玩家身上的金币
	CMD_S_BulletCount()
	{
		memset(this, 0, sizeof(CMD_S_BulletCount));
	}
};
/*-------------------------------------------------------------------------------------*/
//玩家发射子弹
struct	CMD_C_UserShoot
{
	int				iBulletCountKind;		//子弹类型
	BYTE			byShootCount;			//射击数目
	float			fAngle;					//发射角度
	DWORD			dwBulletID;				//子弹标识

	CMD_C_UserShoot()
	{
		memset(this, 0, sizeof(CMD_C_UserShoot));
	}

};
/*-------------------------------------------------------------------------------------*/
//发射炮弹
struct	CMD_S_UserShoot
{
	BYTE			byStation;				//玩家椅子
	bool			bIsRobot;				//判断是否是机器人
	BYTE			byShootCount;			//子弹数目	
	float			fAngle;					//发射角度
	int 			iBulletCountKind;		//子弹类型
	__int64         i64UserScore;			//玩家剩余子弹数
	
	CMD_S_UserShoot()
	{
		memset(this, 0, sizeof(CMD_S_UserShoot));
	}
};
/*-------------------------------------------------------------------------------------*/
//发射炮弹错误
struct	CMD_S_UserShootError
{
	BYTE			byStation;				//玩家椅子
	bool			bIsRobot;				//判断是否是机器人
	int				iFireBase;				//炮火倍率
	__int64         i64UserScore;			//玩家剩余子弹数

	CMD_S_UserShootError()
	{
		memset(this, 0, sizeof(CMD_S_UserShootError));
	}
};
/*-------------------------------------------------------------------------------------*/
struct	CMD_C_AddFireBase
{
	bool		bAdd;			//加/减
	BYTE		byDeskStation;	//座位号
	CMD_C_AddFireBase()
	{
		memset(this, 0, sizeof(CMD_C_AddFireBase));
	}
};
/*-------------------------------------------------------------------------------------*/
struct	CMD_S_AddFireBase
{
	BYTE		byDeskStation;	//座位号
	int			iUserFireBase;	//玩家炮火倍数
	CMD_S_AddFireBase()
	{
		memset(this, 0, sizeof(CMD_S_AddFireBase));
	}
};
/*-------------------------------------------------------------------------------------*/
//击中鱼群
struct CMD_C_HitFish
{
	int				iFishID;			//鱼群标识
	int				iBulletID;			//子弹标识
	int             iBulletUserId;		//发射子弹玩家的ID
	bool            bIsAndroid;			//是否机器人

	CMD_C_HitFish()
	{
		memset(this, 0, sizeof(CMD_C_HitFish));
	}
};
/*-------------------------------------------------------------------------------------*/
//捕获鱼群
struct CMD_S_CaptureFish
{
	bool                bCanSuperPao;               //超级炮
	BYTE				byStation;					//玩家座位
	int					iFishID;					//鱼群标识
	int					iCatchCount;				//抓住鱼的数量
	int					iCatchFishID[150];			//抓住鱼的ID
	int    				iFishKindScore;				//鱼群种类
	LONG				lFishScore;					//鱼群得分
	int					iCatchScore[150];			//捕获对应鱼的得分
	__int64             i64UserScore;			//玩家总分

	CMD_S_CaptureFish()
	{
		memset(this,0,sizeof(CMD_S_CaptureFish));
	}
};
/*-------------------------------------------------------------------------------------*/
//捕获鱼群广播
struct CMD_S_CaptureFishBroadcast
{
	char				szNickName[61];				///<昵称  
	char				szRoomName[61];				///<房间名字  
	int					iDeskNo;					///<桌子ID号
	BYTE				byStation;					//玩家座位
	int					iFishType;					//鱼类标识
	__int64				i64FishScore;				//鱼群得分


	CMD_S_CaptureFishBroadcast()
	{
		memset(this,0,sizeof(CMD_S_CaptureFishBroadcast));
	}
};
/*-------------------------------------------------------------------------------------*/
//取消超级炮弹
struct CMD_S_CanCelSuperPao
{
	bool                bCanSuperPao;               //超级炮
	BYTE				byStation;					//玩家座位

	CMD_S_CanCelSuperPao()
	{
		memset(this,0,sizeof(CMD_S_CanCelSuperPao));
	}
};
/*-------------------------------------------------------------------------------------*/


















/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/
#define ASS_VREB_CHECK					179				//用户处理
/**************************服务端——>客户端消息**************************/
#define ASS_S_USER_COME					51				//玩家进入消息
#define ASS_BUILD_TRACE					52				//生成鱼消息
#define	ASS_CHANGE_SCENCE				53				//切换场景
#define	ASS_FISH_REG					54				//鱼阵

#define	ASS_S_BUY_BULLET				60				//上分下分结果消息
#define	ASS_S_USER_SHOT					61				//玩家发射子弹结果
#define	ASS_S_USER_SHOT_ERROR			62				//玩家发射子弹错误
#define	ASS_S_ADD_FIRE					63				//玩家加炮
#define	ASS_S_HIT_FISH					64				//击中鱼儿
#define	ASS_S_CANCEL_SUPER				65				//取消超级炮弹
#define ASS_S_CATCH_FISH_BROAD			66				//捕获鱼的广播


/**************************客户端——>服务端消息**************************/
#define	ASS_C_BUY_BULLET				80				//上分下分消息	
#define	ASS_C_USER_SHOT					81				//玩家发射
#define	ASS_C_ADD_FIRE					82				//玩家加炮
#define	ASS_C_HIT_FISH					83				//击中鱼儿
/********************************************************************************/
#endif
