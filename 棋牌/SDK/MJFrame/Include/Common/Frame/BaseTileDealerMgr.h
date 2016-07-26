/** @file	BaseTileDealerMgr.h
 *	@brief	城墙管理类
 *	@date 08.08.04
 *
 *	继承于手牌管理类，从而获得所有麻将的控制权
 *	根据SO和C端的不同，从同一基类
 *	CBaseTileDealerMgr分为
 *	C端的CGameTileDealerMgr和
 *	SO的CSoTileDealerMgr
 */
#ifndef ___AFX_BASETILEDEALERMGR_H__02CA6498_356A_4B09_A39F_2F0EFA10916B__INCLUDED___
#define ___AFX_BASETILEDEALERMGR_H__02CA6498_356A_4B09_A39F_2F0EFA10916B__INCLUDED___

#include "IMJTileEvent.h"
#include "BaseTilePlayerMgr.h"
#include "MJTimerMgr.h"
#include "CommonStructs.h"

#ifdef WINDOWS_LOGIC
#include "datastructs.h"
#include <gamechatmanager_i.h> 
#endif

namespace MyGame
{

/**
 * @ingroup MJFrame
 * @brief CBaseTileDealerMgr
 *
 * <p>　 
 *		CBaseTileDealerMgr, 城墙控制类的基类
 * </p>
 * @sa
 */	
template<int nPlayerNbr> 
class CBaseTileDealerMgr : public IMJTileEvent,
						#ifndef WINDOWS_LOGIC
						public CSoTilePlayerMgr<PLAYER_NBR>,
 						public CSoMJTimerMgr
						#else
						public CGameTilePlayerMgr<nPlayerNbr>,
						public CGameMJTimerMgr
						#endif
{
public:
	CBaseTileDealerMgr();
	virtual ~CBaseTileDealerMgr();

	/**
	 *  @brief 处理手牌发送过来的关于牌的事件
	 *  @param[in]  nEventID 事件ID
	 *
	 *  目前没有任何事件
	 */	
	virtual void OnTileEvent( int nEventID );

	/**
	 *  @brief 城墙初始化
	 *  @return 返回0 
	 *
	 * 基类没做任何操作
	 */	
	virtual int InitDealer();

	/**
	 *  @brief 洗牌
	 */	
	virtual void RandomDealer();

	/**
	 *  @brief 起手的发牌到玩家手牌
	 */	
	virtual void SendTilesToPlayer();

	/**
	 *  @brief 清空圈风和门风
	 */	
	virtual void ResetWindRound();

	/**
	 *  @brief 发牌
	 *  @param[in]  nTurn 发给的目标
	 */	
	virtual void DrawTile( int nTurn );

	/**
	 *  @brief 清空城墙
	 */	
	virtual void  OnDealerPostGameEnd();

	/**
	 *  @brief 为下一局初始化
	 *
	 *  会增加圈风和门风的记数,并调用手牌管理的相关初始化
	 */	
	virtual void InitForNext();

	/**
	 *  @brief 获取当前操作的玩家
	 *  @return 返回当前的玩家 
	 */	
	virtual int GetTurn() { return m_nNowTurn; }

	/**
	 *  @brief 设置当前玩家
	 *  @param[in]  nTurn 当前的玩家
	 */	
	virtual void SetTurn( int nTurn );

	/**
	 *  @brief 获取显示当前玩家座位
	 *  @return 当前箭头显示指向的人
	 *  @warning 显示的玩家不一定是真正正在操作的玩家
	 */	
	virtual int GetShowTurn() { return m_nShowTurn; }
	
	/**
	 *  @brief 设置显示当前玩家座位
	 *  @param[in]  nTurn 要显示指向的玩家
	 */	
	virtual void SetShowTurn( int nTurn );
	
	/**
	 *  @brief 获取庄家
	 *  @return 返回庄家
	 */	
	virtual int GetDealer() { return m_nDealer; }

	/**
	 *  @brief 摇色子
	 *
	 *  投掷DICE_NBR数量个的色子,点数1到6
	 */	
	virtual void RandomDice();

	/**
	 *  @brief 设置门风
	 *  @param[in]  nRound 当前的风向
	 */	
	virtual void SetWindRound( int nRound );

	/**
	 *  @brief 设置圈风
	 *  @param[in]  nNbr 当前的圈数
	 */	
	virtual void SetWindRoundNbr( int nNbr );


	/**
	 *  @brief 获取门风
	 *  @return 返回门风向 
	 */	
	virtual int GetWindRound() { return m_nWindRound; }

	/**
	 *  @brief 获取圈风
	 *  @return 返回圈风数
	 */	
	virtual int GetWindRoundNbr() { return m_nWindRoundNbr; }

	/**
	 *  @brief 获取色子数
	 *  @param[in]  nIndex 色子的序列号
	 *  @return 返回色子数,如果序列号不在范围,返回1
	 */	
	virtual int GetDice( int nIndex );
	
	/**
	 *  @brief 通过引用的方式获取城墙数据
	 *  @param[out]  mjWall 城墙数据设置到其中
	 *  @return 返回0
	 */	
	virtual int GetTilesWall( CMJWall& mjWall );

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
	 *  @brief 设置算番环境变量
	 *  @param[in]  env 环境变量结构
	 *  @param[in]  nChair 要计算番的人
	 */	
	virtual void SetFanEvn( ENVIRONMENT &env, int nChair,  int bSetChairTile = TRUE  );

	/**
	 *  @brief 获取花牌的数量
	 *  @param[in]  nChair 要计算花的人
	 *  @return 返回花牌的数量
	 */	
	virtual int GetFlowerNbr( int nChair );

	/**
	 *  @brief 清空花牌信息
	 */	
	virtual void ClearFlower();

	/**
	 *  @brief 判断是否荒番
	 *  @return 如果城墙没牌了就返回TRUE,否则返回FALSE
	 */	
	virtual BOOL CheckNoTile();

	/**
	 *  @brief 补花的时间结束
	 *
	 *  基类没内容
	 */	
	virtual void OnTimeOutFlower(){};

	/**
	 *  @brief 获取城墙
	 *
	 *  基类没内容
	 */	
	virtual CMJWall GetTilesDealer(){ return m_TilesDealer; }

	/**
	 *  @brief 清空连局数据
	 */	
	virtual void ClearRunNbr();

protected:
	/************************************************************************/
	/* 城墙的数据                                                           */
	/************************************************************************/
	CMJWall m_TilesDealer;
	/************************************************************************/
	/* 色子                                                                 */
	/************************************************************************/
	int m_nDice[DICE_NBR];

	/************************************************************************/
	/* 庄家                                                                 */
	/************************************************************************/
	int m_nDealer;

	/************************************************************************/
	/* 风向和风圈                                                           */
	/************************************************************************/
	int m_nWindRound;
	int m_nWindRoundNbr;

	/************************************************************************/
	/* 当前操作的人                                                         */
	/************************************************************************/
	int m_nNowTurn;

	// 作图显示的人
	int m_nShowTurn;
	
	// 连续局数
	int m_nRunNbr;
	// 连续胡数
	int m_nRunWinNbr;
	// 连续荒牌
	int m_nRunNotileNbr;

	/************************************************************************/
	/* 所有正常摸的牌数据(不包括杠牌)                                       */
	/************************************************************************/
	CMJHand m_TilesDraw;

	/************************************************************************/
	/* 所有杠后摸的牌数据                                                   */
	/************************************************************************/
	CMJHand m_TilesGang;
};

#ifndef WINDOWS_LOGIC


/**
 * @ingroup MJFrame
 * @brief CSoTileDealerMgr
 *
 * <p>　 
 *		CSoTileDealerMgr, SO下的城墙控制类
 * </p>
 * @sa CBaseTileDealerMgr
 *
 * 继承于CBaseTileDealerMgr
 */	
template<int nPlayerNbr> 
class CSoTileDealerMgr : public CBaseTileDealerMgr<PLAYER_NBR>
{
public:
	CSoTileDealerMgr();
	virtual ~CSoTileDealerMgr();

	virtual int  OnGameMessage(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 城墙初始化
	 *  @return 返回0 
	 *
	 *  执行洗牌,设置门风向和圈风数,清空花牌,设置万能牌等操作
	 */	
	virtual int InitDealer();

	/**
	 *  @brief 起手的发牌到玩家手牌
	 *
	 *  如果玩家的配牌需求打开,则在此函数里面执行了配牌操作以
	 */	
	virtual void SendTilesToPlayer();

	/**
	 *  @brief 发送结束信息给所有玩家
	 *
	 *  当游戏状态切换到MJ_STATE_END的时候就会执行此函数,发送相关的结束信息给所有玩家(包括旁观)
	 */	
	virtual void SendFinishMsg();

	/**
	 *  @brief 游戏开始
	 *
	 *  调用手牌管理对象的相关函数,本身没有其他操作,在SDK的OnGameStart执行时被MJFrame调用
	 */	
	virtual int  OnGameStart();

	/**
	 *  @brief 游戏结束
	 *
	 */	
	virtual int  OnGameFinish();

	/**
	 *  @brief 发牌
	 *  @param[in]  nTurn 要发给的人
	 *
	 *  发一张牌给指定的玩家,会通过发牌的操作判断下一步切换到的状态
	 */	
	virtual void DrawTile( int nTurn );

    /** 
     * @brief 毫秒级定时器
     * @param[in] nTimerID 定时器ID
     */
    virtual void OnMSTimer(int nTimerID);

	/**
	 *  @brief 支持自动化测试的游戏前配牌消息处理
	 *  @warning 此功能在发布的时候必须关闭
	 */	
    virtual int ProcessRequestReplaceAllCards(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 处理请求补花的消息
	 *  
	 *  在接受到后会执行补花操作,在游戏前补花状态的时候,会判断是否所有人都请求补花而进入游戏中的状态
	 */	
    virtual int ProcessRequestFlower(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 请求换牌
	 *  
	 *  移花接木和偷天换日两个道具执行的消息
	 */	
	virtual int ProcessRequestChangeTiles(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);
	
	/**
	 *  @brief 请求获取牌位置
	 *  
	 *  千里眼道具执行的消息
	 */	
	virtual int ProcessRequestGetPosTile(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 请求获取城墙信息
	 *  
	 *  顺风耳道具执行的消息
	 */	
	virtual int ProcessRequestGetTiles(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);
		
	/**
	 *  @brief 请求配牌
	 *  
	 *  乾坤大挪移道具执行的消息
	 */	
	virtual int ProcessRequestConfigTiles(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);
	
 	/**
	 *  @brief 处理请求出牌信息
	 *
	 *  每次发牌后，到需要把显示位置设置到出牌者的下家
	 */	
	virtual int ProcessRequestGive(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

    /*
    *  @brief 玩家请求发牌完成信息
    */
    virtual int ProcessRequestDealFinish(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 补花状态结束
	 *  
	 *  执行补花状态结束后,判断当前出牌人手牌来决定进入下个状态
	 */	
	virtual void OnTimeOutFlower();

	/**
	 *  @brief 补花后的判断
	 *  @param[in]  nChair 请求补花的人
	 *  
	 *  玩家请求补花后,如果在补花状态,就会判断是否全部请求补花而进入下个状态
	 */	
	virtual void CheckOverFlower( int nChair );

	/**
	 *  @brief 游戏开始时设置庄家
	 */	
	virtual void InitSetDealer();

	/**
	 *  @brief 游戏开始时根据配置填充城墙
	 */	
	virtual void FillWallTiles();

	/**
	 *  @brief 对结算信息的填充
	 *  @param[in/out]  pCmd 结算消息
	 *  
	 *  填充结算信息，包括番，牌，胡牌，连局信息等
	 */	
    virtual int FillFinishMsg(  TCMD_NOTIFY_FINISH *pCmd  );

	/**
	 *  @brief 发送结算信息
	 *  @param[in]  pCmd 结算消息
	 *  
	 *  把积分等信息写入DB，然后发送给客户端
	 */	
    virtual int SendSaveFinishMsg(  TCMD_NOTIFY_FINISH *pCmd, int nCmdLen  );


protected:
	/**
	 *  @brief 发送开局信息给C端
	 *  
	 *  把每个玩家的手牌等相关信息分别发送给每一个玩家
	 */	
	virtual void SendStartInfoToClient();

	/**
	 *  @brief 根据结束信息更新玩家的数据
	 *  @param[in]  cmd 结束的信息
	 *  @return  1表示成功，-1表示失败，0表示逃跑分取积分失败
	 *  
	 *  把每个玩家的手牌等相关信息分别发送给每一个玩家
	 */	
	virtual int SetScoreByFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

	/**
	 *  @brief 扣除门票
	 *  
	 *  执行扣除门票操作
	 */	
	virtual void PayTicket();

	/**
	 *  @brief 填充结束时的牌型
	 *  @param[in/out]  cmd 结束的信息
	 *  
	 *  把手牌,出牌等信息全部填充到结束信息里面去
	 */	
	virtual void FillTilesForFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

	/**
	 *  @brief 填充结束时的填充番型
	 *  @param[in/out]  cmd 结束的信息
	 *  @warning cmd的score代表的是每个人获得的番数,不是实际的积分
	 *  
	 *  游戏结束时的算番操作
	 */	
	virtual void FillFanForFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

	/**
	 *  @brief 把结束时的结果积分换算游戏币(或欢乐豆)
	 *  @param[in/out]  cmd 结束的信息
	 *  
	 *  根据cmd提供的每个玩家的score值来换算成配置的货币值.有最大输赢的限制
	 */	
	virtual void ScoreToMoneyForFinishMsg( TCMD_NOTIFY_FINISH *pCmd );
	
	/**
	 *  @brief 游戏结束时的算分
	 *  @param[in/out]  cmd 结束的信息
	 *  @warning 算分是基于cmd本身的score来计算的,算完后会覆盖本身的值
	 *  
	 *  根据cmd提供的每个玩家的score值来换算成积分
	 */	
	virtual void CountScoreForFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

	/**
	 *  @brief 配牌
	 *  @param[in]  nChair 要配牌的人
	 *  @return 返回配牌的结果,成功TRUE,失败:FALSE
	 *
	 *  随机配置部分特殊的牌,字一色:20%,清一色:30%,对对胡:%50
	 */	
	virtual int ConfigureTiles( int nChair );

	/**
	 *  @brief 配牌青一色
	 *  @param[in]  nChair 要配牌的人
	 *  @return 返回配牌的结果,成功TRUE,失败:FALSE
	 *
	 *  配8张同一颜色的牌到手牌
	 */	
	BOOL ConfigureQingyise( int nChair );

	/**
	 *  @brief 配牌四暗刻
	 *  @param[in]  nChair 要配牌的人
	 *  @return 返回配牌的结果,成功TRUE,失败:FALSE
	 *
	 *  配置2个刻,一个对子到手牌
	 */	
	BOOL ConfigureSianke( int nChair );

	/**
	 *  @brief 配牌字一色
	 *  @param[in]  nChair 要配牌的人
	 *  @return 返回配牌的结果,成功TRUE,失败:FALSE
	 *
	 *  配8张随机的字牌到手牌
	 */	
	BOOL ConfigureZiyise( int nChair );

	/**
	 *  @brief 发送连局信息
	 */	
	virtual void SendRunNbr();

	/**
	 *  @brief 配牌字一色
	 *  @param[out]  pCmd 填充部分信息
	 *
	 */	
	virtual int GetDealInfo( int nChair, TCMD_NOTIFY_DEAL* pCmd );

	/**
	 *  @brief 游戏结束时的算局，在算番和分之前被调用
	 *  @param[in]  pCmd 结束的信息
	 */	
	virtual void CountRunNbr( TCMD_NOTIFY_FINISH *pCmd );

	/**
	 *  @brief 游戏结束时的计算杠分，在结算输赢分之后被调用
	 *  @param[in]  pCmd 结束的信息
	 */	
    virtual void CountGangScoreForFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

protected:
     // 是否配牌了，如果配牌了则自动取消游戏开始时的数据初始化和庄家指定
     BOOL m_bReplaceWall;
     BOOL m_bReplaceDice;
     BOOL m_bReplaceDealer;

	 /********************/
	 /* GM需要换的牌集 */
	 /********************/
	 CMJHand m_TilesSetConfig[nPlayerNbr]; // 指定玩家接下来要摸的牌配置
	 CMJHand m_TilesNextConfig;            // 接下来要摸的牌配置
	 CMJHand m_TielsDiceConfig;            // 骰子配置
	 CMJHand m_TilesDealerConfig;          // 庄家配置

     // 是否有16局换庄的设定，默认没有
     BOOL m_bWindReroll;

     // 记录结算消息
     TCMD_NOTIFY_FINISH m_cmdFinish;

     // 记录是否使用了配牌道具，在游戏开始发牌的时候使用
     BOOL m_bConfigTiles[nPlayerNbr];

     // 记录是否补花了
     BOOL m_bCheckFlower[nPlayerNbr];

     // 发牌完成标志 
     BOOL m_bDealFinish[nPlayerNbr];  
};
		
#else

/**
 * @ingroup MJFrame
 * @brief CGameTileDealerMgr
 *
 * <p>　 
 *		CGameTileDealerMgr, 客户端下的城墙控制类
 * </p>
 * @sa CBaseTileDealerMgr
 *
 * 继承于CBaseTileDealerMgr
 */	
template<int nPlayerNbr> 
class CGameTileDealerMgr : public CBaseTileDealerMgr<nPlayerNbr>
{
public:
	CGameTileDealerMgr();
	virtual ~CGameTileDealerMgr();

	virtual void OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg);

	/**
	 *  @brief 处理开始的发牌消息和定义庄家等
	 */	
	virtual void OnNotifyDeal(UINT nLen, BYTE *pBuf);

	/**
	 *  @brief 处理摸牌消息
	 */	
	virtual void OnNotifyDraw(UINT nLen, BYTE *pBuf);

	/**
	 *  @brief 处理结束消息的逻辑部分
	 */	
	virtual void OnNotifyFinish(UINT nLen, BYTE *pBuf);

	/**
	 *  @brief 处理补花消息
	 */	
	virtual void OnNotifyFlower(UINT nLen, BYTE *pBuf);

	/**
	 *  @brief 处理换牌道具消息
	 */	
	virtual void OnNotifyChangeTiles( UINT nLen, BYTE *pBuf );

	/**
	 *  @brief 处理配牌道具消息
	 */	
	virtual void OnNotifyConfigTiles( UINT nLen, BYTE *pBuf );

	/**
	 *  @brief 处理设置连局信息
	 */	
	virtual void OnNotifyRunNbr( UINT nLen, BYTE *pBuf );

	/**
	 *  @brief 游戏开始时会执行的消息
	 */	
	virtual void  OnGameStart() {};
		
	/**
	 *  @brief 设置当前玩家座位
	 *  @param[in]  nTurn 当前正在操作的玩家
	 */	
	virtual void SetTurn( int nTurn );

	/**
	 *  @brief 获取SDK的道具管理接口
	 *  @return 道具管理类接口
	 *  @warning 城墙管理类本身并不会创建或抓取接口,需要重载
	 */	
	//virtual IItemManager* GetItemManager() { return NULL; }

	/**
	 *  @brief 游戏前补花时间到后执行的操作
	 */	
	virtual void OnTimeOutFlower() {};

	virtual BOOL IsLastDrawTileBack() { return m_bLastDrawBack; }
protected:
	// 是否是从后面摸的牌
	BOOL m_bLastDrawBack;

};

#endif

}

#include "BaseTileDealerMgr.inl"

#endif // !defined(AFX_BASETILEDEALERMGR_H__02CA6498_356A_4B09_A39F_2F0EFA10916B__INCLUDED_)
