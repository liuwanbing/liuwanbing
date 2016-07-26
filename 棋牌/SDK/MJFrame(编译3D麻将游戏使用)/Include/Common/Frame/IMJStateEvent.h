/** @file	IMJStateEvent.h
 *	@brief	提供给让基类调用子类函数的接口部分
 *	@author edward
 *	@date 08.08.04
 *
 *  IMJStateEvent主要提供获取当前游戏中状态的函数
 */
#ifndef ___AFX_IMJSTATEEVENT_H__C459615F_7FFB_48E4_8742_44148C42F1A3__INCLUDED___
#define ___AFX_IMJSTATEEVENT_H__C459615F_7FFB_48E4_8742_44148C42F1A3__INCLUDED___

#include "typeDef.h"

namespace MyGame
{

/**
 * @ingroup MJFrame
 * @brief IMJStateEvent
 *
 * <p>　 
 *		IMJStateEvent, 状态管理的接口
 * </p>
 * @sa
 */	
class IMJStateEvent  
{
public:
	IMJStateEvent();
	virtual ~IMJStateEvent();
	

	/**
	 *  @brief 获取当前的麻将状态
	 *  @return 返回当前的麻将状态
	 *  
	 *  SO和C端的麻将状态不一定相同
	 */	
	virtual int GetMJState();

    /**
    *  @brief 设置状态
    *  @param[in]  nState    设置的状态
    *  @param[in]  bTimelag  TRUE: 延时操作  FALSE: 不延时
    *
    *  在设置了状态后会根据状态执行相关的操作
    */	
    virtual void SetMJState( int nState, BOOL bTimelag = FALSE );

};

}
#endif // !defined(AFX_IMJSTATEEVENT_H__C459615F_7FFB_48E4_8742_44148C42F1A3__INCLUDED_)
