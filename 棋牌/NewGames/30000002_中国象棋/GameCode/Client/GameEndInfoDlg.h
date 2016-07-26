#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "GameCard.h"

//游戏结束对话框
class GameEndInfoDlg : public CGameFaceGo
{

	//变量定义
public:
	CListCtrl			m_List;								//列表视图
	CUpGradeGameCard			m_BackCardView;						//底牌视图
	CNormalBitmapButton			m_btKnow;							//取消按钮
	CNormalBitmapButton			m_btQuit;							//退出按钮
	enum				{ IDD = IDD_GAME_END };				//对话框数据

	//函数定义
public:
	//构造函数
	GameEndInfoDlg(CWnd *pParent);   
	//析够函数
	virtual ~GameEndInfoDlg();
	//设置信息
	BOOL SetEndGameData(GameEndStruct * pGameEnd, UserItemStruct * pUserInfo[], int iPlayCardCount,CWnd * pParent);
	//初始化函数
	virtual BOOL OnInitDialog();

protected:
	//取消
	virtual void OnCancel();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    
	//退出按钮
	afx_msg void OnBnClickedQuit();

	DECLARE_MESSAGE_MAP()
};
