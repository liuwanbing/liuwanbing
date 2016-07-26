#pragma once

#include "../../../../sdk/include/ServerCommon/GameDesk.h"
#include"..\GameMessage\UpgradeLogic.h"

//定时器 ID
#define IDT_USER_CUT				1L								/**< 断线定时器 ID */
#define TIME_SEND_CARD				30								/**< 发牌定时器 */
#define TIME_GAME_FINISH			32								/**< 游戏结束定时器 */
#define TIME_SEND_A_CARD			33								/**< 发送一张牌 */
#define TIME_BEGIN_ANIMAL			34								/**< 游戏开始动画 */
#define TIME_COLLECT_JETON			35								/**< 收集动画 */
#define TIME_NOTE                   36                              /**< 下注计时器 */
#define TIME_WAIT                   37                              /**< 等待下注动画完成计时器 */

//游戏结束标志定义
#define GF_NORMAL					10								/**< 游戏正常结束 */
#define GF_SALE						11								/**< 游戏安全结束 */
#define GF_AHEAD					12								/**< 提前结束 */
#define GF_NO_WINER					13								/**< 未能够产生胜者 */
//#define SEND_CARD_TIME				1000						/**< 发牌时间设置测试用10毫秒,正常用1000 */
#define SH_USER_CARD				5

static __int64	m_iAIHaveWinMoney;		//机器人赢钱数20121126	dwj 这里声明为静态全局变量，因为多个桌子需要及时修改该变量
static __int64	m_iReSetAIHaveWinMoney;	/**<机器人输赢控制：重置机器人已经赢钱的数目  该变量也要声明为静态全局变量 否则每个桌子都会去重置*/
static int		m_iRecFroDayTime;		/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */

/// 游戏桌类
class CServerGameDesk : public CGameDesk
{
protected:
	//运行状态变量
	int						m_iUpGradePeople;						/**< 庄家位置 */
	BYTE					m_iBeenPlayGame;						/**< 已经游戏的局数 */
	BYTE					m_iHaveThingPeople;						/**< 有事要走玩家 */

	//状态信息
	BYTE					m_iUserCardCount[PLAY_COUNT];			/**< 用户手上扑克数目 */
	BYTE					m_iUserCard[PLAY_COUNT][SH_USER_CARD];	/**< 用户手上的扑克 */

	//运行信息
	int						m_iOutCardPeople;						/**< 现在出牌用户 */
	int                     m_iLastOutCardPeople;					/**< 最后出牌用户 */
	int						m_iFirstOutPeople;						/**< 先出牌的用户 */
	int                     m_iFirstNotePeople;                     /**< 最新下注玩家 */
	int						m_iOutCardOrder;				//出牌顺序
	//发牌数据
	BYTE					m_iTotalCard[52];						/**< 总的牌 */
	BYTE					m_iSendCardPos;							/**< 发牌位置 */

	CUpGradeGameLogic		m_Logic;								/**< 游戏逻辑 */
	int						m_iUserStation[PLAY_COUNT];				/**< 记录用户状态 */
	__int64					m_iUserNote[PLAY_COUNT];				/**< 用户每轮压注 */
	__int64					m_iPerJuTotalNote[PLAY_COUNT];			/**< 用户每局压总注 */
	__int64					m_iNowNote[PLAY_COUNT];					/**< 当前押注数 */
	__int64					m_iTotalNote;							/**< 总注 */
	BYTE					m_iLastShuttle;							/**< 上一梭了者 */
	BYTE					m_bNoteTimes;							/**< 下注次数 */
	BYTE					m_bAddTime;								/**< 加注标记 */
	BYTE					m_UserThisTurnAct[PLAY_COUNT];			/**< 四个人下注情况 */
	BYTE					m_iNowBigPeople;						/**< 本轮最大压注者(如第一个玩家押400,第二个玩家加倍,即为最大押注者) */
	BYTE					m_iSendPos;								/**< 新一轮发牌开始位置 */
	__int64					m_iThisNoteLimit;						/**< 每轮限注 */
	BYTE					m_byUserFollow;							/**<  第几把必跟 */
	long					m_dwDeskOwer;							/**< 桌主 */
	bool					m_bNetCutPlayer[PLAY_COUNT];			/**< 强退玩家 */
	__int64                 m_iGameAddBasePoint[13];				/**<游戏的中倍数的等级*/
	bool					m_bUserState[PLAY_COUNT];				/**<玩家状态(增加游戏开始后玩家亦可进入功能时添加的数据成员)*/	
	BYTE					m_byUserAddNum[PLAY_COUNT];				//本轮各个玩家加注次数
	__int64					m_i64JudgeScore[12];					/**<判断等级的分数段*/ //add xyh 2012-1-10

	//机器人输赢控件相关变量
private:
	BOOL					m_bAIWinAndLostAutoCtrl;	//机器人控制输赢	20121122	dwj	

	//--------------add by DWJ--20121122----------------------------------
	bool		m_bIsFirstLoadIni;			//是否第一次读取配置文件 用于保存第一次启动日期，其后没增加一天m_iAIWantWinMoneyA1～A3区域要递增
	__int64		m_iAIWantWinMoneyA1;		/**<机器人输赢控制：机器人赢钱区域1  */
	__int64		m_iAIWantWinMoneyA2;		/**<机器人输赢控制：机器人赢钱区域2  */
	__int64		m_iAIWantWinMoneyA3;		/**<机器人输赢控制：机器人赢钱区域3  */
	int			m_iAIWinLuckyAtA1;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	int			m_iAIWinLuckyAtA2;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	int			m_iAIWinLuckyAtA3;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	int			m_iAIWinLuckyAtA4;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	__int64		m_iAddMoneyEveryDay;		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	bool		m_bChangeRobotCard[PLAY_COUNT];	/**<是否改变了机器人的牌 true 改变了 false 没有改变  */
	__int64     m_i64AcculateGrade[PLAY_COUNT];//玩家累积的成绩
	__int64     m_i64UpGrade[PLAY_COUNT];//上一局的成绩
#ifdef SUPER_VERSION
	BYTE					m_bSuperCardList[8][54];				/**< 超级客户端 */
	int						m_iSuperCardCount[8];					/**<  */
	int						m_bSuperStation;						/**< 超级客户端位置 */
	struct SetCardStruct											/**< 客户端到服务器 */
	{
		BYTE pai[8][54];
		int	paicount[8];
	};
#endif
public:
	static BYTE m_iAllCardCount;									/**< 游戏所用的牌数 */
	__int64 m_iLimitNote;											/**< 上限 */
	__int64 m_iBaseNote;											/**< 底注 */
	__int64 m_iBaseNoteEx;											/**< 底注 */
	int     m_iBaseScale;                                           /**< 自动底注比率 */
	int		m_iFirstSendCardCount;									/**< 第一次发的牌张数 */
	BYTE    m_bShowHa;                                              /**< 第几轮可以梭哈 */    
	BYTE    m_byAddNum;												/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
	BYTE    m_bPK;                                                  ///是否PK PK功能，有一方梭哈了，另一方只能跟，不能放弃
	//扩展
	static 	BYTE m_iSendCardTime;									/**< 游戏思考时间 */
	static 	BYTE m_iThinkTime;										/**< 游戏思考时间 */
	static 	BYTE m_iBeginTime;										/**< 游戏开始时间 */
	static 	DWORD m_dwCardShape;									/**< 牌型例表 */

	//函数定义
public:
	/// 构造函数
	CServerGameDesk(); 
	/// 析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	/// 初始游戏
	virtual bool InitDeskGameStation();

	/// 游戏开始
	/// @param bBeginFlag 开始方式
	virtual bool GameBegin(BYTE bBeginFlag);
	
	/// 设置上限和底注
	BOOL SetLimitNote();
	
	/// 游戏结束
	/// @param bDeskStation 触发游戏结束的玩家ID
	/// @param bCloseFlag 游戏结束的方式
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	
	/// 判断是否正在游戏
	/// @param bDeskStation 玩家ID
	virtual bool IsPlayGame(BYTE bDeskStation);

	/// 框架消息处理函数
	/// @param bDeskStation 玩家ID
	/// @param pNetHead 消息头
	/// @param pNetData 消息内容
	/// @param uDataSize 消息大小
	/// @param bWatchUser 旁观
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	
	/// 游戏数据包处理函数
	/// @param bDeskStation 玩家ID
	/// @param pNetHead 消息头
	/// @param pNetData 消息内容
	/// @param uDataSize 消息大小
	/// @param bWatchUser 旁观
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	
	/// 用户离开游戏桌
	/// @param bDeskStation 玩家ID
	/// @param pUserInfo 玩家信息
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);

	/// 更新台主
	virtual void ChangeDeskOwer(BYTE bDeskStation);

public:
	//读取静态变量
	BOOL LoadIni();
	//根据房间ID加载底注和上限
	//@param iRoomID 房间ID 根据不同房间读取不同的配置
	BOOL LoadExtIni(int iRoomID);

	//重载纯虚函数
public:
	/// 获取游戏状态信息
	/// @param bDeskStation 玩家ID
	/// @param uSocketID 游戏可不管
	/// @param bWatchUser 是否旁观
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	/// 重置游戏状态
	/// @param bLastStation 重置的状态
	virtual bool ReSetGameState(BYTE bLastStation);
	/// 定时器消息
	/// @param uTimerID TimerID
	virtual bool OnTimer(UINT uTimerID);
	
	//处理函数
public:
	/// 发送扑克给用户
	BOOL SendCard();
	/// 發牌結束
	BOOL SendCardFinish();
	/// 游戏开始
	BOOL BeginPlay();
	/// 发送限注
	BOOL SendLimit();
	/// 新一轮开始
	/// @param bDeskStation 玩家ID
	BOOL NewPlayTurn(BYTE bDeskStation);
	/// 发一张新牌
	BOOL SendACard ();
	/// 已经梭哈
	BOOL AlreadyShow();
	/// 发送用户下注
	BOOL SendUserNote();
	/// 用户请求离开
	/// @param bDeskStation 玩家ID
	/// @param szMessage 离开理由
	bool UserHaveThing(BYTE bDeskStation, char * szMessage);
	
	/// 同意用户离开
	/// @param bDeskStation 玩家ID
	/// @param bArgee 是否同意
	bool ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee);
	
	/// 计算升级
	//int GetUpGrade(int iGamePoint, int iPlayCardCount);
	/// 处理用户结果
	bool UserActiveResult(BYTE bDeskStation, BYTE iVerbType,__int64 iNoteVal=0);
	/// 检测是否结束
	BOOL CheckFinish();
	//更新用户所下注
	//void ModifyPlayNote(BYTE bDeskStation);
	
	/// 获取牌最大的位置
	/// @param bExtVal  表示比较明牌还是比较全部牌
	int GetCardGreater(BOOL bExtVal=FALSE);
	
	/// 发牌位置
	BOOL CanSendCard(BYTE bDeskStation);
	/// 统计游戏人数
	BYTE CountPlayer();
	/// 统计游戏人数
	BYTE CountActivePlayer();
	/// 收集筹码
	BOOL CollectJeton(BOOL bOnly=false);
	/// 換牌
	BOOL ChangeBackCard(BYTE bDeskStation,BYTE bSourceCard,BYTE bResultCard);
	/// 隊例中是否有某牌是否替換成功
	BOOL ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard);
	/// 统计倍数(人头)
	__int64 GetHumanBasePoint();
	/// 桌面倍数
	int GetDeskBasePoint();
	/// 房间倍数
	int GetRoomBasePoint();
	/// 逃跑扣分
	int GetRunPublish();
	/// 逃跑扣金币
	/// @param bDeskStation 逃跑玩家ID
	__int64 GetRunPublishMoney(BYTE bDeskStation);
	/// 非强退玩家得分
	/// @param bDeskStation 玩家ID
	int GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation);
	/// 机器人托管
	/// @param bDeskStation 玩家ID
	/// @param bType 1表示断线,2表示强退
	BOOL UserAI(BYTE bDeskStation ,BYTE bType = 1);
	/// 根据游戏判断是否在游戏中
	BOOL  IsPlayingByGameStation() ;
	/// 第一次梭哈者
	BOOL FistShutle();
	/// 判断
	BOOL Judge();
	/// 胜
	BOOL JudgeWiner();
	/// 输
	BOOL JudgeLoser();	
	/// 谁负
	BYTE GetWiner();
	/// 换牌
	BOOL ChangeCard(BYTE bDestStation,BYTE bWinerStation);
	/// 玩家强退
	BOOL UserFourceQuit(BYTE bDeskStation);
	/// 杀所有计时器
	void KillAllTimer();
	/// 玩家坐下
	/// @param pUserSit 坐下位置的桌子信息
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	/// 桌主
	virtual long GetTableOwnerID(){return m_dwDeskOwer;};
	//获取房间的基本倍数
	int  GetRoomLimitBasePoint();

	//判断是否超级用户
	bool IsSuperUser(BYTE station);//lym 2011-11-28
	//生成随机数
	int MyRand();
	//获取权重随机值
	int GetWeight(unsigned int iPoint[], int iCount);
	//机器人控件输赢相关方法
public:
	void GetAIContrlSetFromIni(int iRoomID);				//从配置文件中读取机器人控制设置的参数值

	void CheckNewDayReSetIni(CINIFile *f, CString *key);	//检查是否已经新的一天了，需要重新设定机器人控制配置参数

	void RecordAiHaveWinMoney(GameEndStruct *GameEnd);		//20121203dwj 记录机器人输赢值

	void IAWinAutoCtrl();									//20121122dwj机器人输赢自动控制,
	
	int GameWinerIsRobot(int &iWiner);						///游戏赢家是否是机器人
	__int64 GetHalfforLessUserMoney();//获得最少金币玩家身上金币的一半金币
	int getFirstGreate(void);
};
/******************************************************************************************************/
