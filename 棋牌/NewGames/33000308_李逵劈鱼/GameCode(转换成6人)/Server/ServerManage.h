#pragma once

#include "StdAfx.h"

#include <vector>

struct FishTraceInfo //鱼轨迹信息结构体
{
	FishKind fish_kind;
	int fish_id;
	DWORD build_tick;
};

struct SweepFishInfo //鱼轨迹信息结构体
{
	FishKind fish_kind;
	int fish_id;
	BulletKind bullet_kind;
	int bullet_mulriple;
};

struct ServerBulletInfo //服务端子弹信息结构体
{
	BulletKind bullet_kind;
	int bullet_id;
	int bullet_mulriple;
};

struct SuperManInfo//超端用户数据
{
	int iUserID;//用户ID
	int iCellScore;//单位分数
};

static std::vector<DWORD> g_balck_list_;
static std::vector<DWORD> g_white_list_;

struct Fish20Config {
	DWORD game_id;
	int catch_count;
	double catch_probability;
};
static std::vector<Fish20Config> g_fish20_config_;

const int kBulletIonTimer=							20;

const int kLockTimer		=					31;
const int kClearTraceTimer				=		32;		//清理所有鱼计时器
const int kBuildSmallFishTraceTimer		=		33;		//小鱼计时器
const int kBuildMediumFishTraceTimer	=		34;		//中等鱼计时器
const int kBuildFish18TraceTimer		=		35;		//18号鱼计时器
const int kBuildFish19TraceTimer		=		36;		//19号鱼计时器
const int kBuildFish20TraceTimer		=		37;		//20号鱼计时器
const int kBuildFishLKTraceTimer		=		38;		//李逵计时器
const int kBuildFishBombTraceTimer		=		39;		//炸弹鱼计时器
const int kBuildFishSuperBombTraceTimer	=		40;		//超级炸弹鱼计时器
const int kBuildFishLockBombTraceTimer	=		41;		//锁定炸弹鱼计时器
const int kBuildFishSanTraceTimer		=		42;
const int kBuildFishSiTraceTimer		=		43;
const int kBuildFishKingTraceTimer		=		44;
const int kSwitchSceneTimer				=		45;
const int kSceneEndTimer				=		46;
const int kLKScoreTimer					=		47;

const int kClearTraceElasped			=	60;
//const int kBuildSmallFishTraceElasped			=	4;//小鱼出现时长
//const int kBuildMediumFishTraceElasped		=	5;//中等鱼出现时长
//const int kBuildFish18TraceElasped			=	33;//18号鱼出现时长
//const int kBuildFish19TraceElasped			=	43;//19号鱼出现时长
//const int kBuildFish20TraceElasped			=	41;//20号鱼出现时长
//const int kBuildFishLKTraceElasped			=	58;//李逵出现时长
//const int kBuildFishBombTraceElasped			=	68;//炸弹鱼出现时长
//const int kBuildFishSuperBombTraceElasped		=	77;//超级炸弹鱼出现时长
//const int kBuildFishLockBombTraceElasped		=	64 + 10;//定屏炸弹鱼出现时长
//const int kBuildFishSanTraceElasped			=	80 + 28;//大三元鱼出现时长
//const int kBuildFishSiTraceElasped			=	90 + 17;//大四喜鱼出现时长
//const int kBuildFishKingTraceElasped			=	34;//鱼王出现时长
const int kSwitchSceneElasped					=	600;
const int kSceneEndElasped						=	63;
const int kLKScoreElasped						=	1;


const int kFishAliveTime = 180000;

static SCORE g_stock_score_ = 0;
static SCORE g_revenue_score = 0;

#define GF_NORMAL					10								/**< 游戏正常结束 */
#define GF_SALE						11								/**< 游戏安全结束 */

//游戏桌类
class CServerGameDesk : public CGameDesk
{
//函数定义
public:
	//构造函数
	CServerGameDesk(); 
	//析构函数
	virtual ~CServerGameDesk();

	//重载函数
public:
	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	//拦截框架消息
	virtual	bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);;
	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);
	//断线重回
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);
	///用户重新进入
	virtual  bool UserReCome(BYTE bDeskStation, CGameUserInfo *pLostUserInfo);
	///用户坐到游戏桌
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);

	//重载纯虚函数
public:
	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	//定时器消息
	virtual bool OnTimer(UINT uTimerID);
	//处理函数
public:
	//初始化一系列工作(重新开始游戏才进行此种初始化)
	//BOOL InitThisGame();
	//读取静态变量
	static BOOL LoadIni();	//准备发牌工作
	BOOL LoadExtIni(UINT uRoomID);   //根据房间ID加载配置文件

	//清所有计时器
	void KillAllTimer();

	//////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////游戏辅助功能/////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//请求提前结束
	BOOL UserStop(BYTE bDeskStation,BOOL bAgree= false);
	//统计游戏人数
	BYTE CountPlayer();

	//------------------------------------------------------------------------------
	//捕鱼函数----------------------------------------------------------------------
	//------------------------------------------------------------------------------

	//算分
	void CalcScore(BYTE bDeskStation);
	//启动所有计时器
	void StartAllGameTimer();
	//设置游戏状态
	void SetGameStatus(BYTE game_status) { m_bGameStatus = game_status; }
	//获取游戏状态
	BYTE GetGameStatus() const { return m_bGameStatus; }
	//获得鱼ID
	int GetNewFishID();
	//获得子弹ID
	int GetBulletID(WORD chairid);
	//发送配置信息
	bool SendGameConfig(BYTE bDeskStation, GameStation& cmd);
	//加载配置文件
	bool LoadConfig();

	FishTraceInfo* ActiveFishTrace();
	bool FreeFishTrace(FishTraceInfo* fish_trace_info);
	void FreeAllFishTrace();
	FishTraceInfo* GetFishTraceInfo(int fish_id);


	void SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, int bullet_mulriple);
	bool FreeSweepFish(int fish_id);
	SweepFishInfo* GetSweepFish(int fish_id);

	//从仓库中激活一颗子弹数据给指定玩家
	ServerBulletInfo* ActiveBulletInfo(WORD chairid);
	//清除指定玩家指定子弹信息，并将清除的数据保存到仓库中
	bool FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info);
	//清理指定玩家所有子弹信息
	void FreeAllBulletInfo(WORD chairid);
	//获取子弹信息
	ServerBulletInfo* GetBulletInfo(WORD chairid, int bullet_id);

	//计时器-----------------------------------------------------------------
	bool OnTimerBuildSmallFishTrace();
	bool OnTimerBuildMediumFishTrace();
	bool OnTimerBuildFish18Trace();
	bool OnTimerBuildFish19Trace();
	bool OnTimerBuildFish20Trace();
	bool OnTimerBuildFishLKTrace();
	bool OnTimerBuildFishBombTrace();
	bool OnTimerBuildFishSuperBombTrace();
	bool OnTimerBuildFishLockBombTrace();
	bool OnTimerBuildFishSanTrace();
	bool OnTimerBuildFishSiTrace();
	bool OnTimerBuildFishKingTrace();
	bool OnTimerClearTrace();
	bool OnTimerBulletIonTimeout(WPARAM bind_param);
	bool OnTimerLockTimeout();
	//计时器_转换场景
	bool OnTimerSwitchScene();
	//计时器_转换场景结束
	bool OnTimerSceneEnd();
	bool OnTimerLKScore();

	//收到消息-----------------------------------------------------------------
	//换算捕鱼币
	bool OnSubExchangeFishScore(BYTE bDeskStation, bool increase);
	bool OnSubUserFire(BYTE bDeskStation, BulletKind bullet_kind, float angle, int bullet_mul, int lock_fishid);
	bool OnSubCatchFish(BYTE bDeskStation, int fish_id, BulletKind bullet_kind, int bullet_id, int bullet_mul);
	bool OnSubCatchSweepFish(BYTE bDeskStation, int fish_id, int* catch_fish_id, int catch_fish_count);
	bool OnSubHitFishLK(BYTE bDeskStation, int fish_id);
	bool OnSubStockOperate(BYTE bDeskStation, unsigned char operate_code);
	bool OnSubUserFilter(BYTE bDeskStation, DWORD game_id, unsigned char operate_code);
	bool OnSubFish20Config(BYTE bDeskStation, DWORD game_id, int catch_count, double catch_probability);

	//事件函数-----------------------------------------------------------------
	//游戏开始事件
	bool OnEventGameStart();
	//游戏结束事件
	bool OnEventGameConclude(WORD chair_id, BYTE reason);
	bool OnEventSendGameScene(WORD chair_id, BYTE game_status, GameStation& cmd);

	//鱼阵轨迹-----------------------------------------------------------------
	//创建轨迹
	void BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type);
	//
	void BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end);
	//清理轨迹
	void ClearFishTrace(bool force = false);

	//创建场景-----------------------------------------------------------------
	void BuildSceneKind1();
	void BuildSceneKind2();
	void BuildSceneKind3();
	void BuildSceneKind4();
	void BuildSceneKind5();



	//发送消息方法重写-----------------------------------------------------------------
	//发给所有人
	void SendDataToAllPlayers(void * pData, UINT uSize, BYTE bAssID);
	//发给指定玩家
	void SendDataToPlayer(BYTE bDeskStation, void * pData, UINT uSize, BYTE bAssID);

	//是否是超端用户
	bool IsSuperMan(BYTE bDesk);
	//获取该玩家的单位分数
	int GetSuperCellscore(BYTE bDesk);
	//存在真人
	bool ExistUnAI();
	//计算累积达到最大上分时所用的次数
	int GetTopFen(BYTE bDeskStation);

	//字符转换（UTF_8->GB2312）
	void UTF_8ToUnicode(wchar_t* pOut,char *pText);
	void UnicodeToGB2312(char* pOut,wchar_t uData);
	void UTF_8ToGB2312(char*pOut, char *pText, int pLen);
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
public:
	bool m_bExitWatcher[PLAY_COUNT];//是否存在旁观玩家 flase为 不存在， true为 存 序号为玩家逻辑ID

	char m_cFishName[FISH_KIND_COUNT][MAX_PATH];

private:
	bool			m_bIsplaying;				//游戏进行中

	BYTE			m_bGameStatus;				//游戏状态

	SceneKind		next_scene_kind_;			//下一个场景ID

	bool			special_scene_;				//特殊场景

	WORD			android_chairid_;			//机器人座位

	int				current_fish_lk_multiple_;	//当前李逵的倍数

	int				fish_id_;					//新鱼序列号
	int				fish_id_LK;					//当前李逵序列号
	SCORE			exchange_fish_score_[GAME_PLAYER];//兑换了的分数
	SCORE			fish_score_[GAME_PLAYER];	//玩家获得的分数

	int bullet_id_[GAME_PLAYER];

	//ITableFrame* table_frame_;
	//tagGameServiceOption* game_service_option_;
	//tagGameServiceAttrib* game_service_attrib_;
	SCORE base_score_;

	int stock_crucial_score_[10];
	double stock_increase_probability_[10];
	int stock_crucial_count_;

	int				exchange_ratio_userscore_;	//换算比例中用户金币
	int				exchange_ratio_fishscore_;	//换算比例中鱼币
	int				exchange_count_;			//一次兑换的数目（配置项）

	int				min_bullet_multiple_;		//子弹最小倍数
	int				max_bullet_multiple_;		//子弹最大倍数
	int				bomb_stock_;				//炸弹库存
	int				super_bomb_stock_;			//超级炸弹库存
	int				fish_speed_[FISH_KIND_COUNT];//各种鱼的速度
	int				fish_multiple_[FISH_KIND_COUNT];//各种鱼的分数
	int				fish18_max_multiple_;//18号鱼最大倍数
	int				fish19_max_multiple_;//19号鱼最大倍数
	int				fishLK_max_multiple_;//李逵最大倍数
	int				fish_bounding_box_width_[FISH_KIND_COUNT];//各种鱼边框的宽
	int				fish_bounding_box_height_[FISH_KIND_COUNT];//各种鱼边框的高
	int				fish_hit_radius_[FISH_KIND_COUNT];//各种鱼伤害范围
	double			fish_capture_probability_[FISH_KIND_COUNT];//各种鱼被捕获的概率
	int				bomb_range_width_;//炸弹范围的宽
	int				bomb_range_height_;//炸弹范围的高

	int				bullet_speed_[BULLET_KIND_COUNT];//各种子弹速度
	int				net_radius_[BULLET_KIND_COUNT];//各种子弹产生鱼网的大小
	int				net_bounding_box_width_[BULLET_KIND_COUNT];//各种子弹产生鱼网的宽
	int				net_bounding_box_height_[BULLET_KIND_COUNT];//各种子弹产生鱼网的高

	//鱼的轨迹数据
	typedef std::vector<FishTraceInfo*> FishTraceInfoVecor;
	FishTraceInfoVecor active_fish_trace_vector_;//当前活动的鱼的轨迹
	FishTraceInfoVecor storage_fish_trace_vector_;//鱼的轨迹数据仓库

	std::vector<SweepFishInfo> sweep_fish_info_vector_;

	typedef std::vector<ServerBulletInfo*> ServerBulletInfoVector;
	//每个玩家的子弹数据
	ServerBulletInfoVector server_bullet_info_vector_[GAME_PLAYER];
	//子弹数据仓库
	ServerBulletInfoVector storage_bullet_info_vector_;

	//当前总鱼数
	int m_iFishNum;

	//超端用户数据
	std::vector<SuperManInfo> m_SuperMan;

	int kBuildSmallFishTraceElasped;//小鱼出现时长0
	int kBuildMediumFishTraceElasped;//中等鱼出现时长1
	int kBuildFish18TraceElasped;//18号鱼出现时长2
	int kBuildFish19TraceElasped;//19号鱼出现时长3
	int kBuildFish20TraceElasped;//20号鱼出现时长4
	int kBuildFishLKTraceElasped;//李逵出现时长5
	int kBuildFishBombTraceElasped;//炸弹鱼出现时长6
	int kBuildFishSuperBombTraceElasped;//超级炸弹鱼出现时长7
	int kBuildFishLockBombTraceElasped;//定屏炸弹鱼出现时长8
	int kBuildFishSanTraceElasped;//大三元鱼出现时长9
	int kBuildFishSiTraceElasped;//大四喜鱼出现时长10
	int kBuildFishKingTraceElasped;//鱼王出现时长11
};

/******************************************************************************************************/
