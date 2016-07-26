#pragma once

#ifndef AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE
#define AFC_SERVER_SHOWHAND_MESSAGE_HEAD_FILE



#ifdef	VIDEO
		/// 文件名字定义
		#define GAMENAME						TEXT("象棋视频版")
		#define CLIENT_DLL_NAME					TEXT("XQ_Video.ico")
		#define SERVER_DLL_NAME					TEXT("XQSrv_Video.DLL")

		#define NAME_ID							30000002									///< 名字 ID
#else
		/// 文件名字定义
		#define GAMENAME						TEXT("象棋")
		#define CLIENT_DLL_NAME					TEXT("XQ.ico")
		//#define SERVER_DLL_NAME					TEXT("XQSrv.DLL")

		#define NAME_ID							30000002									///< 名字 ID
#endif

/// 文件名字定义
//#define SKIN_FOLDER							TEXT("xq")									///< 图片文件夹
//#define	SKIN								TEXT("cardskin1")
#define	SKIN_CARD								TEXT("cardskin")

//JMod-文件命名标准化-20090209
#define SOUND_MJ_FOLDER			TEXT("mj")//读取声音的路径使用
static TCHAR szTempStr[MAX_PATH] = {0};//暂存字符串，用来获取 dll_name 等的时候使用
#define GET_CLIENT_DLL_NAME(A)(sprintf(A,"%d",NAME_ID),strcat(A,".ico"))					///< 替代原先的 CLIENT_DLL_NAME 
#define GET_SERVER_DLL_NAME(A)(sprintf(A,"%d",NAME_ID),strcat(A,".dll"))					///< 替代原先的 SERVER_DLL_NAME 
#define GET_SKIN_FOLDER(A)(sprintf(A,"%d",NAME_ID),strcat(A,""))							///< 替代原先的 SKIN_FOLDER 
#define MAX_NAME_INFO					MAX_PATH


/// 版本定义
#define GAME_MAX_VER					1								///< 现在最高版本
#define GAME_LESS_VER					1								///< 现在最低版本
#define GAME_CHANGE_VER					0								///< 修改版本

/// 支持类型
#define SUPPER_TYPE						SUP_NORMAL_GAME|SUP_MATCH_GAME|SUP_MONEY_GAME
#define GAME_TABLE_NAME					"MatchTable_XQ20001"

/// 游戏信息
#define PLAY_COUNT						2								///< 游戏人数
#define MAIN_VERSION					7								///< 主版本号,?年?月
#define ASS_VERSION						0								///< 辅助版本号,?号

/// 游戏开发版本
#define DEV_HEIGHT_VERSION				9								///< 内部开发高版本号
#define DEV_LOW_VERSION					2								///< 内部开发低版本号

/// 游戏状态定义
#define GS_WAIT_SETGAME					0								///< 等待东家设置状态
#define GS_WAIT_ARGEE					1								///< 等待同意设置
#define GS_SEND_CARD					20								///< 发牌状态
#define GS_WAIT_BACK					21								///< 等待扣压底牌
#define GS_PLAY_GAME					22								///< 游戏中状态
#define GS_WAIT_NEXT					23								///< 等待下一盘开始 

/********************************************************************************/

/// 游戏数据包

/********************************************************************************/

/**
 * 游戏状态数据包	（ 等待东家设置状态 ）
 */
struct GameStation_1
{
	/// 游戏信息
	bool        bRequireCoachMoeny;                                     ///是否需要指导费
	BYTE		iCardCount;												///< 扑克数目
	int			iMainVersion;
	int			iLessVersion;
	/// 游戏版本
	BYTE				iVersion;										///< 游戏版本号
	BYTE				iVersion2;										///< 游戏版本号
	int					iRoomBasePoint;									///< 房间基础倍数
	int					iRunPublish;									///< 逃跑扣分
};

/**
 * 游戏状态数据包	（ 等待其他玩家开始 ）
 */
struct GameStation_2
{
	bool        bRequireCoachMoeny;                                     ///是否需要指导费
	int			iJuTimeMinute;											///< 局时分
	int			iJuTimeSecond;											///< 局时秒
	int			iBuTimeMinute;											///< 步时分
	int			iBuTimeSecond;											///< 步时秒
	int			iDuMiaoMinute;											///< 读秒分
	int			iDuMiaoSecond;											///< 读秒秒
	int			iGameMoney;												///游戏指导费
	int			iMainVersion;
	int			iLessVersion;
	/// 游戏版本
	BYTE				iVersion;										///< 游戏版本号
	BYTE				iVersion2;										///< 游戏版本号
	int					iRoomBasePoint;									///< 房间基础倍数
	int					iRunPublish;									///< 逃跑扣分

	BYTE				bDaoShi;
};

/**
 * 游戏状态数据包	（ 游戏正在发牌状态 ）
 * 游戏状态数据包	（ 等待扣押底牌状态 ）
 */
struct GameStation_3
{
	/// 游戏版本
	bool                bRequireCoachMoeny;                                     ///是否需要指导费
	BYTE				iVersion;						///< 游戏版本号
	BYTE				iVersion2;						///< 游戏版本号
	/// 游戏信息
	BYTE				iBeginStation;					///< 开始位置
	BYTE				iEndStation;					///< 结束位置
	BYTE				iMeStation;
	BYTE				iOtherStation;
	BYTE				iPlayCount;						///< 游戏盘数
	BYTE				iCardCount;						///< 扑克数目
	int					iRoomBasePoint;					///< 房间基础倍数
	int					iRunPublish;					///< 逃跑扣分
	/// 运行状态变量
	int					iWinPoint;						///< 游戏积分
	int					iPunishPoint;					///< 用户罚分
	int					iUpGradePeople;					///< 庄家位置
	BYTE				iPlayNTNum[2];					///< 各玩家的主牌数目
	BYTE				iBeenPlayGame;					///< 已经游戏的局数
	/// 反牌信息
	int					iNtPeople;						///< 反牌玩家
	BYTE				iNtHua;							///< 反牌花色
	BYTE				iNtCardCount;					///< 反牌牌数
	/// 态信息
	BYTE				iUserCardCount;					///< 用户手上扑克数目
	BYTE				iUserCard[45];					///< 用户手上的扑克

	BYTE				bDaoShi;

};

/**
 * 游戏状态数据包	（ 游戏中状态 ）
 */
struct GameStation_4	
{
	/// 游戏版本
	bool                bRequireCoachMoeny;                                     ///是否需要指导费
	BYTE				iVersion;						///< 游戏版本号
	BYTE				iVersion2;						///< 游戏版本号
	int                 iBgePeaceCount[PLAY_COUNT];
	int					iRoomBasePoint;					///< 房间基础倍数
	int					iRunPublish;					///< 逃跑扣分
	
	/// 游戏信息
	int					iJuTimeMinute;					///< 局时分
	int					iJuTimeSecond;					///< 局时秒
	int					iBuTimeMinute;					///< 步时分
	int					iBuTimeSecond;					///< 步时秒
	int					iDuMiaoMinute;					///< 读秒分
	int					iDuMiaoSecond;					///< 读秒秒
	char                iRegretStation;                 //< 悔棋状态
	int					iJuUseTime[PLAY_COUNT];			///< 用时
	int					iBuUseTime[PLAY_COUNT];	
	int					iDuUseTime[PLAY_COUNT];	

	BYTE				iPlayCount;						///< 游戏盘数
	BYTE				iBeenPlayGame;					///< 已经游戏的局数
	int					iOutCardPeople;					///< 现在出牌用户
	int					iFirstOutPeople;				///< 先出牌的用户
	int					iLastOutPeople;					///< 上一轮出牌的用户
	int					iStartI;
	int					iStartJ;
	int					iEndI;
	int					iEndJ ;
	int					iQiZi[10][9];

	BYTE				bDaoShi;
};

/**
 * 游戏状态数据包	（ 等待下盘开始状态  ）
 */
struct GameStation_5
{
	/// 游戏版本
	bool                bRequireCoachMoeny;                                     ///是否需要指导费
	BYTE				iVersion;						///< 游戏版本号
	BYTE				iVersion2;						///< 游戏版本号
	int					iRoomBasePoint;					///< 房间基础倍数
	int					iRunPublish;					///< 逃跑扣分
	/// 游戏信息
	int					iJuTimeMinute;					///< 局时分
	int					iJuTimeSecond;					///< 局时秒
	int					iBuTimeMinute;					///< 步时分
	int					iBuTimeSecond;					///< 步时秒
	int					iDuMiaoMinute;					///< 读秒分
	int					iDuMiaoSecond;					///< 读秒秒
	BYTE				iBeginStation;					///< 开始位置
	BYTE				iEndStation;					///< 结束位置
	BYTE				iPlayCount;						///< 游戏盘数
	BYTE				iCardCount;						///< 扑克数目
	/// 运行状态变量
	int					iWinPoint;						///< 游戏积分
	int					iPunishPoint;					///< 用户罚分
	int					iUpGradePeople;					///< 庄家位置
	BYTE				iPlayNTNum[2];					///< 各玩家的主牌数目
	BYTE				iBeenPlayGame;					///< 已经游戏的局数
};

/**
 * 用户同意游戏
 */
struct UserArgeeGame
{
	
	int					iJuTimeMinute;					///< 局时分
	int					iJuTimeSecond;					///< 局时秒
	int					iBuTimeMinute;					///< 步时分
	int					iBuTimeSecond;					///< 步时秒
	int					iDuMiaoMinute;					///< 读秒分
	int					iDuMiaoSecond;					///< 读秒秒
	int					iGameMoney;						///游戏指导费

	//BYTE				iBeginStation;					//开始位置
	//BYTE				iEndStation;					//结束位置
	//BYTE				iPlayCount;						//游戏盘数
	//BYTE				iCardCount;						//扑克数目
};

/**
 * 游戏开始
 */
struct BeginUpgradeStruct
{
	BYTE				iUpgradeStation;				///< 庄家位置
	BYTE				iPlayCount;						///< 游戏回合
	BYTE				iNtStation;						///< 现在的主牌
	BYTE				iStation[2];					///< 主牌的位置
	int                 BgePeaceCount[2];               ///< 求和次数
	int                 idaoshi;                         ///<导师
};

/**
 * 游戏准备,先手位置,白手位置
 */
struct GameReadyStruct
{
	BYTE				iBlackStation;					///< 先手位置
	BYTE				iFirstStation;					///< 白手位置
};

/**
 * 发牌数据包
 */
struct SendCardStruct
{
	BYTE				iCard;							///< 牌标号
};

/**
 * 亮主牌数据包  （发向服务器）
 */
struct ClientNtStruct
{
	BYTE				iHuaKind;						///< 牌标号
	BYTE				iCardCount;						///< 反牌数目
};

/**
 * 亮主牌数据包  （发向客户端）
 */
struct ServerNtStruct
{
	BYTE				bDeskStation;					///< 反牌位置
	BYTE				iHuaKind;						///< 花色
};

/**
 * 底牌数据包
 */
struct BackCardStruct
{
	BYTE				iBackCardCount;					///< 扑克数目
	BYTE				iBackCard[8];					///< 底牌数据
};

/**
 * 底牌数据包
 */
struct BackCardExStruct
{
	BYTE				iNtHua;							///< 主牌花色
	BYTE				iGiveBackPeople;				///< 底牌玩家
	BYTE				iBackCardCount;					///< 扑克数目
	BYTE				iBackCard[8];					///< 底牌数据
};

/**
 * 留底牌状态数据包
 */
struct BackStationStruct
{
	BYTE				iNtPeople;						///< 反牌位置	
	BYTE				iGiveBackStation;				///< 留底位置
};

/**
 * 游戏开始数据包
 */
struct BeginPlayStruct
{
	BYTE				iOutDeskStation;				///< 出牌的位置
	int					m_iBoard[10][9];				///< 棋局
};

/**
 * 檢查是否結束
 */
struct PlayCheckFinish
{
	BOOL bCanFinish;
};

/**
 * 用户出牌数据包 （发向服务器）
 */
struct OutCardStruct
{
	int					iCardCount;						///< 扑克数目
	BYTE				iCardList[39];					///< 扑克信息
};

struct SuperTimer
{
	int                 bDeskStation;					///< 超时者
};

/**
 * 认输数据
 */
struct LoseStruct
{
	BYTE bDeskStation;
	int iFlag;
};

/**
 * 用户甩牌失败数据包 （发向客户端）
 */
struct OutShowCardStruct
{
	BYTE				iCardCount;						///< 扑克数目
	BYTE				bDeskStation;					///< 当前出牌者
	BYTE				iResultCount;					///< 结果扑克数目
	BYTE				iCardList[78];					///< 扑克信息
};

/**
 * 用户出牌数据包 （发向客户端）
 */
struct OutCardMsg
{
	int					iNextDeskStation;				///< 下一出牌者
	BYTE				iCardCount;						///< 扑克数目
	BYTE				bDeskStation;					///< 当前出牌者
	BYTE				iCardList[39];					///< 扑克信息
};

/**
 * 用户出牌数据包 （发向客户端）
 */
struct BatchOutCardMsg
{
	BYTE				iCardCount;						///< 扑克数目
	BYTE				iCardList[156];					///< 扑克信息
};

/**
 * 玩家的分数据包
 */
struct GamePointStruct
{
	bool				bAdd;
	int					iPoint;							///< 得分数目
};

/**
 * 游戏结束统计数据包
 */
struct GameEndStruct
{
	int					iGamePoint;						///< 游戏得分
	__int64					iTurePoint[4];					///< 庄家得分
	__int64					iChangeMoney[8];				///< 金币
	BYTE				iUpGradeStation;				///< 庄家位置
	BYTE				iBackList[8];					///< 底牌扑克

	bool                bShuaLai;						///< 是否是有玩家耍赖
	BYTE                bDeskStation;					///< 耍赖玩家
};

/**
 * 游戏结束统计数据包
 */
struct GameCutStruct
{
	int					bDeskStation;					///< 退出位置
	__int64					iTurePoint[4];					///< 庄家得分
	__int64					iChangeMoney[8];				///< 金币
};

/**
 * @brief 同步时间数据包
 */
struct SyncTimeStruct
{
	int					iBotJuUseTime;
	int					iBotBuUseTime;
	int					iBotDuUseTime;
};

struct HaveThingStruct
{
	BYTE pos;
	char szMessage[101];
};

struct LeaveResultStruct
{
	BYTE bDeskStation;
	int bArgeeLeave;
};

/**
 * 客户端到服务器
 */
struct MessageStruct
{
	char Message[200];
};

struct StopResultStruct
{
	bool bArgee;
};

struct PlayChessStruct
{
	POINT	ptStart;									///< 走子起始点
	POINT   ptEnd;										///< 走子终点
	int		iCurPlay;									///< 当前走子的
	int		iValue;										///< 走子的值
	int		iNextPlay;									///< 下个走棋者
	int		iBoard[10][9];								///< 棋局

	/// 新加成员，为防止玩家耍赖，重复同样的方法将军
	int     iPutChess;      							///< 下棋步数
	int     xChess,yChess;  							///< 棋子的坐标值
	int     idChess;        							///< 是什么棋子

	
};

/**
 * 下一个下子者
 */
struct NextPlayInfo
{
	int iNextPlay;										///< 下一個下子者
	int iCurPlay;										///< 紅子下,還是黑子下
};

/**
 * 悔棋数据包(发向客户端)
 */
struct RegretChessMsg
{
	int					iLastChessArray[10][9];			///< 记录上步走棋
	int					iLastPlay;						///< 记录上步走棋者
	int					iLastDeskStation;				///< 走棋者的位置
};

struct GamerShuaLaiJiang
{
	int     iPutChess;     							     ///< 下棋步数
	int     xChess,yChess; 							     ///< 棋子的坐标值
	int     idChess;       							     ///< 是什么棋子
};

/**
 * 游戏结算
 */
struct GameFinishNotify
{
	bool bExist[4];									   ///< 玩家是否存在
	int	 iUpGradePeople;
	int iGrade2;
	TCHAR name[4][61];
	//	int	iBasePoint;
	BYTE iStyle;										///< 游戏类型是否为强退,还是正常结束
	int	iWardPoint[4];
	//__int64 iTotalPoint[4];
	//int iAwardPoint[4];								//讨赏
	//	int iGetPoint[4];								//各家抓分
	//	int nt;											//定約
	__int64 iMoney[4];
};

/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_UG_USER_SET					50				///< 用户设置游戏
#define ASS_BEGIN_UPGRADE				51				///< 升级游戏开始
#define ASS_SEND_CARD					52				///< 发牌信息
#define ASS_NT_CARD						53				///< 用户反牌
#define ASS_SEND_FINISH					54				///< 发牌完成
#define ASS_BACK_CARD					55				///< 底牌数据
#define ASS_BACK_CARD_EX				56				///< 底牌数据
#define ASS_BACK_CARD_RES				57				///< 底牌数据结果
#define ASS_WAIT_BACK					58				///< 用户留底牌
#define ASS_GAME_PLAY					59				///< 开始游戏
#define ASS_OUT_CARD					60				///< 用户出牌
#define ASS_OUT_CARD_RESULT				61				///< 下子结果
#define ASS_CAN_FINISH					62				///< 客户端差别是否可以结束
#define ASS_GAME_READY					63				///< 游戏准备工作
//#define ASS_SHOW_CARD					61				///< 甩牌失败
//#define ASS_BATCH_OUT					62				///< 批量出牌
//#define ASS_GAME_POINT					63			///< 用户分数
#define ASS_SYNC_TIME					63				///< 同步用时
#define ASS_NEW_TURN					64				///< 新一轮开始
#define ASS_CONTINUE_END				65				///< 游戏结束
#define ASS_NO_CONTINUE_END				66				///< 游戏结束
#define ASS_CUT_END						67				///< 用户强行离开
#define ASS_SALE_END					68				///< 游戏安全结束
#define ASS_HAVE_THING					69
#define ASS_LEFT_RESULT					70
#define ASS_MESSAGE						71
#define ASS_STOP_THING					72
#define ASS_AGREE_STOP					73
#define ASS_SUPER_TIMER					74				///< 超时处理
#define ASS_REN_SHU						75				///< 认输
#define ASS_HUI_QI						76				///< 悔棋
#define ASS_AGREE_HUI_QI				77				///< 同意悔棋

/// 玩家耍赖将军，重复5次则自动判玩家输
#define ASS_GAMER_SHUALAI_JIANG         80
/********************************************************************************/
#endif
