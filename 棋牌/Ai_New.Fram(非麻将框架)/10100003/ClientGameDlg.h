#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpgradeMessage.h"
#include "UpGradeLogic.h"
//消息定义
//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_OUT_CARD					103					//出牌等待时间
#define ID_CALL_SCORE_TIME			107					//叫分定时器
#define ID_ROB_NT_TIME				108					//抢地主计时器
#define ID_ADD_DOUBLE_TIME			109					//加棒
#define ID_SHOW_CARD				102
#define ID_LEAVE_TIME				101

/// 预测牌数据
struct guessCard
{
	BYTE data;     // 牌的花色
	BYTE desk;     // 预测此牌属于的位置，如果为255，则代表可能是除了自己之外的任何人
	guessCard(){data=0;desk=255;}
};

#define GAME_RAND_TIME				2					//智能时间
class realData;
//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	friend bool peiCardAnalQiangDiZhu(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);
	friend bool peiCardAnalJiaBei(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);
	friend bool peiCardFirstOutCard(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);
	int						m_iVersion;					//内部开发版本号
	int						m_iVersion2;				//内部开发版本号
	BYTE					m_iPalyCard;				//游戏扑克副数
	int						m_iPlayCount;				//游戏扑克总数
	BYTE					m_iBackCount;				//底牌数
	int						m_iRoomBasePoint;			//房间倍数
	int						m_iRunPublish;				//逃跑扣分

	//	BYTE					m_iBeginNt;					//开始位置
	//	BYTE					m_iEndNt;					//结束位置 
	//=====================时间表信息===================
	BYTE					m_iThinkTime;				//最少游戏盘数
	BYTE					m_iGiveBackTime;			//留底时间
	BYTE					m_iBeginTime;				//游戏开始时间
	BYTE					m_iCallScoreTime;				//游戏开始时间
	BYTE					m_iAddDoubleTime;				//游戏开始时间

	BYTE					m_iBeenPlayGame;				//已游戏局数
	//自己扑克数据
	BYTE					m_iMeCardCount;				//自己扑克的数目
	BYTE					m_iMeCardList[45];			//自己扑克列表
	BYTE					m_Card[PLAY_COUNT][45];				//各家手中的扑克数组
	BYTE					m_CardCount[PLAY_COUNT];				//各家手中的牌数	

	//主牌信息
	BYTE					m_iNtPeople;				//亮主的游戏者
	BYTE					m_iBackCard[12];				//底牌扑克
	//运行信息
	BYTE					m_iDeskCardCount[PLAY_COUNT];		//每人桌面扑克的数目
	BYTE					m_DeskCard[PLAY_COUNT][45];			//每人桌面的扑克

	//运行信息
	BYTE					m_bCurrentOperationStation;		//現在叫分者
	BYTE					m_iNowOutPeople;				//现在出牌者
	BYTE					m_iBaseCount;					//第一个出牌者牌数目
	BYTE					m_iFirstOutPeople;				//第一个出牌者
	BYTE					m_iBigOutPeople;				//当前最大出牌者
	

	BYTE					m_iBaseCard[45];				//第一个出牌者的扑克

	//上轮显示数据
	BYTE					m_bShowLast;					//是否可以显示上轮扑克
	BYTE					m_bShowIngLast;					//是否正显示上轮扑克
	BYTE					m_iLastCardCount[PLAY_COUNT];		//上轮扑克的数目
	BYTE					m_iLastOutCard[PLAY_COUNT][45];		//上轮的扑克
	BYTE					m_iAIStation[PLAY_COUNT];			//机器人托管
	////定时器计数
	BYTE					m_bTimeOutCount;			//超时
	int						m_btAutoTime;
	BYTE					m_bSortCard;				//排序方式

	DWORD					m_dwCardShape;	  				//牌型
public:
	CUpGradeGameLogic		m_Logic;					//升级逻辑
	bool					m_bAutoCard;				//是否托管
	int						m_ResultCallScore;			//叫分结果
	//函数定义
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();

protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//重载函数
public:
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//重新设置界面
	virtual void ResetGameFrame();
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);
	//同意开始游戏 
	virtual bool OnControlHitBegin();
	//消息函数
public:
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation);
	//叫分
	LRESULT OnCallScore(WPARAM wparam,LPARAM lparam);
	//抢地主
	LRESULT	OnRobNT(WPARAM wparam, LPARAM lparam);
	//加棒
	LRESULT	OnAddDouble(WPARAM wparam, LPARAM lparam);

	LRESULT OnShowCard(WPARAM wparam, LPARAM lparam);
	//用戶出牌
	BOOL UserOutCard();

	//机器人智能出牌
	bool	DwjAiOutCard();
	//获取下一个玩家位置
	BYTE	GetNextDeskStation(BYTE bDeskStation);
	//获得队友的位置
	BYTE	GetFriendStation(BYTE bDeskStation);

	//点击不出按钮
	bool OnControlHitPass(void);

	//杀定时器
	void KillAllTimer();
	DECLARE_MESSAGE_MAP()

///一下均是机器人操作部分

private:
	/// 当同伙出牌后,自己不能保证必赢的情况下的处理
	void AI_DaTongHuo();

    ///抢地主时期的AI操作
	void AI_QiangDiZhu();
	/// 用于判断本把是否抢地主，如果不是把握比较大的情况下，均不要抢地主，
	/// 因为没有同伙的援助对于电脑来说想取得胜利非常难
	bool canQiangDiZhu();

	///加倍与否时期的AI操作
	void AI_JiaBei();
    /// 用于判断是否将分数加倍，如果自己实力很强，只是因为机会不好没当上地主而不加倍分数
	/// 显得过于假，因此需要判断是否够资格加倍分数的情况
	bool canJiaBei();
    
	/// 自己占圈之后的出牌
	void AI_zhanQuanOutCard();

	/// 自己没有占圈之后的出牌
	void AI_buZhanQuanQutCard();

    int AI_getThinkTime();

	/// 计算单张掉张的数量 10以下的 
	int computeDanDiaoZhang(BYTE* handCard,int num);
    /// 计算对子掉张的数量 7以下
	int computeduiDiaoZhang(std::vector<peiOutCardStruct>& myCard);

	/// 计算将三顺带，三带，和炸弹的情况下掉张的数量
	int computeDiaoZhang(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard);
    
	/// 计算定张的数量，2王和炸弹
	int computeDingzhang(std::vector<peiOutCardStruct>& myCard);
private:
	/// 初始化猜的牌
	void initGuessCard();
	/// 去掉已经有主或者打出的牌
	void deleteGuessCard(BYTE *delCard,int num);
    /// 出牌的处理，主要用于记录出牌的信息,和更正猜牌的信息
	void outCardResult(OutCardMsg * pOutCardInfo);
	/// 处理地主的三张底牌，如果地主是自己就直接去掉这三张牌，否则在猜牌中将这三张牌确定下来属于谁的
	void diPai(BackCardExStruct * pBackCard);

	/// 用于初始出牌决策树的状态节点的状态信息
    void initRealData(realData &data);

private:

	
	/// 记录自己和其它几人出过的牌，主要用于分析对方手中的牌，和了解对方对自己的认识
	std::vector<OutCardStruct>     vChuCard[PLAY_COUNT];   
	/// 保留其它二人手中的牌的全部情况，主要用于猜牌
	std::vector<guessCard>              vLeftCard;
    /// 标记自己的位置状态，0为地主，1为地主下家，2为地主上家
	BYTE                                m_index;
	bool                                m_qiangDiZhu;               //用于记录是否具备资格抢地主
	bool                                m_jiaBei;                   //用于记录本局是否具备加倍资格

    OutCardStruct                       m_myOutCard;                //至关重要的一部，这是保存AI计算的结果将要出的牌打出
    int                                 m_diaoZhang;                // 用于记录目前配成的牌当中掉张数量最少的，以确定当中最优的配牌方法
};


/// 配好的牌的形式的判断

/// 判断配好的牌是否具备抢地主的资格
bool peiCardAnalQiangDiZhu(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);
/// 判断配好的牌是否具备加倍分数的资格
bool peiCardAnalJiaBei(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);
/// 判断配好的牌是否满足要求,如果满足了要求按照相应的规则决定出那个牌
bool peiCardFirstOutCard(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg);