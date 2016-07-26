#pragma once

#include "StdAfx.h"
#include"..\GameMessage\UpgradeLogic.h"
#define CardCount			36							//扑克张数
//定时器 ID

#define S_TIMER_WASHCARD			31				//洗牌定时器
#define S_TIMER_					201				//

#define TIMER_SETTLE				40				//结算定时器

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束


static __int64		m_iAddMoneyEveryDay;		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
static __int64		m_iAddMoneyAllDay;			/**<机器人输赢控制：机器人在赢钱A1、A2、A3区域累积的钱的数值  */
static CTime		m_iRecFroDayTime;	/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */

//游戏桌类
class CServerGameDesk : public CGameDesk
{
public:
	//enum GameMode {GAMEMODE_BANKER_BET,GAMEMODE_BANKER_INTURN,GAMEMODE_BANKER_CONST,GAMEMODE_INVALIDE};//游戏模式：抢庄，轮庄，定庄,无效
	CUpGradeGameLogic		m_Logic;												//游戏逻辑

private:
	//游戏属性
	ENUM_GameMode			m_GameMode;												//游戏模式
	ENUM_GameStation		m_EGameStation;											//游戏状态
	bool					m_IsModeSet;											//游戏模式是否设置
	int						m_iBasePoint[3];										//基础分
	int						m_PlayersCountInGame;									//游戏中玩家人数
	int						m_iPKPlayerCount;										//已经比牌玩家的人数
	int						m_iGamesCounter;										//连续玩的局数
	int						m_iBetPlayerCount;										//下注玩家人数

	//玩家属性
	BYTE					m_byBankerDeskStation;									//庄家的座位号
	__int64					m_iBetIn[PLAY_COUNT];									//每个玩家的下注金额
	bool					m_isBetBanker[PLAY_COUNT];								//是否抢庄
	int						m_sendBetBankerPlayer;									//
	

	//玩家的牌属性
	EnCardShape				m_CardResult[PLAY_COUNT];								//玩家的牌型，初始化
	BYTE					m_byUserCard[PLAY_COUNT][5];							//牌数据
	
	//超端
	vector<long>			m_vSuperClient;											//超端用户
	BOOL					m_isWinOrLose[PLAY_COUNT];								//输赢
	TSuper					m_tSuper;												//输赢设置
	
	//////////////////////////////////////////////////////////////////////////

	
	int						m_iStylePrize[8];										//牌型奖分
	int						m_iUnit;												//分数兑换金币比例
	BYTE				    m_iThinkTime;											//游戏思考时间
	BYTE				    m_iBeginTime;											//游戏开始时间
	bool					m_bIsSuperControl;										//是否超端控制
	bool					m_bAuto[PLAY_COUNT];									//是否托管
	bool					m_bIsPrepare[PLAY_COUNT];								//是否准备
	bool					m_bIsCompare[3][PLAY_COUNT];							//是否已经比过牌
	int					    m_iGetPoints[3][PLAY_COUNT];							//每一堆各玩家的得分
	
	int						m_iCardCount[PLAY_COUNT];								//牌数量
	BYTE					m_bCardList[PLAY_COUNT][EveryCount];					//当局所洗的牌
	int					    m_bOutCardCount;										//出牌人数
	int						m_iWhichHeap;											//当前比第几堆牌
	int						m_iEveryPeopleStyle[PLAY_COUNT][3];						//每个玩家每一堆的牌型

	BYTE					m_bCompareRecord[PLAY_COUNT][PLAY_COUNT];				//每个玩家比牌记录
	BYTE					m_byWinSuperControl;									//超端控制赢位置
	//断线重连
	bool					m_bIsOpenCard[PLAY_COUNT];								//是否已经开牌
	bool					m_byIsCompareCard[PLAY_COUNT][3];						//玩家比过的堆牌
	int						m_iCompareStyle[PLAY_COUNT][3];							//比过的堆牌牌型
	vector<long>			m_vlSuperID;											//保存超端玩家的容器

	
	BYTE				   m_byPlayerCards[PLAY_COUNT][5];							//四个玩家的牌
	EnCardShape			   m_cardShape[PLAY_COUNT];									//四个玩家的牌型
	int					   m_iArrMaxOrder[PLAY_COUNT];								//四个玩家牌型大小顺序
	BYTE                   m_byUserStation[PLAY_COUNT];                             //玩家的状态，正在游戏中，中途加入
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();
	//重载函数
public:
	//初始化游戏逻辑
	virtual bool InitDeskGameStation();
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	//框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//用户断线
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);
	//用户断线重来
	virtual bool UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo);
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
public:
	//初始化服务器游戏数据
	BOOL InitData();
	//读取静态变量
	BOOL LoadIni();	
	//发送游戏状态
	//等待同意状态
	void OnSendGameStationAgree(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//洗牌
	void OnSendGameStationWash(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//游戏开始状态
	void OnSendGameStationPlaying(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//游戏消息处理
	//游戏模式选择
	void OnGameMode(void * pData,BYTE bDeskStation);
	//设置游戏模式
	void SetGameMode(void *pData);

	//设置游戏状态
	void SetGameStation(void *pData);
	//游戏状态改变
	void OnGameStationChanged(void *pData);
	//
	/*void OnXuanZhuang(BYTE byDesakStation,void *pBuffer);*/
	//获取当前桌子玩家人数
	int GetPlayersCountInDesk();

	//获取正在玩游戏的玩家的人数，（除去旁观人数）
	int GetInGamePlayerCount();


	//准备消息处理函数
	void OnGMGetReady(BYTE byDeskStation,void *pData);

	//发牌

	void SendCard(int bDeskStation);

	//接收用户发送过来的牌
	void ReceiveClientCards(BYTE byDeskStation,void *buffer);

	//分析牌型
	void analyseCardShape(BYTE byDeskStation,T_CS_OutCard* ptCoutCards);

	//将分好牌的牌数据广播给所有玩家
	void ShowCardsToAll(BYTE byDeskStation,void * pOutCards);

	//结算
	void Settle();

	//重置游戏状态和数据
	void GameOverRest();

	//托管
	void AutoPlay(BYTE byDeskStation);

	//托管判断牌型函数
	EnCardShape AutoPlayGetCardShape(BYTE *pCard,int iLength,T_CS_OutCard* outCards);

	//抢庄
	void BetBanker();
	
	// 获得牌型 
	EnCardShape GetCardShape(BYTE *pbyCards, int iCounter);
	//修改奖池数据1(平台->游戏)
	/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
	/// @return 是否成功
	virtual bool SetRoomPond(bool	bAIWinAndLostAutoCtrl);

	//修改奖池数据2(平台->游戏)
	/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
	/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[]);

	//修改奖池数据3(平台->游戏)
	/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iReSetAIHaveWinMoney);

	//控制胜负
	/// 判断
	virtual BOOL Judge();
	/// 胜
	virtual BOOL JudgeWiner();
	/// 输
	virtual BOOL JudgeLoser();
	//GRM函数
	//更新奖池数据(游戏->平台)
	/// @return 机器人赢钱数
	virtual void UpDataRoomPond(__int64 iAIHaveWinMoney);
	void OnHandleUserNetCut(BYTE bDeskStation, CGameUserInfo* pLostUserInfo);
	void AutoChoiseOutCard(BYTE bDeskStation,T_CS_OutCard &tOutCards);
};
