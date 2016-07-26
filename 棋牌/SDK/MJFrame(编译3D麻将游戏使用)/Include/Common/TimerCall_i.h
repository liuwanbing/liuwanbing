#ifndef __TIMER_CALL_BACK_I_H__
#define __TIMER_CALL_BACK_I_H__
#include "typeDef.h"
#include "gameevent_i.h"

namespace MyGame
{
    class IWinTimerEvent;

	class ISoTimerCall
	{
	public:
		virtual int  OnSoTimer(int cuBeginTime) = 0;
	};


	class IWinTimer 
	{
	public:
		/**
		 *  @brief 设置参数定时器并开始计时
		 *  @param dwMilliSeconds 定时器间隔时间（毫秒）
		 *  @param dwParam 定时器事件回调时返回的参数，由用户给定
		 *	@param pEvent 定时器监听者指针
		 *  @return 返回定时器ID，在本定时器对象中唯一。
		 */		
		virtual UINT SetTimer(UINT dwMilliSeconds, UINT dwParam, IWinTimerEvent* pEvent) = 0;

		/**
		 *  @brief 给某个ID的定时器设置新的间隔时间,定时器将在下一个间隔期计算中改用新的间隔时间
		 *  @param dwTimerID 定时器ID
		 *  @param dwMilliSeconds 新的间隔时间（毫秒）
		 *  @return 成功则返回TRUE，失败返回FALSE，失败的原因通常是该定时器ID不存在
		 */	
		virtual BOOL SetTimer(UINT  dwTimerID, UINT  dwMilliSeconds) = 0;

		/**
		 *  @brief 停止定时器
		 *  @param dwTimerID 定时器ID，由 SetTimer() 函数返回
		 */		
		virtual void KillTimer(UINT  dwTimerID) = 0;

		/**
		 *  @brief 清除本对象中的所有定时器
		 *  @sa 
		 */	
		virtual void ClearAll() = 0;
	};

#ifdef WINDOWS_LOGIC

	/**
	 * @ingroup system
	 * @brief WinTimer事件接口
	 *
	 * <p>
	 *　　 
	 * </p>
	 *
	 * @sa 
	 */	
	class IWinTimerEvent : public IEvent
	{
	public:
		/**
		 *  @brief 定时器事件回调
		 *  @param dwTimerID,定时器ID，由 IWinTimer::SetTimer() 函数返回
		 *  @param dwParam 定时器参数，由 IWinTimer::SetTimer() 函数设置
		 */		
		virtual void OnTimer(DWORD dwTimerID, DWORD dwParam) = 0;
	};
#endif
}

#endif
