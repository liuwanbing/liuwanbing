#pragma once

#include "StdAfx.h"
#include"..\\GameMessage\\UpGradeLogic.h"
//定时器 ID
#define TIME_SEND_CARD				30				//发牌定时器
#define TIME_SEND_ALL_CARD			31				//发所有牌
#define TIME_WAIT_NT				32				//等待反牌
#define TIME_GAME_FINISH			33				//游戏结束定时器
#define TIME_WAIT_NEWTURN			35				//新一轮时间设置
#define TIMER_WAIT_JIAOFEN			36				//等待叫分


#define TIME_WAIT_CONTRIBUTE		38				//手工进贡
#define TIME_WAIT_MOVE_BACK_CONTRIBUTE		39		//等待退贡
#define TIME_CHOICE_CARD			40				//选牌
#define TIME_BACK_CONTRIBUTE_WAIT   41              //退贡后等待3秒
#define TIMER_OUT_CARD				42				//每轮出牌计时器 //此后42-45为出牌定时器

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束
#define	GF_TUOXIANG					12				//游戏因投降而结束
#define GF_LOSE						13				//投降
#define GF_NO_CONTINUE				14				//继续游戏
/******************************************************************************************************/

//游戏桌类
class CServerGameDesk : public CGameDesk
{
protected:
	//游戏基础数据
	BYTE					m_iBeenPlayGame;					//已经游戏的局数
	//运行状态变量
	int						m_iDealtPeople;						//发牌位置
	int						m_iNtPeople;						//上贡大牌者
	int						m_iUpGradePeople;					//庄家位置
	int						m_iGrade2;							//庄的同盟
	int						m_iOutCardPeople;					//现在出牌用户
	int						m_iFirstOutPeople;					//先出牌的用户
	BYTE					m_iPlayNtNum[2];					//各玩家的主牌数字
	//状态信息
	BYTE					m_iUserCardCount[4];				//用户手上扑克数目
	BYTE					m_iUserCard[4][54];					//用户手上的扑克
	BYTE					m_iDeskCardCount[4];				//桌面扑克的数目
	BYTE					m_iDeskCard[4][54];					//桌面的扑克

	//运行信息(分牌和奖分)
	BYTE					m_iPointCardList[4][54];			//用户抓分分牌
	int						m_iPointCardCount[4];				//分牌总数
	int						m_iUserGetPoint[4];					//用户获得的分
	int						m_iTempPoint;						//用户游戏分数
	BYTE					m_iBaseOutCount;					//出牌的数目
	BOOL					m_iStateFlag;						//等待埋底状态分解(叫分，叫花,埋底)
	bool                    m_bIsCut[PLAY_COUNT];              //是否掉线了主要为机器人设置
	BYTE					m_iSequence[4];						//各玩家的出牌的先后顺序
	int						m_iJiaofen[4];						//各家叫分
	int						m_PunishPoint[4];					//用户罚分
	int						m_iAwardPoint[4];					//牌形加分
	BYTE					m_iHeads[4][10];					//统计4-8炸个数
	BYTE					m_iBackCard[12];					//底牌
	BYTE					m_ChangeCard[4];					//四张交换的牌
	//发牌数据
	BYTE					m_iSendCardPos;						//发牌位置
	BYTE					m_bCardOfFriend[4];					//友牌

	BYTE                    m_SetCardTemp[PLAY_COUNT][54];          //设置保存配牌
	BYTE                    m_SetCardCountTemp[PLAY_COUNT];          //设置保存配牌

	CUpGradeGameLogic		m_Logic;							//游戏逻辑
	///增加扩展变量
	BOOL					m_bAdvanceFinish;					//提前结束
//////////////////////////////////辅助变量///////////////////////////////////
	BOOL					m_iLastCard[4];						//最后一手牌(提醒其他玩家)
	BYTE					m_iHaveThingPeople;					//有事要走玩家
	BOOL					m_bAutoOutCard[4];					//托管设置
	BYTE					m_iLeaveAgree;						//离开同意标志
	BYTE					m_iLoseAgree;						//投降同意标志
	//保存10局游戏中各玩家上游中游，下游情况
	BYTE					m_iSequenceHistory[255][4];			//出牌顺序历史记录
	BYTE					m_iHistory;							//出牌历史记录数
	BOOL					m_bWhoIsBig;						//上贡中谁上贡的大(第一输家(winer+1)%4,第二输入家(winer+3)%4)
	BYTE                    m_iOverContribute;                  //完成上贡的人数
	BYTE                    m_ContributeCount;                  //要上贡的人数
	BYTE                    m_iWinSequence[PLAY_COUNT][2];      //上游和未游的次数
	BYTE                    m_PlayANumber[2];                   //打A或者是最后一级的次数
	BYTE                    m_ChoiceNumber;                     //统计选牌时已选牌人数
    BYTE					m_iBeginNt;		                    //游戏起始
    BYTE					m_iEndNt;		                    //游戏终点
	BYTE                    m_iTempTime;                       //临时时间

	BOOL                    m_bInSetCard;                      //当前局设置配牌
	BOOL                    m_bCanSetCard;                     //当前局是否可以配置
	


	//获取上游,二游,三游,四游位置
	BYTE GetPostionBySequence(BYTE iSequence);
	//上贡
	BOOL SendContribute();//ShangGong();
	//抗贡
	BOOL GainstContribute(BYTE bLose,int count);//CanKangGong();
	//手工进贡
	BOOL UserContribute(BYTE bDeskStation,BYTE bCard);
	//超时未进贡的
	BOOL SystemContribute();									//系统处理未退贡的
	//进贡完成
	BOOL ContributeFinish();			
	//玩家选牌(谁选牌,选择谁的牌,选牌的值
	BOOL UserChoiceCard(BYTE bDeskStation,BYTE bDest,BYTE bCard);
	//系统选牌
	BOOL SystemChoiceCard();
	//发送退贡
	BOOL SendMoveBackContribute();
	//退贡
	BOOL UserMoveBackContribute(BYTE bDeskStation,BYTE bCard);//TuiGong();
	//超时未退贡的
	BOOL SystemMoveBackContribute();							//系统处理未进贡的
	//退贡完成
	BOOL MoveBackContributeFinish();			
	//静态变量
private:
	static BOOL m_bHaveKing;//是否有王(DEF=1,0)
	static BOOL m_bKingCanReplace;//王可否代替牌(DEF=1,0)
	static UINT m_iPlayCard;//所有扑克副数(1,DEF=2,3
	static UINT m_iPlayCount;//使用扑克数(52,54,104,DEF=108)
	static UINT m_iSendCount;//发牌数(48,51,DEF=100,108)
	static UINT m_iBackCount;//底牌数(3,6,DEF=8,12)
	static UINT m_iUserCount;//玩家手中牌数(12,13,DEF=25,27)
	static BYTE m_iLimitUpGrade;//限制最多升多少级
	static DWORD m_iCardShape1;	//牌型
	static DWORD m_iCardShape2;	//牌型
	//=============扩展
	static 	BYTE m_iThinkTime;//游戏思考时间
	static 	BYTE m_iBeginTime;//游戏开始时间
	static  BYTE m_EndWaitTime;//游戏结束后等待时间
	static  BYTE m_ContributeTime;//游戏进贡时间
	static  UINT m_iBaseMult;	//游戏倍数

//函数定义
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	/// 判断是否正在游戏
	/// 
	/// @param[in]		BYTE bDeskStation
	/// @return			bool
	virtual bool IsPlayGame(BYTE bDeskStation);
	/// 判断是否正在游戏
	virtual bool IsPlayGameEx(BYTE bDeskStation);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//拦截框架消息
	bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);;
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);

	//
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);

	///用户重新进入
	virtual  bool UserReCome(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);


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
	//初始化一系列工作(重新开始游戏才进行此种初始化)
	//BOOL InitThisGame();
	//读取静态变量
	static BOOL LoadIni();	//准备发牌工作
	BOOL LoadExtIni(UINT uRoomID);   //根据房间ID加载配置文件

	BOOL Ready();
	//全部发送
	BOOL SendAllCard( BYTE iStyle);
	//发送完成
	BOOL SendCardFinish();
	//能否提前结束(有人抓到4个王或者7个头或8个头)
	//BOOL CanAdvanceFinish();
	//游戏开始
	BOOL BeginPlayUpGrade();
	//用户出牌
	BOOL UserOutCard(BYTE bDeskStation, BYTE iOutCard[], int iCardCount);
	////服务端模拟出牌
	BOOL UserAutoOutCard(BYTE bDeskStation);
	//////////////////////////出牌功能函数///////////////////////////////
	//出牌是否为加分牌
	int IsAwardPointCard(BYTE iOutCard[],int iCardCount,BYTE bDeskStation);
	//加分
	BOOL IsGamePoint(BYTE iOutCard[],int iCardCount,BYTE bDeskStation);
	//是否为最后一手牌
	BOOL IsLastCardRemind(BYTE bDeskStation,BYTE iCardList[],int iCardCount);
	//某位置已经出完
	BOOL OutCardFinish(BYTE bDeskStation);
	//检查是否出牌结束
	BOOL CheckFinish();
	//是否为新一轮
	BOOL IsNewTurn();
	//下一个出牌者(当A位置已经出完,他的下家接风,可下家也已经出完,即由下下家接风)
	BYTE GetNextOutCardPeople(BYTE bDeskStation);

	////////////////////////////出牌功能函数完毕///////////////////////////////////
	//新一轮开始
	BOOL NewPlayTurn(int iWiner);

	//清所有计时器
	void KillAllTimer();
public:
	//未出牌时查找王个数
	BOOL FindKingCount();
	//加分判断
	BOOL AwardPoint_OutCard(BYTE iOutCard[],int iCardCount,BYTE bDeskStation);
	//测试是否为同一种花色
	BOOL TestSameColor(BYTE iCardList[],int iCardCount,int iCardNum);
	//是否同一种花色
	BOOL IsSameColor(BYTE iCardList[],int iCardCount);

	///////////////////////////结算相关函数/////////////////////////////
	//雙扣還是單扣或平扣
	int StatWin();
	//统计连续上游局数
	BYTE CountSequence(BYTE bDeskStation ,BYTE FirstOrLast);
	//统计当前一个出完牌者
	BYTE CountSequence(BYTE iFirstOrLast);
	//计算奖分
	int StatAwardPoint(BYTE bDeskStation);
	//////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////游戏辅助功能/////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//托管
	bool UserAuto(BYTE bDeskStation,BOOL bAuto);
	//用户请求离开
	BOOL UserHaveThing(BYTE bDeskStation, char * szMessage);
	//同意用户离开
	BOOL AgreeUserLeft(BYTE bDeskStation, BOOL bArgee);
	//请求提前结束
	BOOL UserStop(BYTE bDeskStation,BOOL bAgree= false);
	//同意提前结束
	BOOL AgreeUserStop(BYTE bDeskStation,BOOL bAgree= false);
	//投降
	BOOL UserLose(BYTE bDeskStation,BOOL bAgree= false);
	//同意投降
	BOOL AgreeUserLose(BYTE bDeskStation,BOOL bAgree= false);
	//购买道俱
	BOOL UserBuyProp(BYTE bDeskStation,BYTE iLevel = 0);
	//统计倍数(人头)
	int GetHumanBasePoint();
	//桌面倍数
	int GetDeskBasePoint();
	//房间倍数
	int GetRoomBasePoint();
	//逃跑扣分
	int GetRunPublish();
	//非强退玩家得分
	int GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation);
	//设置下局主牌数字
	BOOL SetNextNTNum(int iNextNtNum);
	//设置下局庄家位置 
	BOOL SetNextNTStation(int iTurePoint);
	//处理末游问题
	//void LastSequence();
		//统计游戏人数
	BYTE CountPlayer();
		//统计掉线玩家
	BYTE CountCutPlayer();
	//玩家AI设置
	BOOL UserAI(BYTE bDeskStation,BYTE bCutType);
	virtual bool InitDeskGameStation();
};

/******************************************************************************************************/
