#include "IGameImpl.h"
#include "..\GameMessage\UpGradeLogic.h"
#include "..\GameInterface\MUI.h"
#include <irrKlang.h>

//定时器 ID
#define ID_BEGIN_TIME				    100					//开始定时器
#define ID_END_TIME						101					//结束定时器
#define ID_TIME_SEND_CARD				102					//发牌计时器
#define ID_TIME_THINK                   103                 //玩家思考计时器
#define ID_TIME_BASENOTE				104					//每个玩家依次下底注计时器
#define ID_TIME_SEE_CARD				105					//看牌计时器
#define ID_TIME_TIP						106					//提示计时器
#define ID_TIME_AUTOSTART				107					//自动开始计时器

//声音类型设置
#define SOUND_OUT_CARD				1				//出牌声音
#define SOUND_WARNING				2				//警告
#define SOUND_SEND_CARD				3				//发牌声音
#define SOUND_START_DEF				4				//四个人开始游戏后

#define SOUND_WIN					5				//胜利
#define SOUND_LOSE					6				//输
#define SOUND_WAITING				7				//等待
#define SOUND_USER_RUNAWARY			8				//有人逃跑
#define SOUND_USER_LEFT				9				//有人离开
#define SOUND_MOVEBUTTON            10              //移动到按钮

//按注类型(梭哈备用)
#define SOUND_GIVE_UP				11				//放弃
#define SOUND_NOTE					12				//下注
#define SOUND_ADD					13				//加注
#define SOUND_FOLLOW				14				//跟注
#define SOUND_SHUTTLE				15				//梭哈

using namespace irrklang;

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
	/// 游戏重置
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
	//游戏退出
	virtual bool OnGameQuiting();

public:                       ///游戏自己的操作

	//消息处理函数------------------------
	//用户强行离开
	void OnHandleASS_CUT_END(BYTE * buffer,int nLen);
	//玩家准备消息
	void OnHandleGM_AGREE_GAME(BYTE * buffer,int nLen);
	//52游戏开始
	void OnHandleGM_ASS_GAME_BEGIN(BYTE * buffer,int nLen);
	//53发牌信息
	void OnHandleGM_ASS_SEND_CARD(BYTE * buffer,int nLen);
	//54发牌完成
	void OnHandleGM_ASS_SEND_FINISH(BYTE * buffer,int nLen);
	//55开始游戏
	void OnHandleGM_ASS_GAME_PLAY(BYTE * buffer,int nLen);
	//60下注
	void OnHandleGM_ASS_NOTE(BYTE * buffer,int nLen);
	//63下注结果
	void OnHandleGM_ASS_NOTE_RESULT(BYTE * buffer,int nLen);
	//70收集筹码
	void OnHandleGM_ASS_COLLECT_JETON(BYTE * buffer,int nLen);
	//71机器人托管
	void OnHandleGM_ASS_AI_STATION(BYTE * buffer,int nLen);
	//81新发一张牌
	void OnHandleGM_ASS_SEND_A_CARD(BYTE * buffer,int nLen);
	//90游戏结束
	void OnHandleGM_ASS_CONTINUE_END(BYTE * buffer,int nLen);
	//93游戏安全结束
	void OnHandleGM_ASS_SALE_END(BYTE * buffer,int nLen);
	//94游戏结束无胜出者
	void OnHandleGM_ASS_NO_WINER(BYTE * buffer,int nLen);

	//状态处理函数------------------------
	void SetGameStat_GameStation_Base(void * pBuffer,int nLen);
	void SetGameStat_GameStation_1(void * pBuffer,int nLen);
	void SetGameStat_GameStation_2(void * pBuffer,int nLen);
	void SetGameStat_GameStation_3(void * pBuffer,int nLen);
	void SetGameStat_GameStation_4(void * pBuffer,int nLen);
	void SetGameStat_GameStation_5(void * pBuffer,int nLen);

	//UI消息响应函数--------------------
	//动画播放完成消息
	void OnUIMess_CATOONFINISH(UINT32 uControlID);
	//鼠标左键弹起消息
	void OnUIMess_LBUTTONUP(UINT32 uControlID);
	//鼠标移入/移出控件消息
	void OnUIMess_MOUSEENTER(UINT32 uControlID);
	
	/// 用户处理结果
	void OnHitActiveResult(BYTE btag, __int64 iNote);
	/// 用户下注消息
	void OnSelectNote(__int64 i64Num);	
	//同意开始
	bool OnControlHitBegin();

	//UI显示函数------------------------
	//设置房间信息
	void ShowRoomInfo();
	//显示准备的玩家
	void ShowAgree(BYTE bDeskStation);
	//游戏开始时初始化界面
	void ShowBegin();
	//显示总注
	void ShowTotalNote(bool bFlag);
	//显示玩家下的注
	void ShowPlayerNote(BYTE bDeskStation, bool bFlag);
	//显示玩家手牌
	void ShowHandCard(BYTE bDeskStation, BYTE iCard[], int iCount, bool bFlag);
	//玩家下注动画
	void ShowNoteAnima(BYTE bDeskStation, __int64 iNote, int iTime = 0);
	//显示最大牌玩家标记
	void ShowCardMaxMark(bool bFlag);
	//显示下注按钮上的数目
	void ShowNoteButText(bool bFlag);
	//显示各种下注按钮
	void ShowNoteBtnEnable(bool bFollow = false,bool bAllNote = false,bool bNote = false,bool bGiveUp = false);
	/// 显示 结算框
	void ShowResult(GameFinishNotify szResultInfo, bool bFlag);
	//显示设置框
	void ShowInitSet(bool bFlag);
	//显示弃牌
	void ShowGiveUp(BYTE bDeskStation);

	//游戏功能函数----------------------
	///加载配制文件
	void LoadData(void);
	//获得玩家视图位置
	int ViewStation(int desk);
	//是否存在该玩家
	bool IsPlayer(BYTE bDeskStation);
	//游戏UI重配
	void InitGameUI();
	//游戏逻辑数据重配
	void InitGameData();
	/// FlashWindowEx封闭）
	void FlashWindow();
	/// 获取游戏状态
	BYTE GetStationParameter(void);

	/// 设定游戏定时器(专门用来显示思考时间的，与UI有关)
	void SetGameTimer(BYTE bDesk);
	/// 消除定时器(专门用来显示思考时间的，与UI有关)
	void KillGameTimer(BYTE bDesk);
	//杀死所有计时器
	void KillAllTimer();

	//设置总注
	void SetTotalNote(__int64 iNoteTotal);
	//总注累加
	void AddTotalNote(__int64 iNoteTotal);
	//设置下注按钮上的数目
	void SetNoteButText(__int64 iBaseNote);


	/// 代替断线玩家下注
	BOOL ReplaceUserNote(BYTE bDeskStation);
	/// colin090414得到最大值
	__int64 GetMaxNote(__int64 a[4]);

	//声音函数---------------------------------
	//非循环播放声音函数
	void PlaySound(BYTE bDeskStation, BYTE bStyle);
	//非循环播放声音函数(无须判断玩家是男女)
	void PlaySound(BYTE bStyle);
	//随机背景音乐
	void PlaySceneSound();
	
private:
    IGameFrame                *m_pGameFrame;

	CUpGradeGameLogic		  m_Logic;					//逻辑规则

	ISoundEngine              *m_SoundEngine;

	IUserInterface *		 m_pIUI;
	MUI			              m_MyUI;

	BYTE                      m_bGameStation;               ///<游戏状态

	//变量定义
private:
	bool                      m_bNewTurn;
	int						  m_bMyDesk;						///自己的逻辑ID

	BYTE					m_iThinkTime;					/**< 最少游戏盘数 */
	BYTE					m_iBeginTime;					/**< 游戏开始时间 */
	int						m_iBeenPlayGame;				/**< 游戏局数 */
	bool					m_bIsTTProject;				///是否是TT项目 TT项目要求做一些修改

	int						m_iRoomBasePoint;				/**< 房间倍数 */
	int						m_iRunPublish;					/**< 逃跑扣分 */
	__int64					m_iLimitNote;					/**< 最大限额注 */
	__int64					m_iBaseNote;					/**< 底注 */
	__int64					m_iNoteTotalPoint;				//总注
	__int64					m_iThisTurnLimit;				/**< 本轮限注 */
	//运行信息
	BYTE					m_iDeskCardCount[PLAY_COUNT];			/**< 每人桌面扑克的数目 */
	BYTE					m_DeskCard[PLAY_COUNT][5];				/**< 每人桌面的扑克 */
	BYTE					m_UserCard[PLAY_COUNT][5];				/**< 用户扑克 */
	//运行信息
	int						m_iNowOutPeople;				/**< 现在出牌者 */
	BYTE					m_iFirstOutPeople;				/**< 第一个出牌者 */
	__int64					m_iNowBigNote;					/**< 当前最大押注数 */
	__int64					m_iThisGameNote[PLAY_COUNT];				/**< 本局游戏下注数 */
	__int64					m_iThisTurnNote[PLAY_COUNT];				/**< 本轮下注 */
	BYTE					m_iThisNoteTimes;				/**< 押注轮数 */
	BYTE					m_bTimeOutCount;				/**< 超时 */
	BYTE					m_bShowAnimalStation;			/**< 动画显示位置 */
	int						m_iUserStation[PLAY_COUNT];		/**< 记录用户状态 */
	TCHAR					m_szName[PLAY_COUNT][61];		/**< 玩家昵称 */
	int						m_iDoublePointTime[PLAY_COUNT];	/**< 双倍时间 */
	int						m_iiProtectTime[PLAY_COUNT];	/**< 护身符时间 */

	__int64                 m_TempNote[PLAY_COUNT];        /**< colin090418玩家单局下注数 */
	__int64                 m_iTempThisTurnNote[PLAY_COUNT];
	bool					m_bUserState[PLAY_COUNT];		/**<玩家状态(增加游戏开始后玩家亦可进入功能时添加的数据成员)*/	
	__int64					m_iPerJuTotalNote[PLAY_COUNT];	/**< 用户每局压总注 解决收集动画后显示下注数与实际下注数不符的问题 xyh */

	__int64					m_iAddButtonNum[4];		//加注按钮值
	__int64					m_iSelAddMoney;			//拖动下注值

	int						m_Time_BASENOTE_Count;			//下底注动画用计时器

	int						m_iFirstSendCardCount;			/**< 第一次发的牌张数 */
	__int64					m_i64HalfLessMoney;	
	//控件变量
private:	
	int						m_iVersion;
	int						m_iVersion2;

	//背景音乐
	BOOL                    m_bSound;                        ///是否开启音效
	HWND					m_hMciWnd1;						/**< 背景音乐(出牌) */
	HWND					m_hMciWnd2;						/**< 背景音乐(场景) */
	BYTE					m_byUserFollow;					/**< 第几把必跟 */
	BYTE                    m_byCurStyle;           	    /**<  当前下注类型 */
	bool					m_bCanHitLock;					/**< colin能否点击锁 */

	BOOL					m_bNetCutPlayer[PLAY_COUNT];

	BYTE                    m_iSendAnimaCardPos;     ///发牌动画位置
	int                     m_iSendCardTotle;         ///发牌的张数
	int                     m_iNeedSendCardTotle;	 ///当前需要发的牌数目
	int						m_iSendCardNum[PLAY_COUNT];//当前各玩家发的牌数目
	BYTE                    m_bShowHa;               ///第几张可以梭

	bool					m_bIsSuperUser;		//判断自己是否超级客户端：由服务器传来数据，每局都校验//lym 2011-11-28
	BYTE					m_byAddNum;												/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
	BYTE                    m_bPK;                ///是否PK

	BOOL                    m_bPlayer[PLAY_COUNT]; //是否玩家或中途进入观战者

	bool					m_bSendCard;//是否正在发牌

	bool					m_bIsWatching;//是否为旁观玩家

	CRgn					m_crMoveRect;		//移动下注的按钮区域
	bool					m_bCanMove;			//是否可以移动了
	POINT					m_ptMouse;			//鼠标的位置
	bool					m_bAutoStart;//是否可以自动开始
	__int64					m_iTempBt[4];
private:
	//设置鼠标响应
	void	SetButtonOnMoseMove(const int iID,bool bFlag);
	//设置鼠标响应
	void	SetImageOnMoseMove(const int iID,bool bFlag);
	//获取图片的位置
	POINT	GetImagePosition(const int iID);
	//设置图片位置
	void	SetImagePosition(const int iID,int iX, int iY);
	//设置鼠标穿透
	void	SetImageMoseThough(const int iID,bool bFlag);
	//设置图片加载宽度
	void	SetImageSrcX(const int iID,int isrcW);
	//设置控件是否可见
	void SetControlVisible(const int iControlID,bool bFlag);
	//设置文本控件显示文字信息
	void	SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag);
	//设置文本控件显示内容
	void	SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);
	//根据图片控件设置显示图片的尺寸
	void SetPicSize(const int iID,CString sImagSrc, bool bFlag,int x,int y,int w,int h);
	//设置数字控件显示内容
	void SetInumInfo(const int ,wchar_t *,__int64 ,bool );
	//设置单选框控件是否被选中
	void SetRButtonSelected(const int iID,bool bSelected);
	//获取单选框控件是否被选中
	BOOL GetRButtonSelected(const int iID,bool *bSelected);
};