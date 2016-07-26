#pragma once
#include "GameDesk.h"
#include <afxmt.h>
#include "StdAfx.h"
#include"..\GameMessage\UpgradeLogic.h"
#include<vector>
//定时器 ID
#define TIME_CALL_NT				30				//叫庄
#define TIME_NOTE                   31              //下注计时器
#define TIME_NOTE_FINISH			32				//下注完成定时器
#define TIME_SEND_CARD				33				//发牌定时器
#define TIME_TAKE_CARD				34				//要牌计时器
#define TIME_GAME_FINISH			35				//游戏结束定时器
#define TIME_LOST_TIME				36				//遗失时间(每秒执行一次)

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束


static __int64					m_iAIHaveWinMoney;	//机器人赢钱数20121126	dwj 这里声明为静态全局变量，因为多个桌子需要及时修改该变量
static __int64					m_iReSetAIHaveWinMoney;	/**<机器人输赢控制：重置机器人已经赢钱的数目  该变量也要声明为静态全局变量 否则每个桌子都会去重置*/
static int						m_iRecFroDayTime;	/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */


//游戏桌类
class CServerGameDesk : public CGameDesk
{
protected:
	//运行状态变量
	int						m_iNtPeople;                          //赢家
	int						m_iUpGradePeople;					//庄家位置

	BYTE					m_iBeenPlayGame;					//已经游戏的局数
	//BYTE					m_iLeaveArgee;						//离开同意标志
	BYTE					m_iHaveThingPeople;					//有事要走玩家
	
	bool                    m_bUserReady[PLAY_COUNT] ;         ///玩家准备状态
	BOOL					m_bReadyOpenCard[PLAY_COUNT];			//停牌状态
	//------------------------------------------------------------------------------------
	//状态信息
	BYTE					m_iUserCardCount[PLAY_COUNT];					//用户手上扑克数目
	BYTE					m_iUserCard[PLAY_COUNT][SH_USER_CARD];			//用户手上的扑克
	BYTE					m_byGetCardCount[PLAY_COUNT];					//已经亮牌的数量
	BYTE					m_byCanGetCard[PLAY_COUNT];						//是否可以要牌的标志位
	int						m_iCallScore[PLAY_COUNT];						//叫庄状况


	//发牌数据
	BYTE					m_iTotalCard[54];					//总的牌

	CUpGradeGameLogic		m_Logic;							//游戏逻辑

	int						m_iStopCardCount;                  //停牌玩家的数量

	int                     m_MaxPlayer;                        //下注闲家的数量;
	int						m_iUserStation[PLAY_COUNT];				//记录用户状态
	int						m_iSuperCount;							//超端数量
	__int64					m_iPerJuTotalNote[PLAY_COUNT];			//用户每局压总注

	bool                    m_bIsCut[PLAY_COUNT];              //是否掉线了主要为机器人设置
	BYTE					m_iIngPlayNote;					//当前下注者（给旁观）
	BYTE					m_iSendPos;				//新一轮发牌开始位置
	BYTE                    m_iPrepareNT;              //预备庄家
	BYTE					m_bCurrentCallScore;		//当前叫分者
	BYTE                    m_iFirstJiao;
	BYTE                    m_iGameFlag;             //当前游戏分状态
	//BOOL                    m_bIsSuper[PLAY_COUNT];   //是否可以使用发牌

	//超端设置
	SuperUserSetData		m_SuperSetData;				//超端设置的数据

	//__int64					m_iAIHaveWinMoney;		 //机器人赢钱数20121122	dwj
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



private:
	__int64 m_iLimitNote[PLAY_COUNT];	//上限
	__int64	m_i64UserNoteLimite[PLAY_COUNT][4];	//玩家四个下注数字
	__int64 m_iLimit;//配置上限

	//=============扩展
	static 	int m_iSendCardTime;//游戏思考时间
	static 	BYTE m_iThinkTime;//游戏思考时间
	static 	BYTE m_iBeginTime;//游戏开始时间
	static 	BYTE m_iCallScoreTime;//叫庄时间
	static 	BYTE m_iXiaZhuTime;  //下注时间
	static  BOOL m_bTurnRule;	//游戏顺序
	BYTE	m_byLostTime;		//记录定时器剩余时间
	DWORD m_iCardShape;	//牌型

	/// 牌型倍率
	BYTE m_bCardShapeBase[MAX_SHAPE_COUNT];//0-5代表爆点到人小五间的倍率

	int  m_iGoodCard;//发大牌机率十点以上
	bool m_bHaveKing;//是否有王


	//超端数据 
	vector<long>			m_vlSuperID;							//保存超端玩家的容器


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
	virtual bool	GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool	GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
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
//	virtual bool UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo);
	//修改奖池数据1(平台->游戏)
	virtual bool SetRoomPond(bool	bAIWinAndLostAutoCtrl);

	//修改奖池数据2(平台->游戏)
	virtual bool SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[]);

	//修改奖池数据3(平台->游戏)
	virtual bool SetRoomPondEx(__int64	iReSetAIHaveWinMoney);
	//控制胜负
	/// 判断
	virtual BOOL Judge();
	/// 胜
	virtual BOOL JudgeWiner();
	/// 输
	virtual BOOL JudgeLoser();
	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//20121203dwj 记录机器人输赢值
	void RecordAiHaveWinMoney(GameEndStruct *GameEnd);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//处理函数
public:
	//读取静态变量
	BOOL	LoadIni();	

	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);


	//-------ADD-BY-DWJ-20121108--------------------------------
	//检查是否已经新的一天了，需要重新设定机器人控制配置参数
	void CheckNewDayReSetIni(CINIFile *f, CString *key);

	/**从配置文件中读取机器人控制设置的参数值*/
	void GetAIContrlSetFromIni(int iRoomID);			
	//超级用户验证
	void	SuperUserExamine(BYTE bDeskStation);
	//是否超端用户 超端用户要发送结果给他看到
	bool	IsSuperUser(BYTE byDeskStation);

	//统计游戏人数
	BYTE CountPlayer();

	//统计当前未放弃玩家
	BYTE CountNotePeople();

	//房间倍数
	int GetRoomBasePoint();

	//计算各家分数
	int ComputePoint(BYTE DeskStation);
	//得到最大牌和最小牌,win值为1时得到最大,其它为最小
	BYTE GetIndex(int win);

	//换牌
	BOOL ChangeCard(BYTE bDestStation,BYTE bWinStation);
	//交换指定两个玩家的手牌
	void	ChangeTwoUserCard(BYTE byFirstDesk,BYTE bySecondDesk);
	
	//获取下一个玩家位置
	BYTE GetNextDeskStation(BYTE bDeskStation);
	//发送给第一个叫庄
	BOOL	SendCallScore(BYTE bDeskStation);
	//叫庄
	BOOL	UserCallScore(BYTE bDeskStation, int iVal);
	//叫分结束
	BOOL	CallScoreFinish();
	//通知用户下注
	BOOL	NoticeUserNote();
	//处理用户下注结果
	BOOL	UserNoteResult(BYTE bDeskStation, BYTE iVerbType,int iNoteType = 0);
	//发送扑克给用户
	BOOL	SendCard();
	//发牌结束
	BOOL	SendCardFinish();
	//游戏开始
	BOOL	BeginPlayUpGrade();

	//玩家开始游戏（要牌）
	BOOL    UserYaoPai(BYTE byDeskStation,void * pData);
	//停牌
	BOOL    UserStopCard(BYTE byDeskStation,void * pData);

	BOOL	CheckFinish();
	//算出玩家投注最大上限
	__int64 GetLimit(int bDeskStation);
	//统计掉线玩家
	BYTE    CountCutPlayer();
	//玩家AI设置
	BOOL    UserAI(BYTE bDeskStation,BYTE bCutType,bool bNet);

	//清理强退玩家
	void    ClearQuitPlayer();
	///清理所有计时器
	void    KillAllTimer();;

	///获取当前牌中最大的牌型，
	int     GetMaxCardShape(BYTE iCard[], int iCardCount);

	///20121122dwj机器人输赢自动控制,
	void    IAWinAutoCtrl();
	///20121122dwj计算当前牌机器人的输赢钱
	int     CountAIWinMoney();

};

/******************************************************************************************************/
