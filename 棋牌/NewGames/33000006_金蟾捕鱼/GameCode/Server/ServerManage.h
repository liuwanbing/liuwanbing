#pragma once

#include "../../../../sdk/include/ServerCommon/GameDesk.h"


//定时器 ID
#define TIME_MY_TIMER				20				//公用定时器，只有这个定时器是实际定时器，其余都是从这个定时器经过判断来的
#define MY_ELAPSE					250				// 定时器间隔
#define TIMER_COUNT					11				// 用到的定时器数目

/*---------------------------------------------------------------------*/
#define FISH_ALIVE_TIME				50*1000								//存活时间
/*---------------------------------------------------------------------*/
// 下列为游戏中使用到的定时器，由上面的TIME_MY_TIMER计数进行统一处理
#define	TIME_MIN					30				//最小计时器ID
#define TIME_BUILD_TRACE			31				//生成鱼轨迹计时器
#define	TIME_CREAD_FISH				32				//生成鱼儿计时器
#define	TIME_CLEAR_TRACE			33				//鱼儿销毁计时器
#define	TIME_CHANGE_SCENE			34				//切换场景计时器
#define	TIME_REGULAR_FISH			35				//鱼阵计时器

#define	TIME_SUPER_PAO_0			40				//0号位置超级炮计时器
#define	TIME_SUPER_PAO_1			41				//1号位置超级炮计时器
#define	TIME_SUPER_PAO_2			42				//2号位置超级炮计时器
#define	TIME_SUPER_PAO_3			43				//3号位置超级炮计时器
#define	TIME_SUPER_PAO_4			44				//4号位置超级炮计时器
#define	TIME_SUPER_PAO_5			45				//5号位置超级炮计时器



#define	TIME_MAX					50				//最大计时器ID
/*---------------------------------------------------------------------*/

//游戏结束标志定义
#define GF_NORMAL					10				//游戏正常结束
#define GF_SALE						11				//游戏安全结束

/*---------------------------------------------------------------------*/
static	const int G_iFishScore[26]={2,2,3,4,5,6,7,8,9,12,10,15,18,20,25,30,40,100,40,400,300,120,200,150,80,800};
/*---------------------------------------------------------------------*/
//奖池控制参数
//奖池等级
static	__int64		G_i64RewardLevel[3] = {100000,1000000,1000000};
//奖池等级对应的概率值
static	float		G_fRewardLevelProbability[3]={0,0,0};
//当前奖池的金额
static	__int64		G_i64CurrPoolMoney = 0;
//上一次的奖池金额
static	__int64		G_i64PrePoolMoney = 0;

/*---------------------------------------------------------------------*/


// 用来方便地加锁和解锁的类
class BZWLockForCriticalSection
{
	CRITICAL_SECTION *m_pSection;
	BZWLockForCriticalSection();
public:
	BZWLockForCriticalSection(CRITICAL_SECTION *pSection)
	{
		m_pSection = pSection;
		EnterCriticalSection(m_pSection);
	}
	~BZWLockForCriticalSection()
	{
		LeaveCriticalSection(m_pSection);
	}
};


//游戏桌类
class CServerGameDesk : public CGameDesk
{
	CRITICAL_SECTION m_csForIPCMessage;
private:		/*可配置项*/
	int			m_iCreateFishTime;	//生成鱼儿时间
	int			m_iChangeScene;		//切换场景时间
	int			m_iFireBase;		//基础炮火倍数
	int			m_iMaxFireBase;		//最大炮火倍数
	int			m_iMinFire;			//最小炮火倍数
	int         m_fishShoot[26][3];	//击杀枪数
	int			m_iFishBase[26];	//鱼的赔率
	int         m_iBiliDuihuan;		//兑换比例

	int			m_iUserPercent[PLAY_COUNT];	//玩家概率控制

	vector	<long>	m_lvSuperUserID;	//超级用户列表
	vector	<long>	m_lvBlackListID;	//黑名单列表

private:		
	bool		m_bHaveHitFlag;					//是否存在机器人击中鱼儿的判断
	BYTE		m_byHitDesk;						//机器人击中鱼儿的判断的位置号
	bool		m_bIsPlaying;					//游戏是否已经开始
	int         m_iBgindex;						//背景索引
	int         m_iRegfishcount;				//规则鱼个数

	int			m_iUserFireBase[PLAY_COUNT];	//玩家的炮火倍数
	int			m_iUserScore[PLAY_COUNT];		//玩家上分数

	__int64		m_i64UserMoney[PLAY_COUNT];		//玩家身上的金币数
	__int64		m_i64UserAllScore[PLAY_COUNT];	//玩家总上分

	int			m_iUserShootCount[PLAY_COUNT];	//玩家发射子弹的个数
	BulletInfo  m_UserShoot[PLAY_COUNT][100];	//玩家子弹信息
	int			m_SendTime[PLAY_COUNT];			//玩家发射子弹间隔


	bool		m_bUserIsSuperPao[PLAY_COUNT];	//是否超级炮弹

private:
	FishTrace                       m_fishtrace[250][5];//对应鱼儿的坐标路线，5个坐标和角度
	DWORD							m_dwFishID;	//鱼群标识





public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	//游戏开始
	virtual bool	GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool	GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool	IsPlayGame(BYTE bDeskStation);
	///判断此游戏桌是否开始游戏
	virtual BOOL	IsPlayingByGameStation();
	//游戏数据包处理函数
	virtual bool	HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//框架消息处理函数
	virtual bool	HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//用户离开游戏桌
	virtual BYTE	UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	///用户坐到游戏桌
	virtual BYTE	UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	//用户断线
	virtual bool	UserNetCut(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//初始化桌子信息
	virtual bool	InitDeskGameStation();
	//获取游戏状态信息
	virtual bool	OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool	ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool	OnTimer(UINT uTimerID);

private:
	//玩家上下分
	void	OnHandleUserAddOrMoveBullet(BYTE byStation,void * pData, UINT uSize);
	//玩家发射子弹
	void	OnHandleUserShot(BYTE byStation,void * pData, UINT uSize);
	//玩家加减炮
	void	OnHandleUserAddFire(BYTE byStation,void * pData, UINT uSize);
	//击中鱼儿
	void	OnHandleHitFish(BYTE byStation,void * pData, UINT uSize);
	bool	OnSubHitFish(BYTE byStation, int iFishID, int iBulletID);
	//是否概率性打死鱼
	bool	IsProbalityForDead(int iFishIndex);

	//捕获大鱼发送广播
	void	SendCatchBroadCast(int iFishType, CMD_S_CaptureFish TCaptureFish);
	

private:
	//开始启动游戏
	void	StartGame();
	//生成鱼
	void	CreadFish();
	//生成规则小鱼
	void	CreatRegSmalFish();
	//生成小鱼
	void	CreatSmalFish();
	//生成鱼儿1
	void	CreateFishTraceOne();
	//生成鱼儿2
	void	CreateFishTraceTwo();
	//生成鱼儿3
	void	CreateFishTraceThree();
	//生成鱼儿4
	void	CreateFishTraceFour();
	//产生鱼儿ID标识
	DWORD	GetNewFishID();
	//清除鱼儿
	void	ClearFishTrace();
	//切换场景
	void	ChangeScence();
	//鱼阵阵型一
	void	RegFishone();
	//鱼阵阵型二
	void	RegFishtwo();
	//鱼阵阵型三
	void	RegFishthree();
	//取消玩家的超级炮
	void	CanCelUserSuperPao(BYTE	byStation);

	//处理函数
private:
	//清空该位置的数据
	void	IniUserData(BYTE byStation);
	//读取静态变量
	BOOL	LoadIni();	
	//根据房间ID加载底注和上限
	BOOL	LoadExtIni(int iRoomID);
	//是否超级用户
	bool	IsSuperUser(BYTE byStation);
	//是否黑名单用户
	bool	IsBlackListUser(BYTE byStation);

	//保存奖池记录
	void	RecordPool();

	//判断是否全部都是机器人
	bool	IsAllRobot();

	//用户设置游戏

};

/******************************************************************************************************/
