#pragma once

#include "IGameImpl.h"
#include "IGameFrame.h"
#include "..\GameMessage\UpGradeLogic.h"
#include "irrKlang.h"
using namespace irrklang;
interface IGameFrame;
interface IUserInterface;

#define	 SOUND_START						50					//游戏开始
#define	 SOUND_DEAL							51					//发牌
#define  SOUND_HIT_CARD						52					//击牌
#define  SOUND_OUT_CARD						53					//出牌
#define	 SOUND_CLOCK						54					//闹钟
#define	 SOUND_ESCAPLE						55					//逃跑
#define	 SOUND_WIN							56					//赢
#define	 SOUND_LOSE							57					//输
#define	 SOUND_NIUNIU						58					//牛牛

//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_THINK_TIME				103					//思考定时器
#define ID_SCARD_TIME				104					//发牌定时器
#define ID_RESULT_TIME				105					//结束定时器
#define ID_SUPER_TIME				106					//超端设置提示计时器
#define ID_SUPER2_TIME				107					//超端按钮提示计时器

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

	CUpGradeGameLogic		m_Logic;							///逻辑

	UserInfoForGame			m_UserInfo[PLAY_COUNT];			/**<玩家信息*/

	ISoundEngine			*m_SoundEngine;						///音效指针

	CWinThread				*m_tSond;							/**<线程指针*/

	SoundNews				m_pSoundNews;						/**<声音信息*/

	IniData					m_sIData;							///服务端INI文件数据

	BData					m_sBData;							///游戏基本数据

	BYTE					m_bGameStation;						///游戏状态

	BYTE					m_byLgStation;						///玩家逻辑位置

	bool					m_bWatchMode;						///是否为旁观者

	bool					m_bWatch;							///是否充许旁观

	bool					m_bSound;							///是否播放音效

	bool					m_bShowComma;						///是否显示逗号	

	BYTE					m_bySpaceBits;						///配置分隔位数	1到9的数字

	int						m_iUCCount[PLAY_COUNT];				///玩家手牌数量

	BYTE					m_byUCard[PLAY_COUNT][USER_HAND_CARD];	///玩家扑克
				
	BYTE                    m_byDownBCard[PLAY_COUNT][USER_HAND_CARD];	///下墩牌

	int						m_iSCCount;							///发牌张数

	//超端变量
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
	*
	* @param [in] void * pBuffer 数据包
	*
	* @param [in] int nLen 数据包的长度
	*
	* @return void 
	*/
	virtual void SetGameStation(void * pBuffer,int nLen);

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


public:
	/**
	* 加载服务器配置文件
	*
	* return   void
	**/
	void LoadBcf();

	/**
	*初始化游戏数据
	*
	*return bool 初始化游戏数据是否成功
	*/
	bool InitGameData();

	/**
	* 重新设置UI的状态
	*
	* @param BYTE bDeskStation [in] 玩家位置
	*
	* @param bool bSetCommon [in] 是否设置共同属性
	*
	*return bool true:重新设置成功，false:重新设置失败
	*/
	bool  ReSetGameUIStation(BYTE bDeskStation=255,bool bSetCommon = true,bool bAgreeStation = false); 

	/**
	* 获取游戏状态
	*
	*return BYTE 游戏状态
	*/
	BYTE  GetStationParameter();

	void OnHitBegin(void);			//开始按钮函数

	/**
	* 牌上左键弹起事件
	*
	* @return void
	*/
	void OnLButtonUpOnCard();

	/**
	* 提示
	*/
	void OnHitCue(bool bShowChongPai = true); 

	/**
	* 重摆
	*/
	void OnHitChongBai();

	/**
	* 摊牌
	*/
	void OnHitTanPai();

	/**
	* 点击托管 或 取消托管 按钮
	*
	* @param [in] bool bAutoCard; true: 托管;false:取消托管
	*
	* @return void
	*/
	void OnHitOkOrCancleAutoBtn(bool bAutoCard);

	
	BYTE ViewStation(BYTE bDeskStation);

	/**
	* 发送是否充许旁观数据
	*
	* @return void
	*/
	void SendIsWatchData();

	/**
	* 插入普通消息
	*
	* @param CString strTemp [in] 消息内容
	*
	* @param BYTE bDeskStation [in] 说话玩家位置
	*
	* @return void
	*/
	void InsertNormalMessage(const char *p, ...);

	//換系統牌
	BOOL ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard);

	static UINT PlaySond(LPVOID lpParam);

	/**
	* 播放牌型音效
	*
	* @return void
	*/
	void PlayCardShapSound();

	/**
	* 播放动作音效
	*
	* @return void
	*/
	void PlayMotionSound();

	/**
	* 停止播放音效
	*
	* @param [in] bool bFlag  是否暂停
	*
	* @return void
	*/
	void StopPlaySound(bool bFlag );

public:		//界面操作
	/**
	* 显示/隐藏 控件
	*
	* @param [in] int iControlID 控件ID
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return BOOL 是否设置成功
	*/
	BOOL ShowControl(int iControlID,bool bFlag);

	/**
	* 设置可用/禁用 控件（只支持按钮）
	*
	* @param [in] int iControlID 控件ID
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return BOOL 是否设置成功
	*/
	BOOL EnableControl(int iControlID,bool bFlag);

	/**
	* 查询按钮可用/禁用 控件（只支持按钮）
	*
	* @param [in] int iControlID 控件ID
	*
	* @return BOOL 返回控件状态 TRUE:可用，FALSE:不可用
	*/
	BOOL GetControlEnable(int iControlID);

	/**
	* 查询按钮显示/隐藏 控件（只支持按钮）
	*
	* @param [in] int iControlID 控件ID
	*
	* @return BOOL 返回控件状态 TRUE:显示，FALSE:不显示
	*/
	BOOL GetControlShow(int iControlID);

	/**
	* 显示/设置 控件（只支持按钮）
	*
	* @param [in] int iControlID 控件ID
	*
	* @param [in] bool sFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] bool eFlag 设置标志true:设置，false:禁用
	*
	* @return void
	*/
	void ShowOrEnableControl(int iControlID,bool sFlag,bool eFlag);

	/**
	* 显示/隐藏 数字控件
	*
	* @param [in] int iControlID 控件ID
	*
	* @param [in] __int64 i64Num 显示数值
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] bool bSign 是否有符号
	*
	* @return void
	*/
	void ShowOrHideNumControl(int iControlID,__int64 i64Num,bool bFlag,bool bSign = false);

		/**
	* 播放外部动画
	*
	* @param [in] int iControlID 动画控件ID
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] CString StrTemp 动画路径
	*
	* @return void
	*/
	void PlayExAnimate(int iControlID, bool bFlag, CString StrTemp);

	/**
	* 显示或隐藏倒计时时钟
	*	
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] int nShowMaxNum 最大值
	*
	* @param [in] int nShowMinNum 最小值默认为0
	*
	* @param [in] int iTimeID 计时器ID 默认值为 0 无效
	*
	* @return void
	*/
	void ShowOrHideTime(BYTE bDeskStation,bool bFlag,int nShowMaxNum,int nShowMinNum=0,int iTimeID=0);

	/**
	* 显示/隐藏 准备图标
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SOrHReadyMark(BYTE bDeskStation,bool bFlag);

	/**
	* 显示/隐藏底分
	*
	* @param [in] __int64 i64Num 显示数值
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] bool bHaveSign 是否有符号 true:有，false:没有
	*
	* @return void
	*/
	void SOrHBasePoint(__int64 i64Num,bool bFlag,bool bHaveSign=false);

	/**
	* 显示或隐藏手牌牌控件
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] BYTE iCard[] 牌数组
	*
	* @param [in] BYTE iCardCount 牌张数
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void ShowOrHideHandCard(BYTE bDeskStation,BYTE iCard[],BYTE iCardCount,bool bFlag);

	/**
	* 显示或隐藏摆牌后的上牌和下牌控件
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] BYTE iDownCard[] 牌数组
	*
	* @param [in] BYTE iDownCCount 牌张数
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SOrHUAndDCard(BYTE bDeskStation,BYTE iDownCard[],BYTE iDownCCount,bool bFlag);

	/**
	* 显示或隐藏玩家牌数据 包括摆牌前和摆牌后
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] BYTE bType 1:摆牌前状态 2:摆牌后状态 
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SOrHCard(BYTE bDeskStation,BYTE bType,bool bFlag);

	/**
	* 显示或隐藏 摊牌标志
	*
	* @param [in]  BYTE bDeskStation 玩家位置
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SorHTanPaiMark(BYTE bDeskStation,bool bFlag);

	/**
	* 显示或隐藏 玩家牌型
	*
	* @param [in]  BYTE bDeskStation 玩家位置
	*
	* @param [in]  int iCardShape 牌型
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SorHCardShape(BYTE bDeskStation,int iCardShape,bool bFlag);

	/**
	* 显示或隐藏 结算数据
	*
	* @param [in] GameEndStruct *pGameEnd 游戏结束统计数据包
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void SOrHGResultData(GameEndStruct *pGameEnd,bool bFlag);

	/**
	* 显示或隐藏 托管按钮
	*
	* @return void
	*/
	void SOrHAutoBtn();

	/**
	* 显示或隐藏 托管图片
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @return void
	*/
	void ShowOrHideAutoImage(BYTE bDeskStation,bool bFlag);

	/**
	* 判断是不是存在该玩家
	*
	* @param [in] BYTE bDeskStation 玩家位置
	*
	* @return 存在与否
	*/
	bool IsPlayer(BYTE bDeskStation);

	/**
	* 初始化超端设置
	*
	* @param [bool] bFlag 显不显示 超端
	*
	* @return void
	*/
	void InitialSuper(bool bFlag = false);

	/**
	* 点击重置超端设置按钮
	*
	* @param void
	*
	* @return void
	*/
	void OnHitResetSuperBut();

	/**
	* 点击超端中输赢按钮
	*
	* @param void
	*
	* @return void
	*/
	void OnHitSuperWinBut(int iControlID);

	/**
	* 点击超端设置生效按钮
	*
	* @param void
	*
	* @return void
	*/
	void OnHitSuperOKBut();

	/**
	* 点击超端按钮
	*
	* @param void
	*
	* @return void
	*/
	void OnHitSuperBut();
};
