#include "IGameImpl.h"
#include "..\GameMessage\UpGradeLogic.h"
#include <irrKlang.h>

//定时器
#define		TIME_GAME_BEGIN                10    //开始时间

//定时器 ID
#define ID_BEGIN_TIME				    100					//开始定时器
#define ID_OUT_CARD_TIME				110					//出牌等待时间
#define ID_TIME_CLOCK                   115                 //倒数闹钟计时器
#define ID_BOBO_TIME					116					//设置簸簸数
#define ID_SEPARET_CARD_TIME			117					//分牌时间

#define  SOUND_FOLLOW					0x00               //跟
#define  SOUND_SET_ALL		            0x01               //敲
#define  SOUND_BIG		                0x02               //大
#define  SOUND_STOP						0x03               //休
#define  SOUND_GIVE_UP		            0x04               //丢
#define	 SOUND_COLOR					0x05			   //时钟
#define  SOUND_SEND_CARD				0x06			   //发牌
#define  SOUND_CLICK_BUTTON				0x07			   //单击按扭
#define  SOUND_MOVEBUTTON				0x08			   //移动到筹码上面
#define  SOUND_WIN					    0x09			   //胜利声音
#define  SOUND_LOSE						0x0A			   //失败声音
#define  SOUND_OPEN_CARD				0x0B			   //开牌
#define  SOUND_SEPARE_CARD				0x0C			   //分牌
#define  SOUND_READY					0x0D				//准备好了
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束
#define GF_EARLY_END				12				//提前结束

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
private:
	IGameFrame                  *m_pGameFrame;
	IUserInterface              *m_pUI;
	CUpGradeGameLogic		    m_Logic;					//逻辑规则
	ISoundEngine               *m_SoundEngine;
	ISoundEngine			   *m_SoundEnginePlay;			//播放其它音效声音
	ISoundEngine				*m_SoundEnginBg ;           ///背景音效
	bool						m_bSoundPlay ;				//是否可以播放声音
	bool						m_bSoundPlayBG;				//是否放背景音乐
	BYTE                         m_bGameStation;               ///<游戏状态
	///运行状态变量
	BYTE					  m_iUserCardCount[PLAY_COUNT];					///<用户手上扑克数目
	BYTE					  m_iUserCard[PLAY_COUNT][MAX_CARD_COUNT];		///<用户手上的扑克
	BYTE					  m_iDeskCardCount[PLAY_COUNT];					///<桌面扑克的数目
	BYTE					  m_iDeskCard[PLAY_COUNT][MAX_CARD_COUNT];		///<桌面的扑克
	///游戏基本数据
	BYTE                      m_iBeginTime;                     ///<游戏开始时间
	BYTE					  m_byBoBoTime;						///设置簸簸时间
	BYTE                      m_iThinkTime;                     ///<游戏思考时间
	BYTE					  m_bySeparateCardTime;				///分牌时间
	BYTE                      m_iEndWaitTime;                   ///<游戏结束后等待时间
	int                       m_iRunPublish;                    ///<逃跑扣分数
	BYTE					  m_byGameStation;					//游戏当前的状态()
	int                       m_iCurrentTimeId;                 ///当前计时器ID
	int                       m_TimeClockCount;                ///闹钟倒计时时长
	bool					  m_bInLisureRoom;					//该游戏是否在悠闲房
	BYTE					  m_iThisNoteTimes;						//押注轮数
	__int64                   m_i64FrontNote;						//当前有效下注数即前位玩家有效下注数;
	__int64                   m_i64MyAddNote;						//我加注的值 
	int						   m_iFirstOutPeople;					//第一个出牌者
	int						   m_iNowOutPeople;						//现在出牌者
	BYTE                       m_bNtPeople;							//庄家
	BYTE                       m_bSendCardPos;						//当前发牌动画的人
	BYTE                       m_bSendCardCount[PLAY_COUNT];		//当前发牌动画的张数
	bool                       m_bPlayer[PLAY_COUNT];				//是否玩家或中途进入观战者
	bool					   m_bUserReady[PLAY_COUNT];			//玩家准备标志位
	int						   m_iMyLogDesk;						//玩家的位置
	bool					   m_bSepartorFlag;						//分牌消息已经发了
	__int64					   m_i64Bobo[PLAY_COUNT];				//簸簸数
	__int64					   m_i64NoteMoney[PLAY_COUNT][3];		//保存每次下注的信息
	__int64					   m_i64UseAllNote[PLAY_COUNT];			//用户所下的总注
	//房间信息
	__int64					   m_i64BoBase;									//簸底
	__int64					   m_i64SmallBobo;								//最小簸簸
	__int64					   m_i64TempBobo;								//保存簸簸临时变量
	BYTE					   m_byUserOperatorType[PLAY_COUNT];			//玩家的操作
	int							m_iCardShowCount;							//要显示的牌张数
	BYTE						m_byOneOparateHand[MAX_CARD_COUNT];			//第一个牌可操作控件数据
	int							m_iOneOparateHandCount;						//牌的张数
	BYTE						m_byTwoOparateHand[MAX_CARD_COUNT];			//第二个牌可操作控件数据
	int							m_iTwoOparateHandCount;						//牌的张数
	BYTE						m_byNoteTime[PLAY_COUNT];					//第几次下筹码

public:
	/// 初始化
	virtual int Initial();
	/// 初始化UI
	virtual int InitUI();
	/// 游戏消息
	virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
	/// 界面消息
	virtual int OnUIMessage(TUIMessage* pMessage);
	//
	virtual int ResetGameFrame(void);
	//玩家进入
	virtual int GameUserCome(void);
	//玩家离开
	virtual int GameUserLeft(int nIndex); 
	//玩家断线
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
private:     
	//升级
	void OnGameUpgrade(void *buffer);
	//发牌
	void OnHandleSendCardBegin(void *buffer);
	//开始
	void OnHandleGameBegin(void *buffer);
	//押注
	void OnHandleBet(void *buffer,int iStyle);
	//正常结束
	void OnHandleNormalFinish(void *buffer);
	//取得下一个玩家，比牌结束后，需要获取到底哪个玩家达到下注上限
	BYTE GetNextNoteStation(BYTE bCurDeskStation);
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
	/// 设置显示庄家
	void SetNT(int iNTDesk);
	//开始游戏,开始发牌
	bool OnControlHitBegin();
	//设置玩家信息
	void SetUserInfo(int iDesk = -1);
	//游戏逻辑数据重配
	void InitGameData();
	///加载配制文件
	void LoadData(void);
	///播放音效
	void SetPlaySound(int iSoundID, BOOL bISLoop = FALSE);
	//随机背景音乐
	BOOL PlaySound(bool isLoop);
	//设置房间信息
	void UpdateRoomInfo();
	//设置自己的可用余额
	void UpdateMyLeftMoney();
	//设置自己的信息
	void UpDateMyUserInfo();   
	//显示各玩家下注状况
	void UpdateNoteInfo(BYTE bDeskStation, int nNoteValue = 0);
	//播放发牌动画
	void PlaySendCard(BYTE Station);
	void StartPlaySendCard();
	int GetTotalPlayNum();
	void SetCtrlBtn(bool bIsFirstNote);
	bool OnCompareCard(BYTE bViewStation);	// 点击针对某玩家的比牌按钮
	//下注筹码动画
	void PlayXianShiChouMa(int nDeskStation, __int64 iNoteMoney,bool bFlag);
	//收集赢的筹码动画
	void PlayGetWinChouMa(int nDeskStation);
	//获取游戏状态
	BYTE GetStationParameter(void);
//----------------------------------------------------------------------
	void SOrHReadyMark(BYTE bDeskStation,bool bFlag);				//显示准备按钮
	void HideNoteAndPic();											//隐藏游戏因面的按钮及提示文字
	void HideAllReadImag();											//隐藏所有准备图片
	void SetShowOrHideBoboAndResetnoteBNT(bool bFlag);
	void SetNoteEnable(__int64 i64Money);							//依据金币数设置筹码
	void SetBoboData();												//设置簸簸数
	void Ass_SetBobo_Data(BYTE * buffer,int nLen);					//处理服务端发过来的簸簸数
	void SetBoBoValue(int iNoteId);									//设置簸簸数值
	void SetShowBoboValue(int iDesk,__int64 i64dMoney);				//设置显示簸簸数值
	void Bnt_Reset_Bobo();											//重新设置簸簸数
	void Bnt_Reset_Note();											//重新设置其下天津市数
	void SetEnableNoteBNT();										//设置下注按钮使其生效
	void SetNoteValue(int iNoteId);									//设置下注数
//----------------------------------------------------------------------------------------
	void Bnt_Follow();												//跟操作
	void Bnt_Set_All();												//敲操作
	void Bnt_Big();													//大操作
	void Bnt_Stop();												//休操作
	void Bnt_Give_Up();												//丢操作
	void Ass_Sc_Notice_Send_Card(BYTE * buffer,int nLen);			//通知发牌
	void Ass_Sc_Send_One_Card_Finsh(BYTE * buffer,int nLen);		//发一张牌结束
	void Ass_Sc_Notic_Separe_Card(BYTE * buffer,int nLen);			//处理可以分牌消息
	void Ass_Sc_Separat_Card(BYTE * buffer,int nLen);				//玩家分牌成功了	
	void Ass_Sc_Show_All_Separat_Card(BYTE * buffer,int nLen);		//所有的牌都已经分好了
	void Ass_Sc_UserGiveUp(BYTE * buffer,int nLen);					//用户点击放弃
	void SetShowSeparetCardBntAndImageBottomCard(bool bFlage);					//设置显示分牌按钮和底图
	void Operator_Card_One();			//玩家牌的操作(第一个可操作控件)
	void Operator_Card_Two();			//玩家牌的操作(第二个可操作控件)
	void SetShowCardShap(BYTE byCardList[],int iCardCount,int iImagID,bool bShow);			//设置显示牌型牌型
	int GetCardShapePath(BYTE byCardList[],int iCardCount,CString &str);
	void Separate_Card();																	//分牌处理
	void SetShowLeftAndRighCardAndShap(bool bShow);											//设置显示分牌完成后的所有牌
	void SetShowOneCard(BYTE byCardList[],int CardCount,int iINopID,bool bShow);			//设置显示一分牌
	void HideOrShowOperatCard(bool bShow);													//显示或隐藏可操作牌控件
	void HideINoperateCard(int iCardID);
	void SetInumPro(const int iNumID, __int64 i64Num,bool bFlag,bool bSingle);				//设置显示图片数字控件
	void SetShowNotePic(const int iImagID,__int64 iNoteMoney);								//设置显示筹码图片
	void GetNotePicPath(__int64 i64Money,CString &str,int *iNum);							//取得筹码图片路径

	void FinishMoveNote(int iMoveID);														//移动完成的ID控件
	void SetShowNoticPic(int iImageID,int iOperType);										//要显示的提示文件控件
	void GetOperatePicPath(int iOperatType,CString &str);									//获取图牌名及路径
	void GetGiveUpPicPath(int iCardNum,CString &str);										//获取放弃牌的张数
	void SetOperateNote(bool bFollow,bool bSetAll,bool bBig,bool bStop,bool bGiveUp);		//设置操作按扭的使能

	void SetTextShow(int iTextID,__int64 iMoney,bool bShow);								//将数字转成文字显示

	void ShowGiveUpPic(int iCardNum,int iPicID,bool bShow);
	void PlayGameSetShowAllNote();															//在玩电脑时断线重连显示的筹码图片

	void OperatorPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop);	//播放声音

	void SetShowCardSpaceCard(int iImagID,int iCardShap,bool bShow,int iSelectShow);		//显示三花十或三花六图片
};