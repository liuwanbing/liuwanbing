#include "IGameImpl.h"
#include "UserDataCliObject.h"
#include "UI_ForLogic/IUserInterface.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include <stack>
#include <queue>

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A


using namespace irrklang;

const BYTE MY_VIEWSTATION = 4;				// 自己的视图位置总是6

#define IDT_MODIFY_MONEY			 1		// 等待玩家修改带入金额
#define IDT_COLLECT_JETONS			 3		// 收集筹码动画的时间
#define IDT_WAIT_DISPATCH_JETONS	 4		// 等待颁发筹码
#define IDT_DISPATCH_JETONS			 5		// 颁发筹码动画的时间
#define IDT_HIDE_BALANCE_TIMER		 6      // 隐藏结算框时间
#define IDT_IN_MONEY_BOX			 7      // 弹出带入金额框
#define IDT_MONEY_LACK				 8		// 玩家本身金额不足定时器
#define IDT_WIN_DONG_HUA_TIMER		 9		// 胜利动画定时器
#define IDT_OK_LOAD					 10		// 确定带入定时器
#define IDT_AOUT_IN					 11		// 自动带入金额
#define IDT_LOOK_CARD				 12		// 看牌

/// 声音类型设置
#define BT_SEND_BACK_CARD			100		// 发底牌声音
#define BT_FLOP_ROUND				101		// 翻牌公共声音
#define BT_TURN_ROUND				102		// 转牌公共声音
#define BT_RIVER_ROUND				103		// 河牌公共声音
#define BT_FLOP_PUBLIC_CARD			104		// 翻公共牌声音
#define BT_FLOP_BACK_CARD			105		// 翻底牌声音
#define BT_WIN						106		// 胜利声调
#define BT_LOSE						107		// 失败声调
#define BT_FOLD_MUSIC				108		// 弃牌声调
#define BT_CHIP_MUSIC				109		// 筹码声调
#define BT_TRANSFER_TOKEN			110		// 移交令牌声调
#define BT_OK						111		// 确定按钮声调
#define BT_MONEY_BUTTON				112		// 金额选择声调
#define BT_ALARM_CLOCK				113		// 闹钟声调
#define BT_CHIP_MONEY_MUSIC			114		// 筹码音效

#define ST_BET						115		// 下注声音
#define ST_CALL						116		// 跟注声音
#define ST_FOLD						117		// 弃牌声音
#define ST_CHECK					118		// 过牌声音
#define ST_RAISE					119		// 加注声音
#define ST_ALL_IN					120		// 全下声音

interface IGameFrame;
interface IUserInterface;
class CNewGameClient : public IGameImpl
{
#define SAFE_CTRL_OPERATE(CTRL_TYPE,CTRL_ID, OPERATE)\
	{\
	CTRL_TYPE* PRE=NULL; \
	PRE = dynamic_cast<CTRL_TYPE *>(m_pUI->GetIControlbyID(CTRL_ID));\
	if (PRE!=NULL) PRE->OPERATE; \
}
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
	/// 重置
	virtual int ResetGameFrame(void);
	/// 玩家进入
	virtual int GameUserCome();
	/// 玩家离开
	virtual int GameUserLeft(int nIndex);
	/// 玩家断线
	virtual int GameUserOffLine(void);
	/// 是否正在游戏，由游戏实现
	/// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
	virtual bool IsPlayingGame();          
	/// 定时器消息
	virtual void OnTimer(int nTimerID);
	/// 设置状态
	virtual void SetStationParameter(BYTE bGameStation);
	/// 玩家退出
	virtual bool OnGameQuiting();
	/// 设置游戏状态,主要是断线重连来后处理的消息
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);

private:
    IGameFrame                  *m_pGameFrame;
    IUserInterface              *m_pUI;
	BYTE									m_bGameStation;
protected:
	// 获取游戏状态
	BYTE  GetStationParameter();

	int GetGameStatus()
	{
		if(this)
			return GetStationParameter();
		else
			return 0;
	};

public:
	// 全初始化
	void OnInitData();

	//游戏开始
	void OnHandleGameBegin(BYTE * buffer,int nLen);

	//收到系统配置桌子的信息包，并处理
	void OnHandleGetSysDeskCfg(BYTE * buffer,int nLen);

	//设置底注
	void OnHandleCfgDesk(BYTE * buffer,int nLen);

	//设置设置底注超时            
	void OnHandleWaitCfgDeskTimeout();

	//收到注额信息包，并处理
	void OnHandleGetDeskCfg(BYTE * buffer,int nLen);

	//收到代入金额信息包，并处理
	void OnHandleInMoney(BYTE * buffer,int nLen);

	//收到服务器发一张牌包，并处理
	void OnHandleSendACards(BYTE * buffer,int nLen);

	//收到服务器边池更新包，并处理
	void OnHandleBetPoolUp(BYTE * buffer,int nLen);

	//收到服务器发一张牌包，并处理
	void OnHandleSend3Cards(BYTE * buffer,int nLen);

	//收到服务器发一张牌包，并处理
	void OnHandleSend1Cards(BYTE * buffer,int nLen);

	//收到服务器报告所有玩家发牌完成的消息，并处理
	void OnHandleSendCardFinish();

	//收到玩家下注的消息
	void OnHandleGetBetInfo(BYTE * buffer,int nLen);

	//收到令牌消息，激活用户
	void OnHandleGetToken(BYTE * buffer,int nLen);

	//比牌处理接口
	void OnHandleCompareCard(BYTE * buffer,int nLen);

	//收到结算消息包，并处理
	void OnHandleRoundResult(BYTE * buffer,int nLen);

	//收到新的一回合开始消息包，并处理
	void OnHandleNewRoundBegin(BYTE * buffer,int nLen);

	//没有玩家进行游戏，退出处理
	void OnHandleNoPlayer(BYTE * buffer,int nLen);

	//玩家手上的钱不足够时的处理接口
	void OnHandleNotEnoughMoney(BYTE * buffer,int nLen);

public:
	// 按ID号标识定时器的作用，参数为位置，最大时长，用途
	void ShowTimer(BYTE bDeskStation, UINT uTimeID, UINT uMaxTime);

	// 带入金额定时器
	void ShowTimers(UINT uTimeID, UINT uMaxTime);

	//其他玩家得到令牌的UI处理
	void OnShowOtherPlayerGetToken(const TToken * pToken = NULL);

	//系统自动清理片
	void OnUserSortOut();

	//自己得到令牌的UI处理
	void OnShowMeGetToken(const TToken * pToken);

	//自动托管处理
	bool AutoTrusteeship(const TToken * pToken);

	//托管按钮显示/隐藏
	void OnAutoTrusteeship(int nCmdShow);

	//自己下家UI处理
	void OnShowPdg(int nCmdShow);

	//系统配置桌子数据后的UI处理，主要显示游戏规则
	void OnShowSysDeskCfg(const TDeskCfg* pDeskCfg);

	//玩家配置桌子数据后的UI处理，主要显示底/顶注
	void OnShowSubMoney(const TSubMoney * pSubMoney);

	//令牌消息处理
	void OnShowGetToken(const TToken* pToken,BYTE byTime = 0);

	//显示边桌面池数据处理
	void OnShowPots(const TBetPool* pBetPool);

	//逆时针顺序得到下家位置
	BYTE GetNextUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//逆时针顺序得到下家位置
	BYTE GetNextUserStations(BYTE byStation, BOOL bIsAll = TRUE);

	//逆时针顺序得到上家位置
	BYTE GetLastUserStation(BYTE byStation, BOOL bIsAll = TRUE);

	//清空记录上一个操作玩家栈列表
	void ClearAllStack();

	//收集玩家筹码与金币处理接口
	void OnShowCollectJotens(BYTE bDeskStation);

	//确定带入金额
	void OnOKLOAD();

	//显示玩家Logo
	void OnShowUserLogo();

	//关闭所有玩家头像
	void CloseAllLogo(BYTE bDesk = 255);

	// 显示用户信息
	void OnShowUserInfo();

	// 初始化所有UI
	void OnInitYuanSu(bool bShow);

	// 显示庄、大小盲注的标记动画
	void OnPlayMoveFlagAnim(BYTE bDeskStation,UINT type,bool bHandout);

	// 发牌动画
	void OnFaPaiAnim();

	// 停止定时器
	void OnEndTimer(BYTE bDeskStation);

	//结算颁发筹码接口
	void OnHandleDispatchJetons();

	//回合结算显示的UI处理，弹出结算对话框
	void OnShowRoundResult(const TResult* pResult);

	//颁发筹码UI处理接口
	void OnShowDispatchJotens(BYTE bDeskStation,__int64 iMoney);

	//清理桌子环境
	void OnShowDeskTop();

	//清除所有定时器
	void OnClearTimer();

	// 自动带入金额
	void AutoInBox();

	// 玩家设置游戏信息
	int UserSetGameInfo(bool bSoundEnable,bool bWatchEnable,bool bShow);

	// 播放背景音乐
	void PlayBeiJingSound();

	// 播放闹钟音乐
	void PlayTimerSound();

	// 播放游戏下注音乐
	void PlayBetSound(BYTE bDeskStation,int SoundID);

	// 播放游戏公共音乐
	void PlayPublicSound(int SoundID);

	// 游戏闪动
	void FlashWindow();

public:
	// 修改注额
	bool OnLoadSelectionDlg(bool bAuto = false);

	// 玩家下注处按钮理接口
	void OnHandleBet();

	// 玩家跟注按钮处理接口
	void OnHandleCall();

	// 玩家加注按钮处理接口
	void OnHandleRaise();

	// 玩家过牌按钮处理接口
	void OnHandleCheck();

	// 玩家弃牌按钮处理接口
	void  OnHandleFolo();

	// 玩家全下按钮处理接口
	void OnHandleAllin();

	// 玩家金额按钮处理接口
	void OnHandleButtonMoney(int nID);

	// 玩家滑动加注轴处理接口
	void OnHandleSlidingAxle();

	// 玩家加注钮处理接口
	void OnHandleAddMinus();

	// 玩家减注钮处理接口
	void OnHandleSubMinus();

	// 玩家跟注托管处理接口
	void OnHandleCallMandate(bool bFlag);

	// 玩家过牌托管处理接口
	void OnHandleCheckMandate(bool bFlag);

	// 玩家跟任何注托管处理接口
	void OnHandleCallAnyMandate(bool bFlag);

	// 玩家过牌或弃牌托管处理接口
	void OnHandlePassAbandonMandate(bool bFlag);

#ifdef CLOSE_CARD
	// 点击自己的底牌
	void OnHitMySelfBackCard();

	//点击它玩家点牌时 移动玩家的牌
	void OnHandleMovePlayerCard(BYTE * buffer,int nLen);

	//点击展示牌按钮
	void OnHitShowCard();
#endif
public:
	// 恢复基本的游戏数据
	void RecurSaveBase(const LPCVOID lpData);

public:
	//注额信息
	TDeskCfg m_tagDeskCfg;

	// 代入金额信息
	TSubMoney m_tagSubMoney;

	// 结算信息
	TResult m_tagResult;

	//每位玩家手的数据
	CUserDataCliObject m_UserData[PLAY_COUNT];

	//庄家位置
	BYTE m_byNTUser;

	//小盲注位置
	BYTE m_bySmallBlind;

	//大盲注位置
	BYTE m_byBigBlind;

	//底牌数据
	BYTE m_byBackCard[MAX_BACKCARDS_NUM];

	//底牌数量
	int m_iBackCardNums;

	//当前下注金币
	__int64 m_nCurrentMoney; 

	// 跟注金币
	__int64 m_nCallMoney;

	//记录下注玩家
	int m_iBetCounts;

	//游戏中的所有动作时间
	int m_iVerbTime;

	//每个用户可用金币
	__int64 m_nMoney[PLAY_COUNT];

	//每个玩家下注金币 
	__int64 m_nBetMoney[PLAY_COUNT];

	// 弃牌玩家
	bool m_bGiveUp[PLAY_COUNT];

	// 全下玩家
	bool m_bAllBet[PLAY_COUNT];

	// 下注边池
	TBetPool m_tagBetPool;

	// 记录每个玩家代入金额
	__int64 m_iMoneys[PLAY_COUNT];

	// 保存底牌显示
	BYTE m_iCardData[MAX_HANDCARDS_NUM];

	//底牌数3
	int m_iCardDataCounts;

	// 休闲场与搓桌场区分
	bool m_bInLisureRoom;

	//底牌数据
	BYTE m_iUserDataCard[MAX_HANDCARDS_NUM];

	//底牌数
	int m_iCardCounts;

	// 自己的位置
	BYTE m_byMeStation;

	//玩家位置
	int m_iUser;

	//是否已通知服务器玩家准备游戏
	bool m_bAgreeGame;

	//得到令牌的玩家
	BYTE m_byTokenUser;

	//托管标志 0: 自动跟注(只限一轮)  1: 自动跟任何注(每轮都跟注)  2: 自动过牌  3: 自动弃牌(上家有人过牌就过牌，没人过牌就弃牌)
	bool m_bTrusteeship[4];

	//记录每个玩家操作类型
	emType m_emLastOperation[PLAY_COUNT];

	//记录上一个操作玩家
	stack<BYTE> m_stackLastStation;

	//是否处理托管状态
	bool m_bTuoGuan;

	__int64 m_iMinMoney;     // 最小代入金币
	__int64 m_iMaxMoney;     // 最大代入金币
	__int64 m_iCurrentMoney; // 当前要代入的金币
	__int64 m_iSelfMoney;    // 自己本人拥有的金币
	__int64 m_iMinusMoney ;  // 修改注额加减金币

	UINT    m_uTime;         /**< 定时器秒数 */

	// 庄、大小盲注移动动画
	IMoveAction * m_pMove[3];

	// 筹码回放移动动画
	IMoveAction * m_pAction[8];

	// 金额回放移动动画
	IMoveAction * m_pMoveAction[8];

	//已显示的牌
	BYTE m_byShowed[MAX_HANDCARDS_NUM];

	//发牌玩家队列
	struct TSendCardAnimNode
	{
		BYTE byUser;
		int  iPos;

		TSendCardAnimNode()
		{
			byUser = 255;
			iPos = 0;
		}
	};

	std::queue<TSendCardAnimNode> m_qSendCards;

	//发牌类型
	emTypeCard					m_nTypeCard;

	bool						m_bTransceiving;

	// 隐藏掉庄大小盲注
	bool						m_bNtDaXiao;	

	int							m_nCount;
	// 音乐开关
	bool						m_bSoundPlay;	
	// 是否允许旁观
	bool						m_bAllowWatch; 
	// 播放背景声音引擎
	ISoundEngine		*		m_pSoundEngine;
	// 游戏下注声效引擎
	ISoundEngine		*		m_pTimerSMusic;
	// 游戏公共声效引擎
	ISoundEngine		*		m_pSEBkMusic;
	// 游戏下注声效引擎
	ISoundEngine		*		m_pBetSMusic;

	// 旁观者
	bool m_bWatch;

	// 是否是玩家
	int  m_iUserStation;

	// 是否为断线重回玩家
	bool m_bIsOffline;

	// 玩家为真
	bool m_bpUserInfo[PLAY_COUNT];

	// 自动开始时间锁
	bool m_bAutoStartTimer;
	
	// 显示收集筹码总注
	__int64 m_nBetMoneys;

};