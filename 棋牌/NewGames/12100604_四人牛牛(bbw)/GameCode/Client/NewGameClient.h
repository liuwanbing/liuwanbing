#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
//#include "CommonDefine.h"
#include<deque>
using namespace irrklang;

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A

const   int  GAME_WAIT_STATION    = 1 ;
const   int  GAME_BEGIN_STATION   = 2 ; 
const   int  GAME_FINISH_STATION  = 3 ; 
//扑克数
#define CARD_NUMS					5

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_WAIT_NEXT				101					//开始定时器
#define ID_OUT_CARD					102					//出牌等待时间
#define ID_LOOK_BACK				103					//看暗牌
#define ID_CHANGE_MUSIC			    104					//换背景音乐
#define ID_NOTE                     105                 //等待闲家下注
#define ID_OPEN_CARD                106                 //等待闲家出牌
#define ID_CALL_SCORE_TIME			107					//叫分定时器

//游戏音效
#define SOUND_BEGIN_GAME			  0x01			//游戏开始
#define SOUND_TIME_0				  0x02			//时间提示音
#define SOUND_TIME_1			      0x03
#define SOUND_TIME_2			      0x04
#define SOUND_TIME_3			      0x05
#define SOUND_TIME_4			      0x06
#define SOUND_TIME_5			      0x07
#define SOUND_WIN				      0x08			//胜利
#define SOUND_LOSE				      0x09			//失败
#define SOUND_CHONGPAI			      0x0A			//重排
#define SOUND_XIAZHU			      0x0B			//下注
#define SOUND_TIP			          0x0C			//提示
#define SOUND_TANPAI			      0x0D			//摊牌
#define SOUND_CLICKCARD			      0x0E			//点击牌				
#define SOUND_HIT_BEGIN				  0x0F			//点击开始按钮
#define SOUND_HIT_LEAVE				  0x11			//离开
#define SOUND_JIAOZHUANG			  0x12			//叫庄	
#define SOUND_GIVEUP			      0x13			//叫不叫庄

/*无牛-牛九*/
#define SOUND_NIUNIU_0				  0x3F			
#define SOUND_NIUNIU_1				  0x4F	
#define SOUND_NIUNIU_2			      0x5F
#define SOUND_NIUNIU_3			      0x6F
#define SOUND_NIUNIU_4			      0x7F
#define SOUND_NIUNIU_5			      0x8F
#define SOUND_NIUNIU_6			      0x9F
#define SOUND_NIUNIU_7				  0xAF			
#define SOUND_NIUNIU_8				  0xBF			
#define SOUND_NIUNIU_9			      0xCF			
#define SOUND_NIUNIU_10			      0xDF				//牛牛
			


#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\

class IGameFrame;
interface IUserInterface;
class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);//（构造函数第一次调用）
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);


private:
	
	IGameFrame							*m_pGameFrame;									///框架游戏指针
							//游戏信息指针
	
	IUserInterface						*m_pUI;											///界面游戏指针

	ISoundEngine						*m_SoundEngineGame;								//游戏音效

	ISoundEngine						*m_SoundEnginBg ;								//背景音效

	bool                m_bSoundPlay ;		//是否可以播放声音
	bool                m_bSoundBgPlay ;	//背景音效
		
	CUpGradeGameLogic					m_Logic;										//客户端逻辑
public:
	//固定信息
	//=====================时间表信息===================
	BYTE					m_iThinkTime;				//最少游戏盘数
	BYTE					m_iBeginTime;				//游戏开始时间
	BYTE					m_iSendCardTime;			//发一张牌时间(发牌速度)
	BYTE					m_iCallScoreTime;			//游戏开始时间
	BYTE					m_iXiaZhuTime;				//下注时间
	int						m_iRoomBasePoint;			//房间倍数
	int						m_iRunPublish;				//逃跑扣分
	int						m_iVersion;					//游戏版本
	int						m_iVersion2;				//游戏升级版本
	int						m_iMyDeskStation ;			//玩家自己的游戏位子
	int						m_iHaveCattleTip;			//有牛提示时间
	BYTE					m_byGameStation ;			//当前的游戏状态
	BYTE					m_iPalyCardCount;			//游戏扑克数目
	BYTE					m_iCurrentCardCount;		//当前要发的牌的数目
	BYTE					m_iCardCount;				//允许游戏的扑克数目
	__int64					m_iLimitNote[PLAY_COUNT];				//最大限额注
	__int64					m_i64UserNoteLimite[PLAY_COUNT][4];	//玩家四个下注数字
	__int64					m_iThisTurnLimit;			//本轮限注
	//运行信息
	int						m_iUserStation[PLAY_COUNT];						//记录用户状态(标识玩家是否中途进入的 还是一直就是玩家)
	BYTE					m_DeskReadyCard[PLAY_COUNT][SH_USER_CARD];		//玩家扑克牌数据(逻辑位置)
	BYTE					m_iDeskCardCount[PLAY_COUNT];					//玩家扑克牌张数(逻辑位置)

	BYTE                    m_iUpBullCard[PLAY_COUNT][2];//升起的牛牌
	BYTE                    m_bSendCard[PLAY_COUNT];		//发牌数组
	BYTE					m_bUpCard[2];				//上面的牌
	BYTE                    m_bUnderCard[3];			//下面的牌
	BYTE					m_byNtStation;										//庄家位置

	//运行信息
	BYTE				    	m_bCurrentOperationStation;		//現在叫分者
	BYTE						m_bTimeOutCount;			//超时
	BYTE						m_bShowAnimalStation;		//动画显示位置
	int					    	m_iNowOutPeople;			//现在出牌者
	int						    m_iFirstOutPeople;			//第一个出牌者
	int					    	m_iNowBigNote;				//当前最大押注数
//	__int64						m_iThisGameNote[PLAY_COUNT];			//本局游戏下注数(明注)
	__int64						m_iTotalGameNote[PLAY_COUNT];			//本局游戏下注数(实际)
	int						    m_iUpGradePeople;					//庄家位置
	int                         m_iNote;                    //当前有效下注数即前位玩家有效下注数;
	BOOL						m_bBgSound;					//背景音乐
	int                         m_iAfterWaitAction;
	int							m_bCurrentClockLabel;			//当前时钟标签
	int							m_iCurrentClockControlID;		//当前控制的时钟ID

	bool                        m_bAutoBull;                ///是否自动摆牛
	BYTE                        m_iSendAnimaCardPos;     ///发牌动画位置
	int                         m_iSendCardTotle;         ///发牌的张数
	BOOL                        m_bIsSuper;               ///是否超端
	BOOL                        m_bPlayer;                ///是否真实玩家，2011-8-17 因平台修改过程中玩家可以进入游戏所以中途坐上桌子的不算真实玩家
	BOOL						m_bIsHitTip;			//是否点了提示按钮,且有牛牌

	//发牌相关
	deque<BYTE>                 m_sendQueue;			//发牌顺序数组
	BYTE						m_bySendCardPos;		//当前发牌位置
	BYTE						m_bySendCardCount;		//发牌次数
	BYTE						m_bySendCount[PLAY_COUNT];		//已经发了多少张牌
	//游戏设置
	SuperUserSetData			m_SuperSetData;				//超端设置
	bool					m_iGameSet[3];											//游戏设置单选框
private:
	
private:
	//设置控件是否可见
	void SetControlVisible(const int iControlID,bool bFlag);

	//设置按钮是否可用
	void	SetBtEnable(const int iButtonID, bool bFlag);

	//设置按钮是否可见
	void	SetBtVisible(const int iButtonID, bool bFlag);

	//设置时间框显示倒计时
	void	SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime = 0);

	//设置图片控件的位置
	void	SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag);

	//设置图片控件是否可见
	void	SetImageVisible(const int iImageID, bool bFlag);

	//设置图片控件加载显示指定的文件
	void	SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag);

	//设置容器控件可见
	void	SetContainVisible(const int iContainID,bool bFlag);

	//设置容器控件不响应任何鼠标时间
	void  SetContainEnable(const int iContainID,bool bFlag);

	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);
	//设置文本控件显示文字信息
	void	SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag);
	//设置数字空间显示内容
	void	SetInumInfo(const int , __int64 ,bool );

	//设置数字空间显示内容(不带加号和减号及逗号)
	void SetInumInfo_1(const int iNumID, __int64 i64Num, bool bFlag);

	//设置筹码控件显示值
	void	SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);

	//设置控件位置
	void   SetControlPos(const int iControlID,bool bFlag,int x,int y);

	//设置单选框控件是否被选中
	void	SetRButtonSelected(const int iRButtonID,bool bSelected,bool bShow = true);

	//获取单选框控件是否被选中
	bool	GetRButtonSelected(const int iRButtonID);

	//设置可操作牌控件数据
	void	SetOperateCardInfo(const int iOperateCardID, BYTE byCardList[],BYTE byCardCount, bool bShow);
	//设置不可操作牌控件数据
	void	SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount, bool bShow);

public:
	//第一个进入服务器的玩家执行此函数
	void HandleStartStation(void *buffer);
	//抢庄状态
	void	HandleRobNtStation(void *buffer);
	//下注状态
	void	HandleNoteStation(void *buffer);
	//发牌状态
	void	HandleSendCardStation(void *buffer);
	//游戏进行中
	void	HandleOpenCardStation(void *buffer);
	
	///重置数据
	void	ResetGameData();

	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iButtonID, CString sImagSrc,bool bFlag,int x,int y,int w,int h);

	

 public:
    /// 初始化（第二次调用）
    virtual int Initial();
    /// 初始化UI（第三次调用）
    virtual int InitUI();
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    /// 重新设置状态
    virtual int ResetGameFrame(void);
    /// 玩家进入（第四次调用）
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
	///游戏结束（游戏退出时调用）
	virtual bool OnGameQuiting();
	
	//重置UI界面
	void	ResetGameDialog();
	///初始化游戏中的变量
	void	InitialGameData();  
	//读取游戏设置
	void	LoadGameSet();
	//保存游戏设置
	void	SaveGameSet();
	
	///删除所有定时器
	void	KillAllGameTime() ;

	//播放背景音效
	void	PlayBgSound(bool bISLoop);

	//播放游戏音效
	void	PlayGameSound(int SoundID,bool bISLoop = false, BYTE byDeskStation = 0);
	
	///玩家进入游戏
	void	SetGameStation(void * pBuffer,int nLen);



	//按钮
	//开始按钮
	void OnControlHitBeginBt();

	
	//下注按钮
	void OnHandleStartBet(int iHitButton);
	//叫庄按钮
	void OnHandleHitNtBt();
	//放弃叫庄按钮
	void OnHandleHitGiveUpNtBt();
	//重排/摊牌按钮
	void OnHandleIsShowCardBt(int iHitButton);
	//点击可操作牌
	void OnOperateCardBt();
	//点击继续、离开按钮
	void OnClickContinueBt(int iHitButton);

	//发牌完成消息
	void OnSendCardFinish();

	//客服端处理服务端消息
	//处理叫庄
	void OnHandleCallScoreMessage(void *buffer);
	//处理叫庄结果
	void OnHandleCallScoreResultMessage(void *buffer);
	//处理叫庄结束消息
	void OnHandleCallScoreFinishMessage(void *buffer);
	//处理同意游戏消息
	void OnAgreeGame(void *buffer);
	//处理游戏开始下注消息
	void OnHandleBetBegin(void *buffer);
	//处理下注结果
	void OnHandleBetResult(void *buffer);
	//处理发牌结果
	void OnHandleSendCardBegin(void *buffer);
	//处理发牌结束消息
	void OnHandleSendFinish();
	//处理游戏开始
	void OnHandleGameBegin(void *buffer);
	//游戏结束
	void OnGameFinish(void *buffer);
	//处理摊牌消息
	void OnHandleOpenCard(void *buffer);
	//处理玩家退出消息
	void OnHandlePlayerCut(void *buffer);
	//处理玩家正常退出消息
	void OnHandlePlayerSafeEnd(void *buffer);
	//处理服务器发过来的超端用户信息
	void	HandleSuperMessage(void *buffer);

	//启动相应叫庄计时器
	void OnStartClock(int viewStation,BYTE bCallScoreTime);
	
	//显示游戏设置
	void	ShowGameSet(bool bShow);
	//清空抢庄/不抢庄图片
	void CLearScratchNt();
	//清空所有时钟
	void ClearAllClock();
	//禁止下注
	void ClearAllBet();

	//显示下注
	void DisplayBet(__int64 i64MaxBetCount);
	//下注选项设置
	void BetOptionSet(__int64 i64Num,int iOption,bool IsUse);
	//发自己的牌
	void ShowCurrentCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//发别人的牌
	void ShowCurrentNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//展示摊牌玩家的牌
	void ShowOpenCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//根据提示升起牌
	void ShowUpCard(const int iControlID1,const int iControlID2,BYTE bUnderCardList[],BYTE bUnderCardCount,BYTE bUpCardList[],BYTE bUpCardCount);
	//设置数字空间显示内容(第三个参数:0:显示加号，减号，和逗号；1：只显示逗号；2：都不进行设置)
	void	SetInumPro(const int , __int64 , BYTE ,bool );
	//获取升起的牌
	void GetUpCard(const int iControlID,BYTE bCardList[]);
	//获取升起的牌的数量
	int GetUpCardNum(const int iControlID);
	//显示结算框
	void	ShowResultDlg(GameEndStruct *pGameFinish,bool bShow);
	//显示超端结果界面
	void	ShowSuperSet(bool bFlag);
	//发送超端设置数据
	void	SuperUserSet();

	//时间警示音
	void TimeWarning(int iTime); 
	//显示时钟倒计时
	void	ShowTimer(const int , bool, BYTE byMax, BYTE byMin=0 );

	//各种动画
	//启动有牛动画
	void ShowHaveCattleAction(const int iImageID,int iCardShape,bool bShow,BYTE byDeskStation);
	//播放牌型声音
	void	PlayShapeSound(int iCardShape,bool bShow,BYTE byDeskStation);
	//播放发牌动画
	void CNewGameClient::PlaySendCard(BYTE Station);
	//播放发牌动画
	void ShowSendCardAction(const int ACTION_ID,const int END_ID);
	//给用户发牌
	void SendCard(BYTE bCardValue,BYTE bSendPeople);
	//计算发牌顺序
	void SendQueue(int iStart);
	//计算当前牌的拥有者
	BYTE GetSendStation(BYTE (*BCardList)[5],BYTE bCardValue,int iCardCount);
	//当前牌数组是否有效
	BOOL IsUser(BYTE bCardValue);	

};