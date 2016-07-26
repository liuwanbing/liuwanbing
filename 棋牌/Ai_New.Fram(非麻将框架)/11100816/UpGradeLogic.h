/**
* 游戏逻辑类
*
* @author duanxiaohui
* @version 1.0
* @date 2009.12.15
*/

#pragma once
#ifndef EZP_GAMELOGIC_H
#define EZP_GAMELOGIC_H

#include "GameMessage.h"

/// 比牌返回值
enum BIPAI_RET
{
	BIPAI_STATION_ERROR = -1,   /**< 比牌信息错误 */
	BIPAI_FIRST_WIN = 1,		/**< 第一个人的牌大 */
	BIPAI_SECONED_WIN = 2,      /**< 第二个人的牌大 */
	BIPAI_SAME =0				/**< 两个人牌相等 */
};

/// 单牌牌型
enum PAI_XING
{

	PAI_ERROR = 0,              /**< 没有牌型 */

	PAI_TIANJIUWANG = 4,        /**< 天九王 */
	PAI_TIANGANG = 3 ,		    /**< 天杠 */
	PAI_DIGANG = 2,             /**< 地杠 */
	PAI_TIANPAIJIU = 1          /**< 天牌九 */
};

/// 对子牌型，从大到小
enum PAI_DOUBLE_NAME
{
	PAI_KIND_HUANGDI = 12,       /**< 皇帝 */
	PAI_KIND_DUITIAN = 11,       /**< 对天 */
	PAI_KIND_DUIDI = 10,         /**< 对地 */
	PAI_KIND_HUANGDI_EX = 9,     /**< 扩展皇帝(针对72把为一局的版本) */
	PAI_KIND_DUIREN = 8,	     /**< 对人 */
	PAI_KIND_DUIHE = 7,		     /**< 对合 */

	PAI_KIND_DUIZI_1 = 6,        /**< 对黑四、对黑六、对黑十 */
	PAI_KIND_DUIZI_2 = 5,        /**< 对红六、对红七、对红十、对黑J */
	PAI_KIND_DUIZI_3 = 4,        /**< 对红五、对黑七、对黑八、对红九 */

	PAI_KIND_Q9 =  3,            /**< Q + 9组合 *///天九王
	PAI_KIND_Q8 =  2,            /**< Q + 8组合 *///天罡
	PAI_KIND_28 =  1,            /**< 2 + 8组合 *///地罡

	PAI_DUIZI_ERROR = 0          /**< 不是牌型 */
};								

/// 单牌牌型名称
static char * g_PaiXing_Name[5] =
{
	TEXT("没牌型"), TEXT("天牌九"), TEXT("地杠"), TEXT("天杠"), TEXT("天九王")
};

/// 游戏逻辑类
class GameLogic
{
public:
	BYTE            m_cardAry[PAI_COUNT];            /**< 游戏中的牌数据 */
	PaiData         m_userData[4];                   /**< 每个玩家的牌,每个元素对应1个位置 */
	PaiData         m_GameCard[GAME_XIPAI_COUNT][4]; /**< 保存72把牌数据 */

protected:
	BYTE            m_bGameVersion;                  /**< 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代*/
	BOOL            m_bKingAndThreeBig;              /**< 区分大王、黑3 与 双红Q 大小: TRUE: 大王与3最大 FALSE: 双红Q最大*/
	BOOL            m_bUseCard;                      /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
	BOOL            m_bRedJOrBlackJ;                 /**< 红J还是黑J FALSE: 黑J TRUE: 红J */
public:
	/// 构造函数
	GameLogic(void);

	/// 析构函数
	~GameLogic(void);

	/// 初始化数据
	void InitGame();

	/// 逆时针方向算出自己的下一家
	static BYTE NiNextStation(BYTE  station)
	{
		return (station + 3) % 4;
	}

	/// 逆时针方向,根据庄家位置算出指定的下num 家位置
	static BYTE NiNextNumStation(BYTE zhuangStation,BYTE num)
	{
		if (num == 1)
		{
			return NiNextStation(zhuangStation);
		}

		BYTE retStation = zhuangStation ;
		for(int i = 1;i <= num -1;i++)
		{
			retStation = NiNextStation(retStation);
		}
		return retStation;
	}

	/// 顺时针方向下一家
	static BYTE ShunNextStation(BYTE station)
	{
		return (station + 1) / 4;
	}

public:
	/// 获取牌花色
	inline BYTE     GetShape(BYTE bValue) { return (bValue&0xF0)>>4; }

	/// 获取牌点    
	inline BYTE     GetNumber(BYTE bValue) { return (bValue&0x0F)-1; }

	/// 设置游戏模式
	void            SetGameMode(BYTE bGameVersion, BOOL bUseCard, BOOL bKingAndThreeBig = TRUE,BOOL bRedJOrBlackJ = TRUE);

	/// 洗牌,洗完后会自动发牌
	void            GameXiPai(BOOL bRecordCard = FALSE);

	/// 随机一张牌扑克
	BYTE			RandACard();

	/// 将十进制牌数据转换成16进制数据, 返回十六进制值
	BYTE			GetValue(BYTE bValue);

	/// 设置某个玩家手里的牌
	BOOL            SetUserCard(BYTE userStation,BYTE firstCard,BYTE secondCard);

	/// 设置某个玩家手里的牌
	BOOL            SetUserCard(BYTE userStation,PaiData & paiData);

	/// 检测用户的牌是否是对子
	BOOL            IsCardDouble(const PaiData & paiData);

	/// 判断组合牌
	BOOL            IsCompagasCard(const PaiData & paiData);

	/// 得到用户手中的牌对子的大小值, 若牌不是对子，则返回 PAI_DUIZI_ERROR
	PAI_DOUBLE_NAME GetCardDouble(const PaiData & paiData, BYTE bGameMode = GAME_VERSION_1);

	/// 获得二张牌中最大的一张单牌或最大的单牌位置
	int             GetSingleCardMax(const PaiData & paiData, BOOL bSeat = FALSE);

	/// 计算用户牌点数
	int             CalcCardValue(const PaiData & paiData);

	/// 获取牌的点数
	int             GetCardValue(const BYTE card);

	/// 获取牌的牌型名称
	CString         GetPaiValueName(PaiData & paiData);

	/// 获取牌的牌型名称
	CString         GetPaiValueName(BYTE staion);

	/// 获取玩家手中牌的牌型
	PAI_XING        GetCardPaiXing(const PaiData & paiData);

	/// 获取对子牌的名称
	char *          GetDuiZiName(const PAI_DOUBLE_NAME paiDuiZi);
	/// 获取牌型名称
	char *          GetPaiXingName(const PAI_XING & paiXing);

	/// 比较两个玩家的牌大小
	BIPAI_RET       BiPaiFromStation(const BYTE station1, const BYTE station2);

protected:
	/// 检查牌值是否合法
	BOOL            IsPaiValue(const PaiData & paiData);

	/// 发牌
	void            GameFaPai();

	/// 比较两个玩家的牌大小
	BIPAI_RET       BiPaiFromData(const PaiData & paiData1,const PaiData & paiData2);

	/// 比较两个人单牌的大小
	BIPAI_RET       DanPaiBiPai(const PaiData & paiData1,const PaiData & paiData2, BYTE bGameMode = GAME_VERSION_1);
};

#endif