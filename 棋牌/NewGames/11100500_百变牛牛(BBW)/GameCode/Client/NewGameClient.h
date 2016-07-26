#pragma once

#include "IGameImpl.h"
#include "IGameFrame.h"
#include "../GameMessage/UpGradeLogic.h"
#include "irrKlang.h"
using namespace irrklang;
interface IGameFrame;
interface IUserInterface;

#define	 SOUND_START						50					//游戏开始

#define  SOUND_HIT_CARD						52					//击牌
#define  SOUND_OUT_CARD						53					//出牌
#define	 SOUND_CLOCK						54					//闹钟
#define	 SOUND_ESCAPLE						55					//逃跑
#define	 SOUND_WIN							56					//赢
#define	 SOUND_LOSE							57					//输
#define	 SOUND_HE							58					//牛牛
//===========================================================================
#define  SOUND_NOTE							59					//下注声音
#define	 SOUND_SEND_CARD					60					//发牌
#define  SOUND_TONG_SHA						61					//通杀
#define  SOUND_TONG_PEI						62					//通赔	
#define  SOUND_GAME_OVER					63					//游戏结束
#define  SOUND_NOTE_BG						64					//背景声音
#define  SOUND_XIA_ZHU_BEING				65					//开始下注提示音效

#define SOUND_NOTE_10w       0x14                       ///下注背景声音
#define SOUND_NOTE_100w      0x15                      ///下注背景声音
#define SOUND_NOTE_1000w     0x16                       ///下注背景声音


//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_THINK_TIME				103					//思考定时器
#define ID_SCARD_TIME				104					//发牌定时器

#define ID_SUPER_TIME				106					//超端设置提示计时器
#define ID_SUPER2_TIME				107					//超端按钮提示计时器
#define ID_TESTUSER_TIME			108					//测试时间使用


#define ID_XIAZHU_TIME				10001			/// 下注定时器 
#define ID_KAIPAI_TIME				10002			/// 开牌定时器 


#define PROMPT_CHANGE_NT_TIME		10010			/// 提示换庄信息
#define SHOW_ZHAUNGLIST_TIME		10011			/// 显示庄列表信息定时器
#define SHOW_NOTIC_TEXT_TIME		10012			//超端提示文字


#define ID_SEND_CARD_TIMER          100				/// 发牌定时器ID
#define ID_PLAY_ANIMAL_TIMER		101				/// 搓牌
#define ID_SHOW_WIN_AREAS			102				/// 中将区域闪烁
#define ID_XIAZHU_TIME				103				//下注时间

#define ID_XIANPAI					20004			/// 发牌发闲家的牌
#define ID_ZHUANGPAI				20005			/// 发庄家的牌
#define ID_PLAY_SOUND				20006			/// 播放声音

#define ID_XIAZHUBUTTON_NUM			0x06			///有六个下注按钮
#define ID_XIAZHUAREA_NUM			0x03			///有三个下注区域(上门，天门，下门)

#define TYPE_DOWN			0x01		//分辨是未升起的牌
#define TYPE_UP				0x02		//分辨是升起的牌

struct SoundNews
{
	BYTE				byDeskStation;		//桌子位置
	BYTE				byCureentAction;	//当前操作
	BYTE				byCardShape;		//牌型
	int					iCardCount;			//牌张数
	BYTE				byCardPoint;		//牌值
	bool				bExitProess;		//退出线程

	SoundNews()
	{
		memset(this,NULL,sizeof(SoundNews));
	}
};

//游戏框架类 
class CNewGameClient : public IGameImpl
{
private:
	IGameFrame				*m_pGameFrame;						///游戏框架
	IUserInterface			*m_pUI;								///游戏UI
	CPokerGameLogic			m_Logic;							///逻辑
	UserInfoForGame			m_UserInfo[PLAY_COUNT];				/**<玩家信息*/
	ISoundEngine			*m_SoundEngine;						///音效指针
	ISoundEngine            *m_soundEngineBg ;					/// 背景音效	


private:
	int						m_iXiaZhuTime;											///下注时间				
	int						m_iKaiPaiTime;											///开牌时间       
	int						m_iFreeTime;											///空闲时间		
	BYTE					m_bGameStation;				///游戏状态		DWJ
	BYTE					m_byLgStation;				///玩家逻辑位置
	int						m_iNowNTstation;			///当前庄家的位置	DWJ
	int						m_iNTju;					///当前庄家进行的局数（绘制)	DWJ
	__int64					m_iNTmoney;					///当前庄家的输赢金币（记录）	DWJ
	__int64					m_iNTwin;					///当前庄家的成绩（赢的金币绘制）	DWJ
	bool					m_bNtApplyXZ;				//庄家是否申请了下庄	DWJ

	__int64					m_i64MyChengji;				///玩家成绩	DWJ

	int						m_iPlayCount;				///已经进行了几局数			DWJ
	int						m_iGameCount;				///总共进行了几把		DWJ

	BYTE					m_byMaxBase;					/// 最大倍率用来控制玩家下注	DWJ
	__int64					m_i64LimitMoney;				///上庄需要的最少金币DWJ
	__int64					m_i64MaxNote;					/// 每个玩家最多下注数DWJ

	__int64					m_i64RectNoteMax[MAX_NOTE_COUNT];	//四个区域下注限制(0-不限制)DWJ
	__int64					m_i64ChoumaValue[6];					///六个筹码值DWJ
	short int				m_idZhuangList[PLAY_COUNT+1][2];	///抢庄列表	DWJ
	int						m_iZhuangListInedx;					///庄的索引值	DWJ
	int						m_iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];	//记录闲家玩过把数的输赢(用于接收服务器发来的数)	DWJ
	int						m_iLuZiIndex;						//路子下标

private:	//每局游戏开始需要初始化的变量
	__int64				m_iMaxZhu;							///本把最大还可能下的注额
	__int64				m_iZhongZhu;		///本把当前总注额			 	
	__int64				m_iQuYuZhu[MAX_NOTE_COUNT];		///本把每个区域下的注额：	
	__int64				m_iQuYuZhuTrue[MAX_NOTE_COUNT];		///每个区域真实下注	
	__int64				m_iMyZhu[MAX_NOTE_COUNT+1];			///本把玩家自己在每个区域下的注额：
//-------------------------------------------------------------------------------------------

	bool					m_bWatchMode;						///是否为旁观者
	bool					m_bWatch;							///是否充许旁观
	bool					m_bSound;							//是否播放音效
	bool					m_bSoundPlay;						//是否播放音效


	BYTE					m_UserSendCard[5][5];				//玩家手中的牌,发牌时用的一个复本
	BYTE					m_CardSendCount[5];					//玩家手中牌的数目,发牌时用的一个复本
	int						m_iSendCardIndex;					//发牌标识

	BYTE					m_wBullCard[5][5];				//3张组成的牛牌
	BYTE					m_wResultCard[5][5];			//2最后的结果牌

	int						m_iWinQuYu[MAX_NOTE_COUNT];			///游戏的赢钱区域 
	bool					m_bShowRect;					///控制赢区域的闪烁
	

	int                     m_iChooseNoteType;					///筹码类型
	int                     m_iChooseNoteRect ;                 //选择的筹码区域
	BYTE					m_UserCard[5][5];					//玩家手中的牌
	BYTE					m_CardCount[5];						//玩家手中牌的数目
	
	TagSuperSet				m_TSuperSetResult;						//超端设置结果
public:
	int		m_iSendCardTime;										/// 发牌动画的总时间，控制动画。	



	int		m_iMoveSpaceCount;				//搓牌时控制牌间距移动变量
	

	__int64				m_iZhuangFen;					///本把庄家的得分	
	__int64				m_iXianFen;					///本把闲家的得分	
	__int64				m_iUseFen;					///闲家所得的分
	__int64				m_iMeFen;					///本把玩家自己的得分

	
	
	int					m_bNowNtListIndex;			/// 当前庄列表索引
	int					m_bNowGameState;			///当前游戏状态

	BYTE				m_byCardShap;				//用于播放声音
	

	int  m_iControlDecOrAdd;						//搓牌动画相关变量 0x00:加，0x01:减,0x02:播放完成
	int  m_iCardWith;

	int  m_iXuaZhuTimeCount;						//倒计时用

public:
	int  m_TongShaOrTongBeiFlag;			///通杀或通赔的标志位(0:通杀,1:通赔,-1:有输有赢)

	//超端变量
	bool					m_bIsSuperUser;			//是否超端	DWJ
	bool					m_bShowRobortNote;			//是否屏蔽机器人下注数	
	BYTE					m_bSuperTag[PLAY_COUNT];			//超端按钮设置(0为初始值，1为赢，2为输)
	bool					m_bSuperSet;						//超短设置是否成功
	bool					m_bSuperShow;						//显示超端各种设置按钮
	bool					m_bCanSuper;						//自己可以设置超端

	//函数定义
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);

#define SAFE_CTRL_OPERATE(CTRL_TYPE,CTRL_ID, OPERATE)\
	{\
		CTRL_TYPE* PRE=NULL; \
		PRE = dynamic_cast<CTRL_TYPE *>(m_pUI->GetIControlbyID(CTRL_ID));\
		if (PRE!=NULL) PRE->OPERATE; \
	}
public:  ///重载平台函数
    /// 初始化
    virtual int Initial();
    /// 初始化UI
    virtual int InitUI();
	/**
	* 接收服务端发送过来的消息，并设置游戏的当关状态，
	* 当断线重连时会进入，以设置游戏的当前状态
	* @param [in] void * pBuffer 数据包
	* @param [in] int nLen 数据包的长度
	* @return void 
	*/
	virtual void SetGameStation(void * pBuffer,int nLen);
	//空现阶段
	void	SetGameStation_WaitNext(void * pBuffer);
	//下注阶段
	void	SetGameStation_Bet(void * pBuffer);
	//发牌阶段
	void	SetGameStation_SendCard(void * pBuffer);
	//结算阶段
	void	SetGameStation_ShowResult(void * pBuffer);
	
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
	///停止游戏
	virtual bool OnGameQuiting();

private:
	/**
	* 加载服务器配置文件
	* return   void
	**/
	void LoadBcf();
	//重置UI界面
	void	ResetUi();

	/**
	*初始化游戏数据
	*return bool 初始化游戏数据是否成功
	*/
	bool InitGameData();

	/**
	* 重新设置UI的状态
	* @param BYTE bDeskStation [in] 玩家位置
	* @param bool bSetCommon [in] 是否设置共同属性
	*return bool true:重新设置成功，false:重新设置失败
	*/
	bool  ReSetGameUIStation(BYTE bDeskStation=255,bool bSetCommon = true,bool bAgreeStation = false); 
	/**
	* 获取游戏状态
	*return BYTE 游戏状态
	*/
	BYTE  GetStationParameter();
	/**
	* 点击托管 或 取消托管 按钮
	* @param [in] bool bAutoCard; true: 托管;false:取消托管
	* @return void
	*/
	void OnHitOkOrCancleAutoBtn(bool bAutoCard);	
	BYTE ViewStation(BYTE bDeskStation);
	/**
	* 插入普通消息
	* @param CString strTemp [in] 消息内容
	* @param BYTE bDeskStation [in] 说话玩家位置
	* @return void
	*/
	void InsertNormalMessage(const char *p, ...);
	/**
	* 播放动作音效
	* @return void
	*/
	void PlayMotionSoundOgg(irrklang::ISoundEngine *engine,BYTE bySoundType,int iSelect);
	/*
	*  播放背景声音
	*  @return void
	*/
	void SetPlaySoundBgByOgg(irrklang::ISoundEngine *engine,BYTE bSoundType);

public:		
	//显示玩家的信息
	void		ShowUserInfo(bool bShow);
	//显示庄家的信息
	void		ShowNTInfo(bool bShow);	
	//显示上庄列表
	void		ShowWaitNtList(bool bShow,bool bShowNew = false);
	//显示路子
	void		ShowLuZiInfo(bool bShow,bool bShowNew = false);
	//显示当前信息（显示当前把数与局数）
	void		ShowCurrentInfo(bool bShow);
	///显示筹码光标
	int			SetCurrentCursor(int uID);
	//显示上庄提示信息
	void		ShowShangZhuangNotice();
	//显示超端对话框
	void		ShowSuperDlag(bool bShow);
	//发送超端设置
	void		SendSuperSet();
	
	///每局结束后初始化视图数据
	void	ResetGameData();
	//有条件的设置
	void	EnableXiaZhuButton(bool bFlage);	
	//设置一个下注按钮的使用
	void	EnableOneXiaZhuButton(const int iButtonNum,bool bFlag);
	//设置下注按钮的可用(只是算单的设置使能端)
	void	SetXiaZhuButtonEnable(bool bFlage);	
	//开始游戏
	void		OnHandleBet(BYTE * buffer,int nLen);	
	//玩家点击下注
	void		OnXiaZhu(int iChooseRect);	
	//下注结果消息
	void		OnHandleBetResult(BYTE * buffer,int nLen);			
	//显示下注信息
	void		ShowXiaZhuInfo(bool bShow);


	void		OnHandleFree(BYTE * buffer,int nLen);			//接收到等待消息
	void		OnHandleKaiPai(BYTE * buffer,int nLen);			//开牌消息处理
	void		ClearAllDeskSHAndMoneyNumber();					//清除梭哈控件及游戏界面的游戏
	void		RecResultData(BYTE * buffer,int nLen);						//接收结算信息

	void	SetINumSingle(const int iNumID,__int64 iNum,bool bFlag);			//设置数字控件是否要显示符号位
	void	SetShowOrHidePai(int Desk,BYTE CardList[],int CardNum,bool flag);	//设置显示或隐藏	
	void	ShowOrHideResultCard(int Desk,BYTE CardList[],int CardNum,bool flag,BYTE UpOrDownType);
	void		SetShowOrHideClock(int iMaxNum,int iMinNum,bool bflag);			//设置显示时间	


	void	RecXiaZhuDataAfterEnableXiaZhuButton(const int count);		//接收到下注消息之后按钮的处理
	void		CheckIsCanXiaZhu();				//用于检测是否还可以下注

	void	ShowOrHideSettlementFrame(bool bFlag);				//显示或隐藏结算框
	void	SendCardTimer();									//发牌时间控制

	void	AutoMoveCardSpacing(BYTE iStation,int m_iMoveSpaceCount,bool flag); //自动移动牌间距
	//框闪闪
	void	RectXiaZhuGuangFlash(bool bShow);	
	//设置下注区域的显示与不显示
	void	SetXiaZhuAreaShowOrHide(bool bFlage);
	
	void	SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag);		//将字符串显示在静态文本控件上	



	//--------------------------------------------------------------------------------------------------
	bool CheckMeQiangZhuang();									//检测能否抢庄
	void	ShowShangZhuangOrXiaZhuangBt();							//显示早请上庄或我要上庄
	void ShangZhuangOrXiaZhuangButton(const int MsgNum);		//按键按下的处理
	bool CheckMeZhuang(void);									//检测自已是否在庄上
	void OnHandleShangZhuang(BYTE * buffer,int nLen);					//上庄消息
	//--------------------------------------------------------------------------------------------------

													
	//--------------------------------------------------------------------------------------------------
	void SetGameInfordDlg(bool bSoundEnable,bool bBGSoundEnable,bool bShow);		//设置显示游戏设置对话框
	void ButtonDownOK();													//点击确定按钮
	void ButtonCancel();													//点击对消按钮
	//--------------------------------------------------------------------------------------------------------
	int ShowOrHideHandImg(BYTE iStation,bool flag);    //显示或隐藏手图片
	int	ShowUserCardShape(BYTE iStaion , bool bshow ,bool bAllHand);   //显示玩家的牌型
	int GetCardShapePath(BYTE iStation ,CString &str,bool bAllHand);		//取得图片的路径
	int PlayAllWinOrLostAni(BYTE iGameStyle);					//播放通杀或通赔
	int CheckGameResult();										//检测游戏结果
	
	//--------------------------------------------------------




	void SetOneRButton(const int iRbuttonID,bool bSelect);	//设置单选按钮


	bool GetOneRButton(const int iRbuttonID);				//取得当前的单选按钮是否已选


	
	void PutBullCard(BYTE iDesk);		//摆牛

	

	void PlayAnimal(int iDesk,int iRunCount);											//播放动画
	void ControlPlayAnimal();													//控件播放动画


	//设置可操作手牌控件信息
	void	SetOperateCardInfot(const int iID,BYTE CardList[],int CardNum,bool bShow);
	//设置不可操作牌控件信息
	void	SetNoOperateCardInfo(const int iID,BYTE CardList[],int CardNum,bool bShow);
	//设置按钮是否可用
	void	SetButtonEnable(const int iID,bool bEnable);
	//设置按钮是否可见
	void	SetButtonVisible(const int iID,bool bVisible);
	//设置文本控件信息
	void	SetTextinfo(const int iTextID,__int64 iNum, bool bFlag);		
	//设置文本控件信息
	void	SetTextInfoSz(const int iTextID,CString sText, bool bFlag);
	//设置图片是否可见
	void	SetImageVisible(const int iImageID,bool bShow);
	//设置图片信息
	void	SetImageSrcXY(const int iImageID, int iSrcX, int iSrcY,bool bShow);
	//设置梭哈筹码控件信息
	void	SetShCtrlInfo(const int iShCrtlID,int iNum,bool bShow);
	//设置无符号数字控件属性
	void	SetINumInfoNoSymbol(const int iINumID,__int64 i64Num,bool bShow);
	//设置容器是否可见
	void	SetContainVisible(const int iID,bool bVisible);
};
