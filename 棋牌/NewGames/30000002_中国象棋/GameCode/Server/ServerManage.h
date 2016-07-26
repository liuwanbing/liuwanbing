#pragma once

#include "StdAfx.h"

/// 定时器 ID
#define TIME_SEND_CARD				30				///< 发牌定时器
#define TIME_WAIT_NT				31				///< 等待反牌
#define TIME_GAME_FINISH			32				///< 游戏结束定时器

/// 操作掩码
#define	UG_HUA_MASK					0xF0			///< 1111 0000
#define	UG_VALUE_MASK				0x0F			///< 0000 1111

/// 扑克花色
#define UG_FANG_KUAI				0x00			///< 方块	0000 0000
#define UG_MEI_HUA					0x10			///< 梅花	0001 0000
#define UG_HONG_TAO					0x20			///< 红桃	0010 0000
#define UG_HEI_TAO					0x30			///< 黑桃	0011 0000
#define UG_NT_CARD					0x40			///< 主牌	0100 0000
#define UG_ERROR_HUA				0xF0			///< 错误  1111 0000

/// 扑克出牌类型
#define UG_ERROR_KIND				0				///< 错误
#define UG_ONLY_ONE					1				///< 单牌
#define UG_DOUBLE					2				///< 对牌
#define UG_THREE					3				///< 三条
#define UG_DOUBLE_TRACKOR			4				///< 两条拖拉机
#define UG_THREE_TARCKOR			5				///< 三条拖拉机
#define	UG_SHOW_CARD				6				///< 甩牌

/// 扑克数目定义
#define PLAY_TWO_CARD				1				///< 两副扑克
#define PLAY_THREE_CARD				2				///< 三副扑克

/// 游戏结束标志定义
#define GF_NORMAL					10				///< 游戏正常结束
#define GF_SALE						11				///< 游戏安全结束


/******************************************************************************************************/

enum XQ_PLAY_FLAG
{
	XQ_PLAY_FLAG_RED = 0,
	XQ_PLAY_FLAG_BLACK = 1
};


/// 象棋
#define XQ_FLAG_NONE				0
#define XQ_FLAG_RED					1
#define XQ_FLAG_BLACK				-1

#define XQ_QIZI_NONE				0
#define XQ_CROSS					8

/// 红方
#define XQ_RED_JU					1
#define XQ_RED_MA					2
#define XQ_RED_PAO					3
#define XQ_RED_SHI					4
#define XQ_RED_XIANG				5
#define XQ_RED_JIANG				6
#define XQ_RED_BIN					7

/// 黑方
#define XQ_BLACK_JU					-1
#define XQ_BLACK_MA					-2
#define XQ_BLACK_PAO				-3
#define XQ_BLACK_SHI				-4
#define XQ_BLACK_XIANG				-5
#define XQ_BLACK_JIANG				-6
#define XQ_BLACK_BIN				-7

#define XQ_MAX_I					(10)
#define XQ_MAX_J					(9)

/******************************************************************************************************/
/******************************************************************************************************/

/// 升级逻辑类 （支持 2 副和 3 副扑克）
class CUpGradeGameLogic
{
	/// 变量定义
private:
	
	/// 函数定义
public:
	/// 构造函数
	CUpGradeGameLogic(void);	

	/// 功能函数（公共函数）
public:

	/// 初始化棋局
	void InitBoard(int iMeFlag, int qi[][9]);
	/// check win or lose
	BOOL CheckWinOrLose(int iMeFlag, int qi[][9], BOOL &bIfIWin);
	
private:
	/// win
	BOOL Win(int iMeFlag, int qi[][9]);
	/// lose
	BOOL Lose(int iMeFlag, int qi[][9]);
	
};

//*****************************************************************************************

/// 游戏桌类
class CServerGameDesk : public CGameDesk
{
protected:
	/// 游戏基础数据
	BYTE					m_CardCount;						///< 扑克数目（位变量）
	/// 玩家设置数据
	BYTE					m_iLessCount;						///< 最少游戏局数
	BYTE					m_iPlayCardCount;					///< 游戏扑克数目
	BYTE					m_iBeginStation;					///< 游戏开始位置
	BYTE					m_iEndStation;						///< 游戏结束位置
	BYTE					m_iEnableWatch;						///< 是否允许旁观

	int						m_iJuTimeMinute,					///< 局时
							m_iJuTimeSecond,
							m_iBuTimeMinute,					///< 步时
							m_iBuTimeSecond,
							m_iDuMiaoMinute,					///< 读秒
							m_iDuMiaoSecond,
							m_iGameMoney;						///游戏指导费

	int						m_iBotJuUseTime[PLAY_COUNT];		///< 游戏用时
	int						m_iBotBuUseTime[PLAY_COUNT];
	int						m_iBotDuUseTime[PLAY_COUNT];

	int m_iStartI,m_iStartJ,m_iEndI,m_iEndJ ;

	/// 运行状态变量
	int						m_PunishPoint[4];					///< 用户罚分
	int						m_iUpGradePeople;					///< 庄家位置
	int                     m_iBgePeaceCount[2];                ///< 求和次数
	BYTE					m_iPlayNTNum[2];					///< 各玩家的主牌数目
	BYTE					m_iBeenPlayGame;					///< 已经游戏的局数
	BYTE					m_iLeaveArgee;						///< 离开同意标志

	/// 反牌信息
	int						m_iNtPeople;						///< 反牌玩家
	BYTE					m_iNtCardCount;						///< 反牌牌数

	/// 状态信息
	BYTE					m_iUserCardCount[4];				///< 用户手上扑克数目
	BYTE					m_iUserCard[4][45];					///< 用户手上的扑克
	BYTE					m_iBackCard[8];						///< 底牌列表

	/// 运行信息
	int						m_iGamePoint;						///< 用户游戏分数
	int						m_iOutCardPeople;					///< 现在出牌用户
	int						m_iFirstOutPeople;					///< 先出牌的用户
	BYTE					m_iBaseOutCount;					///< 出牌的数目
	BYTE					m_iDeskCardCount[4];				///< 桌面扑克的数目
	BYTE					m_iDeskCard[4][39];					///< 桌面的扑克

	/// 发牌数据
	BYTE					m_iSendCardPos;						///< 发牌位置

	CUpGradeGameLogic		m_Logic;							///< 游戏逻辑
	bool					m_bQueryQuit;						///< 有玩家请求退出


	int						m_iRedBoard[10][9];					///< 红方棋局
	int						m_iBlackBoard[10][9];				///< 黑方棋局
	
	int						m_iWinFlag;							///< 获胜方
	int						m_iTurePoint[2];					///< 真实得分

	bool					m_bFirstRed;
	bool					m_bQiuHe;
	bool                    m_bRequireCoachMoeny;                ///是否需要指导费

	int						m_iRedLastQiZi[10][9];				///< 红方上步棋局
	int						m_iBlackLastQiZi[10][9];			///< 黑方上步棋局
	int						m_iLastPlay;						///< 上步走棋者
	int						m_iLastDeskStation;					///< 上步走棋者的位置
	int						m_iWatchStation;					///< 旁观的
	int 					m_iCurPlayFlag;						///< 当前走棋的
	char                    m_bStationRegret;                  ////<是否处在悔棋状态

	/// 玩家已经走棋的步数        
	/// 规定红方步数为m_iPutChess[1],黑方为 m_iPutChess[0]
	int                     m_iPutChess[2];

	int						m_firstPlay;						///<是否为第1个同意玩家
	int                     m_daoshi_money;						///<导师费
	int						m_daoshi;							///<谁是导师
	/// 函数定义
public:
	/// 构造函数
	CServerGameDesk(); 
	/// 析构函数
	virtual ~CServerGameDesk();
	//配置INI文件
	virtual bool InitDeskGameStation();
	//加载限注
	BOOL LoadExtIni();
	//根据房间ID加载底注和上限
	BOOL LoadExtIni(int iRoomID);

	/// 重载函数
public:
	/// 游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	/// 游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	/// 判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	/// 游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);

	/// 重载纯虚函数
public:
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	/// 重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);
	/// 处理函数
public:
	/// 用户设置游戏
	BOOL UserSetGame(UserArgeeGame * pGameSet,byte desk);
	/// 发送扑克给用户
	BOOL SendCard();
	/// 用户反牌
	BOOL UserNtCard(BYTE bDeskStation, int iHuaKind, int iCardCount);
	/// 准备工作
	BOOL GameReady();
	/// 游戏开始
	BOOL BeginPlayUpGrade();
	/// 新加函数，用于设置用户下子后的步数等数据设置
	BOOL UserOutCard(BYTE bDeskStation,PlayChessStruct * playChessInfo);
	/// 用户出牌
// 	BOOL UserOutCard(BYTE bDeskStation,int iStartI,int iStartJ,int iEndI, int iEndJ,int iCurPlay, int iBoard[][9]);
	/// 能否结束(下子者自己检测)
	BOOL CanFinish(BYTE PutStoneDeskStation,BOOL bCanFinish = false);
	/// 服務端來統計是否結束
	BOOL IsFinish(int iEndI,int iEndJ,int Value);
	/// 发送底牌
	BOOL SendBackCard();
	/// 用户上交底牌
	BOOL UserGiveBackCard(BYTE bDeskStation, BYTE iBackCard[], int iCardCount);
	/// 新一轮开始
	BOOL NewPlayTurn(int iWiner);
	/// 用户请求离开
	BOOL UserHaveThing(BYTE bDeskStation, char * szMessage);
	/// 同意用户离开
	BOOL ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee);
	/// 计算升级
	int	GetUpGrade(int iGamePoint, int iPlayCardCount);
	/// 房间倍数
	int GetRoomBasePoint();
	/// 桌子倍数
	int GetDeskBasePoint();
	/// 人头倍数
	int GetHumanBasePoint();
	/// 逃跑扣分
	int GetRunPublish();
	/// 获取本局分数
	int GetTruePoint(BYTE bDeskStation);
};

/******************************************************************************************************/
