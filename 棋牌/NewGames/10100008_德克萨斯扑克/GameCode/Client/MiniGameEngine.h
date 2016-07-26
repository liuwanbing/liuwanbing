#pragma once

#include "defines.h"
#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "../../../../sdk/include/GameLanchSdk/BZAnima.h"
#include "AFCMessageBox.h"
#include "FinishDlg.h"
#include "LoadSelection.h"

#include "GameMachine.h"

#include <irrKlang.h>
using namespace irrklang;

#include "SoundCfgDlg.h"

#define MAST_PEOPLE			-1							//是否为管理员
//消息定义
#define IDM_BEGIN			WM_USER+120					//开始按钮消息
#define IDM_HAVE_THING		WM_USER+127					//有事要走
#define IDM_SEND_THING		WM_USER+128					//发送离开请求
#define IDM_ARGEE_LEAVE		WM_USER+129					//同意离开消息
#define IDM_STOP			WM_USER+130					//提前结束按钮
#define IDM_STOP_THING		WM_USER+131					//发送提前结束
#define IDM_ARGEE_STOP		WM_USER+132					//同意提前结束
#define IDM_PASS			WM_USER+133					//不出按钮消息
#define IDM_AUTO			WM_USER+134					//托管消息
#define	IDM_ACTIVE_RESULT	WM_USER+136					//用户处理结果
#define IDM_SHOW_NOTE		WM_USER+137					//用户选择下注,加注
#define IDM_SELECT_NOTE_NUM	WM_USER+138					//用户下注
#define	IDM_SOUND			WM_USER+139					//用户设置声音
#define IDM_BGSOUND			WM_USER+140					//背景音乐

//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置
#define GAME_RAND_TIME				5						//系统时间

const BYTE MY_VIEWSTATION = 5;				// 自己的视图位置总是6

#define TT_NONE			1	//提示类型：没有提示
#define TT_READY		2	//提示类型：准备游戏
#define TT_GENG_FAILD	3	//提示类型：跟牌失败
#define TT_THROW_FAILD	4	//提示类型：甩牌失败

#define IDT_CLOCK					204 //钟，请用用作时间不足时的提示
#define IDT_GENG_FAILD				205	//跟牌失败显示全牌的时间
#define IDT_THROW_FAILD				206	//甩牌失败显示全牌的时间
#define IDT_THROW_SHOW_BASE_CARD	207	//庄家显示底牌的时间
#define IDT_LAST_ROUND_OUTCARD		208	//玩家显示上一轮牌的时间
#define	IDT_OFF_BALANCE				209	//隐藏结算框定时器
#define	IDT_AUTO_READY			    210	//自动准备定时器

//游戏框架类 
class CMiniGameEngine : public CLoveSendClassInExe
{

public:

	//构造函数
	CMiniGameEngine();

	//析构函数
	virtual ~CMiniGameEngine();

DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

/*-----------------------------------------------------------------------------------------------------------
	一系列平台提供的需要重载来驱动游戏的函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
  ----------------------------------------------------------------------------------------------------------*/
public:

	//初始窗口，UI控件
	virtual BOOL OnInitDialog();

	// 重置游戏状态
	virtual void ResetGameStation(int iGameStation=1);

	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);

	//重新设置界面
	virtual void ResetGameFrame();

	//视图转换
	virtual BYTE ViewStation(BYTE bDeskStation);

	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

	//从BzMainFrame继承下来的函数
	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);

	//聊天消息
	virtual void ShowUserTalk(MSG_GR_RS_NormalTalk *pNormalTalk);

	//用户离开
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser);

	//用户进入为视频模块重载此函数
	virtual bool GameUserCome(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser);

	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);

	//关闭窗口
	virtual bool AFCCloseFrame();

	//退出
	virtual void OnCancel();

	//安全结束游戏
	virtual bool OnControlSafeFinish(){return false;};


/*-----------------------------------------------------------------------------------------------------------
	    声明一系列桌子特有函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
    包括通用提示，状态，计时，音效，通讯，资源回收等方面。
  ----------------------------------------------------------------------------------------------------------*/
public:	

/*-通用资源管理相关-*/
public:

	//处理人物LOGO
	void ShowLogo();

	//设置玩家姓名显示
	void ShowUserInfo(void);

	//关闭所有玩家头像
	void CloseAllLogo(BYTE bDesk = 255);

	//控件显示或隐藏
	void DispalyAllControl(int iShow);


/*-计时相关-*/
public:

	//按ID号标识定时器的作用，参数为位置，最大时长，用途
	void ShowFlashTimer(BYTE bDeskStation,UINT uMaxTime,UINT uTimeID);

	//停止并隐藏定时器
	void StopAndHideFlashTimer(BYTE bDeskStation);

	//时间动画结束后调用本函数，在里面判断该做什么后续操作
	void OnFlashTimer(BYTE bDeskStation, UINT uEventID);

/*-动画相关-*/
public:

	//处理Flash动画结束消息时的函数
	LRESULT FlashEndMessage(WPARAM wParam, LPARAM lParam);

	// 获取Flash中的牌号
	BYTE GetFlashCardValue(BYTE bCardValue);

/*-通用提示相关-*/
public:

	//判断是否在显示结算框
	bool IsResultDlgShowing();

	//通用提示框
	UINT AFCMessageBox(CString strInfo,CString strTitel, UINT style);

	//玩家设置音效后的UI处理
	void OnShowCfgSound();

/*-音效相关-*/
public:

	//播放人物声效
	void Speak(BYTE bDeskStation,DZPKSound::SpeakType st_type,DZPKSound::LanguageType lt_type = DZPKSound::LanguageType::LT_COMMON);

	//播放游戏声效
	ISound* PlayBaseSound(DZPKSound::BaseType type, bool bMusic = false);


/*-通讯相关-*/
public:

	//发送网络消息函数
	int SendGameData(BYTE bMainID,BYTE bAssistantID, BYTE bHandleCode);

	//发送网络消息函数
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);


/*-资源回收相关-*/
public:

	//退出游戏时判断4
	bool OnCloseGame();

	//安全关闭结算框
	bool SafeCloseFinishDlg();

	//安全关闭修改注额对话框
	bool SafeCloseLoadSelectDlg();

	//修改注额对话框
	bool OnLoadSelectionDlg(bool bAuto = false);

	//安全关闭声音配置框
	bool SafeCloseSoundCfgDlg();

	//关闭所有动画
	void SafeCloseFlash(UINT &ruFlashId);

/*-其它-*/
public:
	//获取玩家信息
	UserItemStructInExe * GetUserInfo(BYTE bDeskStation) { return m_pUserInfo[bDeskStation]; }

	//取得游戏状态
	int	GetGameStatus();

	//修改游戏状态
	void SetGameStatus(BYTE bGameStation);

/*----------------------------------------------------------------------------------------------------------------
	一系列声音播放接口，对内（本对象）实现声音渲染，对外（向GameEngine）暴露操作接口，每个游戏都不同
  ---------------------------------------------------------------------------------------------------------------*/
public:

/*----------------------------------------------------------------------------------------------------------------
	一系列界面更新的接口，对内（本对象）实现UI渲染，对外（向GameEngine）暴露操作接口，每个游戏都不同
  ---------------------------------------------------------------------------------------------------------------*/
/*-关于超时的UI处理-*/
public:

	//显示桌子环境
	void OnShowTableEnv();

	//清理桌子环境
	void OnShowDeskTop();

	//清除所有玩家的牌控件
	void OnClearCard();

	//设置底注UI交互
	bool OnShowCfgDesk(const TCfgDesk* pNoteSelect);

	//等待设置底注UI交互
	bool OnShowWaitCfgDesk();

	//系统配置桌子数据后的UI处理，主要显示游戏规则
	bool OnShowSysDeskCfg(const TDeskCfg* pDeskCfg);

	//点击选择底注按钮后的UI变化
	void OnShowClickedBottomCasting();

	//等待设置底注超时UI交互
	void OnShowWaitCfgDeskTimeout();

	//点击是否同意底注后的UI变化
	void OnShowClickedAgreeDeskCfg();

	//玩家配置桌子数据后的UI处理，主要显示底/顶注
	bool OnShowDeskCfg(const TDeskCfg* pDeskCfg);

	//玩家配置桌子数据后的UI处理，主要显示底/顶注
	bool OnShowSubMoney(const TSubMoney * pSubMoney);

	//显示庄家的标记动画
	void OnPlayMoveFlagAnim(BYTE bDeskStation,UINT type,bool bHandout);

	//显示庄家的标记
	void OnShowNTFlag(const TNextRoundBeginInfo* pNextRoundBeginInfo);

	//点击音效配置按钮后UI的变化处理
	bool OnShowClickedSoundCfg();

	//自己得到令牌的UI处理
	bool OnShowMeGetToken(const TToken * pToken);
	
    //自动托管处理
    bool AutoTrusteeship(const TToken * pToken);

	//显示上一玩家金币到跟注按钮UI处理
	bool OnShowCall();

	//收到下注消息后的UI变化
	bool OnShowGetBetInfo(const TBet* pBet);

	//其他玩家得到令牌的UI处理
	bool OnShowOtherPlayerGetToken(const TToken * pToken = NULL);

	//托管按钮显示/隐藏
	void OnAutoTrusteeship(int nCmdShow);

	//自己下家UI处理
	bool OnShowPdg(int nCmdShow);

	//收集筹码UI处理接口
	bool OnShowCollectJotens(BYTE bDeskStation);

	//清空筹码UI处理接口
	bool OnRemoveCollectJotens();

	//颁发筹码UI处理接口
	bool OnShowDispatchJotens(BYTE bDeskStation,int iMoney);

	//回合结算显示的UI处理，弹出结算对话框
	void OnShowRoundResult(const TResult* pResult);

	//下一回合准备就绪，弹出等待框
	void OnShowNextRoundReady();

	//新回合开始的UI处理
	void OnShowNewRoundBegin(const TNextRoundBeginInfo* pNextRoundBeginInfo);

	//发1张牌的UI处理
	void OnShowACard(const TCards* pCards);

	//发1张牌的动画处理
	void OnPlayACardAnimate(BYTE bDeskStation,int nIndex);

	//发1张牌，当运动结束后，的UI处理
	void OnEndSendMeACard(int nIndex);

	//显示边池数据处理
	void OnShowPot(const TBetPool* pBetPool);

	//显示边桌面池数据处理
	void OnShowPots(const TBetPool* pBetPool);

	//发3张牌的UI处理
	void OnShow3Card(const TCards * pCards);

	//发1张牌的UI处理
	void OnShow1Card(const const TCards * pCards);

	//没有玩家在场的UI处理，给予下注玩家提示
	void OnShowNoPlayer(const TNoPlayer* pNoPlayer, bool bHaveResultDlg = false);

	//玩家同意游戏的UI处理
	void OnShowAgree(BYTE bDeskStation);

	//所有玩家同意游戏的UI处理
	void OnShowAllAgree();

	//玩家不同意游戏的UI处理
	void OnShowDisAgree(BYTE bDeskStation);

	//玩家手上钱不够的UI处理
	void OnShowNotEnoughMoney(const TMoneyLack * pMoneyLack);


/*-关于超时的UI处理-*/
public:
	
	//等待设置底注UI交互
	void OnTimeoutCfgDesk();

	//等待设置底注UI交互
	void OnTimeoutAgreeDeskCfg();

	//等待下一回合的超时后的UI处理
	void OnTimeoutNextRoundBegin();

	//出牌超时后的UI处理
	void OnTimeoutOutCard();

/*-----------------------------------------------------------------------------------------------------------------
	其它通用界面操作函数，仅供内部使用，每个游戏都不同
  ----------------------------------------------------------------------------------------------------------------*/
private:

	//初始闹钟控件
	void OnInitClocks();

	//初始金额显示控件
	void OnInitMoneyStatic();

	//初始喊话控件
	void OnInitHanHuaControl();

	//初始所有牌控件
	void OnInitCardsControl();

	//初始分数显示控件
	void OnInit510KPoints();

	//隐藏所有提示类型的UI控件
	void HideAllNoticeControl();

	//显示发牌给某玩家的动画
	void ShowCardFlash(BYTE bDeskStation, BYTE bCard,int nIndex = 0);

	//真正开始发牌动画
	void ShowCardFlashAction(void);

	//显示刚发过的牌，如果还有未显示动画的牌，则返回非0，无待播动画返回0
	int ShowNextCardFlashAction(void);

	//设置玩家手上的扑克牌
	void SetOutCardsTo(BYTE bDeskStation,BYTE* pCard,BYTE* pUpList,int nCount,int nUpCount);

	//得到玩家手上扑克牌
	void GetHandCards(BYTE bDeskStation,BYTE* pCard,BYTE* pUpList);

	//显示玩家手上的扑克牌
	void ShowHandCards(BYTE bDeskStation,bool bShow = true);

	//清除某玩家已出的牌
	void ClearOutCardTo(BYTE bDeskStation);

	//清除某玩家手上的牌
	void ClearHandCardTo(BYTE bDeskStation);

	//取筹码动画位置
	bool GetJetonAnimRect(BYTE bDeskStation,CRect &rcStart,CRect &rcEnd);


/*-----------------------------------------------------------------------------------------------------------------
	定义桌面的数据，以下这些数据适用于大多数据游戏开发时使用
  ----------------------------------------------------------------------------------------------------------------*/
public:

	// 弹出对话框
	CAFCMessageBox *m_pMsgDlg;

	//游戏状态机
	CGameMachine m_GameMachine;

	//声效引擎
	ISoundEngine* m_pSEBase;
	ISoundEngine* m_pSEHurryUp;
	ISoundEngine* m_pSESpeak;
	ISoundEngine* m_pSEBkMusic;

	//结算框
	CFinishDlg* m_pFinishDlg;	

	//修改注额框
	CLoadSelectionDlg* m_pLoadSelectionDlg;

	//音效设置框
	CSoundCfgDlg* m_pSoundCfgDlg;

	//显示ID还是玩家
	bool m_bShowPlayerID;

	//用来缓存发牌动画数据的结构
	struct tagSendCardFlash
	{
		//哪个玩家
		BYTE byUser; 

		//什么牌
		BYTE byCard;

		//第几张牌
		int  nIndex; 

		tagSendCardFlash():\
			byUser(255),byCard(0),nIndex(0){}
	};

	//发牌状态参数队列
	std::list<tagSendCardFlash>	m_lstSendCardFlash;

	//时钟计数
	int m_iClockCounter;

/*-----------------------------------------------------------------------------------------------------------
	定义桌面的数据，以下这些数据只适用于升级游戏开发时使用
  ----------------------------------------------------------------------------------------------------------*/
private:

	//计时器动画编号 
	UINT m_uTimerFlashID[PLAY_COUNT];

/*-控件指针-*/
private:

	//闹钟数字图片指针
	BzNumberPanel *m_pClocks[PLAY_COUNT];

	//牌控件指针
	CUpGradeGameCard* m_pOutCards[PLAY_COUNT];

	//自己手上的牌控件指针
	CUpGradeGameCard* m_pMyHandCards;

	//各玩家手上底牌运动控件
	BzAnimateNode* m_pBackCardAnim[PLAY_COUNT*2];

	//各玩家手上的筹码运动控件
	BzAnimateNode* m_pJetonAnim[PLAY_COUNT];

	//各标志的运动控件
	BzAnimateNode* m_pFlagAnim[3];

	//玩家位置
	int m_iUser;

public:
	// 记录每个玩家代入金额
	int m_iMoneys[PLAY_COUNT];

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
};
