
#if !defined(SO_USER_MANAGER_I_H)
#define SO_USER_MANAGER_I_H

#include "Svrdef.h"
#include "gameobject_i.h"
#include "gameevent_i.h"
#include "playerManagerCommon.h"
#include "commonStructs.h"

namespace MyGame
{
#ifndef IPC_VERSION
//////////////////////////////////////////////////////////////////////////////
//简体部分： begin

	struct stRegionOption 
	{
		char m_langCode[3];
		char m_areaCode[3];
		int m_timeZone;
	};

	#define		MAX_NAME_LEN		20

	
	typedef struct
	{
		unsigned int m_iUin;				// 玩家的ID号码
		short m_nPlayerID;  				// 玩家在server用户列表中索引
		short m_nChair;     				// 玩家所在的椅子号
		short m_nState;     				// 玩家的状态
		short m_nFaceID;					// 玩家的图标编号
		unsigned int m_uiRightLevel;		// 用户权限
		char  m_szUserName[MAX_NAME_LEN];	// 玩家名
	} TABLE_USER_INFO;

//简体部分: end.
//////////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////////
//IPC 部分 : begin

	#define		MAX_NAME_LEN		30

	struct stRegionOption 
	{
		char m_langCode[3];
		char m_areaCode[3];
		int m_timeZone;
	};
	
//	#ifndef __REGION_OPT__
//	#define __REGION_OPT__
//	#endif

	typedef struct
	{
		unsigned int m_iUin;				// 玩家的ID号码
		short m_nPlayerID;  				// 玩家在server用户列表中索引
		short m_nChair;     				// 玩家所在的椅子号
		short m_nState;     				// 玩家的状态
		short m_nFaceID;					// 玩家的图标编号
		unsigned int m_uiRightLevel;		// 用户权限
		char  m_szUserName[MAX_NAME_LEN + 1];	// 玩家名
// 	#ifdef __REGION_OPT__
		stRegionOption m_region;
// 	#endif

	} TABLE_USER_INFO;

//IPC 部分 end.
//////////////////////////////////////////////////////////////////////////////
#endif

	//BEGIN modified by nekeyzhong for 欢乐体系2006-1-18 sequence:mainsvr_111_20060109
	enum enMutexInfoType
	{
		TMUTEX_MONEY	= 0,				//游戏币
		TMUTEX_HAPPY_ERNERGY =  1,			//欢乐体力值
		TMUTEX_HAPPY_CURRECY = 2			//欢乐币
	};
	//END modified by nekeyzhong 

	typedef struct tagReplacingAvatar
	{
		int m_iAvatarID;
		short m_nAvatarType;
		int m_iValidTime;
		int m_iAvatarEquiping;
	}TReplacingAvatar;

	//! 帐单类型
	typedef enum enmBillType 
	{
		EBT_HAPPY_TICKET		= 1001,	/*!<  分数门票收入*/
		EBT_HAPPY_ESCAPE		= 1002,	/*!<  分数游戏的逃跑收入*/
		EBT_HAPPY_ITEM			= 1003,	/*!<  分数使用*/
		EBT_MONEY_TICKET		= 2001,	/*!<  游戏币门票收入,在我们游戏里平常用得最多的，初始值*/
		EBT_MONEY_ESCAPE		= 2002,	/*!<  游戏币游戏的逃跑收入*/
		EBT_MONEY_ITEM			= 2003,	/*!<  游戏币使用*/
	}ENM_BILL_TYPE;

	enum enPayType
	{
		pay_bySelf		= 0,	// 自己付款
		pay_byOther		= 1,	// 他人付款
		pay_forOther	= 2		// 为他人付帐
	};

	enum enItemType
	{
		public_item = 0,		// 公用道具
		unit_item = 1,			// 次数道具
		time_item = 2			// 时效道具
	};

	// 玩家得分
	typedef struct	tagUserScore
	{ 
		int  lScore;			// 玩家分数         这是增量 可+，可-
		int  lMoney;			// 游戏币数         这是增量 可+，可-

        int  lExperience;     // 增长的经验值，   这是增量 可+，可-
        int  lGameExperience;   //增长的现在的游戏的经验值， 这是增量，可+ ，可-

		UINT lWinCount;		// 赢的盘数         这是增量 可+，不可-
		UINT lLostCount;		// 输的盘数         这是增量 可+，不可-
		UINT lDogfall;		// 平局盘数         这是增量 可+，不可-
		UINT lEscapeCount;	// 逃跑次数         这是增量 可+，不可-

		UINT lCostTime;      // 本次游戏的时间长
		UINT lSharedMoney;   // 抽水游戏币数    平台内部的返回抽水游戏币数
        UINT lShareScore;	// 玩家分数        平台内部的返回抽水分数
		int  lBillType;		// 账单类型        //金币，分数
	}USER_SCORE;
	

	class ISoUser  
	{
	public:
		//获得玩家ID
		virtual UINT GetUserID() = 0;
		virtual UINT GetUserAge() = 0;
		virtual UINT GetUserGender() = 0;        /// GetUserGender 返回 0 表示女性， 返回 1：表示男人
		virtual UINT GetChairID() = 0;
		virtual UINT GetUserStatus() = 0;
		virtual UINT GetFaceID() = 0;
        virtual int  GetRightLevel() = 0;
        virtual int  GetScore() = 0;              /// 获取游戏积分
        virtual int  GetMoney() = 0;              /// 获取游戏金币

		//nSize为给lpszNickName分配的空间
		virtual void  GetNickName(char *lpszNickName, int nSize) = 0;
		virtual char * GetNickName() = 0;
        virtual BOOL  IsReady() = 0;
		virtual BOOL  IsVIP() = 0;                 /// 会员会有等级，返回 bMember 是代表会员的等级信息

		virtual BOOL  IsPlayer()   = 0;
		virtual BOOL  IsOffline()  = 0;

        virtual void   ClearState() = 0;
	};

	class ISoPlayer :public ISoUser
	{
	public:
		virtual void SetTeam(ENM_PLAYER_TEAM enmTeam) = 0;
		virtual ENM_PLAYER_TEAM GetTeam() = 0;

		//设置旁观
		virtual void SetBCanBeLookedOn(BOOL bCanBeLookedOn) = 0;
		virtual BOOL GetBCanBeLookedOn() = 0;
		virtual BOOL IsLockedMoney() = 0;

		//锁定玩家的钱
		virtual void SetPlayerLockMoneyFlag(BOOL bLocked) = 0;

		//设置托管
		virtual BOOL GetTrusteeStatusVal() = 0;

		virtual void SetTrusteeStatusVal(BOOL bTrue) = 0;

		virtual void SetPlayerLockHappyBeanFlag(BOOL bLocked) = 0;
		virtual BOOL IsLockedHappyBean() = 0;

        virtual BOOL  IsForceExit() = 0;
        virtual void   SetForceExit(BOOL bForceExit) = 0;

	};

	class ISoWatcher : public ISoPlayer
	{
		
	};

	//ISoUserManager 与 ISoUserManagerInter
	/**
	 * @ingroup GameSDK
	 * @brief ISoUserManager
	 *
	 * <p>　 
	 *		ISoUserManager 服务端玩家管理模块
	 * </p>
	 * @sa
	 */	
	class ISoUserManager : public IGameObject, public IEventProducer
	{
	public:
 		//获取队伍的数量
		/**
		 *  @brief 获取队伍的数量
		 *  @param bExitAtPlay TRUE游戏中退出的玩家也计算，否则相反
		 *  @sa 
		 *	@return 队伍的数量
		 */	
 		virtual UINT GetTeamCount(BOOL bExitAtPlay /*= FALSE*/) = 0;

		/**
		 *  @brief 根据PlayerID获取用户ISoUser指针
		 *  @param shPlayerID 用户的PlayerId
		 *  @sa ISoUser
		 *	@return 用户的ISoUser接口指针，需判空
		 */	
		virtual ISoUser* GetUserByID(UINT shPlayerID) = 0;

		/**
		 *  @brief 获得当前玩家的数量
		 *	@return 当前玩家的数量
		 */	
		virtual UINT	GetPlayerCount() = 0;

		/**
		 *  @brief 根据PlayerID获取玩家ISoPlayer指针
		 *  @param shPlayerID 玩家的PlayerId
		 *  @sa ISoPlayer
		 *	@return 玩家的ISoPlayer接口指针，需判空
		 */	
		virtual ISoPlayer*	GetPlayerByID(UINT shPlayerID) = 0;

		/**
		 *  @brief 根据玩家ChairId获取玩家ISoPlayer指针
		 *  @param unUin 玩家ChairId
		 *  @sa ISoPlayer
		 *	@return 玩家的ISoPlayer接口指针，需判空
		 */
		virtual ISoPlayer*	GetPlayerByChair(UINT uChair) = 0;

		/**
		 *  @brief 判断一个用户是否是玩家
		 *  @sa 
		 *	@return TRUE表示是，FALSE表示不是
		 */
		virtual BOOL IsPlayer(UINT uPlayerID) = 0;

		/**
		 *  @brief 设置一个玩家是否处于托管状态
		 *  @param cChair 玩家的ChairID
		 *  @param bTrustee 是否处于托管状态
		 *  @sa 
		 */
		virtual void SetPlayerTrusteeStatus(UINT uChair, BOOL bTrustee) = 0;

		/**
		 *  @brief 判断一个用户是否处于托管状态
		 *  @param cChair 玩家的ChairID
		 *  @sa 
		 *	@return TRUE表示是，FALSE表示不是
		 */
		virtual BOOL GetPlayerTrusteeStatus(UINT cChair) = 0;

        /**
        *  @brief  设置游戏结算数据
        *  @param nDealer 庄家位置
        *  @param byWin   赢家位置(支持一炮多响)
        *  @param arPoint 每个玩家需要结算的点数
        *  @param rePoint 结算后的点数
        *  @return 返回值无意义
        */
        virtual STATUS SetGameScore(int nDealer, BYTE *byWin, int *arPoint, int *rePoint) = 0;

		/**
		 *  @brief 获取某个用户的游戏币数量
		 *  @param nPlayerID 用户的PlayerID
		 *  @param piMoney[out]      存放游戏币数量int型的地址
		 *  @param bMinusTicket      获取的游戏币是否已减去门票
		 *	@return 返回值无意义
		 */
		virtual int GetMoney(UINT nPlayerID, int* piMoney, BOOL bMinusTicket = TRUE) = 0;
	
		/**
		 *  @brief 获取房间的类型，游戏币房间、欢乐豆房间还是普通房间
		 *	@return 返回房间的类型
		 *  @sa ENM_ROOM_TYPE
		 */
		virtual int GetRoomType() = 0; 
	};


/*	//游戏正式开始 与 游戏结束时，CSoUserManager中的处理操作，在gameprocess内部调用
	class ISoUserManagerInter
	{
	public:
		//游戏开始
		virtual int  OnGameStart() = 0;
		//游戏结束
		virtual void OnGameEnd() = 0;
	};*/
	
	class ISoUserManagerTrusteeEvent : public IEvent
	{
	public:
		virtual int OnChangeTrustee(UINT nChair, BOOL bIsTrustee) = 0;
	};

	
	typedef enum enmBeforeKickType
	{
		EBKT_OFFLINE_ON_NOTALLOW_OFFLINE = 0,					// 不允许断线续玩的时候 断线和续玩
		EBKT_OFFLINE_ON_WATCHER_OFFLINE_GAMEONTSTART,			// 旁观或游戏还没有开始 断线和续玩
		EBKT_OFFLINE_ON_GAME_END,								// 游戏结束后 对断线玩家的处理
		EBKT_FAIL_LOCK_MONEY,									// 游戏币场锁定游戏币失败
		EBKT_FAIL_LOCK_HAPPYBEAN,								// 欢乐豆场锁定欢乐豆失败
		EBKT_MORE_THAN_UPPER,									// 超出上限
		EBKT_LESS_THAN_TICKET,									// 不够门票
		EBKT_LESS_THAN_LOWER,									// 低于下限
		EBKT_GAME_HAS_START,									// 游戏已经开始
        EBKT_VIP_KICK,                                          // VIP踢人              
        EBKT_GM_KICK                                            // GM 踢人
	}ENM_BEFORE_KICKTYPE;

	class ISoUserManagerEvent : public IEvent
	{
	public:
		virtual int OnGameUserEnter(UINT nPlayerID) = 0;
		virtual int OnGameUserExit(UINT nPlayerID) = 0;
		virtual int OnGameUserOffLine(UINT nPlayerID) = 0;
		virtual int OnGameUserReplay(UINT nPlayerID) = 0;
		virtual int OnGameUserReady(UINT nPlayerID) = 0;

		/**
		 *  @brief 玩家强退
		 *  @param [in] byChair 玩家位置
		 *	@return 返回值暂无意义
		 *  @sa
		 */
		virtual int OnGameUserForceQuit(BYTE byChair) = 0;

        // 暂时没有实现此些功能
        // 如果回应的bResult == 1 则可以踢这个人，不然就不准再踢这个
        // enmKickType  的类型是ENM_BEFORE_KICKTYPE
		virtual int OnBeforeKickUser(UINT nPlayerID, ENM_BEFORE_KICKTYPE enmKickType, BOOL *pbResult) = 0;
		//经TransHandle上来的 
		//nResult为1时才可以坐下，其他不可以
		virtual int OnGameIsPlayerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char*lpszMsg) = 0;
		virtual int OnGameIsViewerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char*lpszMsg) = 0;
		virtual int OnGameNotifyUserExitReason(UINT nReason, UINT nPlayerID, UINT nszBufSize, char* szBuf) = 0;
		
		virtual int OnChangeLookOnSetting(UINT nPlayerID, BOOL bLookOn) = 0;

        //*pbResult 初始是为TRUE, 如果为FALSE， FRAME是不让用户进行改变桌位的
        virtual int OnBeforeGameUserChangeChair(UINT nPlayerID, BYTE bSrcChair, BYTE bDstChair, BOOL* pbResult)= 0;
	};

	class ISoUserMgrToGameProcessEvent : public IEvent
	{
	public:
		//开始倒计时, 返回值没有意义
		virtual int OnBeginCountDown() = 0;
		//取消倒计时, 返回值没有意义
		virtual int OnCancleCountDown() = 0;
	};
}
#endif

