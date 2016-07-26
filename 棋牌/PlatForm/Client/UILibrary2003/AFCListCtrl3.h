#pragma once

#ifndef AFCLISTCTRL_HEAD_FILE
#define AFCLISTCTRL_HEAD_FILE

#include "StdAfx.h"
#include "AFCImage.h"
#include "ClientComStruct.h"

//using namespace AFC::AFCImageGoGoGo;

//排列位置
#define SK_ENEMY_USER						0								//敌人排列索引
#define SK_NORMAL_USER						1								//玩家排列索引

#define SK_NORMAL_MEMBER					2								//普通会员索引
#define SK_HIGHT_MEMBER	                    3								//高级会员索引
#define SK_SPECIAL_MEMBER					4								//特邀嘉宾索引
#define SK_FOREVER_MEMBER					5								//永久会员索引
#define SK_SENIOR_MEMBER					6								//元老玩家索引

#define SK_FRIEND_USER						7								//朋友排列索引
#define SK_MASTER_USER						8								//管理排列索引
#define SK_ME_USER							9								//自己排列索引
#define SK_ALWAYS_USER						10								//固顶排列索引

//子项类型
#define IK_USERID							1								//ID 号码
#define IK_EXPERIENCE						2								//经验值
#define IK_CREDIT							3								//信誉值
#define IK_ACCID							4								//ACC 号码
#define IK_POINT							5								//分数
#define IK_MOBILENUM						6								//手机号码
#define IK_LOGOID							7								//头像 ID 号码
#define IK_WINCOUNT							8								//胜利数目
#define IK_LOSTCOUNT						9								//输数目
#define IK_CUTCOUNT							10								//强退数目
#define IK_MIDCOUNT							11								//和局数目
#define IK_NAME								12								//登录名
#define IK_CLASSNAME						13								//游戏社团
#define IK_DESKNO							14								//游戏桌号
#define IK_DESKSTATION						15								//桌子位置
#define IK_USERSTATE						16								//用户状态
#define IK_MEMBER							17								//会员等级
#define IK_GAMEMASTER						18								//管理等级
#define IK_USERIP							19								//登录IP地址
#define IK_ALL_COUNT						20								//总局数
#define IK_CUT_PERCENT						21								//断线率
#define IK_NOTE								22								//备注类型
#define IK_MENPAI							23								//门派
#define IK_CHENGHAO							24								//身份

//关系定义
#define CM_NORMAL							0								//普通关系
#define CM_ENEMY							1								//敌人关系
#define CM_FRIEND							2								//朋友关系

//宏定义
#define USER_STATE_COUNT						6						//状态数目
#define USER_STATE_WIDTH						27						//状态宽度
#define	USER_STATE_HEIGHT						17						//状态高度

//位置顺序
#define MAX_SORT_INDEX						8								//排列索引最大值

/*namespace AFC
{
	namespace AFCListCtrl
	{*/
		/*******************************************************************************************************/

		//列表头
		class EXT_CLASS CComBodyClass/*C-AFCHeaderCtrl*/ : public CHeaderCtrl
		{
			//函数定义
		public:
			//构造函数
			CComBodyClass();
			//析构函数
			virtual ~CComBodyClass();

			DECLARE_MESSAGE_MAP()
		protected:
			int			m_Type;		//界面类型,两种
			CGameImage m_bkimg;
			CGameImage m_bkimg2;
			COLORREF	m_color;
			COLORREF	m_textcolor;
		public:
			void SetBkImage(TCHAR path[],TCHAR path2[],COLORREF color,COLORREF tcolor);
			//
			void SetBkImage(TCHAR path[],TCHAR path2[],COLORREF color,COLORREF tcolor, int type);
			//消息函数
		protected:
			//重画函数
			afx_msg void OnPaint();
			//背景函数
			afx_msg BOOL OnEraseBkgnd(CDC * pDC);
		};

		/*******************************************************************************************************/

		//列表视图基础类
		class EXT_CLASS CHappyGoDlg/*C-AFCBaseListCtrl*/ : public CListCtrl
		{
			//变量定义
		protected:
			bool							m_bAsc;								//顺序排序
			UINT							* m_pItemKind;						//子项类型
			GetOrderName					* m_pGetOrderFun;					//等级函数
			static bool						m_bInitStatic;						//初始标志
			
			static UINT						m_uSortPos[MAX_SORT_INDEX];			//排列索引
		public:
			static CImageList				m_UserStateList;					//状态列表
			//控件变量
		protected:
			CComBodyClass					m_ListHeader;						//列表头
			COLORREF						m_bkColor;
			COLORREF						m_TextColor;
			COLORREF						m_FocusbkColor;
			COLORREF						m_FocusTextColor;

			//函数定义
		public:
			//构造函数
			CHappyGoDlg(UINT * pItemKind);  
			//析构函数
			virtual ~CHappyGoDlg();
			//获取用户状态 ID
			UINT GetStateImageIndex(UserItemStruct * pUserItem);
			//设置排列索引
			bool SetSortPos(UINT uSortPos[MAX_SORT_INDEX]);
			//获取排列索引
			UINT * GetSortPos(UINT uSortPos[MAX_SORT_INDEX]);
			
			//辅助函数
		protected:
			//获取颜色
			bool GetDrawColor(COLORREF & crTextColor, COLORREF & crBackColor, UserItemStruct * pUserItem, UINT uRow, bool bSelect);
			//控件绑定
			virtual void PreSubclassWindow();
			//绘画函数
			virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
			//排列函数
			static int CALLBACK SortFun(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

			//重载函数
		public:
			//初始化
			virtual void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType);
			//增加用户
			virtual bool AddGameUser(UserItemStruct * pUserItem)=0;
			//删除用户
			virtual bool DeleteGameUser(UserItemStruct * pUserItem)=0;
			//更新状态
			virtual bool UpdateGameUser(UserItemStruct * pUserItem)=0;

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//消息函数
		public:
			//背景函数
			afx_msg BOOL OnEraseBkgnd(CDC * pDC);
			//点击列消息
			afx_msg void OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);
		public:
			void LoadSkin(void);
			//
			void SetColor(COLORREF bkcr, COLORREF textcr, COLORREF focusbkcr, COLORREF focustextcr);
		};

		/*******************************************************************************************************/

	/*};
};*/

#endif