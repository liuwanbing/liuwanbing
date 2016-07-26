/**
* 消息定义类 GameMessage.h   
*
* @author duanxiaohui
* @version 1.0
* @date 2009.12.15
*/

#pragma once

#ifndef EZP_GAMEMESSAGE_H
#define EZP_GAMEMESSAGE_H

/** 玩家人数 */
#ifdef NUM_TWO
  #define  EZP_PLAY_COUNT              2
#else
  #define  EZP_PLAY_COUNT              16
#endif
  

/** 消息定义 服务器客户端消息 160 ~ 200 */
/** 玩家申请上庄 */
#define  EZP_MSG_APPLYZHUANG         160

/** 玩家下注 */
#define  EZP_MSG_XIAZHU              161

/** 发牌 */
#define  EZP_MSG_SENDCARD            163

/** 申请上庄失败 */
#define  EZP_MSG_APPLYZHUANG_ERR     164

/** 下注提示 */
#define  EZP_MSG_XIAZHU_HINT         165

///玩家下注完成
#define EZP_MSG_FINISH_NOTE          166 

/** 玩家拥有的牌数 */
#define  GAMER_CARD  2

/** 牌总张数 */
#define  PAI_COUNT   32

/** 一局洗牌次数 */
#define  GAME_XIPAI_COUNT  72

/** 牌值最小值 */
#define  PAI_VALUE_MIN      1

/** 牌值最大值 */
#define  PAI_VALUE_MAX      55

/** 上庄金币不够错误 */
#define  LESS_APPLYZHUANG_MONEY_ERR  0x01

/** 重复申请上庄错误 */
#define  REPEAT_APPLAYZHUANG_ERR     0x02

/** 申请上庄者为当前庄家 */
#define  CUR_ZHUANG_ERR              0x03

/** 筹码种数 */
#define CHOUMA_KIND_COUNT   6
/** 筹码面值 */
static const int g_ChoumaValue[CHOUMA_KIND_COUNT] = {100, 1000, 10000, 100000, 1000000, 10000000};

/** 牌值是否正确 */
#define  PAI_IS_VALUE(paiKey)  ((paiKey >= PAI_VALUE_MIN && paiKey <= PAI_VALUE_MAX) ? TRUE : FALSE)

/// 玩家手中的牌数据结构
struct PaiData
{
	BYTE   paiAry[GAMER_CARD];  /**< 牌数据 */
	PaiData()
	{
		memset(paiAry,0,sizeof(paiAry));
	}
	PaiData(PaiData & paiData)
	{
		memcpy(paiAry,paiData.paiAry,sizeof(paiData.paiAry));
	}
	PaiData(BYTE first,BYTE second)
	{
		if (PAI_IS_VALUE(first) && PAI_IS_VALUE(second))
		{
			paiAry[0] = first;
			paiAry[1] = second;
			return;
		}
		else
		{
			memset(paiAry,0,sizeof(paiAry));
			return;
		}
	}
	void operator=(PaiData & paiData)
	{
		paiAry[0] = paiData.paiAry[0];
		paiAry[1] = paiData.paiAry[1];
	}
	BOOL operator==(PaiData paiData)
	{
		if (paiAry[0] == paiData.paiAry[0] && paiAry[1] == paiData.paiAry[1])
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

/// 下注数据消息结构
struct EZP_Xiazhu_Data
{
	__int64 iXiaZhu[EZP_PLAY_COUNT][7];    /**< 0横，1横，2顺门，3倒门，4角，5天门，6角 */
	__int64 iDeskInfo[7];                  /**< 桌子上六个区域的总信息 */
	__int64 iRobotZhu[7];                  /**< 机器人每个区域下的注额 */
	__int64 iAllMoney[EZP_PLAY_COUNT];     /**< 玩家总的下注额 */
	BOOL    bChipFlag[EZP_PLAY_COUNT];     /**< 玩家下注标志 TRUE: 表示该玩家有下注 FALSE: 该玩家没有下注 */

	void Init()
	{
		memset(&iXiaZhu, 0, sizeof(iXiaZhu));
		memset(&iDeskInfo, 0, sizeof(iDeskInfo));
		memset(&iRobotZhu, 0, sizeof(iRobotZhu));
		memset(&iAllMoney, 0, sizeof(iAllMoney));
		memset(&bChipFlag, 0, sizeof(bChipFlag));
	}
};

/// 申请上庄消息结构
struct EZP_APPLY_ZHUANG
{
	BYTE bDeskStation;  /**< 申请上庄者 */
	BOOL bFirst;        /**< 是否第一个申请上庄者 */
	BOOL bFlag;         /**< TRUE: 上庄 FALSH: 下庄 */
	BOOL bGoto;         /**< 直接下庄，不检测 */
	BOOL bIsRobot;      /**< 是否为机器人叫庄 */
};

/// 申请上庄错误消息结构
struct EZP_APPLY_ZHUANG_ERR
{
	BYTE bErrorCode;   /**< 错误码 */
};

/// 下注信息消息结构
struct EZP_XIAZHU_INFO
{
	BYTE bDeskStation; /**< 下注者 */
	BYTE bType;        /**< 下注区域 */
	BYTE bMoneyType;   /**< 金币类型 */
	__int64  iMoney;   /**< 下注金币 */
	BOOL bIsRobot;     /**< 标志下注玩家是否为机器人 */		
};

/// 下注信息提示
struct EZP_XIAZHU_HINT
{
	BYTE bState;       /**< 下注提示标志 */	
};
///玩家点击下注完成
struct EZP_FINISH_NOTE
{
	BYTE bDeskStation ; 
};

/// 游戏正式开始消息结构
struct EZP_PLAY_GAME
{
	BYTE bCard[1];     /**< 牌数据 */
};

/// 发送一张牌，区分发牌位置消息结构
struct EZP_SEND_CARD
{
	BYTE      bCard;		 /**< 区分发牌开始位置的牌 */
	PaiData   userCard[4];   /**< 上下左右四叠牌 */
};

/// 开始发牌消息结构
struct GameFaPaiData
{
	BOOL     bUseCard;       /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BYTE     bDiceNum[2];   /**< 色子的点数 */
	BYTE     bStartSendPos;  /**< 开始发送位置 */
	PaiData  userCard[4];    /**< 每个方位的牌,每个元素对应 station 中的位置*/
};

/// 牌型名称结构体
typedef struct PaiValue
{
	char iValue[10];   /**< 牌值 */
	void SetValue(char * str)
	{
		strcpy(iValue,str);
	}
	char * GetValue()
	{
		return iValue;
	}
	PaiValue()
	{
		memset(iValue,0,sizeof(iValue));
	}
};

/// 游戏结束、胜负数据消息结构
struct GameEndData
{
	int        userPaiDian[4];				/**< 牌点数 */
	PaiValue   paiValue[4];					/**< 牌型名称 */
	PaiData    userData[4];                 /**< 每个玩家的牌 */

	//__int64      iUserMoney[EZP_PLAY_COUNT];                  /**< 本局输赢金币 */	
	//__int64      iReturnMoney[EZP_PLAY_COUNT];				 /**< 返回分 */
	__int64      iUserWinMoney ;                 ///玩家赢钱
	__int64      iUserReturnMoney ;              ///玩家返回的金币
	__int64      iZhuangMoney;                   /**< 本局庄家输赢金币 */
	__int64      dwMoney;                        /**< 本人金币, 客户端不能及时刷新金币，所以从服务器上发下去 */
	__int64      dwZhuangMoney;                  /**< 庄家金币, 客户端不能及时刷新金币，所以从服务器上发下去 */
	
	BYTE       bKaiPai;                       /**< 顺、对、倒门开牌 */

	BOOL       bAutoZhuangXZ;                 /**< 庄家金币不够上庄条件则下庄，自动下庄 */
	BOOL       bZhuangXZ;                     /**< 庄家自己申请下庄了或做庄次数满 */
	BOOL       bRobortCanApply;               /**< 机器人是否可以申请上庄 */
	BOOL       bForceLeave ;                   ///是否强制让此玩家离开

	GameEndData()
	{
		bKaiPai = 0;
		memset(paiValue, 0, sizeof(paiValue));
		memset(userPaiDian, 0, sizeof(userPaiDian));
		//memset(iUserMoney,0,sizeof(iUserMoney));
		//memset(iReturnMoney,0,sizeof(iReturnMoney)) ; 
	}
};

/// 每局游戏开牌、下注信息消息结构
struct GameInfoList
{
	BYTE  bKaiPai;        /**< 本局是否开(0x01:顺门、0x02:对门、0x04:倒门) */
	BYTE  bIsXiaZhu;      /**< 本局是否下注(0x01:顺门、0x02:对门、0x04:倒门) */

	GameInfoList()
	{
		bKaiPai = 0;
		bIsXiaZhu = 0;
	}
};

/// 路子信息
struct GameLuziInfo
{
	BYTE bKaiPai;         /**< 开牌信息 */
	char chGameBeen;      /**< 当前第几局 */
	char chGameCount;     /**< 当前第几把 */

	GameLuziInfo()
	{
		bKaiPai = 0;
		chGameBeen = 0;
		chGameCount = 0;
	}
};

#endif