/** @file	BaseTilePlayerMgr.h
 *	@brief	手牌管理类
 *	@author edward
 *	@date 08.08.04
 *
 *	手牌管理类，继承于规则类，实现了对手牌和出牌的所有管理
 *	CBaseTilePlayerMgr是基类，根据C端和SO端分别有不同的MGR
 */
#if !defined(AFX_BASETILEPLAYERMGR_H__E131B1E3_44E0_4A64_B7AC_BEA3D103C999__INCLUDED_)
#define AFX_BASETILEPLAYERMGR_H__E131B1E3_44E0_4A64_B7AC_BEA3D103C999__INCLUDED_
 
//#include "StdAfx.h"

#include "MJ.h"
#include "mjcfgdef.h"

#include "MJFrameBaseData.h"
#include "IMJTileEvent.h"
#include "IMJStateEvent.h"
#include "MJRule.h"
#include "Clog.h"

#ifndef WINDOWS_LOGIC
#include "SoUserManager_i.h"
#else
#include "gameprocess_i.h"
#include "usermanager_i.h"
#endif
 
namespace MyGame
{

/**
 * @ingroup MJFrame
 * @brief CBaseTilePlayerMgr
 *
 * <p>　 
 *		CBaseTilePlayerMgr, 手牌控制类的基类
 * </p>
 * @sa
 */	
template<int nPlayerNbr>
class CBaseTilePlayerMgr : public CMJRule
{
public:
	CBaseTilePlayerMgr();
	virtual ~CBaseTilePlayerMgr();

	/**
	 *  @brief 为下一局所做的初始化
	 *
	 *  在游戏结束时调用，清空拦牌状态和听牌状态
	 */	
	virtual void InitForNext();

	/**
	 *  @brief 设置状态事件接口
	 *  @param[in]  pStateEvent 状态事件的接口指针
	 *
	 *  为了能让手牌获取到一定的状态信息而提供的接口
	 */	
	virtual void SetStateEvent( IMJStateEvent* pStateEvent );

	/**
	 *  @brief 设置城墙事件接口
	 *  @param[in]  pTileEvent 城墙事件的接口指针
	 *
	 *  为了能让手牌获取到一定的城墙信息而提供的接口
	 */	
	virtual void SetTileEvent( IMJTileEvent* pTileEvent );

	/**
	 *  @brief 获取状态事件接口
	 */	
	IMJStateEvent* GetStateEvent() { return m_pStateEvent; }

	/**
	 *  @brief 拦牌状态时间到后处理事件
	 *  @param[in]  bAutoWin 是否强制胡牌
	 */	
	virtual void OnTimeOutBlock( BOOL bAutoWin = TRUE ){};

	/**
	 *  @brief 出牌状态时间到后处理事件
	 */	
	virtual void OnTimeOutGive(){};

	/**
	 *  @brief 设置手牌
	 *  @param[in]  nChair 设置的玩家
	 *  @param[in]  mjHand 设置的手牌
	 *  @param[in]  bNew 是否清楚以前的手牌数据
	 */	
	virtual void SetHandTile( int nChair, CMJHand &mjHand, BOOL bNew = TRUE );

	/**
	 *  @brief 设置拦牌
	 *  @param[in]  nChair 设置的玩家
	 *  @param[in]  mjSet 设置的拦牌
	 *  @param[in]  bNew 是否清楚以前的拦牌数据
	 */	
	virtual void SetSetTile( int nChair, CMJSet &mjSet, BOOL bNew = TRUE );

	/**
	 *  @brief 添加一张手牌
	 *  @param[in]  nChair 要添加的玩家
	 *  @param[in]  ucTile 添加的牌
	 */	
	virtual void AddHandTile( int nChair, TILE ucTile );

	/**
	 *  @brief 删除一张手牌
	 *  @param[in]  nChair 要删除的玩家
	 *  @param[in]  ucTile 删除的牌
	 *  @return  返回删除的结果成功:TRUE,失败:FALSE
	 */	
	virtual BOOL DelHandTile( int nChair, TILE ucTile );

	/**
	 *  @brief 设置玩家听牌
	 *  @param[in]  nChair 要设置的玩家
	 *  @param[in]  bTing 是否听牌
	 */	
	virtual void SetTing( int nChair, BOOL bTing = TRUE );

	/**
	 *  @brief 获取玩家是否听牌
	 *  @param[in]  nChair 要获取的玩家
	 *  @return  返回是否听牌,是:TRUE,否:FALSE
	 */	
	virtual BOOL GetTing( int nChair );

	/**
	 *  @brief 判断是否需要补花
	 *  @param[in]  nChair 判断的玩家
	 *  @return  返回花的数量
	 */	
	virtual int CheckFlower(  int nChair );


	/**
	 *  @brief 判断拦牌
	 *  @param[in]  nChair 判断的玩家
	 *  @param[in]  bSelf 是否只判断自己
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @param[in]  shLevel 设定拦牌等级，低于等级的也不认为可以拦牌
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlock(  int nChair, BOOL bSelf = TRUE, TILE ucLastTile = TILE_CHAR_1, short shLevel = 1 );
	
	/**
	 *  @brief 判断拦牌自摸
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockSelfWin(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );

	/**
	 *  @brief 判断拦牌自己杠
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockSelfQuadruplet(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );
	
	/**
	 *  @brief 判断拦牌自己听
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockTing(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );

	/**
	 *  @brief 判断拦牌吃牌
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockCollect(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1, int nGiveChair = 0 );
	
	/**
	 *  @brief 判断拦牌碰
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockTriplet(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );

	/**
	 *  @brief 判断拦牌杠别人
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockQuadruplet(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );

	/**
	 *  @brief 判断拦牌胡别人
	 *  @param[in]  nChair 判断的玩家
	 *  @param[out] shBlockLevel 如果成立，并是最高等级，会返回此等级
	 *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
	 *  @return  返回拦牌等级
	 */	
	virtual short CheckBlockWinOther(  int nChair, short &shBlockLevel, TILE ucLastTile = TILE_CHAR_1 );

	/**
	 *  @brief 判断听牌
	 *  @param[in]  nChair 判断的玩家
	 *  @return  返回是否可以听牌,是:TRUE,否:FALSE
	 */	
	virtual BOOL CheckTing( int nChair );

	/**
	 *  @brief 获取手牌
	 *  @param[in]  nChair 被获取的玩家
	 *  @param[out]  mjHand 手牌设置到这个引用参数里面
	 *  @return  手牌的长度
	 */	
	virtual int GetTilesHand( int nChair, CMJHand& mjHand );

	/**
	 *  @brief 获取拦牌
	 *  @param[in]  nChair 被获取的玩家
	 *  @param[out]  mjHand 拦牌设置到这个引用参数里面
	 *  @return  拦牌的长度
	 */	
	virtual int GetTilesSet( int nChair, CMJSet& mjSet );

	/**
	 *  @brief 获取出牌
	 *  @param[in]  nChair 被获取的玩家
	 *  @param[out]  mjHand 出牌设置到这个引用参数里面
	 *  @return  出牌的长度
	 */	
	virtual int GetTilesGive( int nChair, CMJGive& mjGive );

	/**
	 *  @brief 获取是否是显示手牌（明牌）
	 *  @param[in]  nChair 被获取的玩家
	 *  @return  明牌:TRUE,暗牌:FALSE
	 */	
	virtual BOOL GetShowTile( int nChair );

	/**
	 *  @brief 设置当前的最后一张牌
	 *  @param[in]  ucTile 最后一张牌
	 *
	 *  这张牌可能是最后摸的或者最后打出去的
	 */	
	void SetLastTile( TILE ucTile );

	/**
	 *  @brief 获取当前的最后一张牌
	 *  @return  最后一张牌
	 *
	 *  这张牌可能是最后摸的或者最后打出去的
	 */	
	TILE GetLastTile() { return m_ucLastTile; }

	/**
	 *  @brief 清空漏胡数据
	 *  @param[in]  nChair 被清空的玩家
	 */	
	virtual void ClearLoseHu( int nChair );

	/**
	 *  @brief 设置成为漏胡的牌
	 *  @param[in]  nChair 设置的玩家
	 *  @param[in]  ucTile 记录漏胡的牌
	 *  @warning  漏胡存储的数据有只有17张,超过17张将不能被记录
	 */	
	virtual void SetLoseHu( int nChair, TILE ucTile );

	/**
	 *  @brief 判断是否为漏胡记录的牌
	 *  @param[in]  nChair 检查的玩家
	 *  @param[in]  ucTile 被检查的牌
	 *  @return  存在漏胡数据中:TRUE,否则:FALSE
	 */	
	virtual BOOL CheckLoseHu( int nChair, TILE ucTile );

	/**
	 *  @brief 获取当前所有杠牌的数量
	 *  @return  所有杠牌的数量
	 */	
	virtual int GetAllGangNbr() { return m_nAllGangNbr; }

	/**
	 *  @brief 排序
	 *  @param[in]  mjHand 需要排序的牌组
	 *  @param[in]  nMod 0:升序,1:降序
	 *  @param[in]  nGod 0:万能牌不做特殊排序处理,1:万能牌放在序列的头部,其他:万能牌放在序列的尾部
	 *  @param[in]  ptilesGod 万能牌组
	 */	
	virtual void SortHandTiles( CMJHand &mjHand, int nMod = 0, int nGod = 0, CMJHand* ptilesGod = NULL );
	
	/**
	 *  @brief 判断是否是最后自动胡的牌
	 *  @return 如果是自动胡的牌TRUE，否则FALSE
	 */	
	virtual BOOL CheckIsAutoWinTile();

	/**
	 *  @brief 获取从城墙后面画的数量
	 *  @return  需要从后面画的数量
	 */	
	virtual int GetDrawWallBackNbr();

	/**
	 *  @brief 设置最后出牌的人
	 *  @param[in]  byChair 出牌的人
	 */	
	virtual void SetLastGiveChair(BYTE byChair);
	
	/**
	 *  @brief 获取最后出牌的人
	 *  @return 返回最后出牌的人
	 */	
	virtual BYTE GetLastGiveChair();

protected:
	// 2个向下访问的接口
	IMJStateEvent*  m_pStateEvent;
	IMJTileEvent*	m_pTileEvent;


	// 手牌和出牌数据
	CMJHand m_TilesHand[nPlayerNbr];
	CMJSet  m_TilesSet[nPlayerNbr];
	CMJGive m_TilesGive[nPlayerNbr];

	// 听牌的类型
	int		m_nTingType[nPlayerNbr];
	// 听的牌，能打出去的
	CMJHand	m_TilesTingGive;
	// 听牌打出去后听的牌
	CMJHand m_TilesTingGiveWin[MAX_HAND_TILES];

	// 听的牌所能听的牌
	// 是否是明牌
	BOOL	m_bShowTile[nPlayerNbr];

	// 拦牌的状态，每次判断前都会清空
	BOOL	m_bBlockEnable[MJ_BLOCK_NBR];

	// 每个玩家的拦牌等级，在CHECKBLOCK中清空
	int		m_nBlockLevel[nPlayerNbr];

	// 杠牌的状态
	short   m_shGangState;

	// 杠牌的数量
	int m_nAllGangNbr;

	// 最后的牌
	TILE m_ucLastTile;

	// 胡了的人
	BOOL m_bWiner[nPlayerNbr];

	// 漏胡
	TILE m_ucLoseHu[nPlayerNbr][MAX_HAND_TILES];

	// 万能牌的替代
	CMJHand m_TilesPlayerGod[nPlayerNbr];

	// 最后一个出牌的人
	BYTE m_byLastChair;

	/**
	 *  @brief 当前进行拦牌判断的玩家，在CHECKBLOCK，SWITCHBLOCK等时候被设置，以便知晓是谁正在被判断
	 */	
	BYTE m_ucCurCheckChair;

	// 记录杠牌用的
	int  m_nOldGangNbr;

	// dxh add 记录吃碰杠标志  
	BOOL m_bIsSet;
};


#ifndef WINDOWS_LOGIC

/**
 * @ingroup MJFrame
 * @brief CSoTilePlayerMgr
 *
 * <p>　 
 *		CSoTilePlayerMgr, SO手牌控制类的基类
 * </p>
 * @sa
 */	
template<int nPlayerNbr>
class CSoTilePlayerMgr : public CBaseTilePlayerMgr<PLAYER_NBR>
{
public:
	CSoTilePlayerMgr();
	virtual ~CSoTilePlayerMgr();

	virtual int  OnGameMessage(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

	/**
	 *  @brief 获取ISoGameProcessor接口
	 *  @return  ISoGameProcessor接口指针
	 *  @warning 此函数本身直接返回NULL,必须要Logic层进行重载
	 */	
	virtual ISoGameProcessor* GetSoGameProcess() { return NULL; }

	/**
	 *  @brief 获取ISoUserManager接口
	 *  @return  ISoUserManager接口指针
	 *  @warning 此函数本身直接返回NULL,必须要Logic层进行重载
	 */	
	virtual ISoUserManager* GetSoUserManager() { return NULL; }
	
    
    /**
    *  @brief 判断拦牌
    *  @param[in]  nChair 判断的玩家
    *  @param[in]  bSelf 是否只判断自己
    *  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
    *  @param[in]  shLevel 设定拦牌等级，低于等级的也不认为可以拦牌
    *  @return  返回拦牌等级
    */	
    virtual short CheckBlock(  int nChair, BOOL bSelf = TRUE, TILE ucLastTile = TILE_CHAR_1, short shLevel = 1 );

  	/**
	 *  @brief 拦牌的执行
	 *  @param[in]  nChair 出牌的人
	 *  @param[in]  ucTile 出的牌
	 *
	 *  如果最后一个参数bTimeOut为TRUE,执行强制操作,前面的参数没有价值
	 */	
	virtual int CheckGiveBlock( int nChair, BYTE ucTile );

 	/**
	 *  @brief 处理请求拦牌信息
	 *  @warning 处理的时候,不一定是在处理发送消息的人的请求
	 */	
	virtual int ProcessRequestBlock(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);
	
  	/**
	 *  @brief 请求补花
	 */	
	virtual int ProcessRequestFlower(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

    /**
    *  @brief 处理请求出牌信息
    */	
    virtual int ProcessRequestGive(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen);

  	/**
	 *  @brief 拦牌时间到后的事件处理
	 *  @param[in]  bAutoWin 是否强制胡牌
	 */	
	virtual void OnTimeOutBlock( BOOL bAutoWin = TRUE );

  	/**
	 *  @brief 出牌时间到后的事件处理
	 *
	 *  SO执行此事件的时候,会强制刷新出牌玩家的手牌数据
	 */	
	virtual void OnTimeOutGive();

  	/**
	 *  @brief 游戏开始的事件处理
	 */	
	virtual int OnGameStart();

  	/**
	 *  @brief 拦牌的执行
	 *  @param[in]  nChair 请求拦牌的玩家
	 *  @param[in]  ucFlag 请求拦牌的标记(吃,碰,杠,听,胡,放弃)
	 *  @param[in]  ucTile 请求拦牌的牌数据
	 *  @param[in]  bTimeOut 是否强制全部放弃
	 *
	 *  如果最后一个参数bTimeOut为TRUE,执行强制操作,前面的参数没有价值
	 */	
	virtual void SwitchBlock( int nChair, BYTE ucFlag, BYTE ucTile[4], BOOL bTimeOut = FALSE );

  	/**
	 *  @brief 拦牌的执行吃
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockCollect( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行碰
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockTriplet( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行暗杠
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockQuadrupletConcealed( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行明杠
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockQuadrupletRevealed( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行胡牌
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockWin( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行后的状态修改
	 *  @param[in]  nChair 请求拦牌的玩家
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockOver( int nChair, TCMD_NOTIFY_BLOCK_RESULT &cmd );
	
	/**
	 *  @brief 刷新玩家手牌
	 *  @param[in]  nChair 被刷新的玩家
	 *
	 *  当SO发现玩家客户端数据出现异步,执行此函数发送刷新消息刷新玩家的手牌
	 */	
	virtual void UpdateHandTiles( int nChair );

  	/**
	 *  @brief 获取玩家的游戏币(或欢乐豆)
	 *  @param[in]  nChair 请求获取的玩家
	 *  @return  玩家的游戏币(或欢乐豆)
	 *
	 *  根据配置决定是返回欢乐豆还是游戏币
	 */	
	virtual int GetMJUserMoney( int nChair );

    /**
    *  @brief 获取拦牌信息
    *  @param[in]  nChair 被获取的玩家
    *  @return TCMD_NOTIFY_BLOCK 结构
    */	
    virtual TCMD_NOTIFY_BLOCK GetBlockInfo( int nChair );

  	/**
	 *  @brief 要求C显示提示对话框
	 *  @param[in]  pMsg 显示的内容
	 *  @param[in]  nLen 内容的长度
	 *  @param[in]  nMsgID 消息的ID(目前默认为0)
	 *  @param[in]  nPlayerID 是否需要对指定玩家发送消息(如果为-1就全部发送)
	 *  @param[in]  byFlag YESNO或者OK类型
	 */	
	virtual void ShowDlgMsg(char* pMsg, int nLen, int nMsgID = 0, short nPlayerID = -1, BYTE byFlag = 0);
	
  	/**
	 *  @brief 获取是否断线
	 *  @param[in]  ucChair 请求获取的玩家
	 *  @return  如果断线返回TRUE,否则返回FALSE
	 */	
	BOOL IsOffline(BYTE ucChair);

	
  	/**
	 *  @brief SO处理托管玩家事情
	 *  @param[in]  ucChair 处理托管的玩家
	 *  @param[in]  shPlayerID 玩家的ID
	 */	
	virtual void SoTrustOffline(BYTE ucChair, short shPlayerID);

	/**
	 *  @brief SO处理托管玩家事情出牌
	 *  @param[in]  ucChair 处理托管的玩家
	 *  @param[in]  shPlayerID 玩家的ID
	 */	
	virtual void SoTrustOfflineGive(BYTE ucChair, short shPlayerID);

	/**
	 *  @brief SO处理托管玩家事情拦牌
	 *  @param[in]  ucChair 处理托管的玩家
	 *  @param[in]  shPlayerID 玩家的ID
	 */	
	virtual void SoTrustOfflineBlock(BYTE ucChair, short shPlayerID);

	/**
	 *  @brief SO处理托管玩家事情补花
	 *  @param[in]  ucChair 处理托管的玩家
	 *  @param[in]  shPlayerID 玩家的ID
	 */	
	virtual void SoTrustOfflineFlower(BYTE ucChair, short shPlayerID);

	/**
	 *  @brief 设置玩家听牌
	 *  @param[in]  nChair 要设置的玩家
	 *  @param[in]  bTing 是否听牌
	 */	
	virtual void SetTing( int nChair, BOOL bTing = TRUE );

   	/**
	 *  @brief 拦牌的执行获取玩家的请求等级
	 *  @param[in]  nChair 请求拦牌的玩家
	 *  @param[in]  ucFlag 请求的类型
	 *  @param[in]  ucTile 请求的牌
	 */	
	virtual int GetRequestLevel( int nChair, BYTE ucFlag, BYTE ucTile );

   	/**
	 *  @brief 拦牌的判断是否需要等待其他人请求
	 *  @param[in]  nChair 请求拦牌的玩家
	 *  @param[in]  ucFlag 请求的类型
	 *  @param[in]  ucTile 请求的牌
	 *  @param[out]  nMaxLevel 请求的等级
	 */	
	virtual int IsWaitOtherRequest( int nChair, BYTE ucFlag, BYTE ucTile, int &nMaxLevel );

   	/**
	 *  @brief 拦牌的执行所有请求操作
	 *  @param[in]  nMaxLevel 请求的等级
	 *  @param[out]  cmd 执行后的结果
	 */	
	virtual int SwitchBlockProcess( int nMaxLevel, TCMD_NOTIFY_BLOCK_RESULT &cmd );
	
   	/**
	 *  @brief 拦牌的执行每一个请求操作
	 *  @param[in]  nNextChair 请求的玩家
	 *  @param[in]  ucRequestTile 请求的牌
	 *  @param[out]  cmd 执行后的结果
	 */	
	virtual int SwitchBlockProcessOne( int nNextChair, BYTE ucRequestTile[4], TCMD_NOTIFY_BLOCK_RESULT &cmd );

   	/**
	 *  @brief 拦牌的执行听牌
	 *  @param[in]  nNextChair 请求拦牌的玩家
	 *  @param[in]  nNextChair 请求拦牌的牌(不一定是最后一张)
	 *  @param[in]  cmd 拦牌的信息
	 */	
	virtual int SwitchBlockTing( int nChair, BYTE ucFlag, BYTE ucTile );

  	/**
	 *  @brief 摸牌后栏牌的判断
	 *  @param[in]  nChair 摸牌的人
	 */	
	virtual int CheckDrawBlock( int nChair );

	virtual int GetDrawWallBackNbr() { return GetAllGangNbr(); };

protected:
	// 玩家请求拦牌的
	int m_nBlockRequest[nPlayerNbr];
	// 拦牌数据
	TILE m_ucRequestTile[nPlayerNbr][4];
    // 听牌记录
    BOOL m_bTing[nPlayerNbr];
	// 花牌数量，花牌的记录方式是，记录每张花牌
	BYTE m_nFlower[MJ_FLOWER_NBR];
	// 结束的类型
	BYTE m_ucEndType;
};
#else

/**
 * @ingroup MJFrame
 * @brief CGameTilePlayerMgr
 *
 * <p>　 
 *		CGameTilePlayerMgr, 客户端手牌控制类的基类
 * </p>
 * @sa
 */	
template<int nPlayerNbr>
class CGameTilePlayerMgr : public CBaseTilePlayerMgr<nPlayerNbr>
{
public:
	CGameTilePlayerMgr();
	virtual ~CGameTilePlayerMgr();

  	/**
	 *  @brief 获取角色管理接口
	 *  @return  角色管理接口
	 *  @warning 此函数必须由外部继承并重载，本身只返回NULL
	 */	
	virtual IUserManager* GetSoUserManager() { return NULL; }

  	/**
	 *  @brief 获取UI管理接口
	 *  @return  UI管理接口
	 *  @warning 此函数必须由外部继承并重载，本身只返回NULL
	 */	
	//virtual IUIManager* GetUIManager() { return NULL; }


  	/**
	 *  @brief 刷新所有麻将数据
     *  @param[in]  bEnforce 强制刷新
	 *  在逻辑部分，会在更新麻将控件数据后，调用此函数来刷新UI表现，需要重载
	 */	
	virtual void RefreshMJCtrl(BOOL bEnforce = FALSE, BOOL bAllRefres = FALSE) = 0;

	
  	/**
	 *  @brief 获取是否是逆时针出牌
	 *  @return  如果是逆时针，返回TRUE，否则返回FALSE
	 */	
	virtual BOOL GetAntiClockWise() { return TRUE; }

  	/**
	 *  @brief 获取IGameProcess接口
	 *  @return  IGameProcess接口
	 *  @warning 此函数必须由外部继承并重载，本身只返回NULL
	 */	
	virtual IGameProcess* GetGameProcess() { return NULL; }

	virtual void  OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg);

  	/**
	 *  @brief 游戏开始时执行的事件
	 */	
	virtual void  OnGameStart();

  	/**
	 *  @brief 接受到补花消息后执行的事件
	 */	
	virtual void OnNotifyFlower(UINT nLen, BYTE *pBuf);

	/**
	 *  @brief 接受到全部补花完毕消息后执行的事件
	 *
	 *  此消息标志着SO离开了游戏前补花阶段而进入游戏中的出牌或拦牌阶段
	 */	
	virtual void OnNotifyFlowerOver(UINT nLen, BYTE *pBuf);


  	/**
	 *  @brief 接受到请求拦牌结果消息后执行的事件
	 */	
	virtual void OnNotifyBlock(UINT nLen, BYTE *pBuf);

   	/**
	 *  @brief 接受到请求拦牌结果消息后执行的吃牌事件
	 *  @param[in]  pCmd 拦牌消息
	 *  @param[in]  nOldTurn 请求时的操作玩家
	 */	
	virtual void OnNotifyBlockCollect( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn );

   	/**
	 *  @brief 接受到请求拦牌结果消息后执行的碰牌事件
	 *  @param[in]  pCmd 拦牌消息
	 *  @param[in]  nOldTurn 请求时的操作玩家
	 */	
	virtual void OnNotifyBlockTriplet( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn );

   	/**
	 *  @brief 接受到请求拦牌结果消息后执行的明杠事件
	 *  @param[in]  pCmd 拦牌消息
	 *  @param[in]  nOldTurn 请求时的操作玩家
	 */	
	virtual void OnNotifyBlockQuadrupletRevealed( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn );

   	/**
	 *  @brief 接受到请求拦牌结果消息后执行的暗杠事件
	 *  @param[in]  pCmd 拦牌消息
	 *  @param[in]  nOldTurn 请求时的操作玩家
	 */	
	virtual void OnNotifyBlockQuadrupletConcealed( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn );

   	/**
	 *  @brief 接受到请求拦牌结果消息后执行的停牌事件
	 *  @param[in]  pCmd 拦牌消息
	 *  @param[in]  nOldTurn 请求时的操作玩家
	 */	
	virtual void OnNotifyBlockTing( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn );

	/**
	 *  @brief 接受到请求出牌结果消息后执行的事件
	 */	
	virtual void OnNotifyGive(UINT nLen, BYTE *pBuf);

  	/**
	 *  @brief 接受到发牌消息后执行的事件
	 */	
	virtual void OnNotifyDraw(UINT nLen, BYTE *pBuf);

  	/**
	 *  @brief 接受到开局发牌消息后执行的事件
	 */	
	virtual void OnNotifyDeal(UINT nLen, BYTE *pBuf);

  	/**
	 *  @brief 接受到刷新手牌消息后执行的事件
	 */	
	virtual void OnNotifyUpdateHandTiles( UINT nLen, BYTE *pBuf );

  	/**
	 *  @brief 接受到修改旁观状态消息后执行的事件
	 */	
	virtual void OnNotifyChangeLookOn( UINT nLen, BYTE *pBuf );

  	/**
	 *  @brief 接受出牌
	 *  @param[in]  ucTile 请求要出的牌
	 *  @param[in]  nSel   出的第几张位置的牌
	 *
	 *  本地会在发送请求消息前，表现自己打出这张牌
	 */	
	virtual void RequestGive( TILE ucTile, int nSel );

  	/**
	 *  @brief 请求拦牌
	 *  @param[in]  ucFlag 请求拦牌的标记
	 *  @param[in]  ucTile 请求要拦的牌
	 */	
	virtual void RequestBlock( BYTE ucFlag, TILE ucTile );

  	/**
	 *  @brief 请求补花
	 *  @param[in]  nChair 请求的玩家
	 */	
	virtual void RequestFlower( int nChair );

  	/**
	 *  @brief 设置所有手牌的选择状态
	 *  @param[in]  bCan 是否可选
	 *
	 *  如果不可选，手牌会灰化
	 */	
	virtual void SetAllCanSelect( BOOL bCan = TRUE );

  	/**
	 *  @brief 设置一组手牌的选择状态
	 *  @param[in]  pTilesSelect 手牌的数组
	 *  @param[in]  nLength 手牌的数组的长度
	 */	
	virtual void SetTilesSelect( int* pTilesSelect, int nLength );

  	/**
	 *  @brief 设置一张手牌的选择状态
	 *  @param[in]  nIndex 手牌的序列号
	 *  @param[in]  nSelect 手牌的选择状态0可选，1不可选，2已经被选择
	 */	
	virtual void SetTileSelect( int nIndex, int nSelect );

  	/**
	 *  @brief 设置可选的牌
	 *  @return 如果有被选的牌放下，返回TRUE，否则返回FALSE
	 *
	 *  设置可选的牌，全部处于可选状态，被选择的牌放下
	 */	
	virtual BOOL SetCanBeSelect();

  	/**
	 *  @brief 获取哪张牌被选择了
	 *  @return 选择牌的序列号
	 */	
	virtual int  GetCanBeSelect();

  	/**
	 *  @brief 获取牌的选择信息
	 *  @param[in]  nIndex 手牌的序列号
	 *  @return 选择牌的状态
	 */	
	virtual int  GetTileSelect( int nIndex );

  	/**
	 *  @brief 获取当前手牌的选择状态
	 *  @return 手牌的选择状态
	 *  @as MJ_STATE_SELECT_NONE
	 */	
	virtual int  GetStateSelect() { return m_nStateSelect; }

  	/**
	 *  @brief 设置当前手牌的选择状态
	 *  @param[in]  nState 手牌的选择状态
	 *  @as MJ_STATE_SELECT_NONE
	 */	
	virtual void SetStateSelect( int nState );

  	/**
	 *  @brief 获取自己的手牌
	 *  @param[out]  mjHand 手牌的数据
	 */	
	virtual void GetLocalMJHand( CMJHand& mjHand );

  	/**
	 *  @brief 获取自己的拦牌
	 *  @param[out]  mjSet 拦牌的数据
	 */	
	virtual void GetLocalMJSet( CMJSet& mjSet );

  	/**
	 *  @brief 获取自己的出牌
	 *  @param[out]  mjGive 出牌的数据
	 */	
	virtual void GetLocalMJGive( CMJGive& mjGive );

  	/**
	 *  @brief 获取自己的位置ID
	 *  @return 自己的位置号
	 */	
	virtual int  GetLocalChair();

  	/**
	 *  @brief 获取自己请求出的牌
	 *  @return 自己请求出的牌
	 */	
	virtual TILE GetLocalGive() { return m_ucLocalGive; }

  	/**
	 *  @brief 设置自己请求出的牌
	 *  @param[in]  ucTIle 请求出的牌
	 */	
	virtual void SetLocalGive( TILE ucTIle );

  	/**
	 *  @brief 设置拦牌的状态
	 *  @param[in]  nIndex 拦牌的序列号(0~4,吃碰杠听胡)
	 *  @param[in]  bEnble 是否可选
	 */	
	virtual void SetBlockEnable( int nIndex, BOOL bEnble );

  	/**
	 *  @brief 获取拦牌的状态
	 *  @return 拦牌的序列号
	 */	
	virtual BOOL GetBlockEnable( int nIndex );

  	/**
	 *  @brief 获取显示位置
	 *  @return 显示的方位
	 */	
	virtual int GetShowTurn() { return 0; }

  	/**
	 *  @brief 设置显示位置
	 *  @param[in]  nTurn 显示的方位
	 */	
	virtual void SetShowTurn( int nTurn ) {};

  	/**
	 *  @brief 拦牌时间到了后执行的事件
	 *  @param[in]  bAutoWin 是否自动胡牌
	 */	
	virtual void OnTimeOutBlock( BOOL bAutoWin = TRUE );

  	/**
	 *  @brief 出牌时间到了后执行的事件
	 */	
	virtual void OnTimeOutGive();
	

  	/**
	 *  @brief 清空玩家相关牌信息
	 *  @param[in]  ucChair 玩家ID
	 */	
	void ClearPlayerTiles( BYTE ucChair );

  	/**
	 *  @brief 刷新所有杠牌积分
	 */	
	virtual void RefreshGangScore();

  	/**
	 *  @brief 获取杠分
	 *  @param[in]  ucChair 玩家ID
	 *  @return  杠牌的得分
	 */	
	int GetGangScore( BYTE ucChair );

  	/**
	 *  @brief 本地是否是玩家
	 *  @return  如果是玩家返回TRUE，旁观者返回FALSE
	 */	
	BOOL CheckIsPlayer();

  	/**
	 *  @brief 获取是否可以旁观手牌
	 *  @return  如果是返回TRUE，否则返回FALSE
	 */	
	BOOL GetBLookOn() { return m_bAllowLookOn; }
	
	/**
	 *  @brief 设置选牌状态为选择吃牌
	 */	
	virtual void OnSetSelectStateColloect();

	/**
	 *  @brief 设置选牌状态为选择杠牌
	 */	
	virtual void OnSetSelectStateQuadruplet();

	/**
	 *  @brief 设置选牌状态为选择听牌
	 */	
	virtual void OnSetSelectStateTing();

	/**
	 *  @brief 设置选牌状态为选择出牌
	 */	
	virtual void OnSetSelectStateGive();

	/**
	 *  @brief 获取打出牌的听牌信息
	 *  @param[in]  ucSelTile 打出的牌
	 *  @param[out]  mjTilesTing	听的牌
	 *  @return   是否可以听
	 *
	 *  听牌最多只记录了18张，对于有些情况可以胡34张的，这里没记录这么多
	 */	
	virtual BOOL GetTingGiveWinTiles( TILE ucSelTile, CMJHand &mjTilesTing );

    // 毫秒级定时器
    virtual void OnMSTimer(int nTimerID);

protected:
	// 0 都可以选择，1 不能选择 2 已经被选
	int m_nTileSelect[MAX_HAND_TILES];

	// 玩家手牌操作的状态
	int m_nStateSelect;

	// 本地出的牌
	// 首先表现的牌，一直到接受到出牌消息才会归0
	TILE m_ucLocalGive;

	int m_nGangScore[nPlayerNbr];
	BOOL m_bAllowLookOn;
};

#endif

}

#include "BaseTilePlayerMgr.inl"

#endif // !defined(AFX_BASETILEPLAYERMGR_H__E131B1E3_44E0_4A64_B7AC_BEA3D103C999__INCLUDED_)
