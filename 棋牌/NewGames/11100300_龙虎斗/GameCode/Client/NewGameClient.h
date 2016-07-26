#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include <deque>
using namespace irrklang;

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A
#define  TIME_SENDLONG_CARD     201     //玩家发龙牌定时器
#define  TIME_SENDHU_CARD       202     //玩家发虎牌定时器
#define  TIME_SHOW_LONG			203		//玩家出龙标志牌定时器
#define  TIME_SHOW_HU			204		//玩家出虎标志牌定时器
#define  TIME_SHOW_WIN_TIME		205		//玩家显示输赢定时器
#define  TIME_SHOW_HUANZHUANG   206		//换庄定时器
#define  TIME_SHOW_MONEY		207		//金币提醒定时器
#define  TIME_SHOW_JIEPAI		208		//玩家出龙标志牌定时器
#define  TIME_AWARDSHOW			209		//中奖框显示
#define  TIME_ClOCK				210		//玩家发龙牌定时器
#define SOUND_GAME_A			  0x01
#define SOUND_GAME_B			  0x02
#define SOUND_GAME_C			  0x03
#define SOUND_GAME_D			  0x04
#define SOUND_GAME_E			  0x05
#define SOUND_GAME_F			  0x06
#define SOUND_GAME_G			  0x07
#define SOUND_GAME_H			  0x08
#define SOUND_GAME_I			  0x09
#define SOUND_GAME_J			  0x0A
#define SOUND_GAME_K			  0x0B
#define SOUND_GAME_L			  0x0C
#define SOUND_GAME_M			  0x0D
#define SOUND_GAME_N			  0x0E
#define SOUND_GAME_O			  0x0F
#define SOUND_GAME_P			  0x1F



interface IGameFrame;
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
	
	IGameFrame         *m_pGameFrame;											//框架游戏指针
	
	IUserInterface     *m_pUI;													//界面游戏指针

	ISoundEngine       *m_SoundEngineGame;										//游戏音效

	ISoundEngine       *m_SoundEnginBg ;										//背景音效
private:
	BOOL		m_bIsSuperUser;													//是否超级用户
	BOOL		m_bIsReady;														//游戏是否准备好
	bool		m_bIsPlaying;													//游戏是否已经开始
	BYTE		m_byCardStyle[2];												//发牌类型
	BYTE		m_byCardSort;													//发到第几张牌了
	BYTE		m_iPlayJuShu;													//游戏局数

	deque<BYTE> m_dbyLuzi;														//路子双端队列
	
	
	
	int			m_iPlayBaShu;													//自己的把数
	int			m_iPlayGrade;													//自己的成绩
	
	__int64		m_i64MeHaveWin;													//玩家输赢情况
	//服务端发过来的结算数据
	__int64     i64ChangeMoney[PLAY_COUNT];										//玩家得分
	__int64     i64ReturnMoney[PLAY_COUNT];										//返还的金币数
	__int64     i64PlayerGrade[PLAY_COUNT];										//玩家成绩
public: 

	CUpGradeGameLogic   m_cLogic ;												///客户端逻辑
	BOOL				m_bAwardDisplayFlag;									//开奖位置显示标签
	BOOL				m_bSoundIsClose;										//是否关闭声音
	BOOL				m_bUserExitIsClose;										//是否关闭用户进出提示信息

	//基础时间
	BYTE				m_bySelChouMaType;										//当前选择的筹码类型
	BYTE				m_byArea;												//中奖区域
	BYTE				m_byWaitNtList[PLAY_COUNT];								//上庄列表
	BYTE				m_byLuziList[52];										//路子
	BYTE                m_byGameStation ;										///当前的游戏状态
	BYTE                m_byFreeTime;											//空闲时间
	BYTE                m_byBetTime;											//下注时间
	BYTE                m_byOpenCardTime;										//开牌时间
	BYTE                byAccountTime;											//结算时间
	BYTE                m_byShowResult;											//展示结果时间
	BYTE                m_bySendSpaceTime;										//发牌间隔时间
	BYTE                m_byShowWinTime;										//显示输赢
	BYTE                m_bySpeedTime;											//遮挡牌移动速度
	BYTE            	m_iClock;												//剩余时间
	BYTE            	m_iClockLable;											//时钟标签
	BYTE				m_byNtStation;											//庄家位置
	BYTE				m_byNtWaitListCounter;									//上庄列表计数器
	BYTE				m_byMaxNtPeople;										//最大上庄人数
	int					m_iNtPlayCount;											//庄家坐庄次数
	int                 m_iMyDeskStation ;										///玩家自己的游戏位子
	int					m_iList;												//列表显示计数器

	__int64             m_i64ApplyNtLimite ;									//庄家上庄条件
	__int64             i64NtGrade;												//庄家成绩
	__int64				m_i64NtHaveWin;											//庄家输赢情况
	__int64             m_i64NtScore;											//庄家成绩
	__int64             m_i64PlayerCurrentMoney;								//玩家当前金币
	

	bool				m_bShowOrHideRobot;										//是否显示机器人下注数

	__int64				m_i64AreaAllSum[BET_ARES];								//含机器人下注
	__int64				m_i64RealityAreaAllSum[BET_ARES];						//不含机器人下注
private:
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

	//设置数字空间显示内容
	void	SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag);

	//设置数字空间显示内容(不带加号和减号及逗号)
	void	SetInumInfo_1(const int iNumID, __int64 i64Num, bool bFlag);

	//设置筹码控件显示值
	void	SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);

	//空闲消息
	void	HandleGameFree(void * pBuffer);

	//处理开始
	void    HandleStart(void *buffer);

	//获取指定玩家的剩余金币
	__int64	GetUserRemaindMoney(BYTE byDestStation);

	//收到开始下注消息
	void	HandleBeginBet(void * pBuffer);

	//显示谁输谁赢
	void HandleWhoWin(void *pBuffer);

	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iButtonID, CString sImagSrc,bool bFlag,int x,int y,int w,int h);

	void ChaoGuo(void *buffer);

	//设置数字空间显示内容(第三个参数:0:显示加号，减号，和逗号；1：只显示逗号；2：都不进行设置)
	void	SetInumPro(const int , __int64 , BYTE ,bool );

	//显示自己的信息
	void ShowMyInfo(bool bFalg);

	//显示庄家信息
	void	ShowNtInfo(bool bFalg);

	//设置下注按钮可用
	void	SetBetBtEnable(bool bFalg);

	//更新显示路子
	void	ShowLuziList(bool bFlag);

	//显示游戏结算框
	void	ShowGameResult(void *buffer) ; 

	//获取某位玩家的总下注情况
	__int64	GetUserBetSum(BYTE byDestStation);
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
	void ReSetUiDlag();

	///初始化游戏中的变量
	void InitialGameData();  

	///初始化部分数据
	void ResetGameData();

	///删除所有定时器
	void KillAllGameTime() ;

	//播放背景音效
	void PlayBgSound(bool bISLoop);

	//播放游戏音效
	void PlayGameSound(int SoundID, bool bISLoop = false);

	///玩家进入游戏
	void SetGameStation(void * pBuffer,int nLen);
	
	//设置文本控件显示内容（显示字符）
	void SetChinfo(const int iTextID,TCHAR ch[61], bool bFlag);

	//申请上庄
	void UserApplyNtStation();

	//申请上庄结果处理
	void HandleApplyResult(void *buffer);

	//开牌
	void HandleKaiPai(void *buffer);

	///显示玩家的发牌动画
	void ShowUserSendCard(const int ID,BYTE cardStyle,BYTE cardCount,bool bShow);

	//处理下注结果
	void HandleUserBetResult(void *buffer);

	//接受结算数据
	void HandleReceiveData(void *buffer);

	//显示结算框
	void HandleShowResult();

	//空闲状态
	void HandleFreeStation(void * pBuffer);

	//下注状态
	void HandleXiaZhuStation(void * pBuffer);

	//开牌状态
	void HandleKaiPaiStation(void * pBuffer);
	
	//申请下庄
	void UserApplyLiveNt();

	//处理申请下庄结果消息
	void HandleLeaveNtResult(void *buffer);

	/// 玩家强退结束游戏
	bool UserExit(WORD nSubId,BYTE * buffer);

	//玩家进入游戏
	void PlayCome(void *buffer);
	

	//升级
	//控制上庄下庄按钮的使用和显示
	void BtNtDisp(BYTE );

	//控制能否下注
	void BtEnable(BOOL);

	//设置下注按钮可用
	void SetBetBtEnable(bool bFalg,BYTE);

	//显示上庄列表
	void ShowNtWaitList(bool bFalg);

	//显示阶段图片
	void ShowSection(BYTE);

	//显示中奖框
	void ShowAwardFrame();

	//用户准备阶段退出消息
	void HandleGamePrepareQuit(void * pBuffer);

	//显示上庄列表
	void ShowWaitNtInfo(bool bFlag);

	//上庄列表移动
	void NtListMove(BYTE);

	//统计上庄列表人数
	BYTE NtListCount();

	//显示路子
	void ShowLuzi();

	//隐藏路子
	void HideLuzi();

	//计算龙虎和的个数
	void AccountResult();

	//清除龙虎和的个数
	void ClearAccountResult();

	//显示超端结果界面
	void ShowSuperResult(bool bFlag);

	//清空超端用户界面
	void ClearShowSuperResult();

	//设置单选框控件是否被选中
	void SetRButtonSelected(const int iRButtonID,bool bSelected);

	//获取单选框控件是否被选中
	BOOL GetRButtonSelected(const int iRButtonID,bool *bSelected);

	//设置显示显示机器人或不显示机器人按钮
	void SetShowOrHideRobotBnt();

	//处理超端用户信息
	void HandleSuperMessage(void *buffer);

	//查看路子
	void ViewLuzi(const int iContainID);

	//游戏设置确定/取消
	void GameSet(BYTE bButton);

	//音量加/减
	void SoundChange(BYTE bButton);
	//设置动画控件播放动画
	//void	SetAnimatePlay(const int AnimateID,bool bFlag,BYTE byPlayNum = 1, bool bIsLoop = false);
	//设置动画控件播放指定动画
	//void	SetAnimatePlay(const int AnimateID,wchar_t wsFilePath[],bool bFlag,BYTE byPlayNum = 1, bool bIsLoop = false);
	//设置动画显示某一帧
	//void	SetAnimateShowFrame(const int AnimateID,bool bFlag,int iFrame);
	//设置是否显示机器人下注数
	void SetShowRobotNote();

};