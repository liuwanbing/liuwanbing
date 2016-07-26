#pragma once

#include "StdAfx.h"
#include "Resource.h"

//用户图片
class CUserAvatar : public CDialog
{
	//函数定义
public:
	//构造函数
	CUserAvatar();
	//析够函数
	virtual ~CUserAvatar();

// 对话框数据
	enum { IDD = IDD_USER_AVATAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
