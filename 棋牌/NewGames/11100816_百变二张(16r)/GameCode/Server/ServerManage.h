#pragma once

/**
* 百变二张牌服务器
*
* @author duanxiaohui
* @version 1.0
* @date 2009.12.10
*/

#include "StdAfx.h"

#include "..\GameMessage\GameMessage.h"
#include "..\GameMessage\UpGradeLogic.h"

/** 断线定时器 ID */
#define IDT_USER_CUT				1L
///无庄模式中等待定时器
#define ID_WIAT_GAME_BEGIN          25 
///通知客户端洗牌
#define TIME_RAND_CARD              28 
/**叫庄定时器*/
#define TIME_JIAO_ZHUANG            29
/** 下注定时器 */
#define TIME_XIAZHU                 30
/** 发牌定时器 */
#define TIME_SEND_CARD				31	
/** 游戏结束定时器 */
#define TIME_GAME_FINISH			32
/** 空闲时间定时器 */
#define TIME_EMPTY                  33

#define TIMT_APPLY_NT               34  



/** 游戏正常结束 */
#define GF_NORMAL					10
/** 游戏安全结束 */
#define GF_SALE						11	
/** 提前结束 */
#define GF_AHEAD					12
/** 未能够产生胜者 */
#define GF_NO_WINER					13

//机器人奖池控制静态全局变量
static __int64			G_i64AIHaveWinMoney;		//机器人赢钱数 这里声明为静态全局变量，因为多个桌子需要及时修改该变量
static __int64			G_i64ReSetAIHaveWinMoney;	/**<机器人输赢控制：重置机器人已经赢钱的数目  该变量也要声明为静态全局变量 否则每个桌子都会去重置*/


/// 游戏桌子类
class CServerGameDesk : public CGameDesk
{
public:
	GameLogic               m_gameLogic;                         /**< 游戏逻辑 */

protected:
	EZP_Xiazhu_Data         m_UserXiaZhu;                        /**< 下注数据 */

	CArray <GameLuziInfo, GameLuziInfo&> m_GameLizuInfo;         /**< 记录游戏路子 */

	CArray <BYTE, BYTE&>    m_GameKPInfo;                        /**< 记录每局开牌信息，只记录最新X局(低4位表示: 0x01:顺门、0x02:对门、0x04:倒门) */

	CArray <BYTE, BYTE&>    m_GameXZInfo[PLAY_COUNT];            /**< 记录每个玩家下注记录, 与开牌记录对应(低4位表示: 0x01:顺门、0x02:对门、0x04:倒门)*/

	CArray <BYTE, BYTE&>    m_ArrUserList;                       /**< 申请上庄的玩家列表 */

	int                     m_iApplyMoneyLimit;                  /**< 申请上庄金币下限*/

	int                     m_iZhuangCount;                      /**< 当前庄家做庄次数 */

	int						m_iZhuangMaxNum;                     /**< 庄家最大做庄次数 */
	
	int                     m_iZhuangStation;                    /**< 当前庄家 */
	
	__int64                 m_iZhuangResult;                     /**< 庄家输赢成绩 */

	__int64                 m_iUserResult[PLAY_COUNT];           /**< 每个玩家的输赢成绩 */
	
	__int64                 m_iAllMoney;                         /**< 所有玩家下注总额 */	
	
	BYTE                    m_bSendCardPos;                      /**< 开始发牌位置 */

	BOOL                    m_bNTGameMode;                       /**< 游戏模式 TRUE:无庄模式  FALSE:有庄模式 */

	BOOL                    m_bUserXZ;                           /**< 当前下庄状态 */	

	BOOL                    m_bInNoteStage[PLAY_COUNT];                      ///是否处于下注阶段

	BOOL                    m_bPrintEnding;                      /**< 是否打印结局 */

	BOOL                    m_bUseCard;                          /**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */

	BOOL                    m_bKingAndThreeBig;                  /**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/

	BYTE                    m_bGameVersion;                      /**< 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代 */

	BOOL                    m_bRedJOrBlackJ;                      /**< 红J还是黑J FALSE: 黑J TRUE: 红J */

	char                    m_chGameCount;                       /**< 游戏把数 72把为一局 */

	char                    m_chGameBeen;                        /**< 游戏局数 */

	CTime                   m_RecordTime;                        /**< 记录当天游戏时间 */

	int                     m_iRobotSZCount;                     /**< 允许机器人上庄数量 */

	int                     m_iXiaZhuTime;                       /**< 下注时间 */

	int                     m_iKaipaiTime;                       /**< 开牌时间 */

	int                     m_iEmptyTime;                        /**< 空闲时间 */

	int                     m_iTime;                             /**< 当前时间 以秒为单位 */


	int                     m_iMaxZhuangCount;                   /**< 最大做庄次数 */

	int                     m_iMinZhuangCount;                   /**< 最小做庄次数 */

	int                     m_iNtTax;                            /**< 抽水类型 0：只扣庄家税 1：扣全部赢家的税 默认值为0 */

	__int64                 m_iMaxNote;                          /**< 闲家最大下注数(所有区域总注) */
	__int64                 m_iRectNoteMax[REGION_COUNT] ;       /**< 配置每个区域的最大下注 0-左横区域；1-右横区域；2-顺门；3-倒门；4-左角；5-对门；6-右角*/

	int                     m_iUserNoteZeroCount[PLAY_COUNT];   ///玩家连续没有下注的记录盘数


public:
	static BYTE m_iAllCardCount;								 /**< 游戏所用的牌数 */
	int m_iLimitNote;											 /**< 上限 */
	int m_iBaseNote;											 /**< 底注 */

	BYTE m_iSendCardTime;										 /**< 游戏思考时间 */
	BYTE m_iThinkTime;											 /**< 游戏思考时间 */
	BYTE m_iBeginTime;											 /**< 游戏开始时间 */
	bool m_bStartGameWithOutNt;					/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 add xyh 2012-1-2*/

	/// 机器人控制输赢变量定义
private:
	bool                    m_bIsRobot[PLAY_COUNT];              /**< 标志是否为机器人 */

	

	int		                m_iAIWantWinMoney;		             /**< 机器人输赢控制：机器人要赢的钱 */	//dwjdelete

	int		                m_iAIHaveWinMoney;		             /**< 机器人输赢控制：机器人已经赢了多少钱 */	//dwjdelete

	int		                m_iAIMaxWinInOne;		             /**< 机器人输赢控制：机器人平均每把最大赢钱数（小于等于0时为不限） */	//dwjdelete

	int		                m_iAICtrlCount;			             /**< 机器人输赢控制：当前机器人已经进行了自动控制的把数 */

	int                     m_iWinQuYu[6];				         /**< 游戏的赢钱区域 0顺门，1顺门-角，2对门，3桥，4倒门，5倒门-角 */

private:	//机器人奖池输赢控制参数	ADD BY DWJ
	bool	        m_bAIWinAndLostAutoCtrl;    /**< 机器人输赢控制：是否开启机器人输赢自动控制 */
	__int64			m_i64AIWantWinMoneyA1;		/**<机器人输赢控制：机器人赢钱区域1  */
	__int64			m_i64AIWantWinMoneyA2;		/**<机器人输赢控制：机器人赢钱区域2  */
	__int64			m_i64AIWantWinMoneyA3;		/**<机器人输赢控制：机器人赢钱区域3  */
	int				m_iAIWinLuckyAtA1;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	int				m_iAIWinLuckyAtA2;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	int				m_iAIWinLuckyAtA3;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	int				m_iAIWinLuckyAtA4;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */


	/// 超级客户端变量定义
private:
	BOOL                    m_bSuperSetting;                     /**< 标识本局是否有控制 */	

	char                    m_bSuperState[3];                    /**< 0: 代表顺门 1: 代表对门 2: 代表倒门 (TRUE: 选中 FALSE: 未选中) */

	bool					m_bIsVipDouble;						//VIP玩家可以双倍下注--( true )

	bool                    m_bUserNetStation[PLAY_COUNT] ;          ///用户断线状态

	bool                     m_bWriteLog ;           ///是否打印日志

	bool					m_bIsRunning;						//游戏是否已经运行


	/// 控制输赢设置
	void OnSuperSetting(BYTE bDeskStation,  SUPERSETTING * pMsg);  

	/// 验证超级用户权限
	void AuthPermissions();

	/// 执行超级客户端设置
	BOOL RunSuperSetting();

public:
	/// 构造函数
	CServerGameDesk(); 
	/// 析构函数
	virtual ~CServerGameDesk();

public:
	/// 初始游戏
	virtual bool InitDeskGameStation();
	/// 游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	/// 游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	/// 判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	/// 框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);	
	//玩家断线处理
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	/// 玩家坐下
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	/// 重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);

	// 添加VIP 双倍下注功能(百人类游戏)
	BOOL  CanDouble(BYTE bDeskStation);

public:
	/// 读取静态变量
	BOOL LoadIni();
	/// 根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);
	/// 设置上限和底注
	BOOL SetLimitNote();
	/// 统计游戏人数
	BYTE CountPlayer();
	/// 杀所有计时器
	void KillAllTimer();
	/// 杀所有计时器
	void KillGameAllTimer();
	/// 数据初始化
	void InitData();
	/// 控制超级客户端和配牌
	void InitSuperConfig();
	/// 判断庄家是否符合游戏规则
	bool IsZhuangRule();
	/// 判断上庄列表
	bool IsZhuangList();
	/// 发牌
	void FaPaiToAllUser();
	/// 房间倍数
	int	 GetRoomBasePoint();
	/// 逃跑扣分
	int	 GetRunPublish();
	/// 玩家申请上庄
	void OnApplyZhuang(BYTE bDeskStation, EZP_APPLY_ZHUANG * pApplySZ);
	/// 玩家下注
	void OnUserXiaZhu(BYTE bDeskStation, EZP_XIAZHU_INFO * pXiazhu);
	/// 游戏结算
	void DoGameResult(BYTE bResult, __int64  iMoney[], __int64  iReturnMoney[]);
	/// 游戏开始
	void StartGame();
	///通知客户端开始下注
    void  NotifyClientNote();
	/// 通知客户端开始叫庄
	/// 下注完成
	void OnUserXiaZhuFinish();
	/// 定时器
	void SetGameTimer(UINT uTimeID, UINT uMaxTime);
	/// 判断游戏模式
	void DecideGameMode();
	/// 打印结局
	void PrintEnding();
    ///打印语句
	void DebugPrintf2(int iRoomID, const char *p, ...) ; 
	/// 清理玩家数据
	void ClearUserInfo(BYTE bDeskStation);
	/// 每局游戏结束后，检测上庄列表中玩家金币是否还够上庄下限
	void CheckAppalyList();
	/// 获取赢钱的区域
	void GetWin();
	/// 描述：机器人输赢自动控制
	void IAWinAutoCtrl();
	//判断该赢还是该输
	bool	JudgeShouldWinOrLost();
	//交换指定两家的牌
	void	ExChangeCard(BYTE byFirstStation, BYTE bySecondStation);
	/// 描述：计算当前牌机器人的输赢钱
	__int64 CountAIWinMoney();

	//记录机器人输赢值
	void	RecordAiHaveWinMoney(__int64 *ChangeMoney);

	///设置配牌功能
	void SetCardFun();

	/*
	* 打印日志文件
	*/
	void DebugPrintf(const char *p, ...);

	//自动读取配制文件相关操作
#ifdef AUTOLOADINI
private:

	void	AutoLoadIni(int iRoomID);		//自动加载配制文件 

	void	LoadModifiedIni();		//加载修改的配制文件 

	void	LoadModifiedIni(int iRoomID);	//根据房间加载修改的配制文件
#endif
};


/******************************************************************************************************/
