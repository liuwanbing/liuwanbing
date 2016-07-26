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
	/// 构造函数
	GameLogic(void);

	/// 析构函数
	~GameLogic(void);
};

#endif