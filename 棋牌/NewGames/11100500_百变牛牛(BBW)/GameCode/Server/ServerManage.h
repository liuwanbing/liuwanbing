// ServerManage.h: Headers and defines for CServerGameDesk
//////////////////////////////////////////////////////////////////////
#if !defined(_SERVERMANAGE_H__INCLUDED_)
#define _SERVERMANAGE_H__INCLUDED_

#include "StdAfx.h"
#include "../GameMessage/UpGradeLogic.h"
#include <vector>
#include <deque>

//游戏结束标志定义
#define GF_NORMAL					10				// 游戏正常结束
#define GF_SAFE						11				// 游戏安全结束

#define ID_GAME_TIMER_FALG_START    0x10			// 能获取最后剩余时间的定时器标志
#define ID_GAME_TIMER_FALG_END      0x20			// 能获取最后剩余时间的定时器标志

#define ID_TIMER_SEND_CARD          0x10			// 获取剩余多少的定时器在0x10~0x20范围内，只需要定义后面一个字节
#define ID_TIMER_OUT_CARD           0x11			// 出牌定时器ID
#define ID_TIMER_GAME				0x12			// 游戏时间

#define ID_TIMER_SEND_HAVETHING     0x21			// 有事要走

#define TIMER_REMAIND_COUNT		30		//剩余时间倒计时
#define ID_TIMER_XIA_ZHU       31           // 下注
#define ID_TIMER_KAI_PAI       32           // 开牌
#define ID_TIMER_SHOW_RESULT   33           // 显示结算框时间
#define ID_TIMER_FREE_TIME     34           // 空闲时间
/*--------------------------------------------------------------------------------*/
// 六个筹码的值
static __int64	G_i64ChoumaValue[6]={100,1000,10000,100000,1000000,10000000};
/*--------------------------------------------------------------------------------*/
//路子信息结构体
struct	GameLuziInfo
{
	int		iResultInfo[MAX_NOTE_COUNT];	//0-天 1-地 2-玄 3-黄
	GameLuziInfo()
	{
		memset(this,0,sizeof(GameLuziInfo));
	}
};
/*--------------------------------------------------------------------------------*/
///服务器游戏桌子
class CServerGameDesk :public CGameDesk
{
protected:
	CPokerGameLogic				m_PokerGameLogic;//牌类游戏逻辑

private: //可配置的变量
	int							m_iXiaZhuTime;		//下注时间	
	int							m_iKaiPaiTime;		//开牌时间     	    
	int							m_iFreeTime;		//空闲时间	
	__int64						m_iShangZhuangLimit;//上庄需要的最少金币 
	int							m_iMaxZhuang;		//庄家一次最多做多少局，默认30局
	bool						m_bCompareKind ;	//游戏中是否比较花色 1 比较花色 ， 0 不比较花色
	bool						m_bHaveKing ;		//是否要大小王
	BOOL						m_bNtTax;			//是否只扣庄家税
	int							m_iRobotSZCount;	//允许机器人上庄个数(0表示不限制)
	__int64						m_iMaxNote;			//玩家最大下注数
	__int64						m_iRectNoteMax[MAX_NOTE_COUNT];//四个区域下注限制(0-不限制)
	BYTE						m_bCardShapeBase[11];//0-10代表无牛到牛牛间的倍率
	/*--------------------------机器人奖池控制参数设置----------------------------------*/
	bool						m_bAIWinAndLostAutoCtrl;///机器人输赢控制：是否开启机器人输赢自动控制
	int							m_iAIWinLuckyAtA1;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	int							m_iAIWinLuckyAtA2;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	int							m_iAIWinLuckyAtA3;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	int							m_iAIWinLuckyAtA4;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	__int64						m_iAIWantWinMoneyA1;		/**<机器人输赢控制：机器人赢钱区域1  */
	__int64						m_iAIWantWinMoneyA2;		/**<机器人输赢控制：机器人赢钱区域2  */
	__int64						m_iAIWantWinMoneyA3;		/**<机器人输赢控制：机器人赢钱区域3  */
	__int64						m_iReSetAIHaveWinMoney;		/**<机器人输赢控制：重置机器人已经赢钱的数目  */
	__int64						m_iAIHaveWinMoney;			///机器人输赢控制：机器人已经赢了多少钱


private:
	vector	<long>				m_vlSuperUserId;			//超端ID容器
	int							m_iRemaindTime;				//各阶段的剩余时间
private:
	int							m_iNowNtStation;	//当前庄家位置	
	bool						m_bXiaZhuang;		//当前庄家申请下庄	
	int							m_iZhuangBaShu;		//庄家进行了几把  
	int							m_iGameCount;		//游戏次数 72把为一局                        
	int							m_iGameBeen;		//游戏局数 

	__int64						m_iNtWin;//当前庄家赢的金币 

	BYTE						m_UserCard[5][5];	//玩家的牌	
	BYTE						m_CardCount[5];//庄闲双方的牌数	
	BYTE						m_UserCardTotal[54];//总的牌	
	deque	<GameLuziInfo>		m_dqResultInfo;		//路子数据
	int							m_iResultInfo[GAME_SAVE_COUNT][MAX_NOTE_COUNT];//最近X把的开牌信息 	0123-天地玄黄

	TagSuperSet					m_TSuperSet;	//超端控制结果


	



	bool						m_bIsStart;//游戏是否可以开始

           
         

	__int64						m_iZhongZhu;//本把当前总注额   					
	__int64						m_iQuYuZhu[MAX_NOTE_COUNT];//本把每个区域下的注额 
	__int64						m_iQuYuZhuTrue[MAX_NOTE_COUNT];//本把真实每个区域下的注额 
	__int64						m_iUserXiaZhuData[PLAY_COUNT][MAX_NOTE_COUNT+1];//每个玩家信息0-3玩家的下注，4玩家下的总注 
	__int64						m_iZhuangFen;//庄家的得分，本把  					
	__int64						m_i64Max;//本局最大下注                          
	__int64						m_iXianFen;//本把闲家的得分						
	

	

									   
						
			
		
	
	int							m_iComputePoint[MAX_NOTE_COUNT];//各玩家的输赢赔率   各闲家位与庄家位比分只计闲位0-3地天地玄黄         
			
	__int64						m_iUserFen[PLAY_COUNT];//各个玩家本局的得分	
	__int64						m_iUserMoney[PLAY_COUNT];//各个玩家本局应的金币	

	
	
	  	              	
	
			
	//设置一个二维数组，第一列表示抢庄号,第二列表示是否抢庄;
	short int					m_idzhuangstation[PLAY_COUNT+1][2];
	           		
	
	

	
	
	BOOL						m_bIsControlWin;//超端控制中

	
	



public:
	CServerGameDesk(void);
public:
	~CServerGameDesk(void);
	//加载服务器配置文件
	BOOL	LoadIni();
	/// 加载服务器配置文件
	BOOL	LoadExtIni(int iRoomID);
	/**从配置文件中读取机器人控制设置的参数值*/
	void	GetAIContrlSetFromIni(int iRoomID);			

	
	//删除定时器
	bool KillGameTimer(BYTE byTimerID);
	//清除所有定时器
	void KillAllTimer(void);

	//初始化游戏逻辑
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
	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);
	/// 定时器消息
	virtual bool OnGameTimer(BYTE byTimerID);
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	///重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);


protected:
	//启动一秒倒计时计时器 用于计算各阶段还剩多少时间
	void	SetTimeRemaindCount(int  iTime);
	//整理数组中的数据，把无用的数组元素清空
	void MoveData(int type);
	//整理上装列表，按照抢庄顺序排列发送出去
	void ResetZhuangStation();
	///lym0512处理玩家的下注
	void HandleXiaZhu(BYTE Station,void * pData, UINT uSize);
	///lym0512处理玩家的上下庄消息
	void HanleSXzhuang(BYTE Station,void * pData, UINT uSize);
	///本局算分lym0512
	void CountFen();
	//收到超端控制消息
	void	OnHandleSuperUserSet(BYTE bDeskStation,void * pData, UINT uSize);

	///控制开庄开闲zht22091120
	void ControlWin(BYTE bDeskStation, BYTE bGameStation, BYTE BGameWin[]);
	///发牌lym0512
	void	SendCard();	
	///获取赢钱的区域lym0513
	void GetWin();
	///清理已计算的牌点数据lym0511
	void ClearWinData(bool all);
	//交换牌
	void ChangeCard(BYTE bDestStation,BYTE bWinStation);

	/*1 得到输赢玩家 zht
	* @param [in] win 得到输或者赢，1为得到赢家，非1为输家
	* @return 得到赢牌或输牌位置
	*/ 
	BYTE GetWiner(int win);
	///一局结束并过了空闲时间后检测是否能够继续开始游戏lym0512
	bool CheckCanBegin();
	///庄家下庄后获取下一个庄家列表中的庄家lym0512
	bool GetNextNT();
	///通过全部举手的方式开始游戏lym0512
	bool StartGame();
	///记录本局的成绩数据ym0512
	void RecordData();
	//初始化服务器数据数据ym0514
	///初始化大部分数据，有些数据不能初始化的：当前庄家进行了几局，前30局的数据，
	void ServerInit();
	///lym重载游戏能否开始的条件，防止无庄的假开始
	bool CanBeginGame();
	///日志记录接口
	void  SrvTestlog(TCHAR *pMessage);
	//得到玩家是否可以控制输赢
	bool CanControlWin(BYTE bDeskStation);
	//是否超端玩家
	bool	IsSuperUser(BYTE bDeskStation);
	///计算各家分数
	int ComputePoint(BYTE DeskStation);
	
	//清理庄家
	void ClearNt();

	//超端控制
	void	SuperUserControl();
	//描述：机器人输赢自动控制
	void IAWinAutoCtrl();
	//描述：计算当前牌机器人的输赢钱
	int CountAIWinMoney();
	BOOL  CanDouble(BYTE bDeskStation);
	//发送空闲消息
	void OnSendFree();
	//发送开牌消息
	void OnSendKaiPai();

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
#endif 