/// ServerManage.h: Headers and defines for CServerGameDesk
///


#if !defined(_SERVERMANAGE_H__INCLUDED_)
#define _SERVERMANAGE_H__INCLUDED_

#include "StdAfx.h"
#include "../GameMessage/UpGradeLogic.h"

///游戏结束标志定义
#define GF_NORMAL					10			   /**<  游戏正常结束   */
#define GF_SAFE						11			   /**<  游戏安全结束   */

#define ID_GAME_TIMER_FALG_START    0x10           /**<   能获取最后剩余时间的定时器标志   */


#define ID_TIMER_SEND_CARD          0x10           /**<  获取剩余多少的定时器在0x10~0x20范围内，只需要定义后面一个字节   */
#define ID_TIMER_OUT_CARD           0x11           /**<  出牌定时器ID   */
#define ID_TIMER_GAME				0x12		   /**<  游戏时间   */
#define ID_TIMER_BEGIN_TIME			0x13		   /**<  游戏开始计时器   */
#define ID_GAME_TIMER_FALG_END      0x20           /**< 能获取最后剩余时间的定时器标志   */

#define ID_TIMER_AI_APPLY_NT     0x21           /**<  机器人申请上庄计时器 （机器人上庄控制）  */
#define ID_TIMER_XIA_ZHU       0x22           /**<  下注   */
#define ID_TIMER_KAI_PAI       0x23           /**<  开牌   */
#define ID_TIMER_FREE_TIME     0x24           /**<  空闲时间   */
#define ID_TIMER_GAME_TIME     37			  //下注时间
#define ID_TIMER_GAME_SENDCARD 38			  //发牌时间   
#define ID_TIMER_GAME_FREE     39			  //空闲时间   

#define ID_TIMER_GAME_NEW_GAME 0x28			  /**<  新一局   */
#define ID_TIMER_GAME_SHOW_JS  0x29			  /**<  显示结算计时器   */
	 

///百家乐下注区域定义定义
#define GF_ZHUANG				0			   /**<  庄   */
#define GF_Z_TIANWANG			1			   /**<  庄天王   */
#define GF_Z_DUIZI				2			   /**<  庄对子   */
#define GF_XIAN					3			   /**<  闲   */
#define GF_X_TIANWANG			4			   /**<  闲天王   */
#define GF_X_DUIZI				5			   /**<  闲对子   */
#define GF_HE					6			   /**<  和   */
#define GF_T_HE					7			   /**<  同点和   */

///游戏服务器处理类
class CServerGameDesk :public CGameDesk
{
public:
	CServerGameDesk(void);
public:
	~CServerGameDesk(void);

	/// 加载配置文件
	BOOL LoadIni();
	BOOL LoadExtIni();
	BOOL LoadExtIni(int iRoomID);

	/// 原各玩家所坐的位置
	void InitUserSeatID(void);

	/// 设置定时器
	bool SetGameTimer(BYTE byTimerID, int uElapse);

	/// 删除定时器
	bool KillGameTimer(BYTE byTimerID);

	/// 清除所有定时器
	void KillAllTimer(void);

	/// 根据玩家的逻辑位置获取其所坐的位置(服务端移动位置时玩家不一定就坐在逻辑位置上)
	virtual BYTE GetSeatID(BYTE byDeskStation);

	/// 根据玩家的所坐的位置获取逻辑位置(服务端移动位置时玩家不一定就坐在逻辑位置上)
	virtual BYTE GetDeskStation(BYTE bySeatID);

	/// 获得当前正在运行的游戏定时器的ID
	virtual BYTE GetGameTimerID();

	/// 获得当前正在运行的游戏定时器的剩余时间
	virtual int GetGameRemainTime();

	///初始化游戏逻辑zht20100205
	virtual bool InitDeskGameStation();
	
// 重载
public:
	///游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	///游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	///判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	///游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	///用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	///用户断线离开
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	///用户坐到游戏桌
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	///旁观参数变化
	virtual bool OnWatchSetChange(BYTE byDeskStation, long int dwTargetUserID, bool bEnableWatch);

	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);

	/// 定时器消息
	virtual bool OnGameTimer(BYTE byTimerID);

	///整理数组中的数据，把无用的数组元素清空
	void MoveData(int type);

	///处理玩家的下注
	void HanleXiaZhu(BYTE Station,void * pData, UINT uSize);
	///处理玩家的上庄消息
	void HanleSzhuang(BYTE Station,void * pData, UINT uSize);

	///处理玩家的下庄消息
	void HanleXzhuang(BYTE Station,void * pData, UINT uSize);

	///向玩家游戏数据（玩家断线重连后会发送数据请求消息，服务器收到该消息就通过本接口）
	void OnGetGameData(BYTE Station,void * pData, UINT uSize);
	///超级客户端设置机器人控制参数
	void SetAICtrl(BYTE Station,void * pData, UINT uSize);

	///打印本局数据
	void PrintEnding();
	///本局算分
	void CountFen();
	///计算牌值
	void CountPaiZhi(int count);
	///控制开庄开闲
	void ConTrolZX();
	///发牌
	void SendCard();

	///闲家拿了第三张牌情况下，庄家是要拿牌lym0511
	///pai:闲家的第三张牌值
	bool IsGetTheThird();

	///获取第三张牌
	bool GetTheThird();

	///获取前两张牌
	void GetTwoCard();

	///获取赢钱的区域
	void GetWin();

	///同点检测
	///@param num,检测前几张牌
	bool CheckTongDian(int num);

	///清理已计算的牌点数据lym0511
	void ClearWinData(bool all);

	///改变赢家
	///zhuang:true为控制庄赢，false为控制闲赢
	void ChangeWinner(bool zhuang);

	///交换庄闲的前两张牌lym0511
	void ChangeCard();

	///一局结束并过了空闲时间后检测是否能够继续开始游戏
	bool CheckCanBegin();

	///庄家下庄后获取下一个庄家列表中的庄家
	bool GetNextNT();

	///通过全部举手的方式开始游戏
	bool StartGame();

	///记录本局的成绩数据
	void RecordData();

	///初始化服务器数据数据ym0514
	///初始化大部分数据，有些数据不能初始化的：当前庄家进行了几局，前30局的数据，
	void ServerInit();

	///lym重载游戏能否开始的条件，防止无庄的假开始
	bool CanBeginGame();

	///vip玩家双倍下注的权限判断
	BOOL CanDouble(BYTE bDeskStation);

	///日志记录接口
	void  SrvTestlog(TCHAR *pMessage);

	///控制输赢
	BOOL  m_bSuperSetting;       // 标识本局是否有控制
	BYTE  m_bSuperState;         // 0:正常  1：庄赢 2:庄输

	// 控制输赢设置
	void OnSuperSetting(BYTE bDeskStation, BYTE bState);  

	// 验证超级用户权限
	void AuthPermissions();   

	/// 获取返还分
	int GetMeFanFen(int station);

	///设置72把牌数据（记录发好的牌数据）
	void SetResultData(int num);
	///获取牌数据
	void GetResultData(int num);

	///获取金钱类型的钱数
	int inline GetMoneyFormType(int type,int power=1)
	{
		return m_iNoteMoney[type-1]/power;
	};

	///m_byQuYuJinBi
	BYTE inline GetWeiZhiRand(BYTE num)
	{
		return num/15+4;
	};

	///描述：机器人输赢自动控制
	void IAWinAutoCtrl();
	//-------ADD-BY-DWJ-20121108--------------------------------
	void CheckNewDayReSetIni(CINIFile *f, CString *key);//检查是否已经新的一天了，需要重新设定机器人控制配置参数

	void GetAIContrlSetFromIni(int iRoomID);			/**从配置文件中读取机器人控制设置的参数值*/

	///描述：计算当前牌机器人的输赢钱
	int CountAIWinMoney();

	///描述：获取一个满足申请上庄条件的机器人位置
	int GetAnAIApplyStation();

	///描述：将一个机器人放的庄家列表中（机器人上庄控制）
	bool SetAIToApplyList(int station);
	//发送开牌消息
	void OnSendOpenCard();
	//发送结算消息
	void OnSendEnd();
	//发送空闲消息
	void OnSenFree();
	//游戏重新开始
	void RestartGame();
	///描述：获取每个区域最大下注
	void GetQuYuCanNote();

public:		//游戏不能下注后立即发牌 相关 操作 和 数据 add xyh 2011-12-6
	///判断下注区下注是否已满
	bool JudgeXiaZhuIsFull(void);

	///发送下注已满消息
	void SendXiaZhuFullNews(void);

	bool m_bNoteFullOpenCard;	//下注已满是否开牌 true:下注满立刻开牌 false:下注满不立刻开牌

protected:
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	///重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);

	/// 托管消息响应函数
	virtual void OnChangeTrustee(BYTE byDeskStation);

	/// 用户请求离开
	virtual bool OnUserHaveThing(S_HAVE_THING &sHaveThing);

	/// 玩家是否同意离开请求
	virtual bool OnAgreeUserLeave(S_AGREE_USER_LEAVE &sAgreeUserLeave);
	///统计机器人上庄数量
	int CountRobortNum();
	///是否有机器人在庄家列表中
	BOOL IsHaveUserInList();
	//配牌
	void MatchCard(WORD (*wCardList)[3]);
	//清理庄家
	void ClearNt();
protected:
	CPokerGameLogic m_PokerGameLogic;  /**< 牌类游戏逻辑*/
	GAME_BASE_INFO m_GameBaseInfo;     /**< 游戏的基本信息*/
	
	BYTE m_abySeatID[PLAY_COUNT];      /**< 玩家坐的位置*/
	BYTE m_bySendStartPos;             /**< 第一个发牌的玩家*/
	BYTE m_byFirstOutPos;              /**< 第一出牌玩家*/
	WORD m_wMingCard;                  /**< 一张明牌*/
	bool m_bShuangKou;                 /**< 上局是否双扣成功 默认是成功的   */
	//配牌器专用
	int m_iIsMatch;	//是否配牌
	BYTE m_byLeaveAgree;				   /**< 离开同意标志*/
	BYTE m_byUserAgreed;				   /**< 离开同意标志(是否已经同意过了)*/
	BYTE m_byHaveThingPeople;		       /**< 有事要走玩家位置*/

	// 状态信息 (数据按照逻辑位置保存)
	BYTE  m_byBaseOutCount;					/**< 出牌的数目*/
	WORD  m_wBaseOutCard[45];				/**< 出牌*/
	BYTE  m_byDeskCardCount[PLAY_COUNT];	/**< 桌面扑克的数目*/
	WORD  m_wDeskCard[PLAY_COUNT][45];		/**< 桌面的扑克*/
	BYTE  m_byUserCardCount[PLAY_COUNT];	/**< 用户手上扑克数目*/
	WORD  m_wUserCard[PLAY_COUNT][45];		/**< 用户手上的扑克*/
	BYTE  m_bySequence[PLAY_COUNT];			/**< 出完牌顺序*/
	bool  m_bIsTrustee[PLAY_COUNT];         /**< 托管*/

	BYTE  m_byLastGameTimer;                /**< 最后一个游戏定时器ID  (0x10 ~ 0x20)*/
	DWORD m_dwLastGameStartTime;            /**< 最后一个游戏定时器启动时间*/
	int   m_iLastTimerTime;                 /**< 最后一个游戏定时器的设置时间(总时间)*/

	//本把保存数据
	__int64  m_iZhongZhu;   					/**< 本把当前总注额*/
	__int64  m_iQuYuZhu[8]; 					/**< 本把每个区域下的注额*/
	__int64  m_iMaxZhu[8];  					/**< 本把每个区域最大能下的注额*/
	__int64  m_iMax[8];							/**< 本把每个区域控制的最大下注额*/
	__int64  m_iPeiLv[8];   					/**< 每个区域的赔率（按本把开牌的结果算，赢区域为正，输区域为负）*/
	__int64  m_iZhuangFen;  					/**< 庄家的得分，本把*/
	__int64  m_iXianFen;						/**< 闲家的得分，本把*/
	__int64  m_iUserXiaZhuData[PLAY_COUNT][11]; /**< 玩家信息0-7玩家的下注，8玩家下的总注额，9玩家得分，10玩家的命中率，*/
	int  m_iQuYuJinBi[8][6];				/**< 每区域下各类（共6类）金币的个数*/
	int  m_iCurrentMessage;					/**< 当前处理的消息*/
	__int64  m_iAIMoney[8];						/**< 机器人的下注*/
	__int64  m_iNoteMoney[6];					/**< 筹码类型对应的金币数*/
	int  m_nPowerMoney;					    /**<放大倍数*/
	bool m_bIsJieSuan;						/**<是否已经结算2010-05-21*/

	//保存各玩家各局下注与否和下注后输赢与否, 0:为没有下注, 1为下注同时赢了, 2为下注但是输了, 3为下注同时玩家不输不赢
	int  m_iUserXiaZhuInfo[PLAY_COUNT][MAXCOUNT];	
	int	 m_iResultInfoArea[MAXCOUNT];			//保存历史开庄闭和情况的历史记录

	int  m_iZongTime;				/**< 总时间*/
    int  m_iSYTime;					/**< 剩余时间*/
	int  m_iTimeLabel;				//时钟标签							   
	int  m_iXiaZhuTime;				/**< 下注时间	*/		
	int  m_iKaiPaiTime;     	    /**< 开牌时间*/
	int  m_iFreeTime;				/**< 空闲时间*/
	int  m_iCtrTime;				/**<控制提前几秒不能下注的时间*/

	int  m_iNowNtStation;			/**< 当前庄家位置*/
	int  m_iLastNtStation;			/**< 上个庄家的位置*/
	bool m_bXiaZhuang;				/**< 当前庄家申请下庄*/

	int       m_iCardCount;				/**< 已经拿牌的总张数*/
	__int64   m_iWinQuYu[8];				/**< 游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和*/
									   
	bool m_bThreeCard;				/**< 庄或闲拿了3张牌*/
	int  m_iZPaiXing[5];    	    /**< 庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和*/
	int  m_iXPaiXing[5];    	    /**< 闲家牌型*/
	int  m_iWinner;					/**< 赢家1 庄，2闲，3和，本赢方*/
	int  m_iKaiPai;					/**< 本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，7和，8同点和*/

	int  m_iXiaZhuType[6];			/**< 各类金币的下注个数*/
	WORD  m_UserCard[2][3];			/**< 玩家的牌*/
	BYTE  m_CardCount[2];			/**< 庄闲双方的牌数*/
	__int64  m_iUserFen[PLAY_COUNT];	/**< 各个玩家本局的得分*/
	__int64  m_iUserMoney[PLAY_COUNT];	/**< 各个玩家本局应的金币*/
	BYTE m_byQuYuJinBi[8];			/**< 各个区域金币个数*/

	//30把保存数据
	int  m_iBaSHu;              	/**< 本局（30把一局）进行的把数（针对庄家）*/
	int  m_iResultInfo[MAXCOUNT][4];/**< 最近30把的开牌信息:庄闲的点数，结果：庄，闲和，赢牌型：*/
	int  m_iGameCount;          	/**< 已经进行的局数，只保存30局。*/

	int  m_iHaveGameCount;			//游戏进行了多少把

	bool  m_bIsSuperStation[PLAY_COUNT];          	/**< 记录超级玩家的位置。*/

	//长久保存数据
	int  m_iZhuangBaShu;      		/**< 庄家进行了几把*/
	__int64  m_iShangZhuangLimit; 		/**< 上庄需要的最少金币*/
	//int  m_iZhuangStation[4]; 	/**< 庄家列表总的庄家位置*/
	/// 庄家列表总的庄家位置 最后一名存诸队表总数
	int  m_iZhuangStation[PLAY_COUNT+1]; 	

	__int64  m_iNtWin;            		/**< 当前庄家赢的金币*/
	int  m_iMaxZhuang;				/**< 庄家一次最多做多少局，默认30局 */

	WORD wCardList[POKER_CARD_NUM*POKER_SET_NUM];  //牌列表

	int   m_iNowJuShu;				/**<本局进行的局数，进行到第几局（72把一局）*/

	RESULT_DATA  m_ResultData;		/**<72把的结果数据lym2010-03-17 */

	bool	m_bGamePlaying;			/**<lym2010-03-19记录游戏是否正在进行中 */
	bool	m_bIsStart;				//游戏是否已经运行过
	bool	m_bPrintEnding;			/**< 是否打印结局 */
	CTime	m_RecordTime;			/**< 记录当天游戏时间 */

	bool	m_bCanNote;				//是否能下注
	bool    m_IsAIStation[PLAY_COUNT];/**< 记录机器人的位置 */

	bool	m_bAIWinAndLostAutoCtrl;///机器人输赢控制：是否开启机器人输赢自动控制
	__int64		m_iAIWantWinMoney;		///机器人输赢控制：机器人要赢的钱
	__int64		m_iAIMaxWinInOne;		///机器人输赢控制：机器人平均每把最大赢钱数	（小于等于0时为不限）	

	int		m_iAICtrlCount;			///机器人输赢控制：当前机器人已经进行了自动控制的把数
	__int64		m_iAIHaveWinMoney;		///机器人输赢控制：机器人已经赢了多少钱
	//--------------add by DWJ--20121108----------------------------------
	bool	m_bIsFirstLoadIni;			//是否第一次读取配置文件 用于保存第一次启动日期，其后没增加一天m_iAIWantWinMoneyA1～A3区域要递增
	__int64 m_iAIWantWinMoneyA1;		/**<机器人输赢控制：机器人赢钱区域1  */
	__int64 m_iAIWantWinMoneyA2;		/**<机器人输赢控制：机器人赢钱区域2  */
	__int64 m_iAIWantWinMoneyA3;		/**<机器人输赢控制：机器人赢钱区域3  */
	int		m_iAIWinLuckyAtA1;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	int		m_iAIWinLuckyAtA2;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	int		m_iAIWinLuckyAtA3;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	int		m_iAIWinLuckyAtA4;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	__int64	m_iAddMoneyEveryDay;		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	__int64 m_iReSetAIHaveWinMoney;		/**<机器人输赢控制：重置机器人已经赢钱的数目  */
	int		m_iRecFroDayTime;			/**<记录服务器当前时间，用于判断是否过了24点，过了24点就在A1、A2、A3区域上递增m_iAddMoneyEveryDay  */
	int     m_iApplyNtAINum;		///想要坐庄的机器人数量（控制机器人上庄）
	int     m_iAINtZhuangCount;		///机器人坐在的把数（控制机器人上庄）

	///庄家抽水控制
	BYTE	m_byNtTax;	

	///当前房间最大能下的总注
	__int64		m_iMaxNote;	

	int   m_iRobotSZCount;                                          /**< 允许机器人上庄数量 */
	BOOL  m_bIsVipDouble;                                           /**< 玩家是否为vip用户且能双倍下注 */
	BOOL  m_bDoubleWin;												/**< 是否控制只要出对子庄闲都赢 */


	__int64	m_iUserOldMoney[PLAY_COUNT];			/* 玩家的带入金币 */

	bool	m_bStartGameWithOutNt;					/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 add xyh 2011-12-30*/

	bool    m_bShowAllResultInfo;                   ///是否显示所有的人的信息

	int     m_iLevelBase;                          ///平的倍数

#ifdef SUPER_VERSION
	WORD  m_wSuperCardList[8][54];				// 超级客户端
	int   m_iSuperCardCount[8];					//
	BYTE  m_bySuperStation;						// 超级客户端位置
	typedef struct tagSetCardStruct	            // 客户端到服务器
	{
		BYTE abyPai[8][54];
		int	aiPaiCount[8];
		tagSetCardStruct()
		{
			memset(this, 0, sizeof(tagSetCardStruct));
		}
	}S_SUPER_SET_CARD;
#endif

//自动读取配制文件相关操作
#ifdef AUTOLOADINI
private:

	void	AutoLoadIni(int iRoomID);		//自动加载配制文件 

	void	LoadModifiedIni();		//加载修改的配制文件 

	void	LoadModifiedIni(int iRoomID);	//根据房间加载修改的配制文件

#endif
	//修改奖池数据1(平台->游戏)
	/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
	/// @return 是否成功
	virtual bool SetRoomPond(bool	bAIWinAndLostAutoCtrl);

	//修改奖池数据2(平台->游戏)
	/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
	/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[]);

	//修改奖池数据3(平台->游戏)
	/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
	/// @return 是否成功
	virtual bool SetRoomPondEx(__int64	iReSetAIHaveWinMoney);

	//控制胜负
	/// 判断
	virtual BOOL Judge();
	/// 胜
	virtual BOOL JudgeWiner();
	/// 输
	virtual BOOL JudgeLoser();
	//GRM函数
	//更新奖池数据(游戏->平台)
	/// @return 机器人赢钱数
	virtual void UpDataRoomPond(__int64 iAIHaveWinMoney);

};
#endif // !defined(_SERVERMANAGE_H__INCLUDED_)