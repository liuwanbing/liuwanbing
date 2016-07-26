#include "IGameImpl.h"
#include "../GameMessage/UpGradeLogic.h"
#include <irrKlang.h>
#include "CommonDefine.h"
#include<vector>
using namespace irrklang;
#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\
//客服端时钟
//定时器
#define TIMER_SELECTGAMEMODE		  200	//选择游戏模式定时器
#define TIMER_SHOWGAMEMODEINFO		  201	//显示游戏模式信息定时器
#define TIME_BEGIN                    101			//等待
#define TIMER_THINK					  102			//思考
#define TIME_TIP_NOSTYLE			  103			//无牌型
#define TIMER_BET					  104			//投注定时器
#define TIMER_NOTIFYMODE			  105			//模式通知定时器
#define TIMER_SHOWSETTLE			  106			//结算容器显示时间
#define TIMER_BETBANKER				  107			//抢庄定时器
//游戏音效
#define SOUND_BUTTON				  0x01			//按钮
//点击消息
#define CLICK_START                 500				//开始
#define CLICK_CARD                  501				//点击牌
#define CLICK_HAND                  502			    //操作
#define CLICK_AUTO                  503				//托管
#define CLICK_SUPER					505				//超端用户
#define CLICK_SET					506				//游戏设置
//动画坐标
POINT	m_AnimateXY[3]={/*洗牌坐标*/{160,220},/*打枪坐标*/{5,28},/*开始比牌*/{400,315}};
//牌型动画
POINT	m_StyleAnimateXY[PLAY_COUNT][3]={
	{{350,142},{350,217},{350,287}},
	{{660,142},{660,217},{660,287}},
	{{215,462},{435,462},{680,462}},
	{{0,143},{0,217},{0,287}}
};
POINT m_SendStartXY[PLAY_COUNT]={{450,280},{450,280},{450,280},{450,280}};//发牌起点坐标
POINT m_SendEndXY[PLAY_COUNT]={{418,105},{744,105},{418,638},{62,105}};//发牌终点坐标

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
	IUserInterface						*m_pUI;											//界面游戏指针
	ISoundEngine						*m_SoundEngineGame;								//游戏音效
	ISoundEngine						*m_SoundEnginBg ;								//背景音效	
	CUpGradeGameLogic					m_Logic;										//客户端逻辑

	bool								m_isSuperDialogShow;							//超端界面是否显示
	BOOL								m_isWinOrLose;									//庄家输赢

public:
	int						m_iMyDeskStation ;										//玩家自己的游戏位子
	ENUM_GameStation		m_byGameStation;										//游戏状态
	BYTE				    m_iThinkTime;											//游戏思考时间
	BYTE				    m_iBeginTime;											//游戏开始时间
	bool					m_bUserPrepare[PLAY_COUNT];						        //玩家准备
	BYTE					m_byOwnCards[5];										//玩家的牌

	short					m_ClockTickCount;										//时钟计时器

	BYTE					m_byFirstHeap[3];										//第一堆牌
	BYTE					m_bySecondHeap[2];										//第二堆牌

	__int64					m_iBetMoney[4];											//四个下注额度

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
public:
	//设置控件是否可见
	void SetControlVisible(const int iControlID,bool bFlag);
	//设置按钮是否可用
	void SetBtEnable(const int iButtonID, bool bFlag);
	//设置时间框显示倒计时
	void SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime = 0);
	//设置图片控件的位置
	void SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag);
	//设置图片控件加载显示指定的文件
	void SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag);
	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iButtonID,CString sImagSrc, bool bFlag,int x,int y,int w,int h);
	//设置容器控件不响应任何鼠标时间
	void SetContainEnable(const int iContainID,bool bFlag);
	//设置文本控件显示内容
	void SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);
	//设置文本控件显示文字信息
	void SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag);
	//设置数字控件显示内容
	void SetInumInfo(const int , __int64 ,bool );
	//设置新形式数字控件显示内容（带加号、减号、逗号）
	void SetNewInumInfo(const int iNumID, __int64 i64Num,bool bFlag);
	//设置筹码控件显示值
	void SetShCtrlInfo(const int iShCtrlID,int iNum, bool bFlag);
	//设置控件位置
	void SetControlPos(const int iControlID,int x,int y);
	//设置单选框控件是否被选中
	void SetRButtonSelected(const int iRButtonID,bool bSelected);
	//获取单选框控件是否被选中
	BOOL GetRButtonSelected(const int iRButtonID,bool *bSelected);
	//可操作牌发牌
	void ShowOperateCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//不可操作牌发牌
	void ShowNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//获得不可操作牌发牌是否显示
	BOOL GetNoMoveIsVisible(const int iControlID);
	//展示摊牌玩家的牌
	void ShowOpenCard(const int iControlID,BYTE bCardList[],BYTE cardCount);
	//根据提示升起牌
	void ShowUpCard(const int iControlID1,BYTE bUpCardList[],BYTE bUpCardCount);
	//获取升起的牌
	void GetUpCard(const int iControlID,BYTE bCardList[]);
	//获取升起的牌的数量
	int GetUpNums(const int iControlID);
	//播放发牌动画
	void ShowSendCardAction(const int ACTION_ID,const int END_ID);
	//给用户发牌
	void SendCard(BYTE bCardValue,BYTE bSendPeople);
	//播放外部动画
	void PlayIExAnimate(const int iID,int x,int y, bool bFlag);
	//关闭外部动画
	void CloseAnimate(const int iID);
	//数据处理
	//CString转TCHAR
	TCHAR* CStringToTCHAR(CString &str);
	//克隆图片
	void CloneImage(const int iID, int iGoalID);
	//播放move动画
	void PlayIMoveAction(const int ACTION_ID,const int CONTROL_ID,POINT startXY,POINT endXY,int second);
	//制定控件是否显示
	bool GetControlShow(const int iID);
	//设定控件的x,y,w,h
	void SetControlPosition(const int iID,int x,int y,int w,int h);

	/////////////////////
	//游戏模式选择，确定按钮
	void OnBtnSelectGameModeOK();
	// 游戏模式选择成功
	void GameModeSelectedMessage(BYTE *pData);
	//退出了游戏模式选择
	void ExitWithoutSelectMode();
	//将分出的牌发送给服务器
	void SendCardToServer();
	//下注
	void Betting(int iButtonID);

	//恢复游戏状态
	void GameOverReset();

	void SetWinOrLose(int iControlID,BOOL IsWin);


//功能函数
private:
	///初始化游戏中的变量
	void InitialGameData();  
	///玩家进入游戏
	void SetGameStation(void * pBuffer,int nLen);
	//重置UI界面
	void ResetGameDialog();
	//重置游戏数据
	void ResetGameData();
	//播放背景音效
	void PlayBgSound(bool bISLoop);
	//播放游戏音效
	void PlayGameSound(int SoundID, bool bISLoop = false);
	//接受发牌消息服务函数
	void OnReveiveSendCard(void *pBuffer);

	//显示出牌结果
	void ShowCardsResult(void *pBuffer);

	//下注界面显示
	void ShowBet();

	//显示亮牌玩家的牌
	void ShowCards(BYTE* pData);

	//显示牌型字体
	void ShowCardShapeInfo(EnCardShape enCardShape,int ControlID);

	//结算
	void OnSettle(void *pData);

	//牌型字符串
	void GetCardShapeStr(EnCardShape enCardShape,WCHAR *buffer);

	//玩家角色
	void GetPlayerRole(bool bBanker,WCHAR* wbuffer);
	
};