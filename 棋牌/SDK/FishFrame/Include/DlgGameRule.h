#ifndef DLG_GAME_RULE_HEAD_FILE
#define DLG_GAME_RULE_HEAD_FILE

#pragma once

#include "GameFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//游戏帮助
class GAME_FRAME_CLASS CDlgGameRule : public CSkinDialog
{
	//控件变量
public:
	CWebBrowser *					m_pHtmlBrowser;						//浏览控件

	//函数定义
public:
	//构造函数
	CDlgGameRule();
	//析构函数
	virtual ~CDlgGameRule();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//消息映射
public:
	//销毁消息
	VOID OnNcDestroy();
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif