#pragma once

#ifndef CAFCBUTTON_HEAD_FILE
#define CAFCBUTTON_HEAD_FILE

#include "AFCImage.h"

#define AFC_BTN_MOUSE_IN		(WM_USER + 1500)	//鼠标移入按钮消息
#define AFC_BTN_MOUSE_OUT		(WM_USER + 1501)    //鼠标移出按钮消息

class CTipWnd;

//using namespace AFC::AFCImageGoGoGo;

		/*******************************************************************************************************/

		//基础按钮类
		class EXT_CLASS CTrueBitmapButton/*CTrueBitmapButton*/ : public CButton
		{
			//变量定义
		public:
			bool						m_bIsMouseMove;					//鼠标标志

			bool						m_bIsPlayMusic;
			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//函数定义
		public:
			//构造函数
			CTrueBitmapButton();

			//函数定义
		protected:
			//析构函数
			virtual ~CTrueBitmapButton();
			//对象附加到现有窗口
			virtual void PreSubclassWindow();

			//全局函数
		public:
			//设置鼠标光标
			static bool LoadButtonCursor(HCURSOR hCursor);
			//设置鼠标光标
			static bool LoadButtonCursor(HINSTANCE hInstance, UINT uCursorID);
			//允许声音
			static bool EnableSound(bool bEnable=true);

			void SetPlayMusic(bool bPlay = true) {m_bIsPlayMusic = bPlay;};
			//消息函数
		protected:
			//鼠标离开消息
			LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
			//鼠标移动消息
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			//鼠标光标消息
			afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
		public:
			afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		public:
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		};

		/*******************************************************************************************************/

		//普通按钮类
		class EXT_CLASS CNormalBitmapButton/*C-AFCButton*/ : public CTrueBitmapButton
		{
			//函数定义
		public:
			//构造函数
			CNormalBitmapButton();
			//析构函数
			virtual ~CNormalBitmapButton();
			//设置颜色
			static void SetColor(COLORREF m_ActiveColor=RGB(255,120,80), COLORREF m_ActiveTextColor=RGB(0,0,255), 
				COLORREF m_NormalTextColor=RGB(0,0,0), COLORREF m_SelectTextColor=RGB(0,0,255),
				COLORREF m_FrameColor=RGB(0,64,128), COLORREF m_FrameHeight=RGB(230,230,230),
				COLORREF m_FrameShadow=RGB(128,128,128), COLORREF m_NormalColorTop=RGB(255,255,255), 
				COLORREF m_NormalColorBottom=RGB(0,128,255));

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//函数定义
		protected:
			//界面绘画函数
			virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
			//绘画界面
			void DrawFace(CDC * pDC, COLORREF crTop, COLORREF crBottom, CRect & rc, CRect & ButtonRect);
			//绘画框架
			void DrawFrame(CDC * pDC, COLORREF HeightLight, COLORREF ShadowLight, COLORREF FrameColor, CRect & rc);
		};

		/*******************************************************************************************************/

		//位图按钮类 （位图格式：常规，按下，焦点，经过，禁止）
		class EXT_CLASS CNormalBitmapButtonEx/*C-AFCPicButton*/ : public CTrueBitmapButton
		{
			//变量定义
		protected:
			bool						m_bExpand;						//是否拉伸
			bool						m_bSetColor;					//设置颜色
			HICON						m_hIcon;						//按钮图标
			COLORREF					m_crTextColor;					//字体颜色
			CGameImage					m_ButtonBitmap;					//按钮地图
	
			DWORD						m_wuiHandle;					//wui句柄
			UINT						m_uControlID;					//控件ID

            // PengJiLin, 2010-9-27, 增加字符显示功能
            bool    m_bUseTextInfo;     // 是否显示额外的字符串，针对那些没有标题字符的按钮，使用此功能显示字符
            CString m_strTextInfo;      // 需要在按钮上显示的字符串

			//函数定义
		public:
			//构造函数
			CNormalBitmapButtonEx();
			//析构函数
			virtual ~CNormalBitmapButtonEx();

			//功能函数
		public:
			//设置WUI参数
			void SetWuiParameter(DWORD wuiHandle,UINT uControlID);
			//调整位置
			bool FixButtonSize();
			//设置图标
			bool SetButtonIcon(HICON hIcon);
			//设置颜色
			bool SetTextColor(COLORREF crTextColor, bool bSetColor);
			//加载位图
			bool LoadButtonBitmap(HINSTANCE hInstance, UINT uBitmapID, bool bExpand);
			//加载位图
			bool LoadButtonBitmap(TCHAR * szFileName, bool bExpand);
			//加载位图
			static bool SetStaticBitmap(TCHAR * szFileName);
			//加载位图
			static bool SetStaticBitmap(HINSTANCE hInstance, UINT uBitmapID);

            // PengJiLin, 2010-9-27, 设置是否显示额外的字符
            void EnableToShowText(CString& strInfo, bool bUseText);
            void EnableToShowText(TCHAR* strInfo, bool bUseText);
            void EnableToShowText(int iInfo, bool bUseText);

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//函数定义
		protected:
			//界面绘画函数
			virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		};


		class EXT_CLASS CNormalBitmapButtonNew: public CNormalBitmapButtonEx
		{

		public:
			//构造函数
			CNormalBitmapButtonNew();
			//析构函数
			virtual ~CNormalBitmapButtonNew();

		protected:

			//鼠标移动消息
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);

			//鼠标离开消息
			LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

			//消息映射
		protected:

			DECLARE_MESSAGE_MAP()

			bool m_bflicker;

		public:

			void SetFlicker(bool bFlicker);

			//设置标签弹出的时间延迟
			void SetTipDelay(DWORD dwMillSecond);

			afx_msg void OnTimer(UINT nIDEvent);

		public:

			bool SetTipText(char* szTipText);

		protected:

			//标签窗口
			CTipWnd* m_pTipWnd;

			char m_szTipText[128];

			//标签弹出的时间延迟
			DWORD m_dwTipDelay;
		};

		class EXT_CLASS CNormalBitmapButtonNoState: public CNormalBitmapButtonEx
		{
		protected:
			//界面绘画函数
			virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		};
		/*******************************************************************************************************/

#endif