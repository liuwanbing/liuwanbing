#include "IGameImpl.h"
#include "..\GameMessage\UpGradeLogic.h"
#include <irrKlang.h>

using namespace irrklang;

interface IGameFrame;
interface IUserInterface;

///定时器
#define    TIME_GAME_BEGIN                10    ///开始时间
#define    TIME_GAME_NOTE                 11    ///下注时间
#define    TIME_GAME_SEND_CARD            12    ///发牌定时器 
#define    TIME_DICE_ANIMAL               13    ///色子动画定时器
#define    TIME_XIPAI_ANIMAL              14    ///洗牌动画
#define    TIME_SEND_CARD_ANIMA           15    ///发牌动画定时器
#define    TIME_KAI_PAI_ANIMAL            16   //开牌定时器
#define    TIME_HAND_ANIMAL               17   //换手定时器
#define    TIME_INTERVAL_TIME             18   //开牌间隔时间定时器
#define    TIMER_FORCE_LEAVE              19   //强制离开桌子定时器 
#define    TIMER_WIN_RECT_WINK            20   //绘画赢的闪烁区域


/// 声音类型设置					
#define SOUND_WARNING		0x10						/** 警告 */
#define SOUND_SEND_CARD		0x20						/** 发牌声音 */
#define SOUND_INVALIDATE	0x30						/** 提示失败 */
#define SOUND_START_DEF		0x40						/** 四个人开始游戏后 */
#define SOUND_GAME_OVER		0x50						/** 游戏结束 */
#define SOUND_WIN           0x60                        /** 胜利 */
#define SOUND_LOST          0x70                        /** 失败 */
#define SOUND_NOTE          0x80                        /** 下注筹码声 */
#define SOUND_START         0x90                        /** 游戏开始 */
#define SOUND_KAIPAI        0xA0                        /** 开牌 */
#define SOUND_NOTE_OVER     0xB0                        /** 下注结束 */
#define SOUND_1000W         0xC0                        /** 1000W声音 */
#define SOUND_100W          0xD0                        /** 100W声音 */
#define SOUND_10W           0xE0                        /** 10W声音 */
#define SOUND_DICE          0x11                          ///色子声音 
#define SOUND_XIPAI         0x12
#define SOUND_NOTE_BG       0x13                       ///下注背景声音

#define GetControlNode(A ,IControl , IControlID ) \
	IControl * A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A
#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));

#define SAFECTRL(CTRL_TYPE,pUI,CTRL_ID, OPERATE)\
{\
	CTRL_TYPE * PRE=NULL; \
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\
	if (PRE!=NULL)\
{\
	PRE->OPERATE; \
}\
}
#define SAFE_CTRL_OPERATE(CTRL_TYPE,CTRL_ID, OPERATE)\
{\
	CTRL_TYPE* PRE=NULL; \
	PRE = dynamic_cast<CTRL_TYPE *>(m_pUI->GetIControlbyID(CTRL_ID));\
	if (PRE!=NULL) PRE->OPERATE; \
}

class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);


public:
    /// 初始化
    virtual int Initial();
    /// 初始化UI
    virtual int InitUI();
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    /// 
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
	///设置游戏状态，主要是断线重连来后处理的消息
    virtual void SetGameStation(void * pBuffer,int nLen);

	virtual bool OnGameQuiting();
private:
	///重新设置UI的状态
	int  ReSetGameUIStation(); 
	///重新设置游戏中客户端的数据
	int  ResetGameData() ;
	//下注完成
	int EnableNoteFinish() ;
	///根据当前的条件显示下注按钮
	int EnableNoteBtnByCondition(bool bShow);
	///设置当前的光标
	int SetCurrentCursor(int uID);
	///下注框按钮的显示状态
	int SetChooseRectStation(int uID,bool bShow ,bool  bAllHideAndShow = false ) ; 
	///显示庄家信息
	int ShowNTInfo(bool bShow);
	///显示玩家信息
	int ShowPlayerInfo(bool bShow = true); 
	///玩家下注的相关显示
    int ShowUserNoteInfo(bool bShow);
	///显示结算上的相关信息
	int ShowResultInfo(bool bshow) ;
	///更新玩家的金币数
	int  ChangeUserMoney(bool bshow); 
	///显示历史信息
	int ShowHistoryInfo(bool  bshow);
	///显示庄家列表中的信息
	int  ShowNtListInfo(bool bShow);
	///显示通吃通赔图片
	int  ShowAllWinOrLost(BYTE iStyle,BOOL bShow);

	///色子动画
	int PlayDiceAnimal(bool bStarPlay,bool bFinish,bool bHide =false);
	///洗牌动画
	int  XiPaiAnimal(bool bStartPlay,bool bFinish,bool bHide) ; 
	///摊牌动画
	int  TanPaiAnimal(bool bStartPlay,bool bFinish,bool bHide) ; 
	///发牌动画
	int  SendCardAnima(BYTE iDir,bool bPlay,bool bFinish = false);
	///开牌动画
	int  OpenCardAnima(BYTE iDir,bool bPlay,bool bFinish) ;
	///显示玩家的牌
	int  ShowUserHandCard(BYTE iDir,BYTE iType,bool bShow,bool bAllCard);
	///显示玩家的牌
	int  ShowUserAllHandCard(BYTE iDir,bool bShow);    
	///显示玩家的手
    int  ShowUserHand(BYTE iStaion,BYTE imgType, bool bshow ,bool bAllHand = false); 
	///超端玩家的状态设置
	int  ShowSuperSettingDlg(bool bShow) ;
	  ///获取牌型的路径
    int  GetCardShapePath(BYTE iStation ,CString &str);
	///显示用户的牌型
	int  ShowUserCardShape(BYTE iStaion , bool bshow ,bool bAllHand) ; 

	int  ShowWinRect(int iWinValue,bool bShow,bool bAllRect);
  	///处理上庄消息
	int   OnhitApplyAndDowmNt(bool bApply);
	///玩家点击下注
	int   OnUserHitNoteRect(int iChooseRect); 
	///上庄和下庄按钮的显示
	int   ShowApplyAndDownNtButton(bool bUp,bool bShow);
	///显示玩家上庄和下庄图片
	int   ShowApplyNtStation(bool bInlist);
	///玩家搓牌消息
	int  ShowAutoAndHandCP(bool bAuto) ; 

	//处理服务器发来的消息——————————————
	///客户端开始消息
	int OnHandleGameBegin(GameBeginStruct *pBeginData);
	///玩家申请上庄消息处理
	int OnHandleUserApplyZhuang(EZP_APPLY_ZHUANG * pApplyZhuang);
	///玩家下注消息
	int OnHandleUserNote(EZP_XIAZHU_INFO *pNoteData);
	///处理服务器的发牌 消息
	int OnHandleSendCard(GameFaPaiData *pCardData);
    ///处理服务器结束消息
	int OnHandleGameFinish(GameEndData *pFinishData);
	int OnHandleGameAheadFinish(void);
	//处理服务器发来的消息——————————————

	//播放音效
	int  SetPlaySound(BYTE bSoundType);
	//删除游戏中所有的定时器
	int  KillAllInGameTimer() ;
    ///播放ogg音效
	int  SetPlaySoundByOgg(irrklang::ISoundEngine *engine,BYTE bSoundType);
	///玩家设置游戏信息
	int  UserSetGameInfo(bool bSoundEnable,bool bWatchEnable,bool bShow);

private:
	 BYTE  GetStationParameter();
	 ///显示或隐藏开始倒计时时钟
	void ShowOrHideTime(bool bFlag,int nShowMaxNum,int nShowMinNum=0);

	int  ShowForceUserLeaveInfo(int iRevTime,bool bShow) ;

	void GetCardIndexPos(int iIndex,POINT &pPos);				//获取牌值在牌图片中的位置

	void ShowCardImage(int iIndex,bool bShow,bool bShowAnimCard);	//显示牌图片
	int GetCardPoint(BYTE byCardList[]);
private:


	GameLogic                     m_Logic;				    /**< 游戏逻辑 */

    IGameFrame                   *m_pGameFrame;
    IUserInterface               *m_pUI;

	ISoundEngine                 *m_SoundEngine;     

	BYTE                         m_bGameStation;
	bool                         m_bGameBegin;                  ///<游戏开始标志

	BYTE                         m_iBeginTime;
	BYTE                         m_iThinkTime;
	BYTE                         m_iXiaZhuTime;
	BYTE                         m_iKaipaiTime; 
	BYTE                         m_iEmptyTime;

	BOOL                         m_bNtApplyDown;

	///游戏相关的限制
	int                          m_iNowNtStation ;                ///当前的庄家
	int                          m_iApplyNTLimit;                  ///上庄限制
	__int64                      m_iMaxNote ;                      ///玩家的最大下注

	int                          m_iChooseNoteRect ;                 //选择的筹码区域
	int                          m_iChooseNoteType ;                ///选择的筹码类型

	CArray <GameInfoList, GameInfoList&> m_ArrGameInfoList;   /**< 每局游戏信息列表 */
	CArray <BYTE, BYTE&>         m_ArrUserList;     /**< 申请上庄的玩家列表 */

	INT_PTR                      m_iNtListInDex ; ///用户庄家列表索引
	INT_PTR                      m_iHistoryInfoInDex ; ///历史信息索引

	bool                         m_iAutoCP;        /**< 是否为自动搓牌 */ 

	BYTE                         m_HandCard[4][2];
	BYTE                         m_DeskCard[4][2];

	PaiData                      m_iUserCard[4];


	__int64                     m_iMyNote[7] ;			///自己在每个区域的下注
	__int64                     m_iQuYuNote[7] ;		/// 区域下注
	__int64						m_iRobotSunNotes[7];	///机器人每个区域的总数

	__int64                     m_iMyNoteTotle ;     //总注
	__int64                     m_iMyHistoryScore;  // 历史成绩


	__int64                     m_iAllUserNote;       //所有玩家下的总注

	__int64                     m_iNtHistoryScore;  //庄家成绩
	int                         m_iNtCount;          //庄家当庄的盘数

	int                         m_iForceLeaveTimeRev;  ///强制玩家离开剩余时间

	bool                        m_bInNoteStage ; //下注是否结束

	BYTE                        m_iSendCardPos;
	BYTE                        m_iSendCardCount;

	BYTE                        m_iKaiPaiPos ;      ///当前开牌位置
	BYTE                        m_iKaiPaiCount ;    ///开牌次数
 
	BYTE                        m_iDiceNum[2] ;     ///色子点数

	BYTE                        m_iCurrentMsg ;     ///当前处理的游戏消息

	int                         m_iWinkcount;   ///闪烁次数
	bool                        m_bShowWink;    /// 是否显示闪烁
	BYTE                        m_iOpenStyle;   ///开牌类型

	GameEndData                 m_gameEndData;  //游戏结束的数据

public:
	bool                        m_bSoundPlay ;  //是否可以播放声音
	bool                        m_bAllowWatch ; // 是否允许旁观
	bool                        m_bWatchMode ; //是否为旁观模式
	bool                        m_bSuperClient; //玩家是否为超级客户端

	CString                     m_strCoinName ; ///货币名称

	int                         m_iAnimalCount ; ///洗牌和摊牌的次数
	BOOL						m_bIsSpace;		 ///金币显示是否要分隔符

	bool						m_bShowRobotChip;		/**< 是否屏蔽示机器人下注 TRUE:屏蔽*/
	bool						m_bLButtonDownOrUp;		/**< 鼠标左键是按下还是弹起 TRUE:Down 辅助手动搓牌*/
	int							m_iCurrentCardControl;	/**< 当前牌控件*/
	POINT						m_pCureentCardPoint[4];	/**< 当前牌控件的位置*/	
};