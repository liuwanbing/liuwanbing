#ifndef __SO_USERMANAGER__
#define __SO_USERMANAGER__

#include "afxwin.h"
#include "SoUserManager_i.h"
#include "SoFireEvent_t.h"
#include "../../Include/ServerCommon/UserInfo.h"

namespace MyGame
{
	class SoPlayer : public ISoPlayer
	{
    public:
        SoPlayer();
        ~SoPlayer();
        void Initialize(CGameUserInfo* pUserInfo);

        // 获得玩家ID
		UINT GetUserID();
		UINT GetUserAge();
		UINT GetUserGender();
		UINT GetChairID();
		UINT GetUserStatus();
		UINT GetFaceID();                   //暂时没有实现FACE ID号
		int  GetRightLevel();
        int  GetScore(); 
        int  GetMoney(); 
     
		// nSize为给lpszNickName分配的空间
		void  GetNickName(char *lpszNickName, int nSize);
		char * GetNickName();
        BOOL  IsReady();
		BOOL  IsVIP() ;
		BOOL  IsPlayer();
		BOOL  IsOffline()  ;
        BOOL  IsForceExit();
        void  SetForceExit(BOOL bForceExit);
        
        // ISoPlayer 
		void SetTeam(ENM_PLAYER_TEAM enmTeam);
		ENM_PLAYER_TEAM GetTeam();

		// 设置旁观
		void SetBCanBeLookedOn(BOOL bCanBeLookedOn);
		BOOL GetBCanBeLookedOn();
		BOOL IsLockedMoney();

		// 锁定玩家的钱
		void SetPlayerLockMoneyFlag(BOOL bLocked);

		//设置托管
		BOOL GetTrusteeStatusVal();

		void SetTrusteeStatusVal(BOOL bTrue);

		void SetPlayerLockHappyBeanFlag(BOOL bLocked);

		BOOL IsLockedHappyBean();

        void ClearState();

    private:
        CGameUserInfo*  m_pUserInfo;
        char           m_bIfCanLook;
        char           m_bLockedMoney;
        char           m_bLockedHappyBean;
        char           m_bTrustee;             // 拖管状态
        char           m_bForceExit;
	};

    //为了接口一样，使得SoWatcher 类型与 SoPlayer 说明一样，只是与原来的方法定义保持一样
	class SoWatcher : public ISoWatcher
	{
    public:
        SoWatcher();
        ~SoWatcher();
	};

    class CDeskLogicImpl;
	class SoUserManager : public TSoEventProducerImpl<ISoUserManager, 5>
	{
    public:
		SoUserManager(CDeskLogicImpl* pDeskLogicImpl);
		~SoUserManager(void);
	public:
		//获取队伍的数量
		/**
		*  @brief 获取队伍的数量
		*  @param bExitAtPlay TRUE游戏中退出的玩家也计算，否则相反
		*  @sa 
		*	@return 队伍的数量
		*/	
		UINT GetTeamCount(BOOL bExitAtPlay /*= FALSE*/);   //暂是不实现

		/**
		*  @brief 根据PlayerID获取用户ISoUser指针
		*  @param shPlayerID 用户的PlayerId
		*  @sa ISoUser
		*	@return 用户的ISoUser接口指针，需判空
		*/	
		ISoUser* GetUserByID(UINT shPlayerID);

		/**
		*  @brief 获得当前玩家的数量
		*	@return 当前玩家的数量
		*/	
		UINT GetPlayerCount();

		/**
		*  @brief 根据PlayerID获取玩家ISoPlayer指针
		*  @param shPlayerID 玩家的PlayerId
		*  @sa ISoPlayer
		*	@return 玩家的ISoPlayer接口指针，需判空
		*/	
		ISoPlayer* GetPlayerByID(UINT shPlayerID);

		/**
		*  @brief 根据玩家ChairId获取玩家ISoPlayer指针
		*  @param unUin 玩家ChairId
		*  @sa ISoPlayer
		*	@return 玩家的ISoPlayer接口指针，需判空
		*/
		ISoPlayer* GetPlayerByChair(UINT uChair);

		/**
		*  @brief 判断一个用户是否是玩家
		*  @sa 
		*  @return TRUE表示是，FALSE表示不是
		*/
		BOOL IsPlayer(UINT uPlayerID);

		/**
		*  @brief 设置一个玩家是否处于托管状态
		*  @param cChair 玩家的ChairID
		*  @param bTrustee 是否处于托管状态
		*  @sa 
		*/
		void SetPlayerTrusteeStatus(UINT uChair, BOOL bTrustee);

		/**
		*  @brief 判断一个用户是否处于托管状态
		*  @param cChair 玩家的ChairID
		*  @sa 
		*  @return TRUE表示是，FALSE表示不是
		*/
		BOOL GetPlayerTrusteeStatus(UINT cChair);

        /**
        *  @brief  设置游戏结算数据
        *  @param nDealer 庄家位置
        *  @param byWin   赢家位置(支持一炮多响)
        *  @param arPoint 每个玩家需要结算的点数
        *  @param rePoint 结算后的点数
        *  @return 返回值无意义
        */
        virtual STATUS SetGameScore(int nDealer, BYTE *byWin, int *arPoint, int *rePoint);

		/**
		*  @brief 获取某个用户的游戏币数量
		*  @param nPlayerID 用户的PlayerID
		*  @param piMoney[out]      存放游戏币数量int型的地址
		*  @param bMinusTicket      获取的游戏币是否已减去门票
		*	@return 返回值无意义
		*/
		int GetMoney(UINT nPlayerID, int* piMoney, BOOL bMinusTicket = TRUE);   // 这个门票东西暂没有实现

		/**
		*  @brief 获取房间的类型，游戏币房间、欢乐豆房间还是普通房间
		*	@return 返回房间的类型
		*  @sa ENM_ROOM_TYPE
		*/
		int GetRoomType();                              //接口先留着，没有实现

    private:
        CDeskLogicImpl* m_pDeskLogicImpl;
        UINT            m_cuChairIndex;
        UINT            m_cuWatcherIndex;
	};

};

#endif
