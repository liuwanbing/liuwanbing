



#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A

const   int  GAME_WAIT_STATION    = 1 ;
const   int  GAME_BEGIN_STATION   = 2 ; 
const   int  GAME_FINISH_STATION  = 3 ; 

const  int  TIME_GAME_BEGIN     = 200 ; ///游戏开始定时器
const  int  TIME_NO_BET			= 201 ;	///多少秒不能下注定时器
const  int  TIME_RESULT_SHOW    = 202 ; ///中奖区域闪烁
const  int  TIME_ONE_TIME		= 203 ; ///1秒倒计时


//声音类型设置
#define SOUND_GAME_BEGIN			0x10				//游戏开始
#define SOUND_BEGIN_BET				0x20				//下注开始声音
#define SOUND_COUNTDWON				0x30				//下注倒计时
#define SOUND_MOUSE_ENTER			0x40				//鼠标移动
#define SOUND_WIN					0x50				//赢了音效
#define	SOUN_LOST					0x60				//输了音效
#define SOUND_BET					0x70				//下注音效


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

	ISoundEngine       *m_SoundEngineGame;        ///游戏音效

	ISoundEngine       *m_SoundEnginBg ;      ///背景音效


public: 

	BYTE                m_byGameStation ;         ///当前的游戏状态
	int                 m_iMyDeskStation ;       ///玩家自己的游戏位子
  
	BYTE				m_byFreeTime;		//空闲时间
	BYTE				m_byBetTime;		//下注时间
	BYTE				m_byNoBetTime;		//多少秒不能下注
	BYTE				m_byBallotTime;		//藏签时间
	BYTE				m_byOpenTime;		//开奖时间
	BYTE				m_byShowResultTime;	//结算显示时间

	BYTE				m_byRemaindTime;		//剩余时间
	
	BYTE				m_byNtStation;		//庄家位置
	BYTE				m_byNtSelArea;		//庄家藏签结果
	BYTE				m_byLastResult;		//上轮开奖结果
	BYTE				m_byCurrResult;		//当局开奖结果
	bool				m_bShowResult;		//是否显示中奖区域

	BYTE				m_byWaitNtList[PLAY_COUNT];	//上庄列表
	BYTE				m_byLuziList[10];	//路子

	int					m_iApplyNtLimite;	//上庄条件

	BYTE				m_bySelChouMaType;	//当前选择的筹码类型
	__int64				m_i64SumAllBet;		//所有总下注
	__int64				m_i64MyBetCount;	//自己的总下注数
	__int64				m_i64AreaBetAllSum[BET_ARES];		//各区域的总下注
	__int64				m_i64UserBetCount[PLAY_COUNT][BET_ARES];	//各玩家在每个区域的下注情况

	int					m_iBetSumLimite;			//总下注限制
	int					m_iBetAreaLimite[BET_ARES];	//下注各区域限制

	int					m_iCurrWaitNtTag;			//当前路子标签

	bool				m_bIsSuperUser;				//是否超级用户
	bool				m_bIsReady;					//是否点击了准备


	bool                m_bSoundPlay ;		//是否可以播放声音
	bool                m_bSoundBgPlay ;	//背景音效
	bool                m_bWatchMode ;		//是否是旁观模式	DWJDELETE



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

private:
	//游戏开始
	void	HandleGameBegin();
	//收到开始下注消息
	void	HandleBeginBet(void * pBuffer);
	//收到开奖出签消息
	void	HandleOpenResult(void * pBuffer);
	//游戏结算
	void	HandleGameResult(void * pBuffer);
	//空闲消息
	void	HandleGameFree(void * pBuffer);
	//申请上庄结果消息
	void	HandleApplyResult(void * pBuffer);
	//申请下庄结果消息
	void	HandleLeaveNtResult(void * pBuffer);
	//玩家下注结果消息
	void	HandleUserBetResult(void * pBuffer);
private:
	//申请上庄
	void	UserApplyNtStation();
	//申请下庄
	void	UserApplyLiveNt();
	//玩家下注
	void	UserBet(int iBetArea);
	///庄家选择藏签
	void	NtSelBallot(int iBetArea);
	//处理庄家藏签确认 
	void 	NtSureBallot();
	//获取玩家剩余的金币
	__int64	GetMyRemaindMoney();
	//获取指定玩家的剩余金币
	__int64	GetUserRemaindMoney(BYTE byDestStation);
	//获取某位玩家的总下注情况
	__int64	GetUserBetSum(BYTE byDestStation);
	//是否在上庄列表中
	bool	IsInWaitNtList();

private:
	//显示提示申请上庄消息
	void	ShowPromptDlg(bool bFlag);
	//显示自己的信息
	void	ShowMyInfo(__int64 i64MyWinMoney,bool bFalg);
	//显示庄家信息
	void	ShowNtInfo(__int64 i64NtHaveWin, int iNtPlayCount,bool bFalg);
	//设置下注按钮可用
	void	SetBetBtEnable(bool bFalg);
	//设置开奖结果图片位置
	void	SetResultPosition(bool bFlag);
	//更新显示路子
	void	ShowLuziList(bool bFlag);
	//显示上庄列表
	void	ShowWaitNtInfo(bool bFlag);
	///显示游戏结算框
	void	ShowGameResult(__int64 iMyPoint, __int64 iNtPoint, __int64 iMyReturn,bool bFlag) ; 
	//显示自己的下注情况
	void	ShowBetInfo(bool bFlag);
	//显示藏签确定提示对话框
	void	ShowBallotDlg(int iBetArea, bool bFlag);
	//显示超端结果界面
	void	ShowSuperResult(bool bFlag);
	//显示游戏设置对话框
	void	ShowGameSetDlg(bool bFlag);
	


private:
	//设置动画控件播放动画
	void	SetAnimatePlay(const int AnimateID,bool bFlag,BYTE byPlayNum = 1, bool bIsLoop = false);
	//设置动画显示某一帧
	void	SetAnimateShowFrame(const int AnimateID,bool bFlag,int iFrame);
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
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,TCHAR sChar[], bool bFlag);
	//设置数字空间显示内容
	void	SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag);
	//设置筹码控件显示值
	void	SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);
	//设置单选框控件是否被选中
	void	SetRButtonSelected(const int iRButtonID,bool bSelected, bool bFlag);


public:
	//重置UI界面
	void	ReSetUiDlag();
	///初始化游戏中的变量
	void	InitialGameData();  
	///初始化部分数据
	void	ResetGameData();
	///删除所有定时器
	void	KillAllGameTime() ;
	//读取游戏设置
	void	LoadGameSet();
	//保存游戏设置
	void	SaveGameSet();

	///玩家进入游戏
	void	SetGameStation(void * pBuffer,int nLen);
	//播放背景音效
	void	PlayBgSound(bool bISLoop);
	//播放游戏音效
	void	PlayGameSound(int SoundID, bool bISLoop = false);

	///出牌声音播放
	void  SetOutCardPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop) ;


};