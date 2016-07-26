#pragma once

#include "StdAfx.h"
#include "Resource.h"

//扑克花色类型
#define SH_FANG_KUAI	1
#define SH_MEI_HUA		2
#define SH_HONG_TAO		3
#define SH_HEI_TAO		4

//扑克类
class CShowHandCard : public CWnd
{
	//枚举定义
public:
	enum XShowMode { SWX_MODE_LEFT, SWX_MODE_CENTER, SWX_MODE_RIGHT };
	enum YShowMode { SWY_MODE_TOP, SWY_MODE_CENTER, SWY_MODE_BOTTOM }; 

	//变量定义
public:
	int								m_iCardHight;					//扑克高度
	int								m_iCardWidth;					//扑克宽度
	int								m_iCardHSpace;					//扑克间隔

	//变量定义
protected:
	bool							m_bShowFirst;					//显示扑克
	bool							m_bUseMouse;					//响应鼠标
	BYTE							m_bCardCount;					//扑克数目
	CPoint							m_PtBasePoint;					//基准坐标
	XShowMode						m_XShowMode;					//显示模式
	YShowMode						m_YShowMode;					//显示模式
	CByteArray						m_CardArray;					//扑克数组

	//静态变量
protected:
	static bool						m_bInit;						//初始标志
	static HCURSOR					m_hCursor;						//鼠标光标
	static CAFCImage				m_CardPic;						//扑克背景
	static CAFCImage				m_BackPic;						//扑克背景

	//函数定义
public:
	//构造函数
	CShowHandCard();
	//析构函数
	virtual ~CShowHandCard();

	//功能函数
public:
	//获取大小函数
	bool GetCardSize(SIZE & Size);
	//设置是否使用鼠标
	void SetUseMouse(bool bUserMouse);
	//设置是否显示第一张扑克
	void SetShowFirst(bool bShowFirst);
	//排列扑克
	void SortCard();
	//设置扑克
	bool SetCard(BYTE bCardList[], BYTE bCardCount);
	//设置扑克
	BYTE GetCard(BYTE bCardBuffer[], BYTE bBufferLength);
	//设置基准点函数
	void SetBasePoint(POINT & BasePoint, XShowMode XShowMode, YShowMode YShowMode);

	//内部函数
private:
	//获取扑克数值
	BYTE GetCardValoe(BYTE bCard);
	//获取扑克花色
	BYTE GetHuaKind(BYTE bCard) { return (bCard-1)/13; };
	//获取扑克位置
	BYTE GetCardXPos(BYTE bCard) { return (bCard-1)%13; };
	//获取扑克位置
	BYTE GetCardYPos(BYTE bCard) { return (bCard-1)/13; };
	//调整窗口位置
	void FixCardControl();
	//对比单只扑克
	bool CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//右键消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//右键消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};
