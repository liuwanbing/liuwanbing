#pragma once

#ifndef GAMEDESK_H_FILE
#define GAMEDESK_H_FILE

#include "UserInfo.h"
#include "AFCSocket.h"
#include "GameMainManage.h"
#include "../common/PropMessage.h"
///#include "GameRoomLogonDT.h"

///游戏结束标志
#define GFF_FORCE_FINISH			0										///< 强行解除
#define GFF_SAFE_FINISH				1										///< 解除游戏

///开始模式
#define FULL_BEGIN					0x00									///< 满人才开始
#define ALL_ARGEE					0x01									///< 所有人同意就开始
#define SYMMETRY_ARGEE				0x02									///< 对称同意开始
#define FOUR_SIX_ARGEE				0x03									///< 6个座位中只要有4个人同意开始


///类说明
class CGameMainManage;
struct MatchInfoStruct;
struct UpdateUserInfo_t;

/// CGameDesk类，游戏桌子
/// 纯虚类，游戏服务器程序由本类派生
/// 提供游戏中需要用到的关于数据库和用户数据操作的接口
/// 
class AFX_EXT_CLASS CGameDesk
{
	///辅助信息
private:
	bool							m_bLock;								///< 是否上锁
	UINT							m_uDeskType;							///< 游戏桌类型，主要为添加百家乐 add by wlr 20090714
	bool                            m_bIsVirtualLock;                       ///< 是否为虚拟锁桌  yjj 090325
	char							m_szLockPass[61];						///< 上锁密码
	long int						m_dwOwnerUserID;						///< 台主 ID
	long							m_dwZhuangjiaUserID;					///< 百家乐类游戏 庄家ID  add by lxl 2010-11-5
 	
	bool							m_bConnect[MAX_PEOPLE];					///< 是否连接

	///游戏信息
private:
	bool							m_bPlayGame;							///< 游戏是否开始标志
	BYTE							m_bEnableWatch;							///< 旁观标志
	long int						m_dwBeginTime;							///< 开始时间
	MatchInfoStruct					* m_pMatchInfo[MAX_PEOPLE];				///< 比赛信息

	///记录信息
private:
	long int						m_dwTax;								///< 每局玩家缴税
	long int						m_dwGameUserID[MAX_PEOPLE];				///< 玩家标识
	int								m_dwScrPoint[MAX_PEOPLE];				///< 原来分数
	__int64							m_dwChangePoint[MAX_PEOPLE];			///< 更改分数
	__int64							m_dwChangeMoney[MAX_PEOPLE];			///< 更改金币
	__int64							m_dwTaxCom[MAX_PEOPLE];					///< 更改税目
	BYTE							m_byDoublePointEffect;					///< 双倍卡是否有效，（按位算）20081125

	///断线信息
private:
	bool							m_bCutGame[MAX_PEOPLE];					///< 是否断线
	BYTE							m_bBreakCount[MAX_PEOPLE];				///< 断线次数
	long int						m_uCutTime[MAX_PEOPLE];					///< 断线时间

	//视频信息
private:
	void							send_video_ip(BYTE bDeskStation);		//向客户端发送视频服务器地址信息
	bool							m_bhavevideo;							//是否有视频
	std::string						m_videoip;								//视频服务器IP地址
	int								m_videoport;							//视频服务器监听端口号
	int								m_audioport;							//视频服务器音频监听端口号
	///公共数据
protected:
	BYTE							m_bWaitTime;							///< 等待时间
	BYTE							m_bGameStation;							///< 游戏状态
	///BYTE                            m_bGameFinishCount;                     ///< game over count
	CRITICAL_SECTION				m_csLock_;								///< 关键段///百家乐 add by wlr 20090714

	///只读变量
public:
	
	BYTE							m_bDeskIndex;							///< 桌子号码
	BYTE							m_bMaxPeople;							///< 游戏人数
	const BYTE						m_bBeginMode;							///< 同意模式
	CPtrArray						m_WatchUserPtr;							///< 旁观用户信息指针
	CGameUserInfo					* m_pUserInfo[MAX_PEOPLE];				///< 用户信息指针，此数组索引不一定是玩家的座位索引。
	CGameMainManage					* m_pDataManage;						///< 数据管理指针
	int								m_DeskBasePoint;						///< 台子倍数（台主设定）
	CGameUserInfo					m_RoundBanker;							///每轮的庄家信息
	///函数定义

public:
	CTime							m_tBeginTime;							///游戏开始的时间
	CTime							m_tEndTime;								///游戏结束的时间

public:
	///构造函数
	explicit CGameDesk(BYTE bBeginMode);
	///析构函数
	virtual ~CGameDesk(void);

	///功能函数
public:
	///初始化函数
	///百家乐UINT uDeskType add by wlr 20090714
	bool Init(BYTE bDeskIndex, BYTE bMaxPeople, CGameMainManage * pDataManage, UINT	uDeskType);
	///初始化游戏逻辑
	virtual bool InitDeskGameStation(){return true;};
	///设置比赛信息
	bool SetMatchInfo(BYTE bDeskStation, MatchInfoStruct * pMatchInfo, bool bNotify);
	///用户断线离开
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	///用户断线重来
	virtual bool UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo);
	///百家乐 add by wlr 20090714
	BYTE FindStation(CGameUserInfo * pUserInfo);
	///内部函数 
	
	///百家乐类游戏 add by lxl 2010-11-5
	///判断是否是百家乐类游戏
	bool IsBJLGameType() const {return m_uDeskType == DESK_TYPE_BJL;};
	///设置当前桌子的庄家ID 
	void SetCurrentZhuangjia(long dwUserID) {m_dwZhuangjiaUserID = dwUserID;};
	///判断玩家是否是庄家 
	bool IsZhuangjia(long dwUserID) const {return m_dwZhuangjiaUserID == dwUserID && dwUserID != 0L;};

private:
	///清理断线资料
	bool CleanCutGameInfo();
protected:
	///清理指定断线玩家
	bool CleanCutGameInfo(BYTE bDeskStation);
protected:
	///记录游戏信息
	bool RecoderGameInfo(__int64 *ChangeMoney);

	///得到双倍积分卡有效信息
	BYTE GetDoublePointEffect(){return m_byDoublePointEffect;};

	///工具函数
public:
	///是否锁定
	bool IsLock() { return m_bLock; };
	///上锁
	void LockDesk() { m_bLock=true;};
	///解锁
	void UnlockDesk() { m_bLock=false;};

	///虚拟锁桌操作  yjj 090325
	bool IsVirtualLock() {return m_bIsVirtualLock;}
	void VirtualLockDesk() {m_bIsVirtualLock = true;}
	void UnVirtualLockDesk(){m_bIsVirtualLock = false;}


	///是否允许旁观
	
	bool IsEnableWatch(BYTE bDeskStation);
	///设置定时器
	bool SetTimer(UINT uTimerID, int uElapse);
	///得到桌子基础分
	int GetTableBasePoint() { return m_DeskBasePoint;};
	///
	void SetTableBasePoint(int BasePoint) {m_DeskBasePoint=BasePoint;};
	///台主
	long GetTableOwnerID(){return m_dwOwnerUserID;};
	///删除定时器
	bool KillTimer(UINT uTimerID);
	///获取游戏时间
	long int GetPlayTimeCount();
	/// 修改用户分数和钱币
	/// 该函数需要完成的功能
	/// 根据输入参数，计算出用户实际输赢的金币数，并将结果保存在m_dwChangeMoney成员中，同时写入数据库
	/// @param[in] dwPoint 用户积分数组，数组大小为m_bMaxPeople，本桌支持的最大人数，由调用者保证数组不越界
	/// @param[in] bCut 用户是否断线数组，数组大小为m_bMaxPeople，本桌支持的最大人数，由调用者保证数组不越界
	/// @param[in] nTaxIndex -1表示扣所以玩家的税，否则只扣该用户的钱
	/// 该函数会修改m_dwChangeMoney[]数组成员的值
	/// 游戏服务器程序调用完此函数的，可通过RecordGameInfo函数获取该值
	bool ChangeUserPointint64(__int64 *arPoint, bool *bCut, int nTaxIndex = -1);

	bool ChangeUserPointContest(__int64 *arPoint, bool *bCut, int nTaxIndex = -1);

	bool ChangeUserPointContest(int *arPoint, bool *bCut, int nTaxIndex = -1);

	bool ChangeUserPoint(int *arPoint, bool *bCut, int nTaxIndex = -1);
	/// 修改用户分数，过去的结算函数，因新函数生效而改名，废弃不用
	bool ChangeUserPoint_old(int *dwPoint, bool *Cut,int *dwMoney=NULL,bool IsCheckInMoney=false);

	///发送游戏状态
	bool SendGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser, void * pStationData, UINT uSize);
	///发送数据函数 （发送消息给游戏者）
	bool SendGameData(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	///发送旁观数据 （发送消息给旁观者）
	bool SendWatchData(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	///发送数据函数 （发送消息给游戏者）
	bool SendGameData(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	///发送旁观数据 （发送消息给旁观者）
	bool SendWatchData(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	///发送旁观数据 （发送消息给旁观者）
	bool SendWatchDataByID(UINT uSendSocketID, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	///用户级别
	int GetOrderClass(int dwPoint);
	///写分数到数据库
	void UpdateUserInfo(long dwUserID,int Point,int Money, int taxCom,bool bcut,long playTime,long logonTime);
	/// 获得桌子中有多少人,为排队机添加
	int GetDeskPlayerNum();
	/// 获得桌子中第一个没有人的座位索引,为排队机添加
	int GetDeskNoPlayerStation();
	/// 判断此桌中玩家的ip是否有相同的,为排队机添加
	bool IsDeskIpSame(CGameUserInfo * pUserInfo);
	/// 设置桌子中的玩家信息,为排队机添加
	void SetDeskUserInfo(CGameUserInfo * pUserInfo, int iDeskStation);
	/// 赠送游戏币,20把
	/// param void
	/// return void
	void PresentCoin();

	///特殊重载函数
public:
	///是否可以开始游戏
	virtual bool CanBeginGame();
	///是否可以断线
	virtual bool CanNetCut(BYTE bDeskStation);
	///用户坐到游戏桌
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	///用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	///旁观用户坐到游戏桌
	virtual BYTE WatchUserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	///旁观用户离开游戏桌
	virtual BYTE WatchUserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	///用户同意开始
	virtual bool UserAgreeGame(BYTE bDeskStation);
	///框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	///旁观参数变化
	virtual bool OnWatchSetChange(BYTE bDeskStation, long int dwTargetUserID, bool bEnableWatch) { return true; }

		// 返回道具数量
	virtual int PropInfo(const TPropChange& propChange) {return 0;};
public:
	/// 更改玩家的金钱(主要用于指导费)，使用需慎重 pp 2010-8-26
	/// @param pbDeskStation为要改变积分（或金币）的座位数组
	/// @param arPoint为要改变的积分（或金币）的数组
	/// @param nCount为要改变积分的人数
	void UpdateUserPoint(BYTE *pbDeskStation,__int64 *arPoint,int nCount);              
	/// 更新所有玩家的数据库资料
	/// zxd 于 20100125创建
	/// 此函数只调用一次存储过程，从而更新所有玩家的数据，不再是一次一个玩家调用
	/// @param pUpdateUserInfo 实时new出的多个用户数据指针
	/// @param nTotalPlayerCount 本次更新玩家的个数
	/// @return 无返回值
	void UpdateAllUserInfo( UpdateUserInfo_t *pUpdateUserInfo, int nCount, int iroundtime = 0);
	/// 强制玩家离线函数
	/// 无论是否在游戏中，都强制玩家离线，调用者须自行处理该玩家离线后的游戏进程
	/// 清空本房间中的该玩家信息，调用结束后，该玩家离开本房间，同时m_pUserInfo[bDeskStation]会被清成NULL
	/// 本函数与UserLeftDesk的区别在于，本函数是服务器根据玩家身上的金币数和其它在服务器端判断的条件，主动令玩家退出房间。而
	/// UserLeftDesk函数则是由客户端发出离桌消息到服务器后，再由平台调用的函数，具体游戏根据自身的规则，作相应处理，根据游戏
	/// 进行状态，不一定会令玩家退出本局游戏或房间
	/// 
	/// @param bDeskStation 玩家在桌子中的位置号
	/// @return 如果bDeskStation大于本房间一桌最大人数，或m_pUserInfo[bDeskStation]已经是空值NULL，则返回false，否则返回true
	bool MakeUserOffLine(BYTE bDeskStation);
	///常规重载函数
public:
	///游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	///游戏结束
	/// @param bDeskStation 异常结束时，引起结束的玩家在桌子中的位置号，正常结束时无意义
	/// @param bCloseFlag  异常结束游戏时的标志，正常结束时为0
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);


	///
	virtual bool GameFinishContest(BYTE bDeskStation, BYTE bCloseFlag);


	///定时器消息
	virtual bool OnTimer(UINT uTimerID);
	///判断游戏桌上的某个玩家是否开始游戏了
	virtual bool IsPlayGame(BYTE bDeskStation) { return m_bPlayGame; }
	///判断此游戏桌是否开始游戏
	virtual BOOL IsPlayingByGameStation(){ return m_bPlayGame; }
	///游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);

	///必须重载函数
protected:
	///获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)=0;
	///重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation)=0;
	///立即写数据库
	virtual BOOL UpdateUserDB(LONG dwUserID,int dwMoney = 0,int dwPoint = 0);
public:
	///道俱
	virtual bool UserUseProp(BYTE bsendDeskStation,BYTE brecvDeskStation,_TAG_PROP_MOIVE_USE * propMoive);
	///增加魅力值
	//rongqiufen
	//修改指定用户的道具信息
	virtual int PropChange(const TPropChange& propChange);
};

#endif