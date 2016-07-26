#pragma once
#include<vector>
#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define GAMENAME						TEXT("马股")
#define NAME_ID							10100204						//名字 ID
#define PLAY_COUNT						4								//游戏人数
//版本定义
#define GAME_MAX_VER					1								//现在最高版本
#define GAME_LESS_VER					1								//现在最低版本
#define GAME_CHANGE_VER					0								//修改版本
//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
//游戏信息
#define GET_STRING(NUM)                 #NUM
#define INT_TO_STR(NUM)                 GET_STRING(NUM)
#define CLIENT_DLL_NAME                 TEXT(INT_TO_STR(NAME_ID)##".ico")        // 客户端ico名字
#define SKIN_FOLDER                     TEXT(INT_TO_STR(NAME_ID))                // 客户端目录
#define SKIN_CARD						TEXT("CardSkin")		                 // 共用图片文件夹


//游戏消息
#define GM_GAMESTATIONCHANGED		50		//游戏状态消息
#define GM_GAMEMODECHANGED			51		//游戏模式
#define GM_GAMEMODESELECTED			52		//游戏模式选择成功
#define GM_GETREADY					53		//准备消息
#define GM_SENDCARD					54		//发牌消息
#define GM_PK						55		//比牌消息
#define GM_GETREADY					65		//准备
#define GM_WASHCARD					66		//洗牌
#define GM_RESULT					67		//出牌结果
#define GM_BET						68		//下注
#define GM_SHOWTOALL				69		//亮牌
#define GM_SETTLE					70		//结算
#define GM_GAMEOVER					71		//游戏结束
#define GM_BETBANKER			72			//选择庄家
#define ASS_HANDSUP					73		//通知玩家举手
#define ASS_SUPER					74		//超端
#define ASS_WINORLOSE				75		//输赢



//游戏模式
enum ENUM_GameMode		
{ 
  GAMEMODE_BANKER_INTURN,	//轮庄
  GAMEMODE_BANKER_CONST,	//定庄
  GAMEMODE_BANKER_BET,		//抢庄
  GAMEMODE_INVALIDE,		//无效
};


//游戏状态
enum ENUM_GameStation
{
	GS_NOMODE = 80,		//模式未选择
	GS_MODESETTING,		//选择模式中
	GS_WITHMODE,		//模式已选择
	GS_READY,			//准备
	GS_GAMEBEGIN,		//游戏开始
	GS_FINDBANKER,		//确定庄家
	GS_BETTING,			//下注
	GS_WASHCARDS,		//洗牌
	GS_SENDCARDS,		//发牌
	GS_PK,				//比牌
	GS_SETTLE,			//结算
	GS_GAMEOVER		//游戏结束
};

//游戏座位
enum ENUM_TablePosition
{
	TP_BANKER,		//庄家
	TP_SECOND,		//顺们
	TP_THIRD,		//天门
	TP_LAST,		//尾门
	TP_NOPOSITION	//未坐下
};

//游戏状态定义
#define GS_WAIT_SETGAME					0				//等待东家设置状态
#define GS_WAIT_AGREE					1				//等待同意设置
#define GS_PLAY_GAME					21				//游戏中
#define GS_WAIT_NEXT					23				//等待下一盘开始 
#define EveryCount					    13				//每个人所发扑克张数


#include "UpGradeLogic.h"

//客户端发送给服务器的数据包



//超端数据包
struct TSuper
{
	TSuper()
	{
		IsSetSuper = false;
		isWinOrLose = FALSE;
	}
	bool IsSetSuper;
	BOOL isWinOrLose;
};

// 1. 基础数据包
struct T_CS_Base 
{ 
	T_CS_Base()
	{
		m_PositionId = -1;
		m_GameMode = GAMEMODE_INVALIDE;
	}
	int m_PositionId;					//游戏座位号
	ENUM_GameMode	   m_GameMode;		//游戏模式
};

// 2.修改游戏状态数据包
struct T_CS_ModifyGameStation : public T_CS_Base	
{
	T_CS_ModifyGameStation()
	{
		m_GameStation = GS_NOMODE;
	}
	ENUM_GameStation m_GameStation;
};

//游戏状态包
struct TGameStation
{
	TGameStation()
	{
		byGameDeskNO = 0;
		enGameStation = GS_NOMODE;
		enGameMode = GAMEMODE_INVALIDE;
	}
	BYTE byGameDeskNO;					//座位号
	ENUM_GameStation enGameStation;		//游戏状态
	ENUM_GameMode enGameMode;			//游戏模式
};


// 3.准备状态数据包
struct T_CS_GetReady : public T_CS_Base
{
	T_CS_GetReady()
	{
		m_GetReady = false;
	}
	bool	m_GetReady;
};


// 结算数据包
struct T_SC_Settle
{
	T_SC_Settle()
	{
		memset(this,0,sizeof(T_SC_Settle));
	}
	char cbNickName[4][61];		//昵称
	bool bIsBanker[4];			//是否庄家
	EnCardShape enCardShape[4];	//牌型
	__int64  iScore[4];			//得分
};

struct T_SC_WhoIsBanker
{
	T_SC_WhoIsBanker()
	{
		byDeskStation = 255;
	}
	BYTE byDeskStation;
};

// 玩家分牌结果数据包
struct T_CS_OutCard
{
	T_CS_OutCard()
	{
		memset(firstHeap,0,sizeof(firstHeap));
		memset(SecondHeap,0,sizeof(SecondHeap));
		memset(allCards,0,sizeof(allCards));
		byDeskStation = 99;
		cardCount = 0;
	}
	BYTE firstHeap[3];	//第一堆
	BYTE SecondHeap[2];	//第二堆	
	BYTE allCards[5];	//所有牌数据，特殊牌型时才用到，如十马，四条
	BYTE byDeskStation;	//座位号
	BYTE cardCount;		//牌的张数
};

//服务器广播分派数据包
struct T_SC_OutCard
{
	T_SC_OutCard()
	{
		memset(this,0,sizeof(T_SC_OutCard));
	}
	BYTE firstHeap[3];
	BYTE secondHeap[2];
	BYTE byDeskStation;
	EnCardShape enCardShape;
};

// 出牌结果数据包
struct T_SC_CardResult
{
	T_SC_CardResult()
	{
		enCardShape = CS_INVALID;
		byDeskStation = 99;
	}
	EnCardShape enCardShape;
	BYTE byDeskStation;
};

// 投注数据包
struct T_CS_Bet
{
	T_CS_Bet()
	{
		iBetMony = 0;
		byDeskStation = 0;
	}
	__int64 iBetMony;
	BYTE byDeskStation;
};

struct TBetBanker
{
	TBetBanker()
	{
		byDeskStation = 99;
		isBet = false;
	}
	BYTE byDeskStation;
	bool isBet;
};

struct T_SC_Cards
{
	T_SC_Cards()
	{
		memset(m_cards,0,sizeof(m_cards));
	}
	BYTE m_cards[5];
};



//玩家游戏中的状态
struct TPlayerInfo
{
	TPlayerInfo()
	{
		byDeskNumber = 0;
		bIsBanker = false;
	}

	BYTE byDeskNumber;	//桌位号
	bool bIsBanker;		//是否为庄家
};

#define ASS_GAMEMODE					70					//请求游戏模式
#define ASS_GAMEMODE_RESULT					71				//请求游戏模式结果

//游戏消息
 #define ASS_RESET_STATION					        50					//出牌
// #define ASS_WASHCARD					        51					//洗牌
// #define ASS_SEND						        52					//发牌
// #define ASS_PREPARE								53					//准备完成
// #define ASS_NORMALFINISH				        54					//游戏正常结束消息
// #define ASS_OUT_CARD_RESULT			            55					//出牌结果
// #define ASS_SUPER					            56					//超端验证
// #define ASS_COMPARE								57					//比牌
// #define ASS_FIRE						        58					//打枪
// #define ASS_AUTO								59					//托管
// #define ASS_AUTO_RESULT							60					//托管结果
// #define ASS_SUPER_CONTROL					    61					//超端控制
// #define ASS_SUPER_CONTROLRES					62					//超端控制结果

#endif
