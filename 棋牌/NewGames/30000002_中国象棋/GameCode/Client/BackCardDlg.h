#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameCard.h"

//底牌信息显示类
class CBackCardDlg : public CGameFaceGo//CAFCBaseDialog
{

	//变量定义
public:
	int					m_iBackCount;					//底牌数目
	BYTE				m_iBackCard[8];					//底牌数据
	CUpGradeGameCard			m_BackCard;						//自己的扑克
//	CAFCButton			m_btOk;							//确定按钮
	CNormalBitmapButtonEx m_btOk;
	enum				{ IDD = IDD_BACK_CARD };		//对话框数据

	//函数定义
public:
	//构造函数
	CBackCardDlg(CWnd* pParent = NULL);
	//析构函数
	virtual ~CBackCardDlg();
	//初始化函数
	virtual BOOL OnInitDialog();
	//设置底牌
	void SetBackCard(BYTE iCardList[], int iCardCount);

protected:
	//取消函数
	virtual void OnCancel();
	//确定函数
	virtual void OnOK();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
};
