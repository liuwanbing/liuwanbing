#pragma once

#include "StdAfx.h"
#include "Resource.h"

//对齐方式
#define	SW_NO_MOVE			0						//不移动
#define SW_MODE_LEFT		1						//左对齐
#define SW_MODE_CENTER		2						//居中
#define SW_MODE_RIGHT		3						//右对齐

//扑克数据
#define CARD_HIDE			95						//扑克高度
#define CARD_WIDTH			70						//扑克宽度
#define CARD_SPACE			15						//扑克间隔

//扑克控制类
class CCardWnd : public CWnd
{
	//变量定义
protected:
	int						m_ShowMode;				//显示模式
	int						m_iMaxCardCount;		//最大派数目
	int						m_iCardCount;			//当前派数目
	int						m_NowDownHitCard;		//当前焦点牌
	BYTE					m_bShowCard;			//是否显示
	BYTE					* m_piCardList;			//牌列数组
	BOOL					* m_pbUp;				//是否顶起
	BYTE					m_bUseMouse;			//是否响应鼠标消息
	CPoint					m_BasePoint;			//基准位置点
	static	HCURSOR			m_Cursor;				//光标
	static 	CBitmap			m_CardBmp[3];			//扑克图
	static  BOOL			m_bInitList;			//是否载入标志

public:
	CWnd * pgg;

	//函数定义
public:
	//构造函数
	CCardWnd();
	//析构函数
	virtual ~CCardWnd();
	//获取大小函数
	BOOL GetWndSize(SIZE &Size);
	//设置基准点函数
	BOOL SetBasePoint(POINT & BasePoint, int iShowMode);
	//初始化扑克
	BOOL InitCard(int iCardCount);
	//获取扑克信息
	int GetCard(BYTE iCard[],BOOL bUp[]);
	//设置是否使用鼠标
	BOOL SetUseMouse(BOOL bUserMouse=TRUE);
	//设置扑克
	BOOL SetCard(BYTE iCardList[], BOOL bUp[], int iCardCount);
	//获取升起的扑克
	int	GetUpCard(BYTE iCard[]);
	//删除升起的扑克
	int RemoveUpCard();
	//设置是否显示扑克
	BOOL ShowCard(BOOL bShow);

protected:
	//按键测试
	int HitCardTest(CPoint & point);
	//绘画扑克函数
	BOOL DrawCard(CDC * pDC, POINT &Point, BYTE iCard, int iStation);
	//调整窗口位置
	BOOL FixCardControl(BOOL bReDraw = FALSE);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

