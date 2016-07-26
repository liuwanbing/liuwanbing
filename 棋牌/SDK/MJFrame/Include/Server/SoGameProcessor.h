#ifndef __GAME_PROCESSOR__
#define __GAME_PROCESSOR__

#include "afxwin.h"
#include "typeDef.h"
#include "SoGameProcessor_i.h"
#include "SoFireEvent_t.h"

namespace MyGame
{
class CDeskLogicImpl;
class GameProcessor : public TSoEventProducerImpl<ISoGameProcessor, 5>
{
public:
	GameProcessor(CDeskLogicImpl* pDeskLogicImpl);

	~GameProcessor();

	/** 
	 *  @brief 设置游戏开始模式
	 *  @param [in] byMode 0: 满人才开始   1: 所有人同意就开始   2: 对称同意开始   3: 6个座位中只要有4个人同意开始
	 */
	virtual void SetStartMode(BYTE byMode);

	/**
	 *  @brief 获得游戏的状态
	 *	@return 返回值为GAME_STATUS枚举中的值
	 *  @sa GAME_STATUS
	 */	
	virtual int GetGameState();

	//////////////////////////////////////////////////////////////////////////
	/**
	 *  @brief 设置游戏开始，gamesdk告诉sogamelogic游戏可以开始之后，sogamelogic如果觉得游戏可以开始就调用此接口，gamesdk会在此接口里面调用mainsvr的开始
	 *  @sa
	 */	
	virtual void SetGameStart();

	//设置游戏结束
	/**
	 *  @brief 设置游戏结束sogamelogic如果觉得游戏可以结束就调用此接口，gamesdk会在此接口里面调用mainsvr的结束
	 *  @param nGameEndType GAME_END_TYPE枚举值
	 *	@return 返回值暂无意义
	 *  @sa GAME_END_TYPE
	 */	
	virtual BOOL SetGameEnd(int nGameEndType);

	//////////////////////////////////////////////////////////////////////////
	/**
	 *  @brief 向该桌所有玩家发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */	
	virtual int SendGameData(char * pcGameData, UINT nDataLen,  UINT uiFlag );

	/**
	 *  @brief  向该桌nUserID玩家发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SendGameDataTo(UINT nPlayerID, char * pcGameData, UINT nDataLen, UINT uiFlag );

    /**
    *  @brief  向该桌nSeatID玩家发送游戏数据
    *  @return 返回值暂无意义
    *  @sa
    */
    virtual int SendGameDataTo(int nSeatID, char * pcGameData, UINT nDataLen, UINT uiFlag );

	/**
	 *  @brief 向该桌指定座位的游戏者和旁观者发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SendGameDataToSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag);

	/**
	 *  @brief 向该桌除了指定座位的所有其他游戏者和旁观者发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SendGameDataExceptSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag);
	
	/**
	 *  @brief 向该桌除了nUserID的所有玩家发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SendGameDataExcept(UINT nPlayerID, char* pcGameData, UINT nDataLen, UINT uiFlag);

	/**
	 *  @brief 向该桌nChair位置上的所有观看者发送游戏数据
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SendGameDataToLookOnUsers(UINT nChair, char* pcGameData, UINT nDataLen, UINT uiFlag);

	/**
	 *  @brief 获取长局信息
	 *	@return 长局数
	 *  @sa
	 */
	virtual int GetGameContinueRounds();

	/**
	 *  @brief 设置长局数长局信息，设置之后gamesdk会自动和客户端的同步
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int SetGameContinueRounds(UINT nRound);
	
	/**
	 *  @brief 设置一个定时器,定时器的ID是nTimerID, 定时器的时长为nTime
	 *  @param[in] nTimerID 定时器的ID
	 *  @param[in] nTime	定时器的时长(ms作为单位)
	 */	
	virtual void  SetTimer(UINT nTimerID, int nTime);

	/**
	 *  @brief 停止定时器
     *  @param[in] nTimerID 定时器的ID
	 */	
	virtual void KillTimer(UINT nTimerID);

    /**
	 *  @brief 增加一个定时器回调用于麻将
	 */
	virtual int AddTimerCall(ISoTimerCall* pTimerCall);

    /**
    *  @brief 获取房间ID
    */
    virtual UINT GetRoomID();

	/**
    *  @brief 获取房间税收比率
    */
    virtual UINT GetRoomTax();

    /**
    *  @brief 获取房间底注
    */
    virtual UINT GetBaseMoney();

    /**
    *  @brief 获取房间最少金币限制
    */
    virtual UINT GetMinLimitMoney();
public:
	/**
	 *  @brief 秒定时器的调用方法，固定服务器就是只有一个秒定时器
	 */
    void OnMillTimer();

private:
    CDeskLogicImpl*  m_pDeskLogicImpl;
    ISoTimerCall*    m_pAddTimerCall;

public:
    UINT             m_GameBeginTimes; // 游戏已进行了多长时间

};

}

#endif
