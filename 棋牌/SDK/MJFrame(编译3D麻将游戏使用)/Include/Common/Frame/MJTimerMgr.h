/** @file	MJTimerMgr.h
 *	@brief	时间管理类
 *	@date 08.08.04
 */
#ifndef ___AFX_MJTIMERMGR_H__E6EAD12F_9702_428F_8B64_0D7CE606EC71__INCLUDED___
#define ___AFX_MJTIMERMGR_H__E6EAD12F_9702_428F_8B64_0D7CE606EC71__INCLUDED___

#include "MJFrameBaseData.h"
#include "gameeventimpl_t.h"

#ifdef WINDOWS_LOGIC
#include "gameprocess_i.h"
#endif

#include "TimerCall_i.h"

namespace MyGame
{

#ifndef WINDOWS_LOGIC
/**
 * @ingroup MJFrame
 * @brief CSoMJTimerMgr
 *
 * <p>　 
 *		CSoMJTimerMgr, SO时间管理类
 * </p>
 * @sa
 */	
class CSoMJTimerMgr :public ISoTimerCall
{
public:
	CSoMJTimerMgr();
	virtual ~CSoMJTimerMgr();

    virtual int OnSoTimer(int cuBeginTime);

	/**
	 *  @brief 设置时间
	 *  @param[in]  nTimerID 时间的ID
	 *  @param[in]  nTime 时间的时长
	 *  
	 *  SO始终只保持一个时间ID
	 */	
	void SetTimer(int nTimerID, int nTime);

	/**
	 *  @brief 停止时间
	 *
	 *  时间并没有真正地停止，只是ID被设置为0
	 */	
	void KillTimer();

    /**
    *  @brief 获得当前正在运行的定时器的ID
    */
    int GetTimerID();

    /**
    *  @brief 获得当前正在运行的定时器的剩余时间， 此功能暂时没有实现
    */
    int GetRemainTime();

protected:
	/**
	 *  @brief 时间到了后的事件处理
	 */	
	virtual void OnTimeOut( int nTimerID );

    UINT  m_nTimerID;
    int   m_cuTimerRemain;
    int   m_SetRemain;
};

#else
/**
 * @ingroup MJFrame
 * @brief 时间ID的映射
 *
 *  因为要支持动画控制之类的时间，所以不像SO那么单一时间，但也有个ID里面映射逻辑TIMER和SO一样
 */	
typedef struct tagMJTIMER_ID_MAP
{
	int dwTimerID;	// 定义的ID
	int nTime;	// 内部ID
}MJTIMER_ID_MAP;

/**
 * @ingroup MJFrame
 * @brief CGameMJTimerMgr
 *
 * <p>　 
 *		CGameMJTimerMgr, 客户端时间管理类
 * </p>
 * @sa
 */	
class SERAPI CGameMJTimerMgr
{   
public:
	CGameMJTimerMgr();
	virtual ~CGameMJTimerMgr();

    void Initialize(IGameProcess* pProcesss);
	/**
	 *  @brief 设置游戏时间
	 *  @param[in]  dwTimerID 时间的ID，内部定时器ID，
	 *  @param[in]  nElapse 时间的间隔,单位为毫秒
	 */	
	void SetGameTimer(UINT dwTimerID,UINT nElapse); // 单位为毫秒

	/**
	 *  @brief 停止时间
	 *  @param[in]  dwTimerID 时间的ID
	 */	
	void KillGameTimer(UINT dwTimerID);  

	/**
	 *  @brief 时间响应的事件处理,外部使用， 内部设置了定时器，内部时间到了，调用它，则使外部得到响应。请下层时，要调用之
	 *  @param[in]  dwGameTimerID 时间的ID
	 *
	 *  外部直接拿这个处理
	 */	
	virtual void OnGameTimer(UINT dwGameTimerID);

	/**
	 *  @brief 根据ID画时间
	 *  @param[in]  nTime 显示的时间
	 *  @param[in]  bVisible 是否显示
	 *  @param[in]  bStart 是否开始计时
	 *  @param[in]  bInCrease 是否是增序显示时间
	 *
	 *  此函数需要外部重载实现, 这个方法是由内部调用的，外部实现，比如内部开始显示出牌等待时间，
     *  在界面上就要显示出，
	 */	
	virtual void StartClock(int nTime, BOOL bVisible = TRUE, BOOL bStart = TRUE, BOOL bInCrease = FALSE) {};

	/**
	 *  @brief 设置逻辑时间
	 *  @param[in]  nLogicTimerID 逻辑时间ID
	 *  @param[in]  nTimer 逻辑时间时长
	 */	
	void SetLogicTimer( int nLogicTimerID, int nTimer );

	/**
	 *  @brief 停止逻辑时间
	 *  @param[in]  bVisible 是否要显示时间
	 */	
	void KillLogicTImer( BOOL bVisible = TRUE );

	/**
	 *  @brief 刷新逻辑时间
	 * 
	 *  如果时间结束,会执行TimeOut
	 */	
	virtual void RefreshLogicTimer();

	/**
	 *  @brief 逻辑时间时间到后执行的事件
	 *  @param[in]  nLogicTimerID 结束的时间ID，
     *  此方法，也是给外部实现的，如发牌等待时间已到，则调用它，外部实现逻加的转换
	 */	
	virtual void OnLogicTimeOut( int nLogicTimerID ){};

	/**
	 *  @brief 清空全部的时间
	 */	
	void ClearAllTimer();

protected:
	MJTIMER_ID_MAP m_tagIDMap[MJ_TIMERID_MAP_NBR];
	int  m_nLogicTimerID;	// 逻辑时间ID
	int  m_nLogicGameTimer;	// 逻辑时长 TimeOut使用
	IGameProcess*  m_TimerMgrProcesss;                     //此变量一定要在初始时把它附值。
};
#endif

}

// #include "MJTimerMgr.inl"

#endif // !defined(AFX_MJTIMERMGR_H__E6EAD12F_9702_428F_8B64_0D7CE606EC71__INCLUDED_)
