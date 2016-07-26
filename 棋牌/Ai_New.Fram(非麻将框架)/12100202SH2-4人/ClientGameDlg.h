#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "UpGradeLogic.h"

#define MAST_PEOPLE				-1							/**< 是否为管理员 */
//消息定义
#define IDM_BEGIN				WM_USER+120					/**< 开始按钮消息 */
#define IDM_HAVE_THING			WM_USER+127					/**< 有事要走 */
#define IDM_SEND_THING			WM_USER+128					/**< 发送离开请求 */
#define IDM_ARGEE_LEAVE			WM_USER+129					/**< 同意离开消息 */
#define IDM_STOP				WM_USER+130					/**< 提前结束按钮 */
#define IDM_STOP_THING			WM_USER+131					/**< 发送提前结束 */
#define IDM_ARGEE_STOP			WM_USER+132					/**< 同意提前结束 */
#define IDM_PASS				WM_USER+133					/**< 不出按钮消息 */
#define IDM_AUTO				WM_USER+134					/**< 托管消息 */
#define	IDM_ACTIVE_RESULT		WM_USER+136					/**< 用户处理结果 */
#define IDM_SHOW_NOTE			WM_USER+137					/**< 用户选择下注,加注 */
#define IDM_SELECT_NOTE_NUM		WM_USER+138					/**< 用户下注 */
#define	IDM_SOUND				WM_USER+139					/**< 用户设置声音 */
#define IDM_BGSOUND				WM_USER+140					/**< 背景音乐 */
//colin
#define IDM_LOCK				WM_USER+141                 /**< 锁桌消息 */
//定时器 ID
#define ID_BEGIN_TIME			100							/**< 开始定时器 */
#define ID_RESULT_TIME			101	
#define ID_OUT_CARD				103							/**< 出牌等待时间 */
#define ID_LOOK_BACK			105							/**< 看暗牌 */
#define ID_CHANGE_MUSIC			106							/**< 换背景音乐 */
//重置函数参数
#define RS_ALL_VIEW				1							/**< 重置所有参数 */
#define RS_GAME_CUT				2							/**< 用户离开重置 */
#define RS_GAME_END				3							/**< 游戏结束重置 */
#define RS_GAME_BEGIN			4							/**< 游戏开始重置 */
#define GAME_RAND_TIME			5							/**< 系统时间 */

/// 游戏框架类 
class CClientGameDlg : public CLoveSendClass//CGameFrameDlg
{
	//变量定义
public:
	BYTE					m_iThinkTime;					/**< 最少游戏盘数 */
	BYTE					m_iBeginTime;					/**< 游戏开始时间 */
	int						m_iBeenPlayGame;				/**< 游戏局数 */

	int						m_iRoomBasePoint;				/**< 房间倍数 */
	int						m_iRunPublish;					/**< 逃跑扣分 */
	__int64					m_iLimitNote;					/**< 最大限额注 */
	__int64					m_iBaseNote;					/**< 底注 */
	__int64					m_iThisTurnLimit;				/**< 本轮限注 */
	BYTE					m_iCardCount;					/**< 允许游戏的扑克数目 */
	BYTE					m_iAfterWaitAction;  			/**< 开始倒计时结束后的动作 1 自动开始 2 自动离开 0900311a1 JJ */
	//运行信息
	BYTE					m_iDeskCardCount[4];			/**< 每人桌面扑克的数目 */
	BYTE					m_DeskCard[4][5];				/**< 每人桌面的扑克 */
	BYTE					m_UserCard[4][5];				/**< 用户扑克 */
	//运行信息
	int						m_iNowOutPeople;				/**< 现在出牌者 */
	BYTE					m_iFirstOutPeople;				/**< 第一个出牌者 */
	__int64					m_iNowBigNote;					/**< 当前最大押注数 */
	__int64					m_iThisGameNote[4];				/**< 本局游戏下注数 */
	__int64					m_iThisTurnNote[4];				/**< 本轮下注 */
	BYTE					m_iThisNoteTimes;				/**< 押注轮数 */
	BYTE					m_bTimeOutCount;				/**< 超时 */
	BYTE					m_bShowAnimalStation;			/**< 动画显示位置 */
	int						m_iUserStation[PLAY_COUNT];		/**< 记录用户状态 */
	TCHAR					m_szName[PLAY_COUNT][61];		/**< 玩家昵称 */
	int						m_iDoublePointTime[PLAY_COUNT];	/**< 双倍时间 */
	int						m_iiProtectTime[PLAY_COUNT];	/**< 护身符时间 */

	bool					m_bSeeCardAnimal;				/**< colin能否播放动画 */
	int                     m_desk;							/**< colin播放动画的位置 */
    UINT                    m_uKanID;						/**< 看牌动画ID号 */

	__int64                 m_TempNote[PLAY_COUNT];        /**< colin090418玩家单局下注数 */
	__int64                 m_iTempThisTurnNote[PLAY_COUNT];
	//CClientPlayView			m_PlayView;						/**< 游戏视图 */
	bool					m_bUserState[PLAY_COUNT];		/**<玩家状态(增加游戏开始后玩家亦可进入功能时添加的数据成员)*/	
	__int64					m_iPerJuTotalNote[PLAY_COUNT];	/**< 用户每局压总注 解决收集动画后显示下注数与实际下注数不符的问题 xyh */
	BYTE					m_iOutCardPeople;				/**< 现在出牌用户 */				

	//控件变量
private:		
	int						m_iVersion;
	int						m_iVersion2;

	//背景音乐
	//HWND					m_hMciWnd1;						/**< 背景音乐(出牌) */
	//HWND					m_hMciWnd2;						/**< 背景音乐(场景) */
	BYTE					m_byUserFollow;					/**< 第几把必跟 */
	BYTE                    m_byCurStyle;           	    /**<  当前下注类型 */
	bool					m_bCanHitLock;					/**< colin能否点击锁 */

	BOOL					m_bNetCutPlayer[PLAY_COUNT];

	BYTE                    m_iSendAnimaCardPos;     ///发牌动画位置
	int                     m_iSendCardTotle;         ///发牌的张数
	int                     m_iNeedSendCardTotle ; 
	BYTE                    m_bShowHa;               ///第几张可以梭

	bool					m_bIsSuperUser;		//判断自己是否超级客户端：由服务器传来数据，每局都校验//lym 2011-11-28
  	BYTE					m_byAddNum;												/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
	BYTE                    m_bPK;                ///是否PK
	CUpGradeGameLogic		m_Logic;					//升级逻辑
	bool					m_bChangeRobotCard;			/**<是否改变了机器人的牌 true 改变了 false 没有改变  */
	int						m_iGameTimes;				/**<游戏局数*/	
	bool					m_bLeftOrGoOn;				/**<游戏结束后是离开还是继续游戏 true:继续游戏 false:离开*/
	BYTE					m_byGameTimes;				/**<多少局游戏后离开游戏*/
	BYTE					m_byLeaveThinkTime;			/**<离开思考时间*/
	//函数定义
public:
	/// 构造函数
	CClientGameDlg();
	/// 析构函数
	virtual ~CClientGameDlg();
    int GetGameStatus();

protected:
	/// 初始化函数
	virtual BOOL OnInitDialog();
	/// 数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	/// 旁观更改
	virtual void OnWatchSetChange(void);
	virtual void OnGameSetting();

	//重载函数
public:
	/// 设置游戏状态
	/// @param pStationData 状态数据
	/// @param uDataSize 数据包大小
	virtual bool SetGameStation(void * pStationData, UINT uDataSize);
	
	/// 游戏消息处理函数
	/// @param pNetHead 消息头
	/// @param pNetData 消息内容
	/// @param uDataSize 消息大小
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	
	/// 重新设置界面
	virtual void ResetGameFrame();
	
	/// 定时器消息
	/// @param bDeskStation 玩家ID
	/// @param uTimeID 计时器ID
	/// @param uTimeCount 当前倒计秒数
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount);
	

	/// 清除所有定时器
	void KillAllTimer();
	/// 同意开始游戏 
	virtual bool OnControlHitBegin();
	/// 安全结束游戏
	virtual bool OnControlSafeFinish(){return false;};
	
	/// 用户离开
	/// @param bDeskStation 玩家ID
	/// @param pUserItem 用户信息结构
	/// @param bWatchUser 是否旁观
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	/// 能否离开桌子
	virtual BOOL CanLeftDesk();
    /// colin090414得到最大值
	__int64 GetMaxNote(__int64 a[4]);
	/// colin090511牌值转换为FLASH的牌值
	BYTE GetFlashCard(BYTE iCard);
	/// 翻转
	BYTE LogicStation(BYTE deskStation);

	//消息函数
public:
	

	
	/// 开始按钮函数
	/// @param wparam 保留
	/// @param lparam 保留
	/// @return LRESULT 保留
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	
	/// 按动扑克按钮函数
	LRESULT	OnHitCard(WPARAM wparam, LPARAM lparam);
	/// 按动扑克按钮函数
	LRESULT	OnMouseMove(WPARAM wparam, LPARAM lparam);
	/// 按动扑克按钮函数
	LRESULT	OnMouseLeft(WPARAM wparam, LPARAM lparam);
	/// 按动有事按钮函数
	LRESULT	OnHitHaveThing(WPARAM wparam, LPARAM lparam);
	/// 发送离开请求函数
	LRESULT	OnSendHaveThing(WPARAM wparam, LPARAM lparam);
	/// 用户请求离开
	LRESULT	OnArgeeUserLeft(WPARAM wparam, LPARAM lparam);
	/// 提前结束按钮
	LRESULT	OnStop(WPARAM wparam, LPARAM lparam);
	/// 发送提前结束
	LRESULT	OnStopThing(WPARAM wparam, LPARAM lparam);
	/// 同意提前结束
	LRESULT	OnAgreeStop(WPARAM wparam, LPARAM lparam);
	/// 用户处理结果
	LRESULT OnHitActiveResult(WPARAM wparam, LPARAM lparam);
	/// 托管
	LRESULT OnHitAuto(WPARAM wparam,LPARAM lparam);
	/// 显示用户下注界面
	LRESULT OnShowNote(WPARAM wparam,LPARAM lparam);
	/// 用户下注消息
	LRESULT OnSelectNote(WPARAM wparam,LPARAM lparam);	
	/// NT 按钮函数
	LRESULT	OnHitNt(WPARAM wparam, LPARAM lparam);
	/// 发牌动画完成后显示各家牌
	LRESULT SendCardShowCard(WPARAM wparam,LPARAM lparam);
	/// 更新下注列表
	BOOL UpdateNtList();
	/// 重置游戏
	/// @param iGameStation 重置什么状态
	void ResetGameStation(int iGameStation);	
	/// colin09423锁桌
	LRESULT	OnHitLock(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()

public:
	/// 退出
	virtual void OnCancel();
	
	/// 检查版本（与服务器版本比较）
	/// @param iVersion 低版本
	/// @param iVersion2 高版本
	BOOL CheckVersion(BYTE iVersion,BYTE iVersion2);
	
	/// 修改用户押注类型
	BOOL ModifyNote(BYTE iViewStation,__int64 iCurNote,BOOL bExtVal=false);
	/// 玩家下注
	BOOL UserNote();
	/// 代替断线玩家下注
	BOOL ReplaceUserNote(BYTE bDeskStation);
public:
	/// 桌号换算到视图位置
	virtual BYTE ViewStation(BYTE bDeskStation);
	

};

extern CClientGameDlg * m_pCClientGameDlg;