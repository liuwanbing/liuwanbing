#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

//文件名字定义
#define GAMENAME						TEXT("李逵劈鱼")


//版本定义
#define GAME_MAX_VER					1							//现在最高版本
#define GAME_LESS_VER					1							//现在最低版本
#define GAME_CHANGE_VER					0							//修改版本

//支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					"MatchTable_gdw"

//游戏信息
#define NAME_ID							33000308							//名字 ID
#define CLIENT_DLL_NAME					TEXT("33000308.ico")
#define SERVER_DLL_NAME					TEXT("33000308.dll")

#define PLAY_COUNT						8							//游戏人数

//游戏内部开始版本核对
#define DEV_HEIGHT_VERSION				1							//内部开发高版本号
#define DEV_LOW_VERSION					1							//内部开发低版本号

#define SKIN_FOLDER						TEXT("33000308")			//图片文件夹
#define SKIN_CARD						TEXT("CardSkin")            //牌图片文件夹

//游戏状态定义
#define GS_WAIT_SETGAME					0							//等待东家设置状态
#define GS_WAIT_AGREE					1							//等待同意设置
#define GS_WAIT_NEXT					100							//等待下一盘开始 

/*------------------------------------------------------------------------------*/
#define DEF_SCREEN_WIDTH	  1280		// 默认屏宽
#define DEF_SCREEN_HEIGHT	  768			// 默认屏高


/*------------------------------------------------------------------------------*/

/********************************************************************************/

//游戏数据包
/********************************************************************************/


//用户同意游戏
struct UserArgeeGame
{
	BYTE				iPlayCount;						//游戏盘数
	BYTE				iCardCount;						//扑克数目
};
//////////////////////////////////////////////////////////////////////////////
//////////////////按照游戏消息处理过程来安排数据//////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//游戏开始
struct BeginUpgradeStruct
{
	int					iUpgradeStation;				//庄家位置
	BYTE				iStation[2];					//主牌的位置
	BYTE                iBeenPlayGame;                 
	            
};
//游戏准备数据包
struct ReadyStruct
{
	BYTE				bDeskStation;					//当前出牌者
};

//用户出牌数据包 （发向客户端）
struct OutCardMsg
{
	int					iNextDeskStation;				//下一出牌者
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iCardList[39];					//扑克信息
};
//出牌完数据包
struct OutCardFinishStruct
{
	BYTE bFriendDesk;									//其盟友位置
	BYTE bDeskStation;									//位置
	BYTE bSequence;										
};

//奖分
struct AwardPointStruct
{
	BYTE	iAwardPoint;
	BYTE	bDeskStation;
};


struct SendCardMsgStruct
{
	BYTE				bDeskStation;						//庄的位置
//	BYTE				bCardFriend[4];					//三张友牌
};

//发牌数据包
struct SendCardStruct
{
	int					bDesk;							//玩家
	BYTE				iCard;							//牌标号
};

//發送所有牌數據
struct SendAllStruct
{
	BYTE iUserCardCount[4];									//发牌数量
	BYTE iUserCardList[216];								//发牌队例
};

//叫分数据包
struct JiaofenStruct
{
	int			iStyle;									//叫牌类型(叫分分数)
	int         bDeskStation;							//当前叫分者
	BOOL		bFlag;									//叫分类型区分标记
};

//抗贡
struct GainstContributeStruct
{

};

//上贡
struct ContributeStruct
{
	BYTE bLoseDesk1;								//第一输家
	BYTE bLoseCard1;								//第一输家上贡牌
	BYTE bLoseDesk2;								//第二输家
	BYTE bLoseCard2;								//第二输家上贡的牌

	BYTE bWinerDesk1;								//第一赢家
	BYTE bWinerCard1;								//第一赢家拿到的牌
	BYTE bWinerDesk2;								//第二赢家
	BYTE bWinerCard2;								//第二赢家拿到的牌

	BYTE PlayCount;                                 //上贡的人数;
};
//选牌结果
struct ChoiceCardStruct 
{
	BYTE bDeskStation;								//选项牌者位置
	BYTE bCard;										//牌
	BYTE bDest;										//选牌人位置号
};

//退贡
struct MoveBackContributeStruct
{
	BYTE bDeskStation;								//退贡位置
	BYTE bCard;										//退贡牌
};

//退贡结果
struct ChangeCardStruct
{
	BYTE bDeskFrom;									//谁进贡
	BYTE bCard;										//进贡牌
	BYTE bDeskTo;									//进贡给谁
	BYTE bDest;                                      //选择第几个确定
};

//亮主牌数据包  （发向服务器）
struct ClientNtStruct
{
	BYTE				bDeskStation;					//反牌位置
	BYTE				iNtCard;						//反牌牌
	BYTE				iCardCount;						//反牌数目
};

//亮主牌数据包  （发向客户端）
struct ServerNtStruct
{
	BYTE				bDeskStation;					//反牌位置
	BYTE				iNtCard;						//所用反牌
	BYTE				iCardCount;						//反牌数量
};

//底牌数据包
struct BackCardStruct
{
	BYTE				iBackCardCount;					//扑克数目
	BYTE				iBackCard[8];					//底牌数据
};

//底牌数据包
struct BackCardExStruct
{
	BYTE				iNtHua;							//主牌花色
	BYTE				iGiveBackPeople;				//底牌玩家
	BYTE				iBackCardCount;					//扑克数目
	BYTE				iBackCard[8];					//底牌数据
};

//留底牌状态数据包
struct BackStationStruct
{
	BOOL				bOtherCanSee;					//其他玩家是否可见
//	BYTE				iNtPeople;						//反牌位置	
	BYTE				iGiveBackStation;				//留底位置
	BYTE				iBackCardCount;					//扑克数目
	BYTE				iBackCard[12];					//底牌数据
};

//游戏开始数据包
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;				//出牌的位置
	BYTE                iUserCard[PLAY_COUNT][54];      //玩家的牌
	BYTE                iUserCardCount[PLAY_COUNT];
	BeginPlayStruct()
	{
		memset(iUserCard,0,sizeof(iUserCard));
		memset(iUserCardCount,0,sizeof(iUserCardCount));
	}
};

//用户出牌数据包 （发向服务器）
struct OutCardStruct
{
	int					iCardCount;						//扑克数目
	BYTE				iCardList[39];					//扑克信息
	OutCardStruct()
	{
		memset(this, 0 ,sizeof(OutCardStruct));
	}
};


//用户甩牌失败数据包 （发向客户端）
struct OutShowCardStruct
{
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iResultCount;					//结果扑克数目
	BYTE				iCardList[78];					//扑克信息
};

//用户出牌数据包 （发向客户端）(最后一手牌）
struct BatchOutCardMsg
{
	BYTE				iCardCount;						//扑克数目
	BYTE				iCardList[156];					//扑克信息
};

//新一轮出牌
struct NewTurnStruct
{
	BYTE bDeskStation;									//出牌玩家
};


//玩家的分数据包
struct GamePointStruct
{
	bool				bAdd;							//未用(如果为true表示iDesk玩家抓得分,false表示成为桌面临时分)
	BYTE 				iDesk;							//哪个玩家得分
	int					iPoint;							//得分数目
	int					iTaxis;							//出牌先后
};

//游戏结束统计数据包
struct GameCutStruct
{
	int					iRoomBasePoint;						//倍数
	int					iDeskBasePoint;					//桌面倍数
	int					iHumanBasePoint;				//人头倍数

//	int					iBasePoint;						//游戲倍數
//	int					iDeskBasePoint;						//游戲倍數
	int					bDeskStation;					//退出位置
	__int64				iTurePoint[4];					//庄家得分
	__int64				iChangeMoney[8];				//玩家金币
};

//游戏结束统计数据包
struct GameEndStruct
{
	int					iRoomBasePoint;						//倍数
	int					iDeskBasePoint;					//桌面倍数
	int					iHumanBasePoint;				//人头倍数

	BYTE				iUpGradeStation;				//庄家位置
	int					iGrade2;						//庄家盟友
	BYTE				iAward[4];						//讨赏多少
	BYTE				iContinueWin[4];				//连胜
	BYTE				 iContinueLose[4];				//连败
	int					iAwardPoint[4];					//讨赏分牌型加分
	BYTE				iSequence[4];
	BYTE				itype;							//类型,普通;独报
	int					iTurePoint[4];					//玩家得分
	int					iSystemPoint[4];				//系统加分
	__int64				iGamePoint[4];					//游戲中輸贏情況
	__int64				iChangeMoney[8];				//玩家金币
	int                 iWinSequence[4][2];             ///头未游情况
};

//游戏结算
struct GameFinishNotify
{
	int					iRoomBasePoint;					//倍数
	int					iDeskBasePoint;					//桌面倍数
	int					iHumanBasePoint;				//人头倍数
	TCHAR				name[4][21];
	BYTE				iUpGradePeople;
	BYTE				iGrade2;
	BYTE				iStyle;							//游戏类型是否为强退,还是正常结束
	BYTE				iContinueWin[4];				//连胜
	BYTE				iContinueLose[4];				//连败
	BYTE				iAward[4];						//讨赏數量
	int					iAwardPoint[4];					//讨赏分
	__int64				iWardPoint[4];				//赢分数
	int					iGamePoint[4];					//游戲輸贏
	BYTE				iSequence[4];					//出完牌顺序

	__int64				iMoney[4];
	
};
/////////////////////////////////////////////////////////
//////////////辅助游戏结构///////////////////////////////
/////////////////////////////////////////////////////////

//托管
struct AutoCardStruct
{
	BYTE bDeskStation;
	bool bAutoCard;
};

//有事要走
struct HaveThingStruct
{
	BYTE pos;
	char szMessage[61];
};

//同意离开
struct LeaveResultStruct
{
	BYTE bDeskStation;
	int bArgeeLeave;
};

//文字消息
struct MessageStruct	//客户端到服务器
{
	char Message[200];
};

//提前结束
struct StopResultStruct
{
	bool bArgee;
};

//同意投降
struct LoseResultStruct
{
	BYTE bDeskStation;
	bool bArgee;
};

//超级
struct SuperClientStruct
{
	int iCardListNum;
	BYTE bCardList[100];
};

struct OutCardErrorStruct
{
	int iCardCount;
	BYTE bCard[45];
	OutCardErrorStruct()
	{
		iCardCount = 0;
		memset(bCard,0,sizeof(bCard));

	}
};


/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/
////游戏流程相关
//#define ASS_UG_USER_SET					50				//用户设置游戏
//#define ASS_BEGIN_UPGRADE				51				//升级游戏开始
//#define ASS_READY						52				//准备工作
//#define ASS_SEND_CARD					53				//发牌信息(一张一张发)
//#define ASS_SEND_ALL_CARD				54				//发送所有牌(一下发放全部)
//#define ASS_SEND_CARD_MSG				55				//发牌中出现的消息(抓到哪一张牌为庄家或者第一个出牌)
//#define ASS_SEND_FINISH					58				//发牌完成
//
////掼掼逻辑
//#define ASS_GAINST_CONTRIBUTE			62				//抗贡
//#define ASS_CONTRIBUTE					63				//玩家上贡
//#define ASS_CONTRIBUTE_RESLULT			64				//玩家退贡结果
//#define ASS_CONTRIBUTE_FINISH			65				//退贡完毕
//#define ASS_MOVE_BACK_CONTRIBUTE		66				//退贡
//#define ASS_MOVE_BACK_RESULT			67				//退贡结果
//#define ASS_MOVE_BACK_FINISH			68				//退贡结束
//#define ASS_CHANGE_CARD_RESULT			69				//换牌情况
//#define ASS_CHOICE_CARD					70				//选牌
//#define ASS_CHOICE_CARD_RESULT			71				//选牌结果
////游戏开始后消息传递
//#define ASS_GAME_PLAY					77				//开始游戏
//#define ASS_OUT_CARD_RESULT				78				//用户出牌结果
//#define ASS_OUT_CARD					80			//用户出牌
//#define ASS_SUPERSEDE_OUT_CARD			81				//代替出牌
//
//#define ASS_AWARD_POINT					83				//用户分数
//#define ASS_PUNISH_POINT				84				//罚分
//#define ASS_GAME_POINT					85				//是否有分
//
//#define ASS_LAST_CARD_REMIND			86				//最后一手牌提醒
//#define ASS_OUT_CARD_FINISH				87				//用户出完牌
//#define ASS_NT_TWO						88				//显示玩家盟友的信息
//#define ASS_NEW_TURN					89				//新一轮开始
////甩牌(升级中使用)
//#define ASS_SHOW_CARD					90				//甩牌失败
//#define ASS_BATCH_OUT					91				//批量出牌
//
//#define ASS_CONTINUE_END				95				//游戏结束
//#define ASS_NO_CONTINUE_END				96				//游戏结束
//#define ASS_CUT_END						97				//用户强行离开
//#define ASS_SALE_END					98				//游戏安全结束
//
////辅助
//#define ASS_MESSAGE						100				//从服务端发送文字消息
//#define ASS_AUTO						101				//托管
//#define ASS_HAVE_THING					102				//有事要走
//#define ASS_LEFT_RESULT					103				//有事要走结果
//#define ASS_STOP_THING					104				//提前结束
//#define ASS_AGREE_STOP					105				//提前结束结果
//#define ASS_LOSE						106				//投降
//#define ASS_LOSE_RESULT					107				//投降结果
//#define ASS_BUY_PROP					108				//买道俱
//#define ASS_SUPER_CLIENT				109				//超级客户端
//#define ASS_OUT_CARD_ERROR              110             //客户端出牌错误
/********************************************************************************/

#endif