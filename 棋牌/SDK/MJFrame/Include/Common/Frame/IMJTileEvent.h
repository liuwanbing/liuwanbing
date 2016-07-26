/** @file	IMJTileEvent.h
 *	@brief	提供给让基类调用子类函数的接口部分
 *	@author edward
 *	@date 08.08.04
 *
 *  IMJTileEvent主要提供获取当前游戏中城墙数据的函数
 */
#if !defined(AFX_IMJTILEEVENT_H__145E99E0_1F35_496C_9AC1_8406E9074115__INCLUDED_)
#define AFX_IMJTILEEVENT_H__145E99E0_1F35_496C_9AC1_8406E9074115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typeDef.h"

namespace MyGame
{

/**
 * @ingroup MJFrame
 * @brief IMJTileEvent
 *
 * <p>　 
 *		IMJTileEvent, 城墙管理的接口
 * </p>
 * @sa
 */	
class IMJTileEvent  
{
public:
	IMJTileEvent();
	virtual ~IMJTileEvent();

	/**
	 *  @brief 获取当前的玩家
	 *  @return 返回当前的玩家
	 */	
	virtual int GetTurn();

	/**
	 *  @brief 设置当前的玩家
	 *  @param[in] nTurn 设置的玩家
	 */	
	virtual void SetTurn( int nTurn );

	/**
	 *  @brief 获取庄家
	 *  @return 返回庄家
	 */	
	virtual int GetDealer() { return 0; }

};

}
#endif // !defined(AFX_IMJTILEEVENT_H__145E99E0_1F35_496C_9AC1_8406E9074115__INCLUDED_)
