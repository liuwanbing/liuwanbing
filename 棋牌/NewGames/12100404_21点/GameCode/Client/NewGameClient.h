#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include "CommonDefine.h"
#include<vector>
using namespace irrklang;

#define GetControlNode(A ,IControl , IControlID ) \
	IControl *A=  dynamic_cast<IControl *>(m_pUI->GetIControlbyID(IControlID)); \
	if (NULL != A) \
	A

const   int  GAME_WAIT_STATION    = 1 ;
const   int  GAME_BEGIN_STATION   = 2 ; 
const   int  GAME_FINISH_STATION  = 3 ; 

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_WAIT_NEXT				101					//下一局定时器
#define ID_OUT_CARD					102					//出牌等待时间
#define ID_LOOK_BACK				103					//看暗牌
#define ID_CHANGE_MUSIC			    104					//换背景音乐
#define ID_NOTE                     105                 //等待闲家下注
#define ID_SHOW_CARD                106                 //等待闲家出牌

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
#define SOUND_ADDDOUBLE			      0x0A			//加倍
#define SOUND_XIAZHU			      0x0B			//下注
#define SOUND_GIVEUP			      0x0C			//放弃
#define SOUND_SENDCARD			      0x0D			//发牌
#define SOUND_CLICKCARD			      0x0E			//点击牌				
#define SOUND_HIT_BEGIN				  0x0F			//点击开始按钮
#define SOUND_GAME_RUN				  0x1F			//游戏进行中
#define SOUND_HIT_LEAVE				  0x2F			//离开


#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\

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

//成员变量

private:
	IGameFrame							*m_pGameFrame;									///框架游戏指针

	GameInfoStructInExe					*m_pGameInfo;									//游戏信息指针
	
	IUserInterface						*m_pUI;											///界面游戏指针

	ISoundEngine						*m_SoundEngineGame;								//游戏音效

	ISoundEngine						*m_SoundEnginBg ;								//背景音效
		
	CUpGradeGameLogic					m_Logic;										//客户端逻辑

public:
	//固定信息
	BYTE					m_byGameStation ;										//当前的游戏状态
	int						m_iMyDeskStation ;										//玩家自己的游戏位子
	BYTE					m_bGameBeginWaitTime;									//游戏开始等待时间
	BYTE					m_bGameBetTime;											//游戏下注时间
	BYTE					m_bCallCardTime;										//游戏叫牌时间

	//运行信息
	bool					m_IsNote;												//是否已下注
	bool					m_IsJoin;												//是否中途加入
	BYTE					m_bNtPeople;											//当局庄家
	BYTE					m_bScorePeople;											//当前叫分者
	int						m_iClockLabel;											//当前时钟标签
	int						m_iCurrentID;											//当前时钟ID
	int						m_iNoteLabel;											//注子标签
	int						m_iSendCardLabel;										//发牌标签
	__int64					m_i64CurrentNote;										//当前选择的下注
	__int64					m_i64UserBet[PLAY_COUNT];								//用户的下注
	__int64					m_i64UserCanBet[3];										//用户下注筹码的值
	__int64					mi64UserMaxBet;											//用户最大下注数
	BYTE					m_iUserCard[PLAY_COUNT][2];								//用户开始的二张扑克
	BYTE					m_iUserAlreadySend[PLAY_COUNT][13];						//用户已经发的扑克
	BYTE					m_iUserAlreadySendCount[PLAY_COUNT];					//用户发到第几张牌
	vector<BYTE>			m_vCardSqueue;											//每张牌的发牌顺序数组
	vector<int>				m_vSendSqueue;											//每个人的发牌顺序
	BOOL					m_bIsSuperUser;											//是否超级用户

	//游戏设置
	bool					m_iGameSet[3];											//游戏设置单选框
	//重载
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

	//控件使用函数
private:
	//设置按钮是否可用
	void	SetBtEnable(const int iButtonID, bool bFlag);

	//设置按钮是否可见
	void	SetBtVisible(const int iButtonID, bool bFlag);

	//设置按钮控件加载制定图片
	void SetBtLoadSrc(const int iButtonID,CString sImagSrc,bool bFlag);

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

	//设置数字控件显示内容
	void SetInumShow(const int,__int64,bool);

	//设置筹码控件显示值
	void	SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);

	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iButtonID, CString sImagSrc,bool bFlag,int x,int y,int w,int h);

	//设置单选框控件是否被选中
	void	SetRButtonSelected(const int iRButtonID,bool bSelected);

	//获取单选框控件是否被选中
	BOOL	GetRButtonSelected(const int iRButtonID,bool *bSelected);

	//设置控件是否显示
	void SetControlVisible(const int iID,bool bFlag);
//初始化函数
private:

	///初始化游戏中的变量
	void	InitialGameData();  

	//重置UI界面
	void	ResetGameDialog();

	//重置游戏数据
	void ResetGameData();

	//功能函数
public:

	///删除所有定时器
	void	KillAllGameTime() ;

	//播放背景音效
	void	PlayBgSound(bool bISLoop);

	///玩家进入游戏
	void	SetGameStation(void * pBuffer,int nLen);

	//播放游戏音效
	void	PlayGameSound(int SoundID, bool bISLoop = false);

	//检查版本号
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

	//时间警示音
	void TimeWarning(int iTime); 

	//展示六个操作按钮
	void DisplayButton();

	//播放下注动画
	void ShowBetAction(const int ACTION_ID,const int CONTROL_ID,const int START_ID,const int END_ID,int second);

	//播放动画
	void ShowAction(const int ACTION_ID,const int CONTROL_ID,const int START_ID,const int END_ID,int second);
	
	//点击服务函数
	void OnButton(const int iBtType,int iParam);

	//动画服务函数
	void OnAction(int iParam);

	//可操作牌发牌
	void OperateSendCardList(const int id,BYTE bCardList[]);

	//不可操作牌发牌
	void NoOperateSendCardList(const int id,BYTE bCardList[]);

	//处理服务器同意游戏消息
	void OnHandleAgreeGame(void *buffer);

	//处理服务器发过来的确定庄家消息
	void OnHandleNt(void *buffer);

	//处理服务器发过来的下注消息
	void OnHandleBet(void *buffer);

	//处理服务器发过来的发牌消息
	void OnHandleSendCard(void *buffer);

	//处理服务器发过来的开始游戏消息
	void OnHandlePlay(void *buffer);

	//处理服务器发过来的游戏安全结束消息
	void OnHandleSafeFinish(void *buffer);

	//处理服务器发过来的下注消息
	void OnHandleBetResult(void *buffer);

	//处理服务器发过来的叫牌消息
	void OnHandleCall(void *buffer);

	//处理服务器发过来的游戏正常结束消息
	void OnHandleNormalFinish(void *buffer);

	//处理服务器发过来的超端用户信息
	void HandleSuperMessage(void *buffer);

	//判断点数
	int GetPoints(BYTE bCardList[],int iSize);

	
	//没有下一个叫分者游戏结束
	void GameEnd();

	//清空超端用户界面
	void	ClearShowSuperResult();

	//显示超端结果界面
	void	ShowSuperResult(bool bFlag);

	//根据筹码值得到图片下标
	int   GetBetIndex(int iValue);

	//下注
	void SetNote(__int64 i64Bet,int iView);
};