#include "IGameImpl.h"
#include "..\GameMessage\UpGradeLogic.h"
// #include "AFCMessageBox.h"
#include <irrKlang.h>

///定时器
#define		TIME_GAME_BEGIN                10    ///<开始时间

//定时器 ID
#define ID_BEGIN_TIME				    100					//开始定时器
#define ID_OUT_CARD_TIME				110					//出牌等待时间
#define ID_TIME_CLOCK                   115                 //倒数闹钟计时器
#define ID_PLAY_MOVE_VSCARD             116                 //开始移回比牌的牌计时器
#define ID_TIME_LOOKCARD                117                 //开始移回比牌的牌计时器


#define  SOUND_OUT_CARD_PASS            0x01               //不出声音
#define  SOUND_OUT_CARD_BOMB            0x02               //炸弹声音
#define  SOUND_OUT_CARD                 0x03               //出牌声音
#define  SOUND_GAME_OVER_WIN            0x04               //胜利声音
#define  SOUND_GAME_OVER_LOSE           0x05               //失败声音
#define  SOUND_START_DEF                0x06               //开始声音
#define  SOUND_WARNING                  0x07               //告警
#define  SOUND_BG_1					    0x10				//背景1
#define  SOUND_BG_2					    0x11				//背景2
#define  SOUND_BG_3					    0x12				//背景3
#define  SOUND_CHOUMAMOVE               0x13               //筹码移动
#define  SOUND_CLICKBUTTON              0x14               //按下按钮上
#define  SOUND_MOVEBUTTON               0x15               //移动到按钮
#define  SOUND_SENDCARD                 0x16               //发牌
#define  SOUND_VSCARD                   0x17               //比牌动画

using namespace irrklang;

#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\
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
	virtual 	void SetGameStation(void * pBuffer,int nLen);

	virtual bool OnGameQuiting();


private:                       ///游戏自己的操作

	//设置单选框控件是否被选中
	void SetRButtonSelected(const int iRButtonID,bool bSelected);
	//获取单选框控件是否被选中
	BOOL GetRButtonSelected(const int iRButtonID,bool *bSelected);
	//设置容器控件可见
	void SetContainVisible(const int iContainID,bool bFlag);
	//设置文本内容
	void	SetTextInfo(const int iTextID,CString sText,bool bShow);
	//设置可操作牌控件信息
	void	SetOperateCardInfo(const int iID,BYTE BList[] ,int nNum,bool bShow);
	//设置按钮不可用
	void	SetButtonEnable(const int iID,bool bEnable);
	//{{Added by zxd 20090820 取得下一个玩家，比牌结束后，需要获取到底哪个玩家达到下注上限
	BYTE GetNextNoteStation(BYTE bCurDeskStation)
	{
		BYTE bNextStation = bCurDeskStation;

		int kkk = PLAY_COUNT - 1;   //逆时针
		for (int i = 0;i < PLAY_COUNT;i++)
		{
			bNextStation = (bNextStation + kkk) % PLAY_COUNT;
			if(m_iDeskCardCount[bNextStation] == 0)
				continue;	

			if (!m_bIsGiveUp[bNextStation])
				break;
		}
		if (bNextStation == bCurDeskStation)
		{
			return 255;
		}
		return bNextStation;

	}

	///处理人物LOGO
	void ShowLogo();
	///关闭所有玩家头像
	void CloseAllLogo(BYTE bDesk = 255);

	int ViewStation(int desk);

	//游戏UI重配
	void InitGameUI();

	//设置手牌
	void SetHandCard(BYTE bDesk, BYTE iCardList[], BYTE iUpCard[], int iCardCount, int iUpCardCount);

	// 显示/隐藏 控件
	BOOL ShowControl(int iControlID, bool bShow);

	/// 查询按钮可用/禁用 控件（只支持按钮）
	BOOL GetControlEnable(int iControlID);

	/// 设置可用/禁用 控件（只支持按钮）
	BOOL EnableControl(int iControlID, bool bShow);

	/// 设定游戏定时器
	BOOL SetGameTimer(BYTE bDesk, int TimeLen, int TimerID);

	/// 消除定时器
	BOOL KillGameTimer(int TimerID = 0);

	/// FlashWindowEx封闭）
	void FlashWindow();

	/// 查询按钮显示/隐藏 控件（只支持按钮）
	BOOL GetControlShow(int iControlID);

	//托管直接调用此函数
	BOOL UserOutCard();

	/// 显示 结算框
	void ShowResult(GameFinishNotify szResultInfo);

	/// 
	void SetCard();

	//
	void SetNT(int iNTDesk);

	//====开始游戏,开始发牌
	bool OnControlHitBegin();


	//设置玩家信息
	void SetUserInfo(int iDesk = -1);

	//游戏逻辑数据重配
	void InitGameData();

	//排序
	void OnSort();

	void OnLast();

	///加载配制文件
	void LoadData(void);

	///  播放音效
	void SetPlaySound(int iSoundID, BOOL bIsBoy = TRUE, BOOL bISLoop = FALSE);

	//随机背景音乐
	BOOL PlayRandSound(BYTE bStartType, BYTE bEndType, bool isLoop);

	/// 设置玩家余牌数
	void SetCardNum(int iDesk,int iNum);

	//设置房间信息
	void UpdateRoomInfo();

	//初始化总注
	void SetTotalNote(BYTE iViewStation,int iNoteTotal);
	
	//加注
	void AddTotalNote(BYTE iViewStation,int iNoteTotal);


	void SetMing(BYTE bDeskStation,BOOL iGiveup);

	//设置自己的可用余额
	void UpdateMyLeftMoney();

	//设置自己的信息
	void UpDateMyUserInfo();

	//隐藏所有的提示信息
	void DisVisibleAllTip(bool bIsShowGaveUpAndLose=false);  


	//屏蔽所有的加注提示
	void DisVisibleAllAddTip();
	//屏蔽所有的下注提示
	void DisVisibleAllChipinTip();
	//屏蔽所有的跟注按钮
	void DisVisibleAllFollowTip();
	//屏蔽所有的放弃提示
	void DisVisibleAllGiveUpTip();    //放弃提示
	void DisVisibleAllOpenLoseTip();  //比牌失败提示
	void DisVisibleAllLookTip();      //看牌提示
	void DisVisibleReachLimiteTip();  //达到上限提示
	void DisVisibleAllOpenWinTip();     //屏蔽所有的比牌胜利提示
	//设置提示信息
	void SetTipInfo(BYTE bDeskStation,int iOperateType);

	//显示各玩家下注状况
	void UpdateNoteInfo(BYTE bDeskStation, int nNoteValue = 0);

	//播放发牌动画
	void PlaySendCard(BYTE Station);

	void StartPlaySendCard();

	int GetTotalPlayNum();
	//--------------------------界面交互函数及按钮控制函数---------------------

	void UpdateShowNoteBtn(bool bOpen=false,bool bAdd=false,bool bGiveUp=false,bool bFollow=false,bool bLook=false,bool bChipIn=false);

	void SetCtrlBtn(bool bIsFirstNote);
	//设置加注多少的按钮状态
	bool UpdatAddNumBtn(bool bFirst);

	//void DrawUserInfo();
	//屏蔽所有的游戏操作按钮
	void DisableAllOperaBtn();
	//
	void EnableAddOrNote();
	
	//设置加筹码显示的操作
	void UpdateAddChipShow(int bMutBase);
	//---------------------------------------------
	inline int GetLimit() {return (m_iLimitPerNote < m_iLimitNote ? m_iLimitPerNote : m_iLimitNote);}

	//游戏按钮操作
	bool OnChipIn();   //下注操作
	bool OnFllow();    //跟注操作
	bool OnAdd();      //加注操作
	bool OnGiveUp();   //放弃操作
	bool OnLook();     //看牌操作  变为名牌玩家  
	bool OnOpen();     //比牌操作

	//玩家换牌
	void	OnUserChangeACard();


	//加注金额
	bool OnAddCount(int bMutBase);  //点击x1,x2,x3按钮
	bool OnAddCancle();              //点击加注旁边的取消按钮
	bool OnCompareCard(BYTE bViewStation);	// 点击针对某玩家的比牌按钮


	//下注筹码动画
	void PlayXianShiChouMa(int nDeskStation, int iNum,bool bFlag);;
	UINT PlayOpenCard(BYTE bDeskStation1,BYTE bDeskStation2,int iCmpRet);
	////移回比较的动画
	UINT MoveVSCard();

	//收集赢的筹码动画
	void PlayGetWinChouMa(int nDeskStation);

	// 比牌动画结束
	void OnFinishOpenCard();

	//用户押注声音(专为扎金花准备)
	bool SetNoteSound(BYTE bNoteStyle,BOOL bIsBoy);

	//获取随机地址
	void GetRandAddress(CString &Str,BYTE bDeskStation);
	//获取随机用户ID
	void GetRandUserID(CString &Str,BYTE bDeskStation);

	/// 获取游戏状态
	BYTE GetStationParameter(void);

	///自动设定比牌背景大小
	void AutoSetBackWH();
	//超端验证处理
	bool SuperTest(void *pBuffer);

	//显示换牌界面
	void	ShowChangeDlg(bool bShow);
	//显示换牌提示界面
	void	ShowChangeMsgDlg(bool bShow);
	
private:
    IGameFrame                  *m_pGameFrame;

    IUserInterface              *m_pUI;

	CUpGradeGameLogic		    m_Logic;					//逻辑规则

	ISoundEngine               *m_SoundEngine;

	BYTE                         m_bGameStation;               ///<游戏状态

	///运行状态变量

	BYTE					  m_iUserCardCount[PLAY_COUNT];		///<用户手上扑克数目

	BYTE					  m_iUserCard[PLAY_COUNT][54];		///<用户手上的扑克

	BYTE					  m_iDeskCardCount[PLAY_COUNT];		///<桌面扑克的数目

	BYTE					  m_iDeskCard[PLAY_COUNT][54];		///<桌面的扑克

	
	///游戏基本数据
	BYTE                      m_iBeginTime;                     ///<游戏开始时间

	BYTE                      m_iThinkTime;                     ///<游戏思考时间

	BYTE                      m_iEndWaitTime;                   ///<游戏结束后等待时间

	int                       m_iPlayCardCount;                 ///<使用扑克数

	int                       m_iRoomBasePoint;                 ///<房间倍数

	int                       m_iDeskBasePoint;                 ///<桌子倍数

	int                       m_iRunPublish;                    ///<逃跑扣分数

	int						  m_iChangeCardMoney;				///换牌需要的金币数
	int                       m_iBeenPlayGame;                  ///游戏局数
 
	int                       m_iCurrentTimeId;                 ///当前计时器ID

	BOOL                      m_bAutoCard;                      ///是否托管

	BYTE                      m_bSortCard;                      ///排序类型

	BOOL                      m_bSendTestCard;                  ///是否开启测试发牌

	BOOL                      m_bSound;                        ///是否开启音效

	int                       m_TimeClockCount;                ///闹钟倒计时时长

	BYTE                      m_bTimeOutCount;                 ///超时次数

	bool					  m_bInLisureRoom;				//该游戏是否在悠闲房


	__int64						m_iNowBigNote;				//当前最大押注数
	__int64						m_iThisGameNote[PLAY_COUNT];			//本局游戏下注数(明注)	
	__int64						m_iTotalGameNote[PLAY_COUNT];			//本局游戏下注数(实际)


	BYTE					   m_iThisNoteTimes;			//押注轮数
	int                        m_iFrontNote;                //当前有效下注数即前位玩家有效下注数;
	int                        m_iMyAddNote;                //我加注的值 
	//房间信息
	__int64						   m_iLimitNote;				//最大限额注
	int						   m_iBaseNote;				//底注
	int                        m_iGuoDi;                   //锅底
	__int64                        m_iLimitPerNote;            //每注限制，为暗注下限制，但显示为明注的限制

	__int64						   m_iThisTurnLimit;			//本轮限注
	//显示玩家还是显示id
	BOOL m_CbIsShowIdOrPlay;

	CString                    m_strText[20]; //用户城市随机地址

	CString                    m_strCityText[PLAY_COUNT];
	CString                    m_strUserIDText[PLAY_COUNT];
	bool                       m_bSuperUser[PLAY_COUNT]; 
	bool                       m_bLookedPai;
	int						   m_iNoteTotalPoint;					//总注
	int						   m_iNoteLimit;						//本轮上限
	BOOL                       m_bMing[PLAY_COUNT];                        //明牌
	int						   m_iNowOutPeople;			//现在出牌者

	int						   m_iFirstOutPeople;			//第一个出牌者
	bool                       m_bOpenLose[PLAY_COUNT];     //玩家的比牌结果
	bool                       m_bIsGiveUp[PLAY_COUNT];     //玩家是否放弃

	

	BYTE                       m_bNtPeople;   //庄家
	bool                       m_bFirstGame;
	BYTE                       m_byWin[PLAY_COUNT];
	BYTE                       m_iComWinStation;			// 比牌胜者
	BYTE                       m_iComLoseStation;			// 比牌负者
	BYTE                       m_iComFinishWinner;			// 比牌结束后，整个游戏
	BYTE                       m_bReachLimitStation;       //达到上限玩家
	bool                       m_bGameFinishAndComp;        //是否比牌游戏结束
	BYTE                       m_bComCardStation[2];         //比牌的两个位置
	POINT                      m_poComCardXY[2];               //两个比牌的起牌位置
	bool                       m_bMovCardStart;            //是否进入比牌动画阶段

	BYTE                       m_bLookCardW;               //看牌的间隔宽度
	BYTE                       m_bSendCardPos;             //当前发牌动画的人
	BYTE                       m_bSendCardCount[PLAY_COUNT]; //当前发牌动画的张数

	BOOL                       m_bPlayer[PLAY_COUNT]; //是否玩家或中途进入观战者
	//超端
	bool					   m_bSuperWin[6];//赢单选框
	bool					   m_bSuperLose[6];//输单选框
	bool					   m_bIsSuperUser;//是否超端

	bool						m_bHaveChanged;	//是否已经换牌了

	BYTE						m_byChangeCard;	//被换牌的数据
};