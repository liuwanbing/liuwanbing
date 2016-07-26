/**
* 消息定义类 UpgradeMessage.h   
*
* @author duanxiaohui
* @version 1.0
* @date 2009.12.15
*/

#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE

#include "GameMessage.h"

/// 文件名字定义
#define GAMENAME						TEXT("温州二张（16人）")

#ifdef VIDEO
#define NAME_ID							11901600					    /** 名字 ID */
#else
#define NAME_ID							11901600						/** 名字 ID */
#endif

#define PLAY_COUNT						16								/** 游戏人数 */

/// 版本定义
#define GAME_MAX_VER					1								/** 现在最高版本 */
#define GAME_LESS_VER					1								/** 现在最低版本 */
#define GAME_CHANGE_VER					0								/** 修改版本 */

/// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME

/// 游戏信息

/// 游戏内部开发版本号
#define DEV_HEIGHT_VERSION				13				/** 内部开发高版本号(每修正一次LOW_VERSIONG+1) */
#define DEV_LOW_VERSION					1				/** 内部开发低版本号 */

#define SKIN_CARD						TEXT("cardskin") /** 共用图片文件夹 */

static TCHAR szTempStr[MAX_PATH] =		{0};			/** 暂存字符串，用来获取 dll_name 等的时候使用 */
#define GET_CLIENT_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))
#define GET_SERVER_DLL_NAME(A)			(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))
#define GET_SKIN_FOLDER(A)				(sprintf(A,"%d",NAME_ID),strcat(A,""))

/// 游戏状态定义
#define GS_WAIT_SETGAME					0				/** 等待东家设置状态 */
#define GS_WAIT_ARGEE					1				/** 等待同意设置 */

#define GS_XIAZHU                       21              /** 下注状态 */
#define GS_PLAY_GAME					22				/** 游戏中状态 */
#define  GS_SEND_CARD                   23              ///发牌
#define GS_WAIT_NEXT					25				/** 等待下一盘开始  */

/// 按注类型
#define TYPE_GIVE_UP					0x00			/** 放弃 */
#define TYPE_NOTE						0x01			/** 下注 */
#define TYPE_ADD						0x02			/** 加注 */
#define TYPE_FOLLOW						0x03			/** 跟注 */
#define TYPE_SHUTTLE					0x04			/** 梭哈 */
													
/// 二张牌下注区域定义定义
#define GF_SHUNMEN_HNEG                 0          /** 横 */
#define GF_DAOMEN_HENG					1			/** 横 */
#define GF_SHUNMEN						2			/** 顺门 */
#define GF_DAOMEN			            3			/** 倒门 */
#define GF_SHUNMEN_JIAO					4		    /** 顺门-角 */
#define GF_DUIMEN						5			/** 对门 */

#define GF_DAOMEN_JIAO					6			    /** 倒门-角 */

// 游戏类型版本定义
#define GAME_VERSION_1                  0               /** 普通版本 */
#define GAME_VERSION_2                  1               /** 六分钟版本 */
#define GAME_VERSION_3                  2               /** 金葫芦2代版本 */

#define  REGION_COUNT                   7              ///当前可下注的区域

/// 玩家设置游戏消息结构
struct UserSetStruct
{
	bool				bFirstFollow;
	bool				bAllFollow;
	BYTE				bDeskStation;
	BYTE				bReserve;
};
///玩家开始叫庄
struct GameJiaoZhaungStruct
{
	BYTE  bDeskStation ; ///通知该玩家叫庄	
};
/// 游戏开始消息结构
struct GameBeginStruct
{
	int                 bZhuangStation;                 /**< 庄家 */
	int 				iBeenPlayGame;					/**< 游戏回合 */
	__int64             iZhuangResult ; 

	int					iBaseNote;						/**< 底注 */	
	int					iLimitNote;						/**< 最大注数 */

	BOOL                bNTGameMode;                    /**< 游戏庄模式: TRUE:无庄模式  FALSE:有庄模式 */
	int                 iZhuangMaxCount;                /**< 庄家最多最多少局 */
	char                chGameCount;                    /**< 游戏把数 */
	char                chGameBeen;                     /**< 游戏局数 */
	int                 iSelfMoney;                     /**< 刷新本身金币, 防止客户端金币不一致 */		
};

/// 游戏提前结束包消息结构
struct GameAheadStruct
{
	BYTE				bDeskStation;					/**< 胜者 */
	int					iTurePoint[PLAY_COUNT];			/**< 庄家得分 */
	int					iChangeMoney[PLAY_COUNT];		/**< 雪茄 */
};

/// 用户处理信息消息结构
struct tagUserProcess
{
	BYTE				iVrebType;					    /**< 所处理的按钮 */
	int					iNote;						    /**< 下注数 */
};

/// 机器人托管消息结构
struct UseAIStation
{
	BYTE				bDeskStation;				    /**< 坐号 */
	BOOL				bState;						    /**< 状态 */
};

/// 结算消息结构
struct GameFinishNotify
{
	__int64             iSelfMoeny;                     /**< 本局自己得分 */
	__int64             iReturnMoney;                   /**< 本局返回分 */
	__int64             iZhuangMoeny;                   /**< 本局庄家得分 */
};

/// 游戏状态数据包消息结构
struct GameStation_1
{
	/// 游戏版本
	BYTE				iVersion;						/**< 游戏版本号 */
	BYTE				iVersion2;						/**< 游戏版本号 */

	int					iRoomBasePoint;					/**< 房间倍数 */
	int					iRunPublish;					/**< 逃跑扣分 */

	/// 游戏信息
	BYTE				iThinkTime;						/**< 游戏盘数 */
	BYTE				iBeginTime;						/**< 游戏盘数 */
	BYTE				iAllCardCount;					/**< 扑克数目 */
	int					iLimitNote;						/**< 最高注 */
	int					iBaseNote;						/**< 底注 */
	__int64             iMaxNote;                       /**< 闲家最高下注额 */
	__int64     		iMaxBaseNote;					//普通玩家的最高下注

	int                 iXiaZhuTime;                    /**< 下注时间 */
	int                 iKaipaiTime;                    /**< 开牌时间 */
	int                 iEmptyTime;                     /**< 空闲时间 */

	BOOL                bUseCard;                       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE                bGameMode;                      /**< 游戏模式 版本 0: 普通 1: 六分钟 2: 金葫芦2代*/
	BOOL				bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
	BOOL                bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	BOOL                bIsVipDouble;                   /**< 是否为双倍下注类型*/	
};

/// 游戏状态数据包	（ 等待其他玩家开始 ）消息结构
struct GameStation_2
{
	/// 游戏版本
	BYTE				iVersion;						/**< 游戏版本号 */
	BYTE				iVersion2;						/**< 游戏版本号 */
	int					iRoomBasePoint;					/**< 房间倍数 */
	int					iRunPublish;					/**< 逃跑扣分 */

	/// 游戏信息
	BYTE				iThinkTime;						/**< 游戏盘数 */
	BYTE				iBeginTime;						/**< 游戏盘数 */
	BYTE				iAllCardCount;					/**< 扑克数目 */
	int					iLimitNote;						/**< 最高注 */
	int					iBaseNote;						/**< 底注 */
	__int64             iMaxNote;                       /**< 闲家最高下注额 */
	__int64     		iMaxBaseNote;					//普通玩家的最高下注
	
	int                 iXiaZhuTime;                    /**< 下注时间 */
	int                 iKaipaiTime;                    /**< 开牌时间 */
	int                 iEmptyTime;                     /**< 空闲时间 */

	BOOL                bIsApplyXZ;                     /**< 庄家是否有申请下庄 */

    int                 iKaiPaiCount;                   /**< 开牌记录次数 */
	BYTE                bIsKaiPai[GAME_XIPAI_COUNT];    /**< 本局是否开(0x01:顺门、0x02:对门、0x04:倒门) */
	BYTE                bIsXiaZhu[GAME_XIPAI_COUNT];    /**< 本局是否下注(0x01:顺门、0x02:对门、0x04:倒门) */

	BYTE                userList[PLAY_COUNT];           /**< 上庄列表数据 */


	BOOL                bUseCard;                       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE                bGameMode;                      /**< 游戏模式 版本 0: 普通 1: 六分钟 2: 金葫芦2代*/	
	BOOL				bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/	
	BOOL                bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	BOOL                bIsVipDouble;                   /**< 是否为双倍下注类型*/

	BYTE                bNTStation;                      ///当前的庄家位置
};

//游戏状态数据包	（ 游戏正在发牌状态 ）消息结构
struct GameStation_3
{
	/// 游戏版本
	BYTE				iVersion;						/**< 游戏版本号 */
	BYTE				iVersion2;						/**< 游戏版本号 */
	int					iRoomBasePoint;					/**< 房间倍数 */
	int					iRunPublish;					/**< 逃跑扣分 */

	/// 游戏信息
	BYTE				iThinkTime;						/**< 游戏盘数 */
	BYTE				iBeginTime;						/**< 游戏盘数 */
	BYTE				iAllCardCount;					/**< 扑克数目 */
	int					iLimitNote;						/**< 最高注 */
	int					iBaseNote;						/**< 底注 */
	__int64             iMaxNote;                       /**< 闲家最高下注额 */
	__int64     		iMaxBaseNote;					//普通玩家的最高下注

	int                 iXiaZhuTime;                    /**< 下注时间 */
	int                 iKaipaiTime;                    /**< 开牌时间 */
	int                 iEmptyTime;                     /**< 空闲时间 */

	int                 iTime;                          /**< 当前剩余时间 */

	/// 运行状态变量
	int					iUpGradePeople;					/**< 庄家位置 */
	BOOL                bIsApplyXZ;                     /**< 庄家是否有申请下庄 */
	BOOL                bIsInNoteStage ;                ///是否是出于下注阶段
	BYTE				iBeenPlayGame;					/**< 已经游戏的局数 */
	__int64             iZhuangMoenyResult;             /**< 庄家输赢金币 */
	__int64             iSelfMoenyResult;               /**< 自己输赢金币 */
	__int64             iXiaZhuInfo[7];                 /**< 桌子上的下注数据  0顺门，1顺门-角，2对门，3桥，4倒门，5倒门-角 */
	__int64             iMyZhu[7];                      /**< 本人下注 */
	__int64             iRobotZhu[7];                   /**< 机器人每个区域下的注额 */

	BYTE                userList[PLAY_COUNT];           /**< 上庄列表数据 */

	int                 iKaiPaiCount;                   /**< 开牌记录次数 */
	BYTE                bIsKaiPai[GAME_XIPAI_COUNT];    /**< 本局是否开(0x01:顺门、0x02:对门、0x04:倒门) */
	BYTE                bIsXiaZhu[GAME_XIPAI_COUNT];    /**< 本局是否下注(0x01:顺门、0x02:对门、0x04:倒门) */

	BOOL                bUseCard;                       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE                bGameMode;                      /**< 游戏模式 版本 0: 普通 1: 六分钟 2: 金葫芦2代*/
	BOOL				bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
	BOOL                bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	BOOL                bIsVipDouble;                   /**< 是否为双倍下注类型*/	
	char                chGameCount;                    /**< 游戏把数 */
	char                chGameBeen;                     /**< 游戏局数 */

	int                 iLuziCount;                     /**< 路子数量 */
	GameLuziInfo        ArrLuzi[GAME_XIPAI_COUNT];      /**< 路子数据 */	
};

/// 游戏状态数据包	（ 游戏中状态 ）消息结构
struct GameStation_4	
{
	/// 游戏版本
	BYTE				iVersion;						/**< 游戏版本号 */
	BYTE				iVersion2;						/**< 游戏版本号 */

	/// 游戏信息
	BYTE				iThinkTime;						/**< 游戏盘数 */
	BYTE				iBeginTime;						/**< 游戏盘数 */
	BYTE				iAllCardCount;				    /**< 扑克数目 */
	
	int					iRoomBasePoint;					/**< 房间倍数 */
	int					iRunPublish;					/**< 逃跑扣分 */
	int					iLimitNote;						/**< 最高注 */
	int					iBaseNote;						/**< 底注 */
	__int64             iMaxNote;                       /**< 闲家最高下注额 */
	__int64     		iMaxBaseNote;					//普通玩家的最高下注

	int                 iXiaZhuTime;                    /**< 下注时间 */
	int                 iKaipaiTime;                    /**< 开牌时间 */
	int                 iEmptyTime;                     /**< 空闲时间 */

	int                 iTime;                          /**< 当前剩余时间 */

	/// 运行状态变量
	int					iUpGradePeople;					/**< 庄家位置 */
	BOOL                bIsApplyXZ;                     /**< 庄家是否有申请下庄 */
	BYTE				iBeenPlayGame;					/**< 已经游戏的局数 */
	__int64             iZhuangMoenyResult;             /**< 庄家输赢金币 */
	__int64             iSelfMoenyResult;               /**< 自己输赢金币 */
	__int64             iXiaZhuInfo[7];                 /**< 桌子上的下注数据  0顺门，1顺门-角，2对门，3桥，4倒门，5倒门-角 */
	__int64             iMyZhu[7];                      /**< 本人下注 */
	__int64             iRobotZhu[7];                   /**< 机器人每个区域下的注额 */

	
	PaiData             cardData[4];                    /**< 牌数据 */
	BYTE                userList[PLAY_COUNT];           /**< 上庄列表数据 */

	int                 iKaiPaiCount;                   /**< 开牌记录次数 */
	BYTE                bIsKaiPai[GAME_XIPAI_COUNT];    /**< 本局是否开(0x01:顺门、0x02:对门、0x04:倒门) */
	BYTE                bIsXiaZhu[GAME_XIPAI_COUNT];    /**< 本局是否下注(0x01:顺门、0x02:对门、0x04:倒门) */

	BOOL                bUseCard;                       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE                bGameMode;                      /**< 游戏模式 版本 0: 普通 1: 六分钟 2: 金葫芦2代*/
	BOOL				bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
	BOOL                bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	BOOL                bIsVipDouble;                   /**< 是否为双倍下注类型*/	
	char                chGameCount;                    /**< 游戏把数 */
	char                chGameBeen;                     /**< 游戏局数 */

	int                 iLuziCount;                     /**< 路子数量 */
	GameLuziInfo        ArrLuzi[GAME_XIPAI_COUNT];      /**< 路子数据 */	
};

/// 游戏状态数据包	（ 等待下盘开始状态  ）消息结构
struct GameStation_5
{
	/// 游戏版本
	BYTE				iVersion;						/**< 游戏版本号 */
	BYTE				iVersion2;						/**< 游戏版本号 */
	int					iRoomBasePoint;					/**< 房间倍数 */
	int					iRunPublish;					/**< 逃跑扣分 */

	BYTE				iThinkTime;						/**< 游戏盘数 */
	BYTE				iBeginTime;						/**< 游戏盘数 */

	int					iLimitNote;						/**< 最高注 */
	int					iBaseNote;						/**< 底注 */
	__int64             iMaxNote;                       /**< 闲家最高下注额 */
	__int64     		iMaxBaseNote;					//普通玩家的最高下注

	int                 iXiaZhuTime;                    /**< 下注时间 */
	int                 iKaipaiTime;                    /**< 开牌时间 */
	int                 iEmptyTime;                     /**< 空闲时间 */

	int                 iTime;                          /**< 当前剩余时间 */

	/// 运行状态变量
	int					iUpGradePeople;					/**< 庄家位置 */
	BOOL                bIsApplyXZ;                     /**< 庄家是否有申请下庄 */
	BYTE				iBeenPlayGame;					/**< 已经游戏的局数 */
	__int64             iZhuangMoenyResult;             /**< 庄家输赢金币 */
	__int64             iSelfMoenyResult;               /**< 自己输赢金币 */

	BYTE                userList[PLAY_COUNT];           /**< 上庄列表数据 */

	int                 iKaiPaiCount;                   /**< 开牌记录次数 */
	BYTE                bIsKaiPai[GAME_XIPAI_COUNT];    /**< 本局是否开(0x01:顺门、0x02:对门、0x04:倒门) */
	BYTE                bIsXiaZhu[GAME_XIPAI_COUNT];    /**< 本局是否下注(0x01:顺门、0x02:对门、0x04:倒门) */

	BOOL                bUseCard;                       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE                bGameMode;                      /**< 游戏模式 版本 0: 普通 1: 六分钟 2: 金葫芦2代*/
	BOOL				bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
	BOOL                bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	BOOL                bIsVipDouble;                   /**< 是否为双倍下注类型*/	
	char                chGameCount;                    /**< 游戏把数 */
	char                chGameBeen;                     /**< 游戏局数 */

	int                 iLuziCount;                     /**< 路子数量 */
	GameLuziInfo        ArrLuzi[GAME_XIPAI_COUNT];      /**< 路子数据 */	
};

/// 控制庄家输赢数据包消息结构
typedef struct tagSuperSetting
{
	char bState[3];         /**< 0: 代表顺门 1: 代表对门 2: 代表倒门 (TRUE: 选中 FALSE: 未选中) */

}SUPERSETTING;

/// 通知客户端是否为超级客户端状态消息结构
typedef struct tagSuperState
{
	BYTE bDeskStation;      /**< 玩家位置 */
	BOOL bEnable;           /**< 是否开通 */
}SUPERSTATE;

/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					49				/** 用户设置游戏 */
#define ASS_UG_USER_SET_CHANGE			50				/** 玩家设置更改 */

                
#define ASS_USER_JIAO_ZHUANG            51              ///玩家叫庄
#define ASS_GAME_BEGIN					52				/** 升级游戏开始 */

#define ASS_SEND_CARD					53				/** 发牌信息 */
#define ASS_SEND_FINISH					54				/** 发牌完成 */
#define ASS_GAME_PLAY					55				/** 开始游戏 */
#define ASS_NOTE_LIMIE					56				/** 本轮限注 */

#define  ASS_SEND_NOTE_MSG              57               ///通知洗牌
#define ASS_APPLY_NT                    58             /** 玩家申请上庄*/

#define ASS_NOTE						60				/** 下注 */
#define ASS_VREB_CHECK					61				/** 用户处理 */
#define ASS_REPLACE_VREB_CHECK			62				/** 代替(断线)用户处理 */
#define ASS_NOTE_RESULT					63				/** 下注结果 */

#define ASS_COLLECT_JETON				70				/** 收集筹码 */
#define ASS_AI_STATION					71				/** 机器人托管 */

#define ASS_NEW_TURN					80				/** 新一轮开始 */
#define ASS_SEND_A_CARD					81				/** 新发一张牌 */

#define ASS_CONTINUE_END				90				/** 游戏结束 */
#define ASS_NO_CONTINUE_END				91				/** 游戏结束 */
#define ASS_CUT_END						92				/** 用户强行离开 */
#define ASS_SALE_END					93				/** 游戏安全结束 */
#define ASS_NO_WINER					94				/** 游戏结束无胜出者 */
#define ASS_AHEAD_END					95				/** 提前结束 */

#define ASS_SUPER_SETTING               150             /** 由客户端发送消息控制本局输赢 */
#define ASS_SUPER_SETTING_SUCCEED       151             /** 由服务端发送客户端超端设置成功消息 */
#define ASS_SUPER_STATE                 152            /** 通知客户端是否为超级状态 */
#define ASS_NOT_START_GAME              153            /**	无庄不开始游戏 */

/********************************************************************************/
#endif
