#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "GameCard.h"

/// 状态视图
class CStationView : public CWnd
{
	DECLARE_DYNAMIC(CStationView)

	/// 变量定义
public:
	CUpGradeGameCard			m_PointCard;			///< 分数扑克

	/// 信息变量
	char						m_iEndStation;			///< 结束位置
	char						m_iMeStation;			///< 本方位置
	char						m_iUpGrade;				///< 庄家位置
	char						m_iBeginStation;		///< 开始位置
	char						m_iOtherStation;		///< 敌方位置
	char						m_iNTStation;			///< 反牌位置
	char						m_iNTCount;				///< 反牌数目
	char						m_iNTHua;				///< 主牌花色
	char						m_iPlayCardCount;		///< 扑克数目
	char						m_iLessPlay;			///< 最少回合
	char						m_iPlayCount;			///< 战斗回合
	int							m_iTurePoint;			///< 真实分数
	int							m_iPunishPoint;			///< 游戏罚分
	BOOL						m_bShowMessage;			///< 是否显示
	
	CGameImage					m_PicBottomBar;			///< 底端贴图
	CGameImage					m_PicBottomBarLeft;		///< 底端贴图
	CGameImage					m_PicBottomBarRight;	///< 底端贴图
	CBitmap						m_NTLogo;				///< 标志图标


	/// 函数定义
public:
	/// 构造函数
	CStationView();
	/// 析构函数
	virtual ~CStationView();
	DECLARE_MESSAGE_MAP()

	/// 功能函数
public:
	BOOL Create(CWnd * pParentWnd, UINT nID);

	/// 消息函数
public:
	/// 建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/// 绘画消息
	afx_msg void OnPaint();

	/// 功能函数
public:
	/// 设置庄家
	void SetUpGrade(int iUpGradeStation);
	/// 设置本方牌局信息
	void SetMeSideInfo(int iBeginStation, int iEndStation, int iPlayStation);
	/// 设置敌方牌局信息
	void SetOtherSideInfo(int iBeginStation, int iEndStation, int iPlayStation);
	/// 设置主牌信息
	void SetNTInfo(int iNTStation, int iNTHua, int iPlayCardCount);
	/// 设置回合信息
	void SetPlayInfo(int iPlayCount, int iLessCount);
	/// 加入分牌
	void AddPointCard(BYTE iCardList[], int iCardCount);
	/// 加入积分
	void SetTurePoint(int iTurePoint);
	/// 加入罚分
	void AddPunishPoint(int iPunishPoint);
	/// 清理信息
	void CleanAllInfo(BOOL bAll);
	/// 是否显示信息
	void SetShowMessage(BOOL bShow);
	void DrawNtLogo(CDC * pDC, int x, int y, int iHuaKind);
};


