#pragma once

#include "StdAfx.h"
#include"..\GameMessage\UpgradeLogic.h"
#include "GameServerData.h"

#define	CARDCOUNT						52			//使用的扑克数量（不要大小王）
//定时器 ID
#define TIME_BET						30				//下注定时器
#define TIME_SENDCARD					31				//发牌定时器
#define TIME_CALL						32				//叫分定时器
#define TIME_FINISH						33				//游戏结束时间
#define TIME_STARTGAME					34				//开始游戏定时器
/*
//游戏配置信息
#define PLAY_MAX_NOTE				500			//最大下注
#define PLAY_BASE_NOTE				5				//底注
#define THINK_TIME					30				//游戏思考时间
#define PLAY_CARD_COUNT				1				//扑克副数

*/

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束

//玩家状态定义
#define PLAYER_READY						299				//用户准备状态
#define PLAYER_GIVEUP						301				//放弃
#define PLAYER_JOIN							302				//中途加入状态

static __int64					m_iAIHaveWinMoney;	//机器人赢钱数20121126	dwj 这里声明为静态全局变量，因为多个桌子需要及时修改该变量
static __int64					m_iReSetAIHaveWinMoney;	/**<机器人输赢控制：重置机器人已经赢钱的数目  该变量也要声明为静态全局变量 否则每个桌子都会去重置*/
static int						m_iRecFroDayTime;	/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */

const BYTE bCardA[4]={0x0D,0x1D,0x2D,0x3D};
const BYTE bCardB[16]=
{0x09, 0x0A, 0x0B, 0x0C,
 0x19, 0x1A, 0x1B, 0x1C,
 0x29, 0x2A, 0x2B, 0x2C,
 0x39, 0x3A, 0x3B, 0x3C
};

//#define SEND_CARD_TIME				1000			//发牌时间设置测试用10毫秒,正常用1000

//游戏桌类
class CServerGameDesk : public CGameDesk
{
	//21点
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//成员变量
private:
	CUpGradeGameLogic		m_Logic;							//游戏逻辑
	GameServerData          m_GameData;							//游戏数据

private:
	//游戏扩展配置
	static BYTE m_bGameBeginWaitTime;									//游戏开始等待时间
	static BYTE m_bGameBetTime;											//游戏下注时间
	static BYTE m_bCallCardTime;										//游戏叫牌时间
	static BYTE m_bSendCardTime;										//游戏发牌时间
	static BYTE m_bFinishTime;											//游戏开奖时间
	//运行状态变量
	bool					m_bIsSuperControl;						//是否是超端用户控制结果
	int						m_iNtPeople;							//庄家位置
	int						m_iGameStation;							//游戏状态
	int                     m_iGameFlag;							//当前游戏分状态
	int						m_iPlayerStation[PLAY_COUNT];			//玩家状态
	bool                    m_bIsCut[PLAY_COUNT];					//是否掉线了主要为机器人设置

	int						m_bUserReady[PLAY_COUNT] ;				//玩家准备状态
	int						m_iCallLabel;							//叫分标签
	int						m_iNeedLabel;							//要牌标签
	int						m_iScoreNum;							//叫牌人数
	BYTE					m_bUserCard[PLAY_COUNT][13];			//用户扑克
	BYTE					m_bUserAlreadySendCard[PLAY_COUNT][13];	//向用户已经发的扑克
	BYTE					m_bScoreSqueue[PLAY_COUNT];				//叫牌顺序
	BYTE					m_bCurrentCallPeople;					//当前叫分者
	BYTE					m_bLastSuperStation;					//最后控制的超端用户
	int						m_iUserPoints[PLAY_COUNT];				//用户点数
	__int64					m_i64UserScore[PLAY_COUNT];				//用户输赢金币
	vector<long>			m_vlSuperID;									//保存超端玩家的容器
	
	//配置变量
private:
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

	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);

public:
	///清理所有计时器
	void KillAllTimer();

	//检测是否结束
	//bool CheckFinish();

	//初始化服务器游戏数据
	BOOL InitData();

	//读取静态变量
	BOOL LoadIni();	

	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);

	//玩家AI设置
	BOOL UserAI(BYTE bDeskStation,BYTE bCutType,bool bNet);

	//发送游戏状态
	//等待同意状态
	void OnSendGameStationAgree(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//下注状态
	void OnSendGameStationBet(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//发牌状态
	void OnSendGameStationSendCard(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//游戏中状态
	void OnSendGameStationPlaying(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//等待下一盘游戏开始
	void OnSendGameStationNext(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//游戏消息处理

	//按钮消息服务函数
	void OnButtonMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);

	//处理超端用户控制结果
	void	DealSuperUserMessage(void *pBuffer);

	//向客服端发送游戏消息函数

	//发送确定庄家消息
	void SendHaveNt();

	//选庄家
	int HaveNt(BYTE bNtStation);

	//开始下注
	void StartNote();

	//开始发牌
	void SendCard();

	//计算下注上限和下限
	__int64 AccountLimit();

	//检查是否能发牌
	void CheckSend();

	//开始游戏
	void StartGame();

	//判断点数
	int GetPoints(BYTE bCardList[],int iSize);

	//超级用户验证
	void	SuperUserExamine(BYTE bDeskStation);

	//是否超端用户 超端用户要发送结果给他看到
	bool	IsSuperUser(BYTE byDeskStation);

	//统计游戏人数
	BYTE   CountPlayer();

	//统计断线人数
	BYTE   CountCut();
};

/******************************************************************************************************/
