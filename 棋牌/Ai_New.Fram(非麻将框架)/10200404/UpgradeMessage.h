#pragma once

//同楼 普通版/视频版 编译  release/视频版  ，提牌器编译提牌器版

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#define GAMENAME						TEXT("升级")

#ifdef VIDEO
#define NAME_ID							10200404								//名字 ID
#else
#define NAME_ID							10200404								//名字 ID
#endif


//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME


//游戏内部开发版本号
#define DEV_HEIGHT_VERSION				2				//内部开发高版本号(每修正一次LOW_VERSIONG+1)
#define DEV_LOW_VERSION					0				//内部开发低版本号



//游戏信息
#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define PLAY_COUNT						4								//游戏人数
#define SKIN_FOLDER						TEXT(INT_TO_STR(NAME_ID))		// 客户端目录
#define SKIN_CARD						TEXT(".\\image\\cardskin")		//共用图片文件夹
static TCHAR szTempStr[MAX_PATH] = {0};//暂存字符串，用来获取 dll_name 等的时候使用
#define GET_CLIENT_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))
#define GET_SERVER_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))

#define GET_CLIENT_BCF_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".bcf"))



//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_ARGEE					1				//等待同意设置

#define GS_WASHING_CARD					19				//洗牌
#define GS_SEND_CARD					20				//发牌状态

#define GS_PLAY_GAME					22				//游戏中状态
#define GS_WAIT_NEXT					23				//等待下一盘开始 

//-------------------------------------------------------------------------------
/********************************************************************************/

// 数据包处理辅助标识
//warning:79号ID预留给超级客户端,永不使用
/********************************************************************************/
#define ASS_UG_USER_SET					50				//设置游戏
#define ASS_GAME_BEGIN					51				//游戏开始
#define ASS_GAME_PREPARE				52				//准备工作
#define ASS_SEND_CARD					53				//发牌信息
#define ASS_SEND_CARD_MSG				54				//发牌过程中处理消息
#define ASS_SEND_FINISH					56				//发牌完成
#define ASS_GAME_PLAY					76				//开始游戏
#define ASS_OUT_CARD					77				//用户出牌
#define ASS_OUT_CARD_RESULT				78				//出牌結果
#define	ASS_SUPER_USER					79				//超端消息
#define ASS_REPLACE_OUT_CARD			80				//代替出牌(79留给超级客户端发牌器)
#define ASS_CONTINUE_END				84				//游戏结束
#define ASS_NO_CONTINUE_END				85				//游戏结束

#define ASS_GETREADY					86				//准备OK
#define ASS_QUIT						87				//退出游戏

#define ASS_WASHINGCARD					88				//洗牌
#define ASS_GAMELEVEL					89				//游戏级别
#define	ASS_CALL						90				//叫主
#define ASS_SHOWCALLLEVELCARD			91				//显示叫主的牌
#define ASS_CALL_COMPLETED				92				//叫主完毕
#define	ASS_WHOISBANKER					93				//庄家
#define ASS_SENDBACKCARD				94				//发底牌
#define ASS_BURYCARD					95				//埋牌
#define ASS_BACKCARDBURIED				96				//埋牌完成
#define ASS_OUTCARD_FAILED				97				//出牌失败
#define ASS_OUTCARD_SUCCESS				98				//出牌成功
#define ASS_OUTMIXEDCARD_FAILED			99				//甩牌失败
#define ASS_SCORE						100				//得分
#define ASS_GAMEOVER					101				//游戏结束
#define ASS_CONFIGINFO					102				//游戏配置信息
#define ASS_SENDOTHERCARD				103				//发送其他玩家的牌


//////////////////////////////////////////////////////////////////////////
//时钟滴答
#define COLOCKCICK					15			//15S


enum eCardShape
{
	E_CS_INVALIED,E_CS_SINGLE, E_CS_DOUBLE,E_CS_SERIALDOUBLE,E_CS_MIXED
};

/********************************************************************************/
//数据包
struct TGameStation
{
	TGameStation()
	{
		byGameStation = 255;
	}
	BYTE byGameStation;
};

//准备包
struct TGetReady
{
	TGetReady()
	{
		byDeskStation = 255;
		memset(bUserGetReady,0,sizeof(bUserGetReady));
	}
	BYTE byDeskStation;
	bool bUserGetReady[PLAY_COUNT];
};

//牌数据包， 不含底牌
struct TSendCards
{
	TSendCards()
	{
		memset(this,0,sizeof(TSendCards));
	}
	BYTE byDeskStation;	//座位号
	BYTE byCards[25];	//牌数据
	BYTE byCardCount;	//牌数量
};
//其他玩家拍数据
struct TOtherCards
{
	TOtherCards()
	{
		memset(this,0,sizeof(TOtherCards));
	}
	TSendCards tsendCards[3];
};

//游戏级别包
struct TGameLevel
{
	TGameLevel()
	{
		byDeskStation = 255;
		byGoup1Level = 255;
		byGoup2Level = 255;	
	}
	BYTE byDeskStation;	//座位号
	BYTE byGoup1Level;	//第一组级别
	BYTE byGoup2Level;	//第二组级别
};

//亮主的牌
struct TCallLevelCard
{
	TCallLevelCard()
	{
		bySize = 0;
		byDeskStation = 255;
		byLevelColor = 255;
		memset(byCallCard,0,sizeof(byCallCard));
	}
	BYTE byDeskStation;					//座位号
	BYTE byCallCard[2];					//亮主的牌
	BYTE bySize;						//亮牌张数
	BYTE byLevelColor;					//级牌花色
};

//客户端发送给服务器，出牌的数据
struct T_CS_OutCard
{
	T_CS_OutCard()
	{
		byCardNums = 0;
		memset(byCards,0,sizeof(BYTE)*25);
		byDeskStation = 255;
		isFirst = false;
		cardShape = E_CS_INVALIED;
	}
	BYTE byCardNums;		//牌张数
	BYTE byCards[25];		//牌数据
	BYTE byDeskStation;		//座位号
	bool isFirst;			//是否第一个出牌
	eCardShape	cardShape;	//牌型
	
};

//服务器发给客户端通知出牌数据包, 包含上一家出的牌，
struct T_SC_OutCardPackage
{
	T_SC_OutCardPackage()
	{
		memset(&tPreoutCards,0,sizeof(T_CS_OutCard));
		IsFirst = false;
		byOutCardPlayerDeskStation = 255;
	}
	T_CS_OutCard tPreoutCards;
	bool IsFirst;
	BYTE byOutCardPlayerDeskStation;
};

//出牌失败数据包
struct T_SC_OutCardFailed
{
	T_SC_OutCardFailed()
	{
		memset(this,0,sizeof(T_SC_OutCardFailed));
	}
	WCHAR MsgFailReason[100];
	BYTE byDeskStation;
};

//甩牌失败数据包
struct T_SC_MixedCardFailed
{
	T_SC_MixedCardFailed()
	{
		memset(this,0,sizeof(T_SC_MixedCardFailed));
	}
	BYTE byMinCard[25];
	BYTE byCardCount;
	BYTE byDeskStation;
};

//得分
struct T_SC_Score
{
	T_SC_Score()
	{
		memset(this,0,sizeof(T_SC_Score));
	}
	int iScore;
};

//游戏配置信息
struct TGameConfigs
{
	TGameConfigs()
	{
		memset(this,0,sizeof(TGameConfigs));
	}
	int iTimeToGetReady;	//准备时间
	int iTimeToCallNT;		//叫主时间
	int iTimeToThink;		//思考时间
	int iSendCardSpeed;		//发牌速度，毫秒
};

#endif
