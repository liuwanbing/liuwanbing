#ifndef _MJ_FRAME_LOGIC_H_
#define _MJ_FRAME_LOGIC_H_

// 麻将客户端框架逻辑类
#include "IGameImpl.h"
#include "Common/UIMessageDefines.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "MJControlsDefine.h"
#include "MJSoundDefines.h"
#include "IGameFrame.h"
#include "Gameroommessage.h"
#include "commonuse.h"
#include "basemessage.h"
#include "XFanCounter.h"
#include "irrKlang/irrKlang.h"
using namespace irrklang;

#define GETCTRL(CTRL_TYPE, PRE, pUI, CTRL_ID) \
	(( PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID)) ) != NULL) \

#define SAFE_CTRL_OPERATE(CTRL_TYPE, CTRL_ID, OPERATE) \
{\
	CTRL_TYPE* PRE = NULL; \
	PRE = dynamic_cast<CTRL_TYPE *>(m_pUI->GetIControlbyID(CTRL_ID));\
	if (PRE != NULL) PRE->OPERATE; \
}

/// char类型转wchar_t类型
static void C2W(char *pchar, wchar_t * wchar)
{
	int nwLen = MultiByteToWideChar(CP_ACP, 0, pchar, -1, NULL, 0); 
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

/// wchar_t类型转char类型
static void W2C(wchar_t * wchar, char *pchar)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, (int)wcslen(wchar), NULL, 0,NULL,NULL);

	// 宽字节编码转换成多字节编码   
	WideCharToMultiByte(CP_ACP, 0, wchar, (int)wcslen(wchar), pchar, len, NULL, NULL);
}

// 平台UI消息事件(不可修改此ID)
#define  UI_TIME_RUN     316     /// 时间控件发送消息，用来播放倒计时声音

// 平台定时器ID定义范围10-40
#define TIME_FRAME_MAX_VALUE 40  /// 平台定时器ID最大值
#define TIME_DEAL            10  /// 发牌定时器
#define TIME_DICE_FINISH     11  /// 骰子完成定时器
#define TIME_FIND_POS        12  /// 查找位置定时器
#define TIME_HAND_MOVE       13  /// 小手指移动计时器
#define TIME_SHOW_FINISH     14  /// 显示结算

/** 
 * @brief 拦牌信息数据结构 
 */
struct ST_BLOCKINFO
{
	BYTE    byFlag;          /**< 拦牌标识: 吃(0x01), 碰(0x02) 杠(0x04), 听(0x08), 胡(0x10) */
	CMJSet  tilesSet;        /**< 拦牌数据: 吃、碰、杠 */
	CMJHand tingGive;        /**< 听牌可以后出去的牌 */
	CMJHand tingWin[20];     /**< 听牌打出对应那张牌后可以胡哪几张牌, 每个下标与 mjTilesTingGive对象每个键值对应*/
	
	void ReleaseAll()
	{
		memset(this, 0, sizeof(ST_BLOCKINFO));
	}   
};

/** 
 * @brief 骰子状态定义
 */
enum ENM_DICE_STATE
{
	En_Dice_None = 0,        /**< 没有状态 */
	En_Dice_1,               /**< 第1次骰子, 确定下一位打骰子的玩家 */
	En_Dice_2,               /**< 第2次骰子, 确定抓牌位置 */
	En_Dice_3,               /**< 第3次骰子, 确定精牌的位置 */
	En_Dice_4,               /**< 第4次骰子, 杠牌骰子 */
};

/** 
 * @brief 声音结构定义
 */
#define MAX_SOUND_RAND 5    // 有几种不一样的声音

struct SOUND_INFO
{
	char szFileName[MAX_SOUND_RAND][MAX_PATH];
	int nCount;       

	SOUND_INFO()
	{
		memset(szFileName, 0, sizeof(szFileName));
		nCount = 0;
	}
	SOUND_INFO(const SOUND_INFO &other)
	{
		memcpy(szFileName, other.szFileName, sizeof(SOUND_INFO));
		nCount = other.nCount;
	}
	SOUND_INFO &operator = (const SOUND_INFO &other)
	{
		if (&other==this)
		{
			return *this;
		}
		memcpy(szFileName, other.szFileName, sizeof(SOUND_INFO));
		nCount = other.nCount;
		return *this;
	}
};

interface IGameFrame;
interface IUserInterface;
class CMJFrameLogic : public IGameImpl
{
public:
    CMJFrameLogic(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CMJFrameLogic();

private:
    CMJFrameLogic(const CMJFrameLogic &other);
    CMJFrameLogic &operator = (const CMJFrameLogic &other);

public:
    /** 
	 * @brief 初始化
	 */
    virtual int Initial();

	/** 
	* @brief 初始化UI
	*/
    virtual int InitUI();

	/** 
	 * @brief 自定义初使化数据
	 */
	virtual void InitData();


	/** 
	 * @brief 游戏消息
	 * @param [in] nMainID 游戏主ID
	 * @param [in] nSubId  游戏次ID
	 * @param [in] buffer  游戏数据包
	 * @param [in] nLen    游戏数据包的长度
	 * @return 
	 */
    virtual int HandleGameMessage(WORD nMainID, WORD nSubId, BYTE * buffer, int nLen);

	/** 
	 * @brief 界面消息
	 * @param [in] pMessage 响应界面操作的控件信息
	 */
    virtual int OnUIMessage(TUIMessage* pMessage);
    
	/** 
	 * @brief 重置游戏
	 */
    virtual int ResetGameFrame(void);

	/** 
	 * @brief 玩家进入
	 */
    virtual int GameUserCome(void);

	/** 
	 * @brief 玩家进入
	 * @param [in] nIndex 游戏桌子位置
	 */
	virtual int GameUserCome(int nIndex);

	/** 
	 * @brief 玩家离开
	 */
    virtual int GameUserLeft(int nIndex);

	/** 
	 * @brief 玩家断线
	 */
    virtual int GameUserOffLine(void);

	/** 
	 * @brief 是否正在游戏，由游戏实现
	 * @return 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
	 */
    virtual bool IsPlayingGame();   

	/** 
	 * @brief 定时器消息
	 * @param [in] nTimerID 定时器ID
	 */
    virtual void OnTimer(int nTimerID);

	/** 
	 * @brief 设置状态
	 * @param [in] bGameStation 游戏状态
	 */
    virtual void SetStationParameter(BYTE bGameStation);

	/** 
	 * @brief 
	 */
	virtual bool OnGameQuiting();

protected:
    IGameFrame     * m_pGameFrame;            /**< 游戏框架对象 */
    IUserInterface * m_pUI;                   /**< 游戏UI对象 */
	ISoundEngine   * m_pSoundEngine;	      /**< 游戏声音对象 */
	
	std::map<BYTE, SOUND_INFO> m_SoundMap;    /**< 声音资源对象 */

protected:
	wchar_t m_wNickName[PLAYER_NBR][MAX_PATH];/**< 游戏昵称 */
	int     m_nTempMoney[PLAYER_NBR];         /**< 临时金币变化信息 */
	int     m_nTimeOutCount;                  /**< 超时次数 */
	bool    m_bSound;                         /**< 声音开关 */
	bool    m_bIsWatch;                       /**< 旁观玩家 */
	bool    m_bIsEnableWatch;                 /**< 是否允许旁观 */

protected:
	CMJHand m_TilesHand[PLAYER_NBR];          /**< 玩家手牌 */
	CMJGive m_TilesGive[PLAYER_NBR];          /**< 玩家出牌 */
	CMJSet  m_TilesSet[PLAYER_NBR];           /**< 玩家拦牌 */
	CMJHand m_TilesFlower[PLAYER_NBR];        /**< 花牌数据 */
	CMJWall m_TilesWall;                      /**< 牌墙 */
	CMJHand m_TilesGod;                       /**< 精牌 */
	BYTE    m_byDice[DICE_NBR];               /**< 骰子 */
	BYTE    m_byCurDice[2];                   /**< 当前骰子 */
	BYTE    m_byDealer;                       /**< 庄家位置 */
	BYTE    m_byMyStation;                    /**< 自己的位置 */
	BYTE    m_byTurnStation;                  /**< 当前操作人 */
	TILE    m_LastTile;                       /**< 上一次打的牌 */
	BYTE    m_byGameState;                    /**< 游戏状态 */
	BYTE    m_byStateSelect;                  /**< 拦牌选择状态 */
	MJ_SETTING_DATA m_tagSetting;             /**< 配置数据 */
	UserInfoForGame m_tmpUserInfo;            /**< 临时用户对象 */
	ST_BLOCKINFO    m_stBlockInfo;            /**< 拦牌信息 */
	bool    m_bReady[PLAYER_NBR];             /**< 准备 */
	bool    m_bTuoGuan[PLAYER_NBR];           /**< 拖管 */
	bool    m_bTing[PLAYER_NBR];              /**< 听牌 */
	
	BYTE    m_byDiceFinishCount;              /**< 骰子播放完成数 */
	BYTE    m_byGangCount;                    /**< 杠牌数量 */
	ENM_DICE_STATE  m_enDiceState;            /**< 骰子状态 */

	TCMD_NOTIFY_DEAL m_stDealInfo;            /**< 发牌信息 */
	TCMD_NOTIFY_DRAW m_stDrawInfo;            /**< 抓牌信息 */
	TCMD_NOTIFY_FINISH  m_stFinishInfo;		  /**< 结算信息 */

	int     m_nSmallHandCount;                /**< 小手指移动次数 */

	/// 定义消息接收函数
public:
	/////////////////////////////////////////////////////////////////////////
	/// 负责处理接收的消息函数
	/**
	 *  @brief 房间基础信息
	 */	
	virtual void OnNotifyBaseInfo(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到进入、重回、数据恢复消息
	 */	
	virtual void OnNotifyGameInfo(BYTE * buffer, int nLen);

	/**
	 *  @brief 处理设置连局信息(游戏开始发送的的第一条游戏消息)
	 */	
	virtual void OnNotifyRunNbr(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到通知庄家位置消息
	 */	
	virtual void OnNotifyDealer(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到开局发牌消息后
	 */	
	virtual void OnNotifyDeal(BYTE * buffer, int nLen);

	/**
	 *  @brief 处理补花结束消息
	 */	
	virtual void OnNotifyFlowerOver(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到摸牌消息
	 */	
	virtual void OnNotifyDraw(BYTE * buffer, int nLen);

    /**
	 *  @brief 接受到请求出牌结果消息
	 */	
	virtual void OnNotifyGive(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到拦牌消息
	 */	
	virtual void OnNotifyBlock(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到请求拦牌结果消息
	 */	
	virtual void OnNotifyBlockResult(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到结束消息
     */	
	virtual void OnNotifyFinish(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到修改旁观状态消息
	 */	
	virtual void OnNotifyChangeLookOn(BYTE * buffer, int nLen);

	/**
	 *  @brief 接受到刷新手牌消息
	 */	
	virtual void OnNotifyUpdateHandTiles(BYTE * buffer, int nLen);
	
	/**
     *  @brief 接受到GM命令消息后执行的事件
     */
    virtual void OnNotifyGMCmd(BYTE * buffer, int nLen);

	/** 
	 * @brief 通知显示消息对话框
	 */
	virtual void OnNotifyShowDlg(BYTE * buffer, int nLen);
	
	/////////////////////////////////////////////////////////////////////////


	/// 定义绘制函数
public:
	/////////////////////////////////////////////////////////////////////////
	/// UI界面相关绘制函数
	/** 
	 * @brief 绘制玩家信息
	 * @param[in] byChair    玩家位置
	 * @param[in] stUserInfo 玩家信息
	 * @param[in] bShow      是否显示
	 */
	virtual void DrawUserInfo(BYTE byChair, UserInfoForGame stUserInfo, bool bShow = true);

	/** 
	 * @brief 绘制准备
	 * @param[in] byChair 玩家位置
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawReady(BYTE byChair, bool bShow = true);

	/** 
	 * @brief 绘制骰子
	 * @param[in] byChair 玩家位置
	 * @param[in] byDice  骰子
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawDice(BYTE byChair, BYTE byDice[], bool bShow = true);

	/** 
	 * @brief 绘制托管
	 * @param[in] byChair 玩家位置
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawTuoGuan(BYTE byChair, bool bShow = true);

	/** 
	 * @brief 绘制听牌
	 * @param[in] byChair 玩家位置
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawTing(BYTE byChair, bool bShow = true);


	/** 
	 * @brief 绘制庄家
	 * @param[in] byChair 玩家位置
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawDealer(BYTE byChair, bool bShow = true);

	/** 
	 * @brief 绘制拦牌框
	 * @param[in] byFlag  拦牌标识: 吃(0x01), 碰(0x02) 杠(0x04), 听(0x08), 胡(0x10)
	 */
	virtual void DrawBlock(BYTE byFlag);

	/** 
	 * @brief 绘制手牌
	 * @param[in] byChair 玩家位置
	 * @param[in] mjHand  手牌数据
	 * @param[in] bIsLaydown 控件是否躺下(摊牌)
	 * @param[in] bIsGiveTileMode 最右边一张牌是否分开
	 */
	virtual void DrawHand(BYTE byChair, CMJHand mjHand, bool bIsGiveTileMode = false, bool bIsLaydown = false);

	/** 
	 * @brief 绘制出牌
	 * @param[in] byChair 玩家位置
	 * @param[in] mjGive  出牌数据
	 */
	virtual void DrawGive(BYTE byChair, CMJGive mjGive);

	/** 
	 * @brief 绘制拦牌
	 * @param[in] byChair 玩家位置
	 * @param[in] mjSet   拦牌数据
	 */
	virtual void DrawSet(BYTE byChair, CMJSet mjSet);

	/** 
	 * @brief 绘制牌墙
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawWall(bool bShow = true);

	/** 
	 * @brief 绘制小手
	 * @param[in] nOffsetPos 小手指补第几墩的牌
	 * @param[in] bShow      是否显示
	 */
	virtual void DrawSmallHand(int nOffsetPos, bool bShow = true);

	/** 
	 * @brief 绘制出牌标记
	 * @param[in] byChair 玩家位置
	 * @param[in] bShow   是否显示
	*/
	virtual void DrawGiveAnimate(BYTE byChair, bool bShow = true);
	
	/** 
	 * @brief 绘制结算框
	 * @param[in] pCmd    结算数据
	 * @param[in] bShow   是否显示
	 */
	virtual void DrawFinish(TCMD_NOTIFY_FINISH * pCmd, bool bShow = true);
	/////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////
	/// 游戏中功能操作函数
	/** 
	 * @brief 设置定时器
	 * @param[in] byChair 玩家位置
	 * @param[in] uTime   时间秒数
	 */
	virtual void SetGameTimer(BYTE byChair, UINT uTime);

	/** 
	 * @brief 停止所有定时器
	 */
	void KillAllTimer();

	/** 
	 * @brief 加载音效资源
	 */
	virtual void LoadSound();

	/** 
	 * @brief 播放声音
	 * @param[in] nSoundID 播放类型ID
	 * @param[in] byChair  玩家位置
	 */
	void PlaySound(int nSoundID, BYTE byChair = -1);

	/** 
	 * @brief 播放声音
	 * @param[in] pSoundName 声音名称
	 */
	void PlaySound(char * pSoundName);

	/** 
	 * @brief 播放背景音乐
	 * @param[in] bPlay 播放开关(true: 播放,  false: 停止)
	 */
	void PlayBackSound(bool bPlay = true);

	/** 
	 * @brief 设置旁观玩家
	 * @param [in] bIsWatch (true: 为旁观玩家,  false: 为非旁观玩家)
	 */
	void SetIsWatching(bool bIsWatch);

	/** 
	 * @brief 是否允许旁观玩家观看
	 * @param [in] bIsEnableWatch (true: 允许,  false: 不允许)
	 */
	void SetIsEnableWatch(bool bIsEnableWatch);

	/** 
	 * @brief 是否为旁观玩家
	 * @return (true: 为旁观玩家,  false: 为非旁观玩家)
	 */
	bool GetIsWatching() { return m_bIsWatch; };

	/** 
	 * @brief 是否允许旁观玩家观看
	 * @return (true: 允许,  false: 不允许)
	 */
	bool GetIsEnableWatch() { return m_bIsEnableWatch; };
	////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////
	/// 通过UI界面交互，操作请求的相关函数
	/** 
	 * @brief 请求GM命令
	 */
	virtual void OnRequestGMCmd();

	/** 
	 * @brief 请求出牌                
	 * @param [in] uControlID 控件ID
	 * @param [in] bAuto      是否自动出牌
	 */
	virtual void OnRequestGive(UINT32 uControlID, bool bAuto = false);

	/** 
	 * @brief 请求拦牌              
	 * @param [in] uControlID 控件ID
	 */
	virtual void OnRequestBlock(UINT32 uControlID);

	/** 
	 * @brief 请求拦牌              
	 * @param [in] uControlID 控件ID
	 */
	virtual void OnRequestBlockSelect(UINT32 uControlID);

	/** 
 	 * @brief 拦牌信息移动提示选择             
 	 * @param [in] uControlID 控件ID
	 * @param [in] nIndex     牌索引位置
	 */
	virtual void OnBlockMoveSelect(UINT32 uControlID, int nIndex = -1);
	///////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////
	/// 动画结束后调用的相关函数
	/** 
	 * @brief 动画结束
	 * @param[in] uControlID 控件ID
	 */
	virtual void OnAnimalFinish(UINT32 uControlID);

	/** 
	 * @brief 动画骰子结束
	 * @param[in] uControlID 控件ID
	 */
	virtual void OnAnimalDiceFinish(UINT32 uControlID);

	/** 
	 * @brief 动画定时器结束
	 * @param[in] uControlID 控件ID
	 */
	virtual void OnAnimalClockFinish(UINT32 uControlID);

	/** 
	 * @brief 出牌超时
	 * @param[in] uControlID 控件ID
	 */
	virtual void OnTimeOutGive(UINT32 uControlID);

	/** 
	 * @brief 拦牌超时
	 * @param[in] uControlID 控件ID
	 */
	virtual void OnTimeOutBlock(UINT32 uControlID);
	/////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////
	/// 定时器结束后调用的相关函数
	/** 
	 * @brief 发牌操作函数
	 */
	virtual void OnDeal();

	/** 
	 * @brief 骰子操作函数
	 */
	virtual void OnDice();

	/** 
	 * @brief 小手移动操作函数
	 */
	virtual void OnSmallHandMove();

	/** 
	 * @brief 显示结算操作函数
	 */
	virtual void OnShowFinish();
	/////////////////////////////////////////////////////////////////////////////
};

#endif