#ifndef _GAMEPROCESSOREVENT_H
#define _GAMEPROCESSOREVENT_H

#include "Svrdef.h"
#include "gameevent_i.h"
#include "TimerCall_i.h"


namespace MyGame
{
    //!游戏状态的类型
    enum GAME_STATUS
    {
	    gsWait = 0,            /*!< 等待状态 */
	    gsCountDown,           /*!< 倒计时状态 */
	    gsAllPlayerReady,      /*!< 所有玩家已经准备状态 */
	    gsPlaying              /*!< 游戏进行中状态 */
    };

	//!游戏结束的类型
	enum GAME_END_TYPE
	{
		enEndByGameOver = 10,  /*!< 游戏正常结束, 此值不能从0开始定义, 平台0为强退 */
		enEndByPlayerEscape,   /*!< 玩家逃跑导致游戏结束 */
		enEndByNetAdmin,	   /*!< 网管解散游戏导致游戏结束 */
		enEndByArrange,		   /*!< 协商退出而结束 */
		enNotEndGame,		   /*!< 客户端结束游戏，但不调用SetGameEnd(连打时使用) */
	};

	/**
	 * @ingroup GameSDK
	 * @brief ISoGameProcessor
	 *
	 * <p>　 
	 *		ISoGameProcessor 服务端游戏过程管理模块
	 * </p>
	 * @sa
	 */	
	class ISoGameProcessor : public IGameObject, public IEventProducer
	{
	public:
		/** 
		 *  @brief 设置游戏开始模式
		 *  @param [in] byMode 0: 满人才开始   1: 所有人同意就开始   2: 对称同意开始   3: 6个座位中只要有4个人同意开始
		 */
		virtual void SetStartMode(BYTE byMode) = 0;

		/**
		 *  @brief 获得游戏的状态
		 *	@return 返回值为GAME_STATUS枚举中的值       
		 *  @sa GAME_STATUS
		 */	
		virtual int GetGameState() = 0;

		//////////////////////////////////////////////////////////////////////////
		/**
		 *  @brief 设置游戏开始，gamesdk告诉sogamelogic游戏可以开始之后，sogamelogic如果觉得游戏可以开始就调用此接口，gamesdk会在此接口里面调用mainsvr的开始
		 *  @sa
		 */	
		virtual void SetGameStart() = 0;

		//设置游戏结束
		/**
		 *  @brief 设置游戏结束sogamelogic如果觉得游戏可以结束就调用此接口，gamesdk会在此接口里面调用mainsvr的结束
		 *  @param nGameEndType GAME_END_TYPE枚举值
		 *	@return 返回值暂无意义
		 *  @sa GAME_END_TYPE
		 */	
		virtual BOOL SetGameEnd(int nGameEndType) = 0;

		//////////////////////////////////////////////////////////////////////////
		//向该桌所有玩家发送游戏数据
		/**
		 *  @brief 向该桌所有玩家发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */	
		virtual int SendGameData(char * pcGameData, UINT nDataLen,  UINT uiFlag = 0) = 0;

		//向该桌nUserID玩家发送游戏数据
		/**
		 *  @brief 向该桌所有玩家发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SendGameDataTo(UINT nPlayerID,char * pcGameData, UINT nDataLen, UINT uiFlag = 0) = 0;

		//向该桌指定座位的游戏者和旁观者发送游戏数据
		/**
		 *  @brief 向该桌指定座位的游戏者和旁观者发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SendGameDataToSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag = 0) = 0;

		//向该桌除了指定座位的所有其他游戏者和旁观者发送游戏数据
		/**
		 *  @brief 向该桌除了指定座位的所有其他游戏者和旁观者发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SendGameDataExceptSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag = 0) = 0;
	
		//向该桌除了nUserID的所有玩家发送游戏数据
		/**
		 *  @brief 向该桌除了nUserID的所有玩家发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SendGameDataExcept(UINT nPlayerID, char* pcGameData, UINT nDataLen, UINT uiFlag = 0) = 0;

		//向该桌nChair位置上的所有观看者发送游戏数据
		/**
		 *  @brief 向该桌nChair位置上的所有观看者发送游戏数据
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SendGameDataToLookOnUsers(UINT nChair, char* pcGameData, UINT nDataLen, UINT uiFlag = 0) = 0;
		

		//获得长局信息
		/**
		 *  @brief 获取长局信息
		 *	@return 长局数
		 *  @sa
		 */
		virtual int GetGameContinueRounds() = 0;

		/**
		 *  @brief 设置长局数长局信息，设置之后gamesdk会自动和客户端的同步
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int SetGameContinueRounds(UINT nRound) = 0;

        /**
        *  @brief 设置一个定时器,定时器的ID是nTimerID, 定时器的时长为nTime
        *  @param[in] nTimerID 定时器的ID
        *  @param[in] nTime	定时器的时长(ms作为单位)
        */	
        virtual void  SetTimer(UINT nTimerID, int nTime) = 0;

        /**
        *  @brief 停止定时器
        *  @param[in] nTimerID 定时器的ID
        */	
        virtual void KillTimer(UINT nTimerID) = 0;

		/**
		 *  @brief 增加一个定时器回调用于麻将
		 */
		virtual int   AddTimerCall(ISoTimerCall* pTimerCall) = 0;

        /**
	 	*  @brief 获取房间ID
	 	*/
		virtual UINT  GetRoomID() = 0;

		/**
		*  @brief 获取房间税收比率
		*/
		virtual UINT GetRoomTax() = 0;

        /**
        *  @brief 获取房间底注
        */
        virtual UINT  GetBaseMoney() = 0;

        /**
        *  @brief 获取房间最少金币限制
        */
        virtual UINT  GetMinLimitMoney() = 0;
	};
	

	/**
	 * @ingroup GameSDK
	 * @brief ISoGameProcessorEvent
	 *
	 * <p>　 
	 *		ISoGameProcessorEvent 游戏过程事件
	 * </p>
	 * @sa
	 */	
	class ISoGameProcessorEvent : public IEvent
	{
	public:
		/**
		 *  @brief 收到玩家发送GameMsg，可以监听此事件获得玩家GameMsg
		 *  @param nMsgAssistantID Msg的副消息ID，这里用于把主消息已无用了，不用传进来
		 *  @param nPlayerID 发送此GameMsg的玩家PlayerId
		 *  @param nChair 发送此GameMsg的玩家的ChairId
		 *  @param pcGameData 消息体结构的指针
		 *  @param nDataLen 消息的长度
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnGameMessage(UINT  nMsgAssistantID, UINT nPlayerID, UINT nChair,  void * pcGameData,  UINT nDataLen) = 0;

		/**
		 *  @brief 收到旁观发送GameMsg，可以监听此事件获得旁观GameMsg
		 *  @param cMsgID Msg的ID
		 *  @param nPlayerID 发送此GameMsg的旁观PlayerId
		 *  @param nChair 发送此GameMsg的旁观的ChairId
		 *  @param pcGameData 消息体结构的指针
		 *  @param nDataLen 消息的长度
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnViewerMessage(UINT  nMsgAssistantID, UINT nPlayerID, UINT nChair,  void * pcGameData,  UINT nDataLen) = 0;
		//游戏开始与结束 都由Logic去设置，这里只起到通知的作用

		/**
		 *  @brief 所有的玩家都已经准备好的事件
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnAllPlayerReady() = 0;
		
		/**
		 *  @brief 该事件每秒触发一次，而且只在游戏进行过程中触发
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnGameTimer() = 0;
		//
	/*	//lightchen 直接监听UserManager
		virtual int OnUserEnter(short shPlayerID) = 0;
		virtual int OnUserExit(short shPlayerID) = 0;
		virtual int OnUserOffline(short shPlayerID) = 0;
		virtual int OnUserReplay(short shPlayerID) = 0;*/
		
		//
//		virtual int OnAllPlayerAgreeEndGame() = 0;	//仅仅是游戏结束的一种类型

// 		virtual int LoadCfg(const char * pConfigFile)  = 0;
// 		virtual int ReloadCfg(const char* pConfigFile) = 0;
	};

	
	/**
	 * @ingroup GameSDK
	 * @brief ISoGameProcessorGameEvent
	 *
	 * <p>　 
	 *		ISoGameProcessorGameEvent 游戏结束开始的事件
	 * </p>
	 * @sa
	 */	
	class ISoGameProcessorGameEvent: public IEvent
	{
	public:
		/**
		 *  @brief 游戏开始事件
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnGameStart() = 0;

		//网管解散游戏、协商退出、
		/**
		 *  @brief 网管解散游戏、协商退出事件的通知，gamesdk发出这个通知之前以及之后并不会设置游戏结束，需要gamelogic处理完之后调用gamesdk的游戏结束接口
		 *  @param nGameEndType GAME_END_TYPE枚举值
		 *	@return 返回值暂无意义
		 *  @sa nGameEndType
         */
	    /*            
        *enum GAME_END_TYPE{
		*enEndByGameOver = 0,	 游戏正常结束 
		*enEndByPlayerEscape,	 玩家逃跑导致游戏结束 
		*enEndByNetAdmin,		 网管解散游戏导致游戏结束 
		*enEndByArrange,		协商退出而结
		*enNotEndGame,			 客户端结束游戏，但不调用SetGameEnd(连打时使用) 
	    };
        */

		virtual int OnGameEnd(int nGameEndType) = 0;

		/**
		 *  @brief 设置游戏结束前触发，触发此事件之后，会离开更新游戏状态和调用mainsvr的游戏结束
		 *  @param nGameEndType GAME_END_TYPE枚举值
		 *	@return 返回值暂无意义
		 *  @sa nGameEndType
		 */
		virtual int OnBeforeGameEnd(int nGameEndType) = 0;

		/**
		 *  @brief 更新游戏状态和调用mainsvr的游戏结束之后触发
		 *  @param nGameEndType GAME_END_TYPE枚举值
		 *	@return 返回值暂无意义
		 *  @sa nGameEndType
		 */
		virtual int OnPostGameEnd(int nGameEndType) = 0;
	};

	/**
	 * @ingroup GameSDK
	 * @brief ISoGameProcessorSendBaseInfoEvent
	 *
	 * <p>　 
	 *		ISoGameProcessorSendBaseInfoEvent 游戏基本事件， 在玩家进入时调用之
	 * </p>
	 * @sa
	 */	
	class ISoGameProcessorSendBaseInfoEvent : public IEvent
	{
	public:
//		virtual int OnSendBaseInfo(int nVer) = 0;
		/**
		 *  @brief 通知其他模块玩家请求游戏的baseinfo
		 *  @param shPlayerID 玩家的PlayerID
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnSendBaseInfo(UINT shPlayerID) = 0;

		/**
		 *  @brief 通知其他模块玩家的客户端版本号
		 *  @param shPlayerID 玩家的PlayerID
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnNotityClientVer(int nVer) = 0;
	};

	/**
	 * @ingroup GameSDK
	 * @brief ISoGameProcessTimerEvent
	 *
	 * <p>　 
	 *		ISoGameProcessTimerEvent SO端Timer处理类(目前只支持一个Timer)
	 * </p>
	 * @sa
	 */	
	class ISoGameProcessTimerEvent : public IEvent
	{
	public:
		/**OnCollectBaseMsg
		 *  @brief 定时器触发时执行的函数
		 *  @param[in] nTimerID          定时器的ID  
		 */
		virtual int  OnTimer(int nTimerID) = 0;
	};

      /**
	 * @ingroup GameSDK
	 * @brief ISoGameProcess
	 *
	 * <p>　 
	 *		ISoGameProcessBaseMsg SO端处理玩家进入与断线的消息
	 * </p>
	 * @sa
	 */	
    class ISoGameProcessBaseMsgEvent : public IEvent
	{
	public:
		/**OnCollectBaseMsg
		 *  @brief 用户进入桌子，建立桌子等调用
		 *  @param[in] shPlayerID          用户的 ID  
		 */
		virtual int OnCollectBaseMsg(UINT uPlayerID, bool bWatchUser) = 0;

    	/**OnCollectMsg
		 *  @brief 定时器触发时执行的函数
		 *  @param[in] nTimerID          定时器的ID  
		 */
		virtual int OnCollectMsg(UINT uPlayerID, bool bWatchUser) = 0;

	};

}

#endif //_GAMEPROCESSOREVENT_H
