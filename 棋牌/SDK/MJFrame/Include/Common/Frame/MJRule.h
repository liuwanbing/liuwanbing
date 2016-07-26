/** @file	MJRule.h
 *	@brief	麻将的规则类
 *	@author duanxiaohui
 *	@date 10.07.15
 *
 *  算番用的函数，包含了大多数现有番型，并可以通过自由组合而形成新的番型
 */
#ifndef AFX_MJRULE_H__F30D5373_AF06_4AAD_BA58_89EB2AA5C183__INCLUDED_
#define AFX_MJRULE_H__F30D5373_AF06_4AAD_BA58_89EB2AA5C183__INCLUDED_

#include "MJ.h"
#include "MJFanCounter.h"

namespace MyGame
{
	/**
	* @ingroup MJFrame
	* @brief CMJRule
	*
	* <p>　 
	*		CMJRule, 麻将的规则类
	* </p>
	* @sa
	*/	
	class /*SERAPI*/ CMJRule  
	{
	public:
		CMJRule();
		virtual ~CMJRule();

		/**
		*  @brief 判断能不能满足胡牌的牌型
		*  @param[in]  mjTilesNoGod 去掉了万能牌的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  nLaiziCount 万能牌数量
		*  @return  如果可以胡返回TRUE,否则返回FALSE
		*
		*  不考虑番
		*/	
		virtual BOOL CheckWin( CMJHand mjTilesNoGod, CMJSet mjSet, int nLaiziCount = 0 );

		/**
		*  @brief 是否能胡
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  nLaiziAdd 附加的万能牌数量
		*  @param[in]  bCheckGod 是否检测万能牌
		*  @return  如果可以胡返回TRUE,否则返回FALSE
		*
		*  考虑番的数量,是判断胡牌的中心函数
		*/	
		virtual BOOL IsWin( CMJHand mjTiles, CMJSet mjSet, int nLaiziAdd = 0, int bCheckGod = TRUE );

		/**
		*  @brief 是否能听
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[out]  mjGive 如果能听，返回听牌可以被打出去的那几张
		*  @return  如果可以听返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsTing( CMJHand mjTiles, CMJSet mjSet, CMJHand& mjGive );

		/**
		*  @brief 是否能听,并获取能听什么
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[out]  mjTing 如果能听，返回可以胡的牌
		*  @return  如果可以听返回TRUE,否则返回FALSE
		*
		*  和IsTing的区别在于，IsTing中mjTiles是包含有1张不能胡的牌。而这里的mjTiles是已经听牌了的牌组
		*/	
		virtual BOOL GetTingTiles( CMJHand mjTiles, CMJSet mjSet, CMJHand& mjTing );

		/**
		*  @brief 是否能胡指定牌
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  ucTile 指定的牌
		*  @return  如果可以胡返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsCanWin( CMJHand mjTiles, CMJSet mjSet, TILE ucTile );

		/**
		*  @brief 是否能吃牌
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  ucTile 指定要吃的牌
		*  @param[in]  nChairGive 出牌的玩家
		*  @param[in]  nChairCheck 判断吃牌的玩家
		*  @param[in]  nPlayerNbr 总的游戏人数(用来判断吃的是不是上家)
		*  @return  如果可以吃返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsCanCollect( CMJHand& mjTiles, TILE ucTile, int nChairGive, int nChairCheck, int nPlayerNbr = 4 );

		/**
		*  @brief 是否能碰牌
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  ucTile 指定要碰的牌
		*  @return  如果可以碰返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsCanTriplet( CMJHand& mjTiles, TILE ucTile );

		/**
		*  @brief 能否杠指定的牌
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  ucTile 指定要杠的牌
		*  @return  如果可以杠返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsCanQuadruplet( CMJHand& mjTiles, TILE ucTile );

		/**
		*  @brief 手中是否有牌可杠
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @return  如果可以杠返回TRUE,否则返回FALSE
		*/	
		virtual BOOL IsCanQuadrupletSelf( CMJHand& mjTiles, CMJSet &mjSet );

		/**
		*  @brief 获取多少番
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  env 除了牌的环境信息
		*  @return 返回番的数目
		*/	
		virtual int GetFanCount( CMJHand mjTiles, CMJSet mjSet, ENVIRONMENT env );

		/**
		*  @brief 能否普通的胡
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @param[in]  bNoJiang 没有将牌
		*  @return 如果可以普通胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinNormal( CMJHand mjTiles, int nLaiziCount, int bNoJiang = 0 );

		/**
		*  @brief 能否7对
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinAllPair( CMJHand mjTiles, int nLaiziCount );

		/**
		*  @brief 能否胡十三幺
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWin131( CMJHand mjTiles, int nLaiziCount );

		/**
		*  @brief 能否胡乱绿
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet   玩家的拦牌数据
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinAllGreen( CMJHand mjTiles, CMJSet mjSet );

		/**
		*  @brief 能否胡乱风
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet   玩家的拦牌数据
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinAllWind( CMJHand mjTiles, CMJSet mjSet );

		/**
		*  @brief 能否胡组合龙
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinZuhelong( CMJHand mjTiles, int nLaiziCount );

		/**
		*  @brief 能否胡全不靠
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @return 如果可以胡牌,返回TRUE,否则返回FALSE
		*/	
		virtual BOOL CheckWinQuanbukao( CMJHand mjTiles, int nLaiziCount );

		/**
		*  @brief 设置算番指针
		*  @param[in]  pFanCount 番对象的指针
		*/	
		virtual void SetFanCounter( CMJFanCounter* pFanCount );

		/**
		*  @brief 设置万能牌
		*  @return 返回0
		*/	
		virtual int SetGodTiles();

		/**
		*  @brief 清空万能牌信息
		*/	
		virtual void ClearGodTiles();

		/**
		*  @brief 把拦牌也加入到手牌中去
		*  @param[out]  mjAll 整合后的手牌数据
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[in]  mjSet 玩家的拦牌
		*/	
		virtual void GetAllTiles( CMJHand &mjAll, CMJHand mjTiles, CMJSet mjSet );

		/**
		*  @brief 获取万能牌
		*  @return 返回万能牌的牌组
		*/	
		CMJHand GetTilesGod() { return m_TilesGod; }

		/**
		*  @brief 获取一个取掉了万能牌的牌组
		*  @param[in]  mjTiles 玩家的手牌
		*  @param[out]  mjTilesNoGod 去掉万能牌后的牌组
		*  @param[out]  mjTilesGod 去掉万能牌剩下的牌组
		*  @return 返回万能牌的数量
		*/	
		int GetTilesNoGod( CMJHand& mjTiles, CMJHand& mjTilesNoGod, CMJHand& mjTilesGod );

		/**
		*  @brief 获取最小要求番
		*  @return 返回最小要求的番
		*/	
		virtual int GetMinFan() { return m_tagMJSetting.nMinFan; }

		/**
		*  @brief 把手上牌的万能牌添成实际的牌
		*  @param[out]  mjTilesNoGod 去掉万能牌后的牌组
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @param[out]  mjLaizi 替换后的牌
		*  @param[in]  mjLaiziTiles 万能牌本身的牌
		*  @return 返回最大的计算番数
		*/	
		virtual int ConfigGodTiles( CMJHand &mjTilesNoGod, CMJSet mjSet, int nLaiziCount, CMJHand &mjLaizi, CMJHand mjLaiziTiles );

		/**
		*  @brief 把手上牌的万能牌添成实际的牌
		*  @param[out]  mjTilesNoGod 去掉万能牌后的牌组
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @param[out]  mjLaizi 替换后的牌
		*  @param[in]  mjLaiziTiles 万能牌本身的牌
		*  @return 返回一个满足条件的值
		*
		*  函数本身不会去遍历找出最大番，只要满足条件的番就可以了
		*/	
		virtual int ConfigGodTilesFast( CMJHand &mjTilesNoGod, CMJSet mjSet, int nLaiziCount, CMJHand &mjLaizi, CMJHand mjLaiziTiles );

		/**
		*  @brief 把手上牌的万能牌添成实际的牌
		*  @param[in]  mjTilesNoGod 去掉万能牌后的牌组
		*  @param[out]  mjTileoGod 去掉的万能牌牌组，返回他们被替换回的牌
		*  @param[in]  mjSet 玩家的拦牌
		*  @param[in]  nLaiziCount 万能牌的数量
		*  @param[in]  bNeedFanCount 是否需要限制番型,如果不需要,直接返回1
		*  @return  如果番不满足,返回0,否则返回满足了的番的值
		*/	
		virtual int CheckNeedFan( CMJHand mjTilesNoGod, CMJHand& mjTilesGod, CMJSet mjSet, int nLaiziCount = 0, int bNeedFanCount = FALSE );

		/**
		*  @brief 设置配置文件
		*  @param[in]  mjdata 设置的数据
		*/	
		virtual void SetMJSetting( MJ_SETTING_DATA mjdata );

		/**
		*  @brief 获取配置文件
		*  @return 配置文件信息
		*/	
		virtual MJ_SETTING_DATA GetMJSetting() { return m_tagMJSetting; }

		/**
		*  @brief 设置算番环境变量
		*  @param[in]  env 环境变量结构
		*  @param[in]  nChair 要计算番的人
		*  @param[in]  bSetChairTile 要设置番的人的牌
		*/	
		virtual void SetFanEvn( ENVIRONMENT &env, int nChair, int bSetChairTile = TRUE );

		/**
		*  @brief 获取万能牌所替代的牌
		*  @return 返回替代的牌
		*/	
		virtual CMJHand GetTilesGodTrans() { return m_TilesGodTrans; }

		/**
		*  @brief 获取吃牌的种类
		*  @return 返回结果种类 100 次吃头 10 吃中间 1吃后面，0什么都不能吃
		*/	
		virtual int GetCollectRes( CMJHand mjHand, TILE ucTile, int nChair, int nGiveChair, int bWind = FALSE );

		/**
		*  @brief 获取指定牌的类型
		*  @param[in]  ucTile 牌数据
		*  @return  万:0，筒:1，条:2，字:3，花:4，其他:5
		*
		*  返回值有宏定义可参考
		*/	
		virtual int GetTileGenre( TILE ucTile );

	public:
		// 算番指针
		CMJFanCounter* m_pFanCount;

	protected:
		// 万能牌序列，一般只有一个
		// 在判断胡牌的时候，万能牌可以充当任何牌型
		CMJHand m_TilesGod;
		// 万能牌所替换的牌，在每次CONFIG后都会被刷新
		CMJHand m_TilesGodTrans;
        // 拦牌操作, 可以操作的吃碰杠数据
        CMJSet  m_tmpBlockSet;
        // 拦牌操作, 听牌后可以打出的牌数据
        CMJHand m_tmpTingGive;


		/************************************************************************/
		/* 
		游戏的配置信息
		*/
		/************************************************************************/
		MJ_SETTING_DATA m_tagMJSetting;

	};

}

#endif // !defined(AFX_MJRULE_H__F30D5373_AF06_4AAD_BA58_89EB2AA5C183__INCLUDED_)
