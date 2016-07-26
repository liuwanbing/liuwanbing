#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>

using namespace irrklang;

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A

const   int  GAME_WAIT_STATION    = 1 ;
const   int  GAME_BEGIN_STATION   = 2 ; 
const   int  GAME_FINISH_STATION  = 3 ; 

const  int  TIME_GAME_BEGIN     = 200 ;    ///游戏开始定时器
const  int  TIME_SEND_CARD      = 201 ;   ///玩家发牌定时器
const  int  TIME_CALL_SCORE     = 202 ;    ///玩家叫分
const  int  TIME_USER_ROB_NT    = 203 ;   ///玩家抢地主
const  int  TIME_GAME_DOUBLE    = 204 ;   ///玩家加倍
const  int  TIME_SHOW_CARD      = 205 ;   ///玩家明牌
const  int  TIME_USER_OUT_CARD  = 208 ; ///玩家出牌定时器

const  int  TIME_LAST_TURN      = 210 ;  ///上一轮


//声音类型设置
#define SOUND_OUT_CARD				0x10				//出牌声音
#define SOUND_WARNING				0x20				//警告
#define SOUND_SEND_CARD				0x30				//发牌声音
#define SOUND_OUT_CARD_DEF			0x40				//默认出牌声音
#define SOUND_OUT_CARD_BOMB			0x50				//出炸彈
#define SOUND_OUT_CARD_PASS			0x60				//不出
#define SOUND_INVALIDATE			0x70				//提示失败
#define SOUND_START_DEF				0x80				//四个人开始游戏后
#define SOUND_GAME_OVER				0x90				//游戏结束
#define SOUND_ADD_DOUBLE			0xA0				//加棒
#define SOUND_NOT_ADD_DOUBLE        0xA1                 //不加倍
#define SOUND_CALL_NT               0xA2                 //叫地主
#define SOUND_NOT_CALL_NT           0xA3                 //不叫
#define SOUND_ROB_NT                0xB0                 //抢地主
#define SOUND_NO_ROB_NT				0xC0				//不抢地主
#define SOUND_SHOW_CARD				0xD0				//明牌

#define SOUND_WIN					0x01				//胜利
#define SOUND_LOSE					0x02				//输
#define SOUND_CUT_END               0x03                ///逃跑
#define SOUND_ERROR					0x04				//出牌出错了
#define SOUND_READY					0x05				//游戏开始
#define SOUND_START					0x06				//准备出第一手牌
#define SOUND_START_OUT_CARD		0x07				//出第一手牌
#define SOUND_CALL_SCORE			0x08				//叫分
#define SOUND_OUT_CARD_FINISH		0x09				//某玩家出牌結束
#define SOUND_GAME_PASS				0x0A				//不出
#define SOUND_FRIEND_APPEAR			0x0B				//盟友出現
#define SOUND_USER_RUNAWARY			0x0C				//有人逃跑
#define SOUND_USER_LEFT				0x0D				//有人离开
#define SOUND_GIVE_BACK				0x0E				//埋底
#define SOUND_SHUNZHI				0x13				//顺子
#define SOUND_FEIJI					0x14				//飞机
#define SOUND_BAOJING               0x15
#define SOUND_LAST_CARD             0x16                // 最后几张牌
#define SOUND_BIG_SHAPE             0x17                //大的牌型

#define SOUND_GAME_BG               0xff                 //背景音效



interface IGameFrame;
interface IUserInterface;
class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);


private:
	///框架游戏指针
	IGameFrame         *m_pGameFrame;
	///界面游戏指针
	IUserInterface     *m_pUI;

	ISoundEngine       *m_SoundEngine;               ///游戏音效

	ISoundEngine       *m_SoundEnginBg ;              ///背景音效


public: 

	CUpGradeGameLogic   m_cLogic ;             ///客户端逻辑

	BYTE                m_byGameStation ;         ///当前的游戏状态
	int                 m_iMyDeskStation ;       ///玩家自己的游戏位子
  
	///时间列表
	BYTE				m_byThinkTime;		   ///最少游戏盘数
	BYTE                m_byRobNTTime ;        ///玩家抢地主
	BYTE				m_byBeginTime;		   ///游戏开始时间
	BYTE				m_byCallScoreTime;	   ///游戏开始时间
	BYTE				m_byAddDoubleTime;	   ///游戏开始时间

	BYTE                m_byNtPeople ;         ///当前的地主 
	BYTE                m_byOperatePerson ;            ///当前操作的玩家(叫分或者亮牌时用到）
	BYTE                m_byOutCardPeople ;    /// 当前出牌的人

	BYTE                m_byStation ;  //发牌过程中的状态

	bool                m_bUserReady[PLAY_COUNT] ;  ///游戏玩家准备状态

	///玩家的牌数据
	BYTE                m_byHandCardList[PLAY_COUNT][45] ;    ///玩家手中的牌数据
	BYTE                m_byHandCardCount[PLAY_COUNT] ;      ///玩家手中牌的张数

	BYTE                m_byBackCardList[12] ;               ///游戏中的底牌数据
	BYTE                m_byBackCardCount ;                  /// 游戏中的底牌数量

	BYTE                m_byDeskCardList[PLAY_COUNT][45] ;   ///游戏中桌面的牌数据
	BYTE                m_byDeskCardCount[PLAY_COUNT] ;      ///游戏中桌面的张数

	BYTE                m_byBaseCardList[45] ;               ///桌面上比较的牌
	BYTE                m_byBaseCardCount ;                  ///桌面上比较牌的张数

	BYTE                m_byLastTurnCardList[PLAY_COUNT][45] ; 
	BYTE                m_byLastTurnCardCount[PLAY_COUNT] ; 

	bool                m_bUserPass[PLAY_COUNT] ;               ///玩家当前一轮pass
	bool                m_bLastTurnPass[PLAY_COUNT] ;           //玩家上一轮pass

	bool                m_bHaveLastTurnData ;    ///是否有上一轮的数据

	BYTE                m_iSendAllCardCount ;             ///当前发送的牌的张数  
	BYTE                m_SendCardList[PLAY_COUNT][45] ;  ///玩家发牌的结果    
	BYTE                m_iSendCardCount[PLAY_COUNT] ;    ///玩家发牌的张数

	BYTE                m_bySortCard ;              ///排序方式

	bool			    m_bAutoCard;				//是否托管处理
	bool                m_bAutoFlag[PLAY_COUNT] ;   ///玩家托管标识

	int                 m_bAutoOutTime ; 
	int                 m_iUserCallScore[PLAY_COUNT] ;
	int                 m_iUserRobNtValue[PLAY_COUNT] ;
	int                 m_iUserDoubleValue[PLAY_COUNT] ;
	
	GameTaskStruct      m_gameTask ; 
	GameEndStruct       m_gameResult ; ///游戏结算

	int                 m_iGameBaseMutiple ;   ///游戏基本底注

	int                 m_iAddDoubleLimit ;  

	int                 m_iGameMaxLimit ; 

	GameMutipleStruct   m_MutipleGather ;  ///游戏中的倍数集合

	bool                m_bSoundPlay ;  //是否可以播放声音
	bool                m_bSoundBgPlay ;  //背景音效
	bool                m_bAllowWatch ; // 是否允许旁观
	bool                m_bStartMode ;  //开始模式还是退出模式
	bool                m_bWatchMode ;  //是否是旁观模式
	CString             m_strCoinName ;   ///游戏中的金币


	bool				m_bIsSuper;			//是否超端用户
	bool				m_bShowOherCard;	//超端控制 是否显示其他玩家的牌


 public:
    /// 初始化
    virtual int Initial();
    /// 初始化UI
    virtual int InitUI();
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    /// 重新设置状态
    virtual int ResetGameFrame(void);
    /// 玩家进入
    virtual int GameUserCome(void);
    /// 玩家离开
    virtual int GameUserLeft(int nIndex);
    /// 玩家断线
    virtual int GameUserOffLine(void);
    // 是否正在游戏，由游戏实现
    // 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
    virtual bool IsPlayingGame();          
    /// 定时器消息
    virtual void OnTimer(int nTimerID);
    /// 置状态
    virtual void SetStationParameter(BYTE bGameStation);
    ///设置游戏状态
	virtual BYTE GetStationParameter();
	///游戏结束
	virtual bool OnGameQuiting();

public:
	///初始化游戏中的变量
	int   InitialGameData(void) ; 
	///初始化游戏的界面
	int   InitialGameUI(void) ; 
	///删除所有定时器
	void  KillAllGameTime(void) ;
	///重新设置游戏中界面
	void  ResetGameByStation(BYTE  iGameStation) ; 

	///玩家进入游戏
	void  SetGameStation(void * pBuffer,int nLen);
	///显示或隐藏开始倒计时时钟
	void  ShowOrHideUserTime(BYTE bDeskStation,int nShowMaxNum,int nShowMinNum,bool bFlag) ;
	///隐藏所有定时器
	void  HideAllTime(void) ;

    ///显示玩家准备状态
	void  ShowGameReady(bool bShow) ; 
	///显示界面元素函数
	void  ShowGameBeginBtn(bool bShow) ; 
	///显示底牌数据
	void  ShowBackCard(bool bShow) ; 
	///客户端先行数据
	void  ShowFirstMyCard(BYTE  iDeleteCard[] ,BYTE iDeleteCount ,BOOL bShow) ; 
    ///显示玩家的手牌
	void  ShowUserHandCard(BYTE bDeskStation , bool bShow ,bool bShowAll = false) ;
	///显示玩家的发牌
	void  ShowUserSendCard(BYTE bDeskStation ,bool bShow) ;
	///显示发的牌的张数
	void  ShowUserCardCount(BYTE bDeskStation , bool  bShow , bool bSend) ; 
	///显示玩家桌面上的牌
	void  ShowUserDeskCard(BYTE bDeskStation , bool bShow) ;
	///显示上一轮扑克
	void  ShowLastTurnCard(bool bShow) ; 
	///显示不出图片
	void  ShowUserPass(BYTE bDeskStation , bool bShow)  ;
	///放下所有的牌控件
	void  DownUserAllCard(bool bShow); 
	///显示托管按钮或取消按钮
	void  ShowAutoBtnStation(bool bShow , bool bEnable); 
	///显示庄家图标
	void  ShowNtFlat(bool bShow) ; 
	///显示游戏中的倍数
	void  ShowGameMutiple(bool bShow) ;
	///显示倍数信息
    void  ShowGameMutipleContent(bool  bShow) ; 
	///显示托管图标
	void  ShowUserAutoFlag(bool bShow) ; 
	///显示玩家叫分状态
	void  ShowUserCallScoreBtn(int iCallValue , bool bShow) ;
	///显示玩家叫分结果标识
	void  ShowUserCallScoreFlag(bool bShow) ; 
	///显示玩家抢地主按钮
	void  ShowUserRobNtBtn(bool bCall , bool bShow) ; 
	///显示玩家抢地主标识
	void  ShowUserRobNtFlag(bool bShow) ; 
	///显示玩家加倍
	void  ShowUserDoubleBtn(bool bShow ,bool bEnable  = true) ; 
	///显示玩家加倍标识
	void  ShowUserDoubleFlag(bool bShow) ;
	///显示玩家亮牌按钮
	void  ShowUserShowCardBtn(bool bShow) ; 
	///显示玩家亮牌标识
	void  ShowUserShowCardFlag(bool bShow) ;  
	///显示玩家出牌界面的按钮
	void  ShowOutCardStationBtn(bool bEnaleOutCard ,bool bShowPass ,bool bShow) ; 
	///显示游戏结算框
	void  ShowGameResult(bool bShow) ; 
	//显示自动配桌界面(比赛专用)
	void	ShowMatchAutoDesk(bool bShow);
	///显示游戏中的任务
	void  ShowGameTask(bool  bShow) ; 
	///显示任务完成图片
	void  ShowGameTaskFinish(bool bShow) ; 
	///用户叫分
	int   OnUserCallScore(int iCallValue); 
	///玩家点击抢地主等操作
	int   OnRobNtButton(int iRobvalue) ; 
	///玩家点击加倍和不加倍
	int   OnAddDoubleButton(int iAddValue) ; 
	///玩家点击亮牌和不亮
	int   OnShowCardButton(int iShowValue) ; 
	///用户出牌相关操作
	int   OnUserOutCardOperate(int  iControlID);	
	///用户点击扑克
	int   OnCheckOutCard(void) ; 
	///玩家点击托管
	int   OnAutoButton(void) ; 
	///玩家点击排序
	int   OnSortButton(void) ; 
	////游戏消息处理

	///播放任务动画
	void  OnPlayTaskAni(bool bPlay ,bool bShow) ; 
    ///播放报警动画
	void  OnPlayWarningAni(BYTE bDeskStation , bool bPlay ,BYTE bType , bool bShow) ;
	///播放倍数增加动画
	void  OnPlayGameAni(int iAniID , bool bPlay , bool bShow ,bool bIsloop = false) ; 
    ///显示地主和农民输赢动画
	void  OnPlayWinLostAni(bool bWin , bool bPlay , bool bShow) ; 
	///处理游戏开始消息
	void	OnHandleGameBegin(GameBeginStruct *pBeginData); 
	///处理游戏发牌消息
	void	OnHandleSendCardMsg(SendAllStruct *pSendCardData) ; 
	///处理游戏发牌结束消息
	void	OnHandleSendCardFinish(void) ;
	///处理叫分消息
	void  OnHandleCallScoreMsg(CallScoreStruct *pCallScoreData); 
	///处理叫分结果
	void  OnHandleCallScoreResult(CallScoreStruct *pCallScoreData); 
	///处理叫分结束
	void  OnHandleCallScoreFinish(CallScoreStruct *pCallScoreData) ; 
	///处理抢地主消息
	void	OnHandleRobNtMsg(RobNTStruct *pRobNtData) ; 
	///处理抢地主结果消息
	void	OnHandleRobNtResult(RobNTStruct *pRobNtData);
	///处理抢地主结束消息
	void  OnHandleRobNtFinish(RobNTStruct *pRobNtData) ; 
	///处理发底牌消息
	void  OnHandleSendBackCard(BackCardExStruct *pBackCard) ; 
	///处理加倍消息
	void  OnHandleAddDoubleMsg(AddDoubleStruct *pAddDoubleData) ;
	///处理加倍结果消息
	void  OnHandleAddDoubleResult(AddDoubleStruct *pAddDoubleData) ; 
	///处理加倍结束消息
	void  OnHandleAddDoubleFinish(void) ;
	///处理明牌消息
	void  OnHandleShowCardMsg(ShowCardStruct *pShowCardData) ; 
	///处理明牌结果消息
	void  OnHandleShowCardResult(ShowCardStruct *pShowCardData) ;
	///处理明牌结束消息
	void  OnHandleShowCardFinish(void) ; 
	///处理玩家开始出牌消息
	void  OnHandleBeginPlayGame(BeginPlayStruct *pBeginPlayData) ;   
	///通知玩家出牌消息
	void  OnHandleNotifyUserOutCard(OutCardMsg *pOutCardData) ;
	///玩家出牌结果消息
	void  OnHandleUserOutCardResult(OutCardMsg *pOutCardResult);
	///一轮结束
	void  OnHandleOneTurnOver(void) ; 
	///新一轮出牌
	void  OnHandleNewTurnBegin(NewTurnStruct *pNewTurnData);  
	///游戏结束
	void  OnHandleGameFinish(GameEndStruct *pGameEndData) ; 
	///玩家强行退出
	void  OnHandGameCutEnd(GameCutStruct *pCutEnd) ; 
 
	///用户超时或者托管操作
	bool  OnAutoOutCard(void) ; 
    ///获取随机任务名称
	void  GetTaskName(CString &strName) ; 

	///玩家设置游戏信息
	int   UserSetGameInfo(bool bSoundEnable,bool bSoundBgEnable ,bool bWatchEnable,bool bShow) ;
	///游戏中的音效
	void  SetPlaySound(BYTE iDeskStation,int SoundID, bool bHaveSex , bool bISLoop) ;
	///出牌声音播放
	void  SetOutCardPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop) ;
	///游戏中的背景音效
	void  SetGameBgSound(int SoundID, bool bISLoop) ;



	//设置动画显示某一帧
	void	SetAnimateShowFrame(const int AnimateID,bool bFlag,int iFrame);


};