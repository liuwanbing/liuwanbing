/** @file	MJFrameBaseData.h
 *	@brief	所有麻将相关的定义信息
 *	@author edward
 *	@date 08.08.04
 */
#ifndef _MJFRAMEBASEDATA_H
#define _MJFRAMEBASEDATA_H

#include "..\commonStructs.h"
#include "mjdef.h"
#include "mjcfgdef.h"
#include "MJ.h"
#include "..\typeDef.h"

namespace MyGame
{

#define MJ_MONEY_TYPE_NORMAL		0
#define MJ_MONEY_TYPE_GAMEMONEY		1
#define MJ_MONEY_TYPE_HAPPYMONEY	2

// 默认显示长度
// #define MJ_INFO_HAND_NBR	18	// 手牌
// #define MJ_INFO_GIVE_NBR	36  // 出牌
#define MJ_INFO_WALL_NBR	36  // 城墙

// 拦牌操作栏显示数
#define MJ_BLOCK_NBR		        8	// (吃碰杠听胡)
#define MJ_BLOCK_COLLECT	        0
#define MJ_BLOCK_TRIPLET	        1
#define MJ_BLOCK_QUADRUPLET	        2
#define MJ_BLOCK_TING		        3
#define MJ_BLOCK_WIN		        4
#define MJ_FLOWER_NBR		        8
#define DICE_NBR	                2

// 自己操作的状态
#define MJ_STATE_SELECT_NONE	    0	// 不该自己选择操作
#define MJ_STATE_SELECT_GIVE	    1	// 选择出牌
#define MJ_STATE_SELECT_COLLECT	    2	// 吃牌选择
#define MJ_STATE_SELECT_QUADRUPLET	3	// 杠牌的选择
#define MJ_STATE_SELECT_TING	    4   // 听牌选择

// 时间ID映射数量
#define MJ_TIMERID_MAP_NBR	        64
	
// 状态部分
#define MJ_STATE_NOSTART	        0	// 没开始
#define MJ_STATE_DEAL		        1	// 发牌
#define MJ_STATE_FLOWER		        2	// 补花
#define MJ_STATE_BLOCK		        3	// 拦牌
#define MJ_STATE_GIVE		        4	// 出牌
#define MJ_STATE_DRAW		        5	// 摸牌
#define MJ_STATE_WAIT		        6	// 等待
#define MJ_STATE_END		        7	// 结束
#define MJ_STATE_DRAW_WAIT	        8	// 摸牌等待
#define MJ_STATE_NONE		        9	// 数量

// 判断番型时使用的胡牌类型
#define MJ_WIN_SELFDRAW				0	// 自摸
#define MJ_WIN_GUN					1	// 点炮
#define MJ_WIN_GANGDRAW				2	// 杠上花
#define MJ_WIN_GANG					3	// 抢杠
#define MJ_WIN_GANGGIVE				4	// 杠上炮

// 胡牌类型
#define MJ_END_NO					0	// 没结束  
#define MJ_END_SELF					1	// 自摸
#define MJ_END_GUN					2	// 放炮
#define MJ_END_NOTILE				3	// 荒牌
#define MJ_END_ESCAPE				4	// 逃跑
#define MJ_END_REQUEST				5	// 协商推出
#define MJ_END_SYSTEM				6	// 解散
#define MJ_END_QIANG				7	// 抢杠


#define MJ_GANG_STATE_NONE			0
#define MJ_GANG_STATE_DRAW			1	// 杠或花
#define MJ_GANG_STATE_GIVE			2	// 炮
#define MJ_GANG_STATE_BLOCK			3	// 抢

#define MJ_TING_NONE				0	
#define MJ_TING_NORMAL				1
#define MJ_TING_BEGIN				2	// 天听

// 时间类型
// 秒级定时器ID范围 1 - 50
#define MJ_TIMER_READY				1
#define MJ_TIMER_GIVE				2
#define MJ_TIMER_BLOCK				3
#define MJ_TIMER_WAIT				4
#define MJ_TIMER_FLOWER				5

// 平台毫秒定时器ID: 游戏人数(4) - 50, 小于等于4或大于等于50无效
#define MJ_TIMER_DRAW       		10   // 定时摸牌
#define MJ_TIMER_DEAL_FINISH        11   // 定时确认发牌完成

// 超时2次及托管
#define MJ_TURST_TIMEOUT_MAX	    2

// 客户端的时间定义
#define MJ_GAME_TIMER_LOGIC	        10	// 逻辑时间，同上面的整一起 游戏启动就运行的时间

// GM命令,下面每一条不能有重复字，不然解析可能有问题
#define GMCMD_SWAP "swap"
#define GMCMD_SET  "set"
#define GMCMD_NEXT "next"

#define GMCMD_SET_WALLLEN	"walllen"
#define GMCMD_SET_AUTOSTART "autostart"
#define GMCMD_SET_DEALER	"dealer"
#define GMCMD_SET_DICE		"dice"
#define GMCMD_SET_WALL		"walltile"

// 通知客户端启动自动开始
#define GMCMD_NOTIFY_AUTOSTART	1

// 麻将类的主消息ID
#define MJFRAME_MOUDLE_MSG	80
	
#define CS_MJ_REQUSET_REPLACE_ALLTILES	1	//自动化配牌
#define SC_MJ_NOTIFY_REPLACE_ALLTILES	2	

#define CS_MJ_REQUEST_GIVE				3
#define SC_MJ_NOTIFY_GIVE				4

#define SC_MJ_NOTIFY_DRAW				5

#define CS_MJ_REQUEST_BLOCK				6
#define SC_MJ_NOTIFY_BLOCK				7
#define SC_MJ_NOTIFY_BLOCK_RESULT		8

#define CS_MJ_REQUEST_FLOWER			9
#define SC_MJ_NOTIFY_FLOWER				10 
#define SC_MJ_NOTIFY_DEAL				11
#define SC_MJ_NOTIFY_FINISH				12
#define SC_MJ_NOTIFY_DEALER             52
#define CS_MJ_REQUEST_DEAL_FINISH       56  // 请求发牌完成

// 请求配置手牌
#define CS_MJ_REQUEST_CHANGE_TILES		13
#define MJ_CHANGE_RANDOM	            0	// 随机换
#define MJ_CHANGE_TARGET	            1	// 换指定目标
#define SC_MJ_NOTIFY_CHANGE_TILES		14

// 请求获取指定牌的位置
#define CS_MJ_REQUEST_GETPOS_TILE		15
#define SC_MJ_NOTIFY_GETPOS_TILE		16

// 请求获取后面的4张牌信息
#define CS_MJ_REQUEST_GETTILES			17
#define MJ_GETTILES_NBR		            4
#define SC_MJ_NOTIFY_GETTILES			18


// 请求配牌
#define CS_MJ_REQUEST_CONFIGTILE		20
#define SC_MJ_NOTIFY_CONFIGTILE			21

// 强制刷新玩家手牌
#define SC_MJ_UPDATEHANDTILES			23

// 旁观和断线续玩的刷新消息
#define SC_MJ_NOTIFY_VIEW_INFO			24

// 玩家修改旁观后发给旁观者的消息
#define SC_MJ_NOTIFY_CHANGE_LOOKON		25

// 玩家进来发的一些基础信息
#define SC_MJ_NOTIFY_BASE_INFO			26

// 结束补花状态，正式开始游戏
#define SC_MJ_NOTIFY_FLOWER_OVER		27

// 弹出对话框提示信息
#define SC_MJ_NOTIFY_SHOWDLG			28

// 测试消息
#define CS_MJ_REQUEST_CHECK_WIN			31
#define SC_MJ_NOTIFY_CHECK_WIN			32

// 测试GM消息
#define CS_MJ_REQUEST_GMCMD				33
#define SC_MJ_NOTIFY_GMCMD				34

// 连局的信息
#define SC_MJ_NOTIFY_RUN_NBR			35

// SO强制刷新客户端数据
#define SC_MJ_NOTIFY_RESET_DATA			50


#pragma pack(1)

// 游戏准备消息(主ID180, 次ID2, 1已被平台截取掉了)
#define ASS_SUB_USERAGREE  50

// 游戏托管消息(主ID180, 次ID10)
#define ASS_SUB_TRUSTEE  51

//用户同意消息结构
struct MSG_GR_S_Agree 
{
	int		bDeskNO;						//游戏桌号
	int		bDeskStation;					//位置号码
	int		bAgreeGame;						//同意标志
	int		bNotUse;						//仅仅用于对齐占位置
};

// 托管消息结构体
struct MSG_GR_S_UserTruste
{
	int byChair;
	int byFlag;   // 0: 取消托管   1: 托管 
};

/**
 * @ingroup MJFrame
 * @brief 请求配牌的数据结构
 *
 * 如果设置成功在初始化一局的时候,SO不会执行洗牌和设置庄家操作
 */	
struct TCMD_REQUEST_REPLACE_ALLCARDS : public TCMD_HEADER
{
	int nCount;		   // 城墙长度
	BYTE szCards[144]; // 牌
	char cbBankUser;   // 庄
    BYTE ucDice[2];	   // 2个色子
	TCMD_REQUEST_REPLACE_ALLCARDS()
	{
		memset(this, 0, sizeof(TCMD_REQUEST_REPLACE_ALLCARDS));
		cCmdPara = CS_MJ_REQUSET_REPLACE_ALLTILES;
		cCmdID = MJFRAME_MOUDLE_MSG;
		nDataLen = sizeof(TCMD_REQUEST_REPLACE_ALLCARDS);
	}
};

/**
 * @ingroup MJFrame
 * @brief 获取配牌结果的数据结构
 */	
struct TCMD_NOTIFY_REPLACE_ALLCARDS : public TCMD_HEADER
{
	// 结果
	int nResult;
	TCMD_NOTIFY_REPLACE_ALLCARDS()
	{
		memset(this, 0, sizeof(TCMD_NOTIFY_REPLACE_ALLCARDS));
		cCmdPara = SC_MJ_NOTIFY_REPLACE_ALLTILES;
		cCmdID = MJFRAME_MOUDLE_MSG;
		nDataLen = sizeof(TCMD_NOTIFY_REPLACE_ALLCARDS);
	}
};

/**
 * @ingroup MJFrame
 * @brief 请求出牌
 */	
struct TCMD_REQUEST_GIVE: public TCMD_HEADER 
{
	int t;		
	int ucTileLength;	// 出牌后的手牌长度
	int ucIndex;		// 出牌玩家出的牌是手牌中哪个位置的牌
	int standBlank;		// 占位置
    TCMD_REQUEST_GIVE()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_GIVE));
        cCmdPara = CS_MJ_REQUEST_GIVE;
 		cCmdID = MJFRAME_MOUDLE_MSG;
		nDataLen = sizeof(TCMD_REQUEST_GIVE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知出牌
 */	
struct TCMD_NOTIFY_GIVE: public TCMD_HEADER 
{
	int t;	
	int chair;
	int ucHandType; // 手动画类型
	int ucIndex;    // 出牌玩家出的牌是手牌中哪个位置的牌
	int ucInsertPos;// 出牌玩家出牌后, 最右边这张刚摸的牌摆放位置
    BOOL bTing;      // 是否为听牌出牌

    TCMD_NOTIFY_GIVE()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_GIVE));
        cCmdPara = SC_MJ_NOTIFY_GIVE;
 		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_GIVE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 请求拦牌
 */	
struct TCMD_REQUEST_BLOCK: public TCMD_HEADER 
{
	BYTE ucFlag;		// 根据吃碰杠定义标志 ACTION_EMPTY
	BYTE ucTile[4];		// 牌数据
    TCMD_REQUEST_BLOCK()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_BLOCK));
        cCmdPara = CS_MJ_REQUEST_BLOCK;
 		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_BLOCK);
    }
};

/**
* @ingroup MJFrame
* @brief 请求发牌完成
*/	
struct TCMD_REQUEST_DEAL_FINISH : public TCMD_HEADER
{
    TCMD_REQUEST_DEAL_FINISH ()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_DEAL_FINISH));
        cCmdPara = CS_MJ_REQUEST_DEAL_FINISH;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_DEAL_FINISH);
    }
};

/**
* @ingroup MJFrame
* @brief 通知庄家位置
*/	
struct TCMD_NOTIFY_DEALER : public TCMD_HEADER
{
    BYTE byDealer;	  // 庄家位置

    TCMD_NOTIFY_DEALER()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_DEALER));
        cCmdPara = SC_MJ_NOTIFY_DEALER;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_DEALER);
    }
};

/**
* @ingroup MJFrame
* @brief 通知拦牌
*/	
struct TCMD_NOTIFY_BLOCK: public TCMD_HEADER 
{
    BYTE ucFlag;		    // 没有(0)、吃(1)、碰(2)、杠(4)、听(8)、和(16)
    BYTE ucTile[5][12];     // 吃(0)、碰(1)、杠(2)、听(3)、和(4)牌数据(按最大计算, 存放3个杠牌, 吃碰杠数据是连续存放的)
    BYTE ucChair;		    // 拦牌位置
    BYTE ucTingTile[12][5]; // 听牌后可以胡的牌与ucTile对应

    TCMD_NOTIFY_BLOCK()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_BLOCK));
        cCmdPara = SC_MJ_NOTIFY_BLOCK;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_BLOCK);
    }
};


/**
 * @ingroup MJFrame
 * @brief 通知拦牌结果
 */	
struct TCMD_NOTIFY_BLOCK_RESULT: public TCMD_HEADER 
{
	BYTE ucFlag;		// 吃、碰、杠、听、和
	BYTE ucTile[4];   	// 牌数据
	BYTE ucChair;		// 谁吃、碰的
	BYTE ucWhich;		// 吃、碰谁的

    TCMD_NOTIFY_BLOCK_RESULT()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_BLOCK_RESULT));
        cCmdPara = SC_MJ_NOTIFY_BLOCK_RESULT;
 		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_BLOCK_RESULT);
    }
};


/**
* @ingroup MJFrame
* @brief 通知摸牌
*/	
struct TCMD_NOTIFY_DRAW : public TCMD_NOTIFY_BLOCK 
{
    // 因所有逻辑全放在服务器判断, 所以继承拦牌消息, 拦牌消息与摸牌消息合并在一起发下去
    BYTE t;	                // 牌数据
    BYTE chair;             // 摸牌人
    BYTE t_target;          // 摸牌方位(0: 正常抓牌,  1:最后的位置抓牌(补花)) 
	BYTE byDice[DICE_NBR];  // 骰子(某些特殊需求, 杠牌被的那张牌由骰子决定)

    TCMD_NOTIFY_DRAW()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_DRAW));
        cCmdPara = SC_MJ_NOTIFY_DRAW;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_DRAW);
    }
};


/**
 * @ingroup MJFrame
 * @brief 请求补花
 */	
struct TCMD_REQUEST_FLOWER: public TCMD_HEADER 
{
	BYTE ucChair;		//	谁补
	BYTE ucNumber;		//	补了几张

    TCMD_REQUEST_FLOWER()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_FLOWER));
        cCmdPara = CS_MJ_REQUEST_FLOWER;
 		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_FLOWER);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知补花
 */	
struct TCMD_NOTIFY_FLOWER: public TCMD_HEADER 
{
	BYTE ucChair;					//	谁补
	BYTE ucNumber;					//	补了几张
	BYTE ucTile[MJ_FLOWER_NBR];	    //	哪几张花牌
	BYTE ucOldTile[MJ_FLOWER_NBR];	//	要换的是哪几张
	BYTE ucFlower[MJ_FLOWER_NBR];	//  8花的所属

    TCMD_NOTIFY_FLOWER()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_FLOWER));
        cCmdPara = SC_MJ_NOTIFY_FLOWER;
		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_FLOWER);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知开局发牌
 */	
struct TCMD_NOTIFY_DEAL: public TCMD_HEADER 
{
	int tiles[17];		    // 自己的手牌
	int ucDealer;		    // 庄家
	int ucRoundWind;	    // 圈风
	int ucRoundWindNbr;	    // 该圈的第几轮
	int ucDice[2];		    // 两个骰子的数字
	int ucHandType;         // 启动骰子时, 使用的手指类型: 0: 食指 1: 中间
	int ucGodLength;	    // 万能牌的长度
	int ucGodTiles[8];	    // 万能牌
	int ucOther[8];	        // 其他信息

    TCMD_NOTIFY_DEAL()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_DEAL));
        cCmdPara = SC_MJ_NOTIFY_DEAL;
		cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_DEAL);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知结束
 */	
struct TCMD_NOTIFY_FINISH: public TCMD_HEADER 
{
	BYTE byFlag;			     // 状态，荒牌、点炮、自摸
	BYTE byFan[4][128];			 // 胡了哪些番
	BYTE byFanScore[4][128];     // 每种番型赢的番数
    int  nFanPoint[4];           // 四家输赢翻数
	int  nGangFen[4];            // 四家输赢杠分
	BYTE byWhoWin[4];		     // 谁和的(要支持一炮多响)
	BYTE byWhoGun;			     // 被点炮的玩家
	BYTE tLast;			         // 和的那一张
	int	 nScore[4];              // 输赢分数
	int	 nMoney[4];              // 输赢金币
                                
	BYTE tile[4][18];            // 四个玩家手上的牌
    BYTE byBlockTiles[4][20];	 // 玩家的拦牌数据(数据连续存放)(吃碰与杠都是按照4个4个的存放)
    BYTE byBlockTilesFlag[4][5]; // 玩家拦牌的类型(吃碰杠标志)
    BYTE byBlockTilesWhich[4][5];// 玩家拦的谁的牌，或是拦的哪一张

	int  nTax[4];                // 台费

	BYTE byOtherData[4];		 // 备用数据
	BYTE byOtherData2[4]; 		 // 备用数据

    TCMD_NOTIFY_FINISH()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_FINISH));
		cCmdPara = SC_MJ_NOTIFY_FINISH;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_FINISH);
    }
};

/**
 * @ingroup MJFrame
 * @brief 请求换牌
 */	
struct TCMD_REQUEST_CHANGE_TILES: public TCMD_HEADER
{
	BYTE ucRequestTile; // 请求的牌
	BYTE ucChangeType;	// 更换类型
	BYTE ucTargetTile;	// 目标牌

    TCMD_REQUEST_CHANGE_TILES()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_CHANGE_TILES));
		cCmdPara = CS_MJ_REQUEST_CHANGE_TILES;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_CHANGE_TILES);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知换牌
 */	
struct TCMD_NOTIFY_CHANGE_TILES: public TCMD_HEADER
{
	BYTE ucRequestTile; // 请求的牌
	BYTE ucChangeType;	// 更换类型
	BYTE ucTargetTile;	// 目标牌
	BYTE ucResult;		// 换牌结果
    TCMD_NOTIFY_CHANGE_TILES()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_CHANGE_TILES));
		cCmdPara = SC_MJ_NOTIFY_CHANGE_TILES;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_CHANGE_TILES);
    }
};

/**
 * @ingroup MJFrame
 * @brief 请求获取目标牌位置
 */	
struct TCMD_REQUEST_GETPOS_TILE: public TCMD_HEADER
{
	BYTE ucRequestTile;	// 请求的牌

    TCMD_REQUEST_GETPOS_TILE()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_GETPOS_TILE));
		cCmdPara = CS_MJ_REQUEST_GETPOS_TILE;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_GETPOS_TILE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知获取的目标牌位置
 */	
struct TCMD_NOTIFY_GETPOS_TILE: public TCMD_HEADER
{
	BYTE ucRequestTile;	// 请求的牌
	BYTE ucTilePos;		// 牌的位置,0为没有

    TCMD_NOTIFY_GETPOS_TILE()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_GETPOS_TILE));
		cCmdPara = SC_MJ_NOTIFY_GETPOS_TILE;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_GETPOS_TILE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 请求获取一定数量的城墙牌数据
 */	
struct TCMD_REQUEST_GETTILES: public TCMD_HEADER
{
	BYTE ucGetNbr;	// 请求获取的数量
    TCMD_REQUEST_GETTILES()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_GETTILES));
		cCmdPara = CS_MJ_REQUEST_GETTILES;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_GETTILES);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知获取的城墙牌数据
 */	
struct TCMD_NOTIFY_GETTILES: public TCMD_HEADER
{
	BYTE ucGetTile[MJ_GETTILES_NBR];	// 查询的牌，最多不超过4个
	BYTE ucGetNbr;						// 查询的数量

    TCMD_NOTIFY_GETTILES()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_GETTILES));
		cCmdPara = SC_MJ_NOTIFY_GETTILES;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_GETTILES);
    }
};


/**
 * @ingroup MJFrame
 * @brief 请求使用配牌
 */	
struct TCMD_REQUSET_CONFIGTILE: public TCMD_HEADER
{
	BYTE ucChair;	// 使用的人
	
    TCMD_REQUSET_CONFIGTILE()
    {
        memset(this, 0, sizeof(TCMD_REQUSET_CONFIGTILE));
		cCmdPara = CS_MJ_REQUEST_CONFIGTILE;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUSET_CONFIGTILE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知使用配牌
 */	
struct TCMD_NOTIFY_CONFIGTILE: public TCMD_HEADER
{
	BYTE ucChair;	// 使用的人
	BYTE bResult;
    TCMD_NOTIFY_CONFIGTILE()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_CONFIGTILE));
		cCmdPara = SC_MJ_NOTIFY_CONFIGTILE;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_CONFIGTILE);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知刷新手牌
 */	
struct TCMD_NOTIFY_UPDATEHANDTILES: public TCMD_HEADER
{
	BYTE ucChair;				// 更新的人
	BYTE ucHandCount;			// 手牌数量
	BYTE ucHandTiles[18];		// 手牌数据
	BYTE ucGiveCount[4];		// 出牌数量
	BYTE ucGiveTiles[4][36];	// 出牌的数据
	TCMD_NOTIFY_UPDATEHANDTILES()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_UPDATEHANDTILES));
		cCmdPara = SC_MJ_UPDATEHANDTILES;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_UPDATEHANDTILES);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知游戏中相关数据
 */	
struct TCMD_NOTIFY_VIEW_INFO: public TCMD_HEADER
{
	BYTE ucDealer;					        // 庄家
	BYTE ucWindRound;				        // 风
	BYTE ucWindRoundNbr;		        	// 圈
	BYTE ucCurTurn;				            // 当前操作者
	BYTE ucWallLength;				        // 城墙剩下的长度
	BYTE ucNowTime;				            // 当前还有的时间
	BYTE ucGameState;				        // 当前的游戏状态
	BYTE ucBLookOn;				            // 旁观
	BYTE ucLastTile;				        // 最后张牌
	BYTE ucLastGiveChair;			        // 最后出牌的人
	BYTE ucDice[2];			                // 色子点数
	BYTE ucHandTilesLength[4];		        // 玩家手牌的长度
	BYTE ucGiveTilesLength[4];		        // 玩家出牌的长度
	BYTE ucSetTilesLength[4];		        // 玩家拦牌的长度
	BYTE ucFlower[8];	                    // 花牌
	BYTE ucHandTiles[18];		            // 玩家的手牌，只有自己这边的人的
    BYTE ucBlockTiles[4][20];	            // 玩家的拦牌数据(数据连续存放)
	BYTE ucBlockTilesFlag[4][5];	        // 玩家拦牌的类型(吃碰杠标志)
	BYTE ucBlockTilesWhich[4][5];           // 玩家拦的谁的牌，或是拦的哪一张
	BYTE ucGiveTiles[4][36];	            // 玩家的出牌
	BYTE ucGodTilesLength;			        // 万能牌的长度
	BYTE ucGodTiles[8];			            // 万能牌
	BYTE ucFlowerNbr[4];			        // 玩家的花牌数量
	BYTE ucTingType[4];			            // 玩家听牌的类型
    BYTE ucReady[4];                        // 准备状态

    BOOL bGiveBlockTiles[4][36];            // 玩家出牌被拦牌了的牌
    BOOL bGiveTingTiles[4][36];             // 玩家出牌是听后打出的牌

    BYTE ucTingTiles[20];                   // 玩家听后可以胡的牌
	BYTE ucTrusteeFlag[4];                  // 拖管标志

	BYTE ucDarwTileLength;                  // 已摸牌数量
	BYTE ucGangTileLength;                  // 已杠牌数量

	
	//int ucTicket;				    // 门票
	//int nVoicePrice;				// 语音道具的
	//int nBuyItemMoney;			// 购买道具的底线

	//BYTE ucGangScore;				// 杠牌的积分
	//BYTE ucMinFan;				// 最小番
	//BYTE ucBaseScore;				// 底分
	//BYTE ucMoneyType;				// 扣费类型

	//BYTE ucTilesNumber;			// 牌的总长
	//BYTE ucTilesHandNbr;			// 手牌长

	//BYTE ucBlockLevelCollect;		// 吃牌等级
	//BYTE ucBlockLevelTriplet;		// 碰牌等级
	//BYTE ucBlockLevelQuadruplet;	// 杠牌等级
	//BYTE ucBlockLevelTing;        // 听牌等级
	//BYTE ucBlockLevelWin;			// 胡牌等级

	//BYTE ucTimerGive;				// 出牌时间
	//BYTE ucTimerFlower;			// 补花时间
	//BYTE ucTimerBlock;			// 拦牌时间
	//BYTE ucTimerReady;			// 准备倒计时

	//BYTE ucLouHu;					// 漏胡
	//BYTE ucCollateWind;			// 风顺
	//BYTE ucAllCanCollate;			// 全吃
	//BYTE ucShowTing;				// 听牌提示
	//BYTE ucTingCanGang;			// 杠牌
	//BYTE ucQiangGang;	
	//BYTE ucMoreWin;	
	//BYTE ucDrawGangBack;
	//BYTE ucCanWinLaizi;
	//BYTE ucCanBlockLaizi;

	//BYTE ucCheck7dui;
	//BYTE ucCheckQuanbukao;
	//BYTE ucCheck131;
	//BYTE ucCheckZuhelong;
	//BYTE ucCheckLualv;
	//BYTE ucCHeckLuafeng;

	TCMD_NOTIFY_VIEW_INFO()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_VIEW_INFO));
		cCmdPara = SC_MJ_NOTIFY_VIEW_INFO;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_VIEW_INFO);
    }
	
};

/**
 * @ingroup MJFrame
 * @brief 通知修改旁观状态
 */	
struct TCMD_NOTIFY_CHANGE_LOOKON: public TCMD_HEADER
{
	BYTE ucLookOn;						// 是否允许旁观
	BYTE ucHandTilesLength;			    // 手牌的长度
	BYTE ucHandTiles[MAX_HAND_TILES];	// 手牌的数据
	BYTE ucQuadrupletConcealedNbr;		// 暗杠的数量
	BYTE ucQuadrupletConcealedTiles[MAX_SET_TILES];	// 暗杠的数据

    TCMD_NOTIFY_CHANGE_LOOKON()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_CHANGE_LOOKON));
		cCmdPara = SC_MJ_NOTIFY_CHANGE_LOOKON;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_CHANGE_LOOKON);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知游戏相关基本信息
 */	
struct TCMD_NOTIFY_MJ_BASE_INFO: public TCMD_HEADER
{
    char ucTimerGive;			   // 出牌时间
    char ucTimerFlower;		       // 补花时间
    char ucTimerBlock;		       // 拦牌时间
    char ucTimerReady;		       // 准备倒计时

    char ucTimerCutCard;           // 切牌操作时间
    char ucTimerDice;              // 骰子操作时间
    char ucTimerDeal;              // 发牌操作时间
    char ucTimerJP;                // 精牌操作时间

    int  nOpenGM;                  // 是否打开了测试通道

    char ucOpenLookTile;           // 是否看牌(游戏结束后, 是否显示未胡牌的玩家的牌)
    char ucTimerLookTile;          // 推完牌后隔多长时间进行洗牌

    char ucGiveBlockShow;          // 是否显示出牌区域中被吃碰杠的牌

	int  ucTilesNumber;		       // 牌的总长
	char ucTilesHandNbr;		   // 手牌长

	BYTE ucGangScore;			   // 杠牌的积分
	BYTE ucMinFan;			       // 最小番
	BYTE ucBaseScore;			   // 底分
	BYTE ucMoneyType;			   // 扣费类型

    /*
    int  ucTicket;			       // 门票
    int  nVoicePrice;			   // 语音道具的
    int  nBuyItemMoney;

	BYTE ucBlockLevelCollect;	   // 吃牌等级
	BYTE ucBlockLevelTriplet;	   // 碰牌等级
	BYTE ucBlockLevelQuadruplet;   // 杠牌等级
	BYTE ucBlockLevelTing;         // 听牌等级
	BYTE ucBlockLevelWin;		   // 胡牌等级

	BYTE ucTimerGive;			   // 出牌时间
	BYTE ucTimerFlower;		       // 补花时间
	BYTE ucTimerBlock;		       // 拦牌时间
	BYTE ucTimerReady;		       // 准备倒计时

	BYTE ucLouHu;				   // 漏胡
	BYTE ucCollateWind;		       // 风牌可吃
    BYTE ucCollateArrow;           // 箭头牌可吃
	BYTE ucAllCanCollate;		   // 全吃
	BYTE ucShowTing;			   // 听牌提示
	BYTE ucTingCanGang;		       // 杠牌
    BYTE ucDongNanXiBeiGang;       // 东南西北杠
    BYTE ucZhongFaBaiGang;         // 中发白X杠
	BYTE ucQiangGang;	
	BYTE ucMoreWin;
	BYTE ucDrawGangBack;
	BYTE ucCanWinLaizi;
	BYTE ucCanBlockLaizi;

	BYTE ucCheck7dui;
	BYTE ucCheckQuanbukao;
	BYTE ucCheck131;
	BYTE ucCheckZuhelong;
	BYTE ucCheckLualv;
	BYTE ucCHeckLuafeng;
    */

	TCMD_NOTIFY_MJ_BASE_INFO()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_MJ_BASE_INFO));
		cCmdPara = SC_MJ_NOTIFY_BASE_INFO;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_MJ_BASE_INFO);
    }
	
};

/**
 * @ingroup MJFrame
 * @brief 通知补花结束,进入下个状态
 */	
struct TCMD_NOTIFY_FLOWER_OVER: public TCMD_NOTIFY_BLOCK/*TCMD_HEADER*/
{
	BYTE ucInfo[4];
	TCMD_NOTIFY_FLOWER_OVER()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_FLOWER_OVER));
		cCmdPara = SC_MJ_NOTIFY_FLOWER_OVER;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_FLOWER_OVER);
    }
};

/**
 * @ingroup MJFrame
 * @brief 通知显示消息对话框
 */	
struct TCMD_MJ_NOTIFY_SHOWDLG: public TCMD_HEADER 
{
	int  nMsgID;		// 消息ID
	BYTE ucFlag;		// 提示对话框的类型
	char szMsg[128];	// 玩家手中的牌

    TCMD_MJ_NOTIFY_SHOWDLG()
    {
        memset(this, 0, sizeof(TCMD_MJ_NOTIFY_SHOWDLG));
		cCmdPara = SC_MJ_NOTIFY_SHOWDLG;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_MJ_NOTIFY_SHOWDLG);
    }
};

/**
 * @ingroup MJFrame
 * @brief 自动化测试消息
 */	
struct TCMD_REQUEST_CHECK_WIN: public TCMD_HEADER 
{
	BYTE	byChair;			// 检查谁的
	BYTE	byTurn;				// 轮到谁，如果是点炮，则是点炮的那个人
	BYTE	tHand[4][MAX_HAND_TILES];		// 四家手上的牌
	BYTE	byHandCount[4];		// 手上有几张牌

	BYTE	tSet[4][MAX_SET_TILES][3];		// 四家，4手牌，flag、tile、chair
	BYTE	bySetCount[4];		// set有几手牌

	BYTE	tGive[4][40];		// 四家出过的牌
	BYTE	byGiveCount[4];		// 每人出了几张牌

	BYTE	tLast;				// 最后和的那张牌
	BYTE	tGodTile;			// 万能牌
	BYTE	byFlag;				// 0自摸、1点炮、2杠上花、3抢杠
	BYTE    ucDealer;			// 庄家
	BYTE    ucDice[DICE_NBR];	// 色子
	BYTE	byRoundWind;		// 圈风
	BYTE	byPlayerWind;		// 门风
	BYTE	byTilesLeft;		// 还剩多少张牌，用来计算海底等
	BYTE    byCollectWind;		// 字牌也可以吃

	BYTE	byFlowerCount[4];	// 4家各有多少张花	
	BYTE	byTing[4];			// 听牌的玩家
	BYTE	byOtherInfo[4];		// 其他消息
	BYTE    byOtherInfo2[4];	// 备用消息

    TCMD_REQUEST_CHECK_WIN()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_CHECK_WIN));
		cCmdPara = CS_MJ_REQUEST_CHECK_WIN;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_CHECK_WIN);
    }
};


struct TCMD_NOTIFY_CHECK_WIN: public TCMD_HEADER 
{
	BYTE    byRes;			    // 判断的结果
	BYTE	byFlag;			    // 状态，荒牌、点炮、自摸
	BYTE	byFan[4][128];		// 胡了哪些番
	BYTE	byFanNumber[4][128];// 有几个，比如补花那种
	BYTE	byWhoWin[4];		// 谁和的(要支持一炮多响)
	BYTE	byWhoGun;			// 谁点炮的
	BYTE	tLast;			    // 和的那一张
	int		nScore[4];
	int		nMoney[4];

	BYTE    tile[4][MAX_HAND_TILES];	// 四个玩家手上的牌
	BYTE	tSet[4][MAX_SET_TILES][3];	// 四个玩家的拦牌

	BYTE    byOtherData[4];		// 备用数据
	BYTE    byOtherData2[4];	// 备用数据
    TCMD_NOTIFY_CHECK_WIN()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_CHECK_WIN));
		cCmdPara = SC_MJ_NOTIFY_CHECK_WIN;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_CHECK_WIN);
    }
};

struct TCMD_REQUEST_GMCMD: public TCMD_HEADER
{
	wchar_t strGMCmd[128];   // C# char类型为2个字节(Unicode类型), 所以C++服务器定义为wchar_t
    TCMD_REQUEST_GMCMD()
    {
        memset(this, 0, sizeof(TCMD_REQUEST_GMCMD));
		cCmdPara = CS_MJ_REQUEST_GMCMD;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_REQUEST_GMCMD);
    }
};

struct TCMD_NOTIFY_GMCMD: public TCMD_HEADER
{
	BYTE ucChair;
	BYTE ucGMCmd;	// SO通知给客户端的部分命令
	char strGMResult[256];
    TCMD_NOTIFY_GMCMD()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_GMCMD));
		cCmdPara = SC_MJ_NOTIFY_GMCMD;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_GMCMD);
    }
};

struct TCMD_NOTIFY_RUN_NBR: public TCMD_HEADER
{
	int nRunNbr;		// 连局数
	int nRunWinNbr;		// 连胡数
	int nRunNotileNbr;	// 连荒数
	
    TCMD_NOTIFY_RUN_NBR()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_RUN_NBR));
		cCmdPara = SC_MJ_NOTIFY_RUN_NBR;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_RUN_NBR);
    }
};

// 强制刷新客户端数据
struct TCMD_NOTIFY_RESET_DATA: public TCMD_HEADER
{
	BYTE ucDataIndex;
	int  nResetData;
    TCMD_NOTIFY_RESET_DATA()
    {
        memset(this, 0, sizeof(TCMD_NOTIFY_RESET_DATA));
		cCmdPara = SC_MJ_NOTIFY_RESET_DATA;
        cCmdID = MJFRAME_MOUDLE_MSG;
        nDataLen = sizeof(TCMD_NOTIFY_RESET_DATA);
    }
};

#pragma pack()

}

#endif

