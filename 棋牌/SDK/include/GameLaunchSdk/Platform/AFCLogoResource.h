#pragma once

#ifndef CAFCLOGORESOURCE_HEAD_FILE
#define CAFCLOGORESOURCE_HEAD_FILE

#include "PublicDefine.h"
#include "AFCResource.h"

//宏定义
#define LOGO_HEIGHT				108										//头像高度
#define LOGO_WIDTH				65										//头像宽度
#define LOGO_COUNT				12										//头像数目
#define ZZ_COUNT				10										//坐姿数目

//消息定义
#define WM_HIT_LOGO				WM_USER+30								//单击头像

/*******************************************************************************************************/

//头像资源类
class EXT_CLASS CGameLogoRes//CGameLogoRes
{
public:
	//变量定义
protected:
	static CImage				m_MatchPic;								//比赛头像
	static CImage				m_LostPic;								//断线头像
	static CImage				m_LogoPicArray[LOGO_COUNT];				//头像数组
	static CImage				SitBoyArray[ZZ_COUNT];				//男人坐姿数组
	static CImage				SitGirlArray[ZZ_COUNT];				//女人坐姿数组
	static COLORREF				m_crBackColor;							//透明颜色

	//函数定义
public:
	//构造函数
	CGameLogoRes(void);
	//析构函数
	virtual ~CGameLogoRes(void);

	//功能函数
public:
	//初始化函数
	static bool Init();
	//获取透明色
	static COLORREF GetBackColor() { return  m_crBackColor; };
	//获取比赛图片
	static CImage * GetMatchImage();
	//获取 LOGO 图片
	static CImage * GetLogoImage(UINT uIndex, bool bLost);
//获取坐姿 图片，坐上的是女人
static CImage *GetSitGirl(UINT uIndex, bool bLost);
//获取坐姿 图片，坐上的是男人
static CImage *GetSitBoy(UINT uIndex, bool bLost);
};

/*******************************************************************************************************/

//头像显示类
class EXT_CLASS CGameClientTranStatic/*CGameClientTranStatic*/ : public CStatic
{
	//变量定义
protected:
	CImage							* m_pLogoImage;						//头像图片

	//函数定义
public:
	//构造函数
	CGameClientTranStatic();
	//析构函数
	virtual ~CGameClientTranStatic();
	//设置图片
	bool SetLogoImage(CImage * pLogoImage);

	DECLARE_MESSAGE_MAP()

	//消息函数
public:
	//鼠标单击
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标双击击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//重画函数
	afx_msg void OnPaint();
};

/*******************************************************************************************************/

#endif