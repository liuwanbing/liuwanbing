/** @file	MJStateMgr.h
 *	@brief	麻将状态管理类
 *	@author edward
 *	@date 08.08.04
 */
#ifndef ___AFX_MJSTATEMGR_H__52351F89_A913_425E_89A1_53CFDA134643__INCLUDED___
#define ___AFX_MJSTATEMGR_H__52351F89_A913_425E_89A1_53CFDA134643__INCLUDED___

#include "IMJStateEvent.h"
#include "BaseTileDealerMgr.h"

namespace MyGame
{ 
/**
 * @ingroup MJFrame
 * @brief CMJStateMgr
 *
 * <p>　 
 *		CMJStateMgr, 麻将状态管理基类
 * </p>
 * @sa
 */	
template<int nPlayerNbr>
class CMJStateMgr : public IMJStateEvent,
					#ifndef WINDOWS_LOGIC
					public CSoTileDealerMgr<PLAYER_NBR>
					#else
					public CGameTileDealerMgr<nPlayerNbr>
					#endif
{
public:
	CMJStateMgr();
	virtual ~CMJStateMgr();

    /**
    *  @brief 设置状态
    *  @param[in]  nState    设置的状态
    *  @param[in]  bTimelag  TRUE: 延时操作  FALSE: 不延时
    *
    *  在设置了状态后会根据状态执行相关的操作
    */	
    virtual void SetMJState( int nState, BOOL bTimelag = FALSE );

	/**
	 *  @brief 获取当前的麻将状态
	 *  @return  麻将状态
	 */	
	virtual int GetMJState() { return m_nGameState; }

protected:
	int m_nGameState;
};

/**
 * @ingroup MJFrame
 * @brief CMJSoStateMgr
 *
 * <p>　 
 *		CMJSoStateMgr, SO麻将状态管理基类
 * </p>
 * @sa
 */	
#ifndef WINDOWS_LOGIC
template<int nPlayerNbr>
class CMJSoStateMgr : public CMJStateMgr<PLAYER_NBR>
{
public:
	CMJSoStateMgr();
	virtual ~CMJSoStateMgr();

    /**
    *  @brief 设置状态
    *  @param[in]  nState    设置的状态
    *  @param[in]  bTimelag  TRUE: 延时操作  FALSE: 不延时
    *
    *  在设置了状态后会根据状态执行相关的操作
    */	
    virtual void SetMJState( int nState, BOOL bTimelag = FALSE );

	virtual int OnGameMessage(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 进入开始时的发牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateDeal( int nOldState );

	/**
	 *  @brief 进入摸牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateDraw( int nOldState );

	/**
	 *  @brief 进入拦牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateBlock( int nOldState );
	
	/**
	 *  @brief 进入结束状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateEnd( int nOldState );

	/**
	 *  @brief 进入出牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateGive( int nOldState );

	/**
	 *  @brief 进入开始前补花状态
	 *  @param[in]  nOldState 上一个状态
	 *
	 *  此状态可以不一定用来补花，这个状态的特点是要等待所有玩家反馈信息再进行游戏的状态
	 */	
	virtual void OnSetStateFlower( int nOldState );

	/**
	 *  @brief 设置游戏结束的类型
	 *  @param[in]  eGameEndType 结束的类型
	 */	
	virtual void SetGameLogicEnd(GAME_END_TYPE eGameEndType) {};

    // 毫秒级定时器
    virtual void OnMSTimer(int nTimerID);

protected:
	/**
	 *  @brief 计时时间到触发的时间
	 */	
	virtual void OnTimeOut( int nTimerID );

};

/**
 * @ingroup MJFrame
 * @brief CMJGameStateMgr
 *
 * <p>　 
 *		CMJGameStateMgr, C麻将状态管理基类
 * </p>
 * @sa
 */	
#else

template<int nPlayerNbr>
class CMJGameStateMgr : public CMJStateMgr<nPlayerNbr>
{
public:
	CMJGameStateMgr();
	virtual ~CMJGameStateMgr();

	/**
	 *  @brief 设置状态
	 *  @param[in]  nState    设置的状态
     *  @param[in]  bTimelag  TRUE: 延时操作  FALSE: 不延时
	 *
	 *  在设置了状态后会根据状态执行相关的操作
	 */	
	virtual void SetMJState( int nState, BOOL bTimelag = FALSE );

	/**
	 *  @brief 进入出牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateGive( int nOldState );
	
	/**
	 *  @brief 进入等待状态
	 *  @param[in]  nOldState 上一个状态
	 *
	 *  此状态下玩家的时间一直循环
	 */	
	virtual void OnSetStateWait( int nOldState );

	/**
	 *  @brief 进入结束状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateEnd( int nOldState );

	/**
	 *  @brief 进入开始前补花状态
	 *  @param[in]  nOldState 上一个状态
	 *  
	 *  此状态可以不一定用来补花，这个状态的特点是要等待所有玩家反馈信息再进行游戏的状态
	 */	
	virtual void OnSetStateFlower( int nOldState );

	/**
	 *  @brief 进入拦牌状态
	 *  @param[in]  nOldState 上一个状态
	 */	
	virtual void OnSetStateBlock( int nOldState );

	virtual void OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg);

	/**
	 *  @brief 设置时间后出发的事件处理
	 *  @param[in]  dwGameTimerID 时间的ID
	 */	
	virtual void OnGameTimer( UINT dwGameTimerID );

	/************************************************************************/
	/* 
	逻辑时间到
	*/
	/************************************************************************/

	/**
	 *  @brief 逻辑时间的事件处理
	 *  @param[in]  nLogicTimerID 逻辑时间的ID
	 */	
	virtual void OnLogicTimeOut( int nLogicTimerID );

	
	/**
	 *  @brief 等待时间到的事件处理
	 *  @param[in]  nLogicTimerID 逻辑时间的ID
	 */	
	virtual void OnTimeOutWait();

	/**
	 *  @brief 准备时间到的事件处理
	 *  @param[in]  nLogicTimerID 逻辑时间的ID
	 */	
	virtual void OnTimeOutReady();

};

#endif

}

#include "MJStateMgr.inl"

#endif // !defined(AFX_MJSTATEMGR_H__52351F89_A913_425E_89A1_53CFDA134643__INCLUDED_)
