/// ServerManage.h: Headers and defines for CServerGameDesk
///


#if !defined(_SERVERMANAGE_H__INCLUDED_)
#define _SERVERMANAGE_H__INCLUDED_

#include "StdAfx.h"
#include "../GameMessage/UpGradeLogic.h"
#include <deque>

///游戏结束标志定义
#define GF_NORMAL					10			   /**<  游戏正常结束   */
#define GF_SAFE						11			   /**<  游戏安全结束   */


/*-------------------------------------------------------------------------*/
#define ID_TIMER_SEND_CARD		30				//开始发牌
#define ID_TIMER_SHOW_RESULT	31				//发牌结束显示结算		
#define ID_TIMER_SHOW_FINISH	32				//结算显示结束		
#define ID_TIMER_GAME_BEGIN		33				//游戏开始	
/*-------------------------------------------------------------------------*/


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
#define GF_Z_DAN				8			   /**<  庄单   */
#define GF_Z_SHUANG				9			   /**<  庄双   */
#define GF_X_DAN				10			   /**<	 闲单   */
#define GF_X_SHUANG				11			   /**<	 闲双   */
/*----------------------------------------------------------------------------*/
//6个筹码值
const	int	G_iChouMaValues[6] = {100,1000,10000,100000,1000000,10000000};
/*----------------------------------------------------------------------------*/
///游戏服务器处理类
class CServerGameDesk :public CGameDesk
{
public:
	CServerGameDesk(void);
public:
	~CServerGameDesk(void);

	/// 加载配置文件
	BOOL LoadIni();
	BOOL LoadExtIni(int iRoomID);
	/// 清除所有定时器
	void KillAllTimer(void);	
public:
	///游戏开始
	virtual bool	GameBegin(BYTE bBeginFlag);
	///游戏结束
	virtual bool	GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	///判断是否正在游戏
	virtual bool	IsPlayGame(BYTE bDeskStation);
	///游戏数据包处理函数
	virtual bool	HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 框架消息处理函数
	virtual bool	HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	///用户离开游戏桌
	virtual BYTE	UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//用来改变用户断线条件的函数
	virtual bool	CanNetCut(BYTE bDeskStation);
	///用户断线离开
	virtual bool	UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
public:
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	///重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);
	///初始化游戏逻辑
	virtual bool InitDeskGameStation();



	///整理数组中的数据，把无用的数组元素清空
	void MoveData(int type);

	///处理玩家的下注
	void	HanleXiaZhu(BYTE Station,void * pData, UINT uSize);
	///处理玩家的上庄消息
	void	HanleSzhuang(BYTE Station,void * pData, UINT uSize);
	///处理玩家的下庄消息
	void	HanleXzhuang(BYTE Station,void * pData, UINT uSize);


	///超级客户端设置机器人控制参数
	void SetAICtrl(BYTE Station,void * pData, UINT uSize);

	
	///本局算分
	void CountFen();
	//获取一张牌
	void	GetACardValue(BYTE &byCard);
	///计算牌值
	void	CountPaiZhi(int count);
	///控制开庄开闲
	void ConTrolZX();
	///发牌
	void SendCard(int iIndex=1);

	///闲家拿了第三张牌情况下，庄家是要拿牌lym0511
	///pai:闲家的第三张牌值
	bool	IsGetTheThird();

	///获取第三张牌
	bool	GetTheThird();

	///获取赢钱的区域
	void	GetWinAres();


	///获取赢钱的区域
	void GetWin();

	///同点检测
	///@param num,检测前几张牌
	bool	CheckTongDian(int num);

	///清理已计算的牌点数据lym0511
	void ClearWinData(bool all);


	///交换庄闲的前两张牌lym0511
	void ChangeCard();

	///一局结束并过了空闲时间后检测是否能够继续开始游戏
	bool CheckCanBegin();

	///庄家下庄后获取下一个庄家列表中的庄家
	bool GetNextNT();

	///通过全部举手的方式开始游戏
	bool StartGame();



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
	void AuthPermissions(BYTE bDeskStation); 
	//判断是否超端玩家
	bool IsSuperUser(BYTE byDeskStation);

	/// 获取返还分
	int GetMeFanFen(int station);



	///描述：机器人输赢自动控制
	void IAWinAutoCtrl();
	//-------ADD-BY-DWJ-20121108--------------------------------
	void CheckNewDayReSetIni(CINIFile *f, CString *key);//检查是否已经新的一天了，需要重新设定机器人控制配置参数

	void GetAIContrlSetFromIni(int iRoomID);			/**从配置文件中读取机器人控制设置的参数值*/

	///描述：计算当前牌机器人的输赢钱
	int CountAIWinMoney();

	///描述：获取一个满足申请上庄条件的机器人位置
	int GetAnAIApplyStation();

	//发送开牌消息
	void	OnSendOpenCard();
	//游戏结算
	void	OnSendGameResult();
	
	//发送空闲消息
	void OnSenFree();
	//游戏重新开始
	void RestartGame();
	///描述：获取每个区域最大下注
	void GetQuYuCanNote();


protected:
	///统计机器人上庄数量
	int CountRobortNum();

	//清理庄家
	void ClearNt();
protected:
	__int64  m_iMax[BET_ARES];							/**< 本把每个区域控制的最大下注额*/



	__int64  m_iAIMoney[BET_ARES];						/**< 机器人的下注*/



			  


	

	
	int  m_iWinner;					/**< 赢家1 庄，2闲，3和，本赢方*/
	int  m_iKaiPai;					/**< 本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，7和，8同点和*/





	bool	m_bCanNote;				//是否能下注
	bool    m_IsAIStation[PLAY_COUNT];/**< 记录机器人的位置 */

	bool	m_bAIWinAndLostAutoCtrl;///机器人输赢控制：是否开启机器人输赢自动控制
	__int64		m_iAIWantWinMoney;		///机器人输赢控制：机器人要赢的钱
	__int64		m_iAIMaxWinInOne;		///机器人输赢控制：机器人平均每把最大赢钱数	（小于等于0时为不限）	

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

	int     m_iLevelBase;                          ///平的倍数

private:
	CUpGradeGameLogic		m_Logic;     						//游戏逻辑
private:	//需要配置的变量
	int  m_iXiaZhuTime;				/**< 下注时间	*/		
	int  m_iKaiPaiTime;     	    /**< 开牌时间*/
	int	 m_iShowResult;				//显示结算框时间
	int  m_iFreeTime;				/**< 空闲时间*/

	int  m_iMaxZhuang;				/**< 庄家一次最多做多少局，默认30局 */

	__int64  m_iShangZhuangLimit; 		/**< 上庄需要的最少金币*/
private:	//一局结束后需要重置的变量
	bool			m_bGamePlaying;					/*记录游戏是否正在进行中 */
	BYTE			m_iTotalCard[52];					//总的牌
	bool			m_bThreeCard;						/**< 庄或闲拿了3张牌*/

	BYTE			m_byUserCard[2][3];					//庄闲的牌数据0-闲家 1-庄家

	int				m_iZPaiXing[5];    					/**< 庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和*/
	int				m_iXPaiXing[5];    					/**< 闲家牌型*/

	int				m_iWinQuYu[BET_ARES];				/**< 游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和，8庄单 9庄双 10闲单 11闲双*/

	__int64			m_i64MaxZhu[BET_ARES];  				/**< 本把每个区域最大能下的注额*/
	__int64			m_i64ZhongZhu;   					/**< 本把当前总注额*/
	__int64			m_iQuYuZhu[BET_ARES]; 				/**< 本把每个区域下的注额*/
	__int64			m_iUserXiaZhuData[PLAY_COUNT][BET_ARES+1]; /**< 玩家信息0-11玩家的下注，BET_ARES玩家下的总注额，9玩家得分，10玩家的命中率，*/

	__int64			m_i64UserFen[PLAY_COUNT];			/**< 各个玩家本局的得分*/

	int				m_iUserPlayCount[PLAY_COUNT][2];		//对应座位的玩家玩了多少局 0-表示玩了多少盘 1-表示赢了多少盘

	bool			m_bXiaZhuang;				/**< 当前庄家申请下庄*/		


private:
	deque <LuziData> m_DqLuziData;							//路子数据
	__int64			m_i64UserWin[PLAY_COUNT];				//玩家的输赢情况
	__int64			m_i64NtWin;            					/**< 当前庄家赢的金币*/
	int				m_iNowNtStation;						/**< 当前庄家位置*/
	int				m_iZhuangBaShu;      					/**< 庄家进行了几把*/
	int				m_iZhuangStation[PLAY_COUNT+1]; 		/// 庄家列表总的庄家位置 最后一名存诸队表总数


	vector	<long int>	m_vtSuperUserID;
};
#endif 