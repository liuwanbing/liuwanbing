#pragma once

#ifndef AFCLISTCTRL_HEAD_FILE
#define AFCLISTCTRL_HEAD_FILE

#include "AFCImage.h"
#include "ClientComStruct.h"
#include "PublicDefine.h"
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
enum{
	IK_NAME,			//用户名
	IK_USERID,			//用户ID
	IK_DESKNO,			//桌号
	IK_MONEY,			//金币
	IK_WEALTH_LEVEL,	//财富级别
	IK_POINT,			//积分
	IK_LEVEL,			//积分等级
	IK_CHARM,			//魅力
	IK_CHARM_LEVEL,		//魅力等级
	IK_SEX,				//性别
	IK_ALL_COUNT,		//总局数
	IK_WINCOUNT,		//胜局
	IK_LOSTCOUNT,		//输局
	IK_MIDCOUNT,		//和局
	IK_CUT_PERCENT,		//逃跑率
	IK_WATCH			//旁观对象
};

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
			void SetBkImage(TCHAR path[],TCHAR path2[],COLORREF color,COLORREF tcolor, int type=0);
			//消息函数
		protected:
			//重画函数
			afx_msg void OnPaint();
			//背景函数
			afx_msg BOOL OnEraseBkgnd(CDC * pDC);
		};

		/*******************************************************************************************************/

		#include <string>
		#include <map>	
		using namespace std;

		struct ColumnStruct{
			int  index;  ///< 列的位置
			int  Width;  ///< 列宽
			char ColumnName[MAX_PATH/2]; ///< 列名
			char Value[32]; ///< 列中显示的值
		};

		typedef map<string,ColumnStruct> MapColumn;

		//BzDui列表用的item数据
		//JianGuankun 2012.4.17
		struct TDuItem 
		{
			MapColumn column;
			UINT_PTR  data;			//附加数据
			UINT_PTR  item_ptr;		//item项控件(指针/句柄)
 		};

		typedef vector<TDuItem> VtDuTable;

		//列表视图基础类
		class EXT_CLASS CHappyGoDlg/*C-AFCBaseListCtrl*/ : public CListCtrl
		{
			//变量定义
		protected:
			bool							m_bAsc;								//顺序排序
			UINT							* m_pItemKind;						//子项类型
			GetOrderName					* m_pPointOrderFun;					//等级函数
			bool						m_bInitStatic;						//初始标志
			
			UINT						m_uSortPos[MAX_SORT_INDEX];			//排列索引
		public:
			CImageList					m_UserStateList;					//状态列表
			CBitmap						m_NewUserStateBmp;			///< Added by zxd 20100709
			MapColumn                   m_MapColumn;                        ///< 保存列表信息
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
			virtual bool GetDrawColor(COLORREF & crTextColor, COLORREF & crBackColor, UserItemStruct * pUserItem, UINT uRow, bool bSelect);
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
			virtual bool AddGameUser(UserItemStruct * pUserItem, bool bIsMySelf = false)=0;
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
			virtual void LoadSkin(void);
			//
			void SetColor(COLORREF bkcr, COLORREF textcr, COLORREF focusbkcr, COLORREF focustextcr);
			//设置背景色
			void SetBKColor(COLORREF bkrgb){m_bkColor = bkrgb ;}
		};

		/*******************************************************************************************************/

	/*};
};*/

#endif