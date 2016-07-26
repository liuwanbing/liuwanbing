#pragma once

#include "Stdafx.h"
#include "AFCLoader.h"

//登录对话框类
class CLogonDialog : public CDialog
{
	//变量定义
public:
	CString							m_strName;						//用户名字 
	CString							m_strPass;						//用户密码

public:
	//构造函数
	CLogonDialog(CWnd * pParent=NULL);
	//析构函数 
	virtual ~CLogonDialog();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();

	//消息函数
public:
	//授权设置
	afx_msg void OnBnClickedSetting();

	DECLARE_MESSAGE_MAP()
};
