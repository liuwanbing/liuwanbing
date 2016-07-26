#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "StationView.h"
#include "NtList.h"
#include "GameNoteWnd.h"
#include "PlaySetDlg.h"
#include "GameInfoDlg.h"
//#include "GameEndInfoDlg.h"
#include "BackCardDlg.h"
#include "HaveThing.h"
#include "LeaveMessage.h"
#include "GameCard.h"
#include "Stop.h"
#include "StopMessage.h"
#include "ChessManual.h"
#include "AdmitDefeat.h"
#include "RegretChess.h"
#include "GameFinishDlg.h"
#include "GameDaoSHiDlg.h"
class CUpGradeDlg;
class CUpGradeView;

/// 控件 ID
#define IDC_GAME_BEGIN				120				///< 开始按钮
#define IDC_OUT_CARD				121				///< 出牌按钮
#define IDC_GIVE_BACK				122				///< 留底按钮
#define IDC_CONTINUE				124				///< 继续按钮
#define IDC_LAST_CARD				125				///< 上轮按钮
#define IDC_BACK_CARD				126				///< 底牌按钮
#define IDC_HAVE_THING				127				///< 有事要走
#define IDC_STOP					128				///< 停止本局
#define IDC_ADMIT_DEFEAT			130				///< 认输
#define IDC_REGRET_CHESS			131				///< 悔棋
#define IDC_CHESS_MANUAL			132				///< 棋谱
#define IDC_USER_FACE				133				///< 用户表情
#define IDC_USER_FACE2				134				///< 用户表情
#define IDC_LEAVE					135				///< 离開

/// 梭哈视图类
class CClientPlayView : public CUsingSkinClass//CGDIPlayView
{
	/// 程序变量
protected:
	CPoint				m_PtNt[4];					///< 用户反牌位置
	CPoint				m_PtNtLogo[4];				///< 用户反牌位置
	CPoint              m_PtGif[PLAY_COUNT];
	
	/// 用户信息
	int					m_iHuaKind;					///< 主牌花色
	int					m_iShowStation;				///< 报主位置
	int					m_iNtStation;				///< 庄家位置

#ifdef	VIDEO
	public:
		CRect				m_RtVideo[2];			///< 用户的视频窗口位置
#endif

	/// 控件变量
public:
	CBitmap				m_NT;									///< NT 图标
	CBitmap				m_NTLogo;								///< 标志图标
	//CNtList				m_NtList;							///< 反牌按钮
	CUpGradeGameCard			m_MeCard;						///< 自己的扑克
//	CUpGradeGameCard			m_MeBackCard;					///< 自己的底牌
	int						m_iCardNum;
//	CUpGradeGameCard			m_UserCard[4];					///< 用户扑克		
	CGameInfoDlg		m_GameInfo;								///< 游戏信息(房主设置的游戏时间信息)
	CGameDaoSHiDlg		m_GameDaoShi;							///< 游戏信息(房主设置的游戏时间信息)
	CPlaySetDlg			m_GameSet;								///< 游戏设置(第一个进入房间弹出的窗田口)
	CGameNoteWnd		m_GameNoteView;							///< 游戏信息
	CStationView		m_StationView;							///< 状态窗口
//	CNormalBitmapButtonEx			m_btUserFace[0];			///< 用户头像
//	CNormalBitmapButtonEx			m_btUserFace[2];			///< 用户头像
	CNormalBitmapButtonEx			m_btOutCard;				///< 出牌按钮
	CNormalBitmapButtonEx			m_btGiveBack;				///< 留底按钮
	CNormalBitmapButtonEx			m_btContinue;				///< 继续按钮
//	CAFCPicButton			m_btLastTurn;						///< 上一轮
	CNormalBitmapButtonEx			m_btLastTurn;				///< 上一轮
	CNormalBitmapButtonEx			m_btBackCard;				///< 底牌
	CNormalBitmapButtonEx			m_btThing;					///< 有事要走
	CNormalBitmapButtonEx			m_btStop;					///< 停止本局  ;求和
	CNormalBitmapButtonEx			m_btAdmitDefeat;			///< 认输
	CNormalBitmapButtonEx			m_btRegretChess;			///< 悔棋
	CNormalBitmapButtonEx			m_btChessManual;			///< 棋谱
	CNormalBitmapButtonEx			m_btStart;					///< 棋谱
	CNormalBitmapButtonEx			m_btLeave;					///< 棋谱


	
	CLeaveMessage		m_ThingMessage;							///< 离开消息
	CBackCardDlg		m_BackCardDlg;							///< 底牌视图
	CHaveThing			m_HaveThing;							///< 有事离开
	CStop				m_StopThing;
	CStopMessage		m_StopMessage;
	CAdmitDefeat		m_AdmitDefeat;							///< 认输
	CChessManual		m_ChessManual;							///< /棋谱
	CRegretChess		m_RegretChess;							///< 悔棋
	CGameFinishDlg		m_GameFinishView;						///< 游戏结算
//	GameEndInfoDlg		m_GameEndView;							///< 结束视图
//	CGameImage			m_PicTopBar;							///< 顶端贴图
//	CGameImage			m_PicTopBarLeft;						///< 顶端贴图
//	CGameImage			m_PicTopBarRight;						///< 顶端贴图

	CGameImage			m_imgBk;  								///< 游戏背景图


	bool				m_bUpdateScoreboard;					///< 更新积分榜

	/// 函数定义
public:
	/// 构造函数
	CClientPlayView();
	/// 析构函数
	virtual ~CClientPlayView();

	/// 重载函数
public:
	/// 重置界面函数
	virtual void ResetGameView();
	/// 绘画界面函数
	virtual void DrawViewFace(CDC * pDC, int iWidth, int iHeight);
	/// 更新用户资料
	virtual void UpdateUserInfo(BYTE bViewStation);
	virtual void FixControlSize(int,int);
	/// 功能函数
public:
	void DrawInfo(CDC *pDC);
	void TransparentBlt2( HDC hdcDest,  // 目标DC
					int nXOriginDest,   // 目标X偏移
					int nYOriginDest,   // 目标Y偏移
					int nWidthDest,     // 目标宽度
					int nHeightDest,    // 目标高度
					HDC hdcSrc,         // 源DC
					int nXOriginSrc,    // 源X起点
					int nYOriginSrc,    // 源Y起点
					int nWidthSrc,      // 源宽度
					int nHeightSrc,     // 源高度
					UINT crTransparent  // 透明色,COLORREF类型
					);

	/// 消息函数
protected:
	/// 建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	/// 提示文字
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	/// 设置背景色
	void SetBackBmp(UINT uResourceID, TCHAR * szFileName=NULL, BOOL bResource=TRUE);
	/// 设置庄家位置
	void SetNTStation(int iViewStation=-1);
	/// 设置报主位置
	void SetShowNtStation(int iViewStation=-1, int iHuaKind=0);
	/// 设置游戏分数
	void SetGamePoint(int iGamePoint=0, int bMeSide=-1);
	/// 绘画 NT 标志
	void DrawNt(CDC * pDC, int x, int y);
	/// 绘画 NT 图标
	void DrawNtLogo(CDC * pDC, int x, int y, int iHuaKind);
	DECLARE_MESSAGE_MAP()
	/// 按钮消息函数
protected:
	/// 开始按钮
	void OnHitBegin();
	/// 留底牌按钮
	void OnHitBackCard();
	/// 出牌按钮
	void OnHitOutCard();
	/// 继续按钮
	void OnHitContinue();
	/// 上轮扑克
	void OnHitLastTurn();
	/// 底牌按钮
	void OnHitLookBack();
	/// 有事要走
	void OnHitThing();
	/// 提前停止
	void OnHitStop();
	/// 认输
	void OnHitAdmitDefeat();
	/// 悔棋
	void OnHitRegretChess();
	/// 棋谱
	void OnHitChessManual();
	/// 棋谱
	void OnHitLeave();

public:
#ifdef	VIDEO
	BOOL						m_bWatch;		///< 自己是否为观战
	CGameImage					m_pVideoFrame;	///< 视频背景
	/// 将此昵称的玩家视频窗口矩形写入参数rect,并返回TRUE;没找到返回falst
	BOOL	GetVideoRect(CString  strNickname, RECT& rect, CString& pic);
#endif

	static CBitmap				m_Bitmap_Num;
	BOOL						m_bRightPower;
	BOOL						m_bMeSide;					///< 是否本方得分
	int							m_iGamePoint;				///< 游戏分数
};
