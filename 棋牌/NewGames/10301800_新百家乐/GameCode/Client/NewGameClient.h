#include "IGameImpl.h"
#include <irrKlang.h>
#include <deque>


using namespace irrklang;

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A


/*------------------------------------------------------------------------*/
const  int  TIME_CARDBG_FALL		= 200 ; ///发牌背景掉下来
const  int  TIME_CARDBG_UP			= 201 ; ///发牌背景升上去
const  int  TIME_SEND_CARD			= 202 ;	///开始发牌
const  int  TIME_SHOW_WIN_ARES		= 203 ;	///显示中奖区域
const  int  TIME_SHOW_RESULT		= 204 ;	///显示结算框



/*------------------------------------------------------------------------*/

//声音类型设置
#define SOUND_GAME_BEGIN			0x10				//游戏开始
#define SOUND_BEGIN_BET				0x20				//下注开始声音
#define SOUND_COUNTDWON				0x30				//下注倒计时
#define SOUND_MOUSE_ENTER			0x40				//鼠标移动
#define SOUND_WIN					0x50				//赢了音效
#define	SOUN_LOST					0x60				//输了音效
#define SOUND_BET					0x70				//下注音效
/*------------------------------------------------------------------------*/

#define SOUND_GAME_BG               0xff                 //背景音效
/*------------------------------------------------------------------------*/


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

	ISoundEngine       *m_SoundEngineGame;		//游戏音效

	ISoundEngine       *m_SoundEnginBg ;		///背景音效

	
public: 
	bool				m_bIsPlaying;
	BYTE                m_byGameStation ;       ///当前的游戏状态
	int                 m_iMyDeskStation ;      ///玩家自己的游戏位子
	bool				m_bWatchMode;			//是否旁观

	int					m_iXiaZhuTime;			///下注时间	
	int					m_iKaiPaiTime;			///开牌时间  
	int					m_iShowResult;			///显示结算框时间
	int					m_iFreeTime;			///空闲时间	

	__int64				m_iLimitMoney;			///<上庄需要的最少金币
	__int64				m_iMaxNote;				///当前房间最大能下的总注


	__int64				m_i64MaxZhu[BET_ARES];       //每个区域能下的最大注	
	
	short int			m_iZhuangList[PLAY_COUNT+1];		///庄家列表

	deque				<LuziData> m_DqLuziData;							//路子数据


	int					m_iNTju;				///<当前庄家进行的局数
	int					m_iNowNTstation;		///<当前庄家的位置

	__int64				m_i64MyBetCount;		//自己下注总数
	__int64				m_i64NtWinMoney;		//庄家的成绩
	__int64				m_i64MyWinMoney;		//自己输赢情况
	__int64				m_i64ZhuangScore;		//庄家本局得分
	__int64				m_i64MeScore;			//玩家本局的得分
	__int64				m_i64MeFanFen;			//本局自己的返还分数

	int					m_iPlayCount;			//玩了多少盘
	int					m_iWinCount;			//赢了多少盘

	BYTE				m_byNowCard[2][3];		///<当前数据//庄闲的牌，0为闲，1为庄
	int					m_iZCardInfo[5];		///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	int					m_iXCardInfo[5];		///<闲的牌信息：庄家的牌数，闲家的牌数，
	int					m_iWinQuYu[BET_ARES];	///<游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和,8庄单，9庄双，10闲单，11闲双
	int					m_iWinShowIndex;		//中将区域闪烁标识


	BYTE				m_bySelChouMaType;			//当前选择的筹码类型
	BYTE				m_bySelBetAres;				//选择的下注筹码区域
	CRgn				m_rgnNoteRect[BET_ARES];	///12个不规则的下注区域

	int					m_iLuziIndex;			//路子下标
	int					m_iWaitIndex;			//上庄列表下标

	bool				m_bIsSuper;

	

	
private:
	int					m_iSendCardBGPoint[2];	//发牌背景位置
 public:
    /// 初始化
    virtual		int Initial();
    /// 初始化UI
    virtual		int InitUI();
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
	void		HandleGameBegin(BEGIN_DATA *psGameBegin);
	//开牌
	void		OnHandleKaiPai(KAI_PAI *pKaipai);
	//收到下注消息
	void		OnHandleXiaZhu(XIA_ZHU *pXiaZhu);
	//游戏结算
	void		HandleGameResult(JIE_SUAN *pJieSuan);
	//空闲消息
	void		HandleGameFree(DENG_DAI *pDengDai);
	//申请上庄结果消息
	void		HandleApplyResult(SHANG_ZHUANG *pShangZhuang);
private:
	//申请上庄
	void		UserApplyNtStation();
	//申请下庄
	void		UserApplyLiveNt();
	//玩家下注
	void		UserBet();
	

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
	void		ShowMyInfo(bool bFalg);
	//显示庄家信息
	void		ShowNtInfo(bool bFalg);
	//显示可下注数信息
	void		ShowCanBetInfo(bool bFalg);
	//设置下注按钮可用
	void		SetBetBtEnable(bool bFalg);
	//启动显示计时器
	void		ShowOrHideTimer(bool bShow,BYTE byMaxTime, BYTE byMinTime = 0);
	//上庄列表
	void		ShowWaitList(bool bFalg);
	//显示中将区域
	void		ShowWinAres(bool bFalg);
	//更新显示路子
	void		ShowLuziPoint(bool bFlag);
	//显示路子区域
	void		ShowLuziAres(bool bFlag);

	//显示上庄列表
	void	ShowWaitNtInfo(bool bFlag);
	//显示游戏结算框
	void		ShowGameResult(bool bFlag) ; 


	//显示超端结果界面
	void		ShowSuperResult(bool bFlag);

private:
	//获取牌的点数
	int		GetCardNum(BYTE byCard);

private:
	//设置动画控件播放动画
	void	SetAnimatePlay(const int AnimateID,bool bFlag,BYTE byPlayNum = 1, bool bIsLoop = false);
	//设置动画控件播放指定动画
	void	SetAnimatePlay(const int AnimateID,wchar_t wsFilePath[],bool bFlag,BYTE byPlayNum = 1, bool bIsLoop = false);
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
	void	SetImageVisible(const int iImageID, bool bFlag,bool bMouseMove = false);
	//设置图片控件加载显示指定的文件
	void	SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag);
	//设置图片控件加载显示指定的文件
	void	SetImageXYInfo(const int iImageID,int iSrcX,bool bFlag);
	//设置容器控件可见
	void	SetContainVisible(const int iContainID,bool bFlag);
	//设置容器控件可见
	void	SetContainPosition(const int iContainID,int iX,int iY,bool bFlag);
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,__int64 iNum, bool bFlag,bool bSymbol=false);
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,TCHAR sChar[], bool bFlag);
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,CString sText, bool bFlag);
	//设置数字空间显示内容
	void	SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag,bool bSymbol=false);
	//设置筹码控件显示值
	void	SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);
	//设置单选框控件是否被选中
	void	SetRButtonSelected(const int iRButtonID,bool bSelected, bool bFlag);
	//设置移动控件播放
	void	SetMoveActionPlay(const int iMoveActionID,const int iControlID,POINT pStart, POINT pEnd,unsigned int uTime,bool bPlay);
	//设置不可操作牌控件数据
	void	SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount,POINT pPT, bool bShow);
	//设置不可操作牌控件数据
	void	SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount,bool bShow);
	//设置单选框
	void	SetRadioButtonSeled(const int iRadioID,bool bSeled);
	//获取单选框是否被选中
	bool	GetRadioBtIsSel(const int iRadioID);
	//获取单选框是否被选中
	void	SetRadioVisible(const int iRadioID,bool bShow);

public:
	//重置UI界面
	void	ReSetUiDlag();
	///初始化游戏中的变量
	void	InitialGameData();  
	//创建不规则下注区域
	void		CreateBetRgn();
	///初始化部分数据
	void	ResetGameData();
	///删除所有定时器
	void	KillAllGameTime() ;



	///玩家进入游戏
	void	SetGameStation(void * pBuffer,int nLen);
	//播放背景音效
	void	PlayBgSound(bool bISLoop);
	//播放游戏音效
	void	PlayGameSound(int SoundID, bool bISLoop = false);

	///出牌声音播放
	void  SetOutCardPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop) ;


};