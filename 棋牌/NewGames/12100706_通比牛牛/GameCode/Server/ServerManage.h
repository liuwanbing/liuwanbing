#pragma once

#include "StdAfx.h"
#include"..\GameMessage\UpgradeLogic.h"

//定时器 ID
#define TIME_WAIT_THINK				30				//等待进入游戏状态
#define TIME_THINK					31				//思考定时器


//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束
#define SH_USER_CARD                5               //牌的张数

static __int64					m_iAIHaveWinMoney;			//机器人赢的钱	20121104dwj	声明为静态全局变量，因为好多个桌子同时运行是随时要改变这个值;	
static __int64					m_iReSetAIHaveWinMoney;		/**<机器人输赢控制：重置机器人已经赢钱的数目 声明为全局变量，保证只重置一次 */
static int						m_iRecFroDayTime;			/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */

//游戏桌类
class CServerGameDesk : public CGameDesk
{
protected:
	CUpGradeGameLogic		m_Logic;							//游戏逻辑

	bool					m_bWatchOther[PLAY_COUNT];			/**<是否充许旁观*/
	bool					m_bIsPrepare[PLAY_COUNT];		//是否准备
	//发牌数据
	int						m_iUCCount[PLAY_COUNT];					//用户手上扑克数目

	BYTE					m_byUCard[PLAY_COUNT][USER_HAND_CARD];	//用户手上的扑克

	BYTE					m_iTotalCard[ALL_CARD_COUNT];			//总的牌

	BYTE                    m_bBullCard[PLAY_COUNT][USER_HAND_CARD];//玩家摆的牛牌

	int						m_byCardShape[PLAY_COUNT];				//玩家牌型

	bool					m_bHaveBaiPai[PLAY_COUNT];				//是否已经摆牌 false:没有 true:已经摆牌

	bool					m_bIsInGame[PLAY_COUNT];				//玩家是否在游戏中

	bool                    m_bIsCut[PLAY_COUNT];				//是否掉线了主要为机器人设置

	BOOL                    m_bIsAuto[PLAY_COUNT];				//是否托管

	bool                    m_bIsSuper[PLAY_COUNT];				//是否可以使用发牌

	int						m_iSurplusTime;						//剩余的时间

	BYTE                    m_bControlWin;						//控制输赢位置

	BYTE                    m_bControlLost;						//控制输赢位置

	BYTE					  m_byCowUserNum;							//牛牛玩家人数
	bool						m_bIsCowUser[PLAY_COUNT];				//是否牛牛玩家
	//--------------add by DWJ--20121203----------------------------------
	BOOL					m_bAIWinAndLostAutoCtrl;	//机器人控制输赢	20121122	dwj	;
	bool					m_bIsFirstLoadIni;			//是否第一次读取配置文件 用于保存第一次启动日期，其后没增加一天m_iAIWantWinMoneyA1～A3区域要递增;
	__int64					m_iAIWantWinMoneyA1;		/**<机器人输赢控制：机器人赢钱区域1  */
	__int64					m_iAIWantWinMoneyA2;		/**<机器人输赢控制：机器人赢钱区域2  */
	__int64					m_iAIWantWinMoneyA3;		/**<机器人输赢控制：机器人赢钱区域3  */
	int						m_iAIWinLuckyAtA1;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	int						m_iAIWinLuckyAtA2;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	int						m_iAIWinLuckyAtA3;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	int						m_iAIWinLuckyAtA4;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	__int64					m_iAddMoneyEveryDay;		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	//__int64					m_iReSetAIHaveWinMoney;		/**<机器人输赢控制：重置机器人已经赢钱的数目  */
	//__int64					m_iAIHaveWinMoney;			//机器人赢的钱			
	//int						m_iRecFroDayTime;			/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */
	BYTE					m_bCardShapeBase[MAX_SHAPE_COUNT];//0-10代表无牛到牛牛间的倍率
	bool					m_bWriteLog;							/**< 是否打印日志 */

private:
	int						m_iBaseNote;						//底注

	BYTE					m_byBTime;							//游戏开始时间

	BYTE					m_bySCTime;							//游戏发牌时间

	BYTE					m_byTTime;							//游戏思考时间

	BYTE					m_byRTime;							//结算后等待时间

	BYTE					m_iAllCardCount;					//游戏所用的牌数

	DWORD					m_iCardShape;						//牌型

	int						m_iGoodCard;						//发大牌机率牛七以上

	bool					m_bHaveKing;						//是否有王

	//超端变量
	bool					m_bSuperSet;						//超短设置是否成功
	int						m_iSuperSet[2];						//被设定的玩家输赢数据
	/*
	0元素：玩家逻辑位置（255为初始值）
	1元素：玩家是赢还是输(0为初始值，1为赢，2为输)
	*/

	//函数定义
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
	//
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);
	//用户断线重来
	virtual bool UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo);

	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//处理函数
public:
	///初始化数据
	void InitGameData();

	//读取静态变量
	BOOL LoadIni();	

	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);
	//-------ADD-BY-DWJ-20121203--------------------------------
	//检查是否已经新的一天了，需要重新设定机器人控制配置参数
	void CheckNewDayReSetIni(CINIFile *f, CString *key);

	//20121203dwj 记录机器人输赢值
	void RecordAiHaveWinMoney(__int64 iTempPoint[PLAY_COUNT]);

	/**从配置文件中读取机器人控制设置的参数值*/
	void GetAIContrlSetFromIni(int iRoomID);

	//发送扑克给用户
	BOOL SendCard();

	//处理玩家摊牌动作
	void HandTanPaiAction(BYTE byDStation,BYTE byDownCard[]);	

	//自动摊牌
	void AutoTanPai(BYTE byDStation);	

	//检测摊牌是否结束
	BOOL CheckTanPaiFinish();

	///获得胜利玩家
	BYTE GetWinPeo();

	///计算结算分数
	void CountResultScore(BYTE bDeskStation,GameEndStruct* pGameEnd,BYTE bCloseFlag);

	//统计游戏人数
	BYTE CountPlayer();

	///处理托管消息
	BOOL UserAuto(BYTE bDeskStation,AutoCardStruct *bAuto);

	//处理超端消息
	BOOL HandleSuperInfo(BYTE bDeskStation,SuperStruct *pSuper);

	//读取超端配置
	BOOL SuperLoad();

	//根据超端需求换牌
	void SuperSet();

	///计算当前超端设置指定玩家的输赢钱
	int CountSuperWinMoney();

	//隊例中是否有某牌是否替換成功
	BOOL ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard);

	//桌面倍数
	int GetDeskBasePoint();

	//房间倍数
	int GetRoomBasePoint();

	//逃跑扣分
	int GetRunPublish();

	//得到最大牌和最小牌,win值为1时得到最大,其它为最小
	//BYTE GetIndex(int win);

	//判断胜家
	//BOOL JudgeWiner();

	//判断输家
	//BOOL JudgeLoser();

	//换牌
	BOOL ChangeCard(BYTE bDestStation,BYTE bWinStation);

	///20121203dwj机器人输赢自动控制,
	void IAWinAutoCtrl();
	///20121203dwj计算当前牌机器人的输赢钱
	int CountAIWinMoney();

	//清理强退玩家
	void ClearQuitPlayer();

	/**
	* 设置游戏 计时器 其响应函数 还是OnTimer
	*
	* @param [in] UINT uTimerID 计时器ID
	*
	* @param [in] int uElapse 间隔时间
	* 
	@param [in] bool bRecTime 是否记录剩余时间
	* 
	* @return void 
	*/
	void SetGameTimer(UINT uTimerID, int uElapse,bool bRecTime);

	///清理所有计时器
	void KillAllTimer();

	///获取当前牌中最大的牌型，
	int GetMaxCardShape(BYTE iCard[], int iCardCount);

	void GetIniData(IniData &sData);	//获取读取INI文件数据

	void GetBaseData(BData &sData, BYTE bDeskStation);		//获取游戏基本数据

	/**
	* 打印日志
	*
	*/
	void DebugPrintf(const char *p, ...);
};
/******************************************************************************************************/
