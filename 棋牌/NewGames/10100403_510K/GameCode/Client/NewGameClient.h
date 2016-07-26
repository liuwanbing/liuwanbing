#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include "CommonDefine.h"
#include<deque>
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
#define ID_SCORE_TIME				101					//叫分定时器
#define ID_TIP_NORULE				102					//规则不符合提示定时器
#define ID_SEND_CARD				103					//发牌定时器

//游戏音效
#define SOUND_BEGIN_GAME			  1				//游戏开始
#define SOUND_TIME_0				  2				//时间提示音
#define SOUND_TIME_1			      3
#define SOUND_TIME_2			      4
#define SOUND_TIME_3			      5
#define SOUND_TIME_4			      6
#define SOUND_TIME_5			      7
#define SOUND_WIN				      8				//胜利
#define SOUND_LOSE				      9				//失败
#define SOUND_CHONGPAI			      10			//不出
#define SOUND_SENDCARD			      11			//发牌
#define SOUND_TIP			          12			//提示
#define SOUND_TANPAI			      13			//开牌
#define SOUND_CLICKCARD			      14			//点击牌				
#define SOUND_HIT_BEGIN				  15			//点击开始按钮
#define SOUND_GAME_RUN				  16			//游戏进行中
#define SOUND_HIT_LEAVE				  17			//离开	
#define SOUND_HIT_BOMB				  18			//炸弹	
#define SOUND_HIT_DANI				  19			//大你	
#define SOUND_HIT_BUYAO				  20			//不要
#define SOUND_HIT_FEIJI				  21			//飞机	
#define SOUND_HIT_LIANDUI			  22			//连对	
#define SOUND_HIT_SHUNZI			  23			//顺子
#define SOUND_HIT_SIDAIER			  24			//四带二	
#define SOUND_HIT_SIDAILIANGDUI		  25			//四带两对
#define SOUND_HIT_SANDAIYI			  26			//三带一	
#define SOUND_HIT_SANDAIDUI		      27			//三带对
#define SOUND_HIT_SANGE		          28			//三个
//单张
#define SOUND_HIT_SIGNAL_2		      190	
#define SOUND_HIT_SIGNAL_3		      191
#define SOUND_HIT_SIGNAL_4		      192
#define SOUND_HIT_SIGNAL_5		      193
#define SOUND_HIT_SIGNAL_6		      194
#define SOUND_HIT_SIGNAL_7		      195
#define SOUND_HIT_SIGNAL_8		      196
#define SOUND_HIT_SIGNAL_9		      197
#define SOUND_HIT_SIGNAL_10		      198
#define SOUND_HIT_SIGNAL_11		      199
#define SOUND_HIT_SIGNAL_12		      200
#define SOUND_HIT_SIGNAL_13		      201
#define SOUND_HIT_SIGNAL_14		      202
#define SOUND_HIT_SIGNAL_15		      203
#define SOUND_HIT_SIGNAL_16		      204
//对子
#define SOUND_HIT_DOUBLE_2		      300			
#define SOUND_HIT_DOUBLE_3		      301
#define SOUND_HIT_DOUBLE_4		      302
#define SOUND_HIT_DOUBLE_5		      303
#define SOUND_HIT_DOUBLE_6		      304
#define SOUND_HIT_DOUBLE_7		      305
#define SOUND_HIT_DOUBLE_8		      306
#define SOUND_HIT_DOUBLE_9		      307
#define SOUND_HIT_DOUBLE_10		      308
#define SOUND_HIT_DOUBLE_11		      309
#define SOUND_HIT_DOUBLE_12		      310
#define SOUND_HIT_DOUBLE_13		      311
#define SOUND_HIT_DOUBLE_14		      312

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


private:
	
	IGameFrame							*m_pGameFrame;									//框架游戏指针
	GameInfoStructInExe					*m_pGameInfo;									//游戏信息指针
	IUserInterface						*m_pUI;											//界面游戏指针
	ISoundEngine						*m_SoundEngineGame;								//游戏音效
	ISoundEngine						*m_SoundEnginBg ;								//背景音效
	CUpGradeGameLogic					m_Logic;										//客户端逻辑
	//固定信息
	BYTE								m_iVersion;
	BYTE								m_iVersion2;
	BYTE                                m_iMyDeskStation;                               //自己位置
	BYTE								m_iThinkTime;									//游戏思考时间
	BYTE								m_iBeginTime;									//游戏开始时间
	//运行信息
	int								    m_bUserPrepare[PLAY_COUNT];						//玩家准备
	BYTE								m_byGameStation;                                //游戏运行信息
	BYTE					            m_bNtPeople;									//庄家
	BYTE								m_bNtViewStation;								//庄家的视图位置
	BYTE								m_DeskCard[PLAY_COUNT][CARD_NUMS];			    //每人桌面的真实扑克
	BYTE								m_bOwnCard[CARD_NUMS];							//自己的扑克
	BYTE								m_iDeskCardCount[PLAY_COUNT];					//每人桌面扑克的数目
	BYTE								m_bHaveCard[12];								//当轮有分牌
	int									m_bCurrentClockLabel;							//当前时钟标签
	int									m_iCurrentClockControlID;						//当前控制的时钟ID
	int									m_iCurrentContainControlID;						//当前控制的时钟容器ID
	BYTE								m_bLeftCard[CARD_NUMS];							//上家出的牌
	BYTE								m_bCurrentCall;									//当前出牌玩家
	bool								m_bIsOutCard;									//时间到后是否自己一定要出牌
	bool								m_bIsAuto;										//是否托管
	//发牌相关
	deque<BYTE>							m_sendQueue;
	//坐标位置
	POINT								m_timeXY[5];
	//提示按钮相关
	bool								m_bIsHaveBig;									//是否有比上家大的牌
	BYTE								m_bCompareCard[CARD_NUMS];						//要比的牌
	//游戏设置相关
	bool								m_bIsPlayAction;								//是否播放动画
	bool								m_bIsPlaySound;									//是否播放游戏音效
	bool								m_bIsWatching;									//是否允许旁观
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
private:
	//设置按钮是否可用
	void SetBtEnable(const int iButtonID, bool bFlag);
	//设置按钮是否可见
	void SetBtVisible(const int iButtonID, bool bFlag);
	//设置时间框显示倒计时
	void SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime = 0);
	//设置图片控件的位置
	void SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag);
	//设置图片控件是否可见
	void SetImageVisible(const int iImageID, bool bFlag);
	//设置图片控件加载显示指定的文件
	void SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag);
	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iButtonID,CString sImagSrc, bool bFlag,int x,int y,int w,int h);
	//获得图片控件是否显示
	BOOL GetImageIsVisible(const int iImageID);
	//设置容器控件可见
	void SetContainVisible(const int iContainID,bool bFlag);
	//获得容器控件可见
	bool GetContainVisible(const int iContainID);
	//设置容器控件不响应任何鼠标时间
	void SetContainEnable(const int iContainID,bool bFlag);
	//设置文本控件显示内容
	void SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);
	//设置文本控件显示文字信息
	void SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag);
	//设置数字空间显示内容
	void SetInumInfo(const int , __int64 ,bool );
	//设置数字空间显示内容
	void SetInumPro(const int iNumID, __int64 i64Num,BYTE bPro,bool bFlag);
	//设置数字空间显示内容(不带加号和减号及逗号)
	void SetInumInfo_1(const int iNumID, __int64 i64Num, bool bFlag);
	//设置筹码控件显示值
	void SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);
	//设置控件位置
	void SetControlPos(const int iControlID,bool bFlag,int x,int y);
	//设置单选框控件是否被选中
	void SetRButtonSelected(const int iRButtonID,bool bSelected);
	//获取单选框控件是否被选中
	BOOL GetRButtonSelected(const int iRButtonID,bool *bSelected);
	//发自己的牌
	void ShowCurrentCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//发别人的牌
	void ShowCurrentNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//展示摊牌玩家的牌
	void ShowOpenCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//根据提示升起牌
	void ShowUpCard(const int iControlID1,BYTE bUpCardList[],BYTE bUpCardCount);
	//获取升起的牌
	void GetUpCard(const int iControlID,BYTE bCardList[]);
	//获取升起的牌的数量
	int GetUpCardNum(const int iControlID);
	//设置可操作牌是否可见
	void SetOperateVisible(const int iControlID,bool bFlag);
	//设置不可操作牌是否可见
	void SetNoOperateVisible(const int iControlID,bool bFlag);
	//播放发牌动画
	void ShowSendCardAction(const int ACTION_ID,const int END_ID);
	//给用户发牌
	void SendCard(BYTE bCardValue,BYTE bSendPeople);
	//播放外部动画
	void PlayAnimate(const int iID,int x,int y, bool bFlag);
	//数据处理
	//CString转TCHAR
	TCHAR* CStringToCHAR(CString &str);
	//删除制定的牌
	void RemoveCard(BYTE bCardList[],BYTE bTarCard,int iCount);
	//重排牌数组
	void RestartSort(BYTE bCardList[],int iCount);
public:
	///重置数据
	void ResetGameData();
	//播放游戏音效
	void PlayGameSound(int SoundID,bool IsBoy=true,bool bISLoop = false);
	//播放背景音效
	void PlayBgSound(bool bISLoop);
	//重置UI界面
	void ResetGameDialog();
	///初始化游戏中的变量
	void InitialGameData();  
	///删除所有定时器
	void KillAllGameTime() ;
	///玩家进入游戏
	void SetGameStation(void * pBuffer,int nLen);
	//检查版本号
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);
	//时间警示音
	void TimeWarning(int iTime); 
	//点击服务函数
	void OnButton(const int iBtType,int iParam,bool bFlag=true);
	//第一个进入服务器的玩家执行此函数
	void HandleStartStation(void *buffer);
	//游戏开始状态
	void HandleGameRuningStation(void *buffer);
	//各种动画
	//计算当前牌的拥有者
	BYTE GetSendStation(BYTE (*BCardList)[5],BYTE bCardValue,int iCardCount);
	//处理同意游戏消息
	void OnAgreeGame(void *buffer);
	//处理发牌
	void OnHandleSendCardBegin(void *buffer);
	//处理游戏开始
	void OnHandleGameBegin(void *buffer);
	//处理服务器发过来的叫牌消息
	void OnHandleCall(void *buffer);
	//显示得分牌
	void OnHandleShowStyle(void *buffer);
	//最后一名摊牌
	void OnHandleLastShowCard(void *buffer);
	//处理服务器发过来的游戏正常结束消息
	void OnHandleNormalFinish(void *buffer);
	//处理服务器发过来的游戏托管消息
	void OnHandleAuto(void *buffer);
	//根据视图位置启动相应定时器
	void StartTime(BYTE bViewStation,int iClockLabel);
	//根据视图位置贴相应510k图片
	void Paste510kPhoto(BYTE bViewStation,BYTE bCardData);
	//查看牌分更新
	void UpdateSeeStyle(BYTE bCardData,int iView);
	//播放牌型提示音
	void PlayCardTip(BYTE bDeskStation,BYTE bCardList[],int iCount);
};