#pragma once

#include "GameDesk.h"
//#include "../GameMessage/UpGradeLogic.h"
#include "GameTaskLogic.h"
#include <afxmt.h>
#include <deque>
#include<list>
#include<map>
//定时器 ID
#define IDT_USER_CUT				1L				//断线定时器 ID

//#define TIME_SEND_CARD				30				//发牌定时器

//#define TIME_SEND_CARD_ANI      	32				//等待反牌
//#define TIME_SEND_CARD_FINISH       33              //发牌结束
//#define TIME_ROB_NT					34				//抢地主
//#define TIME_ADD_DOUBLE				35				//加棒
//#define TIME_OUT_CARD				36				//出牌
//#define TIME_WAIT_NEWTURN			37				//新一轮时间设置
//#define TIME_GAME_FINISH			38				//游戏结束定时器
//#define TIME_JIAO_FEN               39              //叫分定时器
//#define TIME_SHOW_CARD              40              //亮牌

#define	TIME_APPLY_NT				33				//申请上庄计时器
#define TIME_SEND_CARD              34               //发牌定时器
#define TIME_START_GAME				41				//比赛开始时间设置


//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SAFE						11				//游戏安全结束
#define GF_NO_CALL_SCORE			13				//无人叫分
#define GF_TERMINATE				14				//意外停止游戏
#define GF_AHEAD_END				15				//提前结束

//*****************************************************************************************
//游戏桌类
class CServerGameDesk : public CGameDesk
{
private:
	CGameTaskLogic          m_Logic ;								//游戏中的逻辑（包含任务逻辑）

	static 	bool  m_bHaveKing;      								//是否有王(DEF=1,0)
	static 	BOOL  m_bKingCanReplace;								//王可否代替牌(DEF=1,0)
	static 	bool  m_bRobnt;         								//是否可以抢地主
	static 	bool  m_bAdddouble;    									//是否可以加棒
	static 	bool  m_bShowcard;     									//是否可以加倍
	static 	UINT  m_iPlayCard;     									//所有扑克副数(1,DEF=2,3
	static 	UINT  m_iPlayCount;    									//使用扑克数(52,54,104,DEF=108)
	static 	UINT  m_iSendCount;    									//发牌数(48,51,DEF=100,108)
	static 	UINT  m_iBackCount;    									//底牌数(3,6,DEF=8,12)
	static 	UINT  m_iUserCount;    									//玩家手中牌数(12,13,DEF=25,27)
private://游戏配置项
	BYTE			m_byFreeTime;									//空闲准备时间
	BYTE			m_byBetTime;									//下注时间
	BYTE			m_byOpenCardTime;								//开牌时间
	BYTE			m_bySendSpaceTime;								//发牌间隔时间
	BYTE			m_bySpeedTime;									//遮挡牌速度
	BYTE			m_byShowWinTime;								//判断输赢
	BYTE			m_iPlayerCount;									//游戏把数
	BYTE			m_byEvent;										//游戏局数
	BYTE			m_byOpenTime;									//开奖时间
	BYTE			m_byShowResultTime;								//结算时间
	BYTE			m_byShowResult;									//结算显示时间
	BYTE			m_bySendCardTime;								//发牌时间
	BYTE			m_byWinArea;									//中奖区域,0代表龙赢，1代表虎赢，2代表打和
	BYTE			m_byMaxNtPeople;								//上庄人数限制
	BYTE			m_iGameEven;									//多少把为一局
	BYTE            m_iRobotWinLuckyAtA1;							//<机器人在区域1赢钱的概率  
	BYTE			m_iRobotWinLuckyAtA2;							//<机器人输赢控制：机器人在区域2赢钱的概率  
	BYTE			m_iRobotWinLuckyAtA3;							//<机器人输赢控制：机器人在区域3赢钱的概率  
	BYTE			m_iRobotWinLuckyAtA4;							//<机器人输赢控制：机器人在区域4赢钱的概率  
	//BYTE			m_iRobotNtCountLimit;							//机器人最多上庄个数

	int				m_iApplyNtLimite;								//上庄条件
	int				m_iNtPlayLimite;								//庄家坐庄次数限制
	
	__int64			m_iAreaBetLimite;								//区域下注限制
	__int64			m_i64UserBetCount[PLAY_COUNT][BET_ARES];		//各玩家在每个区域的下注情况
	__int64			m_i64AreaBetAllSum[BET_ARES];					//各区域的总下注
	__int64			m_i64UserBetSum[PLAY_COUNT];					//各玩家的总下注
	__int64			m_i64SumAllBet;									//总下注数

	__int64         m_iRobotWinMoneyA1;								//<机器人赢钱区域1  
	__int64         m_iRobotWinMoneyA2;								//<机器人赢钱区域2  
	__int64         m_iRobotWinMoneyA3;								//<机器人赢钱区域3  
	__int64         m_iRobotAddMoneyEveryDay;						//<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  
	__int64         m_iResetRobotHaveWinMoney;						//<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  
	__int64			m_iRobotMaxWinInOne;							//机器人平均每把最大赢钱数	（小于等于0时为不限）
private:
	bool			m_bGameIsRun;									//游戏是否已经启动
	bool			m_bGameIsPlaying;								//游戏是否已经开始了
	bool			m_bIsCanBet;									//是否可以下注
	bool			m_bIsSuperControl;								//是否是超端用户控制结果
	BOOL			m_bIsRobotWinAndLostAutoCtrl;					//是否开启机器人输赢自动控制

	__int64			i64NtGrade;										//庄家成绩

	__int64			i64PlayerGrade[PLAY_COUNT];						//闲家成绩
	__int64			i64PlayCoin[PLAY_COUNT];						//玩家输赢金币
	__int64			i64ChangeMoney[PLAY_COUNT];						//玩家得分
	
	__int64			i64ReturnMoney[PLAY_COUNT];						//返还的金币数
	
	__int64			m_i64NtHaveWin;									//庄家输赢情况
	
	BYTE			m_byNtStation;									//庄家位置
	BYTE			m_byCurrResult;									//当局开奖结果
	BYTE			m_byLastResult;									//上局开奖结果
	//超端用户信息
	BYTE			m_bySuperReslut;								//超端用户控制的结果
	BYTE			iCardArray[52];									//要发的牌
	BYTE			m_sendCardCount;								//发牌张数
	BYTE			m_byRemaindTime;								//服务端剩余时间
	BYTE			SendCardCount;									//发送牌的数量
	BYTE            m_bRobotNtCount;								//目前机器人上庄个数

	vector <long>	m_vlSuperID;									//保存超端玩家的容器
	list<BYTE>      m_blNtWaitList;									//上庄列表
	deque <BYTE>	m_dbyLuzi;										//路子双端队列
	CTime			m_RecordTime;									//当前游戏日期

	int				m_iClock;										//时钟
	int				m_iNtPlayCount;									//庄家坐庄次数
	
	//机器人下注信息
	__int64         m_RobotAreaBetSum[PLAY_COUNT][BET_ARES];		//机器人在每个区域的下注数
	__int64			m_RobotAreaAllSum[BET_ARES];					//每个区域机器人下注总数
	__int64			m_RobotBetSum[PLAY_COUNT];						//每个机器人的下注总数
	__int64			m_RobotAllSum;									//机器人下注总数
protected:
	GameMutipleStruct       m_GameMutiple ;							//游戏中的倍数
	//函数定义
public:
	//构造函数
	CServerGameDesk(); //服务器启动时第一次调用
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:

	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	
    ///拦截窗口消息（客服端启动时第二次调用）
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	
	//发送取消机器人托管
	virtual BOOL SetAIMachine(BYTE bDeskStation,BOOL bState = FALSE);


	//游戏结束(客服端退出时调用)
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	
	//判断是否正在游戏
	virtual bool IsPlayGameEx(BYTE bDeskStation);

	//判断是否正在游戏（客服端退出时第二次调用）
	virtual bool IsPlayGame(BYTE bDeskStation);

	//用户离开游戏桌(客服端退出时第一次调用)
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//重载纯虚函数
public:

	//获取游戏状态信息（客服端启动时第三次调用）
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//重置游戏状态（客服端退出时调用）
	virtual bool ReSetGameState(BYTE bLastStation);

	//定时器消息
	virtual bool OnTimer(UINT uTimerID);

	//配置INI文件（服务器启动时第二次调用）
	virtual bool InitDeskGameStation();
	//处理函数
public:

	//服务端自动开始不由服务端控制
	BOOL StartGame();

	//初始化数据
	void InitGameData();

	//游戏准备工作
	void GamePrepare();

	//读取静态变量
	BOOL LoadIni();

	//游戏开始
	BOOL BeginPlay();

	 //发牌
	void  SendCard();     

	//殺所有計時器
	void KillAllTimer();

	//重置所有数据
	void ResetGameData();

	//一次将所的牌全部发送
	BOOL SendAllCard();

	//發送結束
	BOOL SendCardFinish();

	//发送结算消息
	void sendResult();

private:
	//向客服端发送下注阶段消息
	void sendBetMsg();

	//获取玩家剩余多少钱
	__int64	GetUserRemaindMoney(BYTE byDeskStation);

	//获取玩家下注总额
	__int64	GetUserBetCount(BYTE byDeskStation);

	//处理玩家申请上庄消息
	void DealUserApplyNt(void *pBuffer);

	//处理玩家申请下庄消息
	void DealUserApplyLeaveNt(void *pBuffer);

	//处理玩家下注
	void DealUserBet(void *pBuffer);
	
	//是否在上庄列表中
	bool IsInWaitNtList(BYTE byDeskStation);

	//判断输赢
	BYTE DealWinner();

	//发送显示结果消息
	void sendShowResult();

	//启动服务端定时器
	void launchTimer();

	//将扑克转换成数字大小
	void trandformNum(BYTE card[]);

	//统计玩家金币
	BOOL accountPlayCoin(BYTE whoWin,__int64 i64PlayCoin[]);

	//得到区域剩余下注数
	__int64 RemainingBettingNumber(BYTE);

	//超级用户验证
	void SuperUserExamine(BYTE bDeskStation);

	//是否超端用户 超端用户要发送结果给他看到
	bool IsSuperUser(BYTE byDeskStation);

	//处理超端用户控制结果
	void DealSuperUserMessage(void *pBuffer);

	//游戏结算
	void DoGameResult(__int64 i64TurePoint[]);

	//交换扑克
	void ExchangePoker(BYTE card[],int para);

	//机器人自动控制
	BYTE RobotAutoControl();

	//机器人自动控制选择合适的中奖区域
	BYTE RobotAutoSelectWinArea();

	//临时计算开各个奖机器人和玩家各方的总得分
	__int64 TempAccountRobotAndRealPlayer(BYTE byWinArea,BYTE para);

	//调试信息
	void PrintLog();
	///判断此游戏桌是否开始游戏
	//virtual BOOL IsPlayingByGameStation();
	/*返回false：调用UserNetCut
	返回true：调用CanNetCut
	*/
	//玩家断线处理（客服端退出时调用）
	//virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	//用来改变用户断线条件的函数
	//virtual bool CanNetCut(BYTE bDeskStation);//JAdd-20090116
};

/******************************************************************************************************/
