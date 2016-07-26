/** @file	mjcfgdef.h
 *	@brief	麻将框架的配置信息
 *	@author edward
 *	@date 08.08.04
 */
#ifndef _MJCFGDEF_H
#define _MJCFGDEF_H

#include "..\typeDef.h"

namespace MyGame
{

#define MAX_FAN_NUMBER		128
#define MAX_FAN_NAME		18
#define MAX_FAN_CHECK_NBR	4
#define MAX_FAN_NOCHECK_NBR	12	

/**
 * @ingroup MJFrame
 * @brief 一个番的数据结构
 */	
typedef struct tagFAN_NODE
{
	char		byCount;					     // 数量
	BOOL		bFan;						     // 每种番是否满足
	BOOL		bCheck;						     // 是否检查该项
	char		nCheckNbr;					     // 判断规则的数量
	char		nNoCheckNbr;				     // 取消判断的番的数量
	char        nCheckFunID[MAX_FAN_CHECK_NBR];	 // 判断的规则
	char		nNoCheckFan[MAX_FAN_NOCHECK_NBR];// 当本番满足时，取消判断的其他冲突番
	int		    byFanPoint;					     // 每种番是多少番
	char		szFanName[MAX_FAN_NAME];	     // 每种番的名字
}FAN_NODE; 

/**
 * @ingroup MJFrame
 * @brief 所有番的数据结构
 */	
typedef	struct tagFAN_COUNT
{
	FAN_NODE	m_FanNode[MAX_FAN_NUMBER];
}FAN_COUNT;

/**
 * @ingroup MJFrame
 * @brief 麻将用到的配置信息
 * 10.01.21 排列顺序被打乱，采用字节对齐
 */	
typedef struct tagMJSettingData
{
	// 支持一炮多响 
	char nMoreWin;	

	// 拦牌的等级 0为不拦牌
	// nBlockLevelTing的判断时机依然只有在自己拦牌的时候才会使用
	char nBlockLevelCollect;
	char nBlockLevelTriplet;
	char nBlockLevelQuadruplet;
	char nBlockLevelTing;
	char nBlockLevelWin;

	// 抢杠
	char nQiangGang;
	// 漏胡
	char nLouHu;
    // 无翻胡
    char nWuFanHu;
	// 杠牌的动画显示，从后还是从前摸，不影响逻辑
	char nDrawGangBack;
	// 风牌也算到顺子里面去
	char nCollateWind;
	// 箭头牌也算到顺子里面去
	char nCollateArrow;
	// 都可以吃牌
	char nAllCanCollate;
	// 是否开听牌提示
	char nShowTing;
	// 是否开了听牌后依然可全杠
	char nTingCanGang;
    // 是否暗杠明牌(0: 只有暗杠牌玩家才能看到,  1: 所有玩家都能看到暗杠数据)
    char nAnGangShow;
    // 是否开了东南西北杠
    char nDongNanXiBeiGang;
    // 是否开了中发白杠
    char nZhongFaBaiGang;
	// 是否允许作为普通牌来吃胡
	char nCanWinLaizi;
	// 癞子是否可以被吃碰杠
	char nCanBlockLaizi;
	// 是否打开了测试通道
	char nOpenGM;
	// 城墙组成
	char nTilesChar;
	char nTilesBamboo;
	char nTilesBall;
	char nTilesWind;
	char nTilesArrow;
	char nTilesFlower;
	
	// 特殊牌型判断
	// 如果本地麻将还有新的就不用再写这里了
    char nCheck7dui;		// 7对
    char nCheckQuanbukao;	// 全不靠
    char nCheck131;		    // 131
    char nCheckZuhelong;	// 组合龙
    char nCheckLualv;		// 全绿
    char nCHeckLuafeng; 	// 全风

	char nTimerGive;
	char nTimerBlock;
	char nTimerReady;
	char nTimerServer;
	char nTimerFlower;

    char nTimerCutCard;     // 切牌操作时间
    char nTimerDice;        // 骰子操作时间
    char nTimerDeal;        // 发牌操作时间
    char nTimerJP;          // 精牌操作时间

    char nOpenLookTile;     // 是否看牌(游戏结束后, 是否显示未胡牌的玩家的牌)
    char nTimerLookTile;    // 推完牌后隔多长时间进行洗牌

    char nGiveBlockShow;    // 是否显示出牌区域中被吃碰杠的牌
	char nGiveOrder;        // 出牌顺序(0: 代表顺时针, 1: 代表逆时针, 读取到内在中会转化为1与3, 方便游戏进行下一个玩家位置计算)

	// 门票，结算，逃跑的扣费方式
	char nMoneyType;
	// 逃跑扣分分成
	char nEscapeShareScore;
	// 1局使用道具的数量上限，房间配置
	char nItemLimitTimes;

	// 非胡牌的手牌数量
	char nTilesHandNbr;	
	// 总的牌数量
	int nTilesNumber;
	int nMaxFan;
	int nBaseScore;
	int nMinFan;
    
	// 门票
	int nTicket;
	// 杠牌积分(为0则不提示出来)
	int nGangScore;
	// 分转钱
	int nScoreToMoney;
	// 最大输钱
	int nMaxMoney;

	// 语音道具的价格，如果不为0，使用时会有扣费提示
	int nVoicePrice;
	// 1局使用道具的价格上限，房间配置
	int nItemLimitMoney;

	// 购买道具的价格下限
	int nBuyItemMoney;
	FAN_COUNT tagFanCount;
}MJ_SETTING_DATA;

// 蓝钻赠送相关链接最大存储数量
#define PRESENTGIFT_URL_MAX		6
// 蓝钻赠送相关链接路径		
#define PRESENTGIFT_URL_PATH	128		
// 蓝钻赠送升级最高等级数
#define PRESENTGIFT_RANK_MAX	32
// 蓝钻赠送升级赠送道具数量
#define PRESENTGIFT_LEVEL_GIFT	6
// 蓝钻赠送每日赠送道具数量
#define PRESENTGIFT_TODAY_GIFT  6
// 蓝钻等级，这里设置为7
#define PERSENTGIFT_LEVEL_MAX	7

// 蓝钻赠送相关配置
typedef struct tagMJPresentGiftSetting
{
	// 蓝钻每日赠送间隔时间，如果为0则不会赠送
	int nPresentColdDown;

	// 蓝钻冒泡提示间隔时间
	int  nTipsColdDown;

    // 蓝钻赠送提示按钮的出现几率，最高100
	char  nShowPresentGiftBtn;

	// 蓝钻每日赠送价值
	int nTodayGiftWorth[PERSENTGIFT_LEVEL_MAX];
	// 蓝钻每日赠送道具ID，对应道具ID
	char nTodayGiftID[PERSENTGIFT_LEVEL_MAX][PRESENTGIFT_TODAY_GIFT];
	// 蓝钻每日赠送道具数量，时间道具全部设定为送到当天，不可配
	char nTodayGiftValue[PERSENTGIFT_LEVEL_MAX][PRESENTGIFT_TODAY_GIFT];

	// 蓝钻升级赠送价值
	int nLVGiftWorth[PERSENTGIFT_LEVEL_MAX];
	// 蓝钻升级赠送道具ID，对应道具ID
	char nLVGiftID[PERSENTGIFT_LEVEL_MAX][PRESENTGIFT_LEVEL_GIFT];
	// 蓝钻升级赠送道具数量，时间道具全部设定为送到当天，不可配
	char nLVGiftValue[PERSENTGIFT_LEVEL_MAX][PRESENTGIFT_LEVEL_GIFT];

	// 等级数量
	char ucRankCount;
	// 等级需要的积分
	int  nRankScore[PRESENTGIFT_RANK_MAX];

	// 刷新的时间点，以东8区为标准，精确度为小时，24小时制
	char ucUpdateTime;

    // 超链接数量
	char ucUrlCount; 
	// 超链接版本，如果版本相同，是不会更新客户端的超链接配置的
	char ucUrlVersion;
	// 超链接地址
	char szUrlPath[PRESENTGIFT_URL_MAX][PRESENTGIFT_URL_PATH];

}MJ_PRESENT_GIFT_SETTING;

}

#endif
