#pragma once

//#include "ClientView.h"
//#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "UpGradeLogic.h"
//#include "FinishDlg.h"
//#include "../../../../sdk/include/GameLanchSdk/BZAnima.h"    //动画投文件
//#include "OpenSelDlg.h"
//#include "AFCMessageBox.h"
#define MAST_PEOPLE			-1							//是否为管理员
//消息定义
#define IDM_BEGIN			WM_USER+120					//开始按钮消息
//#define IDM_USER_NT		WM_USER+121					//用户反牌
//#define IDM_GIVE_BACK		WM_USER+123					//留底牌按钮
//#define IDM_OUT_CARD		WM_USER+124					//用户出牌
//#define IDM_LOOK_LAST		WM_USER+125					//看上轮扑克
//#define IDM_BACK_CARD		WM_USER+126					//底牌信息
#define IDM_HAVE_THING		WM_USER+127					//有事要走
#define IDM_SEND_THING		WM_USER+128					//发送离开请求
#define IDM_ARGEE_LEAVE		WM_USER+129					//同意离开消息
#define IDM_STOP			WM_USER+130					//提前结束按钮
#define IDM_STOP_THING		WM_USER+131					//发送提前结束
#define IDM_ARGEE_STOP		WM_USER+132					//同意提前结束

#define IDM_PASS			WM_USER+133					//不出按钮消息
//#define IDM_JIAO_FEN		WM_USER+134					//叫分按钮消息
//#define IDM_GIVE_UP_FEN		WM_USER+135					//不叫按钮消息
#define IDM_AUTO			WM_USER+134					//托管消息
#define	IDM_ACTIVE_RESULT	WM_USER+136					//用户处理结果
#define IDM_SHOW_NOTE		WM_USER+137					//用户选择下注,加注
#define IDM_SELECT_NOTE_NUM	WM_USER+138					//用户下注
#define	IDM_SOUND			WM_USER+139					//用户设置声音
#define IDM_BGSOUND			WM_USER+140					//背景音乐
//定时器 ID
#define ID_BEGIN_TIME				100					//开始定时器
#define ID_OUT_CARD					101					//出牌等待时间
#define ID_LOOK_BACK				102					//看暗牌
#define ID_CHANGE_MUSIC			    103					//换背景音乐
#define ID_BEGIN_ANIMAL				104				//开始动画
#define ID_NOTE_ANIMAL				105				//下注动画
#define ID_SEND_CARD_ANIMAL			106				//发牌动画
#define ID_COLLECT_JETON_ANIMAL		107				//收集筹码



//重置函数参数
#define RS_ALL_VIEW					1					//重置所有参数
#define RS_GAME_CUT					2					//用户离开重置
#define RS_GAME_END					3					//游戏结束重置
#define RS_GAME_BEGIN				4					//游戏开始重置

//以下是新處理的場景聲音
#define SOUND_WIN					0x01				//胜利
#define SOUND_LOSE					0x02				//输
#define SOUND_WAITING				0x03				//等待
#define SOUND_ERROR					0x04				//出牌出错了
#define SOUND_READY					0x05				//准备好了
#define SOUND_START					0x06				//出第一手牌
#define SOUND_JIAO_FEN				0x07				//叫分
#define SOUND_OUT_CARD_FINISH		0x09				//某玩家出牌結束
#define SOUND_GAME_PASS				0x0A				//不出
#define SOUND_FRIEND_APPEAR			0x0B				//盟友出現
#define SOUND_USER_RUNAWARY			0x0C				//有人逃跑
#define SOUND_USER_LEFT				0x0D				//有人离开
#define SOUND_GIVE_BACK				0x0E				//埋底

//声音参数表

#define SOUND_SEND_CARD				0x22					//发牌
#define SOUND_WARNING				0x23					//警告
#define SOUND_INVALIDATE			0x24				//提示失败

#define SOUND_SHUTTLE				0x11					//梭哈
#define SOUND_FOLLOW				0x12					//跟
#define SOUND_ADD					0x13					//加
#define SOUND_NOTE					0x14					//下注
#define SOUND_GIVE_UP				0x15					//放弃


#define MAX_FLASH_COUNT             5               //flash的数量

#define FLSAH_CLOCK_ID           2    //时钟的id号为2

#define					NOTE_NUM				18

static int arrNoteEnun[NOTE_NUM] = 
{
	1,2,5,
	10,20,50,
	100,200,500,
	1000,2000,5000,
	10000,20000,50000,
	100000,200000,500000
};

class AI_CanOperate 
{

public:
	AI_CanOperate()
	{
		Init();

	}
	~AI_CanOperate()
	{
	}
	void Init()
	{
		bCanAdd = false;
		bCanNote = false;
		bCanFlollow = false;
		bCanGiveUp = false;
		bCanLook = false;
		bCanOpen = false;
		iNmuBtnEnableCount = -1;
	}
	void SetAIStation(bool bOpen,bool bAdd,bool bGiveUp,bool bFlollow,bool bLook,bool bChipIn)
	{
		bCanAdd = bAdd;
		bCanNote = bChipIn;
		bCanFlollow = bFlollow;
		bCanGiveUp = bGiveUp;
		bCanLook = bLook;
		bCanOpen = bOpen;
	}
	
	bool GetCanAdd() const {return bCanAdd;}
	bool GetCanNote()const {return bCanNote;}
	bool GetCanFlollow()const {return bCanFlollow;}
	bool GetCanGiveUp()const {return bCanGiveUp;}
	bool GetCanLook()const {return bCanLook;}
	bool GetCanOpen()const {return bCanOpen;}

	void SetNmuBtnEnableCount(int iCount = -1){iNmuBtnEnableCount = iCount;}
	int  GetNmuBtnEnableCount() const {return iNmuBtnEnableCount;}
protected:
	bool bCanAdd;
	bool bCanNote;
	bool bCanFlollow;
	bool bCanGiveUp;
	bool bCanLook;
	bool bCanOpen;

	int iNmuBtnEnableCount;
};
//游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{



	enum
	{
		FLASH_ID_CLOCK_INDEX = 0,       //闹钟动画id			
		FLASH_ID_NT_INDEX=1 ,             //庄的flash动画索引id
		FLASH_ID_LOOK_CARD_IDNEX=2,      //看牌动画索引
		FLASH_ID_OPEN_CARD_INDEX = 3,   //比牌动画id
		FLASH_ID_SEND_CARD_INDEX = 4  //发牌动画id
	};
	//固定信息
	//=====================时间表信息===================
	BYTE					m_iThinkTime;				//最少游戏盘数
	BYTE					m_iBeginTime;				//游戏开始时间
	int						m_iRoomBasePoint;			//房间倍数
	int						m_iRunPublish;				//逃跑扣分
	//	BYTE					m_iBeginStation;			//开始位置
	//	BYTE					m_iEndStation;				//结束位置
	BYTE					m_iCardCount;				//允许游戏的扑克数目
	BYTE					m_iPalyCardCount;			//游戏扑克数目
	
	//房间信息
	int						m_iLimitNote;				//最大限额注
	int						m_iBaseNote;				//底注
	int                     m_iGuoDi;                   //锅底
	int                     m_iLimitPerNote;            //每注限制，为暗注下限制，但显示为明注的限制
	
	int						m_iThisTurnLimit;			//本轮限注
	BYTE					m_iAfterWaitAction;  	//开始倒计时结束后的动作 1 自动开始 2 自动离开 0900311a1 JJ

	//运行信息
	BYTE					m_iDeskCardCount[PLAY_COUNT];		//每人桌面扑克的数目
	BYTE					m_DeskCard[PLAY_COUNT][5];			//每人桌面的扑克
	BYTE					m_UserCard[PLAY_COUNT][5];			//用户扑克
	
	int						m_iNowOutPeople;			//现在出牌者
	
	int						m_iFirstOutPeople;			//第一个出牌者

	BYTE                    m_bNtPeople;   //庄家
	bool                    m_bFirstGame;
	
	
	int						m_iNowBigNote;				//当前最大押注数
	int						m_iThisGameNote[PLAY_COUNT];			//本局游戏下注数(明注)	
	int						m_iTotalGameNote[PLAY_COUNT];			//本局游戏下注数(实际)

	
	BYTE					m_iThisNoteTimes;			//押注轮数
	int                     m_iFrontNote;                  //当前有效下注数即前位玩家有效下注数;
	int                     m_iMyAddNote;                //我加注的值 
	
	BYTE					m_bTimeOutCount;			//超时

	BOOL					m_bBgSound;					//背景音乐
	BYTE					m_bShowAnimalStation;		//动画显示位置
	BOOL                    m_Ming[PLAY_COUNT];          //明牌

	bool                    m_bOpenLose[PLAY_COUNT];     //玩家的比牌结果
	bool                    m_bIsGiveUp[PLAY_COUNT];     //玩家是否放弃

	BYTE                    m_bReachLimitStation;


	BYTE                    m_iComWinStation;
	BYTE                    m_iComLoseStation;


	int                     m_iOldCardCtrlDistH;        //原来牌控件的横向间隔
	int                     m_iNewCardCtrlDistH;
	//控件变量

	
	
	

	CUpGradeGameLogic		m_Logic;					//升级逻辑
	//CClientPlayView			m_PlayView;					//游戏视图
	int						m_iVersion;
	int						m_iVersion2;

	//背景音乐
	HWND					m_hMciWnd1;						//背景音乐(出牌)
	HWND					m_hMciWnd2;						//背景音乐(场景)

	//int                     m_iWaitForGroupID;

	//--------------------------------------原来view下的一些变量---------------------------------

	int								m_iNoteTotalPoint;					//总注
	int								m_iNoteLimit;						//本轮上限
	BOOL                            m_bMing[PLAY_COUNT];                        //明牌
	//CFinishDlg				        m_FinishDlg;					//结算框
	//COpenSelDlg                     m_SelOpenDlg;          //选择开牌多画框
	//CAFCMessageBox                  m_MsgBox;

	UINT                            m_arrUFlashID[MAX_FLASH_COUNT];


	int                             m_arrCartoonCount[PLAY_COUNT];

	bool                            m_bGameFinishAndComp;


	bool m_bWatchMode;

    int                             m_mingPaiNum;                       //用于记录自己明牌后的操作次数

    double                          m_shenglv;


	BYTE							byUserData[PLAY_COUNT][2];	// 玩家数据；是否机器人，牌大小排序
	BYTE							m_byAddCount;				//跟了几把
	BYTE							m_byMaxCount;				//跟几把后开牌（随机值）
	BYTE							m_byCtrPet;					//控制百分比（读配置值）
	bool							m_bIsCtrl;					//本局是否完全控制
	bool							m_bMaxPaiAI;				//最大牌者是否机器人
	bool							m_bMaxPaiMe;				//最大牌者是否自己
	
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();
	
	//检查游戏版本	
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

	//// 从BzMainFrame继承下来的函数
//	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);


	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

public:	//--------------------------与界面的交互-------------------------	
	
	//清除所有定时器
	void KillAllTimer();

	// 根据是否旁观显示各按钮
	void ShowStartButtons();
	// 设置玩家手上的扑克牌
	//void SetHandCard(BYTE bDeskStation, BYTE *pCard,int nCount, BYTE *pUpList=NULL,int nUpCount=0);	
	//按动扑克按钮函数
	LRESULT	OnHitCard(WPARAM wparam, LPARAM lparam);	
	//同意开始游戏 
	bool OnControlHitBegin();

	//--------------------------界面交互函数及按钮控制函数---------------------

	void UpdateShowNoteBtn(bool bOpen=false,bool bAdd=false,bool bGiveUp=false,bool bFollow=false,bool bLook=false,bool bChipIn=false);

	void SetCtrlBtn(bool bIsFirstNote);
	//设置加注多少的按钮状态
	bool UpdatAddNumBtn(bool bFirst);

	//void DrawUserInfo();
	//屏蔽所有的游戏操作按钮
	void DisableAllOperaBtn();
	//
	void EnableAddOrNote();

	//设置玩家信息
	//void SetUserInfo(BYTE bDeskStation, UserItemStruct * pUserItem,bool bIsCome);
	//设置自己的信息
	//void UpDateMyUserInfo();
	//设置自己的可用余额
	//void UpdateMyLeftMoney();
	//更新自己的分数
	//void UpdateFinshiPoint(int iAdd);
	//设置房间信息
	//void UpdateRoomInfo();
	//设置加筹码显示的操作
	//void UpdateAddChipShow(int bMutBase);
	//更新玩家下注信息
	//void UpdateNoteInfo(BYTE bDeskStation);

	//-------------------------------------提示相关操作-----------------------------------
	//设置相关提示信息
	//void SetTipInfo(BYTE bDeskStation,int iOperateType);
    //隐藏所有的提示信息
	//void DisVisibleAllTip(bool bIsShowGaveUpAndLose=false);  
	//
	//
 //   //屏蔽所有的加注提示
 // 	void DisVisibleAllAddTip();
	////屏蔽所有的下注提示
	//void DisVisibleAllChipinTip();
	////屏蔽所有的跟注按钮
	//void DisVisibleAllFollowTip();
	////屏蔽所有的放弃提示
	//void DisVisibleAllGiveUpTip();    //放弃提示
	//void DisVisibleAllOpenLoseTip();  //比牌失败提示
	//void DisVisibleAllLookTip();      //看牌提示
	//void DisVisibleReachLimiteTip();  //达到上限提示
	//void DisVisibleAllOpenWinTip();     //屏蔽所有的比牌胜利提示
	
	//void ClearAllUserInfo();
	//void ClearAllInfo();   //清空界面上所有的显示信息

	//void InvalidateAllTipCtrl();

	//-----------------------------------------------------------------
	//--播放动画操作
	//void PlayFlash(int iIndex,BYTE bDeskStation1=255,BYTE bDeskStation2=255);   //播放动画的索引号和给那个玩家播放，如果座位号为255，则为全体都有的动画
	//UINT PlaySendCard();
	//UINT PlayLookCard(BYTE bDeskStation);
	//UINT PlayOpenCard(BYTE bDeskStation1,BYTE bDeskStation2,int iCmpRet);
	//UINT PlayClock(BYTE bDeskStation,int iThinkTime);
	//UINT PlayNt(BYTE bDeskStation);

	//播放flash动画结束
//	void OnFinishFlash(FlashMsgData* pData);
	//停止所有的非循环播放的动画
	//void DelAllUnReCycFlase();   //删除所有的非循环播放的动画，此处出了庄外，其他的都删除
	//名牌后的相应函数
	void OnFinshLookCard();
    //比牌动画结束
	void OnFinishOpenCard();  //

	//void PlayWaitGroup(); //播放等待分组动画
	//void CloseWiatGroup();
	//---------------------------------播放动画操作，此动画由oyh编制，非flash--------------------
	//void PlayNoteCartoon(BYTE bDeskStation,int iNoteNum); //播放下注动画
	//bool GetStartPoint(BYTE bDeskStation,CPoint& pt);//下注时的开始位置
	//bool GetNoteEndRect(CRect& rc);   //下注时筹码的结束区域
	//bool GetCollectEndRect(BYTE bDeskStation,CRect& rc);//收筹码时的结束区域
	//-----------------------------------获取函数----------------------

	BYTE GetViewStaionToCtr(BYTE bDeskStation);
    //得到牌控件的容器
	//BBaseContainer::Node* GetCardCtrContainer(BYTE bDeskStation);
	//得到牌控件
	//CUpGradeGameCard* GetCardCtrl(BYTE bDeskStation);
	//得到用户信息控件容器节点
	//BBaseContainer::Node* GetUserCtrlContainer(BYTE bDeskStation);

	//BBaseContainer::Node* GetClockContainer(BYTE bDeskStation);
	//BBaseContainer::Node* GetNtContainer(BYTE bDeskStation);






	//-----------------------------------按钮操作--------------------------------------
    bool OnCloseGame();           //关闭游戏
     //游戏按钮操作
	bool OnChipIn();   //下注操作
	bool OnFllow();    //跟注操作
    bool OnAdd();      //加注操作
	bool OnGiveUp();   //放弃操作
	bool OnLook();     //看牌操作  变为名牌玩家  
	bool OnOpen();     //比牌操作

	//加注金额
	bool OnAddCount(int bMutBase);  //点击x1,x2,x3按钮
	bool OnAddCancle();              //点击加注旁边的取消按钮

	//------------------------------------机器人操作-------------------------------------------
    void AutoAI();             //机器人的自动下注
	void getShengLv();         //获取自己本次玩牌的胜率，单纯根据自己看牌的结果进行计算，不涉及根据对方的操作来猜测对方虚实的情况
	bool noPeopleKnow();       //目前还没有人知道自己的牌，即闷锅状态
	int GetAIThinkTime();
	AI_CanOperate m_AiOperateStation;  //玩家机器人操作	
	
	//--------------------------------重载函数-----------------------------------------
     virtual void OnCancel();//{OnCloseGame(); __super::OnCancel();};
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	//视图转换
	virtual BYTE ViewStation(BYTE bDeskStation);
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//重新设置界面
	virtual void ResetGameFrame();
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);
	//用户离开
	//virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	/// 为视频模块重载此函数
	//virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//安全结束游戏
	virtual bool OnControlSafeFinish(){return false;};
	//关闭窗口
	virtual bool AFCCloseFrame();
	//聊天消息
	//virtual void ShowUserTalk(MSG_GR_RS_NormalTalk *pNormalTalk);

	//重设游戏状态
	virtual void ResetGameStation(int iGameStation=1);
	//截获消息
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //初始化窗口
	virtual BOOL OnInitDialog();
	//-----------------------------------------------------------------------以上已经实现，ZXD
	// 获取视频窗口矩形
	BOOL GetVideoRect(CString  strNickname, RECT& rect, CString& pic){return FALSE;};
	// 用来给Flash动画使用的回调函数
	static void FlashEndMessage(CWnd * pWnd,UINT uID,CString msgString);

	//----------------------------------------原来view下的一些函数----------------------------
	//设置总注
	void SetTotalNote(BYTE iViewStation,int iNoteTotal);
	//总注
	void AddTotalNote(BYTE iViewStation,int iNoteTotal);
	
	
	//绘画明牌标志
	void SetMing(BYTE bDeskStation,BOOL iGiveup);
	//---------------------------------------------
	inline int GetLimit() {return (m_iLimitPerNote < m_iLimitNote ? m_iLimitPerNote : m_iLimitNote);}


	//跟具逻辑定义牌，转换成flash的牌
	
	int GetFlashCard(int iCard);

	int GetTotalPlayNum();  //得到所有玩家的数量


	//分析下注情况，把下注内容转换成可以现有的筹谋形式  如 90  会转换成 50 + 20 + 20
	int AnalyseNote(int iTotalNote,int arryHaveNote[],BYTE bAnayResult[]);      //分析筹码

	//播放新家的声音  //
	void PalyExtAddSound(int iSoundType);
    
	


	

	//设置本轮押注上限
	//void SetLimitNote(BYTE iViewStation,int iNoteLimit,int iBaseNote);
	//设置加筹码数
	//void SetAddJeton(int iBaseNote);
	//增加本轮下注数
	//void AddThisTurnNotePoint(BYTE bDeskStation,int iThisTurnPoint);
	//设置本次添加数
	//void SetAddNote(int iAddNote){m_iThisTurnAddNote=iAddNote;}

public:

};
