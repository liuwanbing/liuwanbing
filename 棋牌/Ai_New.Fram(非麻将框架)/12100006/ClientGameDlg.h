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
#define ID_SET_BOBOTIME				101					//设置簸簸时间
#define	ID_FOLLOW_TIME				102					//定时发送跟信息
#define	ID_SET_ALL_TIME				103					//定时发送敲信息
#define ID_BIG_TIME					104					//定时发送大信息
#define	ID_STOP_TIME				105					//定时发送休信息
#define	ID_GIVE_UP_TIME				106					//定时发送丢信息
#define	ID_SEPARATE_TIME			107					//定时发送分牌信息
#define ID_BEGIN_TIME				108					//开始定时器
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
	int						  m_iBoboPercentage;				//设置簸簸数最高是自已钱的百分之几
	CUpGradeGameLogic		  m_Logic;							//升级逻辑
	int						  m_iAfterWaitAction;
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
	BOOL                      m_bSound;                        ///是否开启音效
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
	
	BYTE						m_bySeparatorCard[MAX_CARD_COUNT];			//要分的牌

	bool						m_bSepartorFlag;							//是否已经可以分牌了
public:
	//构造函数
	CClientGameDlg();
	//析构函数
	virtual ~CClientGameDlg();
	
	//检查游戏版本	
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);

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

	BYTE GetViewStaionToCtr(BYTE bDeskStation);
	//-----------------------------------按钮操作--------------------------------------
    bool OnCloseGame();           //关闭游戏
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
	//跟具逻辑定义牌，转换成flash的牌
	int GetFlashCard(int iCard);
	int GetTotalPlayNum();  //得到所有玩家的数量
	//生成随机数
	int MyRand();
//================================================================================================
public:
	void LoadIniConfig();
	void SetBoboTime( void * pNetData, UINT uDataSize);				//设置簸簸数
	void SendBoboData();											//发送簸簸数据
	void Ass_SetBobo_Data(void * pNetData, UINT uDataSize);			//处理接收到的簸簸数据
	void Ass_Send_Card(void * pNetData, UINT uDataSize);			//处理接收发牌的数据
	void OnHandleGameBegin(void * pNetData, UINT uDataSize);		//开始玩游戏消息
	int CheckFrontUserOperator();									//检测之前的玩家是什么操作的


	void Bnt_Follow();												//跟操作
	void Bnt_Set_All();												//敲操作
	void Bnt_Big();													//大操作
	void Bnt_Stop();												//休操作
	void Bnt_Give_Up();												//丢操作
	void SendSeparatorCardData();									//发送分牌消息
	void HavaFollow(BYTE byCardType);									//有人选择跟
	void HaveSetAllNote(BYTE byCardType);								//有人选择敲
	void HaveSetBigNote(BYTE byCardType);								//有人选择大
	void NoOneOperator(BYTE	byCardType);								//还没有人操作或前家先择休或丢
	void OnHandleBet(void * pNetData, UINT uDataSize);					//处理用户所下的注信息
	void Ass_Sc_Send_One_Card_Finsh(void * pNetData, UINT uDataSize);	//发一张牌结束了
	void Ass_Sc_Notice_Send_Card(void * pNetData, UINT uDataSize);		//通知发牌
	int  GetMaxCardShap(int iCardNum,BYTE byOperator);					//获取当前牌最大的牌型
	void Ass_Sc_Notic_Separe_Card( void * pNetData, UINT uDataSize);	//处理接收可分牌消息
	void GetSeparatorCardShap(int iMyDesk);
	void SelectOperator(int iOperatorType,BYTE byCardType);				//要先择的操作类型
};
